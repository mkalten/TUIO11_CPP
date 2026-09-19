/*
 TUIO C++ Library
 Copyright (c) 2005-2017 Martin Kaltenbrunner <martin@tuio.org>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library.
*/

#include "TcpReceiver.h"

using namespace TUIO;
using namespace osc;

// workaround for connect method name conflict
int tcp_connect(int socket, const struct sockaddr *address, socklen_t address_len) {
	return connect(socket, address, address_len);
}

typedef struct {
	TcpReceiver *receiver;
#ifdef WIN32
	SOCKET socket;
#else
	int socket;
#endif
} TcpClientData;

#ifdef WIN32
static DWORD WINAPI ClientThreadFunc( LPVOID obj )
#else
static void* ClientThreadFunc( void* obj )
#endif
{
	TcpClientData *client_data = static_cast<TcpClientData*>(obj);
	TcpReceiver *sender = client_data->receiver;
#ifdef WIN32
	SOCKET client = client_data->socket;
#else
	int client = client_data->socket;
#endif
	delete client_data;

	char data_buffer[MAX_TCP_SIZE+4];
	int data_len = 0;

	int32_t bytes = 1;
	while (bytes>0) {
		bytes = recv(client, data_buffer+data_len, (int)sizeof(data_buffer)-data_len,0);
		if (bytes<=0) break;
		data_len += bytes;
		
		// extract all complete OSC frames from the received TCP stream
		int offset = 0;
		while ((data_len-offset)>=4) {
			
			// the frame size is a 32-bit big-endian integer
			int32_t bundle_size = (int32_t)(0xFF & data_buffer[offset]) << 24;
			bundle_size |= (0xFF & data_buffer[offset+1]) << 16;
			bundle_size |= (0xFF & data_buffer[offset+2]) << 8;
			bundle_size |= (0xFF & data_buffer[offset+3]);
			
			// abort the connection on a corrupt frame size
			if ((bundle_size<0) || (bundle_size>MAX_TCP_SIZE)) {
				offset = data_len;
				data_len = 0;
				bytes = 0;
				break;
			}
			
			// wait for more data on an incomplete frame
			if ((data_len-offset)<(4+bundle_size)) break;
			
			sender->ProcessPacket(&data_buffer[offset+4],(int)bundle_size,IpEndpointName());
			offset += 4+bundle_size;
		}
		
		// compact the remaining data to the front of the buffer
		if (offset>0) {
			memmove(data_buffer,data_buffer+offset,data_len-offset);
			data_len -= offset;
		}
	}

#ifdef WIN32
	WaitForSingleObject(sender->tcp_mutex,INFINITE);
	sender->tcp_client_list.remove(client);
	ReleaseMutex(sender->tcp_mutex);
	closesocket(client);
#else
	pthread_mutex_lock(&sender->tcp_mutex);
	sender->tcp_client_list.remove(client);
	pthread_mutex_unlock(&sender->tcp_mutex);
	close(client);
#endif
	std::cout << "closed TUIO/TCP connection" << std::endl;

	//if (sender->tcp_client_list.size()==0) sender->connected=false;
	//std::cout << sender->tcp_client_list.size() << " clients left"<< std::endl;	

	return 0;
};

#ifndef  WIN32
static void* ServerThreadFunc( void* obj )
#else
static DWORD WINAPI ServerThreadFunc( LPVOID obj )
#endif
{
	TcpReceiver *sender = static_cast<TcpReceiver*>(obj);
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);

	while ((int)sender->tcp_socket>0) {
#ifdef WIN32
		SOCKET tcp_client = -1;
#else
		int tcp_client = -1;
#endif
		len = sizeof(client_addr);
		tcp_client = accept(sender->tcp_socket, (struct sockaddr*)&client_addr, &len);

		if (tcp_client>0) { 
			std::cout << "listening to TUIO/TCP messages from " << inet_ntoa(client_addr.sin_addr) << "@" << client_addr.sin_port << std::endl;

#ifdef WIN32
			WaitForSingleObject(sender->tcp_mutex,INFINITE);
			sender->tcp_client_list.push_back(tcp_client);
			ReleaseMutex(sender->tcp_mutex);
#else
			pthread_mutex_lock(&sender->tcp_mutex);
			sender->tcp_client_list.push_back(tcp_client);
			pthread_mutex_unlock(&sender->tcp_mutex);
#endif
			//sender->connected=true;
			//std::cout << sender->tcp_client_list.size() << " clients connected"<< std::endl;	
			
			TcpClientData *client_data = new TcpClientData;
			client_data->receiver = sender;
			client_data->socket = tcp_client;
			
#ifndef WIN32
			pthread_t client_thread;
			pthread_create(&client_thread , NULL, ClientThreadFunc,client_data);
			pthread_detach(client_thread);
#else
			DWORD ClientThreadId;
			HANDLE client_thread = CreateThread( 0, 0, ClientThreadFunc, client_data, 0, &ClientThreadId );
			if (client_thread) CloseHandle(client_thread);
#endif
		} else break;

	}
	
	return 0;
};

TcpReceiver::TcpReceiver(int port)
: tcp_socket	(-1)
, locked	(false)
, local		(true)
{
#ifndef WIN32
	pthread_mutex_init(&tcp_mutex,NULL);
#else
	tcp_mutex = CreateMutex(NULL,FALSE,NULL);
#endif

	tcp_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcp_socket < 0) std::cerr << "could not create TUIO/TCP socket" << std::endl;
	
	int optval = 1;
#ifdef  WIN32
	int ret = setsockopt(tcp_socket,SOL_SOCKET,SO_REUSEADDR, (const char *)&optval,  sizeof(int));
#else
	int ret = setsockopt(tcp_socket,SOL_SOCKET,SO_REUSEADDR, (const void *)&optval,  sizeof(int));
#endif
	if (ret < 0) {
		std::cerr << "could not reuse TUIO/TCP socket address" << std::endl;
		return;
	}
	
	struct sockaddr_in tcp_server;
	memset( &tcp_server, 0, sizeof (tcp_server));
	
	tcp_server.sin_family = AF_INET;
	tcp_server.sin_addr.s_addr = htonl(INADDR_ANY);
	tcp_server.sin_port = htons(port);
	
	socklen_t len = sizeof(tcp_server);
	ret = bind(tcp_socket,(struct sockaddr*)&tcp_server,len);
	if (ret < 0) {
		std::cerr << "could not bind to TUIO/TCP socket on port " << port << std::endl;
		return;
	}
	
	ret =  listen(tcp_socket, 1);
	if (ret < 0) {
		std::cerr << "could not start listening to TUIO/TCP socket" << std::endl;
#ifdef WIN32
		closesocket(tcp_socket);
#else
		close(tcp_socket);
#endif
		tcp_socket=-1;
		return;
	}

	std::cout << "TUIO/TCP socket created on port " << port << std::endl;
}

TcpReceiver::TcpReceiver(const char *host, int port)
: tcp_socket	(-1)
, locked		(false)
, local			(false)
{
#ifndef WIN32
	pthread_mutex_init(&tcp_mutex,NULL);
#else
	tcp_mutex = CreateMutex(NULL,FALSE,NULL);
#endif

	tcp_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcp_socket < 0) {
		std::cerr << "could not create TUIO/TCP socket" << std::endl;
		return;
	}

	struct sockaddr_in tcp_server;
	memset( &tcp_server, 0, sizeof (tcp_server));
	unsigned long addr = inet_addr(host);
	if (addr != INADDR_NONE) {
		memcpy( (char *)&tcp_server.sin_addr, &addr, sizeof(addr));
	} else {
		struct hostent *host_info = gethostbyname(host);
		if (host_info == NULL) {
			std::cerr << "unknown host name: " << host << std::endl;
#ifdef WIN32
			closesocket(tcp_socket);
#else
			close(tcp_socket);
#endif
			tcp_socket=-1;
			return;
		}
		memcpy( (char *)&tcp_server.sin_addr, host_info->h_addr, host_info->h_length );
	}
	
	tcp_server.sin_family = AF_INET;
	tcp_server.sin_port = htons(port);

	int ret = tcp_connect(tcp_socket,(struct sockaddr*)&tcp_server,sizeof(tcp_server));
	if (ret<0) {
#ifdef WIN32
		closesocket(tcp_socket);
#else
		close(tcp_socket);
#endif		
		std::cerr << "could not connect to TUIO/TCP server at " << host << ":"<< port << std::endl;
		tcp_socket=-1;
		return;
	} else {
		tcp_client_list.push_back(tcp_socket);
		std::cout << "listening to TUIO/TCP messages from " << host << ":" << port << std::endl;
	}
}

TcpReceiver::~TcpReceiver() {
	disconnect();
	
	if ((int)tcp_socket>=0) {
#ifdef WIN32
		closesocket(tcp_socket);
#else
		close(tcp_socket);
#endif
		tcp_socket = -1;
	}
	
#ifdef WIN32
	if (tcp_mutex) CloseHandle(tcp_mutex);
#else
	pthread_mutex_destroy(&tcp_mutex);
#endif
}

void TcpReceiver::connect(bool lk) {
	
	if (connected) return;
	if ((int)tcp_socket<0) return;
	locked = lk;
	connected = true;
	server_thread = 0;
	if (tcp_client_list.size()>0) {
		
		TcpClientData *client_data = new TcpClientData;
		client_data->receiver = this;
		client_data->socket = tcp_socket;
		
		if (!locked) {
#ifndef WIN32
			pthread_create(&server_thread , NULL, ClientThreadFunc, client_data);
#else
			server_thread = CreateThread( 0, 0, ClientThreadFunc, client_data, 0, &ServerThreadId );
#endif
		} else {
			ClientThreadFunc(client_data);
			connected = false;
			locked = false;
		}
	} else {
		if (!locked) {
#ifndef WIN32
			pthread_create(&server_thread , NULL, ServerThreadFunc, this);
#else
			server_thread = CreateThread( 0, 0, ServerThreadFunc, this, 0, &ServerThreadId );
#endif
		} else {
			ServerThreadFunc(this);
			connected = false;
			locked = false;
		}
	}
}

void TcpReceiver::disconnect() {
	
	if (!connected) return;
	if ((int)tcp_socket<0) return;

	// shut down all sockets to unblock the receiver threads
#ifdef WIN32
	WaitForSingleObject(tcp_mutex,INFINITE);
	for (std::list<SOCKET>::iterator client = tcp_client_list.begin(); client!=tcp_client_list.end(); client++)
		shutdown((*client),SD_BOTH);
	ReleaseMutex(tcp_mutex);
	shutdown(tcp_socket,SD_BOTH);
	if (local) closesocket(tcp_socket);
#else
	pthread_mutex_lock(&tcp_mutex);
	for (std::list<int>::iterator client = tcp_client_list.begin(); client!=tcp_client_list.end(); client++)
		shutdown((*client),SHUT_RDWR);
	pthread_mutex_unlock(&tcp_mutex);
	shutdown(tcp_socket,SHUT_RDWR);
	if (local) close(tcp_socket);
#endif
	tcp_socket = -1;
	
	// the client threads remove their sockets from the client list when they terminate
	if (!locked) {
#ifndef WIN32
		if (server_thread) pthread_join(server_thread,NULL);
#else
		if (server_thread) {
			WaitForSingleObject(server_thread,INFINITE);
			CloseHandle(server_thread);
		}
#endif
		server_thread = 0;
	} else locked = false;
	
	// wait for the client threads to terminate
	bool clients_left = true;
	while (clients_left) {
#ifdef WIN32
		WaitForSingleObject(tcp_mutex,INFINITE);
		clients_left = (tcp_client_list.size()>0);
		ReleaseMutex(tcp_mutex);
		if (clients_left) Sleep(1);
#else
		pthread_mutex_lock(&tcp_mutex);
		clients_left = (tcp_client_list.size()>0);
		pthread_mutex_unlock(&tcp_mutex);
		if (clients_left) usleep(1000);
#endif
	}

	connected = false;
}



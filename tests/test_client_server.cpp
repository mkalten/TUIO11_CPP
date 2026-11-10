#include "test_helpers.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "UdpSender.h"
#include "UdpReceiver.h"
#include "TcpSender.h"
#include "TcpReceiver.h"
using namespace TUIO;

TEST(client_server_udp_loopback) {
	TuioClient client(13401);
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();
	CHECK(client.isConnected());

	TuioServer server(new UdpSender("127.0.0.1", 13401));
	server.setSourceName("utest","127.0.0.1");
	server.initFrame(TuioTime::getSessionTime());
	server.addTuioCursor(0.4f, 0.6f);
	server.addTuioObject(5, 0.3f, 0.3f, 0.5f);
	server.addTuioBlob(0.5f, 0.5f, 0.2f, 0.1f, 0.1f, 0.01f);
	server.commitFrame();

	WAIT_FOR(listener.addCur >= 1, 2000);
	WAIT_FOR(listener.addObj >= 1, 2000);
	WAIT_FOR(listener.addBlb >= 1, 2000);
	CHECK_NEAR(listener.lastX, 0.5, 1e-4);
	CHECK_EQ(listener.lastSymbolID, 5);
	WAIT_FOR(listener.refreshCount >= 1, 2000);

	// client-side getters reflect the received state
	CHECK_EQ(client.getTuioCursorCount(), 1);
	CHECK_EQ(client.getTuioObjectCount(), 1);
	CHECK_EQ(client.getTuioBlobCount(), 1);

	// update propagates over the wire
	TuioCursor *tcur = server.getTuioCursors().front();
	server.initFrame(TuioTime::getSessionTime());
	server.updateTuioCursor(tcur, 0.7f, 0.7f);
	server.commitFrame();
	WAIT_FOR(listener.updCur >= 1, 2000);

	// removal propagates
	server.initFrame(TuioTime::getSessionTime());
	server.removeTuioCursor(tcur);
	server.commitFrame();
	WAIT_FOR(listener.delCur >= 1, 2000);

	client.disconnect();
	CHECK(!client.isConnected());
}

TEST(client_server_tcp_loopback) {
	// listen-mode sender + connect-mode receiver
	TcpSender *sender = new TcpSender(13405);
	TuioServer server(sender);
	server.setSourceName("ttest","127.0.0.1");
	TuioClient client(new TcpReceiver("127.0.0.1", 13405));
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();

	// wait for the accept before sending — packets sent to an empty
	// client list are silently dropped
	WAIT_FOR(sender->isConnected(), 2000);

	server.initFrame(TuioTime::getSessionTime());
	server.addTuioCursor(0.2f, 0.8f);
	server.commitFrame();

	WAIT_FOR(listener.addCur >= 1, 3000);
	CHECK_NEAR(listener.lastX, 0.2, 1e-4);
	CHECK_NEAR(listener.lastY, 0.8, 1e-4);

	client.disconnect();
}

TEST(sender_packet_bounds) {
	char buf[128];
	// a packet stream with no data must be rejected
	osc::OutboundPacketStream empty(buf, sizeof(buf));
	CHECK_EQ((int)empty.Size(), 0);

	UdpSender udp("127.0.0.1", 13411);
	CHECK(udp.isConnected());
	CHECK(!udp.sendOscPacket(&empty)); // Size()==0 -> false

	osc::OutboundPacketStream msg(buf, sizeof(buf));
	msg << osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << osc::EndMessage;
	CHECK(udp.sendOscPacket(&msg));

	// on the loopback device packets up to MAX_LOCAL_UDP_SIZE are accepted
	std::vector<char> bigbuf(70000);
	std::vector<char> blob(4100, 'x');
	osc::OutboundPacketStream large(&bigbuf[0], bigbuf.size());
	large << osc::BeginMessage("/t") << osc::Blob(&blob[0], 4100) << osc::EndMessage;
	CHECK((int)large.Size() > 4096);
	CHECK(udp.sendOscPacket(&large));

	// a packet larger than the sender's buffer size is rejected
	std::vector<char> hugeblob(65508, 'x');
	osc::OutboundPacketStream oversized(&bigbuf[0], bigbuf.size());
	oversized << osc::BeginMessage("/t") << osc::Blob(&hugeblob[0], 65508) << osc::EndMessage;
	CHECK((int)oversized.Size() > MAX_LOCAL_UDP_SIZE);
	CHECK(!udp.sendOscPacket(&oversized));
}

TEST(udp_large_packet_loopback) {
	// a single frame of 500 cursors creates a bundle of ~46 KB,
	// which must be delivered as one large loopback datagram
	TuioClient client(13413);
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();
	CHECK(client.isConnected());

	TuioServer server(new UdpSender("127.0.0.1", 13413));
	server.setSourceName("big","127.0.0.1");
	server.initFrame(TuioTime::getSessionTime());
	for (int i=0;i<500;i++) server.addTuioCursor(i/500.0f, 0.5f);
	server.commitFrame();

	WAIT_FOR(listener.addCur >= 500, 5000);
	CHECK_EQ(client.getTuioCursorCount(), 500);

	client.disconnect();
	CHECK(!client.isConnected());
}

TEST(udp_split_bundle_reassembly) {
	// the minimal packet size of 576 bytes forces the server to split
	// a single frame of 40 cursors into a series of bundles that the
	// client must reassemble into the complete frame state
	TuioClient client(13419);
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();
	CHECK(client.isConnected());

	UdpSender *sender = new UdpSender("127.0.0.1", 13419, 576);
	CHECK_EQ(sender->getBufferSize(), 576);
	TuioServer server(sender);
	server.setSourceName("split","127.0.0.1");
	server.initFrame(TuioTime::getSessionTime());
	for (int i=0;i<40;i++) server.addTuioCursor(i/40.0f, 0.25f);
	server.commitFrame();

	// all cursors must arrive despite the frame being split
	WAIT_FOR(client.getTuioCursorCount() == 40, 5000);
	CHECK_EQ(listener.addCur, 40);

	// every cursor arrives with its session id and position intact
	std::list<TuioCursor*> serverCursors = server.getTuioCursors();
	std::list<TuioCursor*> clientCursors = client.getTuioCursors();
	CHECK_EQ((int)clientCursors.size(), 40);
	for (std::list<TuioCursor*>::iterator s = serverCursors.begin(); s != serverCursors.end(); s++) {
		bool found = false;
		for (std::list<TuioCursor*>::iterator c = clientCursors.begin(); c != clientCursors.end(); c++) {
			if ((*c)->getSessionID() == (*s)->getSessionID()) {
				CHECK_NEAR((*c)->getX(), (*s)->getX(), 1e-4);
				CHECK_NEAR((*c)->getY(), (*s)->getY(), 1e-4);
				found = true;
				break;
			}
		}
		CHECK(found);
	}

	// a subsequent update frame must still propagate over the split transport
	TuioCursor *tcur = serverCursors.front();
	server.initFrame(TuioTime::getSessionTime());
	server.updateTuioCursor(tcur, 0.9f, 0.9f);
	server.commitFrame();
	WAIT_FOR(listener.updCur >= 1, 5000);

	client.disconnect();
	CHECK(!client.isConnected());
}

/*
 TUIO C++ Library
 Copyright (c) 2005-2017 Martin Kaltenbrunner <martin@tuio.org>
 Modified by Bremard Nicolas <nicolas@bremard.fr> on 11/2022

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

#ifndef INCLUDED_TUIOCLIENT_H
#define INCLUDED_TUIOCLIENT_H

#include "TuioDispatcher.h"
#include "OscReceiver.h"
#include "osc/OscReceivedElements.h"

#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <cstring>

namespace TUIO {
	
	class OscReceiver; // Forward declaration
	
	/**
	 * <p>The TuioClient class is the central TUIO protocol decoder component. It provides a simple callback infrastructure using the {@link TuioListener} interface.
	 * In order to receive and decode TUIO messages an instance of TuioClient needs to be created. The TuioClient instance then generates TUIO events
	 * which are broadcasted to all registered classes that implement the {@link TuioListener} interface.</p> 
	 * <p><code>
	 * TuioClient *client = new TuioClient();<br/>
	 * client->addTuioListener(myTuioListener);<br/>
	 * client->connect();<br/>
	 * </code></p>
	 *
	 * @author Martin Kaltenbrunner
	 * @version 1.1.7
	 */ 
	class LIBDECL TuioClient : public TuioDispatcher { 
		
	public:
		/**
		 * This constructor creates a TuioClient that uses an internal UdpReceiver listening to the default UDP port 3333
		 *
		 */
		TuioClient();
	
		/**
		 * This constructor creates a TuioClient that uses an internal UdpReceiver listening to the provided UDP port
		 *
		 * @param  port  the UDP port the internal UdpReceiver is listening to
		 */
		TuioClient(int port);
		
		/**
		 * This constructor creates a TuioClient that uses the provided OscReceiver for the incoming OSC data
		 *
		 * @param  oscreceiver  the OscReceiver implementation for the chosen transport method (UDP, TCP ...)
		 */
		TuioClient(OscReceiver *oscreceiver);

		/**
		 * The destructor is doing nothing in particular. 
		 */
		~TuioClient();

		/**
		 * The TuioClient connects and starts receiving TUIO messages from its associated OscReceiver
		 *
		 * @param  lock  running in the background if set to false (default)
		 */
		void connect(bool lock=false);
		
		/**
		 * The TuioClient disconnects and stops receiving TUIO messages from its associated OscReceiver
		 */
		void disconnect();
		
		/**
		 * Returns true if this TuioClient is currently connected.
		 * @return	true if this TuioClient is currently connected
		 */
		bool isConnected();






		/**
		 * Returns a List of all currently active TuioObjects
		 *
		 * @return  a List of TuioObjects
		 */
		std::list<TuioObject*> getTuioObjects() {
			return TuioDispatcher::getTuioObjects();
		}
		
		/**
		 * Returns a List of all currently active TuioObjects
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioObjects
		 */
		std::list<TuioObject*> getTuioObjects(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioObjects
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioObjects
		 */
		std::list<TuioObject> copyTuioObjects(int source_id);

		
		/**
		 * Returns a List with a copy of all currently active TuioObjects
		 *
		 * @return  a List with a copy of TuioObjects
		 */
		std::list<TuioObject> copyTuioObjects() {
			return TuioDispatcher::copyTuioObjects();
		}
		
		/**
		 * Returns the TuioObject corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioObject
		 *
		 * @param  s_id  the session ID of the corresponding TuioObject
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		TuioObject* getTuioObject(long s_id) {
			return getTuioObject(0,s_id);
		};
		
		/**
		 * Returns the TuioObject corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioObject
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioObject
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		TuioObject* getTuioObject(int src_id, long s_id);

		/**
		 * Returns a List of all currently active TuioCursors
		 *
		 * @return  a List of TuioCursors
		 */
		std::list<TuioCursor*> getTuioCursors() {
			return TuioDispatcher::getTuioCursors();
		}
		
		/**
		 * Returns a List of all currently active TuioCursors
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioCursors
		 */
		std::list<TuioCursor*> getTuioCursors(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioCursors
		 *
		 * @return  a List with a copy of TuioCursors
		 */
		std::list<TuioCursor> copyTuioCursors() {
			return TuioDispatcher::copyTuioCursors();
		}
		
		/**
		 * Returns a List with a copy of all currently active TuioCursors
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioCursors
		 */
		std::list<TuioCursor> copyTuioCursors(int source_id);
		
		/**
		 * Returns the TuioCursor corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioCursor
		 *
		 * @param  s_id  the session ID of the corresponding TuioCursor
		 * @return  an active TuioCursor corresponding to the provided Session ID or NULL
		 */
		TuioCursor* getTuioCursor(long s_id) {
			return getTuioCursor(0,s_id);
		};
		
		/**
		 * Returns the TuioCursor corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioCursor
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioCursor
		 * @return  an active TuioCursor corresponding to the provided Session ID or NULL
		 */
		TuioCursor* getTuioCursor(int src_id, long s_id);

		/**
		 * Returns a List of all currently active TuioBlobs
		 *
		 * @return  a List of TuioBlobs
		 */
		std::list<TuioBlob*> getTuioBlobs() {
			return TuioDispatcher::getTuioBlobs();
		}
		
		/**
		 * Returns a List of all currently active TuioBlobs
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioBlobs
		 */
		std::list<TuioBlob*> getTuioBlobs(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioBlobs
		 *
		 * @return  a List with a copy of TuioBlobs
		 */
		std::list<TuioBlob> copyTuioBlobs() {
			return TuioDispatcher::copyTuioBlobs();
		}
		
		/**
		 * Returns a List with a copy of all currently active TuioBlobs
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioBlobs
		 */
		std::list<TuioBlob> copyTuioBlobs(int source_id);
		
		/**
		 * Returns the TuioBlob corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBlob
		 *
		 * @param  s_id  the session ID of the corresponding TuioBlob
		 * @return  an active TuioBlob corresponding to the provided Session ID or NULL
		 */
		TuioBlob* getTuioBlob(long s_id) {
			return getTuioBlob(0,s_id);
		};
		
		/**
		 * Returns the TuioBlob corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioBlob
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioBlob
		 * @return  an active TuioBlob corresponding to the provided Session ID or NULL
		 */
		TuioBlob* getTuioBlob(int src_id, long s_id);









		/**
		 * Returns a List of all currently active TuioObjects25D
		 *
		 * @return  a List of TuioObjects25D
		 */
		std::list<TuioObject25D*> getTuioObjects25D() {
			return TuioDispatcher::getTuioObjects25D();
		}

		/**
		 * Returns a List of all currently active TuioObjects25D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioObjects25D
		 */
		std::list<TuioObject25D*> getTuioObjects25D(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioObjects25D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioObjects
		 */
		std::list<TuioObject25D> copyTuioObjects25D(int source_id);


		/**
		 * Returns a List with a copy of all currently active TuioObjects25D
		 *
		 * @return  a List with a copy of TuioObjects25D
		 */
		std::list<TuioObject25D> copyTuioObjects25D() {
			return TuioDispatcher::copyTuioObjects25D();
		}

		/**
		 * Returns the TuioObject25D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioObject25D
		 *
		 * @param  s_id  the session ID of the corresponding TuioObject25D
		 * @return  an active TuioObject25D corresponding to the provided Session ID or NULL
		 */
		TuioObject25D* getTuioObject25D(long s_id) {
			return getTuioObject25D(0, s_id);
		};

		/**
		 * Returns the TuioObject25D corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioObject25D
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioObject25D
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		TuioObject25D* getTuioObject25D(int src_id, long s_id);

		/**
		 * Returns a List of all currently active TuioCursors25D
		 *
		 * @return  a List of TuioCursors25D
		 */
		std::list<TuioCursor25D*> getTuioCursors25D() {
			return TuioDispatcher::getTuioCursors25D();
		}

		/**
		 * Returns a List of all currently active TuioCursors25D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioCursors25D
		 */
		std::list<TuioCursor25D*> getTuioCursors25D(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioCursors25D
		 *
		 * @return  a List with a copy of TuioCursors25D
		 */
		std::list<TuioCursor25D> copyTuioCursors25D() {
			return TuioDispatcher::copyTuioCursors25D();
		}

		/**
		 * Returns a List with a copy of all currently active TuioCursors25D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioCursors25D
		 */
		std::list<TuioCursor25D> copyTuioCursors25D(int source_id);

		/**
		 * Returns the TuioCursor25D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioCursor25D
		 *
		 * @param  s_id  the session ID of the corresponding TuioCursor25D
		 * @return  an active TuioCursor25D corresponding to the provided Session ID or NULL
		 */
		TuioCursor25D* getTuioCursor25D(long s_id) {
			return getTuioCursor25D(0, s_id);
		};

		/**
		 * Returns the TuioCursor25D corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioCursor25D
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioCursor25D
		 * @return  an active TuioCursor25D corresponding to the provided Session ID or NULL
		 */
		TuioCursor25D* getTuioCursor25D(int src_id, long s_id);

		/**
		 * Returns a List of all currently active TuioBlobs25D
		 *
		 * @return  a List of TuioBlobs25D
		 */
		std::list<TuioBlob25D*> getTuioBlobs25D() {
			return TuioDispatcher::getTuioBlobs25D();
		}

		/**
		 * Returns a List of all currently active TuioBlobs25D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioBlobs25D
		 */
		std::list<TuioBlob25D*> getTuioBlobs25D(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioBlobs25D
		 *
		 * @return  a List with a copy of TuioBlobs25D
		 */
		std::list<TuioBlob25D> copyTuioBlobs25D() {
			return TuioDispatcher::copyTuioBlobs25D();
		}

		/**
		 * Returns a List with a copy of all currently active TuioBlobs25D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioBlobs25D
		 */
		std::list<TuioBlob25D> copyTuioBlobs25D(int source_id);

		/**
		 * Returns the TuioBlob25D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBlob25D
		 *
		 * @param  s_id  the session ID of the corresponding TuioBlob25D
		 * @return  an active TuioBlob25D corresponding to the provided Session ID or NULL
		 */
		TuioBlob25D* getTuioBlob25D(long s_id) {
			return getTuioBlob25D(0, s_id);
		};

		/**
		 * Returns the TuioBlob25D corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioBlob25D
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioBlob25D
		 * @return  an active TuioBlob25D corresponding to the provided Session ID or NULL
		 */
		TuioBlob25D* getTuioBlob25D(int src_id, long s_id);










		/**
 * Returns a List of all currently active TuioObjects3D
 *
 * @return  a List of TuioObjects3D
 */
		std::list<TuioObject3D*> getTuioObjects3D() {
			return TuioDispatcher::getTuioObjects3D();
		}

		/**
		 * Returns a List of all currently active TuioObjects3D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioObjects3D
		 */
		std::list<TuioObject3D*> getTuioObjects3D(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioObjects3D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioObjects
		 */
		std::list<TuioObject3D> copyTuioObjects3D(int source_id);


		/**
		 * Returns a List with a copy of all currently active TuioObjects3D
		 *
		 * @return  a List with a copy of TuioObjects3D
		 */
		std::list<TuioObject3D> copyTuioObjects3D() {
			return TuioDispatcher::copyTuioObjects3D();
		}

		/**
		 * Returns the TuioObject3D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioObject3D
		 *
		 * @param  s_id  the session ID of the corresponding TuioObject3D
		 * @return  an active TuioObject3D corresponding to the provided Session ID or NULL
		 */
		TuioObject3D* getTuioObject3D(long s_id) {
			return getTuioObject3D(0, s_id);
		};

		/**
		 * Returns the TuioObject3D corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioObject3D
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioObject3D
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		TuioObject3D* getTuioObject3D(int src_id, long s_id);

		/**
		 * Returns a List of all currently active TuioCursors3D
		 *
		 * @return  a List of TuioCursors3D
		 */
		std::list<TuioCursor3D*> getTuioCursors3D() {
			return TuioDispatcher::getTuioCursors3D();
		}

		/**
		 * Returns a List of all currently active TuioCursors3D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioCursors3D
		 */
		std::list<TuioCursor3D*> getTuioCursors3D(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioCursors3D
		 *
		 * @return  a List with a copy of TuioCursors3D
		 */
		std::list<TuioCursor3D> copyTuioCursors3D() {
			return TuioDispatcher::copyTuioCursors3D();
		}

		/**
		 * Returns a List with a copy of all currently active TuioCursors3D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioCursors3D
		 */
		std::list<TuioCursor3D> copyTuioCursors3D(int source_id);

		/**
		 * Returns the TuioCursor3D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioCursor3D
		 *
		 * @param  s_id  the session ID of the corresponding TuioCursor3D
		 * @return  an active TuioCursor3D corresponding to the provided Session ID or NULL
		 */
		TuioCursor3D* getTuioCursor3D(long s_id) {
			return getTuioCursor3D(0, s_id);
		};

		/**
		 * Returns the TuioCursor3D corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioCursor3D
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioCursor3D
		 * @return  an active TuioCursor3D corresponding to the provided Session ID or NULL
		 */
		TuioCursor3D* getTuioCursor3D(int src_id, long s_id);

		/**
		 * Returns a List of all currently active TuioBlobs3D
		 *
		 * @return  a List of TuioBlobs3D
		 */
		std::list<TuioBlob3D*> getTuioBlobs3D() {
			return TuioDispatcher::getTuioBlobs3D();
		}

		/**
		 * Returns a List of all currently active TuioBlobs3D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List of TuioBlobs3D
		 */
		std::list<TuioBlob3D*> getTuioBlobs3D(int source_id);

		/**
		 * Returns a List with a copy of all currently active TuioBlobs3D
		 *
		 * @return  a List with a copy of TuioBlobs3D
		 */
		std::list<TuioBlob3D> copyTuioBlobs3D() {
			return TuioDispatcher::copyTuioBlobs3D();
		}

		/**
		 * Returns a List with a copy of all currently active TuioBlobs3D
		 * which are associated to the given Source ID
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @return  a List with a copy of TuioBlobs3D
		 */
		std::list<TuioBlob3D> copyTuioBlobs3D(int source_id);

		/**
		 * Returns the TuioBlob3D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBlob3D
		 *
		 * @param  s_id  the session ID of the corresponding TuioBlob3D
		 * @return  an active TuioBlob3D corresponding to the provided Session ID or NULL
		 */
		TuioBlob3D* getTuioBlob3D(long s_id) {
			return getTuioBlob3D(0, s_id);
		};

		/**
		 * Returns the TuioBlob3D corresponding to the provided Session ID
		 * which is associated to the given Source ID
		 * or NULL if the Session ID does not refer to an active TuioBlob3D
		 *
		 * @param  src_id  the source ID of the corresponding TUIO source
		 * @param  s_id  the session ID of the corresponding TuioBlob3D
		 * @return  an active TuioBlob3D corresponding to the provided Session ID or NULL
		 */
		TuioBlob3D* getTuioBlob3D(int src_id, long s_id);











		
		void processOSC( const osc::ReceivedMessage& message);
		
	private:
		void initialize();
		
		std::list<TuioObject*> frameObjects;
		std::list<long> aliveObjectList;
		std::list<TuioCursor*> frameCursors;
		std::list<long> aliveCursorList;
		std::list<TuioBlob*> frameBlobs;
		std::list<long> aliveBlobList;


		std::list<TuioObject25D*> frameObjects25D;
		std::list<long> aliveObject25DList;
		std::list<TuioCursor25D*> frameCursors25D;
		std::list<long> aliveCursor25DList;
		std::list<TuioBlob25D*> frameBlobs25D;
		std::list<long> aliveBlob25DList;



		std::list<TuioObject3D*> frameObjects3D;
		std::list<long> aliveObject3DList;
		std::list<TuioCursor3D*> frameCursors3D;
		std::list<long> aliveCursor3DList;
		std::list<TuioBlob3D*> frameBlobs3D;
		std::list<long> aliveBlob3DList;


		
		osc::int32 currentFrame;
		TuioTime currentTime;
			
		std::list<TuioCursor*> freeCursorList, freeCursorBuffer;
		std::map<int,int> maxCursorID;

		std::list<TuioBlob*> freeBlobList, freeBlobBuffer;
		std::map<int,int> maxBlobID;


		std::list<TuioCursor25D*> freeCursor25DList, freeCursor25DBuffer;
		std::map<int, int> maxCursor25DID;

		std::list<TuioBlob25D*> freeBlob25DList, freeBlob25DBuffer;
		std::map<int, int> maxBlob25DID;


		std::list<TuioCursor3D*> freeCursor3DList, freeCursor3DBuffer;
		std::map<int, int> maxCursor3DID;

		std::list<TuioBlob3D*> freeBlob3DList, freeBlob3DBuffer;
		std::map<int, int> maxBlob3DID;

		
		std::map<std::string,int> sourceList;
		int source_id;
		char *source_name;
		char *source_addr;
		
		OscReceiver *receiver;
		bool local_receiver;
	};
};
#endif /* INCLUDED_TUIOCLIENT_H */

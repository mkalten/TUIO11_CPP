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

#include "TuioServer.h"
#include "UdpSender.h"

using namespace TUIO;
using namespace osc;

TuioServer::TuioServer() 
	:full_update			(false)
	,periodic_update		(false)
	, objectProfileEnabled(true)
	, cursorProfileEnabled(true)
	, blobProfileEnabled(true)
	, object25DProfileEnabled(true)
	, cursor25DProfileEnabled(true)
	, blob25DProfileEnabled(true)
	, object3DProfileEnabled(true)
	, cursor3DProfileEnabled(true)
	, blob3DProfileEnabled(true)
	,source_name			(NULL)
{
	OscSender *oscsend = new UdpSender();
	initialize(oscsend);
}

TuioServer::TuioServer(const char *host, int port) 
:full_update			(false)
,periodic_update		(false)	
,objectProfileEnabled	(true)
,cursorProfileEnabled	(true)
,blobProfileEnabled		(true)
, object25DProfileEnabled(true)
, cursor25DProfileEnabled(true)
, blob25DProfileEnabled(true)
, object3DProfileEnabled(true)
, cursor3DProfileEnabled(true)
, blob3DProfileEnabled(true)
,source_name			(NULL)
{
	OscSender *oscsend = new UdpSender(host,port);
	initialize(oscsend);
}

TuioServer::TuioServer(OscSender *oscsend)
	:full_update			(false)
	,periodic_update		(false)	
	,objectProfileEnabled	(true)
	,cursorProfileEnabled	(true)
	,blobProfileEnabled		(true)
	, object25DProfileEnabled(true)
	, cursor25DProfileEnabled(true)
	, blob25DProfileEnabled(true)
	, object3DProfileEnabled(true)
	, cursor3DProfileEnabled(true)
	, blob3DProfileEnabled(true)
	,source_name			(NULL)
{
	initialize(oscsend);
}

void TuioServer::initialize(OscSender *oscsend) {
	
	senderList.push_back(oscsend);
	int size = oscsend->getBufferSize();
	oscBuffer = new char[size];
	oscPacket = new osc::OutboundPacketStream(oscBuffer,size);
	fullBuffer = new char[size];
	fullPacket = new osc::OutboundPacketStream(oscBuffer,size);
	
	objectUpdateTime = TuioTime(currentFrameTime);
	cursorUpdateTime = TuioTime(currentFrameTime);
	blobUpdateTime = TuioTime(currentFrameTime);

	object25DUpdateTime = TuioTime(currentFrameTime);
	cursor25DUpdateTime = TuioTime(currentFrameTime);
	blob25DUpdateTime = TuioTime(currentFrameTime);

	object3DUpdateTime = TuioTime(currentFrameTime);
	cursor3DUpdateTime = TuioTime(currentFrameTime);
	blob3DUpdateTime = TuioTime(currentFrameTime);
	
	if (cursorProfileEnabled) sendEmptyCursorBundle();
	if (objectProfileEnabled) sendEmptyObjectBundle();
	if (blobProfileEnabled) sendEmptyBlobBundle();

	if (cursor25DProfileEnabled) sendEmptyCursor25DBundle();
	if (object25DProfileEnabled) sendEmptyObject25DBundle();
	if (blob25DProfileEnabled) sendEmptyBlob25DBundle();

	if (cursor3DProfileEnabled) sendEmptyCursor3DBundle();
	if (object3DProfileEnabled) sendEmptyObject3DBundle();
	if (blob3DProfileEnabled) sendEmptyBlob3DBundle();

	invert_x = false;
	invert_y = false;
	invert_a = false;	
}

TuioServer::~TuioServer() {

	initFrame(TuioTime::getSessionTime());
	stopUntouchedMovingCursors();
	stopUntouchedMovingObjects();
	stopUntouchedMovingBlobs();
	stopUntouchedMovingCursors25D();
	stopUntouchedMovingObjects25D();
	stopUntouchedMovingBlobs25D();
	stopUntouchedMovingCursors3D();
	stopUntouchedMovingObjects3D();
	stopUntouchedMovingBlobs3D();
	
	initFrame(TuioTime::getSessionTime());
	removeUntouchedStoppedCursors();
	removeUntouchedStoppedObjects();
	removeUntouchedStoppedBlobs();
	removeUntouchedStoppedCursors25D();
	removeUntouchedStoppedObjects25D();
	removeUntouchedStoppedBlobs25D();
	removeUntouchedStoppedCursors3D();
	removeUntouchedStoppedObjects3D();
	removeUntouchedStoppedBlobs3D();
	
	if (cursorProfileEnabled) sendEmptyCursorBundle();
	if (objectProfileEnabled) sendEmptyObjectBundle();
	if (blobProfileEnabled) sendEmptyBlobBundle();
	if (cursor25DProfileEnabled) sendEmptyCursor25DBundle();
	if (object25DProfileEnabled) sendEmptyObject25DBundle();
	if (blob25DProfileEnabled) sendEmptyBlob25DBundle();
	if (cursor3DProfileEnabled) sendEmptyCursor3DBundle();
	if (object3DProfileEnabled) sendEmptyObject3DBundle();
	if (blob3DProfileEnabled) sendEmptyBlob3DBundle();
	
	delete []oscBuffer;
	delete oscPacket;
	delete []fullBuffer;
	delete fullPacket;
	
	if (source_name) delete[] source_name;
	for (unsigned int i=0;i<senderList.size();i++)
		delete senderList[i];
}


void TuioServer::addOscSender(OscSender *sender) {

	// add source address if previously local
	/*if ((source_name) && (primary_sender->isLocal()) && (senderList.size()==1)) {
		setSourceName(source_name);
	}*/ 
	
	// resize packets to smallest transport method
	unsigned int size = sender->getBufferSize();
	if (size<oscPacket->Capacity()) {
		osc::OutboundPacketStream *temp = oscPacket;
		oscPacket = new osc::OutboundPacketStream(oscBuffer,size);
		delete temp;
		temp = fullPacket;
		fullPacket = new osc::OutboundPacketStream(oscBuffer,size);
		delete temp;
		
	}
	
	senderList.push_back(sender);
}

void TuioServer::deliverOscPacket(osc::OutboundPacketStream  *packet) {

	for (unsigned int i=0;i<senderList.size();i++)
		senderList[i]->sendOscPacket(packet);
}

void TuioServer::setSourceName(const char *name, const char *ip) {
	if (!source_name) source_name = new char[256];
	sprintf(source_name,"%s@%s",name,ip);
}


void TuioServer::setSourceName(const char *src) {
	
	if (!source_name) source_name = new char[256];

	/*if (senderList[0]->isLocal()) {
		sprintf(source_name,"%s",src);
	} else {*/
		char hostname[64];
		char *source_addr = NULL;
		struct hostent *hp = NULL;
		struct in_addr *addr = NULL;
		
		gethostname(hostname, 64);
		hp = gethostbyname(hostname);
		
		if (hp==NULL) {
			sprintf(hostname, "%s.local", hostname);
			hp = gethostbyname(hostname);
		}
		
		if (hp!=NULL) {
			for (int i = 0; hp->h_addr_list[i] != 0; ++i) {
				addr = (struct in_addr *)(hp->h_addr_list[i]);
				//std::cout << inet_ntoa(*addr) << std::endl;
				source_addr = inet_ntoa(*addr);
			}
		} else {
			//generate a random internet address
			srand ( (unsigned int)time(NULL) );
			int32 r = rand();
			addr = (struct in_addr*)&r;
			source_addr = inet_ntoa(*addr);
		}
		sprintf(source_name,"%s@%s",src,source_addr);
	//}
	
	std::cout << "tuio/src " << source_name << std::endl;
}

void TuioServer::commitFrame() {
	TuioManager::commitFrame();
		
	if(updateObject) {
		startObjectBundle();
		for (std::list<TuioObject*>::iterator  tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++) {
			
			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity()-oscPacket->Size())<OBJ_MESSAGE_SIZE) {
				sendObjectBundle(currentFrame);
				startObjectBundle();
			}
			TuioObject *tobj = (*tuioObject);
			if  ((full_update) || (tobj->getTuioTime()==currentFrameTime)) addObjectMessage(tobj);
		}
		objectUpdateTime = TuioTime(currentFrameTime);
		sendObjectBundle(currentFrame);
	} else if (objectProfileEnabled && periodic_update) {
		
		TuioTime timeCheck = currentFrameTime - objectUpdateTime;
		if(timeCheck.getSeconds()>=update_interval) {
			objectUpdateTime = TuioTime(currentFrameTime);
			startObjectBundle();
			if  (full_update) {
				for (std::list<TuioObject*>::iterator  tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++) {
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity()-oscPacket->Size())<OBJ_MESSAGE_SIZE) {
						sendObjectBundle(currentFrame);
						startObjectBundle();
					}
					addObjectMessage(*tuioObject);
				}
			}
			sendObjectBundle(currentFrame);
		}
	}
	updateObject = false;

	if(updateCursor) {
		startCursorBundle();
		for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++) {
			
			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity()-oscPacket->Size())<CUR_MESSAGE_SIZE) {
				sendCursorBundle(currentFrame);
				startCursorBundle();
			}
			TuioCursor *tcur = (*tuioCursor);
			if ((full_update) || (tcur->getTuioTime()==currentFrameTime)) addCursorMessage(tcur);				
		}
		cursorUpdateTime = TuioTime(currentFrameTime);
		sendCursorBundle(currentFrame);
	} else if (cursorProfileEnabled && periodic_update) {
		TuioTime timeCheck = currentFrameTime - cursorUpdateTime;
		if(timeCheck.getSeconds()>=update_interval) {
			cursorUpdateTime = TuioTime(currentFrameTime);
			startCursorBundle();
			if (full_update) {
				for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++) {
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity()-oscPacket->Size())<CUR_MESSAGE_SIZE) {
						sendCursorBundle(currentFrame);
						startCursorBundle();
					}
					addCursorMessage(*tuioCursor);
				}
			}
			sendCursorBundle(currentFrame);
		}
	}
	updateCursor = false;
	
	if(updateBlob) {
		startBlobBundle();
		for (std::list<TuioBlob*>::iterator tuioBlob =blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++) {
			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity()-oscPacket->Size())<BLB_MESSAGE_SIZE) {
				sendBlobBundle(currentFrame);
				startBlobBundle();
			}
			TuioBlob *tblb = (*tuioBlob);
			if ((full_update) || (tblb->getTuioTime()==currentFrameTime)) addBlobMessage(tblb);		
		}
		blobUpdateTime = TuioTime(currentFrameTime);
		sendBlobBundle(currentFrame);
	} else if (blobProfileEnabled && periodic_update) {
		TuioTime timeCheck = currentFrameTime - blobUpdateTime;
		if(timeCheck.getSeconds()>=update_interval) {
			blobUpdateTime = TuioTime(currentFrameTime);
			startBlobBundle();
			if (full_update) {
				for (std::list<TuioBlob*>::iterator tuioBlob =blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++) {
				
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity()-oscPacket->Size())<BLB_MESSAGE_SIZE) {
						sendBlobBundle(currentFrame);
						startBlobBundle();
					}
					addBlobMessage(*tuioBlob);
				}
			}
			sendBlobBundle(currentFrame);
		}
	}
	updateBlob = false;


	// 25D

	if (updateObject25D) {
		startObject25DBundle();
		for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++) {

			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity() - oscPacket->Size()) < OBJ25D_MESSAGE_SIZE) {
				sendObject25DBundle(currentFrame);
				startObject25DBundle();
			}
			TuioObject25D *tobj = (*tuioObject);
			if ((full_update) || (tobj->getTuioTime() == currentFrameTime)) addObject25DMessage(tobj);
		}
		object25DUpdateTime = TuioTime(currentFrameTime);
		sendObject25DBundle(currentFrame);
	}
	else if (object25DProfileEnabled && periodic_update) {

		TuioTime timeCheck = currentFrameTime - object25DUpdateTime;
		if (timeCheck.getSeconds() >= update_interval) {
			object25DUpdateTime = TuioTime(currentFrameTime);
			startObject25DBundle();
			if (full_update) {
				for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++) {
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity() - oscPacket->Size()) < OBJ25D_MESSAGE_SIZE) {
						sendObject25DBundle(currentFrame);
						startObject25DBundle();
					}
					addObject25DMessage(*tuioObject);
				}
			}
			sendObject25DBundle(currentFrame);
		}
	}
	updateObject25D = false;

	if (updateCursor25D) {
		startCursor25DBundle();
		for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++) {

			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity() - oscPacket->Size()) < CUR25D_MESSAGE_SIZE) {
				sendCursor25DBundle(currentFrame);
				startCursor25DBundle();
			}
			TuioCursor25D *tcur = (*tuioCursor);
			if ((full_update) || (tcur->getTuioTime() == currentFrameTime)) addCursor25DMessage(tcur);
		}
		cursor25DUpdateTime = TuioTime(currentFrameTime);
		sendCursor25DBundle(currentFrame);
	}
	else if (cursor25DProfileEnabled && periodic_update) {
		TuioTime timeCheck = currentFrameTime - cursor25DUpdateTime;
		if (timeCheck.getSeconds() >= update_interval) {
			cursor25DUpdateTime = TuioTime(currentFrameTime);
			startCursor25DBundle();
			if (full_update) {
				for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++) {
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity() - oscPacket->Size()) < CUR25D_MESSAGE_SIZE) {
						sendCursor25DBundle(currentFrame);
						startCursor25DBundle();
					}
					addCursor25DMessage(*tuioCursor);
				}
			}
			sendCursor25DBundle(currentFrame);
		}
	}
	updateCursor25D = false;

	if (updateBlob25D) {
		startBlob25DBundle();
		for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++) {
			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity() - oscPacket->Size()) < BLB25D_MESSAGE_SIZE) {
				sendBlob25DBundle(currentFrame);
				startBlob25DBundle();
			}
			TuioBlob25D *tblb = (*tuioBlob);
			if ((full_update) || (tblb->getTuioTime() == currentFrameTime)) addBlob25DMessage(tblb);
		}
		blob25DUpdateTime = TuioTime(currentFrameTime);
		sendBlob25DBundle(currentFrame);
	}
	else if (blob25DProfileEnabled && periodic_update) {
		TuioTime timeCheck = currentFrameTime - blob25DUpdateTime;
		if (timeCheck.getSeconds() >= update_interval) {
			blob25DUpdateTime = TuioTime(currentFrameTime);
			startBlob25DBundle();
			if (full_update) {
				for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++) {

					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity() - oscPacket->Size()) < BLB25D_MESSAGE_SIZE) {
						sendBlob25DBundle(currentFrame);
						startBlob25DBundle();
					}
					addBlob25DMessage(*tuioBlob);
				}
			}
			sendBlob25DBundle(currentFrame);
		}
	}
	updateBlob25D = false;


	// 3D

	if (updateObject3D) {
		startObject3DBundle();
		for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++) {

			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity() - oscPacket->Size()) < OBJ3D_MESSAGE_SIZE) {
				sendObject3DBundle(currentFrame);
				startObject3DBundle();
			}
			TuioObject3D *tobj = (*tuioObject);
			if ((full_update) || (tobj->getTuioTime() == currentFrameTime)) addObject3DMessage(tobj);
		}
		object3DUpdateTime = TuioTime(currentFrameTime);
		sendObject3DBundle(currentFrame);
	}
	else if (object3DProfileEnabled && periodic_update) {

		TuioTime timeCheck = currentFrameTime - object3DUpdateTime;
		if (timeCheck.getSeconds() >= update_interval) {
			object3DUpdateTime = TuioTime(currentFrameTime);
			startObject3DBundle();
			if (full_update) {
				for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++) {
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity() - oscPacket->Size()) < OBJ3D_MESSAGE_SIZE) {
						sendObject3DBundle(currentFrame);
						startObject3DBundle();
					}
					addObject3DMessage(*tuioObject);
				}
			}
			sendObject3DBundle(currentFrame);
		}
	}
	updateObject3D = false;

	if (updateCursor3D) {
		startCursor3DBundle();
		for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++) {

			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity() - oscPacket->Size()) < CUR3D_MESSAGE_SIZE) {
				sendCursor3DBundle(currentFrame);
				startCursor3DBundle();
			}
			TuioCursor3D *tcur = (*tuioCursor);
			if ((full_update) || (tcur->getTuioTime() == currentFrameTime)) addCursor3DMessage(tcur);
		}
		cursor3DUpdateTime = TuioTime(currentFrameTime);
		sendCursor3DBundle(currentFrame);
	}
	else if (cursor3DProfileEnabled && periodic_update) {
		TuioTime timeCheck = currentFrameTime - cursor3DUpdateTime;
		if (timeCheck.getSeconds() >= update_interval) {
			cursor3DUpdateTime = TuioTime(currentFrameTime);
			startCursor3DBundle();
			if (full_update) {
				for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++) {
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity() - oscPacket->Size()) < CUR3D_MESSAGE_SIZE) {
						sendCursor3DBundle(currentFrame);
						startCursor3DBundle();
					}
					addCursor3DMessage(*tuioCursor);
				}
			}
			sendCursor3DBundle(currentFrame);
		}
	}
	updateCursor3D = false;

	if (updateBlob3D) {
		startBlob3DBundle();
		for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++) {
			// start a new packet if we exceed the packet capacity
			if ((oscPacket->Capacity() - oscPacket->Size()) < BLB3D_MESSAGE_SIZE) {
				sendBlob3DBundle(currentFrame);
				startBlob3DBundle();
			}
			TuioBlob3D *tblb = (*tuioBlob);
			if ((full_update) || (tblb->getTuioTime() == currentFrameTime)) addBlob3DMessage(tblb);
		}
		blob3DUpdateTime = TuioTime(currentFrameTime);
		sendBlob3DBundle(currentFrame);
	}
	else if (blob3DProfileEnabled && periodic_update) {
		TuioTime timeCheck = currentFrameTime - blob3DUpdateTime;
		if (timeCheck.getSeconds() >= update_interval) {
			blob3DUpdateTime = TuioTime(currentFrameTime);
			startBlob3DBundle();
			if (full_update) {
				for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++) {

					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity() - oscPacket->Size()) < BLB3D_MESSAGE_SIZE) {
						sendBlob3DBundle(currentFrame);
						startBlob3DBundle();
					}
					addBlob3DMessage(*tuioBlob);
				}
			}
			sendBlob3DBundle(currentFrame);
		}
	}
	updateBlob3D = false;


}





void TuioServer::sendEmptyCursorBundle() {
	oscPacket->Clear();	
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage( "/tuio/2Dcur") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dcur") << "alive" << osc::EndMessage;	
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dcur") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket( oscPacket );
}

void TuioServer::startCursorBundle() {	
	oscPacket->Clear();	
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage( "/tuio/2Dcur") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dcur") << "alive";
	for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++) {
		/*if ((*tuioCursor)->getTuioState()!=TUIO_ADDED)*/ (*oscPacket) << (int32)((*tuioCursor)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;	
}

void TuioServer::addCursorMessage(TuioCursor *tcur) {
	
	//if (tcur->getTuioState()==TUIO_ADDED) return;

	float xpos = tcur->getX();
	float xvel = tcur->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tcur->getY();
	float yvel = tcur->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}

	(*oscPacket) << osc::BeginMessage( "/tuio/2Dcur") << "set";
	(*oscPacket) << (int32)(tcur->getSessionID()) << xpos << ypos;
	(*oscPacket) << xvel << yvel << tcur->getMotionAccel();	
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendCursorBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dcur") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket( oscPacket );
}

void TuioServer::sendEmptyObjectBundle() {
	oscPacket->Clear();	
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage( "/tuio/2Dobj") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dobj") << "alive" << osc::EndMessage;	
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dobj") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket( oscPacket );
}

void TuioServer::startObjectBundle() {
	oscPacket->Clear();	
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage( "/tuio/2Dobj") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dobj") << "alive";
	for (std::list<TuioObject*>::iterator tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++) {
		(*oscPacket) << (int32)((*tuioObject)->getSessionID());	
	}
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addObjectMessage(TuioObject *tobj) {
	
	float xpos = tobj->getX();
	float xvel = tobj->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tobj->getY();
	float yvel = tobj->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float angle = tobj->getAngle();
	float rvel = tobj->getRotationSpeed();
	if (invert_a) {
		angle = 2.0f*(float)M_PI - angle;
		rvel = -1 * rvel;
	}
	
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dobj") << "set";
	(*oscPacket) << (int32)(tobj->getSessionID()) << tobj->getSymbolID() << xpos << ypos << angle;
	(*oscPacket) << xvel << yvel << rvel << tobj->getMotionAccel() << tobj->getRotationAccel();	
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendObjectBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dobj") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket( oscPacket );
}




void TuioServer::sendEmptyBlobBundle() {
	oscPacket->Clear();	
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage( "/tuio/2Dblb") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dblb") << "alive" << osc::EndMessage;	
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dblb") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket( oscPacket );
}

void TuioServer::startBlobBundle() {	
	oscPacket->Clear();	
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage( "/tuio/2Dblb") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dblb") << "alive";
	for (std::list<TuioBlob*>::iterator tuioBlob = blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++) {
		/*if ((*tuioBlob)->getTuioState()!=TUIO_ADDED)*/ (*oscPacket) << (int32)((*tuioBlob)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;	
}

void TuioServer::addBlobMessage(TuioBlob *tblb) {
	
	//if (tblb->getTuioState()==TUIO_ADDED) return;
	
	float xpos = tblb->getX();
	float xvel = tblb->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tblb->getY();
	float yvel = tblb->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float angle = tblb->getAngle();
	float rvel = tblb->getRotationSpeed();
	if (invert_a) {
		angle = 2.0f*(float)M_PI - angle;
		rvel = -1 * rvel;
	}
	
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dblb") << "set";
	(*oscPacket) << (int32)(tblb->getSessionID()) << xpos << ypos << angle << tblb->getWidth() << tblb->getHeight() << tblb->getArea();
	(*oscPacket) << xvel << yvel  << rvel << tblb->getMotionAccel()  << tblb->getRotationAccel();	
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendBlobBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage( "/tuio/2Dblb") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;

	deliverOscPacket( oscPacket );
}


void TuioServer::sendEmptyCursor25DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/25Dcur") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dcur") << "alive" << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dcur") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::startCursor25DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/25Dcur") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dcur") << "alive";
	for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++) {
		/*if ((*tuioCursor)->getTuioState()!=TUIO_ADDED)*/ (*oscPacket) << (int32)((*tuioCursor)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addCursor25DMessage(TuioCursor25D *tcur) {

	//if (tcur->getTuioState()==TUIO_ADDED) return;

	float xpos = tcur->getX();
	float xvel = tcur->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tcur->getY();
	float yvel = tcur->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float zpos = tcur->getZ();
	float zvel = tcur->getZSpeed();
	if (invert_z) {
		zpos = 1 - zpos;
		zvel = -1 * zvel;
	}

	(*oscPacket) << osc::BeginMessage("/tuio/25Dcur") << "set";
	(*oscPacket) << (int32)(tcur->getSessionID()) << xpos << ypos << zpos;
	(*oscPacket) << xvel << yvel << zvel << tcur->getMotionAccel();
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendCursor25DBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage("/tuio/25Dcur") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::sendEmptyObject25DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/25Dobj") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dobj") << "alive" << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dobj") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::startObject25DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/25Dobj") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dobj") << "alive";
	for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++) {
		(*oscPacket) << (int32)((*tuioObject)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addObject25DMessage(TuioObject25D *tobj) {

	float xpos = tobj->getX();
	float xvel = tobj->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tobj->getY();
	float yvel = tobj->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float zpos = tobj->getZ();
	float zvel = tobj->getZSpeed();
	if (invert_z) {
		zpos = 1 - zpos;
		zvel = -1 * zvel;
	}
	float angle = tobj->getAngle();
	float rvel = tobj->getRotationSpeed();
	if (invert_a) {
		angle = 2.0f*(float)M_PI - angle;
		rvel = -1 * rvel;
	}

	(*oscPacket) << osc::BeginMessage("/tuio/25Dobj") << "set";
	(*oscPacket) << (int32)(tobj->getSessionID()) << tobj->getSymbolID() << xpos << ypos << zpos << angle;
	(*oscPacket) << xvel << yvel << zvel << rvel << tobj->getMotionAccel() << tobj->getRotationAccel();
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendObject25DBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage("/tuio/25Dobj") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}




void TuioServer::sendEmptyBlob25DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/25Dblb") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dblb") << "alive" << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dblb") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::startBlob25DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/25Dblb") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/25Dblb") << "alive";
	for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++) {
		/*if ((*tuioBlob)->getTuioState()!=TUIO_ADDED)*/ (*oscPacket) << (int32)((*tuioBlob)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addBlob25DMessage(TuioBlob25D *tblb) {

	//if (tblb->getTuioState()==TUIO_ADDED) return;

	float xpos = tblb->getX();
	float xvel = tblb->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tblb->getY();
	float yvel = tblb->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float zpos = tblb->getZ();
	float zvel = tblb->getZSpeed();
	if (invert_z) {
		zpos = 1 - zpos;
		zvel = -1 * zvel;
	}
	float angle = tblb->getAngle();
	float rvel = tblb->getRotationSpeed();
	if (invert_a) {
		angle = 2.0f*(float)M_PI - angle;
		rvel = -1 * rvel;
	}

	(*oscPacket) << osc::BeginMessage("/tuio/25Dblb") << "set";
	(*oscPacket) << (int32)(tblb->getSessionID()) << xpos << ypos << zpos << angle << tblb->getWidth() << tblb->getHeight() << tblb->getArea();
	(*oscPacket) << xvel << yvel << zvel << rvel << tblb->getMotionAccel() << tblb->getRotationAccel();
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendBlob25DBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage("/tuio/25Dblb") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;

	deliverOscPacket(oscPacket);
}




void TuioServer::sendEmptyCursor3DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/3Dcur") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dcur") << "alive" << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dcur") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::startCursor3DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/3Dcur") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dcur") << "alive";
	for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++) {
		/*if ((*tuioCursor)->getTuioState()!=TUIO_ADDED)*/ (*oscPacket) << (int32)((*tuioCursor)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addCursor3DMessage(TuioCursor3D *tcur) {

	//if (tcur->getTuioState()==TUIO_ADDED) return;

	float xpos = tcur->getX();
	float xvel = tcur->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tcur->getY();
	float yvel = tcur->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float zpos = tcur->getZ();
	float zvel = tcur->getZSpeed();
	if (invert_z) {
		zpos = 1 - zpos;
		zvel = -1 * zvel;
	}

	(*oscPacket) << osc::BeginMessage("/tuio/3Dcur") << "set";
	(*oscPacket) << (int32)(tcur->getSessionID()) << xpos << ypos << zpos;
	(*oscPacket) << xvel << yvel << zvel << tcur->getMotionAccel();
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendCursor3DBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage("/tuio/3Dcur") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::sendEmptyObject3DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/3Dobj") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dobj") << "alive" << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dobj") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::startObject3DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/3Dobj") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dobj") << "alive";
	for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++) {
		(*oscPacket) << (int32)((*tuioObject)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addObject3DMessage(TuioObject3D *tobj) {

	float xpos = tobj->getX();
	float xvel = tobj->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tobj->getY();
	float yvel = tobj->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float zpos = tobj->getZ();
	float zvel = tobj->getZSpeed();
	if (invert_z) {
		zpos = 1 - zpos;
		zvel = -1 * zvel;
	}
	float roll = tobj->getRoll();
	float rollvel = tobj->getRollSpeed();
	if (invert_a) {
		roll = 2.0f*(float)M_PI - roll;
		rollvel = -1 * rollvel;
	}
	float pitch = tobj->getPitch();
	float pitchvel = tobj->getPitchSpeed();
	if (invert_b) {
		pitch = 2.0f*(float)M_PI - pitch;
		pitchvel = -1 * pitchvel;
	}
	float yaw = tobj->getYaw();
	float yawvel = tobj->getYawSpeed();
	if (invert_c) {
		yaw = 2.0f*(float)M_PI - yaw;
		yawvel = -1 * yawvel;
	}

	(*oscPacket) << osc::BeginMessage("/tuio/3Dobj") << "set";
	(*oscPacket) << (int32)(tobj->getSessionID()) << tobj->getSymbolID() << xpos << ypos << zpos << roll << pitch << yaw;
	(*oscPacket) << xvel << yvel << zvel << rollvel << pitchvel << yawvel << tobj->getMotionAccel() << tobj->getRotationAccel();
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendObject3DBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage("/tuio/3Dobj") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}




void TuioServer::sendEmptyBlob3DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/3Dblb") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dblb") << "alive" << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dblb") << "fseq" << -1 << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket(oscPacket);
}

void TuioServer::startBlob3DBundle() {
	oscPacket->Clear();
	(*oscPacket) << osc::BeginBundleImmediate;
	if (source_name) (*oscPacket) << osc::BeginMessage("/tuio/3Dblb") << "source" << source_name << osc::EndMessage;
	(*oscPacket) << osc::BeginMessage("/tuio/3Dblb") << "alive";
	for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++) {
		/*if ((*tuioBlob)->getTuioState()!=TUIO_ADDED)*/ (*oscPacket) << (int32)((*tuioBlob)->getSessionID());
	}
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addBlob3DMessage(TuioBlob3D *tblb) {

	//if (tblb->getTuioState()==TUIO_ADDED) return;

	float xpos = tblb->getX();
	float xvel = tblb->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tblb->getY();
	float yvel = tblb->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float zpos = tblb->getZ();
	float zvel = tblb->getZSpeed();
	if (invert_z) {
		zpos = 1 - zpos;
		zvel = -1 * zvel;
	}
	float roll = tblb->getRoll();
	float rollvel = tblb->getRollSpeed();
	if (invert_a) {
		roll = 2.0f*(float)M_PI - roll;
		rollvel = -1 * rollvel;
	}
	float pitch = tblb->getPitch();
	float pitchvel = tblb->getPitchSpeed();
	if (invert_b) {
		pitch = 2.0f*(float)M_PI - pitch;
		pitchvel = -1 * pitchvel;
	}
	float yaw = tblb->getYaw();
	float yawvel = tblb->getYawSpeed();
	if (invert_c) {
		yaw = 2.0f*(float)M_PI - yaw;
		yawvel = -1 * yawvel;
	}

	(*oscPacket) << osc::BeginMessage("/tuio/3Dblb") << "set";
	(*oscPacket) << (int32)(tblb->getSessionID()) << xpos << ypos << zpos << roll << pitch << yaw << tblb->getWidth() << tblb->getHeight() << tblb->getDepth() << tblb->getVolume();
	(*oscPacket) << xvel << yvel << zvel << rollvel << pitchvel << yawvel << tblb->getMotionAccel() << tblb->getRotationAccel();
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::sendBlob3DBundle(long fseq) {
	(*oscPacket) << osc::BeginMessage("/tuio/3Dblb") << "fseq" << (int32)fseq << osc::EndMessage;
	(*oscPacket) << osc::EndBundle;

	deliverOscPacket(oscPacket);
}



void TuioServer::sendFullMessages() {
	
	// prepare the cursor packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "source" << source_name << osc::EndMessage;
	// add the cursor alive message
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "alive";
	for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++)
		(*fullPacket) << (int32)((*tuioCursor)->getSessionID());	
	(*fullPacket) << osc::EndMessage;	
	
	// add all current cursor set messages
	for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++) {
		
		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity()-fullPacket->Size())<CUR_MESSAGE_SIZE) {
			
			// add the immediate fseq message and send the cursor packet
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket( fullPacket );
			
			// prepare the new cursor packet
			fullPacket->Clear();	
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "source" << source_name << osc::EndMessage;
			// add the cursor alive message
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "alive";
			for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++)
				(*fullPacket) << (int32)((*tuioCursor)->getSessionID());	
			(*fullPacket) << osc::EndMessage;				
		}
		
		float xpos = (*tuioCursor)->getX();
		float xvel = (*tuioCursor)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioCursor)->getY();
		float yvel = (*tuioCursor)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		
		// add the actual cursor set message
		(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "set";
		(*fullPacket) << (int32)((*tuioCursor)->getSessionID()) << xpos << ypos;
		(*fullPacket) << xvel << yvel <<(*tuioCursor)->getMotionAccel();	
		(*fullPacket) << osc::EndMessage;	
	}
	
	// add the immediate fseq message and send the cursor packet
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket( fullPacket );
	
	// prepare the object packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "alive";
	for (std::list<TuioObject*>::iterator tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++)
		(*fullPacket) << (int32)((*tuioObject)->getSessionID());	
	(*fullPacket) << osc::EndMessage;	
	
	for (std::list<TuioObject*>::iterator tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++) {
		
		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity()-fullPacket->Size())<OBJ_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket( fullPacket );
			
			// prepare the new object packet
			fullPacket->Clear();	
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "source" << source_name << osc::EndMessage;
			// add the object alive message
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "alive";
			for (std::list<TuioObject*>::iterator tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++)
				(*fullPacket) << (int32)((*tuioObject)->getSessionID());	
			(*fullPacket) << osc::EndMessage;	
		}
		
		float xpos = (*tuioObject)->getX();
		float xvel = (*tuioObject)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioObject)->getY();
		float yvel = (*tuioObject)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float angle = (*tuioObject)->getAngle();
		float rvel = (*tuioObject)->getRotationSpeed();
		if (invert_a) {
			angle =  2.0f*(float)M_PI - angle;
			rvel = -1 * rvel;
		}
		
		// add the actual object set message
		(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "set";
		(*fullPacket) << (int32)((*tuioObject)->getSessionID()) << (*tuioObject)->getSymbolID() << xpos << ypos << angle;
		(*fullPacket) << xvel << yvel << rvel << (*tuioObject)->getMotionAccel() << (*tuioObject)->getRotationAccel();	
		(*fullPacket) << osc::EndMessage;
		
	}
	// add the immediate fseq message and send the object packet
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket( fullPacket );
	
	// prepare the blob packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dblb") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dblb") << "alive";
	for (std::list<TuioBlob*>::iterator tuioBlob = blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++)
		(*fullPacket) << (int32)((*tuioBlob)->getSessionID());	
	(*fullPacket) << osc::EndMessage;	
	
	for (std::list<TuioBlob*>::iterator tuioBlob = blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++) {
		
		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity()-fullPacket->Size())<BLB_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dblb") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket( fullPacket );
			
			// prepare the new blob packet
			fullPacket->Clear();	
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dblb") << "source" << source_name << osc::EndMessage;
			// add the blob alive message
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dblb") << "alive";
			for (std::list<TuioBlob*>::iterator tuioBlob = blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++)
				(*fullPacket) << (int32)((*tuioBlob)->getSessionID());	
			(*fullPacket) << osc::EndMessage;	
		}
		
		float xpos = (*tuioBlob)->getX();
		float xvel = (*tuioBlob)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioBlob)->getY();
		float yvel = (*tuioBlob)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float angle = (*tuioBlob)->getAngle();
		float rvel = (*tuioBlob)->getRotationSpeed();
		if (invert_a) {
			angle = 2.0f*(float)M_PI - angle;
			rvel = -1 * rvel;
		}		
		
		// add the actual blob set message
		(*fullPacket) << osc::BeginMessage( "/tuio/2Dblb") << "set";
		(*fullPacket) << (int32)((*tuioBlob)->getSessionID()) << xpos << ypos  << angle << (*tuioBlob)->getWidth() << (*tuioBlob)->getHeight() << (*tuioBlob)->getArea();
		(*fullPacket) << xvel << yvel << rvel << (*tuioBlob)->getMotionAccel() << (*tuioBlob)->getRotationAccel();	
		(*fullPacket) << osc::EndMessage;
		
	}
	// add the immediate fseq message and send the blob packet
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dblb") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket( fullPacket );





	// prepare the cursor 25D packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/25Dcur") << "source" << source_name << osc::EndMessage;
	// add the cursor 25D alive message
	(*fullPacket) << osc::BeginMessage("/tuio/25Dcur") << "alive";
	for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++)
		(*fullPacket) << (int32)((*tuioCursor)->getSessionID());
	(*fullPacket) << osc::EndMessage;

	// add all current cursor set messages
	for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++) {

		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity() - fullPacket->Size()) < CUR25D_MESSAGE_SIZE) {

			// add the immediate fseq message and send the cursor packet
			(*fullPacket) << osc::BeginMessage("/tuio/25Dcur") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket(fullPacket);

			// prepare the new cursor packet
			fullPacket->Clear();
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/25Dcur") << "source" << source_name << osc::EndMessage;
			// add the cursor alive message
			(*fullPacket) << osc::BeginMessage("/tuio/25Dcur") << "alive";
			for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++)
				(*fullPacket) << (int32)((*tuioCursor)->getSessionID());
			(*fullPacket) << osc::EndMessage;
		}

		float xpos = (*tuioCursor)->getX();
		float xvel = (*tuioCursor)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioCursor)->getY();
		float yvel = (*tuioCursor)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float zpos = (*tuioCursor)->getZ();
		float zvel = (*tuioCursor)->getZSpeed();
		if (invert_z) {
			zpos = 1 - zpos;
			zvel = -1 * zvel;
		}

		// add the actual cursor set message
		(*fullPacket) << osc::BeginMessage("/tuio/25Dcur") << "set";
		(*fullPacket) << (int32)((*tuioCursor)->getSessionID()) << xpos << ypos << zpos;
		(*fullPacket) << xvel << yvel << zvel << (*tuioCursor)->getMotionAccel();
		(*fullPacket) << osc::EndMessage;
	}

	// add the immediate fseq message and send the cursor packet
	(*fullPacket) << osc::BeginMessage("/tuio/25Dcur") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket(fullPacket);

	// prepare the object 25D packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/25Dobj") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage("/tuio/25Dobj") << "alive";
	for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++)
		(*fullPacket) << (int32)((*tuioObject)->getSessionID());
	(*fullPacket) << osc::EndMessage;

	for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++) {

		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity() - fullPacket->Size()) < OBJ25D_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage("/tuio/25Dobj") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket(fullPacket);

			// prepare the new object packet
			fullPacket->Clear();
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/25Dobj") << "source" << source_name << osc::EndMessage;
			// add the object alive message
			(*fullPacket) << osc::BeginMessage("/tuio/25Dobj") << "alive";
			for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++)
				(*fullPacket) << (int32)((*tuioObject)->getSessionID());
			(*fullPacket) << osc::EndMessage;
		}

		float xpos = (*tuioObject)->getX();
		float xvel = (*tuioObject)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioObject)->getY();
		float yvel = (*tuioObject)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float zpos = (*tuioObject)->getZ();
		float zvel = (*tuioObject)->getZSpeed();
		if (invert_z) {
			zpos = 1 - zpos;
			zvel = -1 * zvel;
		}
		float angle = (*tuioObject)->getAngle();
		float rvel = (*tuioObject)->getRotationSpeed();
		if (invert_a) {
			angle = 2.0f*(float)M_PI - angle;
			rvel = -1 * rvel;
		}

		// add the actual object set message
		(*fullPacket) << osc::BeginMessage("/tuio/25Dobj") << "set";
		(*fullPacket) << (int32)((*tuioObject)->getSessionID()) << (*tuioObject)->getSymbolID() << xpos << ypos << zpos << angle;
		(*fullPacket) << xvel << yvel << zvel << rvel << (*tuioObject)->getMotionAccel() << (*tuioObject)->getRotationAccel();
		(*fullPacket) << osc::EndMessage;

	}
	// add the immediate fseq message and send the object packet
	(*fullPacket) << osc::BeginMessage("/tuio/25Dobj") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket(fullPacket);

	// prepare the 25D blob packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/25Dblb") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage("/tuio/25Dblb") << "alive";
	for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++)
		(*fullPacket) << (int32)((*tuioBlob)->getSessionID());
	(*fullPacket) << osc::EndMessage;

	for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++) {

		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity() - fullPacket->Size()) < BLB25D_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage("/tuio/25Dblb") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket(fullPacket);

			// prepare the new blob packet
			fullPacket->Clear();
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/25Dblb") << "source" << source_name << osc::EndMessage;
			// add the blob alive message
			(*fullPacket) << osc::BeginMessage("/tuio/25Dblb") << "alive";
			for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++)
				(*fullPacket) << (int32)((*tuioBlob)->getSessionID());
			(*fullPacket) << osc::EndMessage;
		}

		float xpos = (*tuioBlob)->getX();
		float xvel = (*tuioBlob)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioBlob)->getY();
		float yvel = (*tuioBlob)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float zpos = (*tuioBlob)->getZ();
		float zvel = (*tuioBlob)->getZSpeed();
		if (invert_z) {
			zpos = 1 - zpos;
			zvel = -1 * zvel;
		}
		float angle = (*tuioBlob)->getAngle();
		float rvel = (*tuioBlob)->getRotationSpeed();
		if (invert_a) {
			angle = 2.0f*(float)M_PI - angle;
			rvel = -1 * rvel;
		}

		// add the actual blob set message
		(*fullPacket) << osc::BeginMessage("/tuio/25Dblb") << "set";
		(*fullPacket) << (int32)((*tuioBlob)->getSessionID()) << xpos << ypos << zpos << angle << (*tuioBlob)->getWidth() << (*tuioBlob)->getHeight() << (*tuioBlob)->getArea();
		(*fullPacket) << xvel << yvel << zvel << rvel << (*tuioBlob)->getMotionAccel() << (*tuioBlob)->getRotationAccel();
		(*fullPacket) << osc::EndMessage;

	}
	// add the immediate fseq message and send the blob packet
	(*fullPacket) << osc::BeginMessage("/tuio/25Dblb") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket(fullPacket);












	// prepare the cursor 3D packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/3Dcur") << "source" << source_name << osc::EndMessage;
	// add the cursor 3D alive message
	(*fullPacket) << osc::BeginMessage("/tuio/3Dcur") << "alive";
	for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++)
		(*fullPacket) << (int32)((*tuioCursor)->getSessionID());
	(*fullPacket) << osc::EndMessage;

	// add all current cursor set messages
	for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++) {

		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity() - fullPacket->Size()) < CUR3D_MESSAGE_SIZE) {

			// add the immediate fseq message and send the cursor packet
			(*fullPacket) << osc::BeginMessage("/tuio/3Dcur") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket(fullPacket);

			// prepare the new cursor packet
			fullPacket->Clear();
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/3Dcur") << "source" << source_name << osc::EndMessage;
			// add the cursor alive message
			(*fullPacket) << osc::BeginMessage("/tuio/3Dcur") << "alive";
			for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++)
				(*fullPacket) << (int32)((*tuioCursor)->getSessionID());
			(*fullPacket) << osc::EndMessage;
		}

		float xpos = (*tuioCursor)->getX();
		float xvel = (*tuioCursor)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioCursor)->getY();
		float yvel = (*tuioCursor)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float zpos = (*tuioCursor)->getZ();
		float zvel = (*tuioCursor)->getZSpeed();
		if (invert_z) {
			zpos = 1 - zpos;
			zvel = -1 * zvel;
		}

		// add the actual cursor set message
		(*fullPacket) << osc::BeginMessage("/tuio/3Dcur") << "set";
		(*fullPacket) << (int32)((*tuioCursor)->getSessionID()) << xpos << ypos << zpos;
		(*fullPacket) << xvel << yvel << zvel << (*tuioCursor)->getMotionAccel();
		(*fullPacket) << osc::EndMessage;
	}

	// add the immediate fseq message and send the cursor packet
	(*fullPacket) << osc::BeginMessage("/tuio/3Dcur") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket(fullPacket);

	// prepare the object 3D packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/3Dobj") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage("/tuio/3Dobj") << "alive";
	for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++)
		(*fullPacket) << (int32)((*tuioObject)->getSessionID());
	(*fullPacket) << osc::EndMessage;

	for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++) {

		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity() - fullPacket->Size()) < OBJ3D_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage("/tuio/3Dobj") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket(fullPacket);

			// prepare the new object packet
			fullPacket->Clear();
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/3Dobj") << "source" << source_name << osc::EndMessage;
			// add the object alive message
			(*fullPacket) << osc::BeginMessage("/tuio/3Dobj") << "alive";
			for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++)
				(*fullPacket) << (int32)((*tuioObject)->getSessionID());
			(*fullPacket) << osc::EndMessage;
		}

		float xpos = (*tuioObject)->getX();
		float xvel = (*tuioObject)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioObject)->getY();
		float yvel = (*tuioObject)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float zpos = (*tuioObject)->getZ();
		float zvel = (*tuioObject)->getZSpeed();
		if (invert_z) {
			zpos = 1 - zpos;
			zvel = -1 * zvel;
		}
		float roll = (*tuioObject)->getRoll();
		float rollvel = (*tuioObject)->getRollSpeed();
		if (invert_a) {
			roll = 2.0f*(float)M_PI - roll;
			rollvel = -1 * rollvel;
		}
		float pitch = (*tuioObject)->getPitch();
		float pitchvel = (*tuioObject)->getPitchSpeed();
		if (invert_b) {
			pitch = 2.0f*(float)M_PI - pitch;
			pitchvel = -1 * pitchvel;
		}
		float yaw = (*tuioObject)->getYaw();
		float yawvel = (*tuioObject)->getYawSpeed();
		if (invert_c) {
			yaw = 2.0f*(float)M_PI - yaw;
			yawvel = -1 * yawvel;
		}

		// add the actual object set message
		(*fullPacket) << osc::BeginMessage("/tuio/3Dobj") << "set";
		(*fullPacket) << (int32)((*tuioObject)->getSessionID()) << (*tuioObject)->getSymbolID() << xpos << ypos << zpos << roll << pitch << yaw;
		(*fullPacket) << xvel << yvel << zvel << rollvel << pitchvel << yawvel << (*tuioObject)->getMotionAccel() << (*tuioObject)->getRotationAccel();
		(*fullPacket) << osc::EndMessage;

	}
	// add the immediate fseq message and send the object packet
	(*fullPacket) << osc::BeginMessage("/tuio/3Dobj") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket(fullPacket);

	// prepare the 3D blob packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/3Dblb") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage("/tuio/3Dblb") << "alive";
	for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++)
		(*fullPacket) << (int32)((*tuioBlob)->getSessionID());
	(*fullPacket) << osc::EndMessage;

	for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++) {

		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity() - fullPacket->Size()) < BLB3D_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage("/tuio/3Dblb") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket(fullPacket);

			// prepare the new blob packet
			fullPacket->Clear();
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage("/tuio/3Dblb") << "source" << source_name << osc::EndMessage;
			// add the blob alive message
			(*fullPacket) << osc::BeginMessage("/tuio/3Dblb") << "alive";
			for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++)
				(*fullPacket) << (int32)((*tuioBlob)->getSessionID());
			(*fullPacket) << osc::EndMessage;
		}

		float xpos = (*tuioBlob)->getX();
		float xvel = (*tuioBlob)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioBlob)->getY();
		float yvel = (*tuioBlob)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float zpos = (*tuioBlob)->getZ();
		float zvel = (*tuioBlob)->getZSpeed();
		if (invert_z) {
			zpos = 1 - zpos;
			zvel = -1 * zvel;
		}
		float roll = (*tuioBlob)->getRoll();
		float rollvel = (*tuioBlob)->getRollSpeed();
		if (invert_a) {
			roll = 2.0f*(float)M_PI - roll;
			rollvel = -1 * rollvel;
		}
		float pitch = (*tuioBlob)->getPitch();
		float pitchvel = (*tuioBlob)->getPitchSpeed();
		if (invert_b) {
			pitch = 2.0f*(float)M_PI - pitch;
			pitchvel = -1 * pitchvel;
		}
		float yaw = (*tuioBlob)->getYaw();
		float yawvel = (*tuioBlob)->getYawSpeed();
		if (invert_c) {
			yaw = 2.0f*(float)M_PI - yaw;
			yawvel = -1 * yawvel;
		}

		// add the actual blob set message
		(*fullPacket) << osc::BeginMessage("/tuio/3Dblb") << "set";
		(*fullPacket) << (int32)((*tuioBlob)->getSessionID()) << xpos << ypos << zpos << roll << pitch << yaw << (*tuioBlob)->getWidth() << (*tuioBlob)->getHeight() << (*tuioBlob)->getDepth() << (*tuioBlob)->getVolume();
		(*fullPacket) << xvel << yvel << zvel << rollvel << pitchvel << yawvel << (*tuioBlob)->getMotionAccel() << (*tuioBlob)->getRotationAccel();
		(*fullPacket) << osc::EndMessage;

	}
	// add the immediate fseq message and send the blob packet
	(*fullPacket) << osc::BeginMessage("/tuio/3Dblb") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket(fullPacket);






}




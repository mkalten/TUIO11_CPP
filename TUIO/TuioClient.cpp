/*
 TUIO C++ Library
 Copyright (c) 2022 Nicolas Bremard <nicolas.bremard@laposte.net>

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

#include "TuioClient.h"
#include "UdpReceiver.h"

using namespace TUIO;
using namespace osc;


TuioClient::TuioClient()
: currentFrame	(-1)
, source_id		(0)
, source_name	(NULL)
, source_addr	(NULL)
, local_receiver(true)
{
	receiver = new UdpReceiver();
	initialize();
}

TuioClient::TuioClient(int port)
: currentFrame	(-1)
, source_id		(0)
, source_name	(NULL)
, source_addr	(NULL)
, local_receiver(true)
{
	receiver = new UdpReceiver(port);
	initialize();
}

TuioClient::TuioClient(OscReceiver *osc)
: currentFrame	(-1)
, source_id		(0)
, source_name	(NULL)
, source_addr	(NULL)
, receiver		(osc)
, local_receiver(false)
{
	initialize();
}

void TuioClient::initialize()	{	
	receiver->addTuioClient(this);
	maxCursorID[source_id] = -1;
	maxBlobID[source_id]   = -1;
	maxCursor25DID[source_id] = -1;
	maxBlob25DID[source_id] = -1;
	maxCursor3DID[source_id] = -1;
	maxBlob3DID[source_id] = -1;
}

TuioClient::~TuioClient() {
	if (local_receiver) delete receiver;
}

//TODO

void TuioClient::processOSC( const ReceivedMessage& msg ) {
	try {
		ReceivedMessageArgumentStream args = msg.ArgumentStream();
		//ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
		
		if( strcmp( msg.AddressPattern(), "/tuio/2Dobj" ) == 0 ){
			
			const char* cmd;
			args >> cmd;
			
			if (strcmp(cmd,"source")==0) {
				const char* src;
				args >> src;
				
				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");
				
				if (addr!=NULL) source_addr = addr;
				else source_addr = (char*)"localhost";
				
				// check if we know that source
				std::string source_str(src);
				std::map<std::string,int>::iterator iter = sourceList.find(source_str);

				// add a new source
				if (iter==sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
				} else {
				// use the found source_id
					source_id = sourceList[source_str];
				}
				
			} else if (strcmp(cmd,"set")==0) {	
				int32 s_id, c_id;
				float xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel;
				args >> s_id >> c_id >> xpos >> ypos >> angle >> xspeed >> yspeed >> rspeed >> maccel >> raccel;

				lockObjectList();
				std::list<TuioObject*>::iterator tobj;
				for (tobj=objectList.begin(); tobj!= objectList.end(); tobj++)
					if((*tobj)->getSessionID()==(long)s_id) break;

				if (tobj == objectList.end()) {
					
					TuioObject *addObject = new TuioObject((long)s_id,(int)c_id,xpos,ypos,angle);
					frameObjects.push_back(addObject);

				} else if ( ((*tobj)->getX()!=xpos) || ((*tobj)->getY()!=ypos) || ((*tobj)->getAngle()!=angle) || ((*tobj)->getXSpeed()!=xspeed) || ((*tobj)->getYSpeed()!=yspeed) || ((*tobj)->getRotationSpeed()!=rspeed) || ((*tobj)->getMotionAccel()!=maccel) || ((*tobj)->getRotationAccel()!=raccel) ) {

					TuioObject *updateObject = new TuioObject((long)s_id,(*tobj)->getSymbolID(),xpos,ypos,angle);
					updateObject->update(xpos,ypos,angle,xspeed,yspeed,rspeed,maccel,raccel);
					frameObjects.push_back(updateObject);

				}
				unlockObjectList();

			} else if (strcmp(cmd,"alive")==0) {

				int32 s_id;
				aliveObjectList.clear();
				while(!args.Eos()) {
					args >> s_id;
					aliveObjectList.push_back((long)s_id);
				}

			} else if (strcmp(cmd,"fseq")==0) {

				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq>0) {
					if (fseq>currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq>=currentFrame) || ((currentFrame-fseq)>100)) currentFrame = fseq;
					else lateFrame = true;
				} else if ((TuioTime::getSessionTime().getTotalMilliseconds()-currentTime.getTotalMilliseconds())>100) {
					currentTime = TuioTime::getSessionTime();
				}
			
				if (!lateFrame) {
					
					lockObjectList();
					//find the removed objects first
					for (std::list<TuioObject*>::iterator tobj=objectList.begin(); tobj != objectList.end(); tobj++) {
						if ((*tobj)->getTuioSourceID()==source_id) {
							std::list<long>::iterator iter = find(aliveObjectList.begin(), aliveObjectList.end(), (*tobj)->getSessionID());
							if (iter == aliveObjectList.end()) {
								(*tobj)->remove(currentTime);
								frameObjects.push_back(*tobj);							
							}
						}
					}
					unlockObjectList();
					
					for (std::list<TuioObject*>::iterator iter=frameObjects.begin(); iter != frameObjects.end(); iter++) {
						TuioObject *tobj = (*iter);

						TuioObject *frameObject = NULL;
						switch (tobj->getTuioState()) {
							case TUIO_REMOVED:

								frameObject = tobj;
								frameObject->remove(currentTime);

								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->removeTuioObject(frameObject);

								lockObjectList();
								for (std::list<TuioObject*>::iterator delobj=objectList.begin(); delobj!=objectList.end(); delobj++) {
									if((*delobj)->getSessionID()==frameObject->getSessionID()) {
										objectList.erase(delobj);
										break;
									}
								}
								unlockObjectList();
								break;
							case TUIO_ADDED:

								lockObjectList();
								frameObject = new TuioObject(currentTime,tobj->getSessionID(),tobj->getSymbolID(),tobj->getX(),tobj->getY(),tobj->getAngle());
								if (source_name) frameObject->setTuioSource(source_id,source_name,source_addr);
								objectList.push_back(frameObject);
								unlockObjectList();
								
								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->addTuioObject(frameObject);

								break;
							default:

								lockObjectList();
								std::list<TuioObject*>::iterator iter;
								for (iter=objectList.begin(); iter != objectList.end(); iter++) {
									if (((*iter)->getTuioSourceID()==source_id) && ((*iter)->getSessionID()==tobj->getSessionID())) {
										frameObject = (*iter);
										break;
									}
								}	
								
								if (iter==objectList.end()) {
									unlockObjectList();
									break;
								}
								
								if ( (tobj->getX()!=frameObject->getX() && tobj->getXSpeed()==0) || (tobj->getY()!=frameObject->getY() && tobj->getYSpeed()==0) )
									frameObject->update(currentTime,tobj->getX(),tobj->getY(),tobj->getAngle());
								else
									frameObject->update(currentTime,tobj->getX(),tobj->getY(),tobj->getAngle(),tobj->getXSpeed(),tobj->getYSpeed(),tobj->getRotationSpeed(),tobj->getMotionAccel(),tobj->getRotationAccel());
								
								unlockObjectList();
								
								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->updateTuioObject(frameObject);
						}
						delete tobj;
					}
					
					for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);
					
				} else {
					for (std::list<TuioObject*>::iterator iter=frameObjects.begin(); iter != frameObjects.end(); iter++) {
						TuioObject *tobj = (*iter);
						delete tobj;
					}
				}
				
				frameObjects.clear();
			}
		} 
		else if( strcmp( msg.AddressPattern(), "/tuio/2Dcur" ) == 0 ) {
			const char* cmd;
			args >> cmd;
			
			if (strcmp(cmd,"source")==0) {
				const char* src;
				args >> src;
				
				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");
				
				if (addr!=NULL) source_addr = addr;
				else source_addr = (char*)"localhost";
				
				// check if we know that source
				std::string source_str(src);
				std::map<std::string,int>::iterator iter = sourceList.find(source_str);
				
				// add a new source
				if (iter==sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
					maxCursorID[source_id] = -1;
				} else {
					// use the found source_id
					source_id = sourceList[source_str];
				}
				
			} else if (strcmp(cmd,"set")==0) {	

				int32 s_id;
				float xpos, ypos, xspeed, yspeed, maccel;				
				args >> s_id >> xpos >> ypos >> xspeed >> yspeed >> maccel;
				
				lockCursorList();
				std::list<TuioCursor*>::iterator tcur;
				for (tcur=cursorList.begin(); tcur!= cursorList.end(); tcur++)
					if (((*tcur)->getSessionID()==(long)s_id) && ((*tcur)->getTuioSourceID()==source_id)) break;
				
				if (tcur==cursorList.end()) {
									
					TuioCursor *addCursor = new TuioCursor((long)s_id,-1,xpos,ypos);
					frameCursors.push_back(addCursor);

				} else if ( ((*tcur)->getX()!=xpos) || ((*tcur)->getY()!=ypos) || ((*tcur)->getXSpeed()!=xspeed) || ((*tcur)->getYSpeed()!=yspeed) || ((*tcur)->getMotionAccel()!=maccel) ) {

					TuioCursor *updateCursor = new TuioCursor((long)s_id,(*tcur)->getCursorID(),xpos,ypos);
					updateCursor->update(xpos,ypos,0,xspeed,yspeed,0,maccel);
					frameCursors.push_back(updateCursor);

				}
				unlockCursorList();
				
			} else if (strcmp(cmd,"alive")==0) {
				
				int32 s_id;
				aliveCursorList.clear();
				while(!args.Eos()) {
					args >> s_id;
					aliveCursorList.push_back((long)s_id);
				}
				
			} else if( strcmp( cmd, "fseq" ) == 0 ) {
				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq>0) {
					if (fseq>currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq>=currentFrame) || ((currentFrame-fseq)>100)) currentFrame = fseq;
					else lateFrame = true;
				}  else if ((TuioTime::getSessionTime().getTotalMilliseconds()-currentTime.getTotalMilliseconds())>100) {
					currentTime = TuioTime::getSessionTime();
				}
			
				if (!lateFrame) {
					
					lockCursorList();
					// find the removed cursors first
					for (std::list<TuioCursor*>::iterator tcur=cursorList.begin(); tcur != cursorList.end(); tcur++) {
						if ((*tcur)->getTuioSourceID()==source_id) {
							std::list<long>::iterator iter = find(aliveCursorList.begin(), aliveCursorList.end(), (*tcur)->getSessionID());
							
							if (iter == aliveCursorList.end()) {
									(*tcur)->remove(currentTime);
									frameCursors.push_back(*tcur);
							}
						}
					}
					unlockCursorList();
					
					for (std::list<TuioCursor*>::iterator iter=frameCursors.begin(); iter != frameCursors.end(); iter++) {
						TuioCursor *tcur = (*iter);
						
						int c_id = 0;
						int free_size = 0;
						TuioCursor *frameCursor = NULL;
						switch (tcur->getTuioState()) {
							case TUIO_REMOVED:
		
								frameCursor = tcur;
								frameCursor->remove(currentTime);
	
								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->removeTuioCursor(frameCursor);

								lockCursorList();
								for (std::list<TuioCursor*>::iterator delcur=cursorList.begin(); delcur!=cursorList.end(); delcur++) {
									if(((*delcur)->getTuioSourceID()==source_id) && ((*delcur)->getSessionID()==frameCursor->getSessionID())) {
										cursorList.erase(delcur);
										break;
									}
								}

								if (frameCursor->getCursorID()==maxCursorID[source_id]) {
									maxCursorID[source_id] = -1;
									delete frameCursor;
									
									if (cursorList.size()>0) {
										std::list<TuioCursor*>::iterator clist;
										for (clist=cursorList.begin(); clist != cursorList.end(); clist++) {
											if ((*clist)->getTuioSourceID()==source_id) {
												c_id = (*clist)->getCursorID();
												if (c_id>maxCursorID[source_id]) maxCursorID[source_id]=c_id;
											}
										}
			
										freeCursorBuffer.clear();
										for (std::list<TuioCursor*>::iterator flist=freeCursorList.begin(); flist != freeCursorList.end(); flist++) {
											TuioCursor *freeCursor = (*flist);
											if (freeCursor->getTuioSourceID()==source_id) {
												if (freeCursor->getCursorID()>maxCursorID[source_id]) delete freeCursor;
												else freeCursorBuffer.push_back(freeCursor);
											} else freeCursorBuffer.push_back(freeCursor);
										}	
										freeCursorList = freeCursorBuffer;

									} else {
										freeCursorBuffer.clear();
										for (std::list<TuioCursor*>::iterator flist=freeCursorList.begin(); flist != freeCursorList.end(); flist++) {
											TuioCursor *freeCursor = (*flist);
											if (freeCursor->getTuioSourceID()==source_id) delete freeCursor;
											else freeCursorBuffer.push_back(freeCursor);
										}	
										freeCursorList = freeCursorBuffer;
										
									}
								} else if (frameCursor->getCursorID()<maxCursorID[source_id]) {
									freeCursorList.push_back(frameCursor);
								} 
								
								unlockCursorList();
								break;
							case TUIO_ADDED:
								
								lockCursorList();
								for(std::list<TuioCursor*>::iterator iter = cursorList.begin();iter!= cursorList.end(); iter++)
									if ((*iter)->getTuioSourceID()==source_id) c_id++;
								
								for(std::list<TuioCursor*>::iterator iter = freeCursorList.begin();iter!= freeCursorList.end(); iter++)
									if ((*iter)->getTuioSourceID()==source_id) free_size++;
								
								if ((free_size<=maxCursorID[source_id]) && (free_size>0)) {
									std::list<TuioCursor*>::iterator closestCursor = freeCursorList.begin();
									
									for(std::list<TuioCursor*>::iterator iter = freeCursorList.begin();iter!= freeCursorList.end(); iter++) {
										if (((*iter)->getTuioSourceID()==source_id) && ((*iter)->getDistance(tcur)<(*closestCursor)->getDistance(tcur))) closestCursor = iter;
									}
									
									if (closestCursor!=freeCursorList.end()) {
										TuioCursor *freeCursor = (*closestCursor);
										c_id = freeCursor->getCursorID();
										freeCursorList.erase(closestCursor);
										delete freeCursor;
									}
								} else maxCursorID[source_id] = c_id;									
								
								frameCursor = new TuioCursor(currentTime,tcur->getSessionID(),c_id,tcur->getX(),tcur->getY());
								if (source_name) frameCursor->setTuioSource(source_id,source_name,source_addr);
								cursorList.push_back(frameCursor);
								
								delete tcur;
								unlockCursorList();
								
								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->addTuioCursor(frameCursor);
								
								break;
							default:
								
								lockCursorList();
								std::list<TuioCursor*>::iterator iter;
								for (iter=cursorList.begin(); iter != cursorList.end(); iter++) {
									if (((*iter)->getTuioSourceID()==source_id) && ((*iter)->getSessionID()==tcur->getSessionID())) {
										frameCursor = (*iter);
										break;
									}
								}	
								
								if (iter==cursorList.end()) {
									unlockCursorList();
									break;
								}
								
								if ( (tcur->getX()!=frameCursor->getX() && tcur->getXSpeed()==0) || (tcur->getY()!=frameCursor->getY() && tcur->getYSpeed()==0) )
									frameCursor->update(currentTime,tcur->getX(),tcur->getY(), tcur->getZ());
								else
									frameCursor->update(currentTime,tcur->getX(),tcur->getY(), tcur->getZ(),tcur->getXSpeed(),tcur->getYSpeed(), tcur->getZSpeed(),tcur->getMotionAccel());
			
								delete tcur;
								unlockCursorList();

								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->updateTuioCursor(frameCursor);

						}	
					}
					
					for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);
					
				} else {
					for (std::list<TuioCursor*>::iterator iter=frameCursors.begin(); iter != frameCursors.end(); iter++) {
						TuioCursor *tcur = (*iter);
						delete tcur;
					}
				}
				
				frameCursors.clear();
			}
		} else 
		if( strcmp( msg.AddressPattern(), "/tuio/2Dblb" ) == 0 ){
			const char* cmd;
			args >> cmd;
			
			if (strcmp(cmd,"source")==0) {	
				const char* src;
				args >> src;
				
				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");
				
				if (addr!=NULL) source_addr = addr;
				else source_addr = (char*)"localhost";
				
				// check if we know that source
				std::string source_str(src);
				std::map<std::string,int>::iterator iter = sourceList.find(source_str);
				
				// add a new source
				if (iter==sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
					maxBlobID[source_id]   = -1;
				} else {
					// use the found source_id
					source_id = sourceList[source_str];
				}
				
			} else if (strcmp(cmd,"set")==0) {	
				
				int32 s_id;
				float xpos, ypos, angle, width, height, area, xspeed, yspeed, rspeed, maccel, raccel;				
				args >> s_id >> xpos >> ypos >> angle >> width >> height >> area >> xspeed >> yspeed >> rspeed >> maccel >> raccel;
				
				lockBlobList();
				std::list<TuioBlob*>::iterator tblb;
				for (tblb=blobList.begin(); tblb!= blobList.end(); tblb++)
					if((*tblb)->getSessionID()==(long)s_id) break;
				
				if (tblb==blobList.end()) {
					
					TuioBlob *addBlob = new TuioBlob((long)s_id,-1,xpos,ypos,angle,width,height,area);
					frameBlobs.push_back(addBlob);
					
				} else if ( ((*tblb)->getX()!=xpos) || ((*tblb)->getY()!=ypos) || ((*tblb)->getAngle()!=angle) || ((*tblb)->getWidth()!=width) || ((*tblb)->getHeight()!=height) || ((*tblb)->getArea()!=area) || ((*tblb)->getXSpeed()!=xspeed) || ((*tblb)->getYSpeed()!=yspeed) || ((*tblb)->getMotionAccel()!=maccel) ) {
					
					TuioBlob *updateBlob = new TuioBlob((long)s_id,(*tblb)->getBlobID(),xpos,ypos,angle,width,height,area);
					updateBlob->update(xpos,ypos,angle,width,height,area,xspeed,yspeed,rspeed,maccel,raccel);
					frameBlobs.push_back(updateBlob);
				}
				unlockBlobList();
				
			} else if (strcmp(cmd,"alive")==0) {
				
				int32 s_id;
				aliveBlobList.clear();
				while(!args.Eos()) {
					args >> s_id;
					aliveBlobList.push_back((long)s_id);
				}
				
			} else if( strcmp( cmd, "fseq" ) == 0 ) {
				
				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq>0) {
					if (fseq>currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq>=currentFrame) || ((currentFrame-fseq)>100)) currentFrame = fseq;
					else lateFrame = true;
				}  else if ((TuioTime::getSessionTime().getTotalMilliseconds()-currentTime.getTotalMilliseconds())>100) {
					currentTime = TuioTime::getSessionTime();
				}
				
				if (!lateFrame) {
					
					lockBlobList();
					// find the removed blobs first
					for (std::list<TuioBlob*>::iterator tblb=blobList.begin(); tblb != blobList.end(); tblb++) {
						if ((*tblb)->getTuioSourceID()==source_id) {
							std::list<long>::iterator iter = find(aliveBlobList.begin(), aliveBlobList.end(), (*tblb)->getSessionID());
							
							if (iter == aliveBlobList.end()) {
								(*tblb)->remove(currentTime);
								frameBlobs.push_back(*tblb);
							}
						}
					}
					unlockBlobList();
					
					for (std::list<TuioBlob*>::iterator iter=frameBlobs.begin(); iter != frameBlobs.end(); iter++) {
						TuioBlob *tblb = (*iter);
						
						int b_id = 0;
						int free_size = 0;
						TuioBlob *frameBlob = NULL;
						switch (tblb->getTuioState()) {
							case TUIO_REMOVED:
								frameBlob = tblb;
								frameBlob->remove(currentTime);
								
								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->removeTuioBlob(frameBlob);
								
								lockBlobList();
								for (std::list<TuioBlob*>::iterator delblb=blobList.begin(); delblb!=blobList.end(); delblb++) {
									if(((*delblb)->getTuioSourceID()==source_id) && ((*delblb)->getSessionID()==frameBlob->getSessionID())) {
										blobList.erase(delblb);
										break;
									}
								}
								
								if (frameBlob->getBlobID()==maxBlobID[source_id]) {
									maxBlobID[source_id] = -1;
									delete frameBlob;
									
									if (blobList.size()>0) {
										std::list<TuioBlob*>::iterator clist;
										for (clist=blobList.begin(); clist != blobList.end(); clist++) {
											if ((*clist)->getTuioSourceID()==source_id) {
												b_id = (*clist)->getBlobID();
												if (b_id>maxBlobID[source_id]) maxBlobID[source_id]=b_id;
											}
										}
										
										freeBlobBuffer.clear();
										for (std::list<TuioBlob*>::iterator flist=freeBlobList.begin(); flist != freeBlobList.end(); flist++) {
											TuioBlob *freeBlob = (*flist);
											if (freeBlob->getTuioSourceID()==source_id) {
												if (freeBlob->getBlobID()>maxBlobID[source_id]) delete freeBlob;
												else freeBlobBuffer.push_back(freeBlob);
											} else freeBlobBuffer.push_back(freeBlob);
										}	
										freeBlobList = freeBlobBuffer;
										
									} else {
										freeBlobBuffer.clear();
										for (std::list<TuioBlob*>::iterator flist=freeBlobList.begin(); flist != freeBlobList.end(); flist++) {
											TuioBlob *freeBlob = (*flist);
											if (freeBlob->getTuioSourceID()==source_id) delete freeBlob;
											else freeBlobBuffer.push_back(freeBlob);
										}	
										freeBlobList = freeBlobBuffer;
										
									}
								} else if (frameBlob->getBlobID()<maxBlobID[source_id]) {
									freeBlobList.push_back(frameBlob);
								} 
								
								unlockBlobList();
								break;
							case TUIO_ADDED:
								
								lockBlobList();
								for(std::list<TuioBlob*>::iterator iter = blobList.begin();iter!= blobList.end(); iter++)
									if ((*iter)->getTuioSourceID()==source_id) b_id++;
								
								for(std::list<TuioBlob*>::iterator iter = freeBlobList.begin();iter!= freeBlobList.end(); iter++)
									if ((*iter)->getTuioSourceID()==source_id) free_size++;
								
								if ((free_size<=maxBlobID[source_id]) && (free_size>0)) {
									std::list<TuioBlob*>::iterator closestBlob = freeBlobList.begin();
									
									for(std::list<TuioBlob*>::iterator iter = freeBlobList.begin();iter!= freeBlobList.end(); iter++) {
										if (((*iter)->getTuioSourceID()==source_id) && ((*iter)->getDistance(tblb)<(*closestBlob)->getDistance(tblb))) closestBlob = iter;
									}
									
									if (closestBlob!=freeBlobList.end()) {
										TuioBlob *freeBlob = (*closestBlob);
										b_id = freeBlob->getBlobID();
										freeBlobList.erase(closestBlob);
										delete freeBlob;
									}
								} else maxBlobID[source_id] = b_id;									
								
								frameBlob = new TuioBlob(currentTime,tblb->getSessionID(),b_id,tblb->getX(),tblb->getY(),tblb->getAngle(),tblb->getWidth(),tblb->getHeight(),tblb->getArea());
								if (source_name) frameBlob->setTuioSource(source_id,source_name,source_addr);
								blobList.push_back(frameBlob);
								
								delete tblb;
								unlockBlobList();
								
								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->addTuioBlob(frameBlob);
							
								break;
							default:
								
								lockBlobList();
								std::list<TuioBlob*>::iterator iter;
								for (iter=blobList.begin(); iter != blobList.end(); iter++) {
									if (((*iter)->getTuioSourceID()==source_id) && ((*iter)->getSessionID()==tblb->getSessionID())) {
										frameBlob = (*iter);
										break;
									}
								}	
								
								if (iter==blobList.end()) {
									unlockBlobList();
									break;
								}
								
								if ( (tblb->getX()!=frameBlob->getX() && tblb->getXSpeed()==0) || (tblb->getY()!=frameBlob->getY() && tblb->getYSpeed()==0) || (tblb->getAngle()!=frameBlob->getAngle() && tblb->getRotationSpeed()==0) )
									frameBlob->update(currentTime,tblb->getX(),tblb->getY(),tblb->getAngle(),tblb->getWidth(),tblb->getHeight(),tblb->getArea());
								else
									frameBlob->update(currentTime,tblb->getX(),tblb->getY(),tblb->getAngle(),tblb->getWidth(),tblb->getHeight(),tblb->getArea(),tblb->getXSpeed(),tblb->getYSpeed(),tblb->getRotationSpeed(),tblb->getMotionAccel(),tblb->getRotationAccel());
								
								delete tblb;
								unlockBlobList();
								
								for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
									(*listener)->updateTuioBlob(frameBlob);
						}	
					}
					
					for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);
					
				} else {
					for (std::list<TuioBlob*>::iterator iter=frameBlobs.begin(); iter != frameBlobs.end(); iter++) {
						TuioBlob *tblb = (*iter);
						delete tblb;
					}
				}
				
				frameBlobs.clear();
			}
		}
		else


		//25D
		if (strcmp(msg.AddressPattern(), "/tuio/25Dobj") == 0) {

			const char* cmd;
			args >> cmd;

			if (strcmp(cmd, "source") == 0) {
				const char* src;
				args >> src;

				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");

				if (addr != NULL) source_addr = addr;
				else source_addr = (char*)"localhost";

				// check if we know that source
				std::string source_str(src);
				std::map<std::string, int>::iterator iter = sourceList.find(source_str);

				// add a new source
				if (iter == sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
				}
				else {
					// use the found source_id
					source_id = sourceList[source_str];
				}

			}
			else if (strcmp(cmd, "set") == 0) {
				int32 s_id, c_id;
				float xpos, ypos,zpos, angle, xspeed, yspeed,zspeed, rspeed, maccel, raccel;
				args >> s_id >> c_id >> xpos >> ypos >> zpos >> angle >> xspeed >> yspeed >> zspeed >> rspeed >> maccel >> raccel;

				lockObject25DList();
				std::list<TuioObject25D*>::iterator tobj;
				for (tobj = object25DList.begin(); tobj != object25DList.end(); tobj++)
					if ((*tobj)->getSessionID() == (long)s_id) break;

				if (tobj == object25DList.end()) {

					TuioObject25D *addObject = new TuioObject25D((long)s_id, (int)c_id, xpos, ypos,zpos, angle);
					frameObjects25D.push_back(addObject);

				}
				else if (((*tobj)->getX() != xpos) || ((*tobj)->getY() != ypos) || ((*tobj)->getZ() != zpos) || ((*tobj)->getAngle() != angle) || ((*tobj)->getXSpeed() != xspeed) || ((*tobj)->getYSpeed() != yspeed) || ((*tobj)->getZSpeed() != zspeed) || ((*tobj)->getRotationSpeed() != rspeed) || ((*tobj)->getMotionAccel() != maccel) || ((*tobj)->getRotationAccel() != raccel)) {

					TuioObject25D *updateObject = new TuioObject25D((long)s_id, (*tobj)->getSymbolID(), xpos, ypos,zpos, angle);
					updateObject->update(xpos, ypos, zpos, angle, xspeed, yspeed, zspeed, rspeed, maccel, raccel);
					frameObjects25D.push_back(updateObject);

				}
				unlockObject25DList();

			}
			else if (strcmp(cmd, "alive") == 0) {

				int32 s_id;
				aliveObject25DList.clear();
				while (!args.Eos()) {
					args >> s_id;
					aliveObject25DList.push_back((long)s_id);
				}

			}
			else if (strcmp(cmd, "fseq") == 0) {

				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq > 0) {
					if (fseq > currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq >= currentFrame) || ((currentFrame - fseq) > 100)) currentFrame = fseq;
					else lateFrame = true;
				}
				else if ((TuioTime::getSessionTime().getTotalMilliseconds() - currentTime.getTotalMilliseconds()) > 100) {
					currentTime = TuioTime::getSessionTime();
				}

				if (!lateFrame) {

					lockObject25DList();
					//find the removed objects first
					for (std::list<TuioObject25D*>::iterator tobj = object25DList.begin(); tobj != object25DList.end(); tobj++) {
						if ((*tobj)->getTuioSourceID() == source_id) {
							std::list<long>::iterator iter = find(aliveObject25DList.begin(), aliveObject25DList.end(), (*tobj)->getSessionID());
							if (iter == aliveObject25DList.end()) {
								(*tobj)->remove(currentTime);
								frameObjects25D.push_back(*tobj);
							}
						}
					}
					unlockObject25DList();

					for (std::list<TuioObject25D*>::iterator iter = frameObjects25D.begin(); iter != frameObjects25D.end(); iter++) {
						TuioObject25D *tobj = (*iter);

						TuioObject25D *frameObject = NULL;
						switch (tobj->getTuioState()) {
						case TUIO_REMOVED:

							frameObject = tobj;
							frameObject->remove(currentTime);

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->removeTuioObject25D(frameObject);

							lockObject25DList();
							for (std::list<TuioObject25D*>::iterator delobj = object25DList.begin(); delobj != object25DList.end(); delobj++) {
								if ((*delobj)->getSessionID() == frameObject->getSessionID()) {
									object25DList.erase(delobj);
									break;
								}
							}
							unlockObject25DList();
							break;
						case TUIO_ADDED:

							lockObject25DList();
							frameObject = new TuioObject25D(currentTime, tobj->getSessionID(), tobj->getSymbolID(), tobj->getX(), tobj->getY(), tobj->getZ(), tobj->getAngle());
							if (source_name) frameObject->setTuioSource(source_id, source_name, source_addr);
							object25DList.push_back(frameObject);
							unlockObject25DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->addTuioObject25D(frameObject);

							break;
						default:

							lockObject25DList();
							std::list<TuioObject25D*>::iterator iter;
							for (iter = object25DList.begin(); iter != object25DList.end(); iter++) {
								if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getSessionID() == tobj->getSessionID())) {
									frameObject = (*iter);
									break;
								}
							}

							if (iter == object25DList.end()) {
								unlockObject25DList();
								break;
							}

							if ((tobj->getX() != frameObject->getX() && tobj->getXSpeed() == 0) || (tobj->getY() != frameObject->getY() && tobj->getYSpeed() == 0) || (tobj->getZ() != frameObject->getZ() && tobj->getZSpeed() == 0) || (tobj->getAngle() != frameObject->getAngle() && tobj->getRotationSpeed() == 0))
								frameObject->update(currentTime, tobj->getX(), tobj->getY(), tobj->getZ(), tobj->getAngle());
							else
								frameObject->update(currentTime, tobj->getX(), tobj->getY(), tobj->getZ(), tobj->getAngle(), tobj->getXSpeed(), tobj->getYSpeed(), tobj->getZSpeed(), tobj->getRotationSpeed(), tobj->getMotionAccel(), tobj->getRotationAccel());

							unlockObject25DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->updateTuioObject25D(frameObject);
						}
						delete tobj;
					}

					for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);

				}
				else {
					for (std::list<TuioObject25D*>::iterator iter = frameObjects25D.begin(); iter != frameObjects25D.end(); iter++) {
						TuioObject25D *tobj = (*iter);
						delete tobj;
					}
				}

				frameObjects25D.clear();
			}
		}
		else if (strcmp(msg.AddressPattern(), "/tuio/25Dcur") == 0) {
			const char* cmd;
			args >> cmd;

			if (strcmp(cmd, "source") == 0) {
				const char* src;
				args >> src;

				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");

				if (addr != NULL) source_addr = addr;
				else source_addr = (char*)"localhost";

				// check if we know that source
				std::string source_str(src);
				std::map<std::string, int>::iterator iter = sourceList.find(source_str);

				// add a new source
				if (iter == sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
					maxCursor25DID[source_id] = -1;
				}
				else {
					// use the found source_id
					source_id = sourceList[source_str];
				}

			}
			else if (strcmp(cmd, "set") == 0) {

				int32 s_id;
				float xpos, ypos,zpos, xspeed, yspeed,zspeed, maccel;
				args >> s_id >> xpos >> ypos >> zpos >> xspeed >> yspeed >> zspeed >> maccel;

				lockCursor25DList();
				std::list<TuioCursor25D*>::iterator tcur;
				for (tcur = cursor25DList.begin(); tcur != cursor25DList.end(); tcur++)
					if (((*tcur)->getSessionID() == (long)s_id) && ((*tcur)->getTuioSourceID() == source_id)) break;

				if (tcur == cursor25DList.end()) {

					TuioCursor25D *addCursor = new TuioCursor25D((long)s_id, -1, xpos, ypos,zpos);
					frameCursors25D.push_back(addCursor);

				}
				else if (((*tcur)->getX() != xpos) || ((*tcur)->getY() != ypos) || ((*tcur)->getZ() != zpos) || ((*tcur)->getXSpeed() != xspeed) || ((*tcur)->getYSpeed() != yspeed) || ((*tcur)->getZSpeed() != zspeed) || ((*tcur)->getMotionAccel() != maccel)) {

					TuioCursor25D *updateCursor = new TuioCursor25D((long)s_id, (*tcur)->getCursorID(), xpos, ypos,zpos);
					updateCursor->update(xpos, ypos, zpos, xspeed, yspeed, zpos, maccel);
					frameCursors25D.push_back(updateCursor);

				}
				unlockCursor25DList();

			}
			else if (strcmp(cmd, "alive") == 0) {

				int32 s_id;
				aliveCursor25DList.clear();
				while (!args.Eos()) {
					args >> s_id;
					aliveCursor25DList.push_back((long)s_id);
				}

			}
			else if (strcmp(cmd, "fseq") == 0) {
				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq > 0) {
					if (fseq > currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq >= currentFrame) || ((currentFrame - fseq) > 100)) currentFrame = fseq;
					else lateFrame = true;
				}
				else if ((TuioTime::getSessionTime().getTotalMilliseconds() - currentTime.getTotalMilliseconds()) > 100) {
					currentTime = TuioTime::getSessionTime();
				}

				if (!lateFrame) {

					lockCursor25DList();
					// find the removed cursors first
					for (std::list<TuioCursor25D*>::iterator tcur = cursor25DList.begin(); tcur != cursor25DList.end(); tcur++) {
						if ((*tcur)->getTuioSourceID() == source_id) {
							std::list<long>::iterator iter = find(aliveCursor25DList.begin(), aliveCursor25DList.end(), (*tcur)->getSessionID());

							if (iter == aliveCursor25DList.end()) {
								(*tcur)->remove(currentTime);
								frameCursors25D.push_back(*tcur);
							}
						}
					}
					unlockCursor25DList();

					for (std::list<TuioCursor25D*>::iterator iter = frameCursors25D.begin(); iter != frameCursors25D.end(); iter++) {
						TuioCursor25D *tcur = (*iter);

						int c_id = 0;
						int free_size = 0;
						TuioCursor25D *frameCursor = NULL;
						switch (tcur->getTuioState()) {
						case TUIO_REMOVED:

							frameCursor = tcur;
							frameCursor->remove(currentTime);

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->removeTuioCursor25D(frameCursor);

							lockCursor25DList();
							for (std::list<TuioCursor25D*>::iterator delcur = cursor25DList.begin(); delcur != cursor25DList.end(); delcur++) {
								if (((*delcur)->getTuioSourceID() == source_id) && ((*delcur)->getSessionID() == frameCursor->getSessionID())) {
									cursor25DList.erase(delcur);
									break;
								}
							}

							if (frameCursor->getCursorID() == maxCursor25DID[source_id]) {
								maxCursor25DID[source_id] = -1;
								delete frameCursor;

								if (cursor25DList.size() > 0) {
									std::list<TuioCursor25D*>::iterator clist;
									for (clist = cursor25DList.begin(); clist != cursor25DList.end(); clist++) {
										if ((*clist)->getTuioSourceID() == source_id) {
											c_id = (*clist)->getCursorID();
											if (c_id > maxCursor25DID[source_id]) maxCursor25DID[source_id] = c_id;
										}
									}

									freeCursor25DBuffer.clear();
									for (std::list<TuioCursor25D*>::iterator flist = freeCursor25DList.begin(); flist != freeCursor25DList.end(); flist++) {
										TuioCursor25D *freeCursor = (*flist);
										if (freeCursor->getTuioSourceID() == source_id) {
											if (freeCursor->getCursorID() > maxCursor25DID[source_id]) delete freeCursor;
											else freeCursor25DBuffer.push_back(freeCursor);
										}
										else freeCursor25DBuffer.push_back(freeCursor);
									}
									freeCursor25DList = freeCursor25DBuffer;

								}
								else {
									freeCursor25DBuffer.clear();
									for (std::list<TuioCursor25D*>::iterator flist = freeCursor25DList.begin(); flist != freeCursor25DList.end(); flist++) {
										TuioCursor25D *freeCursor = (*flist);
										if (freeCursor->getTuioSourceID() == source_id) delete freeCursor;
										else freeCursor25DBuffer.push_back(freeCursor);
									}
									freeCursor25DList = freeCursor25DBuffer;

								}
							}
							else if (frameCursor->getCursorID() < maxCursor25DID[source_id]) {
								freeCursor25DList.push_back(frameCursor);
							}

							unlockCursor25DList();
							break;
						case TUIO_ADDED:

							lockCursor25DList();
							for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) c_id++;

							for (std::list<TuioCursor25D*>::iterator iter = freeCursor25DList.begin(); iter != freeCursor25DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) free_size++;

							if ((free_size <= maxCursor25DID[source_id]) && (free_size > 0)) {
								std::list<TuioCursor25D*>::iterator closestCursor = freeCursor25DList.begin();

								for (std::list<TuioCursor25D*>::iterator iter = freeCursor25DList.begin(); iter != freeCursor25DList.end(); iter++) {
									if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getDistance(tcur) < (*closestCursor)->getDistance(tcur))) closestCursor = iter;
								}

								if (closestCursor != freeCursor25DList.end()) {
									TuioCursor25D *freeCursor = (*closestCursor);
									c_id = freeCursor->getCursorID();
									freeCursor25DList.erase(closestCursor);
									delete freeCursor;
								}
							}
							else maxCursor25DID[source_id] = c_id;

							frameCursor = new TuioCursor25D(currentTime, tcur->getSessionID(), c_id, tcur->getX(), tcur->getY(), tcur->getZ());
							if (source_name) frameCursor->setTuioSource(source_id, source_name, source_addr);
							cursor25DList.push_back(frameCursor);

							delete tcur;
							unlockCursor25DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->addTuioCursor25D(frameCursor);

							break;
						default:

							lockCursor25DList();
							std::list<TuioCursor25D*>::iterator iter;
							for (iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
								if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getSessionID() == tcur->getSessionID())) {
									frameCursor = (*iter);
									break;
								}
							}

							if (iter == cursor25DList.end()) {
								unlockCursor25DList();
								break;
							}

							if ((tcur->getX() != frameCursor->getX() && tcur->getXSpeed() == 0) || (tcur->getY() != frameCursor->getY() && tcur->getYSpeed() == 0) || (tcur->getZ() != frameCursor->getZ() && tcur->getZSpeed() == 0))
								frameCursor->update(currentTime, tcur->getX(), tcur->getY(), tcur->getZ());
							else
								frameCursor->update(currentTime, tcur->getX(), tcur->getY(), tcur->getZ(), tcur->getXSpeed(), tcur->getYSpeed(), tcur->getZSpeed(), tcur->getMotionAccel());

							delete tcur;
							unlockCursor25DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->updateTuioCursor25D(frameCursor);

						}
					}

					for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);

				}
				else {
					for (std::list<TuioCursor25D*>::iterator iter = frameCursors25D.begin(); iter != frameCursors25D.end(); iter++) {
						TuioCursor25D *tcur = (*iter);
						delete tcur;
					}
				}

				frameCursors25D.clear();
			}
		}
		else if (strcmp(msg.AddressPattern(), "/tuio/25Dblb") == 0) {
			const char* cmd;
			args >> cmd;

			if (strcmp(cmd, "source") == 0) {
				const char* src;
				args >> src;

				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");

				if (addr != NULL) source_addr = addr;
				else source_addr = (char*)"localhost";

				// check if we know that source
				std::string source_str(src);
				std::map<std::string, int>::iterator iter = sourceList.find(source_str);

				// add a new source
				if (iter == sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
					maxBlob25DID[source_id] = -1;
				}
				else {
					// use the found source_id
					source_id = sourceList[source_str];
				}

			}
			else if (strcmp(cmd, "set") == 0) {

				int32 s_id;
				float xpos, ypos,zpos, angle, width, height, area, xspeed, yspeed,zspeed, rspeed, maccel, raccel;
				args >> s_id >> xpos >> ypos >> zpos >> angle >> width >> height >> area >> xspeed >> yspeed >> zspeed >> rspeed >> maccel >> raccel;

				lockBlob25DList();
				std::list<TuioBlob25D*>::iterator tblb;
				for (tblb = blob25DList.begin(); tblb != blob25DList.end(); tblb++)
					if ((*tblb)->getSessionID() == (long)s_id) break;

				if (tblb == blob25DList.end()) {

					TuioBlob25D *addBlob = new TuioBlob25D((long)s_id, -1, xpos, ypos,zpos, angle, width, height, area);
					frameBlobs25D.push_back(addBlob);

				}
				else if (((*tblb)->getX() != xpos) || ((*tblb)->getY() != ypos) || ((*tblb)->getZ() != zpos) || ((*tblb)->getAngle() != angle) || ((*tblb)->getWidth() != width) || ((*tblb)->getHeight() != height) || ((*tblb)->getArea() != area) || ((*tblb)->getXSpeed() != xspeed) || ((*tblb)->getYSpeed() != yspeed) || ((*tblb)->getZSpeed() != zspeed) || ((*tblb)->getRotationSpeed() != rspeed) || ((*tblb)->getMotionAccel() != maccel)) {

					TuioBlob25D *updateBlob = new TuioBlob25D((long)s_id, (*tblb)->getBlobID(), xpos, ypos,zpos, angle, width, height, area);
					updateBlob->update(xpos, ypos,zpos, angle, width, height, area, xspeed, yspeed,zspeed, rspeed, maccel, raccel);
					frameBlobs25D.push_back(updateBlob);
				}
				unlockBlob25DList();

			}
			else if (strcmp(cmd, "alive") == 0) {

				int32 s_id;
				aliveBlob25DList.clear();
				while (!args.Eos()) {
					args >> s_id;
					aliveBlob25DList.push_back((long)s_id);
				}

			}
			else if (strcmp(cmd, "fseq") == 0) {

				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq > 0) {
					if (fseq > currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq >= currentFrame) || ((currentFrame - fseq) > 100)) currentFrame = fseq;
					else lateFrame = true;
				}
				else if ((TuioTime::getSessionTime().getTotalMilliseconds() - currentTime.getTotalMilliseconds()) > 100) {
					currentTime = TuioTime::getSessionTime();
				}

				if (!lateFrame) {

					lockBlob25DList();
					// find the removed blobs first
					for (std::list<TuioBlob25D*>::iterator tblb = blob25DList.begin(); tblb != blob25DList.end(); tblb++) {
						if ((*tblb)->getTuioSourceID() == source_id) {
							std::list<long>::iterator iter = find(aliveBlob25DList.begin(), aliveBlob25DList.end(), (*tblb)->getSessionID());

							if (iter == aliveBlob25DList.end()) {
								(*tblb)->remove(currentTime);
								frameBlobs25D.push_back(*tblb);
							}
						}
					}
					unlockBlob25DList();

					for (std::list<TuioBlob25D*>::iterator iter = frameBlobs25D.begin(); iter != frameBlobs25D.end(); iter++) {
						TuioBlob25D *tblb = (*iter);

						int b_id = 0;
						int free_size = 0;
						TuioBlob25D *frameBlob = NULL;
						switch (tblb->getTuioState()) {
						case TUIO_REMOVED:
							frameBlob = tblb;
							frameBlob->remove(currentTime);

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->removeTuioBlob25D(frameBlob);

							lockBlob25DList();
							for (std::list<TuioBlob25D*>::iterator delblb = blob25DList.begin(); delblb != blob25DList.end(); delblb++) {
								if (((*delblb)->getTuioSourceID() == source_id) && ((*delblb)->getSessionID() == frameBlob->getSessionID())) {
									blob25DList.erase(delblb);
									break;
								}
							}

							if (frameBlob->getBlobID() == maxBlob25DID[source_id]) {
								maxBlob25DID[source_id] = -1;
								delete frameBlob;

								if (blob25DList.size() > 0) {
									std::list<TuioBlob25D*>::iterator clist;
									for (clist = blob25DList.begin(); clist != blob25DList.end(); clist++) {
										if ((*clist)->getTuioSourceID() == source_id) {
											b_id = (*clist)->getBlobID();
											if (b_id > maxBlob25DID[source_id]) maxBlob25DID[source_id] = b_id;
										}
									}

									freeBlob25DBuffer.clear();
									for (std::list<TuioBlob25D*>::iterator flist = freeBlob25DList.begin(); flist != freeBlob25DList.end(); flist++) {
										TuioBlob25D *freeBlob = (*flist);
										if (freeBlob->getTuioSourceID() == source_id) {
											if (freeBlob->getBlobID() > maxBlob25DID[source_id]) delete freeBlob;
											else freeBlob25DBuffer.push_back(freeBlob);
										}
										else freeBlob25DBuffer.push_back(freeBlob);
									}
									freeBlob25DList = freeBlob25DBuffer;

								}
								else {
									freeBlob25DBuffer.clear();
									for (std::list<TuioBlob25D*>::iterator flist = freeBlob25DList.begin(); flist != freeBlob25DList.end(); flist++) {
										TuioBlob25D *freeBlob = (*flist);
										if (freeBlob->getTuioSourceID() == source_id) delete freeBlob;
										else freeBlob25DBuffer.push_back(freeBlob);
									}
									freeBlob25DList = freeBlob25DBuffer;

								}
							}
							else if (frameBlob->getBlobID() < maxBlob25DID[source_id]) {
								freeBlob25DList.push_back(frameBlob);
							}

							unlockBlob25DList();
							break;
						case TUIO_ADDED:

							lockBlob25DList();
							for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) b_id++;

							for (std::list<TuioBlob25D*>::iterator iter = freeBlob25DList.begin(); iter != freeBlob25DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) free_size++;

							if ((free_size <= maxBlob25DID[source_id]) && (free_size > 0)) {
								std::list<TuioBlob25D*>::iterator closestBlob = freeBlob25DList.begin();

								for (std::list<TuioBlob25D*>::iterator iter = freeBlob25DList.begin(); iter != freeBlob25DList.end(); iter++) {
									if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getDistance(tblb) < (*closestBlob)->getDistance(tblb))) closestBlob = iter;
								}

								if (closestBlob != freeBlob25DList.end()) {
									TuioBlob25D *freeBlob = (*closestBlob);
									b_id = freeBlob->getBlobID();
									freeBlob25DList.erase(closestBlob);
									delete freeBlob;
								}
							}
							else maxBlob25DID[source_id] = b_id;

							frameBlob = new TuioBlob25D(currentTime, tblb->getSessionID(), b_id, tblb->getX(), tblb->getY(), tblb->getZ(), tblb->getAngle(), tblb->getWidth(), tblb->getHeight(), tblb->getArea());
							if (source_name) frameBlob->setTuioSource(source_id, source_name, source_addr);
							blob25DList.push_back(frameBlob);

							delete tblb;
							unlockBlob25DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->addTuioBlob25D(frameBlob);

							break;
						default:

							lockBlob25DList();
							std::list<TuioBlob25D*>::iterator iter;
							for (iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
								if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getSessionID() == tblb->getSessionID())) {
									frameBlob = (*iter);
									break;
								}
							}

							if (iter == blob25DList.end()) {
								unlockBlob25DList();
								break;
							}

							if ((tblb->getX() != frameBlob->getX() && tblb->getXSpeed() == 0) || (tblb->getY() != frameBlob->getY() && tblb->getYSpeed() == 0) || (tblb->getZ() != frameBlob->getZ() && tblb->getZSpeed() == 0) || (tblb->getAngle() != frameBlob->getAngle() && tblb->getRotationSpeed() == 0))
								frameBlob->update(currentTime, tblb->getX(), tblb->getY(), tblb->getZ(), tblb->getAngle(), tblb->getWidth(), tblb->getHeight(), tblb->getArea());
							else
								frameBlob->update(currentTime, tblb->getX(), tblb->getY(), tblb->getZ(), tblb->getAngle(), tblb->getWidth(), tblb->getHeight(), tblb->getArea(), tblb->getXSpeed(), tblb->getYSpeed(), tblb->getZSpeed(), tblb->getRotationSpeed(), tblb->getMotionAccel(), tblb->getRotationAccel());

							delete tblb;
							unlockBlob25DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->updateTuioBlob25D(frameBlob);
						}
					}

					for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);

				}
				else {
					for (std::list<TuioBlob25D*>::iterator iter = frameBlobs25D.begin(); iter != frameBlobs25D.end(); iter++) {
						TuioBlob25D *tblb = (*iter);
						delete tblb;
					}
				}

				frameBlobs25D.clear();
			}
		}
		else

		//3D
		if (strcmp(msg.AddressPattern(), "/tuio/3Dobj") == 0) {

			const char* cmd;
			args >> cmd;

			if (strcmp(cmd, "source") == 0) {
				const char* src;
				args >> src;

				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");

				if (addr != NULL) source_addr = addr;
				else source_addr = (char*)"localhost";

				// check if we know that source
				std::string source_str(src);
				std::map<std::string, int>::iterator iter = sourceList.find(source_str);

				// add a new source
				if (iter == sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
				}
				else {
					// use the found source_id
					source_id = sourceList[source_str];
				}

			}
			else if (strcmp(cmd, "set") == 0) {
				int32 s_id, c_id;
				float xpos, ypos, zpos, roll, pitch, yaw, xspeed, yspeed, zspeed, rollspeed, pitchspeed, yawspeed, maccel, raccel;
				args >> s_id >> c_id >> xpos >> ypos >> zpos >> roll >> pitch >> yaw >> xspeed >> yspeed >> zspeed >> rollspeed >> pitchspeed >> yawspeed >> maccel >> raccel;

				lockObject3DList();
				std::list<TuioObject3D*>::iterator tobj;
				for (tobj = object3DList.begin(); tobj != object3DList.end(); tobj++)
					if ((*tobj)->getSessionID() == (long)s_id) break;

				if (tobj == object3DList.end()) {

					TuioObject3D *addObject = new TuioObject3D((long)s_id, (int)c_id, xpos, ypos, zpos, roll, pitch, yaw);
					frameObjects3D.push_back(addObject);

				}
				else if (((*tobj)->getX() != xpos) || ((*tobj)->getY() != ypos) || ((*tobj)->getZ() != zpos) || ((*tobj)->getRoll() != roll) || ((*tobj)->getPitch() != pitch) || ((*tobj)->getYaw() != yaw) || ((*tobj)->getXSpeed() != xspeed) || ((*tobj)->getYSpeed() != yspeed) || ((*tobj)->getZSpeed() != zspeed) || ((*tobj)->getRollSpeed() != rollspeed) || ((*tobj)->getPitchSpeed() != pitchspeed) || ((*tobj)->getYawSpeed() != yawspeed) || ((*tobj)->getMotionAccel() != maccel) || ((*tobj)->getRotationAccel() != raccel)) {

					TuioObject3D *updateObject = new TuioObject3D((long)s_id, (*tobj)->getSymbolID(), xpos, ypos, zpos, roll, pitch, yaw);
					updateObject->update(xpos, ypos, zpos, roll, pitch, yaw, xspeed, yspeed, zspeed, rollspeed, pitchspeed, yawspeed, maccel, raccel);
					frameObjects3D.push_back(updateObject);

				}
				unlockObject3DList();

			}
			else if (strcmp(cmd, "alive") == 0) {

				int32 s_id;
				aliveObject3DList.clear();
				while (!args.Eos()) {
					args >> s_id;
					aliveObject3DList.push_back((long)s_id);
				}

			}
			else if (strcmp(cmd, "fseq") == 0) {

				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq > 0) {
					if (fseq > currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq >= currentFrame) || ((currentFrame - fseq) > 100)) currentFrame = fseq;
					else lateFrame = true;
				}
				else if ((TuioTime::getSessionTime().getTotalMilliseconds() - currentTime.getTotalMilliseconds()) > 100) {
					currentTime = TuioTime::getSessionTime();
				}

				if (!lateFrame) {

					lockObject3DList();
					//find the removed objects first
					for (std::list<TuioObject3D*>::iterator tobj = object3DList.begin(); tobj != object3DList.end(); tobj++) {
						if ((*tobj)->getTuioSourceID() == source_id) {
							std::list<long>::iterator iter = find(aliveObject3DList.begin(), aliveObject3DList.end(), (*tobj)->getSessionID());
							if (iter == aliveObject3DList.end()) {
								(*tobj)->remove(currentTime);
								frameObjects3D.push_back(*tobj);
							}
						}
					}
					unlockObject3DList();

					for (std::list<TuioObject3D*>::iterator iter = frameObjects3D.begin(); iter != frameObjects3D.end(); iter++) {
						TuioObject3D *tobj = (*iter);

						TuioObject3D *frameObject = NULL;
						switch (tobj->getTuioState()) {
						case TUIO_REMOVED:

							frameObject = tobj;
							frameObject->remove(currentTime);

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->removeTuioObject3D(frameObject);

							lockObject3DList();
							for (std::list<TuioObject3D*>::iterator delobj = object3DList.begin(); delobj != object3DList.end(); delobj++) {
								if ((*delobj)->getSessionID() == frameObject->getSessionID()) {
									object3DList.erase(delobj);
									break;
								}
							}
							unlockObject3DList();
							break;
						case TUIO_ADDED:

							lockObject3DList();
							frameObject = new TuioObject3D(currentTime, tobj->getSessionID(), tobj->getSymbolID(), tobj->getX(), tobj->getY(), tobj->getZ(), tobj->getRoll(), tobj->getPitch(), tobj->getYaw());
							if (source_name) frameObject->setTuioSource(source_id, source_name, source_addr);
							object3DList.push_back(frameObject);
							unlockObject3DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->addTuioObject3D(frameObject);

							break;
						default:

							lockObject3DList();
							std::list<TuioObject3D*>::iterator iter;
							for (iter = object3DList.begin(); iter != object3DList.end(); iter++) {
								if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getSessionID() == tobj->getSessionID())) {
									frameObject = (*iter);
									break;
								}
							}

							if (iter == object3DList.end()) {
								unlockObject3DList();
								break;
							}

							if ((tobj->getX() != frameObject->getX() && tobj->getXSpeed() == 0) || (tobj->getY() != frameObject->getY() && tobj->getYSpeed() == 0) || (tobj->getZ() != frameObject->getZ() && tobj->getZSpeed() == 0) || (tobj->getRoll() != frameObject->getRoll() && tobj->getRollSpeed() == 0) || (tobj->getPitch() != frameObject->getPitch() && tobj->getPitchSpeed() == 0) || (tobj->getYaw() != frameObject->getYaw() && tobj->getYawSpeed() == 0))
								frameObject->update(currentTime, tobj->getX(), tobj->getY(), tobj->getZ(), tobj->getRoll(), tobj->getPitch(), tobj->getYaw());
							else
								frameObject->update(currentTime, tobj->getX(), tobj->getY(), tobj->getZ(), tobj->getRoll(), tobj->getPitch(), tobj->getYaw(), tobj->getXSpeed(), tobj->getYSpeed(), tobj->getZSpeed(), tobj->getRollSpeed(), tobj->getPitchSpeed(), tobj->getYawSpeed(), tobj->getMotionAccel(), tobj->getRotationAccel());

							unlockObject3DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->updateTuioObject3D(frameObject);
						}
						delete tobj;
					}

					for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);

				}
				else {
					for (std::list<TuioObject3D*>::iterator iter = frameObjects3D.begin(); iter != frameObjects3D.end(); iter++) {
						TuioObject3D *tobj = (*iter);
						delete tobj;
					}
				}

				frameObjects3D.clear();
			}
		}
		else if (strcmp(msg.AddressPattern(), "/tuio/3Dcur") == 0) {
			const char* cmd;
			args >> cmd;

			if (strcmp(cmd, "source") == 0) {
				const char* src;
				args >> src;

				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");

				if (addr != NULL) source_addr = addr;
				else source_addr = (char*)"localhost";

				// check if we know that source
				std::string source_str(src);
				std::map<std::string, int>::iterator iter = sourceList.find(source_str);

				// add a new source
				if (iter == sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
					maxCursor3DID[source_id] = -1;
				}
				else {
					// use the found source_id
					source_id = sourceList[source_str];
				}

			}
			else if (strcmp(cmd, "set") == 0) {

				int32 s_id;
				float xpos, ypos, zpos, xspeed, yspeed, zspeed, maccel;
				args >> s_id >> xpos >> ypos >> zpos >> xspeed >> yspeed >> zspeed >> maccel;

				lockCursor3DList();
				std::list<TuioCursor3D*>::iterator tcur;
				for (tcur = cursor3DList.begin(); tcur != cursor3DList.end(); tcur++)
					if (((*tcur)->getSessionID() == (long)s_id) && ((*tcur)->getTuioSourceID() == source_id)) break;

				if (tcur == cursor3DList.end()) {

					TuioCursor3D *addCursor = new TuioCursor3D((long)s_id, -1, xpos, ypos, zpos);
					frameCursors3D.push_back(addCursor);

				}
				else if (((*tcur)->getX() != xpos) || ((*tcur)->getY() != ypos) || ((*tcur)->getZ() != zpos) || ((*tcur)->getXSpeed() != xspeed) || ((*tcur)->getYSpeed() != yspeed) || ((*tcur)->getZSpeed() != zspeed) || ((*tcur)->getMotionAccel() != maccel)) {

					TuioCursor3D *updateCursor = new TuioCursor3D((long)s_id, (*tcur)->getCursorID(), xpos, ypos, zpos);
					updateCursor->update(xpos, ypos, zpos, xspeed, yspeed, zpos, maccel);
					frameCursors3D.push_back(updateCursor);

				}
				unlockCursor3DList();

			}
			else if (strcmp(cmd, "alive") == 0) {

				int32 s_id;
				aliveCursor3DList.clear();
				while (!args.Eos()) {
					args >> s_id;
					aliveCursor3DList.push_back((long)s_id);
				}

			}
			else if (strcmp(cmd, "fseq") == 0) {
				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq > 0) {
					if (fseq > currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq >= currentFrame) || ((currentFrame - fseq) > 100)) currentFrame = fseq;
					else lateFrame = true;
				}
				else if ((TuioTime::getSessionTime().getTotalMilliseconds() - currentTime.getTotalMilliseconds()) > 100) {
					currentTime = TuioTime::getSessionTime();
				}

				if (!lateFrame) {

					lockCursor3DList();
					// find the removed cursors first
					for (std::list<TuioCursor3D*>::iterator tcur = cursor3DList.begin(); tcur != cursor3DList.end(); tcur++) {
						if ((*tcur)->getTuioSourceID() == source_id) {
							std::list<long>::iterator iter = find(aliveCursor3DList.begin(), aliveCursor3DList.end(), (*tcur)->getSessionID());

							if (iter == aliveCursor3DList.end()) {
								(*tcur)->remove(currentTime);
								frameCursors3D.push_back(*tcur);
							}
						}
					}
					unlockCursor3DList();

					for (std::list<TuioCursor3D*>::iterator iter = frameCursors3D.begin(); iter != frameCursors3D.end(); iter++) {
						TuioCursor3D *tcur = (*iter);

						int c_id = 0;
						int free_size = 0;
						TuioCursor3D *frameCursor = NULL;
						switch (tcur->getTuioState()) {
						case TUIO_REMOVED:

							frameCursor = tcur;
							frameCursor->remove(currentTime);

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->removeTuioCursor3D(frameCursor);

							lockCursor3DList();
							for (std::list<TuioCursor3D*>::iterator delcur = cursor3DList.begin(); delcur != cursor3DList.end(); delcur++) {
								if (((*delcur)->getTuioSourceID() == source_id) && ((*delcur)->getSessionID() == frameCursor->getSessionID())) {
									cursor3DList.erase(delcur);
									break;
								}
							}

							if (frameCursor->getCursorID() == maxCursor3DID[source_id]) {
								maxCursor3DID[source_id] = -1;
								delete frameCursor;

								if (cursor3DList.size() > 0) {
									std::list<TuioCursor3D*>::iterator clist;
									for (clist = cursor3DList.begin(); clist != cursor3DList.end(); clist++) {
										if ((*clist)->getTuioSourceID() == source_id) {
											c_id = (*clist)->getCursorID();
											if (c_id > maxCursor3DID[source_id]) maxCursor3DID[source_id] = c_id;
										}
									}

									freeCursor3DBuffer.clear();
									for (std::list<TuioCursor3D*>::iterator flist = freeCursor3DList.begin(); flist != freeCursor3DList.end(); flist++) {
										TuioCursor3D *freeCursor = (*flist);
										if (freeCursor->getTuioSourceID() == source_id) {
											if (freeCursor->getCursorID() > maxCursor3DID[source_id]) delete freeCursor;
											else freeCursor3DBuffer.push_back(freeCursor);
										}
										else freeCursor3DBuffer.push_back(freeCursor);
									}
									freeCursor3DList = freeCursor3DBuffer;

								}
								else {
									freeCursor3DBuffer.clear();
									for (std::list<TuioCursor3D*>::iterator flist = freeCursor3DList.begin(); flist != freeCursor3DList.end(); flist++) {
										TuioCursor3D *freeCursor = (*flist);
										if (freeCursor->getTuioSourceID() == source_id) delete freeCursor;
										else freeCursor3DBuffer.push_back(freeCursor);
									}
									freeCursor3DList = freeCursor3DBuffer;

								}
							}
							else if (frameCursor->getCursorID() < maxCursor3DID[source_id]) {
								freeCursor3DList.push_back(frameCursor);
							}

							unlockCursor3DList();
							break;
						case TUIO_ADDED:

							lockCursor3DList();
							for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) c_id++;

							for (std::list<TuioCursor3D*>::iterator iter = freeCursor3DList.begin(); iter != freeCursor3DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) free_size++;

							if ((free_size <= maxCursor3DID[source_id]) && (free_size > 0)) {
								std::list<TuioCursor3D*>::iterator closestCursor = freeCursor3DList.begin();

								for (std::list<TuioCursor3D*>::iterator iter = freeCursor3DList.begin(); iter != freeCursor3DList.end(); iter++) {
									if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getDistance(tcur) < (*closestCursor)->getDistance(tcur))) closestCursor = iter;
								}

								if (closestCursor != freeCursor3DList.end()) {
									TuioCursor3D *freeCursor = (*closestCursor);
									c_id = freeCursor->getCursorID();
									freeCursor3DList.erase(closestCursor);
									delete freeCursor;
								}
							}
							else maxCursor3DID[source_id] = c_id;

							frameCursor = new TuioCursor3D(currentTime, tcur->getSessionID(), c_id, tcur->getX(), tcur->getY(), tcur->getZ());
							if (source_name) frameCursor->setTuioSource(source_id, source_name, source_addr);
							cursor3DList.push_back(frameCursor);

							delete tcur;
							unlockCursor3DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->addTuioCursor3D(frameCursor);

							break;
						default:

							lockCursor3DList();
							std::list<TuioCursor3D*>::iterator iter;
							for (iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
								if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getSessionID() == tcur->getSessionID())) {
									frameCursor = (*iter);
									break;
								}
							}

							if (iter == cursor3DList.end()) {
								unlockCursor3DList();
								break;
							}

							if ((tcur->getX() != frameCursor->getX() && tcur->getXSpeed() == 0) || (tcur->getY() != frameCursor->getY() && tcur->getYSpeed() == 0) || (tcur->getZ() != frameCursor->getZ() && tcur->getZSpeed() == 0))
								frameCursor->update(currentTime, tcur->getX(), tcur->getY(), tcur->getZ());
							else
								frameCursor->update(currentTime, tcur->getX(), tcur->getY(), tcur->getZ(), tcur->getXSpeed(), tcur->getYSpeed(), tcur->getZSpeed(), tcur->getMotionAccel());

							delete tcur;
							unlockCursor3DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->updateTuioCursor3D(frameCursor);

						}
					}

					for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);

				}
				else {
					for (std::list<TuioCursor3D*>::iterator iter = frameCursors3D.begin(); iter != frameCursors3D.end(); iter++) {
						TuioCursor3D *tcur = (*iter);
						delete tcur;
					}
				}

				frameCursors3D.clear();
			}
		}
		else if (strcmp(msg.AddressPattern(), "/tuio/3Dblb") == 0) {
			const char* cmd;
			args >> cmd;

			if (strcmp(cmd, "source") == 0) {
				const char* src;
				args >> src;

				source_name = strtok((char*)src, "@");
				char *addr = strtok(NULL, "@");

				if (addr != NULL) source_addr = addr;
				else source_addr = (char*)"localhost";

				// check if we know that source
				std::string source_str(src);
				std::map<std::string, int>::iterator iter = sourceList.find(source_str);

				// add a new source
				if (iter == sourceList.end()) {
					source_id = (int)sourceList.size();
					sourceList[source_str] = source_id;
					maxBlob3DID[source_id] = -1;
				}
				else {
					// use the found source_id
					source_id = sourceList[source_str];
				}

			}
			else if (strcmp(cmd, "set") == 0) {

				int32 s_id;
				float xpos, ypos, zpos, roll, pitch, yaw, width, height, depth, volume, xspeed, yspeed, zspeed, rollspeed, pitchspeed, yawspeed, maccel, raccel;
				args >> s_id >> xpos >> ypos >> zpos >> roll >> pitch >> yaw >> width >> height >> depth >> volume >> xspeed >> yspeed >> zspeed >> rollspeed >> pitchspeed >> yawspeed >> maccel >> raccel;

				lockBlob3DList();
				std::list<TuioBlob3D*>::iterator tblb;
				for (tblb = blob3DList.begin(); tblb != blob3DList.end(); tblb++)
					if ((*tblb)->getSessionID() == (long)s_id) break;

				if (tblb == blob3DList.end()) {

					TuioBlob3D *addBlob = new TuioBlob3D((long)s_id, -1, xpos, ypos, zpos, roll, pitch, yaw, width, height, depth, volume);
					frameBlobs3D.push_back(addBlob);

				}
				else if (((*tblb)->getX() != xpos) || ((*tblb)->getY() != ypos) || ((*tblb)->getZ() != zpos) || ((*tblb)->getRoll() != roll) || ((*tblb)->getPitch() != pitch) || ((*tblb)->getYaw() != yaw) || ((*tblb)->getWidth() != width) || ((*tblb)->getHeight() != height) || ((*tblb)->getDepth() != depth) || ((*tblb)->getVolume() != volume) || ((*tblb)->getXSpeed() != xspeed) || ((*tblb)->getYSpeed() != yspeed) || ((*tblb)->getZSpeed() != zspeed) || ((*tblb)->getRollSpeed() != rollspeed) || ((*tblb)->getPitchSpeed() != pitchspeed) || ((*tblb)->getYawSpeed() != yaw) || ((*tblb)->getMotionAccel() != maccel)) {

					TuioBlob3D *updateBlob = new TuioBlob3D((long)s_id, (*tblb)->getBlobID(), xpos, ypos, zpos, roll, pitch, yaw, width, height, depth, volume);
					updateBlob->update(xpos, ypos, zpos, roll, pitch, yaw, width, height, depth, volume, xspeed, yspeed, zspeed, rollspeed, pitchspeed, yawspeed, maccel, raccel);
					frameBlobs3D.push_back(updateBlob);
				}
				unlockBlob3DList();

			}
			else if (strcmp(cmd, "alive") == 0) {

				int32 s_id;
				aliveBlob3DList.clear();
				while (!args.Eos()) {
					args >> s_id;
					aliveBlob3DList.push_back((long)s_id);
				}

			}
			else if (strcmp(cmd, "fseq") == 0) {

				int32 fseq;
				args >> fseq;
				bool lateFrame = false;
				if (fseq > 0) {
					if (fseq > currentFrame) currentTime = TuioTime::getSessionTime();
					if ((fseq >= currentFrame) || ((currentFrame - fseq) > 100)) currentFrame = fseq;
					else lateFrame = true;
				}
				else if ((TuioTime::getSessionTime().getTotalMilliseconds() - currentTime.getTotalMilliseconds()) > 100) {
					currentTime = TuioTime::getSessionTime();
				}

				if (!lateFrame) {

					lockBlob3DList();
					// find the removed blobs first
					for (std::list<TuioBlob3D*>::iterator tblb = blob3DList.begin(); tblb != blob3DList.end(); tblb++) {
						if ((*tblb)->getTuioSourceID() == source_id) {
							std::list<long>::iterator iter = find(aliveBlob3DList.begin(), aliveBlob3DList.end(), (*tblb)->getSessionID());

							if (iter == aliveBlob3DList.end()) {
								(*tblb)->remove(currentTime);
								frameBlobs3D.push_back(*tblb);
							}
						}
					}
					unlockBlob3DList();

					for (std::list<TuioBlob3D*>::iterator iter = frameBlobs3D.begin(); iter != frameBlobs3D.end(); iter++) {
						TuioBlob3D *tblb = (*iter);

						int b_id = 0;
						int free_size = 0;
						TuioBlob3D *frameBlob = NULL;
						switch (tblb->getTuioState()) {
						case TUIO_REMOVED:
							frameBlob = tblb;
							frameBlob->remove(currentTime);

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->removeTuioBlob3D(frameBlob);

							lockBlob3DList();
							for (std::list<TuioBlob3D*>::iterator delblb = blob3DList.begin(); delblb != blob3DList.end(); delblb++) {
								if (((*delblb)->getTuioSourceID() == source_id) && ((*delblb)->getSessionID() == frameBlob->getSessionID())) {
									blob3DList.erase(delblb);
									break;
								}
							}

							if (frameBlob->getBlobID() == maxBlob3DID[source_id]) {
								maxBlob3DID[source_id] = -1;
								delete frameBlob;

								if (blob3DList.size() > 0) {
									std::list<TuioBlob3D*>::iterator clist;
									for (clist = blob3DList.begin(); clist != blob3DList.end(); clist++) {
										if ((*clist)->getTuioSourceID() == source_id) {
											b_id = (*clist)->getBlobID();
											if (b_id > maxBlob3DID[source_id]) maxBlob3DID[source_id] = b_id;
										}
									}

									freeBlob3DBuffer.clear();
									for (std::list<TuioBlob3D*>::iterator flist = freeBlob3DList.begin(); flist != freeBlob3DList.end(); flist++) {
										TuioBlob3D *freeBlob = (*flist);
										if (freeBlob->getTuioSourceID() == source_id) {
											if (freeBlob->getBlobID() > maxBlob3DID[source_id]) delete freeBlob;
											else freeBlob3DBuffer.push_back(freeBlob);
										}
										else freeBlob3DBuffer.push_back(freeBlob);
									}
									freeBlob3DList = freeBlob3DBuffer;

								}
								else {
									freeBlob3DBuffer.clear();
									for (std::list<TuioBlob3D*>::iterator flist = freeBlob3DList.begin(); flist != freeBlob3DList.end(); flist++) {
										TuioBlob3D *freeBlob = (*flist);
										if (freeBlob->getTuioSourceID() == source_id) delete freeBlob;
										else freeBlob3DBuffer.push_back(freeBlob);
									}
									freeBlob3DList = freeBlob3DBuffer;

								}
							}
							else if (frameBlob->getBlobID() < maxBlob3DID[source_id]) {
								freeBlob3DList.push_back(frameBlob);
							}

							unlockBlob3DList();
							break;
						case TUIO_ADDED:

							lockBlob3DList();
							for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) b_id++;

							for (std::list<TuioBlob3D*>::iterator iter = freeBlob3DList.begin(); iter != freeBlob3DList.end(); iter++)
								if ((*iter)->getTuioSourceID() == source_id) free_size++;

							if ((free_size <= maxBlob3DID[source_id]) && (free_size > 0)) {
								std::list<TuioBlob3D*>::iterator closestBlob = freeBlob3DList.begin();

								for (std::list<TuioBlob3D*>::iterator iter = freeBlob3DList.begin(); iter != freeBlob3DList.end(); iter++) {
									if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getDistance(tblb) < (*closestBlob)->getDistance(tblb))) closestBlob = iter;
								}

								if (closestBlob != freeBlob3DList.end()) {
									TuioBlob3D *freeBlob = (*closestBlob);
									b_id = freeBlob->getBlobID();
									freeBlob3DList.erase(closestBlob);
									delete freeBlob;
								}
							}
							else maxBlob3DID[source_id] = b_id;

							frameBlob = new TuioBlob3D(currentTime, tblb->getSessionID(), b_id, tblb->getX(), tblb->getY(), tblb->getZ(), tblb->getRoll(), tblb->getPitch(), tblb->getYaw(), tblb->getWidth(), tblb->getHeight(), tblb->getDepth(), tblb->getVolume());
							if (source_name) frameBlob->setTuioSource(source_id, source_name, source_addr);
							blob3DList.push_back(frameBlob);

							delete tblb;
							unlockBlob3DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->addTuioBlob3D(frameBlob);

							break;
						default:

							lockBlob3DList();
							std::list<TuioBlob3D*>::iterator iter;
							for (iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
								if (((*iter)->getTuioSourceID() == source_id) && ((*iter)->getSessionID() == tblb->getSessionID())) {
									frameBlob = (*iter);
									break;
								}
							}

							if (iter == blob3DList.end()) {
								unlockBlob3DList();
								break;
							}

							if ((tblb->getX() != frameBlob->getX() && tblb->getXSpeed() == 0) || (tblb->getY() != frameBlob->getY() && tblb->getYSpeed() == 0) || (tblb->getZ() != frameBlob->getZ() && tblb->getZSpeed() == 0) || (tblb->getRoll() != frameBlob->getRoll() && tblb->getRollSpeed() == 0) || (tblb->getPitch() != frameBlob->getPitch() && tblb->getPitchSpeed() == 0) || (tblb->getYaw() != frameBlob->getYaw() && tblb->getYawSpeed() == 0))
								frameBlob->update(currentTime, tblb->getX(), tblb->getY(), tblb->getZ(), tblb->getRoll(), tblb->getPitch(), tblb->getYaw(), tblb->getWidth(), tblb->getHeight(), tblb->getDepth(), tblb->getVolume());
							else
								frameBlob->update(currentTime, tblb->getX(), tblb->getY(), tblb->getZ(), tblb->getRoll(), tblb->getPitch(), tblb->getYaw(), tblb->getWidth(), tblb->getHeight(), tblb->getDepth(), tblb->getVolume(), tblb->getXSpeed(), tblb->getYSpeed(), tblb->getZSpeed(), tblb->getRollSpeed(), tblb->getPitchSpeed(), tblb->getYawSpeed(), tblb->getMotionAccel(), tblb->getRotationAccel());

							delete tblb;
							unlockBlob3DList();

							for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
								(*listener)->updateTuioBlob3D(frameBlob);
						}
					}

					for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
						(*listener)->refresh(currentTime);

				}
				else {
					for (std::list<TuioBlob3D*>::iterator iter = frameBlobs3D.begin(); iter != frameBlobs3D.end(); iter++) {
						TuioBlob3D *tblb = (*iter);
						delete tblb;
					}
				}

				frameBlobs3D.clear();
			}
		}



	} catch( Exception& e ){
		std::cerr << "error parsing TUIO message: "<< msg.AddressPattern() <<  " - " << e.what() << std::endl;
	}
}

bool TuioClient::isConnected() {	
	return receiver->isConnected();
}

void TuioClient::connect(bool lock) {
			
	TuioTime::initSession();
	currentTime.reset();
	
	receiver->connect(lock);
	
	unlockCursorList();
	unlockObjectList();
	unlockBlobList();

	unlockCursor25DList();
	unlockObject25DList();
	unlockBlob25DList();

	unlockCursor3DList();
	unlockObject3DList();
	unlockBlob3DList();


}

void TuioClient::disconnect() {
	
	receiver->disconnect();
	
	aliveObjectList.clear();
	aliveCursorList.clear();
	aliveBlobList.clear();

	for (std::list<TuioObject*>::iterator iter=objectList.begin(); iter != objectList.end(); iter++)
		delete (*iter);
	objectList.clear();

	for (std::list<TuioCursor*>::iterator iter=cursorList.begin(); iter != cursorList.end(); iter++)
		delete (*iter);
	cursorList.clear();

	for (std::list<TuioBlob*>::iterator iter=blobList.begin(); iter != blobList.end(); iter++)
		delete (*iter);
	blobList.clear();
	
	for (std::list<TuioCursor*>::iterator iter=freeCursorList.begin(); iter != freeCursorList.end(); iter++)
		delete(*iter);
	freeCursorList.clear();

	for (std::list<TuioBlob*>::iterator iter=freeBlobList.begin(); iter != freeBlobList.end(); iter++)
		delete(*iter);
	freeBlobList.clear();





	aliveObject25DList.clear();
	aliveCursor25DList.clear();
	aliveBlob25DList.clear();

	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++)
		delete (*iter);
	object25DList.clear();

	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++)
		delete (*iter);
	cursor25DList.clear();

	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++)
		delete (*iter);
	blob25DList.clear();

	for (std::list<TuioCursor25D*>::iterator iter = freeCursor25DList.begin(); iter != freeCursor25DList.end(); iter++)
		delete(*iter);
	freeCursor25DList.clear();

	for (std::list<TuioBlob25D*>::iterator iter = freeBlob25DList.begin(); iter != freeBlob25DList.end(); iter++)
		delete(*iter);
	freeBlob25DList.clear();




	aliveObject3DList.clear();
	aliveCursor3DList.clear();
	aliveBlob3DList.clear();

	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++)
		delete (*iter);
	object3DList.clear();

	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++)
		delete (*iter);
	cursor3DList.clear();

	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++)
		delete (*iter);
	blob3DList.clear();

	for (std::list<TuioCursor3D*>::iterator iter = freeCursor3DList.begin(); iter != freeCursor3DList.end(); iter++)
		delete(*iter);
	freeCursor3DList.clear();

	for (std::list<TuioBlob3D*>::iterator iter = freeBlob3DList.begin(); iter != freeBlob3DList.end(); iter++)
		delete(*iter);
	freeBlob3DList.clear();



}






TuioObject* TuioClient::getTuioObject(int src_id, long s_id) {
	lockObjectList();
	for (std::list<TuioObject*>::iterator iter=objectList.begin(); iter != objectList.end(); iter++) {
		if (((*iter)->getTuioSourceID()==src_id) && ((*iter)->getSessionID()==s_id)) {
			unlockObjectList();
			return (*iter);
		}
	}	
	unlockObjectList();
	return NULL;
}

TuioCursor* TuioClient::getTuioCursor(int src_id, long s_id) {
	lockCursorList();
	for (std::list<TuioCursor*>::iterator iter=cursorList.begin(); iter != cursorList.end(); iter++) {
		if (((*iter)->getTuioSourceID()==src_id) && ((*iter)->getSessionID()==s_id)) {
			unlockCursorList();
			return (*iter);
		}
	}	
	unlockCursorList();
	return NULL;
}

TuioBlob* TuioClient::getTuioBlob(int src_id, long s_id) {
	lockBlobList();
	for (std::list<TuioBlob*>::iterator iter=blobList.begin(); iter != blobList.end(); iter++) {
		if (((*iter)->getTuioSourceID()==src_id) && ((*iter)->getSessionID()==s_id)) {
			unlockBlobList();
			return (*iter);
		}
	}	
	unlockBlobList();
	return NULL;
}


std::list<TuioObject*> TuioClient::getTuioObjects(int source_id) {
	lockObjectList();
	std::list<TuioObject*> listBuffer;
	for (std::list<TuioObject*>::iterator iter=objectList.begin(); iter != objectList.end(); iter++) {
		TuioObject *tobj = (*iter);
		if (tobj->getTuioSourceID()==source_id)  listBuffer.push_back(tobj);
	}	
	unlockObjectList();
	return listBuffer;
}

std::list<TuioCursor*> TuioClient::getTuioCursors(int source_id) {
	lockCursorList();
	std::list<TuioCursor*> listBuffer;
	for (std::list<TuioCursor*>::iterator iter=cursorList.begin(); iter != cursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		if (tcur->getTuioSourceID()==source_id) listBuffer.push_back(tcur);
	}
	unlockCursorList();
	return listBuffer;
}

std::list<TuioBlob*> TuioClient::getTuioBlobs(int source_id) {
	lockBlobList();
	std::list<TuioBlob*> listBuffer;
	for (std::list<TuioBlob*>::iterator iter=blobList.begin(); iter != blobList.end(); iter++) {
		TuioBlob *tblb = (*iter);
		if (tblb->getTuioSourceID()==source_id) listBuffer.push_back(tblb);
	}	
	unlockBlobList();
	return listBuffer;
}


std::list<TuioObject> TuioClient::copyTuioObjects(int source_id) {
	lockObjectList();
	std::list<TuioObject> listBuffer;
	for (std::list<TuioObject*>::iterator iter=objectList.begin(); iter != objectList.end(); iter++) {
		TuioObject *tobj = (*iter);
		if (tobj->getTuioSourceID()==source_id)  listBuffer.push_back(*tobj);
	}	
	unlockObjectList();
	return listBuffer;
}

std::list<TuioCursor> TuioClient::copyTuioCursors(int source_id) {
	lockCursorList();
	std::list<TuioCursor> listBuffer;
	for (std::list<TuioCursor*>::iterator iter=cursorList.begin(); iter != cursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		if (tcur->getTuioSourceID()==source_id) listBuffer.push_back(*tcur);
	}
	unlockCursorList();
	return listBuffer;
}

std::list<TuioBlob> TuioClient::copyTuioBlobs(int source_id) {
	lockBlobList();
	std::list<TuioBlob> listBuffer;
	for (std::list<TuioBlob*>::iterator iter=blobList.begin(); iter != blobList.end(); iter++) {
		TuioBlob *tblb = (*iter);
		if (tblb->getTuioSourceID()==source_id) listBuffer.push_back(*tblb);
	}	
	unlockBlobList();
	return listBuffer;
}











TuioObject25D* TuioClient::getTuioObject25D(int src_id, long s_id) {
	lockObject25DList();
	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++) {
		if (((*iter)->getTuioSourceID() == src_id) && ((*iter)->getSessionID() == s_id)) {
			unlockObject25DList();
			return (*iter);
		}
	}
	unlockObject25DList();
	return NULL;
}

TuioCursor25D* TuioClient::getTuioCursor25D(int src_id, long s_id) {
	lockCursor25DList();
	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
		if (((*iter)->getTuioSourceID() == src_id) && ((*iter)->getSessionID() == s_id)) {
			unlockCursor25DList();
			return (*iter);
		}
	}
	unlockCursor25DList();
	return NULL;
}

TuioBlob25D* TuioClient::getTuioBlob25D(int src_id, long s_id) {
	lockBlob25DList();
	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
		if (((*iter)->getTuioSourceID() == src_id) && ((*iter)->getSessionID() == s_id)) {
			unlockBlob25DList();
			return (*iter);
		}
	}
	unlockBlob25DList();
	return NULL;
}


std::list<TuioObject25D*> TuioClient::getTuioObjects25D(int source_id) {
	lockObject25DList();
	std::list<TuioObject25D*> listBuffer;
	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++) {
		TuioObject25D *tobj = (*iter);
		if (tobj->getTuioSourceID() == source_id)  listBuffer.push_back(tobj);
	}
	unlockObject25DList();
	return listBuffer;
}

std::list<TuioCursor25D*> TuioClient::getTuioCursors25D(int source_id) {
	lockCursor25DList();
	std::list<TuioCursor25D*> listBuffer;
	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
		TuioCursor25D *tcur = (*iter);
		if (tcur->getTuioSourceID() == source_id) listBuffer.push_back(tcur);
	}
	unlockCursor25DList();
	return listBuffer;
}

std::list<TuioBlob25D*> TuioClient::getTuioBlobs25D(int source_id) {
	lockBlob25DList();
	std::list<TuioBlob25D*> listBuffer;
	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
		TuioBlob25D *tblb = (*iter);
		if (tblb->getTuioSourceID() == source_id) listBuffer.push_back(tblb);
	}
	unlockBlob25DList();
	return listBuffer;
}


std::list<TuioObject25D> TuioClient::copyTuioObjects25D(int source_id) {
	lockObject25DList();
	std::list<TuioObject25D> listBuffer;
	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++) {
		TuioObject25D *tobj = (*iter);
		if (tobj->getTuioSourceID() == source_id)  listBuffer.push_back(*tobj);
	}
	unlockObject25DList();
	return listBuffer;
}

std::list<TuioCursor25D> TuioClient::copyTuioCursors25D(int source_id) {
	lockCursor25DList();
	std::list<TuioCursor25D> listBuffer;
	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
		TuioCursor25D *tcur = (*iter);
		if (tcur->getTuioSourceID() == source_id) listBuffer.push_back(*tcur);
	}
	unlockCursor25DList();
	return listBuffer;
}

std::list<TuioBlob25D> TuioClient::copyTuioBlobs25D(int source_id) {
	lockBlob25DList();
	std::list<TuioBlob25D> listBuffer;
	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
		TuioBlob25D *tblb = (*iter);
		if (tblb->getTuioSourceID() == source_id) listBuffer.push_back(*tblb);
	}
	unlockBlob25DList();
	return listBuffer;
}





TuioObject3D* TuioClient::getTuioObject3D(int src_id, long s_id) {
	lockObject3DList();
	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++) {
		if (((*iter)->getTuioSourceID() == src_id) && ((*iter)->getSessionID() == s_id)) {
			unlockObject3DList();
			return (*iter);
		}
	}
	unlockObject3DList();
	return NULL;
}

TuioCursor3D* TuioClient::getTuioCursor3D(int src_id, long s_id) {
	lockCursor3DList();
	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
		if (((*iter)->getTuioSourceID() == src_id) && ((*iter)->getSessionID() == s_id)) {
			unlockCursor3DList();
			return (*iter);
		}
	}
	unlockCursor3DList();
	return NULL;
}

TuioBlob3D* TuioClient::getTuioBlob3D(int src_id, long s_id) {
	lockBlob3DList();
	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
		if (((*iter)->getTuioSourceID() == src_id) && ((*iter)->getSessionID() == s_id)) {
			unlockBlob3DList();
			return (*iter);
		}
	}
	unlockBlob3DList();
	return NULL;
}


std::list<TuioObject3D*> TuioClient::getTuioObjects3D(int source_id) {
	lockObject3DList();
	std::list<TuioObject3D*> listBuffer;
	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++) {
		TuioObject3D *tobj = (*iter);
		if (tobj->getTuioSourceID() == source_id)  listBuffer.push_back(tobj);
	}
	unlockObject3DList();
	return listBuffer;
}

std::list<TuioCursor3D*> TuioClient::getTuioCursors3D(int source_id) {
	lockCursor3DList();
	std::list<TuioCursor3D*> listBuffer;
	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
		TuioCursor3D *tcur = (*iter);
		if (tcur->getTuioSourceID() == source_id) listBuffer.push_back(tcur);
	}
	unlockCursor3DList();
	return listBuffer;
}

std::list<TuioBlob3D*> TuioClient::getTuioBlobs3D(int source_id) {
	lockBlob3DList();
	std::list<TuioBlob3D*> listBuffer;
	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
		TuioBlob3D *tblb = (*iter);
		if (tblb->getTuioSourceID() == source_id) listBuffer.push_back(tblb);
	}
	unlockBlob3DList();
	return listBuffer;
}


std::list<TuioObject3D> TuioClient::copyTuioObjects3D(int source_id) {
	lockObject3DList();
	std::list<TuioObject3D> listBuffer;
	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++) {
		TuioObject3D *tobj = (*iter);
		if (tobj->getTuioSourceID() == source_id)  listBuffer.push_back(*tobj);
	}
	unlockObject3DList();
	return listBuffer;
}

std::list<TuioCursor3D> TuioClient::copyTuioCursors3D(int source_id) {
	lockCursor3DList();
	std::list<TuioCursor3D> listBuffer;
	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
		TuioCursor3D *tcur = (*iter);
		if (tcur->getTuioSourceID() == source_id) listBuffer.push_back(*tcur);
	}
	unlockCursor3DList();
	return listBuffer;
}

std::list<TuioBlob3D> TuioClient::copyTuioBlobs3D(int source_id) {
	lockBlob3DList();
	std::list<TuioBlob3D> listBuffer;
	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
		TuioBlob3D *tblb = (*iter);
		if (tblb->getTuioSourceID() == source_id) listBuffer.push_back(*tblb);
	}
	unlockBlob3DList();
	return listBuffer;
}




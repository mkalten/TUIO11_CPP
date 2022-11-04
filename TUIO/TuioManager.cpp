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

#include "TuioManager.h"
using namespace TUIO;


TuioManager::TuioManager() 
	: currentFrameTime(TuioTime::getSystemTime())
	, currentFrame(-1)
	, maxCursorID(-1)
	, maxBlobID(-1)
	, maxCursor25DID(-1)
	, maxBlob25DID(-1)
	, maxCursor3DID(-1)
	, maxBlob3DID(-1)
	, sessionID(-1)
	, updateObject(false)
	, updateCursor(false)
	, updateBlob(false)
	, updateObject25D(false)
	, updateCursor25D(false)
	, updateBlob25D(false)
	, updateObject3D(false)
	, updateCursor3D(false)
	, updateBlob3D(false)
	, verbose(false)
	, invert_x(false)
	, invert_y(false)
	, invert_z(false)
	, invert_a(false)
	, invert_b(false)
	, invert_c(false)
{

}

TuioManager::~TuioManager() {
}


TuioObject* TuioManager::addTuioObject(int f_id, float x, float y, float a) {
	sessionID++;
	TuioObject *tobj = new TuioObject(currentFrameTime, sessionID, f_id, x, y, a);
	objectList.push_back(tobj);
	updateObject = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioObject(tobj);

	if (verbose)
		std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;

	return tobj;
}

void TuioManager::addExternalTuioObject(TuioObject *tobj) {
	if (tobj==NULL) return;
	tobj->setSessionID(++sessionID);
	objectList.push_back(tobj);
	updateObject = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioObject(tobj);

	if (verbose)
		std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;
}

void TuioManager::updateTuioObject(TuioObject *tobj, float x, float y, float a) {
	if (tobj==NULL) return;
	if (tobj->getTuioTime()==currentFrameTime) return;
	tobj->update(currentFrameTime,x,y,a);
	updateObject = true;

	if (tobj->isMoving()) {
		for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioObject(tobj);
		
		if (verbose)	
			std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() 
			<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
	}	
}

void TuioManager::updateExternalTuioObject(TuioObject *tobj) {
	if (tobj==NULL) return;
	updateObject = true;

	if (tobj->isMoving()) {
		for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioObject(tobj);
		
		if (verbose)	
			std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() 
			<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
	}
}

void TuioManager::removeTuioObject(TuioObject *tobj) {
	if (tobj==NULL) return;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioObject(tobj);
	
	if (verbose)
		std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
    
    objectList.remove(tobj);
    delete tobj;
    updateObject = true;
}

void TuioManager::removeExternalTuioObject(TuioObject *tobj) {
	if (tobj==NULL) return;
	objectList.remove(tobj);
	updateObject = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioObject(tobj);

	if (verbose)
		std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
}

TuioCursor* TuioManager::addTuioCursor(float x, float y) {
	sessionID++;
	
	int cursorID = (int)cursorList.size();
	if ((int)(cursorList.size())<=maxCursorID) {
		std::list<TuioCursor*>::iterator closestCursor = freeCursorList.begin();
		
		for(std::list<TuioCursor*>::iterator iter = freeCursorList.begin();iter!= freeCursorList.end(); iter++) {
			if((*iter)->getDistance(x,y,0)<(*closestCursor)->getDistance(x,y,0)) closestCursor = iter;
		}
		
		TuioCursor *freeCursor = (*closestCursor);
		cursorID = (*closestCursor)->getCursorID();
		freeCursorList.erase(closestCursor);
		delete freeCursor;
	} else maxCursorID = cursorID;	
	
	TuioCursor *tcur = new TuioCursor(currentFrameTime, sessionID, cursorID, x, y);
	cursorList.push_back(tcur);
	updateCursor = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioCursor(tcur);
	
	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "add cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << std::endl;

	return tcur;
}

void TuioManager::addExternalTuioCursor(TuioCursor *tcur) {
	if (tcur==NULL) return;
	tcur->setSessionID(++sessionID);
	cursorList.push_back(tcur);
	updateCursor = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioCursor(tcur);

	if (verbose) 
		std::cout << "add cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << std::endl;
}

void TuioManager::updateTuioCursor(TuioCursor *tcur,float x, float y) {
	if (tcur==NULL) return;
	//if (tcur->getTuioTime()==currentFrameTime) return;
	tcur->update(currentFrameTime,x,y,0);
	updateCursor = true;

	if (tcur->isMoving()) {	
		for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioCursor(tcur);

		if (verbose)	 	
			std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() 
			<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
	}
}

void TuioManager::updateExternalTuioCursor(TuioCursor *tcur) {
	if (tcur==NULL) return;
	updateCursor = true;
	
	if (tcur->isMoving()) {	
		for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioCursor(tcur);
				
		if (verbose)		
			std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() 
			<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
	}
}

void TuioManager::removeTuioCursor(TuioCursor *tcur) {
	if (tcur==NULL) return;

	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ")" << std::endl;
	
	cursorList.remove(tcur);
	tcur->remove(currentFrameTime);
	updateCursor = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioCursor(tcur);

	if (tcur->getCursorID()==maxCursorID) {
		maxCursorID = -1;
		delete tcur;
		
		if (cursorList.size()>0) {
			std::list<TuioCursor*>::iterator clist;
			for (clist=cursorList.begin(); clist != cursorList.end(); clist++) {
				int cursorID = (*clist)->getCursorID();
				if (cursorID>maxCursorID) maxCursorID=cursorID;
			}
			
			freeCursorBuffer.clear();
			for (std::list<TuioCursor*>::iterator flist=freeCursorList.begin(); flist != freeCursorList.end(); flist++) {
				TuioCursor *freeCursor = (*flist);
				if (freeCursor->getCursorID()>maxCursorID) delete freeCursor;
				else freeCursorBuffer.push_back(freeCursor);
			}
			
			freeCursorList = freeCursorBuffer;

		} else {
			for (std::list<TuioCursor*>::iterator flist=freeCursorList.begin(); flist != freeCursorList.end(); flist++) {
				TuioCursor *freeCursor = (*flist);
				delete freeCursor;
			}
			freeCursorList.clear();
		}
	} else if (tcur->getCursorID()<maxCursorID) {
		freeCursorList.push_back(tcur);	
	}
}

void TuioManager::removeExternalTuioCursor(TuioCursor *tcur) {
	if (tcur==NULL) return;
	
	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ")" << std::endl;
	
	cursorList.remove(tcur);
	updateCursor = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioCursor(tcur);
}

TuioBlob* TuioManager::addTuioBlob(float x, float y, float a, float w, float h, float f) {
	sessionID++;
	
	int blobID = (int)blobList.size();
	if ((int)(blobList.size())<=maxBlobID) {
		std::list<TuioBlob*>::iterator closestBlob = freeBlobList.begin();
		
		for(std::list<TuioBlob*>::iterator iter = freeBlobList.begin();iter!= freeBlobList.end(); iter++) {
			if((*iter)->getDistance(x,y,0)<(*closestBlob)->getDistance(x,y,0)) closestBlob = iter;
		}
		
		TuioBlob *freeBlob = (*closestBlob);
		blobID = (*closestBlob)->getBlobID();
		freeBlobList.erase(closestBlob);
		delete freeBlob;
	} else maxBlobID = blobID;	
	
	TuioBlob *tblb = new TuioBlob(currentFrameTime, sessionID, blobID, x, y, a, w, h, f);
	blobList.push_back(tblb);
	updateBlob = true;
	
	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioBlob(tblb);
	
	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "add blb " << tblb->getBlobID() << " (" <<  tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY()  << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea() << std::endl;
	
	return tblb;
}

void TuioManager::addExternalTuioBlob(TuioBlob *tblb) {
	if (tblb==NULL) return;
	
	int blobID = (int)blobList.size();
	if (blobID <= maxBlobID) {
		std::list<TuioBlob*>::iterator closestBlob = freeBlobList.begin();
		
		for(std::list<TuioBlob*>::iterator iter = freeBlobList.begin();iter!= freeBlobList.end(); iter++) {
			if((*iter)->getDistance(tblb->getX(),tblb->getY(),0)<(*closestBlob)->getDistance(tblb->getX(),tblb->getY(),0)) closestBlob = iter;
		}
		
		TuioBlob *freeBlob = (*closestBlob);
		blobID = (*closestBlob)->getBlobID();
		freeBlobList.erase(closestBlob);
		delete freeBlob;
	} else maxBlobID = blobID;
	
	tblb->setSessionID(++sessionID);
	tblb->setBlobID(blobID);
	
	blobList.push_back(tblb);
	updateBlob = true;
	
	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioBlob(tblb);
	
	if (verbose) 
		std::cout << "add blb " << tblb->getBlobID() << " (" <<  tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY()  << " " << tblb->getAngle() << " " << tblb->getWidth()  << " " << tblb->getHeight() << " " << tblb->getArea() << std::endl;
}

void TuioManager::updateTuioBlob(TuioBlob *tblb,float x, float y, float a, float w, float h, float f) {
	if (tblb==NULL) return;
	if (tblb->getTuioTime()==currentFrameTime) return;
	tblb->update(currentFrameTime,x,y,a,w,h,f);
	updateBlob = true;
	
	if (tblb->isMoving()) {	
		for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioBlob(tblb);
		
		if (verbose)	 	
			std::cout << "set blb " << tblb->getBlobID() << " (" <<  tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY()  << " " << tblb->getAngle() << " " << tblb->getWidth()  << " " << tblb->getHeight() << " " << tblb->getArea()
			<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed()  << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel()<< " " << tblb->getRotationAccel() << " " << std::endl;
	}
}

void TuioManager::updateExternalTuioBlob(TuioBlob *tblb) {
	if (tblb==NULL) return;
	updateBlob = true;
	
	if (tblb->isMoving()) {	
		for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioBlob(tblb);
		
		if (verbose)		
			std::cout << "set blb " << tblb->getBlobID() << " (" <<  tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getAngle() << " " << tblb->getWidth()  << " " << tblb->getHeight() << " " << tblb->getArea()
			<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel()<< " " << tblb->getRotationAccel() << " " << std::endl;
	}
}

void TuioManager::removeTuioBlob(TuioBlob *tblb) {
	if (tblb==NULL) return;
	
	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del blb " << tblb->getBlobID() << " (" <<  tblb->getSessionID() << ")" << std::endl;
	
	blobList.remove(tblb);
	tblb->remove(currentFrameTime);
	updateBlob = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioBlob(tblb);
	
	if (tblb->getBlobID()==maxBlobID) {
		maxBlobID = -1;
		delete tblb;
		
		if (blobList.size()>0) {
			std::list<TuioBlob*>::iterator clist;
			for (clist=blobList.begin(); clist != blobList.end(); clist++) {
				int blobID = (*clist)->getBlobID();
				if (blobID>maxBlobID) maxBlobID=blobID;
			}
			
			freeBlobBuffer.clear();
			for (std::list<TuioBlob*>::iterator flist=freeBlobList.begin(); flist != freeBlobList.end(); flist++) {
				TuioBlob *freeBlob = (*flist);
				if (freeBlob->getBlobID()>maxBlobID) delete freeBlob;
				else freeBlobBuffer.push_back(freeBlob);
			}
			
			freeBlobList = freeBlobBuffer;
			
		} else {
			for (std::list<TuioBlob*>::iterator flist=freeBlobList.begin(); flist != freeBlobList.end(); flist++) {
				TuioBlob *freeBlob = (*flist);
				delete freeBlob;
			}
			freeBlobList.clear();
		}
	} else if (tblb->getBlobID()<maxBlobID) {
		freeBlobList.push_back(tblb);	
	}
	
}

void TuioManager::removeExternalTuioBlob(TuioBlob *tblb) {
	if (tblb==NULL) return;
	
	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del blb " << tblb->getBlobID() << " (" <<  tblb->getSessionID() << ")" << std::endl;
	
	blobList.remove(tblb);
	updateBlob = true;
	
	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioBlob(tblb);
}








TuioObject25D* TuioManager::addTuioObject25D(int f_id, float x, float y, float z, float a) {
	sessionID++;
	TuioObject25D *tobj = new TuioObject25D(currentFrameTime, sessionID, f_id, x, y, z, a);
	object25DList.push_back(tobj);
	updateObject25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioObject25D(tobj);

	if (verbose)
		std::cout << "add 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getAngle() << std::endl;

	return tobj;
}

void TuioManager::addExternalTuioObject25D(TuioObject25D *tobj) {
	if (tobj == NULL) return;
	tobj->setSessionID(++sessionID);
	object25DList.push_back(tobj);
	updateObject25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioObject25D(tobj);

	if (verbose)
		std::cout << "add 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getAngle() << std::endl;
}

void TuioManager::updateTuioObject25D(TuioObject25D *tobj, float x, float y,float z, float a) {
	if (tobj == NULL) return;
	if (tobj->getTuioTime() == currentFrameTime) return;
	tobj->update(currentFrameTime, x, y,z, a);
	updateObject25D = true;

	if (tobj->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioObject25D(tobj);

		if (verbose)
			std::cout << "set 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getAngle()
			<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
	}
}

void TuioManager::updateExternalTuioObject25D(TuioObject25D *tobj) {
	if (tobj == NULL) return;
	updateObject25D = true;

	if (tobj->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioObject25D(tobj);

		if (verbose)
			std::cout << "set 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getAngle()
			<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
	}
}

void TuioManager::removeTuioObject25D(TuioObject25D  *tobj) {
	if (tobj == NULL) return;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioObject25D(tobj);

	if (verbose)
		std::cout << "del 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;

	object25DList.remove(tobj);
	delete tobj;
	updateObject25D = true;
}

void TuioManager::removeExternalTuioObject25D(TuioObject25D *tobj) {
	if (tobj == NULL) return;
	object25DList.remove(tobj);
	updateObject25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioObject25D(tobj);

	if (verbose)
		std::cout << "del 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
}

TuioCursor25D* TuioManager::addTuioCursor25D(float x, float y, float z) {
	sessionID++;

	int cursorID = (int)cursor25DList.size();
	if ((int)(cursor25DList.size()) <= maxCursor25DID) {
		std::list<TuioCursor25D*>::iterator closestCursor = freeCursor25DList.begin();

		for (std::list<TuioCursor25D*>::iterator iter = freeCursor25DList.begin(); iter != freeCursor25DList.end(); iter++) {
			if ((*iter)->getDistance(x, y, z) < (*closestCursor)->getDistance(x, y, z)) closestCursor = iter;
		}

		TuioCursor25D *freeCursor = (*closestCursor);
		cursorID = (*closestCursor)->getCursorID();
		freeCursor25DList.erase(closestCursor);
		delete freeCursor;
	}
	else maxCursor25DID = cursorID;

	TuioCursor25D *tcur = new TuioCursor25D(currentFrameTime, sessionID, cursorID, x, y,z);
	cursor25DList.push_back(tcur);
	updateCursor25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioCursor25D(tcur);

	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "add 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ() << std::endl;

	return tcur;
}

void TuioManager::addExternalTuioCursor25D(TuioCursor25D *tcur) {
	if (tcur == NULL) return;
	tcur->setSessionID(++sessionID);
	cursor25DList.push_back(tcur);
	updateCursor25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioCursor25D(tcur);

	if (verbose)
		std::cout << "add 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ() << std::endl;
}

void TuioManager::updateTuioCursor25D(TuioCursor25D *tcur, float x, float y, float z) {
	if (tcur == NULL) return;
	//if (tcur->getTuioTime()==currentFrameTime) return;
	tcur->update(currentFrameTime, x, y, z);
	updateCursor25D = true;

	if (tcur->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioCursor25D(tcur);

		if (verbose)
			std::cout << "set 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
			<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
	}
}

void TuioManager::updateExternalTuioCursor25D(TuioCursor25D *tcur) {
	if (tcur == NULL) return;
	updateCursor25D = true;

	if (tcur->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioCursor25D(tcur);

		if (verbose)
			std::cout << "set 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
			<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
	}
}

void TuioManager::removeTuioCursor25D(TuioCursor25D *tcur) {
	if (tcur == NULL) return;

	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ")" << std::endl;

	cursor25DList.remove(tcur);
	tcur->remove(currentFrameTime);
	updateCursor25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioCursor25D(tcur);

	if (tcur->getCursorID() == maxCursor25DID) {
		maxCursor25DID = -1;
		delete tcur;

		if (cursor25DList.size() > 0) {
			std::list<TuioCursor25D*>::iterator clist;
			for (clist = cursor25DList.begin(); clist != cursor25DList.end(); clist++) {
				int cursorID = (*clist)->getCursorID();
				if (cursorID > maxCursor25DID) maxCursor25DID = cursorID;
			}

			freeCursor25DBuffer.clear();
			for (std::list<TuioCursor25D*>::iterator flist = freeCursor25DList.begin(); flist != freeCursor25DList.end(); flist++) {
				TuioCursor25D *freeCursor = (*flist);
				if (freeCursor->getCursorID() > maxCursor25DID) delete freeCursor;
				else freeCursor25DBuffer.push_back(freeCursor);
			}

			freeCursor25DList = freeCursor25DBuffer;

		}
		else {
			for (std::list<TuioCursor25D*>::iterator flist = freeCursor25DList.begin(); flist != freeCursor25DList.end(); flist++) {
				TuioCursor25D *freeCursor = (*flist);
				delete freeCursor;
			}
			freeCursor25DList.clear();
		}
	}
	else if (tcur->getCursorID() < maxCursor25DID) {
		freeCursor25DList.push_back(tcur);
	}
}

void TuioManager::removeExternalTuioCursor25D(TuioCursor25D *tcur) {
	if (tcur == NULL) return;

	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ")" << std::endl;

	cursor25DList.remove(tcur);
	updateCursor25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioCursor25D(tcur);
}

TuioBlob25D* TuioManager::addTuioBlob25D(float x, float y,float z, float a, float w, float h, float f) {
	sessionID++;

	int blobID = (int)blob25DList.size();
	if ((int)(blob25DList.size()) <= maxBlob25DID) {
		std::list<TuioBlob25D*>::iterator closestBlob = freeBlob25DList.begin();

		for (std::list<TuioBlob25D*>::iterator iter = freeBlob25DList.begin(); iter != freeBlob25DList.end(); iter++) {
			if ((*iter)->getDistance(x, y, z) < (*closestBlob)->getDistance(x, y, z)) closestBlob = iter;
		}

		TuioBlob25D *freeBlob = (*closestBlob);
		blobID = (*closestBlob)->getBlobID();
		freeBlob25DList.erase(closestBlob);
		delete freeBlob;
	}
	else maxBlob25DID = blobID;

	TuioBlob25D *tblb = new TuioBlob25D(currentFrameTime, sessionID, blobID, x, y,z, a, w, h, f);
	blob25DList.push_back(tblb);
	updateBlob25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioBlob25D(tblb);

	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "add 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea() << std::endl;

	return tblb;
}

void TuioManager::addExternalTuioBlob25D(TuioBlob25D *tblb) {
	if (tblb == NULL) return;

	int blobID = (int)blob25DList.size();
	if (blobID <= maxBlob25DID) {
		std::list<TuioBlob25D*>::iterator closestBlob = freeBlob25DList.begin();

		for (std::list<TuioBlob25D*>::iterator iter = freeBlob25DList.begin(); iter != freeBlob25DList.end(); iter++) {
			if ((*iter)->getDistance(tblb->getX(), tblb->getY(), tblb->getZ()) < (*closestBlob)->getDistance(tblb->getX(), tblb->getY(), tblb->getY())) closestBlob = iter;
		}

		TuioBlob25D *freeBlob = (*closestBlob);
		blobID = (*closestBlob)->getBlobID();
		freeBlob25DList.erase(closestBlob);
		delete freeBlob;
	}
	else maxBlob25DID = blobID;

	tblb->setSessionID(++sessionID);
	tblb->setBlobID(blobID);

	blob25DList.push_back(tblb);
	updateBlob25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioBlob25D(tblb);

	if (verbose)
		std::cout << "add 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea() << std::endl;
}

void TuioManager::updateTuioBlob25D(TuioBlob25D *tblb, float x, float y,float z, float a, float w, float h, float f) {
	if (tblb == NULL) return;
	if (tblb->getTuioTime() == currentFrameTime) return;
	tblb->update(currentFrameTime, x, y, z, a, w, h, f);
	updateBlob25D = true;

	if (tblb->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioBlob25D(tblb);

		if (verbose)
			std::cout << "set 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea()
			<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << " " << std::endl;
	}
}

void TuioManager::updateExternalTuioBlob25D(TuioBlob25D *tblb) {
	if (tblb == NULL) return;
	updateBlob25D = true;

	if (tblb->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioBlob25D(tblb);

		if (verbose)
			std::cout << "set 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea()
			<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << " " << std::endl;
	}
}

void TuioManager::removeTuioBlob25D(TuioBlob25D *tblb) {
	if (tblb == NULL) return;

	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ")" << std::endl;

	blob25DList.remove(tblb);
	tblb->remove(currentFrameTime);
	updateBlob25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioBlob25D(tblb);

	if (tblb->getBlobID() == maxBlob25DID) {
		maxBlob25DID = -1;
		delete tblb;

		if (blob25DList.size() > 0) {
			std::list<TuioBlob25D*>::iterator clist;
			for (clist = blob25DList.begin(); clist != blob25DList.end(); clist++) {
				int blobID = (*clist)->getBlobID();
				if (blobID > maxBlob25DID) maxBlob25DID = blobID;
			}

			freeBlob25DBuffer.clear();
			for (std::list<TuioBlob25D*>::iterator flist = freeBlob25DList.begin(); flist != freeBlob25DList.end(); flist++) {
				TuioBlob25D *freeBlob = (*flist);
				if (freeBlob->getBlobID() > maxBlob25DID) delete freeBlob;
				else freeBlob25DBuffer.push_back(freeBlob);
			}

			freeBlob25DList = freeBlob25DBuffer;

		}
		else {
			for (std::list<TuioBlob25D*>::iterator flist = freeBlob25DList.begin(); flist != freeBlob25DList.end(); flist++) {
				TuioBlob25D *freeBlob = (*flist);
				delete freeBlob;
			}
			freeBlob25DList.clear();
		}
	}
	else if (tblb->getBlobID() < maxBlob25DID) {
		freeBlob25DList.push_back(tblb);
	}

}

void TuioManager::removeExternalTuioBlob25D(TuioBlob25D *tblb) {
	if (tblb == NULL) return;

	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ")" << std::endl;

	blob25DList.remove(tblb);
	updateBlob25D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioBlob25D(tblb);
}













TuioObject3D* TuioManager::addTuioObject3D(int f_id, float x, float y, float z, float roll, float pitch, float yaw) {
	sessionID++;
	TuioObject3D *tobj = new TuioObject3D(currentFrameTime, sessionID, f_id, x, y, z, roll, pitch, yaw);
	object3DList.push_back(tobj);
	updateObject3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioObject3D(tobj);

	if (verbose)
		std::cout << "add 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getRoll() << " " << tobj->getPitch() << " " << tobj->getYaw() << std::endl;

	return tobj;
}

void TuioManager::addExternalTuioObject3D(TuioObject3D *tobj) {
	if (tobj == NULL) return;
	tobj->setSessionID(++sessionID);
	object3DList.push_back(tobj);
	updateObject3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioObject3D(tobj);

	if (verbose)
		std::cout << "add 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getRoll() << " " << tobj->getPitch() << " " << tobj->getYaw() << std::endl;
}

void TuioManager::updateTuioObject3D(TuioObject3D *tobj, float x, float y, float z, float roll, float pitch, float yaw) {
	if (tobj == NULL) return;
	if (tobj->getTuioTime() == currentFrameTime) return;
	tobj->update(currentFrameTime, x, y, z, roll, pitch, yaw);
	updateObject3D = true;

	if (tobj->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioObject3D(tobj);

		if (verbose)
			std::cout << "set 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getRoll() << " " << tobj->getPitch() << " " << tobj->getYaw()
			<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRollSpeed() << " " << tobj->getPitchSpeed() << " " << tobj->getYawSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
	}
}

void TuioManager::updateExternalTuioObject3D(TuioObject3D *tobj) {
	if (tobj == NULL) return;
	updateObject3D = true;

	if (tobj->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioObject3D(tobj);

		if (verbose)
			std::cout << "set 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getZ() << " " << tobj->getRoll() << " " << tobj->getPitch() << " " << tobj->getYaw()
			<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRollSpeed() << " " << tobj->getPitchSpeed() << " " << tobj->getYawSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
	}
}

void TuioManager::removeTuioObject3D(TuioObject3D  *tobj) {
	if (tobj == NULL) return;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioObject3D(tobj);

	if (verbose)
		std::cout << "del 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;

	object3DList.remove(tobj);
	delete tobj;
	updateObject3D = true;
}

void TuioManager::removeExternalTuioObject3D(TuioObject3D *tobj) {
	if (tobj == NULL) return;
	object3DList.remove(tobj);
	updateObject3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioObject3D(tobj);

	if (verbose)
		std::cout << "del 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
}

TuioCursor3D* TuioManager::addTuioCursor3D(float x, float y, float z) {
	sessionID++;

	int cursorID = (int)cursor3DList.size();
	if ((int)(cursor3DList.size()) <= maxCursor3DID) {
		std::list<TuioCursor3D*>::iterator closestCursor = freeCursor3DList.begin();

		for (std::list<TuioCursor3D*>::iterator iter = freeCursor3DList.begin(); iter != freeCursor3DList.end(); iter++) {
			if ((*iter)->getDistance(x, y, z) < (*closestCursor)->getDistance(x, y, z)) closestCursor = iter;
		}

		TuioCursor3D *freeCursor = (*closestCursor);
		cursorID = (*closestCursor)->getCursorID();
		freeCursor3DList.erase(closestCursor);
		delete freeCursor;
	}
	else maxCursor3DID = cursorID;

	TuioCursor3D *tcur = new TuioCursor3D(currentFrameTime, sessionID, cursorID, x, y, z);
	cursor3DList.push_back(tcur);
	updateCursor3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioCursor3D(tcur);

	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "add 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ() << std::endl;

	return tcur;
}

void TuioManager::addExternalTuioCursor3D(TuioCursor3D *tcur) {
	if (tcur == NULL) return;
	tcur->setSessionID(++sessionID);
	cursor3DList.push_back(tcur);
	updateCursor3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioCursor3D(tcur);

	if (verbose)
		std::cout << "add 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ() << std::endl;
}

void TuioManager::updateTuioCursor3D(TuioCursor3D *tcur, float x, float y, float z) {
	if (tcur == NULL) return;
	//if (tcur->getTuioTime()==currentFrameTime) return;
	tcur->update(currentFrameTime, x, y, z);
	updateCursor3D = true;

	if (tcur->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioCursor3D(tcur);

		if (verbose)
			std::cout << "set 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
			<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
	}
}

void TuioManager::updateExternalTuioCursor3D(TuioCursor3D *tcur) {
	if (tcur == NULL) return;
	updateCursor3D = true;

	if (tcur->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioCursor3D(tcur);

		if (verbose)
			std::cout << "set 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
			<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
	}
}

void TuioManager::removeTuioCursor3D(TuioCursor3D *tcur) {
	if (tcur == NULL) return;

	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ")" << std::endl;

	cursor3DList.remove(tcur);
	tcur->remove(currentFrameTime);
	updateCursor3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioCursor3D(tcur);

	if (tcur->getCursorID() == maxCursor3DID) {
		maxCursor3DID = -1;
		delete tcur;

		if (cursor3DList.size() > 0) {
			std::list<TuioCursor3D*>::iterator clist;
			for (clist = cursor3DList.begin(); clist != cursor3DList.end(); clist++) {
				int cursorID = (*clist)->getCursorID();
				if (cursorID > maxCursor3DID) maxCursor3DID = cursorID;
			}

			freeCursor3DBuffer.clear();
			for (std::list<TuioCursor3D*>::iterator flist = freeCursor3DList.begin(); flist != freeCursor3DList.end(); flist++) {
				TuioCursor3D *freeCursor = (*flist);
				if (freeCursor->getCursorID() > maxCursor3DID) delete freeCursor;
				else freeCursor3DBuffer.push_back(freeCursor);
			}

			freeCursor3DList = freeCursor3DBuffer;

		}
		else {
			for (std::list<TuioCursor3D*>::iterator flist = freeCursor3DList.begin(); flist != freeCursor3DList.end(); flist++) {
				TuioCursor3D *freeCursor = (*flist);
				delete freeCursor;
			}
			freeCursor3DList.clear();
		}
	}
	else if (tcur->getCursorID() < maxCursor3DID) {
		freeCursor3DList.push_back(tcur);
	}
}

void TuioManager::removeExternalTuioCursor3D(TuioCursor3D *tcur) {
	if (tcur == NULL) return;

	if (verbose /*&& tcur->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ")" << std::endl;

	cursor3DList.remove(tcur);
	updateCursor3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioCursor3D(tcur);
}

TuioBlob3D* TuioManager::addTuioBlob3D(float x, float y, float z, float roll, float pitch, float yaw, float width, float height, float depth, float volume) {
	sessionID++;

	int blobID = (int)blob3DList.size();
	if ((int)(blob3DList.size()) <= maxBlob3DID) {
		std::list<TuioBlob3D*>::iterator closestBlob = freeBlob3DList.begin();

		for (std::list<TuioBlob3D*>::iterator iter = freeBlob3DList.begin(); iter != freeBlob3DList.end(); iter++) {
			if ((*iter)->getDistance(x, y, z) < (*closestBlob)->getDistance(x, y, z)) closestBlob = iter;
		}

		TuioBlob3D *freeBlob = (*closestBlob);
		blobID = (*closestBlob)->getBlobID();
		freeBlob3DList.erase(closestBlob);
		delete freeBlob;
	}
	else maxBlob3DID = blobID;

	TuioBlob3D *tblb = new TuioBlob3D(currentFrameTime, sessionID, blobID, x, y, z, roll, pitch, yaw, width, height, depth, volume);
	blob3DList.push_back(tblb);
	updateBlob3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioBlob3D(tblb);

	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "add 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getRoll() << " " << tblb->getPitch() << " " << tblb->getYaw() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getDepth() << " " << tblb->getVolume() << std::endl;

	return tblb;
}

void TuioManager::addExternalTuioBlob3D(TuioBlob3D *tblb) {
	if (tblb == NULL) return;

	int blobID = (int)blob3DList.size();
	if (blobID <= maxBlob3DID) {
		std::list<TuioBlob3D*>::iterator closestBlob = freeBlob3DList.begin();

		for (std::list<TuioBlob3D*>::iterator iter = freeBlob3DList.begin(); iter != freeBlob3DList.end(); iter++) {
			if ((*iter)->getDistance(tblb->getX(), tblb->getY(), tblb->getZ()) < (*closestBlob)->getDistance(tblb->getX(), tblb->getY(), tblb->getY())) closestBlob = iter;
		}

		TuioBlob3D *freeBlob = (*closestBlob);
		blobID = (*closestBlob)->getBlobID();
		freeBlob3DList.erase(closestBlob);
		delete freeBlob;
	}
	else maxBlob3DID = blobID;

	tblb->setSessionID(++sessionID);
	tblb->setBlobID(blobID);

	blob3DList.push_back(tblb);
	updateBlob3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->addTuioBlob3D(tblb);

	if (verbose)
		std::cout << "add 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getRoll() << " " << tblb->getPitch() << " " << tblb->getYaw() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getDepth() << " " << tblb->getVolume() << std::endl;
}

void TuioManager::updateTuioBlob3D(TuioBlob3D *tblb, float x, float y, float z, float roll, float pitch, float yaw, float width, float height, float depth, float volume) {
	if (tblb == NULL) return;
	if (tblb->getTuioTime() == currentFrameTime) return;
	tblb->update(currentFrameTime, x, y, z, roll,pitch,yaw,width,height,depth,volume);
	updateBlob3D = true;

	if (tblb->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioBlob3D(tblb);

		if (verbose)
			std::cout << "set 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getRoll() << " " << tblb->getPitch() << " " << tblb->getYaw() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getDepth() << " " << tblb->getVolume()
			<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRollSpeed() << " " << tblb->getPitchSpeed() << " " << tblb->getYawSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << " " << std::endl;
	}
}

void TuioManager::updateExternalTuioBlob3D(TuioBlob3D *tblb) {
	if (tblb == NULL) return;
	updateBlob3D = true;

	if (tblb->isMoving()) {
		for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
			(*listener)->updateTuioBlob3D(tblb);

		if (verbose)
			std::cout << "set 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getRoll() << " " << tblb->getPitch() << " " << tblb->getYaw() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getDepth() << " " << tblb->getVolume()
			<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRollSpeed() << " " << tblb->getPitchSpeed() << " " << tblb->getYawSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << " " << std::endl;
	}
}

void TuioManager::removeTuioBlob3D(TuioBlob3D *tblb) {
	if (tblb == NULL) return;

	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ")" << std::endl;

	blob3DList.remove(tblb);
	tblb->remove(currentFrameTime);
	updateBlob3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioBlob3D(tblb);

	if (tblb->getBlobID() == maxBlob3DID) {
		maxBlob3DID = -1;
		delete tblb;

		if (blob3DList.size() > 0) {
			std::list<TuioBlob3D*>::iterator clist;
			for (clist = blob3DList.begin(); clist != blob3DList.end(); clist++) {
				int blobID = (*clist)->getBlobID();
				if (blobID > maxBlob3DID) maxBlob3DID = blobID;
			}

			freeBlob3DBuffer.clear();
			for (std::list<TuioBlob3D*>::iterator flist = freeBlob3DList.begin(); flist != freeBlob3DList.end(); flist++) {
				TuioBlob3D *freeBlob = (*flist);
				if (freeBlob->getBlobID() > maxBlob3DID) delete freeBlob;
				else freeBlob3DBuffer.push_back(freeBlob);
			}

			freeBlob3DList = freeBlob3DBuffer;

		}
		else {
			for (std::list<TuioBlob3D*>::iterator flist = freeBlob3DList.begin(); flist != freeBlob3DList.end(); flist++) {
				TuioBlob3D *freeBlob = (*flist);
				delete freeBlob;
			}
			freeBlob3DList.clear();
		}
	}
	else if (tblb->getBlobID() < maxBlob3DID) {
		freeBlob3DList.push_back(tblb);
	}

}

void TuioManager::removeExternalTuioBlob3D(TuioBlob3D *tblb) {
	if (tblb == NULL) return;

	if (verbose /*&& tblb->getTuioState()!=TUIO_ADDED*/)
		std::cout << "del 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ")" << std::endl;

	blob3DList.remove(tblb);
	updateBlob3D = true;

	for (std::list<TuioListener*>::iterator listener = listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->removeTuioBlob3D(tblb);
}






















long TuioManager::getSessionID() {
	sessionID++;
	return sessionID;
}

long TuioManager::getFrameID() {
	return currentFrame;
}

TuioTime TuioManager::getFrameTime() {
	return currentFrameTime;
}

void TuioManager::initFrame(TuioTime ttime) {
	currentFrameTime = TuioTime(ttime);
	currentFrame++;
}

void TuioManager::commitFrame() {
	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->refresh(currentFrameTime);
}









TuioObject* TuioManager::getClosestTuioObject(float xp, float yp) {
	
	TuioObject *closestObject = NULL;
	float closestDistance = 1.0f;
	
	for (std::list<TuioObject*>::iterator iter=objectList.begin(); iter != objectList.end(); iter++) {
		float distance = (*iter)->getDistance(xp,yp,0);
		if(distance<closestDistance) {
			closestObject = (*iter);
			closestDistance = distance;
		}
	}
	
	return closestObject;
}

TuioCursor* TuioManager::getClosestTuioCursor(float xp, float yp) {

	TuioCursor *closestCursor = NULL;
	float closestDistance = 1.0f;

	for (std::list<TuioCursor*>::iterator iter=cursorList.begin(); iter != cursorList.end(); iter++) {
		float distance = (*iter)->getDistance(xp,yp,0);
		if(distance<closestDistance) {
			closestCursor = (*iter);
			closestDistance = distance;
		}
	}
	
	return closestCursor;
}

TuioBlob* TuioManager::getClosestTuioBlob(float xp, float yp) {
	
	TuioBlob *closestBlob = NULL;
	float closestDistance = 1.0f;
	
	for (std::list<TuioBlob*>::iterator iter=blobList.begin(); iter != blobList.end(); iter++) {
		float distance = (*iter)->getDistance(xp,yp,0);
		if(distance<closestDistance) {
			closestBlob = (*iter);
			closestDistance = distance;
		}
	}
	
	return closestBlob;
}

std::list<TuioObject*> TuioManager::getUntouchedObjects() {
	
	std::list<TuioObject*> untouched;
	for (std::list<TuioObject*>::iterator tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++) {
		TuioObject *tobj = (*tuioObject);
		if (tobj->getTuioTime()!=currentFrameTime) untouched.push_back(tobj);
	}	
	return untouched;
}

void TuioManager::stopUntouchedMovingObjects() {
	
	std::list<TuioObject*> untouched;
	for (std::list<TuioObject*>::iterator tuioObject = objectList.begin(); tuioObject!=objectList.end(); tuioObject++) {
		
		TuioObject *tobj = (*tuioObject);
		if ((tobj->getTuioTime()!=currentFrameTime) && (tobj->isMoving())) {
			tobj->stop(currentFrameTime);
			updateObject = true;
			if (verbose)		
				std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() 
				<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
		}
	}
}

void TuioManager::removeUntouchedStoppedObjects() {
	
	std::list<TuioObject*>::iterator tuioObject = objectList.begin();
	while (tuioObject!=objectList.end()) {
		TuioObject *tobj = (*tuioObject);
		if ((tobj->getTuioTime()!=currentFrameTime) && (!tobj->isMoving())) {
			removeTuioObject(tobj);
			tuioObject = objectList.begin();
		} else tuioObject++;
	}
}

void TuioManager::resetTuioObjects() {
	
	std::list<TuioObject*>::iterator tuioObject = objectList.begin();
	while (tuioObject!=objectList.end()) {
		removeTuioObject((*tuioObject));
		tuioObject = objectList.begin();
	}
}

std::list<TuioCursor*> TuioManager::getUntouchedCursors() {
	
	std::list<TuioCursor*> untouched;
	for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++) {
		TuioCursor *tcur = (*tuioCursor);
		if (tcur->getTuioTime()!=currentFrameTime) untouched.push_back(tcur);
	}	
	return untouched;
}

void TuioManager::stopUntouchedMovingCursors() {
	
	std::list<TuioCursor*> untouched;
	for (std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin(); tuioCursor!=cursorList.end(); tuioCursor++) {
		TuioCursor *tcur = (*tuioCursor);
		if ((tcur->getTuioTime()!=currentFrameTime) && (tcur->isMoving())) {
			tcur->stop(currentFrameTime);
			updateCursor = true;
			if (verbose) 	
				std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() 
				<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed()<< " " << tcur->getMotionAccel() << " " << std::endl;							
		}
	}	
}

void TuioManager::removeUntouchedStoppedCursors() {
	
	if (cursorList.size()==0) return;
	std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin();
	while (tuioCursor!=cursorList.end()) {
		TuioCursor *tcur = (*tuioCursor);
		if ((tcur->getTuioTime()!=currentFrameTime) && (!tcur->isMoving())) {
			removeTuioCursor(tcur);
			tuioCursor = cursorList.begin();
		} else tuioCursor++;
	}	
}

void TuioManager::resetTuioCursors() {
	
	std::list<TuioCursor*>::iterator tuioCursor = cursorList.begin();
	while (tuioCursor!=cursorList.end()) {
		removeTuioCursor((*tuioCursor));
		tuioCursor = cursorList.begin();
	}
}

std::list<TuioBlob*> TuioManager::getUntouchedBlobs() {
	
	std::list<TuioBlob*> untouched;
	for (std::list<TuioBlob*>::iterator tuioBlob = blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++) {
		TuioBlob *tblb = (*tuioBlob);
		if (tblb->getTuioTime()!=currentFrameTime) untouched.push_back(tblb);
	}	
	return untouched;
}

void TuioManager::stopUntouchedMovingBlobs() {
	
	std::list<TuioBlob*> untouched;
	for (std::list<TuioBlob*>::iterator tuioBlob = blobList.begin(); tuioBlob!=blobList.end(); tuioBlob++) {
		TuioBlob *tblb = (*tuioBlob);
		if ((tblb->getTuioTime()!=currentFrameTime) && (tblb->isMoving())) {
			tblb->stop(currentFrameTime);
			updateBlob = true;
			if (verbose) 	
				std::cout << "set blb " << tblb->getBlobID() << " (" <<  tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY()  << " " << tblb->getAngle() << " " << tblb->getWidth()  << " " << tblb->getHeight() << " " << tblb->getArea()
				<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed()  << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel()<< " " << tblb->getRotationAccel() << " " << std::endl;
		}
	}	
}

void TuioManager::removeUntouchedStoppedBlobs() {
	
	std::list<TuioBlob*>::iterator tuioBlob = blobList.begin();
	while (tuioBlob!=blobList.end()) {
		TuioBlob *tblb = (*tuioBlob);
		if ((tblb->getTuioTime()!=currentFrameTime) && (!tblb->isMoving())) {
			removeTuioBlob(tblb);
			tuioBlob = blobList.begin();
		} else tuioBlob++;
	}	
}

void TuioManager::resetTuioBlobs() {
	
	std::list<TuioBlob*>::iterator tuioBlob = blobList.begin();
	while (tuioBlob!=blobList.end()) {
		removeTuioBlob((*tuioBlob));
		tuioBlob = blobList.begin();
	}
}

















TuioObject25D* TuioManager::getClosestTuioObject25D(float xp, float yp,float zp) {

	TuioObject25D *closestObject = NULL;
	float closestDistance = 1.0f;

	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++) {
		float distance = (*iter)->getDistance(xp, yp, zp);
		if (distance < closestDistance) {
			closestObject = (*iter);
			closestDistance = distance;
		}
	}

	return closestObject;
}

TuioCursor25D* TuioManager::getClosestTuioCursor25D(float xp, float yp, float zp) {

	TuioCursor25D *closestCursor = NULL;
	float closestDistance = 1.0f;

	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
		float distance = (*iter)->getDistance(xp, yp, zp);
		if (distance < closestDistance) {
			closestCursor = (*iter);
			closestDistance = distance;
		}
	}

	return closestCursor;
}

TuioBlob25D* TuioManager::getClosestTuioBlob25D(float xp, float yp,float zp) {

	TuioBlob25D *closestBlob = NULL;
	float closestDistance = 1.0f;

	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
		float distance = (*iter)->getDistance(xp, yp, zp);
		if (distance < closestDistance) {
			closestBlob = (*iter);
			closestDistance = distance;
		}
	}

	return closestBlob;
}

std::list<TuioObject25D*> TuioManager::getUntouchedObjects25D() {

	std::list<TuioObject25D*> untouched;
	for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++) {
		TuioObject25D *tobj = (*tuioObject);
		if (tobj->getTuioTime() != currentFrameTime) untouched.push_back(tobj);
	}
	return untouched;
}

void TuioManager::stopUntouchedMovingObjects25D() {

	std::list<TuioObject25D*> untouched;
	for (std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin(); tuioObject != object25DList.end(); tuioObject++) {

		TuioObject25D *tobj = (*tuioObject);
		if ((tobj->getTuioTime() != currentFrameTime) && (tobj->isMoving())) {
			tobj->stop(currentFrameTime);
			updateObject25D = true;
			if (verbose)
				std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getAngle()
				<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
		}
	}
}

void TuioManager::removeUntouchedStoppedObjects25D() {

	std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin();
	while (tuioObject != object25DList.end()) {
		TuioObject25D *tobj = (*tuioObject);
		if ((tobj->getTuioTime() != currentFrameTime) && (!tobj->isMoving())) {
			removeTuioObject25D(tobj);
			tuioObject = object25DList.begin();
		}
		else tuioObject++;
	}
}

void TuioManager::resetTuioObjects25D() {

	std::list<TuioObject25D*>::iterator tuioObject = object25DList.begin();
	while (tuioObject != object25DList.end()) {
		removeTuioObject25D((*tuioObject));
		tuioObject = object25DList.begin();
	}
}

std::list<TuioCursor25D*> TuioManager::getUntouchedCursors25D() {

	std::list<TuioCursor25D*> untouched;
	for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++) {
		TuioCursor25D *tcur = (*tuioCursor);
		if (tcur->getTuioTime() != currentFrameTime) untouched.push_back(tcur);
	}
	return untouched;
}

void TuioManager::stopUntouchedMovingCursors25D() {

	std::list<TuioCursor25D*> untouched;
	for (std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin(); tuioCursor != cursor25DList.end(); tuioCursor++) {
		TuioCursor25D *tcur = (*tuioCursor);
		if ((tcur->getTuioTime() != currentFrameTime) && (tcur->isMoving())) {
			tcur->stop(currentFrameTime);
			updateCursor25D = true;
			if (verbose)
				std::cout << "set cur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
				<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
		}
	}
}

void TuioManager::removeUntouchedStoppedCursors25D() {

	if (cursor25DList.size() == 0) return;
	std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin();
	while (tuioCursor != cursor25DList.end()) {
		TuioCursor25D *tcur = (*tuioCursor);
		if ((tcur->getTuioTime() != currentFrameTime) && (!tcur->isMoving())) {
			removeTuioCursor25D(tcur);
			tuioCursor = cursor25DList.begin();
		}
		else tuioCursor++;
	}
}

void TuioManager::resetTuioCursors25D() {

	std::list<TuioCursor25D*>::iterator tuioCursor = cursor25DList.begin();
	while (tuioCursor != cursor25DList.end()) {
		removeTuioCursor25D((*tuioCursor));
		tuioCursor = cursor25DList.begin();
	}
}

std::list<TuioBlob25D*> TuioManager::getUntouchedBlobs25D() {

	std::list<TuioBlob25D*> untouched;
	for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++) {
		TuioBlob25D *tblb = (*tuioBlob);
		if (tblb->getTuioTime() != currentFrameTime) untouched.push_back(tblb);
	}
	return untouched;
}

void TuioManager::stopUntouchedMovingBlobs25D() {

	std::list<TuioBlob25D*> untouched;
	for (std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin(); tuioBlob != blob25DList.end(); tuioBlob++) {
		TuioBlob25D *tblb = (*tuioBlob);
		if ((tblb->getTuioTime() != currentFrameTime) && (tblb->isMoving())) {
			tblb->stop(currentFrameTime);
			updateBlob25D = true;
			if (verbose)
				std::cout << "set blb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea()
				<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << " " << std::endl;
		}
	}
}

void TuioManager::removeUntouchedStoppedBlobs25D() {

	std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin();
	while (tuioBlob != blob25DList.end()) {
		TuioBlob25D *tblb = (*tuioBlob);
		if ((tblb->getTuioTime() != currentFrameTime) && (!tblb->isMoving())) {
			removeTuioBlob25D(tblb);
			tuioBlob = blob25DList.begin();
		}
		else tuioBlob++;
	}
}

void TuioManager::resetTuioBlobs25D() {

	std::list<TuioBlob25D*>::iterator tuioBlob = blob25DList.begin();
	while (tuioBlob != blob25DList.end()) {
		removeTuioBlob25D((*tuioBlob));
		tuioBlob = blob25DList.begin();
	}
}












TuioObject3D* TuioManager::getClosestTuioObject3D(float xp, float yp, float zp) {

	TuioObject3D *closestObject = NULL;
	float closestDistance = 1.0f;

	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++) {
		float distance = (*iter)->getDistance(xp, yp, zp);
		if (distance < closestDistance) {
			closestObject = (*iter);
			closestDistance = distance;
		}
	}

	return closestObject;
}

TuioCursor3D* TuioManager::getClosestTuioCursor3D(float xp, float yp, float zp) {

	TuioCursor3D *closestCursor = NULL;
	float closestDistance = 1.0f;

	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
		float distance = (*iter)->getDistance(xp, yp, zp);
		if (distance < closestDistance) {
			closestCursor = (*iter);
			closestDistance = distance;
		}
	}

	return closestCursor;
}

TuioBlob3D* TuioManager::getClosestTuioBlob3D(float xp, float yp, float zp) {

	TuioBlob3D *closestBlob = NULL;
	float closestDistance = 1.0f;

	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
		float distance = (*iter)->getDistance(xp, yp, zp);
		if (distance < closestDistance) {
			closestBlob = (*iter);
			closestDistance = distance;
		}
	}

	return closestBlob;
}

std::list<TuioObject3D*> TuioManager::getUntouchedObjects3D() {

	std::list<TuioObject3D*> untouched;
	for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++) {
		TuioObject3D *tobj = (*tuioObject);
		if (tobj->getTuioTime() != currentFrameTime) untouched.push_back(tobj);
	}
	return untouched;
}

void TuioManager::stopUntouchedMovingObjects3D() {

	std::list<TuioObject3D*> untouched;
	for (std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin(); tuioObject != object3DList.end(); tuioObject++) {

		TuioObject3D *tobj = (*tuioObject);
		if ((tobj->getTuioTime() != currentFrameTime) && (tobj->isMoving())) {
			tobj->stop(currentFrameTime);
			updateObject3D = true;
			if (verbose)
				std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getRoll() << " " << tobj->getPitch() << " " << tobj->getYaw()
				<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRollSpeed() << " " << tobj->getPitchSpeed() << " " << tobj->getYawSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
		}
	}
}

void TuioManager::removeUntouchedStoppedObjects3D() {

	std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin();
	while (tuioObject != object3DList.end()) {
		TuioObject3D *tobj = (*tuioObject);
		if ((tobj->getTuioTime() != currentFrameTime) && (!tobj->isMoving())) {
			removeTuioObject3D(tobj);
			tuioObject = object3DList.begin();
		}
		else tuioObject++;
	}
}

void TuioManager::resetTuioObjects3D() {

	std::list<TuioObject3D*>::iterator tuioObject = object3DList.begin();
	while (tuioObject != object3DList.end()) {
		removeTuioObject3D((*tuioObject));
		tuioObject = object3DList.begin();
	}
}

std::list<TuioCursor3D*> TuioManager::getUntouchedCursors3D() {

	std::list<TuioCursor3D*> untouched;
	for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++) {
		TuioCursor3D *tcur = (*tuioCursor);
		if (tcur->getTuioTime() != currentFrameTime) untouched.push_back(tcur);
	}
	return untouched;
}

void TuioManager::stopUntouchedMovingCursors3D() {

	std::list<TuioCursor3D*> untouched;
	for (std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin(); tuioCursor != cursor3DList.end(); tuioCursor++) {
		TuioCursor3D *tcur = (*tuioCursor);
		if ((tcur->getTuioTime() != currentFrameTime) && (tcur->isMoving())) {
			tcur->stop(currentFrameTime);
			updateCursor3D = true;
			if (verbose)
				std::cout << "set cur " << tcur->getCursorID() << " (" << tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
				<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
		}
	}
}

void TuioManager::removeUntouchedStoppedCursors3D() {

	if (cursor3DList.size() == 0) return;
	std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin();
	while (tuioCursor != cursor3DList.end()) {
		TuioCursor3D *tcur = (*tuioCursor);
		if ((tcur->getTuioTime() != currentFrameTime) && (!tcur->isMoving())) {
			removeTuioCursor3D(tcur);
			tuioCursor = cursor3DList.begin();
		}
		else tuioCursor++;
	}
}

void TuioManager::resetTuioCursors3D() {

	std::list<TuioCursor3D*>::iterator tuioCursor = cursor3DList.begin();
	while (tuioCursor != cursor3DList.end()) {
		removeTuioCursor3D((*tuioCursor));
		tuioCursor = cursor3DList.begin();
	}
}

std::list<TuioBlob3D*> TuioManager::getUntouchedBlobs3D() {

	std::list<TuioBlob3D*> untouched;
	for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++) {
		TuioBlob3D *tblb = (*tuioBlob);
		if (tblb->getTuioTime() != currentFrameTime) untouched.push_back(tblb);
	}
	return untouched;
}

void TuioManager::stopUntouchedMovingBlobs3D() {

	std::list<TuioBlob3D*> untouched;
	for (std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin(); tuioBlob != blob3DList.end(); tuioBlob++) {
		TuioBlob3D *tblb = (*tuioBlob);
		if ((tblb->getTuioTime() != currentFrameTime) && (tblb->isMoving())) {
			tblb->stop(currentFrameTime);
			updateBlob3D = true;
			if (verbose)
				std::cout << "set blb " << tblb->getBlobID() << " (" << tblb->getSessionID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getRoll() << " " << tblb->getPitch() << " " << tblb->getYaw() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getDepth() << " " << tblb->getVolume()
				<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRollSpeed() << " " << tblb->getPitchSpeed() << " " << tblb->getYawSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << " " << std::endl;
		}
	}
}

void TuioManager::removeUntouchedStoppedBlobs3D() {

	std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin();
	while (tuioBlob != blob3DList.end()) {
		TuioBlob3D *tblb = (*tuioBlob);
		if ((tblb->getTuioTime() != currentFrameTime) && (!tblb->isMoving())) {
			removeTuioBlob3D(tblb);
			tuioBlob = blob3DList.begin();
		}
		else tuioBlob++;
	}
}

void TuioManager::resetTuioBlobs3D() {

	std::list<TuioBlob3D*>::iterator tuioBlob = blob3DList.begin();
	while (tuioBlob != blob3DList.end()) {
		removeTuioBlob3D((*tuioBlob));
		tuioBlob = blob3DList.begin();
	}
}

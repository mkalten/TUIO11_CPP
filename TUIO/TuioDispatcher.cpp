
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


#include "TuioDispatcher.h"

#include <iostream>
#include <list>
#include <algorithm>
#include <cstring>

using namespace TUIO;

TuioDispatcher::TuioDispatcher() {
#ifdef WIN32
	cursorMutex = CreateMutex(NULL, FALSE, TEXT("cursorMutex"));
	objectMutex = CreateMutex(NULL, FALSE, TEXT("objectMutex"));
	blobMutex = CreateMutex(NULL, FALSE, TEXT("blobMutex"));
	cursor25DMutex = CreateMutex(NULL, FALSE, TEXT("cursor25DMutex"));
	object25DMutex = CreateMutex(NULL, FALSE, TEXT("object25DMutex"));
	blob25DMutex = CreateMutex(NULL, FALSE, TEXT("blob25DMutex"));
	cursor3DMutex = CreateMutex(NULL, FALSE, TEXT("cursor3DMutex"));
	object3DMutex = CreateMutex(NULL, FALSE, TEXT("object3DMutex"));
	blob3DMutex = CreateMutex(NULL, FALSE, TEXT("blob3DMutex"));
#else
	pthread_mutex_init(&cursorMutex, NULL);
	pthread_mutex_init(&objectMutex, NULL);
	pthread_mutex_init(&blobMutex, NULL);
	pthread_mutex_init(&cursor25DMutex, NULL);
	pthread_mutex_init(&object25DMutex, NULL);
	pthread_mutex_init(&blob25DMutex, NULL);
	pthread_mutex_init(&cursor3DMutex, NULL);
	pthread_mutex_init(&object3DMutex, NULL);
	pthread_mutex_init(&blob3DMutex, NULL);

#endif
}

TuioDispatcher::~TuioDispatcher() {
#ifdef WIN32
	CloseHandle(cursorMutex);
	CloseHandle(objectMutex);
	CloseHandle(blobMutex);
	CloseHandle(cursor25DMutex);
	CloseHandle(object25DMutex);
	CloseHandle(blob25DMutex);
	CloseHandle(cursor3DMutex);
	CloseHandle(object3DMutex);
	CloseHandle(blob3DMutex);
#else
	pthread_mutex_destroy(&cursorMutex);
	pthread_mutex_destroy(&objectMutex);
	pthread_mutex_destroy(&blobMutex);
	pthread_mutex_destroy(&cursor25DMutex);
	pthread_mutex_destroy(&object25DMutex);
	pthread_mutex_destroy(&blob25DMutex);
	pthread_mutex_destroy(&cursor3DMutex);
	pthread_mutex_destroy(&object3DMutex);
	pthread_mutex_destroy(&blob3DMutex);
#endif
}

void TuioDispatcher::lockObjectList() {
#ifdef WIN32
	WaitForSingleObject(objectMutex, INFINITE);
#else
	pthread_mutex_lock(&objectMutex);
#endif
}

void TuioDispatcher::unlockObjectList() {
#ifdef WIN32
	ReleaseMutex(objectMutex);
#else
	pthread_mutex_unlock(&objectMutex);
#endif
}

void TuioDispatcher::lockCursorList() {
#ifdef WIN32
	WaitForSingleObject(cursorMutex, INFINITE);
#else
	pthread_mutex_lock(&cursorMutex);
#endif
}

void TuioDispatcher::unlockCursorList() {
#ifdef WIN32
	ReleaseMutex(cursorMutex);
#else
	pthread_mutex_unlock(&cursorMutex);
#endif
}

void TuioDispatcher::lockBlobList() {
#ifdef WIN32
	WaitForSingleObject(blobMutex, INFINITE);
#else
	pthread_mutex_lock(&blobMutex);
#endif
}

void TuioDispatcher::unlockBlobList() {
#ifdef WIN32
	ReleaseMutex(blobMutex);
#else
	pthread_mutex_unlock(&blobMutex);
#endif
}

void TuioDispatcher::addTuioListener(TuioListener *listener) {
	listenerList.push_back(listener);
}

void TuioDispatcher::removeTuioListener(TuioListener *listener) {
	std::list<TuioListener*>::iterator result = find(listenerList.begin(),listenerList.end(),listener);
	if (result!=listenerList.end()) listenerList.remove(listener);
}

void TuioDispatcher::removeAllTuioListeners() {	
	listenerList.clear();
}

TuioObject* TuioDispatcher::getTuioObject(long s_id) {
	lockObjectList();
	for (std::list<TuioObject*>::iterator iter=objectList.begin(); iter != objectList.end(); iter++) {
		if((*iter)->getSessionID()==s_id) {
			unlockObjectList();
			return (*iter);
		}
	}	
	unlockObjectList();
	return NULL;
}

TuioCursor* TuioDispatcher::getTuioCursor(long s_id) {
	lockCursorList();
	for (std::list<TuioCursor*>::iterator iter=cursorList.begin(); iter != cursorList.end(); iter++) {
		if((*iter)->getSessionID()==s_id) {
			unlockCursorList();
			return (*iter);
		}
	}	
	unlockCursorList();
	return NULL;
}

TuioBlob* TuioDispatcher::getTuioBlob(long s_id) {
	lockBlobList();
	for (std::list<TuioBlob*>::iterator iter=blobList.begin(); iter != blobList.end(); iter++) {
		if((*iter)->getSessionID()==s_id) {
			unlockBlobList();
			return (*iter);
		}
	}	
	unlockBlobList();
	return NULL;
}

std::list<TuioObject*> TuioDispatcher::getTuioObjects() {
	lockObjectList();
	std::list<TuioObject*> listBuffer;
	listBuffer.insert(listBuffer.end(), objectList.begin(), objectList.end());
	//std::list<TuioObject*> listBuffer = objectList;
	unlockObjectList();
	return listBuffer;
}

std::list<TuioCursor*> TuioDispatcher::getTuioCursors() {
	lockCursorList();
	std::list<TuioCursor*> listBuffer;
	listBuffer.insert(listBuffer.end(), cursorList.begin(), cursorList.end());
	//std::list<TuioCursor*> listBuffer = cursorList;
	unlockCursorList();
	return listBuffer;
}

std::list<TuioBlob*> TuioDispatcher::getTuioBlobs() {
	lockBlobList();
	std::list<TuioBlob*> listBuffer;
	listBuffer.insert(listBuffer.end(), blobList.begin(), blobList.end());
	//std::list<TuioBlob*> listBuffer = blobList;
	unlockBlobList();
	return listBuffer;
}

int TuioDispatcher::getTuioObjectCount() {
	return objectList.size();
}

int TuioDispatcher::getTuioCursorCount() {
	return cursorList.size();
}

int TuioDispatcher::getTuioBlobCount() {
	return blobList.size();
}

std::list<TuioObject> TuioDispatcher::copyTuioObjects() {
	lockObjectList();
	std::list<TuioObject> listBuffer;
	for (std::list<TuioObject*>::iterator iter=objectList.begin(); iter != objectList.end(); iter++) {
		TuioObject *tobj = (*iter);
		listBuffer.push_back(*tobj);
	}	
	unlockObjectList();
	return listBuffer;
}

std::list<TuioCursor> TuioDispatcher::copyTuioCursors() {
	lockCursorList();
	std::list<TuioCursor> listBuffer;
	for (std::list<TuioCursor*>::iterator iter=cursorList.begin(); iter != cursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		listBuffer.push_back(*tcur);
	}
	unlockCursorList();

	return listBuffer;
}

std::list<TuioBlob> TuioDispatcher::copyTuioBlobs() {
	lockBlobList();
	std::list<TuioBlob> listBuffer;
	for (std::list<TuioBlob*>::iterator iter=blobList.begin(); iter != blobList.end(); iter++) {
		TuioBlob *tblb = (*iter);
		listBuffer.push_back(*tblb);
	}	
	unlockBlobList();
	return listBuffer;
}






void TuioDispatcher::lockObject25DList() {
#ifdef WIN32
	WaitForSingleObject(object25DMutex, INFINITE);
#else
	pthread_mutex_lock(&object25DMutex);
#endif
}

void TuioDispatcher::unlockObject25DList() {
#ifdef WIN32
	ReleaseMutex(object25DMutex);
#else
	pthread_mutex_unlock(&object25DMutex);
#endif
}

void TuioDispatcher::lockCursor25DList() {
#ifdef WIN32
	WaitForSingleObject(cursor25DMutex, INFINITE);
#else
	pthread_mutex_lock(&cursorMutex25D);
#endif
}

void TuioDispatcher::unlockCursor25DList() {
#ifdef WIN32
	ReleaseMutex(cursor25DMutex);
#else
	pthread_mutex_unlock(&cursor25DMutex);
#endif
}

void TuioDispatcher::lockBlob25DList() {
#ifdef WIN32
	WaitForSingleObject(blob25DMutex, INFINITE);
#else
	pthread_mutex_lock(&blob25DMutex);
#endif
}

void TuioDispatcher::unlockBlob25DList() {
#ifdef WIN32
	ReleaseMutex(blob25DMutex);
#else
	pthread_mutex_unlock(&blob25DMutex);
#endif
}

TuioObject25D* TuioDispatcher::getTuioObject25D(long s_id) {
	lockObject25DList();
	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++) {
		if ((*iter)->getSessionID() == s_id) {
			unlockObject25DList();
			return (*iter);
		}
	}
	unlockObject25DList();
	return NULL;
}

TuioCursor25D* TuioDispatcher::getTuioCursor25D(long s_id) {
	lockCursor25DList();
	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
		if ((*iter)->getSessionID() == s_id) {
			unlockCursor25DList();
			return (*iter);
		}
	}
	unlockCursor25DList();
	return NULL;
}

TuioBlob25D* TuioDispatcher::getTuioBlob25D(long s_id) {
	lockBlob25DList();
	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
		if ((*iter)->getSessionID() == s_id) {
			unlockBlob25DList();
			return (*iter);
		}
	}
	unlockBlob25DList();
	return NULL;
}

std::list<TuioObject25D*> TuioDispatcher::getTuioObjects25D() {
	lockObject25DList();
	std::list<TuioObject25D*> listBuffer;
	listBuffer.insert(listBuffer.end(), object25DList.begin(), object25DList.end());
	//std::list<TuioObject25D*> listBuffer = objectList;
	unlockObject25DList();
	return listBuffer;
}

std::list<TuioCursor25D*> TuioDispatcher::getTuioCursors25D() {
	lockCursor25DList();
	std::list<TuioCursor25D*> listBuffer;
	listBuffer.insert(listBuffer.end(), cursor25DList.begin(), cursor25DList.end());
	//std::list<TuioCursor25D*> listBuffer = cursorList;
	unlockCursor25DList();
	return listBuffer;
}

std::list<TuioBlob25D*> TuioDispatcher::getTuioBlobs25D() {
	lockBlob25DList();
	std::list<TuioBlob25D*> listBuffer;
	listBuffer.insert(listBuffer.end(), blob25DList.begin(), blob25DList.end());
	//std::list<TuioBlob25D*> listBuffer = blobList;
	unlockBlob25DList();
	return listBuffer;
}

int TuioDispatcher::getTuioObject25DCount() {
	return object25DList.size();
}

int TuioDispatcher::getTuioCursor25DCount() {
	return cursor25DList.size();
}

int TuioDispatcher::getTuioBlob25DCount() {
	return blob25DList.size();
}

std::list<TuioObject25D> TuioDispatcher::copyTuioObjects25D() {
	lockObject25DList();
	std::list<TuioObject25D> listBuffer;
	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++) {
		TuioObject25D *tobj = (*iter);
		listBuffer.push_back(*tobj);
	}
	unlockObject25DList();
	return listBuffer;
}

std::list<TuioCursor25D> TuioDispatcher::copyTuioCursors25D() {
	lockCursor25DList();
	std::list<TuioCursor25D> listBuffer;
	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
		TuioCursor25D *tcur = (*iter);
		listBuffer.push_back(*tcur);
	}
	unlockCursor25DList();

	return listBuffer;
}

std::list<TuioBlob25D> TuioDispatcher::copyTuioBlobs25D() {
	lockBlob25DList();
	std::list<TuioBlob25D> listBuffer;
	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
		TuioBlob25D *tblb = (*iter);
		listBuffer.push_back(*tblb);
	}
	unlockBlob25DList();
	return listBuffer;
}







void TuioDispatcher::lockObject3DList() {
#ifdef WIN32
	WaitForSingleObject(object3DMutex, INFINITE);
#else
	pthread_mutex_lock(&object3DMutex);
#endif
}

void TuioDispatcher::unlockObject3DList() {
#ifdef WIN32
	ReleaseMutex(object3DMutex);
#else
	pthread_mutex_unlock(&object3DMutex);
#endif
}

void TuioDispatcher::lockCursor3DList() {
#ifdef WIN32
	WaitForSingleObject(cursor3DMutex, INFINITE);
#else
	pthread_mutex_lock(&cursorMutex3D);
#endif
}

void TuioDispatcher::unlockCursor3DList() {
#ifdef WIN32
	ReleaseMutex(cursor3DMutex);
#else
	pthread_mutex_unlock(&cursor3DMutex);
#endif
}

void TuioDispatcher::lockBlob3DList() {
#ifdef WIN32
	WaitForSingleObject(blob3DMutex, INFINITE);
#else
	pthread_mutex_lock(&blob3DMutex);
#endif
}

void TuioDispatcher::unlockBlob3DList() {
#ifdef WIN32
	ReleaseMutex(blob3DMutex);
#else
	pthread_mutex_unlock(&blob3DMutex);
#endif
}

TuioObject3D* TuioDispatcher::getTuioObject3D(long s_id) {
	lockObject3DList();
	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++) {
		if ((*iter)->getSessionID() == s_id) {
			unlockObject3DList();
			return (*iter);
		}
	}
	unlockObject3DList();
	return NULL;
}

TuioCursor3D* TuioDispatcher::getTuioCursor3D(long s_id) {
	lockCursor3DList();
	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
		if ((*iter)->getSessionID() == s_id) {
			unlockCursor3DList();
			return (*iter);
		}
	}
	unlockCursor3DList();
	return NULL;
}

TuioBlob3D* TuioDispatcher::getTuioBlob3D(long s_id) {
	lockBlob3DList();
	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
		if ((*iter)->getSessionID() == s_id) {
			unlockBlob3DList();
			return (*iter);
		}
	}
	unlockBlob3DList();
	return NULL;
}

std::list<TuioObject3D*> TuioDispatcher::getTuioObjects3D() {
	lockObject3DList();
	std::list<TuioObject3D*> listBuffer;
	listBuffer.insert(listBuffer.end(), object3DList.begin(), object3DList.end());
	//std::list<TuioObject3D*> listBuffer = objectList;
	unlockObject3DList();
	return listBuffer;
}

std::list<TuioCursor3D*> TuioDispatcher::getTuioCursors3D() {
	lockCursor3DList();
	std::list<TuioCursor3D*> listBuffer;
	listBuffer.insert(listBuffer.end(), cursor3DList.begin(), cursor3DList.end());
	//std::list<TuioCursor3D*> listBuffer = cursorList;
	unlockCursor3DList();
	return listBuffer;
}

std::list<TuioBlob3D*> TuioDispatcher::getTuioBlobs3D() {
	lockBlob3DList();
	std::list<TuioBlob3D*> listBuffer;
	listBuffer.insert(listBuffer.end(), blob3DList.begin(), blob3DList.end());
	//std::list<TuioBlob3D*> listBuffer = blobList;
	unlockBlob3DList();
	return listBuffer;
}

int TuioDispatcher::getTuioObject3DCount() {
	return object3DList.size();
}

int TuioDispatcher::getTuioCursor3DCount() {
	return cursor3DList.size();
}

int TuioDispatcher::getTuioBlob3DCount() {
	return blob3DList.size();
}

std::list<TuioObject3D> TuioDispatcher::copyTuioObjects3D() {
	lockObject3DList();
	std::list<TuioObject3D> listBuffer;
	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++) {
		TuioObject3D *tobj = (*iter);
		listBuffer.push_back(*tobj);
	}
	unlockObject3DList();
	return listBuffer;
}

std::list<TuioCursor3D> TuioDispatcher::copyTuioCursors3D() {
	lockCursor3DList();
	std::list<TuioCursor3D> listBuffer;
	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
		TuioCursor3D *tcur = (*iter);
		listBuffer.push_back(*tcur);
	}
	unlockCursor3DList();

	return listBuffer;
}

std::list<TuioBlob3D> TuioDispatcher::copyTuioBlobs3D() {
	lockBlob3DList();
	std::list<TuioBlob3D> listBuffer;
	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
		TuioBlob3D *tblb = (*iter);
		listBuffer.push_back(*tblb);
	}
	unlockBlob3DList();
	return listBuffer;
}


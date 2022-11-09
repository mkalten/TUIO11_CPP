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

#ifndef INCLUDED_TUIODISPATCHER_H
#define INCLUDED_TUIODISPATCHER_H

#include "TuioListener.h"

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace TUIO {
	
	/**
	 * <p>The TuioDispatcher generates TUIO events which are broadcasted to all 
	 * registered classes that implement the {@link TuioListener} interface.</p> 
	 *
	 * @author Martin Kaltenbrunner
	 * @version 1.1.6
	 */ 
	class LIBDECL TuioDispatcher { 
		
	public:
		/**
		 * This constructor creates a TuioDispatcher
		 *
		 */
		TuioDispatcher();

		/**
		 * The destructor is doing nothing in particular. 
		 */
		~TuioDispatcher();
				
		/**
		 * Adds the provided TuioListener to the list of registered TUIO event listeners
		 *
		 * @param  listener  the TuioListener to add
		 */
		void addTuioListener(TuioListener *listener);

		/**
		 * Removes the provided TuioListener from the list of registered TUIO event listeners
		 *
		 * @param  listener  the TuioListener to remove
		 */
		void removeTuioListener(TuioListener *listener);

		/**
		 * Removes all TuioListener from the list of registered TUIO event listeners
		 */
		void removeAllTuioListeners();
		
#pragma region region2D
		/**
		 * Returns a List of all currently active TuioObjects
		 *
		 * @return  a List of all currently active TuioObjects
		 */
		std::list<TuioObject*> getTuioObjects();

		/**
		* Returns the number of all currently active TuioObjects
		*
		* @return  the of all currently active TuioObjects
		*/
		int getTuioObjectCount();

		/**
		 * Returns a List with a copy of currently active TuioObjects
		 *
		 * @return  a List with a copy of all currently active TuioObjects
		 */
		std::list<TuioObject> copyTuioObjects();
		
		/**
		 * Returns a List of all currently active TuioCursors
		 *
		 * @return  a List of all currently active TuioCursors
		 */
		std::list<TuioCursor*> getTuioCursors();

		/**
		* Returns the number of all currently active TuioCursors
		*
		* @return  the of all currently active TuioCursors
		*/
		int getTuioCursorCount();

		/**
		 * Returns a List with a copy of currently active TuioCursors
		 *
		 * @return  a List with a copy of all currently active TuioCursors
		 */
		std::list<TuioCursor> copyTuioCursors();
		
		/**
		 * Returns a List of all currently active TuioBlobs
		 *
		 * @return  a List of all currently active TuioBlobs
		 */
		std::list<TuioBlob*> getTuioBlobs();

		/**
		* Returns the number of all currently active TuioBlobs
		*
		* @return  the of all currently active TuioBlobs
		*/
		int getTuioBlobCount();

		/**
		 * Returns a List with a copy of currently active TuioBlobs
		 *
		 * @return  a List with a copy of all currently active TuioBlobs
		 */
		std::list<TuioBlob> copyTuioBlobs();
		
		/**
		 * Returns the TuioObject corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioObject
		 *
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		TuioObject* getTuioObject(long s_id);

		/**
		 * Returns the TuioCursor corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioCursor
		 *
		 * @return  an active TuioCursor corresponding to the provided Session ID or NULL
		 */
		TuioCursor* getTuioCursor(long s_id);

		/**
		 * Returns the TuioBlob corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBlob
		 *
		 * @return  an active TuioBlob corresponding to the provided Session ID or NULL
		 */
		TuioBlob* getTuioBlob(long s_id);
		
		/**
		 * Locks the TuioObject list in order to avoid updates during access
		 */
		void lockObjectList();

		/**
		 * Releases the lock of the TuioObject list
		 */
		void unlockObjectList();

		/**
		 * Locks the TuioCursor list in order to avoid updates during access
		 */
		void lockCursorList();

		/**
		 * Releases the lock of the TuioCursor list
		 */
		void unlockCursorList();

		/**
		 * Locks the TuioBlob list in order to avoid updates during access
		 */
		void lockBlobList();
		
		/**
		 * Releases the lock of the TuioBlob list
		 */
		void unlockBlobList();
#pragma endregion 

#pragma region region25D
		/**
		 * Returns a List of all currently active TuioObjects25D
		 *
		 * @return  a List of all currently active TuioObjects25D
		 */
		std::list<TuioObject25D*> getTuioObjects25D();

		/**
		* Returns the number of all currently active TuioObjects25D
		*
		* @return  the of all currently active TuioObjects25D
		*/
		int getTuioObject25DCount();

		/**
		 * Returns a List with a copy of currently active TuioObjects25D
		 *
		 * @return  a List with a copy of all currently active TuioObjects25D
		 */
		std::list<TuioObject25D> copyTuioObjects25D();

		/**
		 * Returns a List of all currently active TuioCursors25D
		 *
		 * @return  a List of all currently active TuioCursors25D
		 */
		std::list<TuioCursor25D*> getTuioCursors25D();

		/**
		* Returns the number of all currently active TuioCursors25D
		*
		* @return  the of all currently active TuioCursors25D
		*/
		int getTuioCursor25DCount();

		/**
		 * Returns a List with a copy of currently active TuioCursors25D
		 *
		 * @return  a List with a copy of all currently active TuioCursors25D
		 */
		std::list<TuioCursor25D> copyTuioCursors25D();

		/**
		 * Returns a List of all currently active TuioBlobs25D
		 *
		 * @return  a List of all currently active TuioBlobs25D
		 */
		std::list<TuioBlob25D*> getTuioBlobs25D();

		/**
		* Returns the number of all currently active TuioBlobs25D
		*
		* @return  the of all currently active TuioBlobs25D
		*/
		int getTuioBlob25DCount();

		/**
		 * Returns a List with a copy of currently active TuioBlobs25D
		 *
		 * @return  a List with a copy of all currently active TuioBlobs25D
		 */
		std::list<TuioBlob25D> copyTuioBlobs25D();

		/**
		 * Returns the TuioObject corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioObject
		 *
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		TuioObject25D* getTuioObject25D(long s_id);

		/**
		 * Returns the TuioCursor25D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioCursor25D
		 *
		 * @return  an active TuioCursor25D corresponding to the provided Session ID or NULL
		 */
		TuioCursor25D* getTuioCursor25D(long s_id);

		/**
		 * Returns the TuioBlob25D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBlob25D
		 *
		 * @return  an active TuioBlob25D corresponding to the provided Session ID or NULL
		 */
		TuioBlob25D* getTuioBlob25D(long s_id);

		/**
		 * Locks the TuioObject25D list in order to avoid updates during access
		 */
		void lockObject25DList();

		/**
		 * Releases the lock of the TuioObject list
		 */
		void unlockObject25DList();

		/**
		 * Locks the TuioCursor25D list in order to avoid updates during access
		 */
		void lockCursor25DList();

		/**
		 * Releases the lock of the TuioCursor25D list
		 */
		void unlockCursor25DList();

		/**
		 * Locks the TuioBlob25D list in order to avoid updates during access
		 */
		void lockBlob25DList();

		/**
		 * Releases the lock of the TuioBlob25D list
		 */
		void unlockBlob25DList();
#pragma endregion 


#pragma region region3D
		/**
		 * Returns a List of all currently active TuioObjects3D
		 *
		 * @return  a List of all currently active TuioObjects3D
		 */
		std::list<TuioObject3D*> getTuioObjects3D();

		/**
		* Returns the number of all currently active TuioObjects3D
		*
		* @return  the of all currently active TuioObjects3D
		*/
		int getTuioObject3DCount();

		/**
		 * Returns a List with a copy of currently active TuioObjects3D
		 *
		 * @return  a List with a copy of all currently active TuioObjects3D
		 */
		std::list<TuioObject3D> copyTuioObjects3D();

		/**
		 * Returns a List of all currently active TuioCursors3D
		 *
		 * @return  a List of all currently active TuioCursors3D
		 */
		std::list<TuioCursor3D*> getTuioCursors3D();

		/**
		* Returns the number of all currently active TuioCursors3D
		*
		* @return  the of all currently active TuioCursors3D
		*/
		int getTuioCursor3DCount();

		/**
		 * Returns a List with a copy of currently active TuioCursors3D
		 *
		 * @return  a List with a copy of all currently active TuioCursors3D
		 */
		std::list<TuioCursor3D> copyTuioCursors3D();

		/**
		 * Returns a List of all currently active TuioBlobs3D
		 *
		 * @return  a List of all currently active TuioBlobs3D
		 */
		std::list<TuioBlob3D*> getTuioBlobs3D();

		/**
		* Returns the number of all currently active TuioBlobs3D
		*
		* @return  the of all currently active TuioBlobs3D
		*/
		int getTuioBlob3DCount();

		/**
		 * Returns a List with a copy of currently active TuioBlobs3D
		 *
		 * @return  a List with a copy of all currently active TuioBlobs3D
		 */
		std::list<TuioBlob3D> copyTuioBlobs3D();

		/**
		 * Returns the TuioObject corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioObject
		 *
		 * @return  an active TuioObject corresponding to the provided Session ID or NULL
		 */
		TuioObject3D* getTuioObject3D(long s_id);

		/**
		 * Returns the TuioCursor3D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioCursor3D
		 *
		 * @return  an active TuioCursor3D corresponding to the provided Session ID or NULL
		 */
		TuioCursor3D* getTuioCursor3D(long s_id);

		/**
		 * Returns the TuioBlob3D corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBlob3D
		 *
		 * @return  an active TuioBlob3D corresponding to the provided Session ID or NULL
		 */
		TuioBlob3D* getTuioBlob3D(long s_id);

		/**
		 * Locks the TuioObject3D list in order to avoid updates during access
		 */
		void lockObject3DList();

		/**
		 * Releases the lock of the TuioObject list
		 */
		void unlockObject3DList();

		/**
		 * Locks the TuioCursor3D list in order to avoid updates during access
		 */
		void lockCursor3DList();

		/**
		 * Releases the lock of the TuioCursor3D list
		 */
		void unlockCursor3DList();

		/**
		 * Locks the TuioBlob3D list in order to avoid updates during access
		 */
		void lockBlob3DList();

		/**
		 * Releases the lock of the TuioBlob3D list
		 */
		void unlockBlob3DList();
#pragma endregion 



	protected:
		std::list<TuioListener*> listenerList;
		
		std::list<TuioObject*> objectList;
		std::list<TuioCursor*> cursorList;
		std::list<TuioBlob*> blobList;

		std::list<TuioObject25D*> object25DList;
		std::list<TuioCursor25D*> cursor25DList;
		std::list<TuioBlob25D*> blob25DList;

		std::list<TuioObject3D*> object3DList;
		std::list<TuioCursor3D*> cursor3DList;
		std::list<TuioBlob3D*> blob3DList;
		
#ifdef WIN32
		HANDLE objectMutex;
		HANDLE cursorMutex;
		HANDLE blobMutex;
		HANDLE object25DMutex;
		HANDLE cursor25DMutex;
		HANDLE blob25DMutex;
		HANDLE object3DMutex;
		HANDLE cursor3DMutex;
		HANDLE blob3DMutex;
#else
		pthread_mutex_t objectMutex;
		pthread_mutex_t cursorMutex;
		pthread_mutex_t blobMutex;
		pthread_mutex_t object25DMutex;
		pthread_mutex_t cursor25DMutex;
		pthread_mutex_t blob25DMutex;
		pthread_mutex_t object3DMutex;
		pthread_mutex_t cursor3DMutex;
		pthread_mutex_t blob3DMutex;
#endif
				
	};
}
#endif /* INCLUDED_TUIODISPATCHER_H */

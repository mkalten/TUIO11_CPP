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

#ifndef INCLUDED_TUIOMANAGER_H
#define INCLUDED_TUIOMANAGER_H

#include "TuioDispatcher.h"

#include <iostream>
#include <list>
#include <algorithm>

#define OBJ_MESSAGE_SIZE 108	// setMessage + fseqMessage size
#define CUR_MESSAGE_SIZE 88
#define BLB_MESSAGE_SIZE 116

#define OBJ25D_MESSAGE_SIZE 124	// setMessage + fseqMessage size
#define CUR25D_MESSAGE_SIZE 104
#define BLB25D_MESSAGE_SIZE 132

#define OBJ3D_MESSAGE_SIZE 132	// setMessage + fseqMessage size
#define CUR3D_MESSAGE_SIZE 104
#define BLB3D_MESSAGE_SIZE 144

namespace TUIO {
	/**
	 * <p>The TuioManager class is the central TUIO session management component.</p> 
	 * <p>During runtime the each frame is marked with the initFrame and commitFrame methods, 
	 * while the currently present TuioObjects are managed by the server with ADD, UPDATE and REMOVE methods in analogy to the TuioClient's TuioListener interface.</p> 
	 * <p><code>
	 * TuioManager *manager = new TuioManager();<br/>
	 * ...<br/>
	 * server->initFrame(TuioTime::getSessionTime());<br/>
	 * TuioObject *tobj = server->addTuioObject(xpos,ypos, angle);<br/>
	 * TuioCursor *tcur = server->addTuioObject(xpos,ypos);<br/>
	 * TuioBlob *tblb = server->addTuioBlob(xpos,ypos,width,height,angle);<br/>
	 * server->commitFrame();<br/>
	 * ...<br/>
	 * server->initFrame(TuioTime::getSessionTime());<br/>
	 * server->updateTuioObject(tobj, xpos,ypos, angle);<br/>
	 * server->updateTuioCursor(tcur, xpos,ypos);<br/>
	 * server->updateTuioBlob(tblb, xpos,ypos,width,height,angle);<br/>
	 * server->commitFrame();<br/>
	 * ...<br/>
	 * server->initFrame(TuioTime::getSessionTime());<br/>
	 * server->removeTuioObject(tobj);<br/>
	 * server->removeTuioCursor(tcur);<br/>
	 * server->removeTuioBlob(tblb);<br/>
	 * server->commitFrame();<br/>
	 * </code></p>
	 *
	 * @author Martin Kaltenbrunner
	 * @version 1.1.6
	 */ 
	class LIBDECL TuioManager : public TuioDispatcher { 
	
	public:

		/**
		 * The default constructor creates a TuioManager
		 */
		TuioManager();

		/**
		 * The destructor is doing nothing in particular. 
		 */
		~TuioManager();
		
		/**
		 * Creates a new TuioObject based on the given arguments.
		 * The new TuioObject is added to the TuioServer's internal list of active TuioObjects 
		 * and a reference is returned to the caller.
		 *
		 * @param	sym	the Symbol ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	a	the angle to assign
		 * @return	reference to the created TuioObject
		 */
		TuioObject* addTuioObject(int sym, float xp, float yp, float a);

		/**
		 * Updates the referenced TuioObject based on the given arguments.
		 *
		 * @param	tobj	the TuioObject to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	a	the angle to assign
		 */
		void updateTuioObject(TuioObject *tobj, float xp, float yp, float a);

		/**
		 * Removes the referenced TuioObject from the TuioServer's internal list of TuioObjects
		 * and deletes the referenced TuioObject afterwards
		 *
		 * @param	tobj	the TuioObject to remove
		 */
		void removeTuioObject(TuioObject *tobj);

		/**
		 * Adds an externally managed TuioObject to the TuioServer's internal list of active TuioObjects 
		 *
		 * @param	tobj	the TuioObject to add
		 */
		void addExternalTuioObject(TuioObject *tobj);

		/**
		 * Updates an externally managed TuioObject 
		 *
		 * @param	tobj	the TuioObject to update
		 */
		void updateExternalTuioObject(TuioObject *tobj);

		/**
		 * Removes an externally managed TuioObject from the TuioServer's internal list of TuioObjects
		 * The referenced TuioObject is not deleted
		 *
		 * @param	tobj	the TuioObject to remove
		 */
		void removeExternalTuioObject(TuioObject *tobj);
		
		/**
		 * Creates a new TuioCursor based on the given arguments.
		 * The new TuioCursor is added to the TuioServer's internal list of active TuioCursors 
		 * and a reference is returned to the caller.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @return	reference to the created TuioCursor
		 */
		TuioCursor* addTuioCursor(float xp, float yp);

		/**
		 * Updates the referenced TuioCursor based on the given arguments.
		 *
		 * @param	tcur	the TuioObject to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 */
		void updateTuioCursor(TuioCursor *tcur, float xp, float yp);

		/**
		 * Removes the referenced TuioCursor from the TuioServer's internal list of TuioCursors
		 * and deletes the referenced TuioCursor afterwards
		 *
		 * @param	tcur	the TuioCursor to remove
		 */
		void removeTuioCursor(TuioCursor *tcur);

		/**
		 * Adds an externally managed TuioCursor 
		 *
		 * @param	tcur	the TuioCursor to add
		 */
		void addExternalTuioCursor(TuioCursor *tcur);

		/**
		 * Updates an externally managed TuioCursor 
		 *
		 * @param	tcur	the TuioCursor to update
		 */
		void updateExternalTuioCursor(TuioCursor *tcur);

		/**
		 * Removes an externally managed TuioCursor from the TuioServer's internal list of TuioCursor
		 * The referenced TuioCursor is not deleted
		 *
		 * @param	tcur	the TuioCursor to remove
		 */
		void removeExternalTuioCursor(TuioCursor *tcur);

		/**
		 * Creates a new TuioBlob based on the given arguments.
		 * The new TuioBlob is added to the TuioServer's internal list of active TuioBlobs 
		 * and a reference is returned to the caller.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	angle	the angle to assign
		 * @param	width	the width to assign
		 * @param	height	the height to assign
		 * @param	area	the area to assign
		 * @return	reference to the created TuioBlob
		 */
		TuioBlob* addTuioBlob(float xp, float yp, float angle, float width, float height, float area);
		
		/**
		 * Updates the referenced TuioBlob based on the given arguments.
		 *
		 * @param	tblb	the TuioObject to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	angle	the angle to assign
		 * @param	width	the width to assign
		 * @param	height	the height to assign
		 * @param	area	the area to assign
		 */
		void updateTuioBlob(TuioBlob *tblb, float xp, float yp, float angle, float width, float height, float area);
		
		/**
		 * Removes the referenced TuioBlob from the TuioServer's internal list of TuioBlobs
		 * and deletes the referenced TuioBlob afterwards
		 *
		 * @param	tblb	the TuioBlob to remove
		 */
		void removeTuioBlob(TuioBlob *tblb);
		
		/**
		 * Updates an externally managed TuioBlob 
		 *
		 * @param	tblb	the TuioBlob to update
		 */
		void addExternalTuioBlob(TuioBlob *tblb);
		
		/**
		 * Updates an externally managed TuioBlob 
		 *
		 * @param	tblb	the TuioBlob to update
		 */
		void updateExternalTuioBlob(TuioBlob *tblb);
		
		/**
		 * Removes an externally managed TuioBlob from the TuioServer's internal list of TuioBlob
		 * The referenced TuioBlob is not deleted
		 *
		 * @param	tblb	the TuioBlob to remove
		 */
		void removeExternalTuioBlob(TuioBlob *tblb);		
		
















		/**
		 * Creates a new TuioObject25D based on the given arguments.
		 * The new TuioObject25D is added to the TuioServer's internal list of active TuioObjects
		 * and a reference is returned to the caller.
		 *
		 * @param	sym	the Symbol ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	a	the angle to assign
		 * @return	reference to the created TuioObject
		 */
		TuioObject25D* addTuioObject25D(int sym, float xp, float yp, float zp, float a);

		/**
		 * Updates the referenced TuioObject25D based on the given arguments.
		 *
		 * @param	tobj	the TuioObject25D to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	a	the angle to assign
		 */
		void updateTuioObject25D(TuioObject25D *tobj, float xp, float yp, float zp, float a);

		/**
		 * Removes the referenced TuioObject25D from the TuioServer's internal list of TuioObjects25D
		 * and deletes the referenced TuioObject25D afterwards
		 *
		 * @param	tobj	the TuioObject to remove
		 */
		void removeTuioObject25D(TuioObject25D *tobj);

		/**
		 * Adds an externally managed TuioObject25D to the TuioServer's internal list of active TuioObjects
		 *
		 * @param	tobj	the TuioObject25D to add
		 */
		void addExternalTuioObject25D(TuioObject25D *tobj);

		/**
		 * Updates an externally managed TuioObject25D
		 *
		 * @param	tobj	the TuioObject25D to update
		 */
		void updateExternalTuioObject25D(TuioObject25D *tobj);

		/**
		 * Removes an externally managed TuioObject25D from the TuioServer's internal list of TuioObjects25D
		 * The referenced TuioObject25D is not deleted
		 *
		 * @param	tobj	the TuioObject25D to update
		 */
		void removeExternalTuioObject25D(TuioObject25D *tobj);

		/**
		 * Creates a new TuioCursor25D based on the given arguments.
		 * The new TuioCursor25D is added to the TuioServer's internal list of active TuioCursors25D
		 * and a reference is returned to the caller.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @return	reference to the created TuioCursor
		 */
		TuioCursor25D* addTuioCursor25D(float xp, float yp, float zp);

		/**
		 * Updates the referenced TuioCursor25D based on the given arguments.
		 *
		 * @param	tcur	the TuioObject25D to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 */
		void updateTuioCursor25D(TuioCursor25D *tcur, float xp, float yp, float zp);

		/**
		 * Removes the referenced TuioCursor25D from the TuioServer's internal list of TuioCursors25D
		 * and deletes the referenced TuioCursor afterwards
		 *
		 * @param	tcur	the TuioCursor25D to remove
		 */
		void removeTuioCursor25D(TuioCursor25D *tcur);

		/**
		 * Adds an externally managed TuioCursor25D
		 *
		 * @param	tcur	the TuioCursor25D to add
		 */
		void addExternalTuioCursor25D(TuioCursor25D *tcur);

		/**
		 * Updates an externally managed TuioCursor25D
		 *
		 * @param	tcur	the TuioCursor25D to update
		 */
		void updateExternalTuioCursor25D(TuioCursor25D *tcur);

		/**
		 * Removes an externally managed TuioCursor from the TuioServer's internal list of TuioCursor
		 * The referenced TuioCursor is not deleted
		 *
		 * @param	tcur	the TuioCursor25D to remove
		 */
		void removeExternalTuioCursor25D(TuioCursor25D *tcur);

		/**
		 * Creates a new TuioBlob25D based on the given arguments.
		 * The new TuioBlob is added to the TuioServer's internal list of active TuioBlobs
		 * and a reference is returned to the caller.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	angle	the angle to assign
		 * @param	width	the width to assign
		 * @param	height	the height to assign
		 * @param	area	the area to assign
		 * @return	reference to the created TuioBlob
		 */
		TuioBlob25D* addTuioBlob25D(float xp, float yp, float zp, float angle, float width, float height, float area);

		/**
		 * Updates the referenced TuioBlob25D based on the given arguments.
		 *
		 * @param	tblb	the TuioObject to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	angle	the angle to assign
		 * @param	width	the width to assign
		 * @param	height	the height to assign
		 * @param	area	the area to assign
		 */
		void updateTuioBlob25D(TuioBlob25D *tblb, float xp, float yp, float zp, float angle, float width, float height, float area);

		/**
		 * Removes the referenced TuioBlob25D from the TuioServer's internal list of TuioBlobs25D
		 * and deletes the referenced TuioBlob afterwards
		 *
		 * @param	tblb	the TuioBlob25D to remove
		 */
		void removeTuioBlob25D(TuioBlob25D *tblb);

		/**
		 * Updates an externally managed TuioBlob25D
		 *
		 * @param	tblb	the TuioBlob25D to update
		 */
		void addExternalTuioBlob25D(TuioBlob25D *tblb);

		/**
		 * Updates an externally managed TuioBlob25D
		 *
		 * @param	tblb	the TuioBlob25D to update
		 */
		void updateExternalTuioBlob25D(TuioBlob25D *tblb);

		/**
		 * Removes an externally managed TuioBlob25D from the TuioServer's internal list of TuioBlob25D
		 * The referenced TuioBlob25D is not deleted
		 *
		 * @param	tblb	the TuioBlob25D to remove
		 */
		void removeExternalTuioBlob25D(TuioBlob25D *tblb);




















		/**
		 * Creates a new TuioObject3D based on the given arguments.
		 * The new TuioObject3D is added to the TuioServer's internal list of active TuioObjects
		 * and a reference is returned to the caller.
		 *
		 * @param	sym	the Symbol ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	roll	the roll angle to assign
		 * @param	pitch	the pitch angle to assign
		 * @param	yaw	   the yaw angle to assign
		 * @return	reference to the created TuioObject
		 */
		TuioObject3D* addTuioObject3D(int sym, float xp, float yp, float zp, float roll, float pitch, float yaw);

		/**
		 * Updates the referenced TuioObject3D based on the given arguments.
		 *
		 * @param	tobj	the TuioObject3D to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	roll	the roll angle to assign
		 * @param	pitch	the pitch angle to assign
		 * @param	yaw	   the yaw angle to assign
		 */
		void updateTuioObject3D(TuioObject3D *tobj, float xp, float yp, float zp, float roll, float pitch, float yaw);

		/**
		 * Removes the referenced TuioObject3D from the TuioServer's internal list of TuioObjects3D
		 * and deletes the referenced TuioObject3D afterwards
		 *
		 * @param	tobj	the TuioObject to remove
		 */
		void removeTuioObject3D(TuioObject3D *tobj);

		/**
		 * Adds an externally managed TuioObject3D to the TuioServer's internal list of active TuioObjects
		 *
		 * @param	tobj	the TuioObject3D to add
		 */
		void addExternalTuioObject3D(TuioObject3D *tobj);

		/**
		 * Updates an externally managed TuioObject3D
		 *
		 * @param	tobj	the TuioObject3D to update
		 */
		void updateExternalTuioObject3D(TuioObject3D *tobj);

		/**
		 * Removes an externally managed TuioObject3D from the TuioServer's internal list of TuioObjects3D
		 * The referenced TuioObject3D is not deleted
		 *
		 * @param	tobj	the TuioObject3D to update
		 */
		void removeExternalTuioObject3D(TuioObject3D *tobj);

		/**
		 * Creates a new TuioCursor3D based on the given arguments.
		 * The new TuioCursor3D is added to the TuioServer's internal list of active TuioCursors3D
		 * and a reference is returned to the caller.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @return	reference to the created TuioCursor
		 */
		TuioCursor3D* addTuioCursor3D(float xp, float yp, float zp);

		/**
		 * Updates the referenced TuioCursor3D based on the given arguments.
		 *
		 * @param	tcur	the TuioObject3D to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 */
		void updateTuioCursor3D(TuioCursor3D *tcur, float xp, float yp, float zp);

		/**
		 * Removes the referenced TuioCursor3D from the TuioServer's internal list of TuioCursors3D
		 * and deletes the referenced TuioCursor afterwards
		 *
		 * @param	tcur	the TuioCursor3D to remove
		 */
		void removeTuioCursor3D(TuioCursor3D *tcur);

		/**
		 * Adds an externally managed TuioCursor3D
		 *
		 * @param	tcur	the TuioCursor3D to add
		 */
		void addExternalTuioCursor3D(TuioCursor3D *tcur);

		/**
		 * Updates an externally managed TuioCursor3D
		 *
		 * @param	tcur	the TuioCursor3D to update
		 */
		void updateExternalTuioCursor3D(TuioCursor3D *tcur);

		/**
		 * Removes an externally managed TuioCursor from the TuioServer's internal list of TuioCursor
		 * The referenced TuioCursor is not deleted
		 *
		 * @param	tcur	the TuioCursor3D to remove
		 */
		void removeExternalTuioCursor3D(TuioCursor3D *tcur);

		/**
		 * Creates a new TuioBlob3D based on the given arguments.
		 * The new TuioBlob is added to the TuioServer's internal list of active TuioBlobs
		 * and a reference is returned to the caller.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	roll	the roll angle to assign
		 * @param	pitch	the pitch angle to assign
		 * @param	yaw	   the yaw angle to assign
		 * @param	width	the width to assign
		 * @param	height	the height to assign
		 * @param	depth	the depth to assign
		 * @param	volume	the volume to assign
		 * @return	reference to the created TuioBlob
		 */
		TuioBlob3D* addTuioBlob3D(float xp, float yp, float zp, float roll, float pitch, float yaw, float width, float height, float depth, float volume);

		/**
		 * Updates the referenced TuioBlob3D based on the given arguments.
		 *
		 * @param	tblb	the TuioObject to update
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	roll	the roll angle to assign
		 * @param	pitch	the pitch angle to assign
		 * @param	yaw	   the yaw angle to assign
		 * @param	width	the width to assign
		 * @param	height	the height to assign
		 * @param	depth	the depth to assign
		 * @param	volume	the volume to assign
		 */
		void updateTuioBlob3D(TuioBlob3D *tblb, float xp, float yp, float zp, float roll, float pitch, float yaw, float width, float height, float depth, float volume);

		/**
		 * Removes the referenced TuioBlob3D from the TuioServer's internal list of TuioBlobs3D
		 * and deletes the referenced TuioBlob afterwards
		 *
		 * @param	tblb	the TuioBlob3D to remove
		 */
		void removeTuioBlob3D(TuioBlob3D *tblb);

		/**
		 * Updates an externally managed TuioBlob3D
		 *
		 * @param	tblb	the TuioBlob3D to update
		 */
		void addExternalTuioBlob3D(TuioBlob3D *tblb);

		/**
		 * Updates an externally managed TuioBlob3D
		 *
		 * @param	tblb	the TuioBlob3D to update
		 */
		void updateExternalTuioBlob3D(TuioBlob3D *tblb);

		/**
		 * Removes an externally managed TuioBlob3D from the TuioServer's internal list of TuioBlob3D
		 * The referenced TuioBlob3D is not deleted
		 *
		 * @param	tblb	the TuioBlob3D to remove
		 */
		void removeExternalTuioBlob3D(TuioBlob3D *tblb);

















		/**
		 * Initializes a new frame with the given TuioTime
		 *
		 * @param	ttime	the frame time
		 */
		void initFrame(TuioTime ttime);
		
		/**
		 * Commits the current frame.
		 * Generates and sends TUIO messages of all currently active and updated TuioObjects and TuioCursors.
		 */
		void commitFrame();

		/**
		 * Returns the next available Session ID for external use.
		 * @return	the next available Session ID for external use
		 */
		long getSessionID();

		/**
		 * Returns the current frame ID for external use.
		 * @return	the current frame ID for external use
		 */
		long getFrameID();
		
		/**
		 * Returns the current frame ID for external use.
		 * @return	the current frame ID for external use
		 */
		TuioTime getFrameTime();
		
		/**
		 * Returns a List of all currently inactive TuioObjects
		 *
		 * @return  a List of all currently inactive TuioObjects
		 */
		std::list<TuioObject*> getUntouchedObjects();

		/**
		 * Returns a List of all currently inactive TuioCursors
		 *
		 * @return  a List of all currently inactive TuioCursors
		 */
		std::list<TuioCursor*> getUntouchedCursors();

		/**
		 * Returns a List of all currently inactive TuioBlobs
		 *
		 * @return  a List of all currently inactive TuioBlobs
		 */
		std::list<TuioBlob*> getUntouchedBlobs();
		
		/**
		 * Calculates speed and acceleration values for all currently inactive TuioObjects
		 */
		void stopUntouchedMovingObjects();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioCursors
		 */
		void stopUntouchedMovingCursors();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioBlobs
		 */
		void stopUntouchedMovingBlobs();
		
		/**
		 * Removes all currently inactive TuioObjects from the TuioServer's internal list of TuioObjects
		 */
		void removeUntouchedStoppedObjects();

		/**
		 * Removes all currently inactive TuioCursors from the TuioServer's internal list of TuioCursors
		 */
		void removeUntouchedStoppedCursors();

		/**
		 * Removes all currently inactive TuioCursors from the TuioServer's internal list of TuioBlobs
		 */
		void removeUntouchedStoppedBlobs();
		
		/**
		 * Returns the TuioObject closest to the provided coordinates
		 * or NULL if there isn't any active TuioObject
		 *
		 * @return  the closest TuioObject to the provided coordinates or NULL
		 */
		TuioObject* getClosestTuioObject(float xp, float yp);
		
		/**
		 * Returns the TuioCursor closest to the provided coordinates
		 * or NULL if there isn't any active TuioCursor
		 *
		 * @return  the closest TuioCursor corresponding to the provided coordinates or NULL
		 */
		TuioCursor* getClosestTuioCursor(float xp, float yp);

		/**
		 * Returns the TuioBlob closest to the provided coordinates
		 * or NULL if there isn't any active TuioBlob
		 *
		 * @return  the closest TuioBlob corresponding to the provided coordinates or NULL
		 */
		TuioBlob* getClosestTuioBlob(float xp, float yp);
		






		/**
		 * Returns a List of all currently inactive TuioObjects25D
		 *
		 * @return  a List of all currently inactive TuioObjects25D
		 */
		std::list<TuioObject25D*> getUntouchedObjects25D();

		/**
		 * Returns a List of all currently inactive TuioCursors25D
		 *
		 * @return  a List of all currently inactive TuioCursors25D
		 */
		std::list<TuioCursor25D*> getUntouchedCursors25D();

		/**
		 * Returns a List of all currently inactive TuioBlobs25D
		 *
		 * @return  a List of all currently inactive TuioBlobs25D
		 */
		std::list<TuioBlob25D*> getUntouchedBlobs25D();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioObjects25D
		 */
		void stopUntouchedMovingObjects25D();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioCursors25D
		 */
		void stopUntouchedMovingCursors25D();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioBlobs25D
		 */
		void stopUntouchedMovingBlobs25D();

		/**
		 * Removes all currently inactive TuioObjects from the TuioServer's internal list of TuioObjects25D
		 */
		void removeUntouchedStoppedObjects25D();

		/**
		 * Removes all currently inactive TuioCursors from the TuioServer's internal list of TuioCursors25D
		 */
		void removeUntouchedStoppedCursors25D();

		/**
		 * Removes all currently inactive TuioCursors from the TuioServer's internal list of TuioBlobs25D
		 */
		void removeUntouchedStoppedBlobs25D();

		/**
		 * Returns the TuioObject25D closest to the provided coordinates
		 * or NULL if there isn't any active TuioObject25D
		 *
		 * @return  the closest TuioObject25D to the provided coordinates or NULL
		 */
		TuioObject25D* getClosestTuioObject25D(float xp, float yp, float zp);

		/**
		 * Returns the TuioCursor25D closest to the provided coordinates
		 * or NULL if there isn't any active TuioCursor25D
		 *
		 * @return  the closest TuioCursor25D corresponding to the provided coordinates or NULL
		 */
		TuioCursor25D* getClosestTuioCursor25D(float xp, float yp, float zp);

		/**
		 * Returns the TuioBlob25D closest to the provided coordinates
		 * or NULL if there isn't any active TuioBlob25D
		 *
		 * @return  the closest TuioBlob25D corresponding to the provided coordinates or NULL
		 */
		TuioBlob25D* getClosestTuioBlob25D(float xp, float yp, float zp);





		/**
		 * Returns a List of all currently inactive TuioObjects3D
		 *
		 * @return  a List of all currently inactive TuioObjects3D
		 */
		std::list<TuioObject3D*> getUntouchedObjects3D();

		/**
		 * Returns a List of all currently inactive TuioCursors3D
		 *
		 * @return  a List of all currently inactive TuioCursors3D
		 */
		std::list<TuioCursor3D*> getUntouchedCursors3D();

		/**
		 * Returns a List of all currently inactive TuioBlobs3D
		 *
		 * @return  a List of all currently inactive TuioBlobs3D
		 */
		std::list<TuioBlob3D*> getUntouchedBlobs3D();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioObjects3D
		 */
		void stopUntouchedMovingObjects3D();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioCursors3D
		 */
		void stopUntouchedMovingCursors3D();

		/**
		 * Calculates speed and acceleration values for all currently inactive TuioBlobs3D
		 */
		void stopUntouchedMovingBlobs3D();

		/**
		 * Removes all currently inactive TuioObjects from the TuioServer's internal list of TuioObjects3D
		 */
		void removeUntouchedStoppedObjects3D();

		/**
		 * Removes all currently inactive TuioCursors from the TuioServer's internal list of TuioCursors3D
		 */
		void removeUntouchedStoppedCursors3D();

		/**
		 * Removes all currently inactive TuioCursors from the TuioServer's internal list of TuioBlobs3D
		 */
		void removeUntouchedStoppedBlobs3D();

		/**
		 * Returns the TuioObject3D closest to the provided coordinates
		 * or NULL if there isn't any active TuioObject3D
		 *
		 * @return  the closest TuioObject3D to the provided coordinates or NULL
		 */
		TuioObject3D* getClosestTuioObject3D(float xp, float yp, float zp);

		/**
		 * Returns the TuioCursor3D closest to the provided coordinates
		 * or NULL if there isn't any active TuioCursor3D
		 *
		 * @return  the closest TuioCursor3D corresponding to the provided coordinates or NULL
		 */
		TuioCursor3D* getClosestTuioCursor3D(float xp, float yp, float zp);

		/**
		 * Returns the TuioBlob3D closest to the provided coordinates
		 * or NULL if there isn't any active TuioBlob3D
		 *
		 * @return  the closest TuioBlob3D corresponding to the provided coordinates or NULL
		 */
		TuioBlob3D* getClosestTuioBlob3D(float xp, float yp, float zp);

















		/**
		 * The TuioServer prints verbose TUIO event messages to the console if set to true.
		 * @param	verbose	print verbose messages if set to true
		 */
		void setVerbose(bool verbose) { this->verbose=verbose; }
		bool isVerbose() { return verbose; }

		void setInversion(bool ix, bool iy, bool ia) { 
			invert_x = ix; 
			invert_y = iy; 
			invert_a = ia; 
		};		
		
		void setInversion(bool ix, bool iy, bool iz, bool ia, bool ib, bool ic) {
			invert_x = ix;
			invert_y = iy;
			invert_z = iz;
			invert_a = ia;
			invert_b = ib;
			invert_c = ic;
		};

		void setInvertXpos(bool ix) { invert_x = ix; };
		void setInvertYpos(bool iy) { invert_y = iy; };
		void setInvertZpos(bool iz) { invert_z = iz; };
		void setInvertAngle(bool ia) { invert_a = ia; };
		void setInvertRoll(bool ir) { invert_a = ir; };
		void setInvertPitch(bool ip) { invert_b = ip; };
		void setInvertYaw(bool iy) { invert_c = iy; };
		bool getInvertXpos() { return invert_x; };
		bool getInvertYpos() { return invert_y; };
		bool getInvertZpos() { return invert_z; };
		bool getInvertAngle() { return invert_a; };
		bool getInvertRoll() { return invert_a; };
		bool getInvertPitch() { return invert_b; };
		bool getInvertYaw() { return invert_c; };
		void resetTuioObjects();
		void resetTuioCursors();		
		void resetTuioBlobs();
		void resetTuioObjects25D();
		void resetTuioCursors25D();
		void resetTuioBlobs25D();
		void resetTuioObjects3D();
		void resetTuioCursors3D();
		void resetTuioBlobs3D();
		
	protected:

		// freeList and freeBuffer are nor appliable on Object because they have their own ids

		std::list<TuioCursor*> freeCursorList;
		std::list<TuioCursor*> freeCursorBuffer;

		std::list<TuioBlob*> freeBlobList;
		std::list<TuioBlob*> freeBlobBuffer;

		std::list<TuioCursor25D*> freeCursor25DList;
		std::list<TuioCursor25D*> freeCursor25DBuffer;

		std::list<TuioBlob25D*> freeBlob25DList;
		std::list<TuioBlob25D*> freeBlob25DBuffer;

		std::list<TuioCursor3D*> freeCursor3DList;
		std::list<TuioCursor3D*> freeCursor3DBuffer;

		std::list<TuioBlob3D*> freeBlob3DList;
		std::list<TuioBlob3D*> freeBlob3DBuffer;

		TuioTime currentFrameTime;
		long currentFrame;
		int maxCursorID;
		int maxBlobID;
		int maxCursor25DID;
		int maxBlob25DID;
		int maxCursor3DID;
		int maxBlob3DID;
		long sessionID;

		bool updateObject;
		bool updateCursor;
		bool updateBlob;
		bool updateObject25D;
		bool updateCursor25D;
		bool updateBlob25D;
		bool updateObject3D;
		bool updateCursor3D;
		bool updateBlob3D;
		bool verbose;

		bool invert_x;
		bool invert_y;
		bool invert_z;
		bool invert_a;
		bool invert_b;
		bool invert_c;
	};
}
#endif /* INCLUDED_TUIOMANAGER_H */

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

#ifndef INCLUDED_TUIOPOINT_H
#define INCLUDED_TUIOPOINT_H

#define _USE_MATH_DEFINES
#include "TuioTime.h"
#include "OneEuroFilter.h"
#include <cmath>

namespace TUIO {

	/**
	 * The TuioPoint class on the one hand is a simple container and utility class to handle TUIO positions in general, 
	 * on the other hand the TuioPoint is the base class for the TuioCursor, TuioObject and TuioBlob classes.
	 *
	 * @author Nicolas Bremard
	 * @version 1.1.7
	 */ 
	class LIBDECL TuioPoint {

	protected:
		/**
		 * X coordinate, representated as a floating point value in a range of 0..1  
		 */
		float xpos;
		/**
		 * Y coordinate, representated as a floating point value in a range of 0..1  
		 */
		float ypos;
		/**
		 * Z coordinate, representated as a floating point value in a range of 0..1
		 */
		float zpos;
		/**
		 * The time stamp of the last update represented as TuioTime (time since session start)
		 */
		TuioTime currentTime;
		/**
		 * The creation time of this TuioPoint represented as TuioTime (time since session start)
		 */
		TuioTime startTime;

		OneEuroFilter *xposFilter;
		OneEuroFilter *yposFilter;
		OneEuroFilter *zposFilter;
		float posThreshold;

	public:
		/**
		 * The default constructor takes no arguments and sets
		 * its coordinate attributes to zero and its time stamp to the current session time.
		 */
		TuioPoint (float xp, float yp, float zp);

		/**
		 * This constructor takes a TuioTime object and two floating point coordinate arguments and sets
		 * its coordinate attributes to these values and its time stamp to the provided TUIO time object.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 */
		TuioPoint (TuioTime ttime, float xp, float yp, float zp);

		/**
		 * This constructor takes a TuioPoint argument and sets its coordinate attributes
		 * to the coordinates of the provided TuioPoint and its time stamp to the current session time.
		 *
		 * @param	tpoint	the TuioPoint to assign
		 */
		TuioPoint (TuioPoint *tpoint);

		/**
		 * The destructor is doing nothing in particular.
		 */
		virtual ~TuioPoint(){
			if (xposFilter) delete xposFilter;
			if (yposFilter) delete yposFilter;
			if (zposFilter) delete zposFilter;
		};

		/**
		 * Takes a TuioPoint argument and updates its coordinate attributes
		 * to the coordinates of the provided TuioPoint and leaves its time stamp unchanged.
		 *
		 * @param	tpoint	the TuioPoint to assign
		 */
		void update (TuioPoint *tpoint);

		/**
		 * Takes two floating point coordinate arguments and updates its coordinate attributes 
		 * to the coordinates of the provided TuioPoint and leaves its time stamp unchanged.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 */
		void update (float xp, float yp, float zp);

		/**
		 * Takes a TuioTime object and two floating point coordinate arguments and updates its coordinate attributes 
		 * to the coordinates of the provided TuioPoint and its time stamp to the provided TUIO time object.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 */
		void update (TuioTime ttime, float xp, float yp, float zp);


		/**
		 * Takes two floating point coordinate arguments and updates its coordinate attributes
		 * to the coordinates of the provided TuioPoint and leaves its time stamp unchanged.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 */
		void update(float xp, float yp);

		/**
		 * Takes a TuioTime object and two floating point coordinate arguments and updates its coordinate attributes
		 * to the coordinates of the provided TuioPoint and its time stamp to the provided TUIO time object.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 */
		void update(TuioTime ttime, float xp, float yp);

		
		/**
		 * Returns the X coordinate of this TuioPoint. 
		 * @return	the X coordinate of this TuioPoint
		 */
		float getX() const;
		
		/**
		 * Returns the Y coordinate of this TuioPoint. 
		 * @return	the Y coordinate of this TuioPoint
		 */
		float getY() const;

		/**
		 * Returns the Z coordinate of this TuioPoint.
		 * @return	the Z coordinate of this TuioPoint
		 */
		float getZ() const;
		
		/**
		 * Returns the distance to the provided coordinates 
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Y coordinate of the distant point
		 * @return	the distance to the provided coordinates
		 */
		float getDistance(float xp, float yp, float zp) const;


		/**
		 * Returns the distance to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @return	the distance to the provided coordinates
		 */
		float getDistance(float xp, float yp) const;


		/**
		 * Returns the distance to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Y coordinate of the distant point
		 * @param	w	the width of the space
		 * @param	h	the height of the space
		 * @param	d	the depth of the space
		 * @return	the distance to the provided coordinates
		 */
		float getSpaceDistance(float xp, float yp, float zp, int w, int h, int d) const;

		/**
		 * Returns the distance to the provided TuioPoint 
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the distance to the provided TuioPoint
		 */
		float getDistance(TuioPoint *tpoint) const;


		/**
		 * Returns the 2D angle  to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @return	the 2D angle to the provided coordinates
		 */
		float getAngle(float xp, float yp) const;
		/**
		 * Returns the 2D angle to the provided TuioPoint
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the 2D angle to the provided TuioPoint
		 */
		float getAngle(TuioPoint *tpoint) const;
		/**
		 * Returns the 2D angle in degrees to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Z coordinate of the distant point
		 * @return	the 2D angle in degrees to the provided TuioPoint
		 */
		float getAngleDegrees(float xp, float yp) const;
		/**
		 * Returns the 2D angle in degrees to the provided TuioPoint
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the 2D angle in degrees to the provided TuioPoint
		 */
		float getAngleDegrees(TuioPoint *tpoint) const;

		/**
		 * Returns the roll angle  to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Z coordinate of the distant point
		 * @return	the roll angle to the provided coordinates
		 */
		float getRollAngle(float xp, float yp, float zp) const;
		/**
		 * Returns the roll angle to the provided TuioPoint
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the roll angle to the provided TuioPoint
		 */
		float getRollAngle(TuioPoint *tpoint) const;
		/**
		 * Returns the roll angle in degrees to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Z coordinate of the distant point
		 * @return	the roll angle in degrees to the provided TuioPoint
		 */
		float getRollAngleDegrees(float xp, float yp, float zp) const;
		/**
		 * Returns the roll angle in degrees to the provided TuioPoint
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the roll angle in degrees to the provided TuioPoint
		 */
		float getRollAngleDegrees(TuioPoint *tpoint) const;


		/**
		 * Returns the pitch angle  to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Z coordinate of the distant point
		 * @return	the pitch angle to the provided coordinates
		 */
		float getPitchAngle(float xp, float yp, float zp) const;
		/**
		 * Returns the pitch angle to the provided TuioPoint
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the pitch angle to the provided TuioPoint
		 */
		float getPitchAngle(TuioPoint *tpoint) const;
		/**
		 * Returns the pitch angle in degrees to the provided coordinates
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Z coordinate of the distant point
		 * @return	the pitch angle in degrees to the provided TuioPoint
		 */
		float getPitchAngleDegrees(float xp, float yp, float zp) const;
		/**
		 * Returns the pitch angle in degrees to the provided TuioPoint
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the pitch angle in degrees to the provided TuioPoint
		 */
		float getPitchAngleDegrees(TuioPoint *tpoint) const;

		/**
		 * Returns the yaw angle  to the provided coordinates 
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Z coordinate of the distant point
		 * @return	the yaw angle to the provided coordinates
		 */
		float getYawAngle(float xp, float yp, float zp) const;
		/**
		 * Returns the yaw angle to the provided TuioPoint 
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the yaw angle to the provided TuioPoint
		 */
		float getYawAngle(TuioPoint *tpoint) const;
		/**
		 * Returns the yaw angle in degrees to the provided coordinates 
		 *
		 * @param	xp	the X coordinate of the distant point
		 * @param	yp	the Y coordinate of the distant point
		 * @param	zp	the Z coordinate of the distant point
		 * @return	the yaw angle in degrees to the provided TuioPoint
		 */
		float getYawAngleDegrees(float xp, float yp, float zp) const;
		/**
		 * Returns the yaw angle in degrees to the provided TuioPoint 
		 *
		 * @param	tpoint	the distant TuioPoint
		 * @return	the yaw angle in degrees to the provided TuioPoint
		 */
		float getYawAngleDegrees(TuioPoint *tpoint) const;


		/**
		 * Returns the X coordinate in units relative to the provided space width. 
		 *
		 * @param	width	the space width
		 * @return	the X coordinate of this TuioPoint in units relative to the provided space width
		 */
		int getSpaceX(int width) const;
		/*
		 * Returns the Y coordinate in units relative to the provided space height. 
		 *
		 * @param	height	the space height
		 * @return	the Y coordinate of this TuioPoint in units relative to the provided space height
		 */
		int getSpaceY(int height) const;		
		/*
		 * Returns the Z coordinate in pixels relative to the provided space depth.
		 *
		 * @param	depth	the space depth
		 * @return	the Z coordinate of this TuioPoint in units relative to the provided space depth
		 */
		int getSpaceZ(int depth) const;


		/**
		 * Returns the X coordinate in unit relative to the provided screen width. (Kept for retro compatibility)
		 *
		 * @param	width	the space width
		 * @return	the X coordinate of this TuioPoint in units relative to the provided screen width
		 */
		int getScreenX(int width) const { return getSpaceX(width); }
		/*
		 * Returns the Y coordinate in units relative to the provided screen height. (Kept for retro compatibility)
		 *
		 * @param	height	the space height
		 * @return	the Y coordinate of this TuioPoint in units relative to the provided screen height
		 */
		int getScreenY(int height) const { return getSpaceY(height); }




		/**
		 * Returns current time stamp of this TuioPoint as TuioTime 
		 *
		 * @return	the  time stamp of this TuioPoint as TuioTime
		 */
		TuioTime getTuioTime() const;
		/**
		 * Returns the start time of this TuioPoint as TuioTime. 
		 *
		 * @return	the start time of this TuioPoint as TuioTime
		 */
		TuioTime getStartTime() const;
		
		void addPositionThreshold(float thresh);
		
		void removePositionThreshold();

		void addPositionFilter(float mcut, float beta);

		void removePositionFilter();
	};
}
#endif

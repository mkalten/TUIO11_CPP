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

#ifndef INCLUDED_TUIOBLOB3D_H
#define INCLUDED_TUIOBLOB3D_H

#include "TuioContainer.h"

namespace TUIO {

	/**
	 * The TuioBlob3D class encapsulates /tuio/3Dblb TUIO objects.
	 *
	 * @author Nicolas Bremard
	 * @version 1.1.7
	 */
	class LIBDECL TuioBlob3D : public TuioContainer {

	protected:
		/**
		 * The individual blob ID number that is assigned to each TuioBlob3D.
		 */
		int blob_id;

		/**
		 * The roll angle value.
		 */
		float roll;
		/**
		 * The roll speed value.
		 */
		float roll_speed;
		/**
		 * The roll acceleration value.
		 */
		float roll_accel;
		/**
		 * The pitch angle value.
		 */
		float pitch;
		/**
		 * The pitch speed value.
		 */
		float pitch_speed;
		/**
		 * The pitch acceleration value.
		 */
		float pitch_accel;
		/**
		 * The yaw angle value.
		 */
		float yaw;
		/**
		 * The yaw speed value.
		 */
		float yaw_speed;
		/**
		 * The yaw acceleration value.
		 */
		float yaw_accel;

		/**
		 * The rotation acceleration value.
		 */
		float rotation_accel;


		float rollThreshold, pitchThreshold, yawThreshold;
		OneEuroFilter *rollFilter, *pitchFilter, *yawFilter;


		/**
		 * The width value.
		 */
		float width;
		/**
		 * The height value.
		 */
		float height;
		/**
		 * The depth value.
		 */
		float depth;

		
		float widthThreshold, heightThreshold, depthThreshold;
		OneEuroFilter *widthFilter, *heightFilter, *depthFilter;



		/**
		 * The volume value.
		 */
		float volume;

	public:
		using TuioContainer::update;

		/**
		 * This constructor takes a TuioTime argument and assigns it along with the provided
		 * Session ID, X and Y coordinate, width, height and angle to the newly created TuioBlob3D.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	si	the Session ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	d	the depth to assign
		 * @param	v	the volume to assign
		 */
		TuioBlob3D(TuioTime ttime, long si, int bi, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v);

		/**
		 * This constructor takes the provided Session ID, X and Y coordinate
		 *  width, height and angle, and assigs these values to the newly created TuioBlob3D.
		 *
		 * @param	si	the Session ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	d	the depth to assign
		 * @param	v	the volume to assign
		 */
		TuioBlob3D(long si, int bi, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v);

		/**
		 * This constructor takes the atttibutes of the provided TuioBlob3D
		 * and assigs these values to the newly created TuioBlob3D.
		 *
		 * @param	tblb	the TuioBlob3D to assign
		 */
		TuioBlob3D(TuioBlob3D *tblb);

		/**
		 * The destructor is doing nothing in particular.
		 */
		virtual ~TuioBlob3D() {
			if (rollFilter) delete rollFilter;
			if (pitchFilter) delete pitchFilter;
			if (yawFilter) delete yawFilter;
			if (widthFilter) delete widthFilter;
			if (heightFilter) delete heightFilter;
			if (depthFilter) delete depthFilter ;
		};

		/**
		 * Returns the Blob ID of this TuioBlob3D.
		 * @return	the Blob ID of this TuioBlob3D
		 */
		int getBlobID() const;

		/**
		 * Sets the Blob ID of this TuioBlob3D.
		 * @param bi	the new Blob ID for this TuioBlob3D
		 */
		void setBlobID(int bi);

		/**
		 * Takes a TuioTime argument and assigns it along with the provided
		 * X and Y coordinate, angle, X and Y velocity, motion acceleration,
		 * rotation speed and rotation acceleration to the private TuioBlob3D attributes.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	d	the depth to assign
		 * @param	v	the volume to assign
		 * @param	xs	the X velocity to assign
		 * @param	ys	the Y velocity to assign
		 * @param	zs	the Z velocity to assign
		 * @param	ros	the roll orientation speed to assign
		 * @param	pos	the pitch orientation speed to assign
		 * @param	yos	the yaw orientation speed to assign
		 * @param	ma	the motion acceleration to assign
		 * @param	ra	the rotation acceleration to assign
		 */
		void update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra);

		/**
		 * Assigns the provided X and Y coordinate, angle, X and Y velocity, motion acceleration
		 * rotation velocity and rotation acceleration to the private TuioContainer attributes.
		 * The TuioTime time stamp remains unchanged.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	d	the depth to assign
		 * @param	v	the volume to assign
		 * @param	xs	the X velocity to assign
		 * @param	ys	the Y velocity to assign
		 * @param	zs	the Z velocity to assign
		 * @param	ros	the roll orientation speed to assign
		 * @param	pos	the pitch orientation speed to assign
		 * @param	yos	the yaw orientation speed to assign
		 * @param	ma	the motion acceleration to assign
		 * @param	ra	the rotation acceleration to assign
		 */
		void update(float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra);

		/**
		 * Takes a TuioTime argument and assigns it along with the provided
		 * X and Y coordinate and angle to the private TuioBlob3D attributes.
		 * The speed and accleration values are calculated accordingly.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	d	the depth to assign
		 * @param	v	the volume to assign
		 */
		void update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v);

		/**
		 * This method is used to calculate the speed and acceleration values of a
		 * TuioBlob3D with unchanged position and angle.
		 */
		void stop(TuioTime ttime);

		/**
		 * Takes the atttibutes of the provided TuioBlob3D
		 * and assigs these values to this TuioBlob3D.
		 * The TuioTime time stamp of this TuioContainer remains unchanged.
		 *
		 * @param	tblb	the TuioContainer to assign
		 */
		void update(TuioBlob3D *tblb);

		/**
		 * Returns the width of this TuioBlob3D.
		 * @return	the width of this TuioBlob3D
		 */
		float getWidth() const;

		/**
		 * Returns the height of this TuioBlob3D.
		 * @return	the height of this TuioBlob3D
		 */
		float getHeight() const;

		/**
		 * Returns the depth of this TuioBlob3D.
		 * @return	the depth of this TuioBlob3D
		 */
		float getDepth() const;

		/**
		 * Returns the width of this TuioBlob3D in real unit.
		 * @return	the width of this TuioBlob3D
		 */
		int getSpaceWidth(int w) const;

		/**
		 * Returns the height of this TuioBlob3D in real unit.
		 * @return	the height of this TuioBlob3D
		 */
		int getSpaceHeight(int h) const;

		/**
		 * Returns the depth of this TuioBlob3D in real unit.
		 * @return	the depth of this TuioBlob3D
		 */
		int getSpaceDepth(int d) const;

		/**
		 * Returns the volume of this TuioBlob3D.
		 * @return	the volume of this TuioBlob3D
		 */
		float getVolume() const;

		


		/**
		 * Returns the roll angle of this TuioObject.
		 * @return	the roll angle of this TuioObject
		 */
		float getRoll() const;

		/**
		 * Returns the roll angle in degrees of this TuioObject.
		 * @return	the roll angle in degrees of this TuioObject
		 */
		float getRollDegrees() const;

		/**
		 * Returns the roll speed of this TuioObject.
		 * @return	the roll speed of this TuioObject
		 */
		float getRollSpeed() const;

		/**
		 * Returns the roll accel of this TuioObject.
		 * @return	the roll accel of this TuioObject
		 */
		float getRollAccel() const;

		/**
		 * Returns the pitch angle of this TuioObject.
		 * @return	the pitch angle of this TuioObject
		 */
		float getPitch() const;

		/**
		 * Returns the pitch angle in degrees of this TuioObject.
		 * @return	the pitch angle in degrees of this TuioObject
		 */
		float getPitchDegrees() const;

		/**
		 * Returns the pitch speed of this TuioObject.
		 * @return	the pitch speed of this TuioObject
		 */
		float getPitchSpeed() const;

		/**
		 * Returns the pitch accel of this TuioObject.
		 * @return	the pitch accel of this TuioObject
		 */
		float getPitchAccel() const;

		/**
		 * Returns the yaw angle of this TuioObject.
		 * @return	the yaw angle of this TuioObject
		 */
		float getYaw() const;

		/**
		 * Returns the yaw angle in degrees of this TuioObject.
		 * @return	the yaw angle in degrees of this TuioObject
		 */
		float getYawDegrees() const;

		/**
		 * Returns the yaw speed of this TuioObject.
		 * @return	the yaw speed of this TuioObject
		 */
		float getYawSpeed() const;

		/**
		 * Returns the yaw accel of this TuioObject.
		 * @return	the yaw accel of this TuioObject
		 */
		float getYawAccel() const;




		/**
		 * Returns the rotation acceleration of this TuioObject.
		 * @return	the rotation acceleration of this TuioObject
		 */
		float getRotationAccel() const;



		/**
		 * Returns true of this TuioBlob3D is moving.
		 * @return	true of this TuioBlob3D is moving
		 */
		bool isMoving() const;


		void addRollThreshold(float thresh);
		void addPitchThreshold(float thresh);
		void addYawThreshold(float thresh);

		void removeRollThreshold();
		void removePitchThreshold();
		void removeYawThreshold();

		void addRollFilter(float mcut, float beta);
		void addPitchFilter(float mcut, float beta);
		void addYawFilter(float mcut, float beta);

		void removeRollFilter();
		void removePitchFilter();
		void removeYawFilter();





		void addWidthThreshold(float thresh);
		void addHeightThreshold(float thresh);
		void addDepthThreshold(float thresh);

		void removeWidthThreshold();
		void removeHeightThreshold();
		void removeDepthThreshold();

		void addWidthFilter(float mcut, float beta);
		void addHeightFilter(float mcut, float beta);
		void addDepthFilter(float mcut, float beta);

		void removeWidthFilter();
		void removeHeightFilter();
		void removeDepthFilter();

	};
}
#endif

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

#ifndef INCLUDED_TUIOOBJECT3D_H
#define INCLUDED_TUIOOBJECT3D_H

#include "TuioContainer.h"

namespace TUIO {

	/**
	 * The TuioObject3D class encapsulates /tuio/3Dobj TUIO objects.
	 *
	 * @author Nicolas Bremard
	 * @version 1.1.7
	 */
	class LIBDECL TuioObject3D : public TuioContainer {

	protected:
		/**
		 * The individual symbol ID number that is assigned to each TuioObject.
		 */
		int symbol_id;
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

		float rollThreshold,pitchThreshold,yawThreshold;
		OneEuroFilter *rollFilter, *pitchFilter, *yawFilter;

	public:
		using TuioContainer::update;

		/**
		 * This constructor takes a TuioTime argument and assigns it along with the provided
		 * Session ID, Symbol ID, X and Y coordinate and angle to the newly created TuioObject.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	si	the Session ID  to assign
		 * @param	sym	the Symbol ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 */
		TuioObject3D(TuioTime ttime, long si, int sym, float xp, float yp, float zp, float ro, float po, float yo);

		/**
		 * This constructor takes the provided Session ID, Symbol ID, X and Y coordinate
		 * and angle, and assigs these values to the newly created TuioObject.
		 *
		 * @param	si	the Session ID  to assign
		 * @param	sym	the Symbol ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 */
		TuioObject3D(long si, int sym, float xp, float yp, float zp, float ro, float po, float yo);

		/**
		 * This constructor takes the atttibutes of the provided TuioObject
		 * and assigs these values to the newly created TuioObject.
		 *
		 * @param	tobj	the TuioObject to assign
		 */
		TuioObject3D(TuioObject3D *tobj);

		/**
		 * The destructor is doing nothing in particular.
		 */
		virtual ~TuioObject3D() {
			if (rollFilter) delete rollFilter;
			if (pitchFilter) delete pitchFilter;
			if (yawFilter) delete yawFilter;
		};

		/**
		 * Takes a TuioTime argument and assigns it along with the provided
		 * X and Y coordinate, angle, X and Y velocity, motion acceleration,
		 * rotation speed and rotation acceleration to the private TuioObject attributes.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 * @param	xs	the X velocity to assign
		 * @param	ys	the Y velocity to assign
		 * @param	zs	the Z velocity to assign
		 * @param	ros	the roll orientation speed to assign
		 * @param	pos	the pitch orientation speed to assign
		 * @param	yos	the yaw orientation speed to assign
		 * @param	ma	the motion acceleration to assign
		 * @param	ra	the rotation acceleration to assign
		 */
		void update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra);

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
		 * @param	xs	the X velocity to assign
		 * @param	ys	the Y velocity to assign
		 * @param	zs	the Z velocity to assign
		 * @param	ros	the roll orientation speed to assign
		 * @param	pos	the pitch orientation speed to assign
		 * @param	yos	the yaw orientation speed to assign
		 * @param	ma	the motion acceleration to assign
		 * @param	ra	the rotation acceleration to assign
		 */
		void update(float xp, float yp, float zp, float ro, float po, float yo, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra);

		/**
		 * Takes a TuioTime argument and assigns it along with the provided
		 * X and Y coordinate and angle to the private TuioObject attributes.
		 * The speed and accleration values are calculated accordingly.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	ro	the roll orientation to assign
		 * @param	po	the pitch orientation to assign
		 * @param	yo	the yaw orientation to assign
		 */
		void update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo);

		/**
		 * This method is used to calculate the speed and acceleration values of a
		 * TuioObject with unchanged position and angle.
		 */
		void stop(TuioTime ttime);

		/**
		 * Takes the atttibutes of the provided TuioObject
		 * and assigs these values to this TuioObject.
		 * The TuioTime time stamp of this TuioContainer remains unchanged.
		 *
		 * @param	tobj	the TuioContainer to assign
		 */
		void update(TuioObject3D *tobj);

		/**
		 * Returns the symbol ID of this TuioObject.
		 * @return	the symbol ID of this TuioObject
		 */
		int getSymbolID() const;


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
		 * Returns true of this TuioObject is moving.
		 * @return	true of this TuioObject is moving
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
	};
}
#endif

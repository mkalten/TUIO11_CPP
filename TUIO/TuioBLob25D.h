/*
 TUIO C++ Library
 Copyright (c) 2022 Nicolas Bremard <nicolas@bremard.fr>
 Based on TuioBlob by Martin Kaltenbrunner <martin@tuio.org>

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

#ifndef INCLUDED_TUIOBLOB25D_H
#define INCLUDED_TUIOBLOB25D_H

#include "TuioContainer.h"

namespace TUIO {

	/**
	 * The TuioBlob2.5D class encapsulates /tuio/25Dblb TUIO objects.
	 *
	 * @author Nicolas Bremard
	 * @version 1.1.7
	 */
	class LIBDECL TuioBlob25D : public TuioContainer {

	protected:
		/**
		 * The individual blob ID number that is assigned to each TuioBlob25D.
		 */
		int blob_id;
		/**
		 * The rotation angle value.
		 */
		float angle;
		/**
		 * The width value.
		 */
		float width;
		/**
		 * The height value.
		 */
		float height;
		/**
		 * The area value.
		 */
		float area;
		/**
		 * The rotation speed value.
		 */
		float rotation_speed;
		/**
		 * The rotation acceleration value.
		 */
		float rotation_accel;

		float angleThreshold;
		OneEuroFilter *angleFilter;
		float sizeThreshold;
		OneEuroFilter *widthFilter;
		OneEuroFilter *heightFilter;

	public:
		using TuioContainer::update;

		/**
		 * This constructor takes a TuioTime argument and assigns it along with the provided
		 * Session ID, X and Y coordinate, width, height and angle to the newly created TuioBlob25D.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	si	the Session ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	a	the angle to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	f	the area to assign
		 */
		TuioBlob25D(TuioTime ttime, long si, int bi, float xp, float yp, float zp, float a, float w, float h, float f);

		/**
		 * This constructor takes the provided Session ID, X and Y coordinate
		 *  width, height and angle, and assigs these values to the newly created TuioBlob25D.
		 *
		 * @param	si	the Session ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	a	the angle to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	f	the area to assign
		 */
		TuioBlob25D(long si, int bi, float xp, float yp, float zp, float a, float  w, float h, float f);

		/**
		 * This constructor takes the atttibutes of the provided TuioBlob25D
		 * and assigs these values to the newly created TuioBlob25D.
		 *
		 * @param	tblb	the TuioBlob25D to assign
		 */
		TuioBlob25D(TuioBlob25D *tblb);

		/**
		 * The destructor is doing nothing in particular.
		 */
		virtual ~TuioBlob25D() {
			if (widthFilter) delete widthFilter;
			if (heightFilter) delete heightFilter;
			if (angleFilter) delete angleFilter;
		};

		/**
		 * Returns the Blob ID of this TuioBlob25D.
		 * @return	the Blob ID of this TuioBlob25D
		 */
		int getBlobID() const;

		/**
		 * Sets the Blob ID of this TuioBlob25D.
		 * @param bi	the new Blob ID for this TuioBlob25D
		 */
		void setBlobID(int bi);

		/**
		 * Takes a TuioTime argument and assigns it along with the provided
		 * X and Y coordinate, angle, X and Y velocity, motion acceleration,
		 * rotation speed and rotation acceleration to the private TuioBlob25D attributes.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	a	the rotation angle to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	f	the area to assign
		 * @param	xs	the X velocity to assign
		 * @param	ys	the Y velocity to assign
		 * @param	zs	the Z velocity to assign
		 * @param	rs	the rotation velocity to assign
		 * @param	ma	the motion acceleration to assign
		 * @param	ra	the rotation acceleration to assign
		 */
		void update(TuioTime ttime, float xp, float yp, float zp, float a, float w, float h, float f, float xs, float ys, float zs, float rs, float ma, float ra);

		/**
		 * Assigns the provided X and Y coordinate, angle, X and Y velocity, motion acceleration
		 * rotation velocity and rotation acceleration to the private TuioContainer attributes.
		 * The TuioTime time stamp remains unchanged.
		 *
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	a	the angle to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	f	the area to assign
		 * @param	xs	the X velocity to assign
		 * @param	ys	the Y velocity to assign
		 * @param	zs	the Z velocity to assign
		 * @param	rs	the rotation velocity to assign
		 * @param	ma	the motion acceleration to assign
		 * @param	ra	the rotation acceleration to assign
		 */
		void update(float xp, float yp, float zp, float a, float w, float h, float f, float xs, float ys, float zs, float rs, float ma, float ra);

		/**
		 * Takes a TuioTime argument and assigns it along with the provided
		 * X and Y coordinate and angle to the private TuioBlob25D attributes.
		 * The speed and accleration values are calculated accordingly.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	a	the angle coordinate to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	f	the area to assign
		 */
		void update(TuioTime ttime, float xp, float yp, float zp, float a, float w, float h, float f);

		/**
		 * This method is used to calculate the speed and acceleration values of a
		 * TuioBlob25D with unchanged position and angle.
		 */
		void stop(TuioTime ttime);

		/**
		 * Takes the atttibutes of the provided TuioBlob25D
		 * and assigs these values to this TuioBlob25D.
		 * The TuioTime time stamp of this TuioContainer remains unchanged.
		 *
		 * @param	tblb	the TuioContainer to assign
		 */
		void update(TuioBlob25D *tblb);

		/**
		 * Returns the width of this TuioBlob25D.
		 * @return	the width of this TuioBlob25D
		 */
		float getWidth() const;

		/**
		 * Returns the height of this TuioBlob25D.
		 * @return	the height of this TuioBlob25D
		 */
		float getHeight() const;

		/**
		 * Returns the width of this TuioBlob25D in real unit.
		 * @return	the width of this TuioBlob25D
		 */
		int getSpaceWidth(int w) const;

		/**
		 * Returns the height of this TuioBlob25D in real unit.
		 * @return	the height of this TuioBlob25D
		 */
		int getSpaceHeight(int h) const;

		/**
		 * Returns the area of this TuioBlob25D.
		 * @return	the area of this TuioBlob25D
		 */
		float getArea() const;

		/**
		 * Returns the rotation angle of this TuioBlob25D.
		 * @return	the rotation angle of this TuioBlob25D
		 */
		float getAngle() const;

		/**
		 * Returns the rotation angle in degrees of this TuioBlob25D.
		 * @return	the rotation angle in degrees of this TuioBlob25D
		 */
		float getAngleDegrees() const;

		/**
		 * Returns the rotation speed of this TuioBlob25D.
		 * @return	the rotation speed of this TuioBlob25D
		 */
		float getRotationSpeed() const;

		/**
		 * Returns the rotation acceleration of this TuioBlob25D.
		 * @return	the rotation acceleration of this TuioBlob25D
		 */
		float getRotationAccel() const;

		/**
		 * Returns true of this TuioBlob25D is moving.
		 * @return	true of this TuioBlob25D is moving
		 */
		bool isMoving() const;

		void addAngleThreshold(float thresh);

		void removeAngleThreshold();

		void addAngleFilter(float mcut, float beta);

		void removeAngleFilter();

		void addSizeThreshold(float thresh);

		void removeSizeThreshold();

		void addSizeFilter(float mcut, float beta);

		void removeSizeFilter();
	};
}
#endif

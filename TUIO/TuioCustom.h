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

#ifndef INCLUDED_TUIOCUSTOM_H
#define INCLUDED_TUIOCUSTOM_H


#include "TuioContainer.h"

namespace TUIO {

	/**
	 * The TuioCustom class encapsulates /tuio/ TUIO custom messages.
	 *
	 * @author Nicolas Bremard
	 * @version 1.1.7
	 */
	class LIBDECL TuioCustom {


	public:



		/**
		 * This constructor takes the provided Session ID, X and Y coordinate
		 *  width, height and angle, and assigs these values to the newly created TuioBlob.
		 *
		 * @param	si	the Session ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	a	the angle to assign
		 * @param	w	the width to assign
		 * @param	h	the height to assign
		 * @param	f	the area to assign
		 */
		TuioCustom();

		/**
		 * This constructor takes the atttibutes of the provided TuioBlob
		 * and assigs these values to the newly created TuioBlob.
		 *
		 * @param	tblb	the TuioBlob to assign
		 */
		TuioCustom(TuioCustom *tblb);

		/**
		 * The destructor is doing nothing in particular.
		 */
	    ~TuioCustom() {};




	
	};
}
#endif

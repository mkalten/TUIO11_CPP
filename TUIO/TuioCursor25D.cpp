/*
 TUIO C++ Library
 Copyright (c) 2022 Nicolas Bremard <nicolas@bremard.fr>
 Based on TuioCursor by Martin Kaltenbrunner <martin@tuio.org>

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

#include "TuioCursor25D.h"

using namespace TUIO;

TuioCursor25D::TuioCursor25D(TuioTime ttime, long si, int ci, float xp, float yp, float zp) :TuioContainer(ttime, si, xp, yp, zp) {
	cursor_id = ci;
}

TuioCursor25D::TuioCursor25D(long si, int ci, float xp, float yp, float zp) : TuioContainer(si, xp, yp, zp) {
	cursor_id = ci;
}

TuioCursor25D::TuioCursor25D(TuioCursor25D *tcur) : TuioContainer(tcur) {
	cursor_id = tcur->getCursorID();
}

int TuioCursor25D::getCursorID() const {
	return cursor_id;
};


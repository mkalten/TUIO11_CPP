/*
	TUIO C++ Example - part of the reacTIVision project
	http://reactivision.sourceforge.net/
	Copyright (c) 2005-2016 Martin Kaltenbrunner <martin@tuio.org>

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	Any person wishing to distribute modifications to the Software is
	requested to send the modifications to the original developer so that
	they can be incorporated into the canonical version.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef INCLUDED_TUIODUMP_H
#define INCLUDED_TUIODUMP_H

#include "TuioListener.h"
#include "TuioClient.h"
#include "UdpReceiver.h"
#include "TcpReceiver.h"
#include <math.h>

using namespace TUIO;

class TuioDump : public TuioListener {
	
	public:
		void addTuioObject(TuioObject *tobj);
		void updateTuioObject(TuioObject *tobj);
		void removeTuioObject(TuioObject *tobj);

		void addTuioCursor(TuioCursor *tcur);
		void updateTuioCursor(TuioCursor *tcur);
		void removeTuioCursor(TuioCursor *tcur);

		void addTuioBlob(TuioBlob *tblb);
		void updateTuioBlob(TuioBlob *tblb);
		void removeTuioBlob(TuioBlob *tblb);

		void addTuioObject25D(TuioObject25D *tobj);
		void updateTuioObject25D(TuioObject25D *tobj);
		void removeTuioObject25D(TuioObject25D *tobj);

		void addTuioCursor25D(TuioCursor25D *tcur);
		void updateTuioCursor25D(TuioCursor25D *tcur);
		void removeTuioCursor25D(TuioCursor25D *tcur);

		void addTuioBlob25D(TuioBlob25D *tblb);
		void updateTuioBlob25D(TuioBlob25D *tblb);
		void removeTuioBlob25D(TuioBlob25D *tblb);


		void addTuioObject3D(TuioObject3D *tobj);
		void updateTuioObject3D(TuioObject3D *tobj);
		void removeTuioObject3D(TuioObject3D *tobj);

		void addTuioCursor3D(TuioCursor3D *tcur);
		void updateTuioCursor3D(TuioCursor3D *tcur);
		void removeTuioCursor3D(TuioCursor3D *tcur);

		void addTuioBlob3D(TuioBlob3D *tblb);
		void updateTuioBlob3D(TuioBlob3D *tblb);
		void removeTuioBlob3D(TuioBlob3D *tblb);

		void refresh(TuioTime frameTime);
};

#endif /* INCLUDED_TUIODUMP_H */

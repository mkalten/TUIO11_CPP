/*
 TUIO C++ GUI Demo
 Copyright (c) 2005-2016 Martin Kaltenbrunner <martin@tuio.org>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_TUIODEMO_H
#define INCLUDED_TUIODEMO_H

#include "TuioListener.h"
#include "TuioClient.h"
#include "UdpReceiver.h"
#include "TcpReceiver.h"
#include <list>
#include <math.h>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include "SDL.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

using namespace TUIO;

class TuioDemo : public TuioListener { 
	
public:
	TuioDemo(const char *host, int port, bool udp, bool verbose, bool fullscreen);
	~TuioDemo() {
		tuioClient->disconnect();
		delete tuioClient;
		delete osc_receiver;
	}
	
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
	
	void run();
	
private:
	void drawObjects();
	void drawString(char *str);
	void processEvents();
	void initWindow();
	SDL_Window *window;
	SDL_Renderer *renderer;
	bool verbose, fullscreen, running;
	
	int width, height;
	int screen_width, screen_height;
	int window_width, window_height;

	TuioClient *tuioClient;
	OscReceiver *osc_receiver;
};

#endif /* INCLUDED_TUIODEMO_H */

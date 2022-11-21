/*
 TUIO C++ Server Demo
 Copyright (c) 2005-2016 Martin Kaltenbrunner <martin@tuio.org>
 Modified by Bremard Nicolas <nicolas@bremard.fr> on 11/2022
 
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

#ifndef INCLUDED_SimpleSimulator_H
#define INCLUDED_SimpleSimulator_H

#include "TuioServer.h"
#include "TuioCursor.h"
#include "osc/OscTypes.h"
#include <list>
#include <math.h>

#include "FlashSender.h"
#include "TcpSender.h"
#include "WebSockSender.h"

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

class SimpleSimulator { 
	
public:
	SimpleSimulator(TuioServer *server);
	~SimpleSimulator() {};
	
	void run();
	TuioServer *tuioServer;
	int mode; // 0: 2Dcur; 1: 2Dobj; 2: 2Dblb; 3: 25Dcur; 4: 25Dobj; 5: 25Dblb; 6: 3Dcur; 7: 3Dobj; 8: 3Dblb
	int control;// 0: z; 1: roll/angle; 2: pitch; 3: yaw; 4: width; 5: height; 6: depth

	std::list<TuioCursor*> stickyCursorList;
	std::list<TuioCursor*> jointCursorList;
	std::list<TuioCursor*> activeCursorList;

	int objectID;
	std::list<TuioObject*> stickyObjectList;
	std::list<TuioObject*> jointObjectList;
	std::list<TuioObject*> activeObjectList;

	std::list<TuioBlob*> stickyBlobList;
	std::list<TuioBlob*> jointBlobList;
	std::list<TuioBlob*> activeBlobList;


	std::list<TuioCursor25D*> stickyCursor25DList;
	std::list<TuioCursor25D*> jointCursor25DList;
	std::list<TuioCursor25D*> activeCursor25DList;

	int object25DID;
	std::list<TuioObject25D*> stickyObject25DList;
	std::list<TuioObject25D*> jointObject25DList;
	std::list<TuioObject25D*> activeObject25DList;

	std::list<TuioBlob25D*> stickyBlob25DList;
	std::list<TuioBlob25D*> jointBlob25DList;
	std::list<TuioBlob25D*> activeBlob25DList;



	std::list<TuioCursor3D*> stickyCursor3DList;
	std::list<TuioCursor3D*> jointCursor3DList;
	std::list<TuioCursor3D*> activeCursor3DList;

	int object3DID;
	std::list<TuioObject3D*> stickyObject3DList;
	std::list<TuioObject3D*> jointObject3DList;
	std::list<TuioObject3D*> activeObject3DList;

	std::list<TuioBlob3D*> stickyBlob3DList;
	std::list<TuioBlob3D*> jointBlob3DList;
	std::list<TuioBlob3D*> activeBlob3DList;
	
private:
	void drawFrame();
	void drawString(const char *str);
	void processEvents();
	void initWindow();

	SDL_Window *window;
	SDL_Renderer *renderer;
	bool verbose, fullupdate, periodic, fullscreen, running, help;
	
	int width, height;
	int screen_width, screen_height;
	int window_width, window_height;
	TuioTime frameTime;
	
	void mousePressed(float x, float y);
	void mouseReleased(float x, float y);
	void mouseDragged(float x, float y);
	void applyExtraDimension(float scroll);
	//int s_id;
};

#endif /* INCLUDED_SimpleSimulator_H */

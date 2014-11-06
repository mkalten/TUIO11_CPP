/*
	TUIO C++ Server Demo
	Copyright (c) 2005-2012 Martin Kaltenbrunner <martin@tuio.org>

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

#include "SimpleSimulator.h"
void SimpleSimulator::drawFrame() {

	if(!running) return;
	glClear(GL_COLOR_BUFFER_BIT);
	char id[3];

	// draw the cursors
	std::list<TuioCursor*> cursorList = tuioServer->getTuioCursors();
	for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		std::list<TuioPoint> path = tcur->getPath();
		if (path.size()>0) {

			TuioPoint last_point = path.front();
			glBegin(GL_LINES);
			glColor3f(0.0, 0.0, 1.0);

			for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
				glVertex3f(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
				glVertex3f(point->getScreenX(width), point->getScreenY(height), 0.0f);
				last_point.update(point->getX(),point->getY());
			}
			glEnd();

			// draw the finger tip
			glColor3f(0.75, 0.75, 0.75);
			std::list<TuioCursor*>::iterator joint = std::find( jointCursorList.begin(), jointCursorList.end(), tcur );
			if( joint != jointCursorList.end() ) {
				glColor3f(0.5, 0.5, 0.5);
			}
			glPushMatrix();
			glTranslatef(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
			glBegin(GL_TRIANGLE_FAN);
			for(double a = 0.0f; a <= 2*M_PI; a += 0.2f) {
				glVertex2d(cos(a) * height/100.0f, sin(a) * height/100.0f);
			}
			glEnd();
			glPopMatrix();

			glColor3f(0.0, 0.0, 0.0);
			glRasterPos2f(tcur->getScreenX(width),tcur->getScreenY(height));
			sprintf(id,"%d",tcur->getCursorID());
			drawString(id);
		}
	}

	if (help) {
		glColor3f(0.0, 0.0, 1.0);
		glRasterPos2f(10,20);
		drawString("h - toggle help display");
		glRasterPos2f(10,35);
		if (verbose) drawString("v - disable verbose mode");
		else drawString("v - enable verbose mode");
		glRasterPos2f(10,50);
		drawString("r - reset session");
		glRasterPos2f(10,65);
		if (tuioServer->fullUpdateEnabled()) drawString("f - disable full update");
		else drawString("f - enable full update");
		glRasterPos2f(10,80);
		if (tuioServer->periodicMessagesEnabled()) drawString("p - disable periodic messages");
		else drawString("p - enable periodic messages");
		glRasterPos2f(10,95);
		drawString("SHIFT click - create persistent cursor");
		glRasterPos2f(10,110);
		drawString("CTRL click - add to cursor group");
		glRasterPos2f(10,125);
		if (fullscreen) drawString("F1 - exit fullscreen mode");
		else drawString("F1 - enter fullscreen mode");
		glRasterPos2f(10,140);
		drawString("ESC - Quit");
	}

	SDL_GL_SwapBuffers();
}

void SimpleSimulator::drawString(const char *str) {
	if (str && strlen(str)) {
		while (*str) {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *str);
			str++;
		}
	}
}

void SimpleSimulator::processEvents()
{
    SDL_Event event;

    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
		case SDL_KEYDOWN:
			if( event.key.keysym.sym == SDLK_ESCAPE ){
				running = false;
				SDL_Quit();
			} else if( event.key.keysym.sym == SDLK_F1 ){
				fullscreen=!fullscreen;
				initWindow();
			} else if( event.key.keysym.sym == SDLK_f ){
				fullupdate=!tuioServer->fullUpdateEnabled();
				if (fullupdate) tuioServer->enableFullUpdate();
				else tuioServer->disableFullUpdate();
			} else if( event.key.keysym.sym == SDLK_p ){
				periodic=!tuioServer->periodicMessagesEnabled();
				if (periodic) tuioServer->enablePeriodicMessages();
				else tuioServer->disablePeriodicMessages();
			} else if( event.key.keysym.sym == SDLK_v ){
				verbose = !verbose;
				tuioServer->setVerbose(verbose);
			} else if( event.key.keysym.sym == SDLK_h ){
				help = !help;
			} else if( event.key.keysym.sym == SDLK_r ){
				tuioServer->resetTuioCursors();
				stickyCursorList.clear();
				jointCursorList.clear();
				activeCursorList.clear();
			}
			break;

		case SDL_MOUSEMOTION:
			if (event.button.button == SDL_BUTTON_LEFT) mouseDragged((float)event.button.x/width, (float)event.button.y/height);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) mousePressed((float)event.button.x/width, (float)event.button.y/height);
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) mouseReleased((float)event.button.x/width, (float)event.button.y/height);
			break;
		case SDL_QUIT:
			running = false;
			SDL_ShowCursor(true);
			SDL_Quit();
			break;
        }
    }
}

void SimpleSimulator::mousePressed(float x, float y) {
	//printf("pressed %f %f\n",x,y);

	TuioCursor *match = NULL;
	float distance  = 0.05f;
	for (std::list<TuioCursor*>::iterator iter = stickyCursorList.begin(); iter!=stickyCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x,y);
		if ((test < distance) && (test < 8.0f/width)) {
			distance = test;
			match = tcur;
		}
	}

	Uint8 *keystate = SDL_GetKeyState(NULL);
	if ((keystate[SDLK_LSHIFT]) || (keystate[SDLK_RSHIFT]))  {

		if (match!=NULL) {
			std::list<TuioCursor*>::iterator joint = std::find( jointCursorList.begin(), jointCursorList.end(), match );
			if( joint != jointCursorList.end() ) {
				jointCursorList.erase( joint );
			}
			stickyCursorList.remove(match);
			activeCursorList.remove(match);
			tuioServer->removeTuioCursor(match);
		} else {
			TuioCursor *cursor = tuioServer->addTuioCursor(x,y);
			stickyCursorList.push_back(cursor);
			activeCursorList.push_back(cursor);
		}
	} else if ((keystate[SDLK_LCTRL]) || (keystate[SDLK_RCTRL])) {

		if (match!=NULL) {
			std::list<TuioCursor*>::iterator joint = std::find( jointCursorList.begin(), jointCursorList.end(), match );
			if( joint != jointCursorList.end() ) {
				jointCursorList.remove( match );
			} else jointCursorList.push_back(match);
		}
	} else {
		if (match==NULL) {
			TuioCursor *cursor = tuioServer->addTuioCursor(x,y);
			activeCursorList.push_back(cursor);
		} else activeCursorList.push_back(match);
	}
}

void SimpleSimulator::mouseDragged(float x, float y) {
	//printf("dragged %f %f\n",x,y);

	TuioCursor *cursor = NULL;
	float distance  = width;
	for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter!=activeCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x,y);
		if (test<distance) {
			distance = test;
			cursor = tcur;
		}
	}

	if (cursor==NULL) return;
	if (cursor->getTuioTime()==frameTime) return;

	std::list<TuioCursor*>::iterator joint = std::find( jointCursorList.begin(), jointCursorList.end(), cursor );
	if( joint != jointCursorList.end() ) {
		float dx = x-cursor->getX();
		float dy = y-cursor->getY();
		for (std::list<TuioCursor*>::iterator iter = jointCursorList.begin(); iter!=jointCursorList.end(); iter++) {
			TuioCursor *jointCursor = (*iter);
			 tuioServer->updateTuioCursor(jointCursor,jointCursor->getX()+dx,jointCursor->getY()+dy);
		}
	} else tuioServer->updateTuioCursor(cursor,x,y);
}

void SimpleSimulator::mouseReleased(float x, float y) {
	//printf("released %f %f\n",x,y);

	TuioCursor *cursor = NULL;
	float distance  = 0.05f;
	for (std::list<TuioCursor*>::iterator iter = stickyCursorList.begin(); iter!=stickyCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x,y);
		if (test<distance) {
			distance = test;
			cursor = tcur;
		}
	}

	if (cursor!=NULL) {
		activeCursorList.remove(cursor);
		return;
	}

	distance = 0.05f;
	for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter!=activeCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x,y);
		if (test<distance) {
			distance = test;
			cursor = tcur;
		}
	}

	if (cursor!=NULL) {
		activeCursorList.remove(cursor);
		tuioServer->removeTuioCursor(cursor);
	}
}

SimpleSimulator::SimpleSimulator(TuioServer *server)
	: verbose		(false)
	, fullupdate	(false)
	, periodic		(false)
	, fullscreen	(false)
	, help			(false)
	, screen_width	(1024)
	, screen_height	(768)
	, window_width	(640)
	, window_height	(480)
{

	TuioTime::initSession();
	frameTime = TuioTime::getSessionTime();

	tuioServer = server;
	tuioServer->setSourceName("SimpleSimulator");
	tuioServer->enableObjectProfile(false);
	tuioServer->enableBlobProfile(false);

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "SDL could not be initialized: " <<  SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	initWindow();
}

void SimpleSimulator::initWindow() {

	int videoFlags = SDL_OPENGL | SDL_DOUBLEBUF;
	if( fullscreen ) {
		videoFlags |= SDL_FULLSCREEN;
		SDL_ShowCursor(false);
		width = screen_width;
		height = screen_height;
	} else {
		width = window_width;
		height = window_height;
	}

	window = SDL_SetVideoMode(width, height, 32, videoFlags);
	if ( window == NULL ) {
		std::cerr << "Could not open window: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	SDL_ShowCursor(true);
	SDL_WM_SetCaption("SimpleSimulator", NULL);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (float)width,  (float)height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void SimpleSimulator::run() {
	running=true;
	while (running) {
		frameTime = TuioTime::getSessionTime();
		tuioServer->initFrame(frameTime);
		processEvents();
		tuioServer->stopUntouchedMovingCursors();
		tuioServer->commitFrame();
		drawFrame();

		// simulate 50Hz compensating the previous processing time
		int delay = 20 - (TuioTime::getSessionTime().getTotalMilliseconds() - frameTime.getTotalMilliseconds());
		if (delay>0) SDL_Delay(delay);
	}
}

int main(int argc, char* argv[])
{
/*	if (( argc != 1) && ( argc != 3)) {
        	std::cout << "usage: SimpleSimulator [host] [port]\n";
        	return 0;
	}*/

#ifndef __MACOSX__
	glutInit(&argc,argv);
#endif

	TuioServer *server;
	if( argc == 3 ) {
		server = new TuioServer(argv[1],atoi(argv[2]));
	} else server = new TuioServer(); // default is UDP port 3333 on localhost

	// add an additional TUIO/TCP sender
	OscSender *tcp_sender;
	if( argc == 2 ) {
		tcp_sender = new TcpSender(atoi(argv[1]));
	} else if ( argc == 3 ) {
		tcp_sender = new TcpSender(argv[1],atoi(argv[2]));
	} else tcp_sender = new TcpSender(3333);
	server->addOscSender(tcp_sender);

	// add an additional TUIO/FLC sender
	OscSender *flash_sender = new FlashSender();
	server->addOscSender(flash_sender);

	SimpleSimulator *app = new SimpleSimulator(server);
	app->run();

	delete app;
	delete server;
#ifndef LINUX
	delete flash_sender;
#endif
	delete tcp_sender;
	return 0;
}



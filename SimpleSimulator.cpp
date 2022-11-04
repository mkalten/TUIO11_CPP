/*
	TUIO C++ Server Demo
	Copyright (c)

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

	if (!running) return;
	glClear(GL_COLOR_BUFFER_BIT);
	char id[20];

	// draw the cursors
	std::list<TuioCursor*> cursorList = tuioServer->getTuioCursors();
	for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter != cursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		std::list<TuioPoint> path = tcur->getPath();
		if (path.size() > 0) {

			TuioPoint last_point = path.front();
			glBegin(GL_LINES);
			glColor3f(0.0, 0.0, 1.0);

			for (std::list<TuioPoint>::iterator point = path.begin(); point != path.end(); point++) {
				glVertex3f(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
				glVertex3f(point->getScreenX(width), point->getScreenY(height), 0.0f);
				last_point.update(point->getX(), point->getY());
			}
			glEnd();

			// draw the finger tip
			glColor3f(0.75, 0.75, 0.75);
			std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), tcur);
			if (joint != jointCursorList.end()) {
				glColor3f(0.5, 0.5, 0.5);
			}
			glPushMatrix();
			glTranslatef(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
			glBegin(GL_TRIANGLE_FAN);
			for (double a = 0.0f; a <= 2 * M_PI; a += 0.2f) {
				glVertex2d(cos(a) * height / 100.0f, sin(a) * height / 100.0f);
			}
			glEnd();
			glPopMatrix();

			glColor3f(0.0, 0.0, 0.0);
			glRasterPos2f(tcur->getScreenX(width), tcur->getScreenY(height));
			sprintf(id, "2Dcur %d", tcur->getCursorID());
			drawString(id);
		}
	}


	// draw the objects
	std::list<TuioObject*> objectList = tuioServer->getTuioObjects();
	for (std::list<TuioObject*>::iterator iter = objectList.begin(); iter != objectList.end(); iter++) {
		TuioObject *tuioObject = (*iter);
		int pos_size = height / 20.0f;
		int neg_size = -1 * pos_size;
		float xpos = tuioObject->getScreenX(width);
		float ypos = tuioObject->getScreenY(height);
		float angle = tuioObject->getAngleDegrees();

		glColor3f(0.75, 0.75, 0.75);
		std::list<TuioObject*>::iterator joint = std::find(jointObjectList.begin(), jointObjectList.end(), tuioObject);
		if (joint != jointObjectList.end()) {
			glColor3f(0.5, 0.5, 0.5);
		}


		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
		glVertex2f(neg_size, neg_size);
		glVertex2f(neg_size, pos_size);
		glVertex2f(pos_size, pos_size);
		glVertex2f(pos_size, neg_size);
		glEnd();
		glPopMatrix();

		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f(xpos, ypos + 5);
		sprintf(id, "2Dobj %d", tuioObject->getSymbolID());
		drawString(id);
	}

	// draw the blobs
	std::list<TuioBlob*> blobList = tuioServer->getTuioBlobs();
	for (std::list<TuioBlob*>::iterator iter = blobList.begin(); iter != blobList.end(); iter++) {
		TuioBlob *tuioBlob = (*iter);
		float blob_width = tuioBlob->getScreenWidth(width) / 2;
		float blob_height = tuioBlob->getScreenHeight(height) / 2;
		float xpos = tuioBlob->getScreenX(width);
		float ypos = tuioBlob->getScreenY(height);
		float angle = tuioBlob->getAngleDegrees();


		glColor3f(0.75, 0.75, 0.75);
		std::list<TuioBlob*>::iterator joint = std::find(jointBlobList.begin(), jointBlobList.end(), tuioBlob);
		if (joint != jointBlobList.end()) {
			glColor3f(0.5, 0.5, 0.5);
		}


		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);


		glBegin(GL_TRIANGLE_FAN);
		for (double a = 0.0f; a <= 2 * M_PI; a += 0.2f) {
			glVertex2d(cos(a) * blob_width, sin(a) * blob_height);
		} glEnd();

		glPopMatrix();

		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f(xpos, ypos + 5);
		sprintf(id, "2Dblb %d", tuioBlob->getBlobID());
		drawString(id);
	}




	// draw the 25Dcursors
	std::list<TuioCursor25D*> cursor25DList = tuioServer->getTuioCursors25D();
	for (std::list<TuioCursor25D*>::iterator iter = cursor25DList.begin(); iter != cursor25DList.end(); iter++) {
		TuioCursor25D *tcur = (*iter);
		std::list<TuioPoint> path = tcur->getPath();
		if (path.size() > 0) {

			TuioPoint last_point = path.front();
			glBegin(GL_LINES);
			//glColor3f(0.0, 0.0, 1.0);

			for (std::list<TuioPoint>::iterator point = path.begin(); point != path.end(); point++) {

				glColor3f(1.0 - point->getZ(), point->getZ(), 0.0); // change color according to Z coordinates from red (0) to green (1)
				glVertex3f(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
				glVertex3f(point->getScreenX(width), point->getScreenY(height), 0.0f);
				last_point.update(point->getX(), point->getY());
			}
			glEnd();

			// draw the finger tip
			glColor3f(1.0 - last_point.getZ(), last_point.getZ(), 0.75);
			std::list<TuioCursor25D*>::iterator joint = std::find(jointCursor25DList.begin(), jointCursor25DList.end(), tcur);
			if (joint != jointCursor25DList.end()) {
				glColor3f(1.0 - last_point.getZ(), last_point.getZ(), 0.5);
			}
			glPushMatrix();
			glTranslatef(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
			glBegin(GL_TRIANGLE_FAN);
			for (double a = 0.0f; a <= 2 * M_PI; a += 0.2f) {
				glVertex2d(cos(a) * height / 100.0f, sin(a) * height / 100.0f);
			}
			glEnd();
			glPopMatrix();

			glColor3f(0.0, 0.0, 0.0);
			glRasterPos2f(tcur->getScreenX(width), tcur->getScreenY(height));
			sprintf(id, "25Dcur %d", tcur->getCursorID());
			drawString(id);
		}
	}


	// draw the 25Dobjects
	std::list<TuioObject25D*> object25DList = tuioServer->getTuioObjects25D();
	for (std::list<TuioObject25D*>::iterator iter = object25DList.begin(); iter != object25DList.end(); iter++) {
		TuioObject25D *tuioObject = (*iter);
		int pos_size = height / 20.0f;
		int neg_size = -1 * pos_size;
		float xpos = tuioObject->getScreenX(width);
		float ypos = tuioObject->getScreenY(height);
		float angle = tuioObject->getAngleDegrees();

		glColor3f(1.0 - tuioObject->getZ(), tuioObject->getZ(), 0.75);
		std::list<TuioObject25D*>::iterator joint = std::find(jointObject25DList.begin(), jointObject25DList.end(), tuioObject);
		if (joint != jointObject25DList.end()) {
			glColor3f(1.0 - tuioObject->getZ(), tuioObject->getZ(), 0.5);
		}


		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
		glVertex2f(neg_size, neg_size);
		glVertex2f(neg_size, pos_size);
		glVertex2f(pos_size, pos_size);
		glVertex2f(pos_size, neg_size);
		glEnd();
		glPopMatrix();

		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f(xpos, ypos + 5);
		sprintf(id, "25Dobj %d", tuioObject->getSymbolID());
		drawString(id);
	}

	// draw the 25Dblobs
	std::list<TuioBlob25D*> blob25DList = tuioServer->getTuioBlobs25D();
	for (std::list<TuioBlob25D*>::iterator iter = blob25DList.begin(); iter != blob25DList.end(); iter++) {
		TuioBlob25D *tuioBlob = (*iter);
		float blob_width = tuioBlob->getSpaceWidth(width) / 2;
		float blob_height = tuioBlob->getSpaceHeight(height) / 2;
		float xpos = tuioBlob->getScreenX(width);
		float ypos = tuioBlob->getScreenY(height);
		float angle = tuioBlob->getAngleDegrees();


		glColor3f(1.0 - tuioBlob->getZ(), tuioBlob->getZ(), 0.75);
		std::list<TuioBlob25D*>::iterator joint = std::find(jointBlob25DList.begin(), jointBlob25DList.end(), tuioBlob);
		if (joint != jointBlob25DList.end()) {
			glColor3f(1.0 - tuioBlob->getZ(), tuioBlob->getZ(), 0.5);
		}


		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);


		glBegin(GL_TRIANGLE_FAN);
		for (double a = 0.0f; a <= 2 * M_PI; a += 0.2f) {
			glVertex2d(cos(a) * blob_width, sin(a) * blob_height);
		} glEnd();

		glPopMatrix();

		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f(xpos, ypos + 5);
		sprintf(id, "25Dblb %d", tuioBlob->getBlobID());
		drawString(id);
	}





	// draw the 3Dcursors
	std::list<TuioCursor3D*> cursor3DList = tuioServer->getTuioCursors3D();
	for (std::list<TuioCursor3D*>::iterator iter = cursor3DList.begin(); iter != cursor3DList.end(); iter++) {
		TuioCursor3D *tcur = (*iter);
		std::list<TuioPoint> path = tcur->getPath();
		if (path.size() > 0) {

			TuioPoint last_point = path.front();
			glBegin(GL_LINES);
			//glColor3f(0.0, 0.0, 1.0);

			for (std::list<TuioPoint>::iterator point = path.begin(); point != path.end(); point++) {

				glColor3f(1.0 - point->getZ(), point->getZ(), 0.0); // change color according to Z coordinates from red (0) to green (1)
				glVertex3f(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
				glVertex3f(point->getScreenX(width), point->getScreenY(height), 0.0f);
				last_point.update(point->getX(), point->getY());
			}
			glEnd();

			// draw the finger tip
			glColor3f(1.0 - last_point.getZ(), last_point.getZ(), 0.75);
			std::list<TuioCursor3D*>::iterator joint = std::find(jointCursor3DList.begin(), jointCursor3DList.end(), tcur);
			if (joint != jointCursor3DList.end()) {
				glColor3f(1.0 - last_point.getZ(), last_point.getZ(), 0.5);
			}
			glPushMatrix();
			glTranslatef(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
			glBegin(GL_TRIANGLE_FAN);
			for (double a = 0.0f; a <= 2 * M_PI; a += 0.2f) {
				glVertex2d(cos(a) * height / 100.0f, sin(a) * height / 100.0f);
			}
			glEnd();
			glPopMatrix();

			glColor3f(0.0, 0.0, 0.0);
			glRasterPos2f(tcur->getScreenX(width), tcur->getScreenY(height));
			sprintf(id, "3Dcur %d", tcur->getCursorID());
			drawString(id);
		}
	}


	// draw the 3Dobjects
	std::list<TuioObject3D*> object3DList = tuioServer->getTuioObjects3D();
	for (std::list<TuioObject3D*>::iterator iter = object3DList.begin(); iter != object3DList.end(); iter++) {
		TuioObject3D *tuioObject = (*iter);
		int pos_size = height / 20.0f;
		int neg_size = -1 * pos_size;
		float xpos = tuioObject->getScreenX(width);
		float ypos = tuioObject->getScreenY(height);
		float roll = tuioObject->getRollDegrees();
		float pitch = tuioObject->getPitchDegrees();
		float yaw = tuioObject->getYawDegrees();

		glColor3f(1.0 - tuioObject->getZ(), tuioObject->getZ(), 0.75);
		std::list<TuioObject3D*>::iterator joint = std::find(jointObject3DList.begin(), jointObject3DList.end(), tuioObject);
		if (joint != jointObject3DList.end()) {
			glColor3f(1.0 - tuioObject->getZ(), tuioObject->getZ(), 0.5);
		}


		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(roll, pitch, yaw, 1.0);
		glBegin(GL_QUADS);
		glVertex2f(neg_size, neg_size);
		glVertex2f(neg_size, pos_size);
		glVertex2f(pos_size, pos_size);
		glVertex2f(pos_size, neg_size);
		glEnd();
		glPopMatrix();

		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f(xpos, ypos + 5);
		sprintf(id, "3Dobj %d", tuioObject->getSymbolID());
		drawString(id);
	}

	// draw the 3Dblobs
	std::list<TuioBlob3D*> blob3DList = tuioServer->getTuioBlobs3D();
	for (std::list<TuioBlob3D*>::iterator iter = blob3DList.begin(); iter != blob3DList.end(); iter++) {
		TuioBlob3D *tuioBlob = (*iter);
		float blob_width = tuioBlob->getSpaceWidth(width) / 2;
		float blob_height = tuioBlob->getSpaceHeight(height) / 2;
		float xpos = tuioBlob->getScreenX(width);
		float ypos = tuioBlob->getScreenY(height);
		float roll = tuioBlob->getRollDegrees();
		float pitch = tuioBlob->getPitchDegrees();
		float yaw = tuioBlob->getYawDegrees();


		glColor3f(1.0 - tuioBlob->getZ(), tuioBlob->getZ(), 0.75);
		std::list<TuioBlob3D*>::iterator joint = std::find(jointBlob3DList.begin(), jointBlob3DList.end(), tuioBlob);
		if (joint != jointBlob3DList.end()) {
			glColor3f(1.0 - tuioBlob->getZ(), tuioBlob->getZ(), 0.5);
		}


		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(roll, pitch, yaw, 1.0);


		glBegin(GL_TRIANGLE_FAN);
		for (double a = 0.0f; a <= 2 * M_PI; a += 0.2f) {
			glVertex2d(cos(a) * blob_width, sin(a) * blob_height);
		} glEnd();

		glPopMatrix();

		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f(xpos, ypos + 5);
		sprintf(id, "3Dblb %d", tuioBlob->getBlobID());
		drawString(id);
	}






	glColor3f(0.0, 0.0, 1.0);
	glRasterPos2f(width-150, 20);	
	switch (mode)
	{
	case 0: sprintf(id, "Mode 2Dcur"); break;
	case 1: sprintf(id, "Mode 2Dobj"); break;
	case 2: sprintf(id, "Mode 2Dblb"); break;
	case 3: sprintf(id, "Mode 25Dcur"); break;
	case 4: sprintf(id, "Mode 25Dobj"); break;
	case 5: sprintf(id, "Mode 25Dblb"); break;
	case 6: sprintf(id, "Mode 3Dcur"); break;
	case 7: sprintf(id, "Mode 3Dobj"); break;
	case 8: sprintf(id, "Mode 3Dblb"); break;
	}
	drawString(id);

	glRasterPos2f(width - 150, 45);
	switch (control)
	{
	case 0: sprintf(id, "Control Z"); break;
	case 1: sprintf(id, "Control roll/angle"); break;
	case 2: sprintf(id, "Control pitch"); break;
	case 3: sprintf(id, "Control yaw"); break;
	case 4: sprintf(id, "Control width"); break;
	case 5: sprintf(id, "Control height"); break;
	case 6: sprintf(id, "Control depth"); break;
	}
	drawString(id);

	if (help) {
		glColor3f(0.0, 0.0, 1.0);
		glRasterPos2f(10, 20);
		drawString("h - toggle help display");
		glRasterPos2f(10, 35);
		if (verbose) drawString("v - disable verbose mode");
		else drawString("v - enable verbose mode");
		glRasterPos2f(10, 50);
		drawString("r - reset session");
		glRasterPos2f(10, 65);
		if (tuioServer->fullUpdateEnabled()) drawString("f - disable full update");
		else drawString("f - enable full update");
		glRasterPos2f(10, 80);
		if (tuioServer->periodicMessagesEnabled()) drawString("p - disable periodic messages");
		else drawString("p - enable periodic messages");
		glRasterPos2f(10, 95);
		drawString("SHIFT click - create persistent cursor");
		glRasterPos2f(10, 110);
		drawString("CTRL click - add to cursor group");
		glRasterPos2f(10, 125);
		if (fullscreen) drawString("F1 - exit fullscreen mode");
		else drawString("F1 - enter fullscreen mode");
		glRasterPos2f(10, 140);
		drawString("ESC - Quit");
		glRasterPos2f(10, 155);
		drawString("m - mode : 0=2Dcur/1=2Dobj/2=2Dblb/3=25Dcur/4=25Dobj/5=25Dblb/6=3Dcur/7=3Dobj/8=3Dblb");

		glRasterPos2f(10, 170);
		drawString("SCROLL - change the parameter defined by control");
		glRasterPos2f(10, 185);
		drawString("control (NumPad) : 0=Z/1=Roll/2=Pitch/3=Yaw/4=Width/5=Height/6=Depth");
	}

	SDL_GL_SwapWindow(window);
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

	while (SDL_PollEvent(&event)) {

		switch (event.type) {
		case SDL_KEYDOWN:
			if (event.key.repeat) continue;
			else if (event.key.keysym.sym == SDLK_ESCAPE) {
				running = false;
				SDL_Quit();
			}
			else if (event.key.keysym.sym == SDLK_F1) {
				if (fullscreen)
				{
					width = window_width;
					height = window_height;
					SDL_SetWindowFullscreen(window, SDL_FALSE);
					fullscreen = false;
				}
				else
				{
					width = screen_width;
					height = screen_height;
					SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					fullscreen = true;
				}

				glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
				glViewport(0, 0, (GLint)width, (GLint)height);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluOrtho2D(0, (GLfloat)width, (GLfloat)height, 0);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			}
			else if (event.key.keysym.sym == SDLK_f) {
				fullupdate = !tuioServer->fullUpdateEnabled();
				if (fullupdate) tuioServer->enableFullUpdate();
				else tuioServer->disableFullUpdate();
			}
			else if (event.key.keysym.sym == SDLK_p) {
				periodic = !tuioServer->periodicMessagesEnabled();
				if (periodic) tuioServer->enablePeriodicMessages();
				else tuioServer->disablePeriodicMessages();
			}
			else if (event.key.keysym.sym == SDLK_v) {
				verbose = !verbose;
				tuioServer->setVerbose(verbose);
			}
			else if (event.key.keysym.sym == SDLK_h) {
				help = !help;
			}
			else if (event.key.keysym.sym == SDLK_r) { 
				tuioServer->resetTuioCursors();
				stickyCursorList.clear();
				jointCursorList.clear();
				activeCursorList.clear();
				tuioServer->resetTuioObjects();
				stickyObjectList.clear();
				jointObjectList.clear();
				activeObjectList.clear();
				tuioServer->resetTuioBlobs();
				stickyBlobList.clear();
				jointBlobList.clear();
				activeBlobList.clear();

				tuioServer->resetTuioCursors25D();
				stickyCursor25DList.clear();
				jointCursor25DList.clear();
				activeCursor25DList.clear();
				tuioServer->resetTuioObjects25D();
				stickyObject25DList.clear();
				jointObject25DList.clear();
				activeObject25DList.clear();
				tuioServer->resetTuioBlobs25D();
				stickyBlob25DList.clear();
				jointBlob25DList.clear();
				activeBlob25DList.clear();

				tuioServer->resetTuioCursors3D();
				stickyCursor3DList.clear();
				jointCursor3DList.clear();
				activeCursor3DList.clear();
				tuioServer->resetTuioObjects3D();
				stickyObject3DList.clear();
				jointObject3DList.clear();
				activeObject3DList.clear();
				tuioServer->resetTuioBlobs3D();
				stickyBlob3DList.clear();
				jointBlob3DList.clear();
				activeBlob3DList.clear();
			}
			else if (event.key.keysym.sym == SDLK_0) {
				mode = 0;
			}
			else if (event.key.keysym.sym == SDLK_1) {
				mode = 1;
			}
			else if (event.key.keysym.sym == SDLK_2) {
				mode = 2;
			}
			else if (event.key.keysym.sym == SDLK_3) {
				mode = 3;
			}
			else if (event.key.keysym.sym == SDLK_4) {
				mode = 4;
			}
			else if (event.key.keysym.sym == SDLK_5) {
				mode = 5;
			}
			else if (event.key.keysym.sym == SDLK_6) {
				mode = 6;
			}
			else if (event.key.keysym.sym == SDLK_7) {
				mode = 7;
			}
			else if (event.key.keysym.sym == SDLK_8) {
				mode = 8;
			}
			else if (event.key.keysym.sym == SDLK_KP_0) {
				control = 0;
			}
			else if (event.key.keysym.sym == SDLK_KP_1) {
				control = 1;
			}
			else if (event.key.keysym.sym == SDLK_KP_2) {
				control = 2;
			}
			else if (event.key.keysym.sym == SDLK_KP_3) {
				control = 3;
			}
			else if (event.key.keysym.sym == SDLK_KP_4) {
				control = 4;
			}
			else if (event.key.keysym.sym == SDLK_KP_5) {
				control = 5;
			}
			else if (event.key.keysym.sym == SDLK_KP_6) {
				control = 6;
			}

			break;

		case SDL_MOUSEMOTION:
			if (event.button.button == SDL_BUTTON_LEFT) mouseDragged((float)event.button.x / width, (float)event.button.y / height);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) mousePressed((float)event.button.x / width, (float)event.button.y / height);
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) mouseReleased((float)event.button.x / width, (float)event.button.y / height);
			break;

		case SDL_MOUSEWHEEL:
			applyExtraDimension(event.wheel.y);
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

	switch (mode)
	{
		case(0):
		{
			TuioCursor *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioCursor*>::iterator iter = stickyCursorList.begin(); iter != stickyCursorList.end(); iter++) {
				TuioCursor *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), match);
					if (joint != jointCursorList.end()) {
						jointCursorList.erase(joint);
					}
					stickyCursorList.remove(match);
					activeCursorList.remove(match);
					tuioServer->removeTuioCursor(match);
				}
				else {
					TuioCursor *cursor = tuioServer->addTuioCursor(x, y);
					stickyCursorList.push_back(cursor);
					activeCursorList.push_back(cursor);
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), match);
					if (joint != jointCursorList.end()) {
						jointCursorList.remove(match);
					}
					else jointCursorList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioCursor *cursor = tuioServer->addTuioCursor(x, y);
					activeCursorList.push_back(cursor);
				}
				else activeCursorList.push_back(match);
			}
			break;
			}
		case(1):
		{
			TuioObject *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioObject*>::iterator iter = stickyObjectList.begin(); iter != stickyObjectList.end(); iter++) {
				TuioObject *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioObject*>::iterator joint = std::find(jointObjectList.begin(), jointObjectList.end(), match);
					if (joint != jointObjectList.end()) {
						jointObjectList.erase(joint);
					}
					stickyObjectList.remove(match);
					activeObjectList.remove(match);
					tuioServer->removeTuioObject(match);
				}
				else {
					TuioObject *cursor = tuioServer->addTuioObject(objectID, x, y, 0);
					stickyObjectList.push_back(cursor);
					activeObjectList.push_back(cursor); 
					objectID++;
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioObject*>::iterator joint = std::find(jointObjectList.begin(), jointObjectList.end(), match);
					if (joint != jointObjectList.end()) {
						jointObjectList.remove(match);
					}
					else jointObjectList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioObject *cursor = tuioServer->addTuioObject(objectID, x, y, 0);
					activeObjectList.push_back(cursor);
					objectID++;
				}
				else activeObjectList.push_back(match);
			}
			break;
			}
		case(2):
		{
			TuioBlob *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioBlob*>::iterator iter = stickyBlobList.begin(); iter != stickyBlobList.end(); iter++) {
				TuioBlob *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioBlob*>::iterator joint = std::find(jointBlobList.begin(), jointBlobList.end(), match);
					if (joint != jointBlobList.end()) {
						jointBlobList.erase(joint);
					}
					stickyBlobList.remove(match);
					activeBlobList.remove(match);
					tuioServer->removeTuioBlob(match);
				}
				else {
					TuioBlob *cursor = tuioServer->addTuioBlob( x, y,0, 0.1,0.1,0.1);
					stickyBlobList.push_back(cursor);
					activeBlobList.push_back(cursor);
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioBlob*>::iterator joint = std::find(jointBlobList.begin(), jointBlobList.end(), match);
					if (joint != jointBlobList.end()) {
						jointBlobList.remove(match);
					}
					else jointBlobList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioBlob *cursor = tuioServer->addTuioBlob(x, y,0, 0.1, 0.1, 0.1);
					activeBlobList.push_back(cursor);
				}
				else activeBlobList.push_back(match);
			}
			break;
		}
		case(3):
		{
			TuioCursor25D *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioCursor25D*>::iterator iter = stickyCursor25DList.begin(); iter != stickyCursor25DList.end(); iter++) {
				TuioCursor25D *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioCursor25D*>::iterator joint = std::find(jointCursor25DList.begin(), jointCursor25DList.end(), match);
					if (joint != jointCursor25DList.end()) {
						jointCursor25DList.erase(joint);
					}
					stickyCursor25DList.remove(match);
					activeCursor25DList.remove(match);
					tuioServer->removeTuioCursor25D(match);
				}
				else {
					TuioCursor25D *cursor = tuioServer->addTuioCursor25D(x, y,0);
					stickyCursor25DList.push_back(cursor);
					activeCursor25DList.push_back(cursor);
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioCursor25D*>::iterator joint = std::find(jointCursor25DList.begin(), jointCursor25DList.end(), match);
					if (joint != jointCursor25DList.end()) {
						jointCursor25DList.remove(match);
					}
					else jointCursor25DList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioCursor25D *cursor = tuioServer->addTuioCursor25D(x, y,0);
					activeCursor25DList.push_back(cursor);
				}
				else activeCursor25DList.push_back(match);
			}
			break;
		}
		case(4):
		{
			TuioObject25D *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioObject25D*>::iterator iter = stickyObject25DList.begin(); iter != stickyObject25DList.end(); iter++) {
				TuioObject25D *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioObject25D*>::iterator joint = std::find(jointObject25DList.begin(), jointObject25DList.end(), match);
					if (joint != jointObject25DList.end()) {
						jointObject25DList.erase(joint);
					}
					stickyObject25DList.remove(match);
					activeObject25DList.remove(match);
					tuioServer->removeTuioObject25D(match);
				}
				else {
					TuioObject25D *cursor = tuioServer->addTuioObject25D(objectID, x, y,0, 0);
					stickyObject25DList.push_back(cursor);
					activeObject25DList.push_back(cursor);
					object25DID++;
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioObject25D*>::iterator joint = std::find(jointObject25DList.begin(), jointObject25DList.end(), match);
					if (joint != jointObject25DList.end()) {
						jointObject25DList.remove(match);
					}
					else jointObject25DList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioObject25D *cursor = tuioServer->addTuioObject25D(objectID, x, y, 0, 0);
					activeObject25DList.push_back(cursor);
					object25DID++;
				}
				else activeObject25DList.push_back(match);
			}
			break;
		}
		case(5):
		{
			TuioBlob25D *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioBlob25D*>::iterator iter = stickyBlob25DList.begin(); iter != stickyBlob25DList.end(); iter++) {
				TuioBlob25D *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioBlob25D*>::iterator joint = std::find(jointBlob25DList.begin(), jointBlob25DList.end(), match);
					if (joint != jointBlob25DList.end()) {
						jointBlob25DList.erase(joint);
					}
					stickyBlob25DList.remove(match);
					activeBlob25DList.remove(match);
					tuioServer->removeTuioBlob25D(match);
				}
				else {
					TuioBlob25D *cursor = tuioServer->addTuioBlob25D(x, y,0, 0, 0.1, 0.1, 0.1);
					stickyBlob25DList.push_back(cursor);
					activeBlob25DList.push_back(cursor);
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioBlob25D*>::iterator joint = std::find(jointBlob25DList.begin(), jointBlob25DList.end(), match);
					if (joint != jointBlob25DList.end()) {
						jointBlob25DList.remove(match);
					}
					else jointBlob25DList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioBlob25D *cursor = tuioServer->addTuioBlob25D(x, y,0, 0, 0.1, 0.1, 0.1);
					activeBlob25DList.push_back(cursor);
				}
				else activeBlob25DList.push_back(match);
			}
			break;
		}
		case(6):
		{
			TuioCursor3D *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioCursor3D*>::iterator iter = stickyCursor3DList.begin(); iter != stickyCursor3DList.end(); iter++) {
				TuioCursor3D *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioCursor3D*>::iterator joint = std::find(jointCursor3DList.begin(), jointCursor3DList.end(), match);
					if (joint != jointCursor3DList.end()) {
						jointCursor3DList.erase(joint);
					}
					stickyCursor3DList.remove(match);
					activeCursor3DList.remove(match);
					tuioServer->removeTuioCursor3D(match);
				}
				else {
					TuioCursor3D *cursor = tuioServer->addTuioCursor3D(x, y, 0);
					stickyCursor3DList.push_back(cursor);
					activeCursor3DList.push_back(cursor);
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioCursor3D*>::iterator joint = std::find(jointCursor3DList.begin(), jointCursor3DList.end(), match);
					if (joint != jointCursor3DList.end()) {
						jointCursor3DList.remove(match);
					}
					else jointCursor3DList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioCursor3D *cursor = tuioServer->addTuioCursor3D(x, y, 0);
					activeCursor3DList.push_back(cursor);
				}
				else activeCursor3DList.push_back(match);
			}
			break;
		}
		case(7):
		{
			TuioObject3D *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioObject3D*>::iterator iter = stickyObject3DList.begin(); iter != stickyObject3DList.end(); iter++) {
				TuioObject3D *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioObject3D*>::iterator joint = std::find(jointObject3DList.begin(), jointObject3DList.end(), match);
					if (joint != jointObject3DList.end()) {
						jointObject3DList.erase(joint);
					}
					stickyObject3DList.remove(match);
					activeObject3DList.remove(match);
					tuioServer->removeTuioObject3D(match);
				}
				else {
					TuioObject3D *cursor = tuioServer->addTuioObject3D(objectID, x, y, 0, 0,0,0);
					stickyObject3DList.push_back(cursor);
					activeObject3DList.push_back(cursor);
					object3DID++;
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioObject3D*>::iterator joint = std::find(jointObject3DList.begin(), jointObject3DList.end(), match);
					if (joint != jointObject3DList.end()) {
						jointObject3DList.remove(match);
					}
					else jointObject3DList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioObject3D *cursor = tuioServer->addTuioObject3D(objectID, x, y, 0, 0, 0, 0);
					activeObject3DList.push_back(cursor);
					object3DID++;
				}
				else activeObject3DList.push_back(match);
			}
			break;
		}
		case(8):
		{
			TuioBlob3D *match = NULL;
			float distance = 0.01f;
			for (std::list<TuioBlob3D*>::iterator iter = stickyBlob3DList.begin(); iter != stickyBlob3DList.end(); iter++) {
				TuioBlob3D *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if ((test < distance) && (test < 8.0f / width)) {
					distance = test;
					match = tcur;
				}
			}

			const Uint8 *keystate = SDL_GetKeyboardState(NULL);

			if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

				if (match != NULL) {
					std::list<TuioBlob3D*>::iterator joint = std::find(jointBlob3DList.begin(), jointBlob3DList.end(), match);
					if (joint != jointBlob3DList.end()) {
						jointBlob3DList.erase(joint);
					}
					stickyBlob3DList.remove(match);
					activeBlob3DList.remove(match);
					tuioServer->removeTuioBlob3D(match);
				}
				else {
					TuioBlob3D *cursor = tuioServer->addTuioBlob3D(x, y, 0, 0,0,0, 0.1, 0.1,0.1, 0.1);
					stickyBlob3DList.push_back(cursor);
					activeBlob3DList.push_back(cursor);
				}
			}
			else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

				if (match != NULL) {
					std::list<TuioBlob3D*>::iterator joint = std::find(jointBlob3DList.begin(), jointBlob3DList.end(), match);
					if (joint != jointBlob3DList.end()) {
						jointBlob3DList.remove(match);
					}
					else jointBlob3DList.push_back(match);
				}
			}
			else {
				if (match == NULL) {
					TuioBlob3D *cursor = tuioServer->addTuioBlob3D(x, y, 0, 0, 0, 0, 0.1, 0.1, 0.1, 0.1);
					activeBlob3DList.push_back(cursor);
				}
				else activeBlob3DList.push_back(match);
			}
			break;
		}
	}

}

void SimpleSimulator::mouseDragged(float x, float y) {
	//printf("dragged %f %f\n",x,y);
	switch (mode)
	{
	case 0 :
		{
			TuioCursor *cursor = NULL;
			float distance = width;
			for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter != activeCursorList.end(); iter++) {
				TuioCursor *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if (test < distance) {
					distance = test;
					cursor = tcur;
				}
			}

			if (cursor == NULL) return;
			if (cursor->getTuioTime() == frameTime) return;

			std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), cursor);
			if (joint != jointCursorList.end()) {
				float dx = x - cursor->getX();
				float dy = y - cursor->getY();
				for (std::list<TuioCursor*>::iterator iter = jointCursorList.begin(); iter != jointCursorList.end(); iter++) {
					TuioCursor *jointCursor = (*iter);
					tuioServer->updateTuioCursor(jointCursor, jointCursor->getX() + dx, jointCursor->getY() + dy);
				}
			}
			else tuioServer->updateTuioCursor(cursor, x, y);
		}
		break;
	case 1:
	{
		TuioObject *cursor = NULL;
		float distance = width;
		for (std::list<TuioObject*>::iterator iter = activeObjectList.begin(); iter != activeObjectList.end(); iter++) {
			TuioObject *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioObject*>::iterator joint = std::find(jointObjectList.begin(), jointObjectList.end(), cursor);
		if (joint != jointObjectList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioObject*>::iterator iter = jointObjectList.begin(); iter != jointObjectList.end(); iter++) {
				TuioObject *jointObject = (*iter);
				tuioServer->updateTuioObject(jointObject, jointObject->getX() + dx, jointObject->getY() + dy, jointObject->getAngle());
			}
		}
		else tuioServer->updateTuioObject(cursor, x, y, cursor->getAngle());
	}
	break;
	case 2:
	{
		TuioBlob *cursor = NULL;
		float distance = width;
		for (std::list<TuioBlob*>::iterator iter = activeBlobList.begin(); iter != activeBlobList.end(); iter++) {
			TuioBlob *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioBlob*>::iterator joint = std::find(jointBlobList.begin(), jointBlobList.end(), cursor);
		if (joint != jointBlobList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioBlob*>::iterator iter = jointBlobList.begin(); iter != jointBlobList.end(); iter++) {
				TuioBlob *jointBlob = (*iter);
				tuioServer->updateTuioBlob(jointBlob, jointBlob->getX() + dx, jointBlob->getY() + dy, jointBlob->getAngle(), jointBlob->getWidth(), jointBlob->getHeight(), jointBlob->getArea());
			}
		}
		else tuioServer->updateTuioBlob(cursor, x, y, cursor->getAngle(), cursor->getWidth(), cursor->getHeight(), cursor->getArea());
	}
	break;
	case 3:
	{
		TuioCursor25D *cursor = NULL;
		float distance = width;
		for (std::list<TuioCursor25D*>::iterator iter = activeCursor25DList.begin(); iter != activeCursor25DList.end(); iter++) {
			TuioCursor25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioCursor25D*>::iterator joint = std::find(jointCursor25DList.begin(), jointCursor25DList.end(), cursor);
		if (joint != jointCursor25DList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioCursor25D*>::iterator iter = jointCursor25DList.begin(); iter != jointCursor25DList.end(); iter++) {
				TuioCursor25D *jointCursor = (*iter);
				tuioServer->updateTuioCursor25D(jointCursor, jointCursor->getX() + dx, jointCursor->getY() + dy, cursor->getZ() );
			}
		}
		else tuioServer->updateTuioCursor25D(cursor, x, y, cursor->getZ());
	}
	break;
	case 4:
	{
		TuioObject25D *cursor = NULL;
		float distance = width;
		for (std::list<TuioObject25D*>::iterator iter = activeObject25DList.begin(); iter != activeObject25DList.end(); iter++) {
			TuioObject25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioObject25D*>::iterator joint = std::find(jointObject25DList.begin(), jointObject25DList.end(), cursor);
		if (joint != jointObject25DList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioObject25D*>::iterator iter = jointObject25DList.begin(); iter != jointObject25DList.end(); iter++) {
				TuioObject25D *jointObject = (*iter);
				tuioServer->updateTuioObject25D(jointObject, jointObject->getX() + dx, jointObject->getY() + dy, jointObject->getZ(), jointObject->getAngle());
			}
		}
		else tuioServer->updateTuioObject25D(cursor, x, y, cursor->getZ(), cursor->getAngle());
	}
	break;
	case 5:
	{
		TuioBlob25D *cursor = NULL;
		float distance = width;
		for (std::list<TuioBlob25D*>::iterator iter = activeBlob25DList.begin(); iter != activeBlob25DList.end(); iter++) {
			TuioBlob25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioBlob25D*>::iterator joint = std::find(jointBlob25DList.begin(), jointBlob25DList.end(), cursor);
		if (joint != jointBlob25DList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioBlob25D*>::iterator iter = jointBlob25DList.begin(); iter != jointBlob25DList.end(); iter++) {
				TuioBlob25D *jointBlob = (*iter);
				tuioServer->updateTuioBlob25D(jointBlob, jointBlob->getX() + dx, jointBlob->getY() + dy, jointBlob->getZ(), jointBlob->getAngle(), jointBlob->getWidth(), jointBlob->getHeight(), jointBlob->getArea());
			}
		}
		else tuioServer->updateTuioBlob25D(cursor, x, y, cursor->getZ(), cursor->getAngle(), cursor->getWidth(), cursor->getHeight(), cursor->getArea());
	}
	break;
	case 6:
	{
		TuioCursor3D *cursor = NULL;
		float distance = width;
		for (std::list<TuioCursor3D*>::iterator iter = activeCursor3DList.begin(); iter != activeCursor3DList.end(); iter++) {
			TuioCursor3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioCursor3D*>::iterator joint = std::find(jointCursor3DList.begin(), jointCursor3DList.end(), cursor);
		if (joint != jointCursor3DList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioCursor3D*>::iterator iter = jointCursor3DList.begin(); iter != jointCursor3DList.end(); iter++) {
				TuioCursor3D *jointCursor = (*iter);
				tuioServer->updateTuioCursor3D(jointCursor, jointCursor->getX() + dx, jointCursor->getY() + dy, cursor->getZ());
			}
		}
		else tuioServer->updateTuioCursor3D(cursor, x, y, cursor->getZ());
	}
	break;
	case 7:
	{
		TuioObject3D *cursor = NULL;
		float distance = width;
		for (std::list<TuioObject3D*>::iterator iter = activeObject3DList.begin(); iter != activeObject3DList.end(); iter++) {
			TuioObject3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioObject3D*>::iterator joint = std::find(jointObject3DList.begin(), jointObject3DList.end(), cursor);
		if (joint != jointObject3DList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioObject3D*>::iterator iter = jointObject3DList.begin(); iter != jointObject3DList.end(); iter++) {
				TuioObject3D *jointObject = (*iter);
				tuioServer->updateTuioObject3D(jointObject, jointObject->getX() + dx, jointObject->getY() + dy, jointObject->getZ(), jointObject->getRoll(), jointObject->getPitch(), jointObject->getYaw());
			}
		}
		else tuioServer->updateTuioObject3D(cursor, x, y, cursor->getZ(), cursor->getRoll(), cursor->getPitch(), cursor->getYaw());
	}
	break;
	case 8:
	{
		TuioBlob3D *cursor = NULL;
		float distance = width;
		for (std::list<TuioBlob3D*>::iterator iter = activeBlob3DList.begin(); iter != activeBlob3DList.end(); iter++) {
			TuioBlob3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor == NULL) return;
		if (cursor->getTuioTime() == frameTime) return;

		std::list<TuioBlob3D*>::iterator joint = std::find(jointBlob3DList.begin(), jointBlob3DList.end(), cursor);
		if (joint != jointBlob3DList.end()) {
			float dx = x - cursor->getX();
			float dy = y - cursor->getY();
			for (std::list<TuioBlob3D*>::iterator iter = jointBlob3DList.begin(); iter != jointBlob3DList.end(); iter++) {
				TuioBlob3D *jointBlob = (*iter);
				tuioServer->updateTuioBlob3D(jointBlob, jointBlob->getX() + dx, jointBlob->getY() + dy, jointBlob->getZ(), jointBlob->getRoll(), jointBlob->getPitch(), jointBlob->getYaw(), jointBlob->getWidth(), jointBlob->getHeight(), jointBlob->getDepth(), jointBlob->getVolume() );
			}
		}
		else tuioServer->updateTuioBlob3D(cursor, x, y, cursor->getZ(), cursor->getRoll(), cursor->getPitch(), cursor->getYaw(), cursor->getWidth(), cursor->getHeight(), cursor->getDepth(), cursor->getVolume());
	}
	break;
	}

}

void SimpleSimulator::mouseReleased(float x, float y) {
	//printf("released %f %f\n",x,y);

	switch (mode)
	{
	case 0 :
		{
			TuioCursor *cursor = NULL;
			float distance = 0.01f;
			for (std::list<TuioCursor*>::iterator iter = stickyCursorList.begin(); iter != stickyCursorList.end(); iter++) {
				TuioCursor *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if (test < distance) {
					distance = test;
					cursor = tcur;
				}
			}

			if (cursor != NULL) {
				activeCursorList.remove(cursor);
				return;
			}

			distance = 0.01f;
			for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter != activeCursorList.end(); iter++) {
				TuioCursor *tcur = (*iter);
				float test = tcur->getDistance(x, y);
				if (test < distance) {
					distance = test;
					cursor = tcur;
				}
			}

			if (cursor != NULL) {
				activeCursorList.remove(cursor);
				tuioServer->removeTuioCursor(cursor);
			}
		}
		break;
	case 1: 
	{
		TuioObject *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioObject*>::iterator iter = stickyObjectList.begin(); iter != stickyObjectList.end(); iter++) {
			TuioObject *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeObjectList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioObject*>::iterator iter = activeObjectList.begin(); iter != activeObjectList.end(); iter++) {
			TuioObject *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeObjectList.remove(cursor);
			tuioServer->removeTuioObject(cursor);
		}
	}
	break;
	case 2:
	{
		TuioBlob *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioBlob*>::iterator iter = stickyBlobList.begin(); iter != stickyBlobList.end(); iter++) {
			TuioBlob *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeBlobList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioBlob*>::iterator iter = activeBlobList.begin(); iter != activeBlobList.end(); iter++) {
			TuioBlob *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeBlobList.remove(cursor);
			tuioServer->removeTuioBlob(cursor);
		}
	}
	break;
	case 3:
	{
		TuioCursor25D *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioCursor25D*>::iterator iter = stickyCursor25DList.begin(); iter != stickyCursor25DList.end(); iter++) {
			TuioCursor25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeCursor25DList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioCursor25D*>::iterator iter = activeCursor25DList.begin(); iter != activeCursor25DList.end(); iter++) {
			TuioCursor25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeCursor25DList.remove(cursor);
			tuioServer->removeTuioCursor25D(cursor);
		}
	}
	break;
	case 4:
	{
		TuioObject25D *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioObject25D*>::iterator iter = stickyObject25DList.begin(); iter != stickyObject25DList.end(); iter++) {
			TuioObject25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeObject25DList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioObject25D*>::iterator iter = activeObject25DList.begin(); iter != activeObject25DList.end(); iter++) {
			TuioObject25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeObject25DList.remove(cursor);
			tuioServer->removeTuioObject25D(cursor);
		}
	}
	break;
	case 5:
	{
		TuioBlob25D *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioBlob25D*>::iterator iter = stickyBlob25DList.begin(); iter != stickyBlob25DList.end(); iter++) {
			TuioBlob25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeBlob25DList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioBlob25D*>::iterator iter = activeBlob25DList.begin(); iter != activeBlob25DList.end(); iter++) {
			TuioBlob25D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeBlob25DList.remove(cursor);
			tuioServer->removeTuioBlob25D(cursor);
		}
	}
	break;
	case 6:
	{
		TuioCursor3D *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioCursor3D*>::iterator iter = stickyCursor3DList.begin(); iter != stickyCursor3DList.end(); iter++) {
			TuioCursor3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeCursor3DList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioCursor3D*>::iterator iter = activeCursor3DList.begin(); iter != activeCursor3DList.end(); iter++) {
			TuioCursor3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeCursor3DList.remove(cursor);
			tuioServer->removeTuioCursor3D(cursor);
		}
	}
	break;
	case 7:
	{
		TuioObject3D *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioObject3D*>::iterator iter = stickyObject3DList.begin(); iter != stickyObject3DList.end(); iter++) {
			TuioObject3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeObject3DList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioObject3D*>::iterator iter = activeObject3DList.begin(); iter != activeObject3DList.end(); iter++) {
			TuioObject3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeObject3DList.remove(cursor);
			tuioServer->removeTuioObject3D(cursor);
		}
	}
	break;
	case 8:
	{
		TuioBlob3D *cursor = NULL;
		float distance = 0.01f;
		for (std::list<TuioBlob3D*>::iterator iter = stickyBlob3DList.begin(); iter != stickyBlob3DList.end(); iter++) {
			TuioBlob3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeBlob3DList.remove(cursor);
			return;
		}

		distance = 0.01f;
		for (std::list<TuioBlob3D*>::iterator iter = activeBlob3DList.begin(); iter != activeBlob3DList.end(); iter++) {
			TuioBlob3D *tcur = (*iter);
			float test = tcur->getDistance(x, y);
			if (test < distance) {
				distance = test;
				cursor = tcur;
			}
		}

		if (cursor != NULL) {
			activeBlob3DList.remove(cursor);
			tuioServer->removeTuioBlob3D(cursor);
		}
	}
	break;
	}



}

void SimpleSimulator::applyExtraDimension(float scroll) {
	//printf("scroll %f %f\n",x,y);
	//if (verbose) printf("Scroll %f\n", scroll); // 1 ou -1

	float diffZ = 0, diffRoll = 0, diffPitch = 0, diffYaw = 0, diffWidth = 0, diffHeight = 0, diffDepth = 0;
	switch (control)
	{
	case 0: diffZ += (scroll / 100.0f);
		break;
	case 1: diffRoll += scroll;
		diffRoll = diffRoll * 3.14159 / 180.0;
		break;
	case 2: diffPitch += scroll;
		diffPitch = diffPitch * 3.14159 / 180.0;
		break;
	case 3: diffYaw += scroll;
		diffYaw = diffYaw * 3.14159 / 180.0;
		break;
	case 4: diffWidth += (scroll / 100.0f);
		break;
	case 5: diffHeight += (scroll / 100.0f);
		break;
	case 6: diffDepth += (scroll / 100.0f);
		break;
	}




	switch (mode)
	{
	case 0:
		{
		printf("Nb sticky cursor : %d\n", stickyCursorList.size());
		printf("Nb joint cursor : %d\n", jointCursorList.size());
		printf("Nb active cursor : %d\n", activeCursorList.size());

		}
	break;
	case 1:
	{

		for (std::list<TuioObject*>::iterator iter = jointObjectList.begin(); iter != jointObjectList.end(); iter++) {
			TuioObject *jointObject = (*iter);
			tuioServer->updateTuioObject(jointObject, jointObject->getX(), jointObject->getY(), jointObject->getAngle()+diffRoll);
		}
	}
	break;
	case 2:
	{

		for (std::list<TuioBlob*>::iterator iter = jointBlobList.begin(); iter != jointBlobList.end(); iter++) {
			TuioBlob *jointBlob = (*iter);
			tuioServer->updateTuioBlob(jointBlob, jointBlob->getX(), jointBlob->getY(), jointBlob->getAngle() + diffRoll, jointBlob->getWidth() + diffWidth, jointBlob->getHeight() + diffHeight, jointBlob->getArea());
		}
	}
	break;
	case 3:
	{
		for (std::list<TuioCursor25D*>::iterator iter = jointCursor25DList.begin(); iter != jointCursor25DList.end(); iter++) {
			TuioCursor25D *jointObject = (*iter);
			tuioServer->updateTuioCursor25D(jointObject, jointObject->getX(), jointObject->getY(), jointObject->getZ() + diffZ);
		}

	}
	break;
	case 4:
	{

		for (std::list<TuioObject25D*>::iterator iter = jointObject25DList.begin(); iter != jointObject25DList.end(); iter++) {
			TuioObject25D *jointObject = (*iter);
			tuioServer->updateTuioObject25D(jointObject, jointObject->getX(), jointObject->getY(), jointObject->getZ() + diffZ, jointObject->getAngle() + diffRoll);
		}
	}
	break;
	case 5:
	{

		for (std::list<TuioBlob25D*>::iterator iter = jointBlob25DList.begin(); iter != jointBlob25DList.end(); iter++) {
			TuioBlob25D *jointBlob = (*iter);
			tuioServer->updateTuioBlob25D(jointBlob, jointBlob->getX(), jointBlob->getY(), jointBlob->getZ() + diffZ, jointBlob->getAngle() + diffRoll, jointBlob->getWidth() + diffWidth, jointBlob->getHeight() + diffHeight, jointBlob->getArea());
		}
	}
	break;
	case 6:
	{
		for (std::list<TuioCursor3D*>::iterator iter = jointCursor3DList.begin(); iter != jointCursor3DList.end(); iter++) {
			TuioCursor3D *jointObject = (*iter);
			tuioServer->updateTuioCursor3D(jointObject, jointObject->getX(), jointObject->getY(), jointObject->getZ() + diffZ);
		}

	}
	break;
	case 7:
	{

		for (std::list<TuioObject3D*>::iterator iter = jointObject3DList.begin(); iter != jointObject3DList.end(); iter++) {
			TuioObject3D *jointObject = (*iter);
			tuioServer->updateTuioObject3D(jointObject, jointObject->getX(), jointObject->getY(), jointObject->getZ() + diffZ, jointObject->getRoll() + diffRoll, jointObject->getPitch() + diffPitch, jointObject->getYaw() + diffYaw);
		}
	}
	break;
	case 8:
	{

		for (std::list<TuioBlob3D*>::iterator iter = jointBlob3DList.begin(); iter != jointBlob3DList.end(); iter++) {
			TuioBlob3D *jointBlob = (*iter);
			tuioServer->updateTuioBlob3D(jointBlob, jointBlob->getX(), jointBlob->getY(), jointBlob->getZ() + diffZ, jointBlob->getRoll() + diffRoll, jointBlob->getPitch() + diffPitch, jointBlob->getYaw() + diffYaw, jointBlob->getWidth() + diffWidth, jointBlob->getHeight() + diffHeight, jointBlob->getDepth() + diffDepth, jointBlob->getVolume());
		}
	}
	break;
	}



}




SimpleSimulator::SimpleSimulator(TuioServer *server)
	: verbose(false)
	, fullupdate(false)
	, periodic(false)
	, fullscreen(false)
	, help(false)
	, screen_width(1024)
	, screen_height(768)
	, window_width(640)
	, window_height(480)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not be initialized: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	TuioTime::initSession();
	frameTime = TuioTime::getSessionTime();

	tuioServer = server;
	tuioServer->setSourceName("SimpleSimulator");

	initWindow();
	SDL_SetWindowTitle(window, "SimpleSimulator");
}

void SimpleSimulator::initWindow() {

	SDL_DisplayMode displaymode;
	SDL_GetCurrentDisplayMode(0, &displaymode);
	screen_width = displaymode.w;
	screen_height = displaymode.h;

	int videoFlags = SDL_WINDOW_OPENGL;
	if (fullscreen) {
		videoFlags |= SDL_WINDOW_FULLSCREEN;
		width = screen_width;
		height = screen_height;
	}
	else {
		width = window_width; 
		height = window_height;
	}

	SDL_CreateWindowAndRenderer(width, height, videoFlags, &window, &renderer);

	if (window == NULL) {
		std::cerr << "Could not open window: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context) {
		fprintf(stderr, "Couldn't create context: %s\n", SDL_GetError());
		return;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (float)width, (float)height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void SimpleSimulator::run() {
	running = true;
	while (running) {
		frameTime = TuioTime::getSessionTime();
		tuioServer->initFrame(frameTime);
		processEvents();
		tuioServer->stopUntouchedMovingCursors();
		tuioServer->stopUntouchedMovingObjects();
		tuioServer->stopUntouchedMovingBlobs();
		tuioServer->stopUntouchedMovingCursors25D();
		tuioServer->stopUntouchedMovingObjects25D();
		tuioServer->stopUntouchedMovingBlobs25D();
		tuioServer->stopUntouchedMovingCursors3D();
		tuioServer->stopUntouchedMovingObjects3D();
		tuioServer->stopUntouchedMovingBlobs3D();
		tuioServer->commitFrame();
		drawFrame();

		// simulate 50Hz compensating the previous processing time
		int delay = 20 - (TuioTime::getSessionTime().getTotalMilliseconds() - frameTime.getTotalMilliseconds());
		if (delay > 0) SDL_Delay(delay);
	}
}

int main(int argc, char* argv[])
{
	/*	if (( argc != 1) && ( argc != 3)) {
				std::cout << "usage: SimpleSimulator [host] [port]\n";
				return 0;
		}*/

#ifndef __MACOSX__
	glutInit(&argc, argv);
#else
	if ((argc > 1) && ((std::string(argv[1]).find("-NSDocumentRevisionsDebugMode") == 0) || (std::string(argv[1]).find("-psn_") == 0))) argc = 1;
#endif

	TuioServer *server = NULL;
	if (argc == 3) {
		server = new TuioServer(argv[1], atoi(argv[2]));
	}
	else server = new TuioServer(); // default is UDP port 3333 on localhost

 // add an additional TUIO/TCP sender
	OscSender *tcp_sender = NULL;
	if (argc == 2) {
		try { tcp_sender = new TcpSender(atoi(argv[1])); }
		catch (std::exception e) { tcp_sender = NULL; }
	}
	else if (argc == 3) {
		try { tcp_sender = new TcpSender(argv[1], atoi(argv[2])); }
		catch (std::exception e) { tcp_sender = NULL; }
	}
	else {
		try { tcp_sender = new TcpSender(3333); }
		catch (std::exception e) { tcp_sender = NULL; }
	}
	if (tcp_sender) server->addOscSender(tcp_sender);


	// add an additional TUIO/WEB sender
	OscSender *web_sender = NULL;
	try { web_sender = new WebSockSender(8080); }
	catch (std::exception e) { web_sender = NULL; }
	if (web_sender) server->addOscSender(web_sender);

	// add an additional TUIO/FLC sender
	OscSender *flash_sender = NULL;
	try { flash_sender = new FlashSender(); }
	catch (std::exception e) { flash_sender = NULL; }
	if (flash_sender) server->addOscSender(flash_sender);

	SimpleSimulator *app = new SimpleSimulator(server);
	app->run();

	delete app;
	delete server;
	return 0;
}
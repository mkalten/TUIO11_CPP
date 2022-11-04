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

#include "TuioPoint.h"

using namespace TUIO;

TuioPoint::TuioPoint (float xp, float yp, float zp) {
	xpos = xp;
	ypos = yp;
	zpos = zp;
	currentTime = TuioTime::getSessionTime();
	startTime = currentTime;

	xposFilter = NULL;
	yposFilter = NULL;
	zposFilter = NULL;
	
	posThreshold = 0.0f;
}

TuioPoint::TuioPoint (TuioTime ttime, float xp, float yp, float zp) {
	xpos = xp;
	ypos = yp;
	zpos = zp;
	currentTime = ttime;
	startTime = currentTime;

	xposFilter = NULL;
	yposFilter = NULL;
	zposFilter = NULL;
	
	posThreshold = 0.0f;
}

TuioPoint::TuioPoint (TuioPoint *tpoint) {
	xpos = tpoint->getX();
	ypos = tpoint->getY();
	zpos = tpoint->getZ();
	currentTime = TuioTime::getSessionTime();
	startTime = currentTime;

	xposFilter = NULL;
	yposFilter = NULL;
	zposFilter = NULL;
	
	posThreshold = 0.0f;
}

void TuioPoint::update (TuioPoint *tpoint) {
	xpos = tpoint->getX();
	ypos = tpoint->getY();
	zpos = tpoint->getZ();
}

void TuioPoint::update (float xp, float yp, float zp) {
	xpos = xp;
	ypos = yp;
	zpos = zp;
}

void TuioPoint::update (TuioTime ttime, float xp, float yp, float zp) {
	
	if (xposFilter && yposFilter && zposFilter) {
		TuioTime diffTime = ttime - currentTime;
		float dt = diffTime.getTotalMilliseconds()/1000.0f;
		xp = xposFilter->filter(xp,dt);
		yp = yposFilter->filter(yp,dt);
		zp = yposFilter->filter(zp, dt);
	}
		
	float dx = fabs(xpos - xp);
	float dy = fabs(ypos - yp);
	float dz = fabs(zpos - zp);
	if ((dx>posThreshold) || (dy>posThreshold) || (dz > posThreshold)) {
		xpos = xp;
		ypos = yp;
		zpos = zp;
	}

	currentTime = ttime;
}

void TuioPoint::update(float xp, float yp) {
	update(xp, yp, 0);
}

void TuioPoint::update(TuioTime ttime, float xp, float yp) {
	update(ttime, xp, yp, 0);
}


float TuioPoint::getX() const{
	return xpos;
}

float TuioPoint::getY() const{
	return ypos;
}

float TuioPoint::getZ() const {
	return zpos;
}

float TuioPoint::getDistance(float xp, float yp, float zp) const{
	float dx = xpos-xp;
	float dy = ypos - yp;
	float dz = zpos - zp;
	return sqrtf(dx*dx+dy*dy+dz*dz);
}

float TuioPoint::getDistance(float xp, float yp) const {
	float dx = xpos - xp;
	float dy = ypos - yp;
	return sqrtf(dx*dx + dy * dy);
}

float TuioPoint::getSpaceDistance(float xp, float yp, float zp, int w, int h, int d) const{
	float dx = w*xpos-w*xp;
	float dy = h*ypos-h*yp;
	float dz = d * zpos - d * zp;
	return sqrtf(dx*dx+dy*dy+dz*dz);
}

float TuioPoint::getDistance(TuioPoint *tpoint) const{
	return getDistance(tpoint->getX(),tpoint->getY(), tpoint->getZ());
}


float TuioPoint::getAngle(float xp, float yp) const {
	float side = xpos - xp;
	float height = ypos - yp;
	float distance = getDistance(xp, yp);

	float angle = (float)(asin(side / distance) + M_PI / 2);
	if (height < 0) angle = 2.0f*(float)M_PI - angle;

	return angle;

}

float TuioPoint::getAngle(TuioPoint *tpoint) const {
	return getAngle(tpoint->getX(), tpoint->getY());
}

float TuioPoint::getAngleDegrees(float xp, float yp) const {
	return ((getAngle(xp, yp) / (float)M_PI)*180.0f);
}

float TuioPoint::getAngleDegrees(TuioPoint *tpoint) const {
	return ((getAngle(tpoint) / (float)M_PI)*180.0f);
}



float TuioPoint::getRollAngle(float xp, float yp, float zp) const{

	return getAngle(xp,yp);
}

float TuioPoint::getRollAngle(TuioPoint *tpoint) const{
	return getAngle(tpoint->getX(),tpoint->getY());
}

float TuioPoint::getRollAngleDegrees(float xp, float yp, float zp) const{
	return ((getAngle(xp,yp)/(float)M_PI)*180.0f);
}

float TuioPoint::getRollAngleDegrees(TuioPoint *tpoint) const{
	return ((getRollAngle(tpoint)/(float)M_PI)*180.0f);
}





float TuioPoint::getPitchAngle(float xp, float yp, float zp) const {

	return getAngle(zp, yp);
}

float TuioPoint::getPitchAngle(TuioPoint *tpoint) const {
	return getAngle(tpoint->getZ(), tpoint->getY());
}

float TuioPoint::getPitchAngleDegrees(float xp, float yp, float zp) const {
	return ((getAngle(zp, yp) / (float)M_PI)*180.0f);
}

float TuioPoint::getPitchAngleDegrees(TuioPoint *tpoint) const {
	return ((getPitchAngle(tpoint) / (float)M_PI)*180.0f);
}

float TuioPoint::getYawAngle(float xp, float yp, float zp) const {

	return getAngle(xp, zp);
}

float TuioPoint::getYawAngle(TuioPoint *tpoint) const {
	return getAngle(tpoint->getX(), tpoint->getZ());
}

float TuioPoint::getYawAngleDegrees(float xp, float yp, float zp) const {
	return ((getAngle(xp, zp) / (float)M_PI)*180.0f);
}

float TuioPoint::getYawAngleDegrees(TuioPoint *tpoint) const {
	return ((getYawAngle(tpoint) / (float)M_PI)*180.0f);
}


int TuioPoint::getSpaceX(int width) const{
	return (int)floor(xpos*width+0.5f);
}

int TuioPoint::getSpaceY(int height) const{
	return (int)floor(ypos*height+0.5f);
}

int TuioPoint::getSpaceZ(int depth) const {
	return (int)floor(zpos*depth + 0.5f);
}

TuioTime TuioPoint::getTuioTime() const{
	return currentTime;
}

TuioTime TuioPoint::getStartTime() const{
	return startTime;
}

void TuioPoint::addPositionThreshold(float thresh) {
	posThreshold = thresh;
}

void TuioPoint::removePositionThreshold() {
	posThreshold = 0.0f;
}

void TuioPoint::addPositionFilter(float mcut, float beta) {

	if (xposFilter) delete xposFilter;
	xposFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
	if (yposFilter) delete yposFilter;
	yposFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioPoint::removePositionFilter() {

	if (xposFilter) delete xposFilter;
	xposFilter = NULL;
	if (yposFilter) delete yposFilter;
	yposFilter = NULL;
	if (zposFilter) delete zposFilter;
	zposFilter = NULL;
}

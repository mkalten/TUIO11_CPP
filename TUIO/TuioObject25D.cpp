/*
 TUIO C++ Library
 Copyright (c) 2022 Nicolas Bremard <nicolas@bremard.fr>
 Based on TuioObject by Martin Kaltenbrunner <martin@tuio.org>

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

#include "TuioObject25D.h"
#include <iostream>

using namespace TUIO;

TuioObject25D::TuioObject25D(TuioTime ttime, long si, int sym, float xp, float yp, float zp, float a) :TuioContainer(ttime, si, xp, yp, zp) {
	symbol_id = sym;
	angle = a;
	rotation_speed = 0.0f;
	rotation_accel = 0.0f;

	angleFilter = NULL;
	angleThreshold = 0.0f;
}

TuioObject25D::TuioObject25D(long si, int sym, float xp, float yp, float zp, float a) :TuioContainer(si, xp, yp, zp) {
	symbol_id = sym;
	angle = a;
	rotation_speed = 0.0f;
	rotation_accel = 0.0f;

	angleFilter = NULL;
	angleThreshold = 0.0f;
}

TuioObject25D::TuioObject25D(TuioObject25D *tobj) :TuioContainer(tobj) {
	symbol_id = tobj->getSymbolID();
	angle = tobj->getAngle();
	rotation_speed = 0.0f;
	rotation_accel = 0.0f;

	angleFilter = NULL;
	angleThreshold = 0.0f;
}

void TuioObject25D::update(TuioTime ttime, float xp, float yp, float zp, float a, float xs, float ys, float zs, float rs, float ma, float ra) {
	TuioContainer::update(ttime, xp, yp, zp, xs, ys, zs, ma);
	angle = a;
	rotation_speed = rs;
	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}


void TuioObject25D::update(float xp, float yp, float zp, float a, float xs, float ys, float zs, float rs, float ma, float ra) {
	TuioContainer::update(xp, yp, zp, xs, ys, zs, ma);
	angle = a;
	rotation_speed = rs;
	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}

void TuioObject25D::update(TuioTime ttime, float xp, float yp, float zp, float a) {
	TuioPoint lastPoint = path.back();
	TuioContainer::update(ttime, xp, yp, zp);

	TuioTime diffTime = currentTime - lastPoint.getTuioTime();
	float dt = diffTime.getTotalMilliseconds() / 1000.0f;
	float last_rotation_speed = rotation_speed;

	float da = a - angle;
	if (da > M_PI) da -= 2 * M_PI;
	else if (da < -M_PI) da += 2 * M_PI;

	float prev_angle = angle;
	if (angleFilter) angle = angleFilter->filter(angle + da, dt);
	else angle = angle + da;
	if (fabs(angle - prev_angle) < angleThreshold) angle = prev_angle;

	if (angle > 2 * M_PI) angle -= 2 * M_PI;
	else if (angle < 0) angle += 2 * M_PI;

	da = angle - prev_angle;
	if (da > M_PI) da -= 2 * M_PI;
	else if (da < -M_PI) da += 2 * M_PI;
	da = da / (2 * M_PI);

	rotation_speed = (float)da / dt;
	rotation_accel = (rotation_speed - last_rotation_speed) / dt;

	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}

void TuioObject25D::stop(TuioTime ttime) {
	update(ttime, xpos, ypos, zpos, angle);
}

void TuioObject25D::update(TuioObject25D *tobj) {
	TuioContainer::update(tobj);
	angle = tobj->getAngle();
	rotation_speed = tobj->getRotationSpeed();
	rotation_accel = tobj->getRotationAccel();
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}

int TuioObject25D::getSymbolID() const {
	return symbol_id;
}

float TuioObject25D::getAngle() const {
	return angle;
}

float TuioObject25D::getAngleDegrees() const {
	return (float)(angle / M_PI * 180);
}

float TuioObject25D::getRotationSpeed() const {
	return rotation_speed;
}

float TuioObject25D::getRotationAccel() const {
	return rotation_accel;
}

bool TuioObject25D::isMoving() const {
	if ((state == TUIO_ACCELERATING) || (state == TUIO_DECELERATING) || (state == TUIO_ROTATING)) return true;
	else return false;
}

void TuioObject25D::addAngleThreshold(float thresh) {
	angleThreshold = thresh;
}

void TuioObject25D::removeAngleThreshold() {
	angleThreshold = 0.0f;
}

void TuioObject25D::addAngleFilter(float mcut, float beta) {

	if (angleFilter) delete angleFilter;
	angleFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioObject25D::removeAngleFilter() {

	if (angleFilter) delete angleFilter;
	angleFilter = NULL;
}

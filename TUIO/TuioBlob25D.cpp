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

#include "TuioBlob25D.h"

using namespace TUIO;

TuioBlob25D::TuioBlob25D(TuioTime ttime, long si, int bi, float xp, float yp, float zp, float a, float w, float h, float f) :TuioContainer(ttime, si, xp, yp, zp) {
	blob_id = bi;
	angle = a;
	width = w;
	height = h;
	area = f;
	rotation_speed = 0.0f;
	rotation_accel = 0.0f;

	angleFilter = NULL;
	angleThreshold = 0.0f;
	widthFilter = NULL;
	heightFilter = NULL;
	sizeThreshold = 0.0f;
}

TuioBlob25D::TuioBlob25D(long si, int bi, float xp, float yp, float zp, float a, float  w, float h, float f) :TuioContainer(si, xp, yp, zp) {
	blob_id = bi;
	angle = a;
	width = w;
	height = h;
	area = f;
	rotation_speed = 0.0f;
	rotation_accel = 0.0f;

	angleFilter = NULL;
	angleThreshold = 0.0f;
	widthFilter = NULL;
	heightFilter = NULL;
	sizeThreshold = 0.0f;
}

TuioBlob25D::TuioBlob25D(TuioBlob25D *tblb) :TuioContainer(tblb) {
	blob_id = tblb->getBlobID();
	angle = tblb->getAngle();
	width = tblb->getWidth();
	height = tblb->getHeight();
	area = tblb->getArea();
	rotation_speed = 0.0f;
	rotation_accel = 0.0f;

	angleFilter = NULL;
	angleThreshold = 0.0f;
	widthFilter = NULL;
	heightFilter = NULL;
	sizeThreshold = 0.0f;
}

int TuioBlob25D::getBlobID() const {
	return blob_id;
}

void TuioBlob25D::setBlobID(int bi) {
	blob_id = bi;
}

void TuioBlob25D::update(TuioTime ttime, float xp, float yp, float zp, float a, float w, float h, float f, float xs, float ys, float zs, float rs, float ma, float ra) {
	TuioContainer::update(ttime, xp, yp, zp, xs, ys, zs, ma);
	angle = a;
	float dw = width - w;
	float dh = height - h;
	width = w;
	height = h;
	area = f;
	rotation_speed = rs;
	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
	if (dw != 0 || dh != 0) state = TUIO_RESIZED; // If size change -> update data
}

void TuioBlob25D::update(float xp, float yp, float zp, float a, float w, float h, float f, float xs, float ys, float zs, float rs, float ma, float ra) {
	TuioContainer::update(xp, yp, zp, xs, ys, zs, ma);
	angle = a;
	float dw = width - w;
	float dh = height - h;
	width = w;
	height = h;
	area = f;
	rotation_speed = rs;
	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
	if (dw != 0 || dh != 0) state = TUIO_RESIZED; // If size change -> update data
}

void TuioBlob25D::update(TuioTime ttime, float xp, float yp, float zp, float a, float w, float h, float f) {
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

	angle = a;

	if (widthFilter && heightFilter) {
		w = widthFilter->filter(w, dt);
		h = heightFilter->filter(h, dt);
	}

	float dw = fabs(width - w);
	float dh = fabs(height - h);
	if ((dw > sizeThreshold) || (dh > sizeThreshold)) {
		width = w;
		height = h;
	}

	area = f;

	rotation_speed = (float)da / dt;
	rotation_accel = (rotation_speed - last_rotation_speed) / dt;

	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
	if(dw!=0 || dh != 0) state = TUIO_RESIZED; // If size change -> update data
}

void TuioBlob25D::stop(TuioTime ttime) {
	update(ttime, xpos, ypos, zpos, angle, width, height, area);
}

void TuioBlob25D::update(TuioBlob25D *tblb) {
	TuioContainer::update(tblb);
	angle = tblb->getAngle();
	width = tblb->getWidth();
	height = tblb->getHeight();
	area = tblb->getArea();
	rotation_speed = tblb->getRotationSpeed();
	rotation_accel = tblb->getRotationAccel();
	state = tblb->getTuioState();
}

float TuioBlob25D::getWidth() const {
	return width;
}

float TuioBlob25D::getHeight() const {
	return height;
}

int TuioBlob25D::getSpaceWidth(int w) const {
	return (int)(w*width);
}

int TuioBlob25D::getSpaceHeight(int h) const {
	return (int)(h*height);
}

float TuioBlob25D::getArea() const {
	return area;
}

float TuioBlob25D::getAngle() const {
	return angle;
}

float TuioBlob25D::getAngleDegrees() const {
	return (float)(angle / M_PI * 180);
}

float TuioBlob25D::getRotationSpeed() const {
	return rotation_speed;
}

float TuioBlob25D::getRotationAccel() const {
	return rotation_accel;
}

bool TuioBlob25D::isMoving() const {
	if ((state == TUIO_ACCELERATING) || (state == TUIO_DECELERATING) || (state == TUIO_ROTATING) || (state == TUIO_RESIZED)) return true;
	else return false;
}

void TuioBlob25D::addAngleThreshold(float thresh) {
	angleThreshold = thresh;
}

void TuioBlob25D::removeAngleThreshold() {
	angleThreshold = 0.0f;
}

void TuioBlob25D::addAngleFilter(float mcut, float beta) {

	if (angleFilter) delete angleFilter;
	angleFilter = new OneEuroFilter(60.0f, mcut, beta, 1.0f);
}

void TuioBlob25D::removeAngleFilter() {

	if (angleFilter) delete angleFilter;
	angleFilter = NULL;
}

void TuioBlob25D::addSizeThreshold(float thresh) {
	sizeThreshold = thresh;
}

void TuioBlob25D::removeSizeThreshold() {
	sizeThreshold = 0.0f;
}

void TuioBlob25D::addSizeFilter(float mcut, float beta) {

	if (widthFilter) delete widthFilter;
	widthFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
	if (heightFilter) delete heightFilter;
	heightFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioBlob25D::removeSizeFilter() {

	if (widthFilter) delete widthFilter;
	widthFilter = NULL;
	if (heightFilter) delete heightFilter;
	heightFilter = NULL;
}


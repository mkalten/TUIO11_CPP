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

#include "TuioBlob3D.h"

using namespace TUIO;

TuioBlob3D::TuioBlob3D(TuioTime ttime, long si, int bi, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v) :TuioContainer(ttime, si, xp, yp, zp) {
	blob_id = bi;

	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = 0.0f;
	pitch_speed = 0.0f;
	yaw_speed = 0.0f;
	roll_accel = 0.0f;
	pitch_accel = 0.0f;
	yaw_accel = 0.0f;
	rollFilter = NULL;
	pitchFilter = NULL;
	yawFilter = NULL;
	rollThreshold = 0.0f;
	pitchThreshold = 0.0f;
	yawThreshold = 0.0f;

	rotation_accel = 0.0f;


	width = w;
	height = h;
	depth = d;
	volume = v;
	widthFilter = NULL;
	heightFilter = NULL;
	depthFilter = NULL;
	widthThreshold = 0.0f;
	heightThreshold = 0.0f;
	depthThreshold = 0.0f;

}

TuioBlob3D::TuioBlob3D(long si, int bi, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v) :TuioContainer(si, xp, yp, zp) {
	blob_id = bi;

	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = 0.0f;
	pitch_speed = 0.0f;
	yaw_speed = 0.0f;
	roll_accel = 0.0f;
	pitch_accel = 0.0f;
	yaw_accel = 0.0f;
	rollFilter = NULL;
	pitchFilter = NULL;
	yawFilter = NULL;
	rollThreshold = 0.0f;
	pitchThreshold = 0.0f;
	yawThreshold = 0.0f;

	rotation_accel = 0.0f;


	width = w;
	height = h;
	depth = d;
	volume = v;
	widthFilter = NULL;
	heightFilter = NULL;
	depthFilter = NULL;
	widthThreshold = 0.0f;
	heightThreshold = 0.0f;
	depthThreshold = 0.0f;
}

TuioBlob3D::TuioBlob3D(TuioBlob3D *tblb) :TuioContainer(tblb) {
	blob_id = tblb->getBlobID();

	volume = tblb->getVolume();

	roll = tblb->getRoll();
	pitch = tblb->getPitch();
	yaw = tblb->getYaw();
	roll_speed = tblb->getRollSpeed();
	pitch_speed = tblb->getPitchSpeed();
	yaw_speed = tblb->getYawSpeed();
	roll_accel = tblb->getRollAccel();
	pitch_accel = tblb->getPitchAccel();
	yaw_accel = tblb->getYawAccel();

	rotation_accel = tblb->getRotationAccel();

	rollFilter = NULL;
	pitchFilter = NULL;
	yawFilter = NULL;
	rollThreshold = 0.0f;
	pitchThreshold = 0.0f;
	yawThreshold = 0.0f;


	width = tblb->getWidth();
	height = tblb->getHeight();
	depth = tblb->getDepth();

	widthFilter = NULL;
	heightFilter = NULL;
	depthFilter = NULL;
	widthThreshold = 0.0f;
	heightThreshold = 0.0f;
	depthThreshold = 0.0f;
}

int TuioBlob3D::getBlobID() const {
	return blob_id;
}

void TuioBlob3D::setBlobID(int bi) {
	blob_id = bi;
}

void TuioBlob3D::update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra) {
	TuioContainer::update(ttime, xp, yp, zp, xs, ys, zs, ma);
	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = ros;
	yaw_speed = pos;
	pitch_speed = yos;


	float dw = width - w;
	float dh = height - h;
	float dd = depth - d;
	width = w;
	height = h;
	depth = d;
	volume = v;

	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
	if (dw != 0 || dh != 0 || dd != 0) state = TUIO_RESIZED; // If size change -> update data
}

void TuioBlob3D::update(float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra) {
	TuioContainer::update(xp, yp, zp, xs, ys, zs, ma);
	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = ros;
	yaw_speed = pos;
	pitch_speed = yos;

	float dw = width - w;
	float dh = height - h;
	float dd = depth - d;
	width = w;
	height = h;
	depth = d;
	volume = v;

	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
	if (dw != 0 || dh != 0 || dd != 0) state = TUIO_RESIZED; // If size change -> update data
}

void TuioBlob3D::update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo, float w, float h, float d, float v) {
	TuioPoint lastPoint = path.back();
	TuioContainer::update(ttime, xp, yp, zp);

	TuioTime diffTime = currentTime - lastPoint.getTuioTime();
	float dt = diffTime.getTotalMilliseconds() / 1000.0f;


	float last_roll_speed = roll_speed;
	float dr = ro - roll;
	if (dr > M_PI) dr -= 2 * M_PI;
	else if (dr < -M_PI) dr += 2 * M_PI;

	float prev_roll = roll;
	if (rollFilter) roll = rollFilter->filter(roll + dr, dt);
	else roll = roll + dr;
	if (fabs(roll - prev_roll) < rollThreshold) roll = prev_roll;

	if (roll > 2 * M_PI) roll -= 2 * M_PI;
	else if (roll < 0) roll += 2 * M_PI;

	dr = roll - prev_roll;
	if (dr > M_PI) dr -= 2 * M_PI;
	else if (dr < -M_PI) dr += 2 * M_PI;
	dr = dr / (2 * M_PI);

	roll_speed = (float)dr / dt;
	roll_accel = (roll_speed - last_roll_speed) / dt;


	float last_pitch_speed = pitch_speed;
	float dp = po - pitch;
	if (dp > M_PI) dp -= 2 * M_PI;
	else if (dp < -M_PI) dp += 2 * M_PI;

	float prev_pitch = pitch;
	if (pitchFilter) pitch = pitchFilter->filter(pitch + dp, dt);
	else pitch = pitch + dp;
	if (fabs(pitch - prev_pitch) < pitchThreshold) pitch = prev_pitch;

	if (pitch > 2 * M_PI) pitch -= 2 * M_PI;
	else if (pitch < 0) pitch += 2 * M_PI;

	dp = pitch - prev_pitch;
	if (dp > M_PI) dp -= 2 * M_PI;
	else if (dp < -M_PI) dp += 2 * M_PI;
	dp = dp / (2 * M_PI);

	pitch_speed = (float)dp / dt;
	pitch_accel = (pitch_speed - last_pitch_speed) / dt;


	float last_yaw_speed = yaw_speed;
	float dy = yo - yaw;
	if (dy > M_PI) dy -= 2 * M_PI;
	else if (dy < -M_PI) dy += 2 * M_PI;

	float prev_yaw = yaw;
	if (yawFilter) yaw = yawFilter->filter(yaw + dy, dt);
	else yaw = yaw + dy;
	if (fabs(yaw - prev_yaw) < yawThreshold) yaw = prev_yaw;

	if (yaw > 2 * M_PI) yaw -= 2 * M_PI;
	else if (yaw < 0) yaw += 2 * M_PI;

	dy = yaw - prev_yaw;
	if (dy > M_PI) dy -= 2 * M_PI;
	else if (dy < -M_PI) dy += 2 * M_PI;
	dy = dy / (2 * M_PI);

	yaw_speed = (float)dy / dt;
	yaw_accel = (yaw_speed - last_yaw_speed) / dt;

	roll = ro;
	pitch = po;
	yaw = yo;

	rotation_accel = (roll_accel + pitch_accel + yaw_accel) / 3;

	if (((roll != 0) || (pitch != 0) || (yaw != 0)) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;




	if (widthFilter && heightFilter && depthFilter) {
		w = widthFilter->filter(w, dt);
		h = heightFilter->filter(h, dt);
		d = depthFilter->filter(d, dt);
	}

	float dw = fabs(width - w);
	float dh = fabs(height - h);
	float dd = fabs(depth - d);
	if ((dw > widthThreshold) || (dh > heightThreshold) || (dd > depthThreshold)) {
		width = w;
		height = h;
		depth = d;
	}


	if (dw != 0 || dh != 0 || dd != 0) state = TUIO_RESIZED; // If size change -> update data

	volume = v;

}

void TuioBlob3D::stop(TuioTime ttime) {
	update(ttime, xpos, ypos, zpos, roll, pitch, yaw , width, height, depth, volume);
}

void TuioBlob3D::update(TuioBlob3D *tblb) {
	TuioContainer::update(tblb);

	roll = tblb->getRoll();
	pitch = tblb->getPitch();
	yaw = tblb->getYaw();
	roll_speed = tblb->getRollSpeed();
	pitch_speed = tblb->getPitchSpeed();
	yaw_speed = tblb->getYawSpeed();
	roll_accel = tblb->getRollAccel();
	pitch_accel = tblb->getPitchAccel();
	yaw_accel = tblb->getYawAccel();
	rotation_accel = tblb->getRotationAccel();

	state = tblb->getTuioState();


	width = tblb->getWidth();
	height = tblb->getHeight();
	depth = tblb->getDepth();
	volume = tblb->getVolume();

}

float TuioBlob3D::getWidth() const {
	return width;
}

float TuioBlob3D::getHeight() const {
	return height;
}

float TuioBlob3D::getDepth() const {
	return depth;
}


int TuioBlob3D::getSpaceWidth(int w) const {
	return (int)(w*width);
}

int TuioBlob3D::getSpaceHeight(int h) const {
	return (int)(h*height);
}

int TuioBlob3D::getSpaceDepth(int d) const {
	return (int)(d*depth);
}

float TuioBlob3D::getVolume() const {
	return volume;
}



float TuioBlob3D::getRoll() const {
	return roll;
}

float TuioBlob3D::getRollDegrees() const {
	return (float)(roll / M_PI * 180);
}

float TuioBlob3D::getRollSpeed() const {
	return roll_speed;
}

float TuioBlob3D::getRollAccel() const {
	return roll_accel;
}

float TuioBlob3D::getPitch() const {
	return pitch;
}

float TuioBlob3D::getPitchDegrees() const {
	return (float)(pitch / M_PI * 180);
}

float TuioBlob3D::getPitchSpeed() const {
	return pitch_speed;
}

float TuioBlob3D::getPitchAccel() const {
	return pitch_accel;
}

float TuioBlob3D::getYaw() const {
	return yaw;
}

float TuioBlob3D::getYawDegrees() const {
	return (float)(yaw / M_PI * 180);
}

float TuioBlob3D::getYawSpeed() const {
	return yaw_speed;
}

float TuioBlob3D::getYawAccel() const {
	return yaw_accel;
}

float TuioBlob3D::getRotationAccel() const {
	return rotation_accel;
}



bool TuioBlob3D::isMoving() const {
	if ((state == TUIO_ACCELERATING) || (state == TUIO_DECELERATING) || (state == TUIO_ROTATING) || (state == TUIO_RESIZED)) return true;
	else return false;
}


void TuioBlob3D::addRollThreshold(float thresh) {
	rollThreshold = thresh;
}

void TuioBlob3D::addPitchThreshold(float thresh) {
	pitchThreshold = thresh;
}

void TuioBlob3D::addYawThreshold(float thresh) {
	yawThreshold = thresh;
}


void TuioBlob3D::removeRollThreshold() {
	rollThreshold = 0.0f;
}

void TuioBlob3D::removePitchThreshold() {
	pitchThreshold = 0.0f;
}

void TuioBlob3D::removeYawThreshold() {
	yawThreshold = 0.0f;
}
void TuioBlob3D::addRollFilter(float mcut, float beta) {

	if (rollFilter) delete rollFilter;
	rollFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioBlob3D::addPitchFilter(float mcut, float beta) {

	if (pitchFilter) delete pitchFilter;
	pitchFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioBlob3D::addYawFilter(float mcut, float beta) {

	if (yawFilter) delete yawFilter;
	yawFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioBlob3D::removeRollFilter() {

	if (rollFilter) delete rollFilter;
	rollFilter = NULL;
}

void TuioBlob3D::removePitchFilter() {

	if (pitchFilter) delete pitchFilter;
	pitchFilter = NULL;
}

void TuioBlob3D::removeYawFilter() {

	if (yawFilter) delete yawFilter;
	yawFilter = NULL;
}







void TuioBlob3D::addWidthThreshold(float thresh) {
	widthThreshold = thresh;
}

void TuioBlob3D::addHeightThreshold(float thresh) {
	heightThreshold = thresh;
}

void TuioBlob3D::addDepthThreshold(float thresh) {
	depthThreshold = thresh;
}


void TuioBlob3D::removeWidthThreshold() {
	widthThreshold = 0.0f;
}

void TuioBlob3D::removeHeightThreshold() {
	heightThreshold = 0.0f;
}

void TuioBlob3D::removeDepthThreshold() {
	depthThreshold = 0.0f;
}
void TuioBlob3D::addWidthFilter(float mcut, float beta) {

	if (widthFilter) delete widthFilter;
	widthFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioBlob3D::addHeightFilter(float mcut, float beta) {

	if (heightFilter) delete heightFilter;
	heightFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioBlob3D::addDepthFilter(float mcut, float beta) {

	if (depthFilter) delete depthFilter;
	depthFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioBlob3D::removeWidthFilter() {

	if (widthFilter) delete widthFilter;
	widthFilter = NULL;
}

void TuioBlob3D::removeHeightFilter() {

	if (heightFilter) delete heightFilter;
	heightFilter = NULL;
}

void TuioBlob3D::removeDepthFilter() {

	if (depthFilter) delete depthFilter;
	depthFilter = NULL;
}








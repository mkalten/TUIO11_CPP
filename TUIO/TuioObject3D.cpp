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

#include "TuioObject3D.h"
#include <iostream>

using namespace TUIO;

TuioObject3D::TuioObject3D(TuioTime ttime, long si, int sym, float xp, float yp, float zp, float ro, float po, float yo) :TuioContainer(ttime, si, xp, yp, zp) {
	symbol_id = sym;
	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = 0.0f;
	pitch_speed = 0.0f;
	yaw_speed = 0.0f;
	roll_accel = 0.0f;
	pitch_accel = 0.0f;
	yaw_accel = 0.0f;
	rotation_accel = 0.0f;

	rollFilter = NULL;
	pitchFilter = NULL;
	yawFilter = NULL;
	rollThreshold = 0.0f;
	pitchThreshold = 0.0f;
	yawThreshold = 0.0f;
}

TuioObject3D::TuioObject3D(long si, int sym, float xp, float yp, float zp, float ro, float po, float yo) :TuioContainer(si, xp, yp, zp) {
	symbol_id = sym;
	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = 0.0f;
	pitch_speed = 0.0f;
	yaw_speed = 0.0f;
	roll_accel = 0.0f;
	pitch_accel = 0.0f;
	yaw_accel = 0.0f;
	rotation_accel = 0.0f;

	rollFilter = NULL;
	pitchFilter = NULL;
	yawFilter = NULL;
	rollThreshold = 0.0f;
	pitchThreshold = 0.0f;
	yawThreshold = 0.0f;
}

TuioObject3D::TuioObject3D(TuioObject3D *tobj) :TuioContainer(tobj) {
	symbol_id = tobj->getSymbolID();

	roll = tobj->getRoll();
	pitch = tobj->getPitch();
	yaw = tobj->getYaw();
	roll_speed = tobj->getRollSpeed();
	pitch_speed = tobj->getPitchSpeed();
	yaw_speed = tobj->getYawSpeed();
	roll_accel = tobj->getRollAccel();
	pitch_accel = tobj->getPitchAccel();
	yaw_accel = tobj->getYawAccel();
	rotation_accel = tobj->getRotationAccel();

	rollFilter = NULL;
	pitchFilter = NULL;
	yawFilter = NULL;
	rollThreshold = 0.0f;
	pitchThreshold = 0.0f;
	yawThreshold = 0.0f;

	rollFilter = NULL;
	pitchFilter = NULL;
	yawFilter = NULL;
	rollThreshold = 0.0f;
	pitchThreshold = 0.0f;
	yawThreshold = 0.0f;
}

void TuioObject3D::update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra) {
	TuioContainer::update(ttime, xp, yp, zp, xs, ys, zs, ma);
	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = ros;
	yaw_speed = pos;
	pitch_speed = yos;
	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}


void TuioObject3D::update(float xp, float yp, float zp, float ro, float po, float yo, float xs, float ys, float zs, float ros, float pos, float yos, float ma, float ra) {
	TuioContainer::update(xp, yp, zp, xs, ys, zs, ma);
	roll = ro;
	pitch = po;
	yaw = yo;
	roll_speed = ros;
	yaw_speed = pos;
	pitch_speed = yos;
	rotation_accel = ra;
	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}

void TuioObject3D::update(TuioTime ttime, float xp, float yp, float zp, float ro, float po, float yo) {
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


	rotation_accel = (roll_accel + pitch_accel + yaw_accel ) / 3;

	if (((roll != 0)|| (pitch != 0)|| (yaw != 0)) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}

void TuioObject3D::stop(TuioTime ttime) {
	update(ttime, xpos, ypos, zpos);
}

void TuioObject3D::update(TuioObject3D *tobj) {
	TuioContainer::update(tobj);

	roll = tobj->getRoll();
	pitch = tobj->getPitch();
	yaw = tobj->getYaw();
	roll_speed = tobj->getRollSpeed();
	pitch_speed = tobj->getPitchSpeed();
	yaw_speed = tobj->getYawSpeed();
	roll_accel = tobj->getRollAccel();
	pitch_accel = tobj->getPitchAccel();
	yaw_accel = tobj->getYawAccel();
	rotation_accel = tobj->getRotationAccel();

	if ((rotation_accel != 0) && (state == TUIO_STOPPED)) state = TUIO_ROTATING;
}

int TuioObject3D::getSymbolID() const {
	return symbol_id;
}

float TuioObject3D::getRoll() const {
	return roll;
}

float TuioObject3D::getRollDegrees() const {
	return (float)(roll / M_PI * 180);
}

float TuioObject3D::getRollSpeed() const {
	return roll_speed;
}

float TuioObject3D::getRollAccel() const {
	return roll_accel;
}

float TuioObject3D::getPitch() const {
	return pitch;
}

float TuioObject3D::getPitchDegrees() const {
	return (float)(pitch / M_PI * 180);
}

float TuioObject3D::getPitchSpeed() const {
	return pitch_speed;
}

float TuioObject3D::getPitchAccel() const {
	return pitch_accel;
}

float TuioObject3D::getYaw() const {
	return yaw;
}

float TuioObject3D::getYawDegrees() const {
	return (float)(yaw / M_PI * 180);
}

float TuioObject3D::getYawSpeed() const {
	return yaw_speed;
}

float TuioObject3D::getYawAccel() const {
	return yaw_accel;
}

float TuioObject3D::getRotationAccel() const {
	return rotation_accel;
}

bool TuioObject3D::isMoving() const {
	if ((state == TUIO_ACCELERATING) || (state == TUIO_DECELERATING) || (state == TUIO_ROTATING)) return true;
	else return false;
}

void TuioObject3D::addRollThreshold(float thresh) {
	rollThreshold = thresh;
}

void TuioObject3D::addPitchThreshold(float thresh) {
	pitchThreshold = thresh;
}

void TuioObject3D::addYawThreshold(float thresh) {
	yawThreshold = thresh;
}


void TuioObject3D::removeRollThreshold() {
	rollThreshold = 0.0f;
}

void TuioObject3D::removePitchThreshold() {
	pitchThreshold = 0.0f;
}

void TuioObject3D::removeYawThreshold() {
	yawThreshold = 0.0f;
}
void TuioObject3D::addRollFilter(float mcut, float beta) {

	if (rollFilter) delete rollFilter;
	rollFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioObject3D::addPitchFilter(float mcut, float beta) {

	if (pitchFilter) delete pitchFilter;
	pitchFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioObject3D::addYawFilter(float mcut, float beta) {

	if (yawFilter) delete yawFilter;
	yawFilter = new OneEuroFilter(60.0f, mcut, beta, 10.0f);
}

void TuioObject3D::removeRollFilter() {

	if (rollFilter) delete rollFilter;
	rollFilter = NULL;
}

void TuioObject3D::removePitchFilter() {

	if (pitchFilter) delete pitchFilter;
	pitchFilter = NULL;
}

void TuioObject3D::removeYawFilter() {

	if (yawFilter) delete yawFilter;
	yawFilter = NULL;
}

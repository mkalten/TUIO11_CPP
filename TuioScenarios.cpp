/*
	TUIO C++ Audit - part of the reacTIVision project
	http://reactivision.sourceforge.net/

	Copyright (c) 2005-2026 Martin Kaltenbrunner <martin@tuio.org>

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

#include "TuioScenarios.h"
#include "osc/OscOutboundPacketStream.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAW_SOURCE_NAME "TuioAudit"

static float orbitX(float t, float phase, float radius) {
	return 0.5f + radius*(float)cos(2.0*M_PI*t + phase);
}
static float orbitY(float t, float phase, float radius) {
	return 0.5f + radius*(float)sin(2.0*M_PI*t + phase);
}

// removes all remaining entities of all three profiles
void TuioScenario::stop(TuioServer *server) {
	server->resetTuioObjects();
	server->resetTuioCursors();
	server->resetTuioBlobs();
}

// a single cursor on a continuous circular path
class CircleScenario : public TuioScenario {
public:
	CircleScenario() : cursor(NULL) {}
	const char *name() { return "circle"; }
	const char *description() { return "one cursor on a continuous circular path"; }
	void start(TuioServer *server) {
		cursor = server->addTuioCursor(orbitX(0,0,0.25f),orbitY(0,0,0.25f));
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		if (cursor) server->updateTuioCursor(cursor,orbitX(t*0.25f,0,0.25f),orbitY(t*0.25f,0,0.25f));
	}
	void stop(TuioServer *server) { cursor = NULL; TuioScenario::stop(server); }
private:
	TuioCursor *cursor;
};

// multiple cursors on phase-offset circles
class CursorsScenario : public TuioScenario {
public:
	static const int COUNT = 5;
	CursorsScenario() {
		for (int i=0;i<COUNT;i++) cursors[i] = NULL;
	}
	const char *name() { return "cursors"; }
	const char *description() { return "five cursors on phase-offset circular paths"; }
	void start(TuioServer *server) {
		for (int i=0;i<COUNT;i++)
			cursors[i] = server->addTuioCursor(orbitX(0,i*2.0f*(float)M_PI/COUNT,0.25f),orbitY(0,i*2.0f*(float)M_PI/COUNT,0.25f));
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		for (int i=0;i<COUNT;i++) {
			float phase = i*2.0f*(float)M_PI/COUNT;
			if (cursors[i]) server->updateTuioCursor(cursors[i],orbitX(t*0.25f,phase,0.25f),orbitY(t*0.25f,phase,0.25f));
		}
	}
	void stop(TuioServer *server) {
		for (int i=0;i<COUNT;i++) cursors[i] = NULL;
		TuioScenario::stop(server);
	}
private:
	TuioCursor *cursors[COUNT];
};

// one object orbiting while rotating
class ObjectScenario : public TuioScenario {
public:
	ObjectScenario() : object(NULL) {}
	const char *name() { return "object"; }
	const char *description() { return "one object orbiting while rotating"; }
	void start(TuioServer *server) {
		object = server->addTuioObject(0,orbitX(0,0,0.3f),orbitY(0,0,0.3f),0.0f);
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		if (object) server->updateTuioObject(object,orbitX(t*0.2f,0,0.3f),orbitY(t*0.2f,0,0.3f),t);
	}
	void stop(TuioServer *server) { object = NULL; TuioScenario::stop(server); }
private:
	TuioObject *object;
};

// several objects with distinct symbol IDs
class ObjectsScenario : public TuioScenario {
public:
	static const int COUNT = 4;
	ObjectsScenario() {
		for (int i=0;i<COUNT;i++) objects[i] = NULL;
	}
	const char *name() { return "objects"; }
	const char *description() { return "four objects with distinct symbol IDs"; }
	void start(TuioServer *server) {
		for (int i=0;i<COUNT;i++)
			objects[i] = server->addTuioObject(i,orbitX(0,i*2.0f*(float)M_PI/COUNT,0.3f),orbitY(0,i*2.0f*(float)M_PI/COUNT,0.3f),i*(float)M_PI/2.0f);
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		for (int i=0;i<COUNT;i++) {
			float phase = i*2.0f*(float)M_PI/COUNT;
			if (objects[i]) server->updateTuioObject(objects[i],orbitX(t*0.2f,phase,0.3f),orbitY(t*0.2f,phase,0.3f),i*(float)M_PI/2.0f+t*(i+1)*0.25f);
		}
	}
	void stop(TuioServer *server) {
		for (int i=0;i<COUNT;i++) objects[i] = NULL;
		TuioScenario::stop(server);
	}
private:
	TuioObject *objects[COUNT];
};

// one blob moving, rotating and pulsing its dimensions
class BlobScenario : public TuioScenario {
public:
	BlobScenario() : blob(NULL) {}
	const char *name() { return "blob"; }
	const char *description() { return "one blob moving, rotating and pulsing its size"; }
	void start(TuioServer *server) {
		blob = server->addTuioBlob(0.5f,0.5f,0.0f,0.1f,0.1f,0.01f);
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		if (!blob) return;
		float pulse = 0.05f*(1.0f+(float)sin(2.0*M_PI*t));
		server->updateTuioBlob(blob,orbitX(t*0.2f,0,0.3f),orbitY(t*0.2f,0,0.3f),t,pulse,pulse,pulse*pulse);
	}
	void stop(TuioServer *server) { blob = NULL; TuioScenario::stop(server); }
private:
	TuioBlob *blob;
};

// object, cursor and blob simultaneously on all three profiles
class MixedScenario : public TuioScenario {
public:
	MixedScenario() : object(NULL), cursor(NULL), blob(NULL) {}
	const char *name() { return "mixed"; }
	const char *description() { return "one object, cursor and blob on all three profiles"; }
	void start(TuioServer *server) {
		object = server->addTuioObject(0,orbitX(0,0,0.3f),orbitY(0,0,0.3f),0.0f);
		cursor = server->addTuioCursor(orbitX(0,0,0.2f),orbitY(0,0,0.2f));
		blob   = server->addTuioBlob(orbitX(0,0,0.4f),orbitY(0,0,0.4f),0.0f,0.1f,0.1f,0.01f);
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		if (object) server->updateTuioObject(object,orbitX(t*0.2f,0,0.3f),orbitY(t*0.2f,0,0.3f),t);
		if (cursor) server->updateTuioCursor(cursor,orbitX(t*0.3f,(float)M_PI,0.2f),orbitY(t*0.3f,(float)M_PI,0.2f));
		if (blob)   server->updateTuioBlob(blob,orbitX(t*0.15f,(float)M_PI,0.4f),orbitY(t*0.15f,(float)M_PI,0.4f),t,0.1f,0.1f,0.01f);
	}
	void stop(TuioServer *server) {
		object = NULL; cursor = NULL; blob = NULL;
		TuioScenario::stop(server);
	}
private:
	TuioObject *object;
	TuioCursor *cursor;
	TuioBlob *blob;
};

// one entity of each profile added once, then only alive/fseq frames
class StaticScenario : public TuioScenario {
public:
	const char *name() { return "static"; }
	const char *description() { return "stationary entities, alive messages only after the first frame"; }
	void start(TuioServer *server) {
		server->addTuioObject(0,0.3f,0.5f,0.0f);
		server->addTuioCursor(0.5f,0.5f);
		server->addTuioBlob(0.7f,0.5f,0.0f,0.1f,0.1f,0.01f);
	}
	void update(TuioServer *, float, float) {}
};

// no entities at all, only source/alive/fseq frames
class EmptyScenario : public TuioScenario {
public:
	const char *name() { return "empty"; }
	const char *description() { return "no entities, empty alive messages only"; }
	void update(TuioServer *, float, float) {}
};

// a cursor touring the four corners with exact 0.0/1.0 coordinates
class BoundsScenario : public TuioScenario {
public:
	// each corner: 0.5 s pause, 0.5 s travel to the next corner
	static const float LEG;
	BoundsScenario() : cursor(NULL) {}
	const char *name() { return "bounds"; }
	const char *description() { return "one cursor touring the corners with exact 0.0 and 1.0 coordinates"; }
	void start(TuioServer *server) {
		cursor = server->addTuioCursor(0.0f,0.0f);
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		if (!cursor) return;
		int leg = (int)(t/LEG);
		float phase = t - leg*LEG;
		int from = leg%4, to = (leg+1)%4;
		float x = corners[from][0], y = corners[from][1];
		if (phase>LEG/2.0f) {
			float k = (phase-LEG/2.0f)/(LEG/2.0f);
			x += (corners[to][0]-x)*k;
			y += (corners[to][1]-y)*k;
		}
		server->updateTuioCursor(cursor,x,y);
	}
	void stop(TuioServer *server) { cursor = NULL; TuioScenario::stop(server); }
private:
	static const float corners[4][2];
	TuioCursor *cursor;
};
const float BoundsScenario::LEG = 1.0f;
const float BoundsScenario::corners[4][2] = {{0.0f,0.0f},{1.0f,0.0f},{1.0f,1.0f},{0.0f,1.0f}};

// a cursor added, moved briefly and removed on a one second cycle
class ChurnScenario : public TuioScenario {
public:
	ChurnScenario() : cursor(NULL) {}
	const char *name() { return "churn"; }
	const char *description() { return "a cursor added, moved and removed every second (session ID churn)"; }
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		float phase = t - (float)floor(t);
		int cycle = (int)floor(t);
		if (phase<0.1f) {
			if (!cursor) {
				float x = 0.5f + 0.2f*(float)cos(2.0*M_PI*cycle/7.0);
				float y = 0.5f + 0.2f*(float)sin(2.0*M_PI*cycle/7.0);
				cursor = server->addTuioCursor(x,y);
			}
		} else if (phase<0.8f) {
			if (cursor) {
				float x = cursor->getX() + 0.001f;
				float y = cursor->getY() + 0.001f;
				server->updateTuioCursor(cursor,x,y);
			}
		} else if (cursor) {
			server->removeTuioCursor(cursor);
			cursor = NULL;
		}
	}
	void stop(TuioServer *server) { cursor = NULL; TuioScenario::stop(server); }
private:
	TuioCursor *cursor;
};

// many cursors held simultaneously with a slight wobble
class BurstScenario : public TuioScenario {
public:
	static const int COUNT = 16;
	BurstScenario() {
		for (int i=0;i<COUNT;i++) cursors[i] = NULL;
	}
	const char *name() { return "burst"; }
	const char *description() { return "16 cursors held simultaneously (large alive lists)"; }
	void start(TuioServer *server) {
		for (int i=0;i<COUNT;i++) {
			float x = 0.1f + 0.8f*(i%4)/3.0f;
			float y = 0.1f + 0.8f*(i/4)/3.0f;
			cursors[i] = server->addTuioCursor(x,y);
		}
	}
	void update(TuioServer *server, float t, float dt) {
		(void)dt;
		for (int i=0;i<COUNT;i++) {
			if (!cursors[i]) continue;
			float x = 0.1f + 0.8f*(i%4)/3.0f + 0.01f*(float)sin(2.0*M_PI*t+i);
			float y = 0.1f + 0.8f*(i/4)/3.0f + 0.01f*(float)cos(2.0*M_PI*t+i);
			server->updateTuioCursor(cursors[i],x,y);
		}
	}
	void stop(TuioServer *server) {
		for (int i=0;i<COUNT;i++) cursors[i] = NULL;
		TuioScenario::stop(server);
	}
private:
	TuioCursor *cursors[COUNT];
};

// base class for scenarios emitting hand-crafted /tuio/2Dcur bundles
// that violate the protocol on purpose — a cursor (s_id 4242) orbits
// while subclasses decide which messages the bundle contains
class RawCursorScenario : public TuioScenario {
public:
	RawCursorScenario() : fseq(0) {}
	bool raw() { return true; }
	void update(TuioServer *, float, float) {}

	void updateRaw(std::vector<OscSender*> &senders, float t, float dt) {
		(void)dt;
		char buffer[8192];
		osc::OutboundPacketStream packet(buffer,sizeof(buffer));
		packet << osc::BeginBundleImmediate;
		writeMessages(packet,t);
		packet << osc::EndBundle;
		for (std::vector<OscSender*>::iterator s=senders.begin(); s!=senders.end(); s++)
			(*s)->sendOscPacket(&packet);
		fseq++;
	}

protected:
	// subclasses emit the bundle's messages; x/y provide the orbiting position
	virtual void writeMessages(osc::OutboundPacketStream &packet, float t) = 0;

	void writeSource(osc::OutboundPacketStream &packet) {
		packet << osc::BeginMessage("/tuio/2Dcur") << "source" << RAW_SOURCE_NAME << osc::EndMessage;
	}
	void writeAlive(osc::OutboundPacketStream &packet) {
		packet << osc::BeginMessage("/tuio/2Dcur") << "alive" << SID << osc::EndMessage;
	}
	void writeFseq(osc::OutboundPacketStream &packet) {
		packet << osc::BeginMessage("/tuio/2Dcur") << "fseq" << fseq << osc::EndMessage;
	}
	static float cursorX(float t) { return orbitX(t*0.25f,0,0.25f); }
	static float cursorY(float t) { return orbitY(t*0.25f,0,0.25f); }

	static const int SID = 4242;
	int fseq;
};

// source + set + fseq, the alive message is missing
class NoAliveScenario : public RawCursorScenario {
public:
	const char *name() { return "noalive"; }
	const char *description() { return "2Dcur bundles without an alive message"; }
	void writeMessages(osc::OutboundPacketStream &packet, float t) {
		writeSource(packet);
		packet << osc::BeginMessage("/tuio/2Dcur") << "set" << SID << cursorX(t) << cursorY(t) << 0.0f << 0.0f << 0.0f << osc::EndMessage;
		writeFseq(packet);
	}
};

// only a set message, no source, alive or fseq
class SetOnlyScenario : public RawCursorScenario {
public:
	const char *name() { return "setonly"; }
	const char *description() { return "2Dcur bundles with only a set message"; }
	void writeMessages(osc::OutboundPacketStream &packet, float t) {
		packet << osc::BeginMessage("/tuio/2Dcur") << "set" << SID << cursorX(t) << cursorY(t) << 0.0f << 0.0f << 0.0f << osc::EndMessage;
	}
};

// source + alive + set, the fseq message is missing
class NoFseqScenario : public RawCursorScenario {
public:
	const char *name() { return "nofseq"; }
	const char *description() { return "2Dcur bundles without an fseq message"; }
	void writeMessages(osc::OutboundPacketStream &packet, float t) {
		writeSource(packet);
		writeAlive(packet);
		packet << osc::BeginMessage("/tuio/2Dcur") << "set" << SID << cursorX(t) << cursorY(t) << 0.0f << 0.0f << 0.0f << osc::EndMessage;
	}
};

// set message with only 4 of the expected 6 arguments
class ShortSetScenario : public RawCursorScenario {
public:
	const char *name() { return "shortset"; }
	const char *description() { return "2Dcur set messages missing arguments"; }
	void writeMessages(osc::OutboundPacketStream &packet, float t) {
		writeSource(packet);
		writeAlive(packet);
		packet << osc::BeginMessage("/tuio/2Dcur") << "set" << SID << cursorX(t) << cursorY(t) << 0.0f << osc::EndMessage;
		writeFseq(packet);
	}
};

// set message with 8 instead of the expected 6 arguments
class LongSetScenario : public RawCursorScenario {
public:
	const char *name() { return "longset"; }
	const char *description() { return "2Dcur set messages with extra arguments"; }
	void writeMessages(osc::OutboundPacketStream &packet, float t) {
		writeSource(packet);
		writeAlive(packet);
		packet << osc::BeginMessage("/tuio/2Dcur") << "set" << SID << cursorX(t) << cursorY(t) << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << osc::EndMessage;
		writeFseq(packet);
	}
};

// set for a session ID that is missing from the alive list
class DeadSetScenario : public RawCursorScenario {
public:
	const char *name() { return "deadset"; }
	const char *description() { return "2Dcur set for a session ID absent from the alive list"; }
	void writeMessages(osc::OutboundPacketStream &packet, float t) {
		writeSource(packet);
		// alive deliberately lists a different session ID
		packet << osc::BeginMessage("/tuio/2Dcur") << "alive" << 9999 << osc::EndMessage;
		packet << osc::BeginMessage("/tuio/2Dcur") << "set" << SID << cursorX(t) << cursorY(t) << 0.0f << 0.0f << 0.0f << osc::EndMessage;
		writeFseq(packet);
	}
};

// set message with a string argument where a float is expected
class BadTypeScenario : public RawCursorScenario {
public:
	const char *name() { return "badtype"; }
	const char *description() { return "2Dcur set messages with wrongly-typed arguments"; }
	void writeMessages(osc::OutboundPacketStream &packet, float t) {
		writeSource(packet);
		writeAlive(packet);
		packet << osc::BeginMessage("/tuio/2Dcur") << "set" << SID << cursorX(t) << "bogus" << 0.0f << 0.0f << 0.0f << osc::EndMessage;
		writeFseq(packet);
	}
};

static CircleScenario   scenario_circle;
static CursorsScenario  scenario_cursors;
static ObjectScenario   scenario_object;
static ObjectsScenario  scenario_objects;
static BlobScenario     scenario_blob;
static MixedScenario    scenario_mixed;
static StaticScenario   scenario_static;
static EmptyScenario    scenario_empty;
static BoundsScenario   scenario_bounds;
static ChurnScenario    scenario_churn;
static BurstScenario    scenario_burst;
static NoAliveScenario  scenario_noalive;
static SetOnlyScenario  scenario_setonly;
static NoFseqScenario   scenario_nofseq;
static ShortSetScenario scenario_shortset;
static LongSetScenario  scenario_longset;
static BadTypeScenario  scenario_badtype;
static DeadSetScenario  scenario_deadset;

std::vector<TuioScenario*> getTuioScenarios() {
	std::vector<TuioScenario*> scenarios;
	scenarios.push_back(&scenario_mixed);
	scenarios.push_back(&scenario_circle);
	scenarios.push_back(&scenario_cursors);
	scenarios.push_back(&scenario_object);
	scenarios.push_back(&scenario_objects);
	scenarios.push_back(&scenario_blob);
	scenarios.push_back(&scenario_static);
	scenarios.push_back(&scenario_empty);
	scenarios.push_back(&scenario_bounds);
	scenarios.push_back(&scenario_churn);
	scenarios.push_back(&scenario_burst);
	scenarios.push_back(&scenario_noalive);
	scenarios.push_back(&scenario_setonly);
	scenarios.push_back(&scenario_nofseq);
	scenarios.push_back(&scenario_shortset);
	scenarios.push_back(&scenario_longset);
	scenarios.push_back(&scenario_badtype);
	scenarios.push_back(&scenario_deadset);
	return scenarios;
}

TuioScenario *findTuioScenario(const char *name) {
	std::vector<TuioScenario*> scenarios = getTuioScenarios();
	for (std::vector<TuioScenario*>::iterator s=scenarios.begin(); s!=scenarios.end(); s++)
		if (strcmp((*s)->name(),name)==0) return *s;
	return NULL;
}

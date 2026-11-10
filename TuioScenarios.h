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

#ifndef INCLUDED_TuioScenarios_H
#define INCLUDED_TuioScenarios_H

#include "TuioServer.h"
#include <vector>

using namespace TUIO;

/**
 * A TuioScenario drives a TuioServer with a scripted entity pattern.
 * start() is called once before the first frame, update() once per frame
 * between initFrame() and commitFrame(), and stop() when the scenario ends.
 * The update method receives the elapsed time t and the frame delta dt
 * in seconds, so scenarios produce deterministic, reproducible streams.
 * Scenarios must track the containers they create and clear their
 * references when removing them, since the server deletes the objects.
 */
class TuioScenario {
public:
	virtual ~TuioScenario() {}

	// the short CLI name of this scenario
	virtual const char *name() = 0;

	// a one-line description for the scenario list
	virtual const char *description() = 0;

	// populates the initial entities, called before the first frame
	virtual void start(TuioServer *server) { (void)server; }

	// advances the scenario, called once per frame between initFrame and commitFrame
	virtual void update(TuioServer *server, float t, float dt) = 0;

	// returns true for scenarios emitting hand-crafted OSC bundles through
	// the sender list instead of driving the TuioServer — used for streams
	// that violate the protocol on purpose for client robustness testing
	virtual bool raw() { return false; }

	// called once per frame when raw() is true, delivers raw packets
	// through each sender's sendOscPacket
	virtual void updateRaw(std::vector<OscSender*> &senders, float t, float dt) { (void)senders; (void)t; (void)dt; }

	// removes all remaining entities, the default resets all three profiles
	virtual void stop(TuioServer *server);
};

// returns the registry of all available scenarios, ownership is static
std::vector<TuioScenario*> getTuioScenarios();

// returns the scenario with the provided name, or NULL if there is none
TuioScenario *findTuioScenario(const char *name);

#endif /* INCLUDED_TuioScenarios_H */

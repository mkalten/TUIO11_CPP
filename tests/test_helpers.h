/*
 Shared helpers for the TUIO library tests:
 a TuioListener that records callbacks, an OscSender that captures
 the serialized OSC data, and an OscReceiver stub exposing ProcessPacket.
*/

#ifndef INCLUDED_TEST_HELPERS_H
#define INCLUDED_TEST_HELPERS_H

#include "test_util.h"
#include "TuioListener.h"
#include "OscSender.h"
#include "OscReceiver.h"

using namespace TUIO;

// records every listener callback with the last received container state
class RecordingListener : public TuioListener {
public:
	int addObj = 0, updObj = 0, delObj = 0;
	int addCur = 0, updCur = 0, delCur = 0;
	int addBlb = 0, updBlb = 0, delBlb = 0;
	int refreshCount = 0;

	int lastSessionID = -1, lastSymbolID = -1, lastCursorID = -1, lastBlobID = -1;
	float lastX = -1, lastY = -1, lastAngle = -1;
	int lastState = -1;

	void addTuioObject(TuioObject *tobj) {
		addObj++; lastSessionID = tobj->getSessionID(); lastSymbolID = tobj->getSymbolID();
		lastX = tobj->getX(); lastY = tobj->getY(); lastAngle = tobj->getAngle();
		lastState = tobj->getTuioState();
	}
	void updateTuioObject(TuioObject *tobj) { updObj++; lastSessionID = tobj->getSessionID(); lastX = tobj->getX(); }
	void removeTuioObject(TuioObject *tobj) { delObj++; lastSessionID = tobj->getSessionID(); lastState = tobj->getTuioState(); }

	void addTuioCursor(TuioCursor *tcur) {
		addCur++; lastSessionID = tcur->getSessionID(); lastCursorID = tcur->getCursorID();
		lastX = tcur->getX(); lastY = tcur->getY();
	}
	void updateTuioCursor(TuioCursor *tcur) { updCur++; lastSessionID = tcur->getSessionID(); lastX = tcur->getX(); }
	void removeTuioCursor(TuioCursor *tcur) { delCur++; lastSessionID = tcur->getSessionID(); lastCursorID = tcur->getCursorID(); lastState = tcur->getTuioState(); }

	void addTuioBlob(TuioBlob *tblb) {
		addBlb++; lastSessionID = tblb->getSessionID(); lastBlobID = tblb->getBlobID();
		lastX = tblb->getX(); lastY = tblb->getY();
	}
	void updateTuioBlob(TuioBlob *tblb) { updBlb++; lastSessionID = tblb->getSessionID(); lastX = tblb->getX(); }
	void removeTuioBlob(TuioBlob *tblb) { delBlb++; lastSessionID = tblb->getSessionID(); }

	void refresh(TuioTime) { refreshCount++; }
};

// captures the serialized OSC data instead of sending it
class CaptureSender : public OscSender {
public:
	std::vector<char> lastPacket;
	int packets = 0;

	CaptureSender() { buffer_size = 8192; local = true; }
	bool sendOscPacket(osc::OutboundPacketStream *bundle) {
		lastPacket.assign(bundle->Data(), bundle->Data()+bundle->Size());
		packets++;
		return true;
	}
	bool isConnected() { return true; }
	const char* tuio_type() { return "TEST"; }
};

// minimal OscReceiver that allows feeding packets directly
class StubReceiver : public OscReceiver {
public:
	void connect(bool lock=false) { (void)lock; connected = true; }
	void disconnect() { connected = false; }
};

// exposes the protected TuioDispatcher lists for direct population
class TestDispatcher : public TuioDispatcher {
public:
	~TestDispatcher() {
		for (std::list<TuioObject*>::iterator i=objectList.begin(); i!=objectList.end(); i++) delete *i;
		for (std::list<TuioCursor*>::iterator i=cursorList.begin(); i!=cursorList.end(); i++) delete *i;
		for (std::list<TuioBlob*>::iterator i=blobList.begin(); i!=blobList.end(); i++) delete *i;
	}
	int listenerCount() { return (int)listenerList.size(); }
	void injectObject(TuioObject *tobj) { lockObjectList(); objectList.push_back(tobj); unlockObjectList(); }
	void injectCursor(TuioCursor *tcur) { lockCursorList(); cursorList.push_back(tcur); unlockCursorList(); }
	void injectBlob(TuioBlob *tblb)     { lockBlobList();  blobList.push_back(tblb);   unlockBlobList(); }
};

#endif /* INCLUDED_TEST_HELPERS_H */

#include "test_helpers.h"
#include "TuioManager.h"
using namespace TUIO;

TEST(manager_add_update_remove_object) {
	TuioManager manager;
	RecordingListener listener;
	manager.addTuioListener(&listener);

	TuioObject *tobj = manager.addTuioObject(5, 0.1f, 0.2f, 0.3f);
	CHECK(tobj != NULL);
	CHECK_EQ(tobj->getTuioState(), TUIO_ADDED);
	CHECK_EQ(tobj->getSymbolID(), 5);
	CHECK_EQ(listener.addObj, 1);
	CHECK_EQ(listener.lastSymbolID, 5);
	CHECK_EQ(tobj->getSessionID(), 0); // sessionID starts at -1

	// a new frame is needed so the update has a time delta and reports moving
	manager.initFrame(TuioTime::getSystemTime() + 500000);
	manager.updateTuioObject(tobj, 0.4f, 0.5f, 0.6f);
	CHECK_EQ(listener.updObj, 1);

	manager.removeTuioObject(tobj);
	CHECK_EQ(listener.delObj, 1);
	CHECK_EQ(listener.lastState, TUIO_REMOVED);
	CHECK((int)manager.getTuioObjects().size() == 0);

	manager.removeTuioListener(&listener);
}

TEST(manager_cursor_blob_add_remove) {
	TuioManager manager;
	RecordingListener listener;
	manager.addTuioListener(&listener);

	TuioCursor *tcur = manager.addTuioCursor(0.3f, 0.4f);
	TuioBlob *tblb = manager.addTuioBlob(0.5f, 0.6f, 0.7f, 0.1f, 0.1f, 0.01f);
	CHECK_EQ(listener.addCur, 1);
	CHECK_EQ(listener.addBlb, 1);
	CHECK_EQ(listener.lastCursorID, 0);
	CHECK_EQ(listener.lastBlobID, 0);
	CHECK_EQ(tcur->getSessionID(), 0);
	CHECK_EQ(tblb->getSessionID(), 1); // shared sessionID counter

	manager.initFrame(TuioTime::getSystemTime() + 500000);
	manager.updateTuioCursor(tcur, 0.5f, 0.6f);
	manager.updateTuioBlob(tblb, 0.7f, 0.6f, 0.8f, 0.2f, 0.2f, 0.04f);
	CHECK_EQ(listener.updCur, 1);
	CHECK_EQ(listener.updBlb, 1);

	manager.removeTuioCursor(tcur);
	manager.removeTuioBlob(tblb);
	CHECK_EQ(listener.delCur, 1);
	CHECK_EQ(listener.delBlb, 1);

	manager.removeAllTuioListeners();
}

TEST(manager_cursor_id_reuse) {
	TuioManager manager;
	TuioCursor *c1 = manager.addTuioCursor(0.1f, 0.1f);
	CHECK_EQ(c1->getCursorID(), 0);
	TuioCursor *c2 = manager.addTuioCursor(0.9f, 0.9f);
	CHECK_EQ(c2->getCursorID(), 1);

	// removing the max-ID cursor frees the ID for reuse
	manager.removeTuioCursor(c1);
	TuioCursor *c3 = manager.addTuioCursor(0.2f, 0.2f);
	CHECK_EQ(c3->getCursorID(), 0); // reused freed ID (nearest free position)
}

TEST(manager_external_containers) {
	TuioManager manager;
	RecordingListener listener;
	manager.addTuioListener(&listener);

	// external cursor gets session + cursor ID assigned by the manager
	TuioCursor *ext = new TuioCursor(TuioTime(0,0), 999, -1, 0.5f, 0.5f);
	manager.addExternalTuioCursor(ext);
	CHECK_EQ(listener.addCur, 1);
	CHECK(ext->getCursorID() >= 0);
	CHECK_EQ(ext->getSessionID(), 0);

	// external removal does not delete the container
	manager.removeExternalTuioCursor(ext);
	CHECK_EQ(listener.delCur, 1);
	delete ext;

	TuioBlob *extb = new TuioBlob(TuioTime(0,0), 999, -1, 0.5f, 0.5f, 0.0f, 0.1f, 0.1f, 0.01f);
	manager.addExternalTuioBlob(extb);
	CHECK(extb->getBlobID() >= 0);
	manager.removeExternalTuioBlob(extb);
	delete extb;

	manager.removeAllTuioListeners();
}

TEST(manager_untouched_helpers) {
	TuioManager manager;
	TuioTime t0 = TuioTime::getSystemTime();

	manager.initFrame(t0 + 500000);
	TuioObject *tobj = manager.addTuioObject(1, 0.5f, 0.5f, 0.0f);
	manager.commitFrame();

	// new frame without touching the object -> untouched
	manager.initFrame(t0 + 1000000);
	std::list<TuioObject*> untouched = manager.getUntouchedObjects();
	CHECK_EQ((int)untouched.size(), 1);

	// update marks it touched for this frame
	manager.updateTuioObject(tobj, 0.6f, 0.5f, 0.0f);
	CHECK_EQ((int)manager.getUntouchedObjects().size(), 0);

	// untouched moving objects can be stopped and removed — each stage
	// needs its own frame since stop() marks the object as touched
	manager.initFrame(t0 + 1500000);
	manager.stopUntouchedMovingObjects();          // decelerating
	manager.initFrame(t0 + 2000000);
	manager.stopUntouchedMovingObjects();          // stopped
	manager.initFrame(t0 + 2500000);
	manager.removeUntouchedStoppedObjects();
	CHECK_EQ((int)manager.getTuioObjects().size(), 0);
}

TEST(manager_closest_inversion_verbose) {
	TuioManager manager;
	manager.addTuioCursor(0.2f, 0.2f);
	manager.addTuioCursor(0.8f, 0.8f);

	TuioCursor *closest = manager.getClosestTuioCursor(0.25f, 0.25f);
	CHECK(closest != NULL);
	CHECK_NEAR(closest->getX(), 0.2, 1e-6);

	manager.resetTuioCursors();
	CHECK_EQ((int)manager.getTuioCursors().size(), 0);

	// the invert flags are stored but never applied to the coordinates
	manager.setInversion(true, true, false);
	TuioCursor *inv = manager.addTuioCursor(0.3f, 0.4f);
	CHECK_NEAR(inv->getX(), 0.3, 1e-6); // documents the no-op
	CHECK_NEAR(inv->getY(), 0.4, 1e-6);
	manager.setInversion(false, false, false);

	manager.setVerbose(true);
	CHECK(manager.isVerbose());
	manager.setVerbose(false);
}

TEST(manager_frame_session_ids) {
	TuioManager manager;
	manager.initFrame(TuioTime(10,0));
	CHECK_EQ(manager.getFrameID(), 0);
	manager.commitFrame();
	manager.initFrame(TuioTime(11,0));
	CHECK_EQ(manager.getFrameID(), 1);

	// the session ID advances with each added container
	int s0 = manager.getSessionID();
	manager.addTuioObject(1, 0.5f, 0.5f, 0.0f);
	CHECK(manager.getSessionID() > s0);
}

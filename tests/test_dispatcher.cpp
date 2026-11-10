#include "test_helpers.h"
#include "TuioDispatcher.h"
using namespace TUIO;

TEST(dispatcher_listeners) {
	TestDispatcher disp;
	RecordingListener l1, l2;
	disp.addTuioListener(&l1);
	disp.addTuioListener(&l2);
	CHECK_EQ(disp.listenerCount(), 2);

	disp.removeTuioListener(&l1);
	CHECK_EQ(disp.listenerCount(), 1);
	disp.removeAllTuioListeners();
	CHECK_EQ(disp.listenerCount(), 0);
}

TEST(dispatcher_getters_counts) {
	TestDispatcher disp;
	disp.injectObject(new TuioObject(TuioTime(0,0), 11, 5, 0.1f, 0.2f, 0.3f));
	disp.injectObject(new TuioObject(TuioTime(0,0), 12, 6, 0.3f, 0.4f, 0.5f));
	disp.injectCursor(new TuioCursor(TuioTime(0,0), 21, 0, 0.5f, 0.5f));
	disp.injectBlob(new TuioBlob(TuioTime(0,0), 31, 0, 0.5f, 0.5f, 0.0f, 0.1f, 0.1f, 0.01f));

	CHECK_EQ(disp.getTuioObjectCount(), 2);
	CHECK_EQ(disp.getTuioCursorCount(), 1);
	CHECK_EQ(disp.getTuioBlobCount(), 1);
	CHECK_EQ((int)disp.getTuioObjects().size(), 2);
	CHECK_EQ((int)disp.copyTuioObjects().size(), 2);
	CHECK_EQ((int)disp.copyTuioCursors().size(), 1);
	CHECK_EQ((int)disp.copyTuioBlobs().size(), 1);

	TuioObject *found = disp.getTuioObject(12);
	CHECK(found != NULL);
	CHECK_EQ(found->getSymbolID(), 6);
	CHECK(disp.getTuioObject(99) == NULL);
	CHECK(disp.getTuioCursor(21) != NULL);
	CHECK(disp.getTuioBlob(31) != NULL);

	// copies are independent values
	std::list<TuioObject> copies = disp.copyTuioObjects();
	copies.front().setSymbolID(999);
	CHECK_EQ(disp.getTuioObject(12)->getSymbolID(), 6);
}

TEST(dispatcher_lock_unlock) {
	TestDispatcher disp;
	// balanced lock/unlock pairs must not deadlock
	disp.lockObjectList();
	disp.unlockObjectList();
	disp.lockCursorList();
	disp.unlockCursorList();
	disp.lockBlobList();
	disp.unlockBlobList();
	CHECK(true);
}

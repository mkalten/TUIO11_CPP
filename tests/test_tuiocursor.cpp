#include "test_util.h"
#include "TuioCursor.h"
using namespace TUIO;

TEST(tuiocursor_id) {
	TuioCursor tcur(TuioTime(0,0), 5, 3, 0.2f, 0.4f);
	CHECK_EQ(tcur.getCursorID(), 3);
	tcur.setCursorID(9);
	CHECK_EQ(tcur.getCursorID(), 9);
}

TEST(tuiocursor_update) {
	TuioCursor tcur(TuioTime(0,0), 5, 0, 0.0f, 0.0f);
	tcur.update(TuioTime(1,0), 0.6f, 0.8f);
	CHECK_NEAR(tcur.getX(), 0.6, 1e-6);
	CHECK_NEAR(tcur.getY(), 0.8, 1e-6);
	CHECK_NEAR(tcur.getMotionSpeed(), 1.0, 1e-4); // 0.6^2+0.8^2 = 1.0 over 1s
	CHECK_EQ(tcur.getTuioState(), TUIO_ACCELERATING);
}

TEST(tuiocursor_copy) {
	TuioCursor tcur(TuioTime(0,0), 5, 1, 0.1f, 0.2f);
	tcur.update(TuioTime(1,0), 0.3f, 0.4f);

	TuioCursor copy(&tcur); // pointer ctor: state reset to ADDED, new time
	CHECK_EQ(copy.getSessionID(), 5);
	CHECK_EQ(copy.getTuioState(), TUIO_ADDED);
	CHECK_NEAR(copy.getX(), 0.3, 1e-6);

	TuioCursor ref(tcur); // copy ctor preserves state
	CHECK_EQ(ref.getTuioState(), tcur.getTuioState());
	CHECK_EQ((int)ref.getPath().size(), (int)tcur.getPath().size());
}

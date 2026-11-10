#include "test_util.h"
#include "TuioObject.h"
#include "TuioCursor.h"
#include "TuioBlob.h"
using namespace TUIO;

// shared TuioContainer behavior, exercised through each subclass

TEST(container_initial_state) {
	TuioTime tt(1, 0);
	TuioObject tobj(tt, 42, 7, 0.1f, 0.2f, 0.5f);
	TuioCursor tcur(tt, 43, 0, 0.3f, 0.4f);
	TuioBlob tblb(tt, 44, 1, 0.5f, 0.6f, 0.2f, 0.1f, 0.1f, 0.01f);

	CHECK_EQ(tobj.getSessionID(), 42);
	CHECK_EQ(tcur.getSessionID(), 43);
	CHECK_EQ(tblb.getSessionID(), 44);
	CHECK_EQ(tobj.getTuioState(), TUIO_ADDED);
	CHECK_EQ(tcur.getTuioState(), TUIO_ADDED);
	CHECK_EQ(tblb.getTuioState(), TUIO_ADDED);
	CHECK(tobj.getTuioTime()==tt);

	// ctor seeds the path with the initial position
	CHECK_EQ((int)tobj.getPath().size(), 1);
	CHECK_EQ((int)tcur.getPath().size(), 1);
	CHECK_EQ((int)tblb.getPath().size(), 1);
}

TEST(container_update_speeds) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	// move 0.4 units in x over 1 second -> x_speed 0.4, accelerating
	tobj.update(TuioTime(1,0), 0.4f, 0.0f, 0.0f);
	CHECK_NEAR(tobj.getXSpeed(), 0.4, 1e-5);
	CHECK_NEAR(tobj.getYSpeed(), 0.0, 1e-6);
	CHECK_NEAR(tobj.getMotionSpeed(), 0.4, 1e-5);
	CHECK(tobj.getMotionAccel() > 0);
	CHECK_EQ(tobj.getTuioState(), TUIO_ACCELERATING);
	CHECK(tobj.isMoving());
	CHECK_EQ((int)tobj.getPath().size(), 2);

	// slowing to a stop first reports decelerating ...
	tobj.update(TuioTime(2,0), 0.5f, 0.0f, 0.0f);
	CHECK_NEAR(tobj.getXSpeed(), 0.1, 1e-5);
	CHECK(tobj.getMotionAccel() < 0);
	CHECK_EQ(tobj.getTuioState(), TUIO_DECELERATING);

	// ... one further motionless update is required before it stops
	tobj.update(TuioTime(3,0), 0.5f, 0.0f, 0.0f);
	CHECK_EQ(tobj.getTuioState(), TUIO_DECELERATING);
	tobj.update(TuioTime(4,0), 0.5f, 0.0f, 0.0f);
	CHECK_EQ(tobj.getTuioState(), TUIO_STOPPED);
	CHECK(!tobj.isMoving());
}

TEST(container_update_dt0_no_nan) {
	TuioCursor tcur(TuioTime(1,0), 1, 0, 0.0f, 0.0f);
	// same timestamp -> dt==0 must not produce NaN speeds
	tcur.update(TuioTime(1,0), 0.5f, 0.5f);
	CHECK(std::isfinite(tcur.getXSpeed()));
	CHECK(std::isfinite(tcur.getMotionSpeed()));
	CHECK(std::isfinite(tcur.getMotionAccel()));
}

TEST(container_update_explicit_speeds) {
	TuioBlob tblb(TuioTime(0,0), 1, 0, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.01f);
	tblb.update(TuioTime(1,0), 0.3f, 0.4f, 0.0f, 0.1f, 0.1f, 0.01f, 0.6f, 0.8f, 0.0f, 0.5f, 0.0f);
	CHECK_NEAR(tblb.getXSpeed(), 0.6, 1e-6);
	CHECK_NEAR(tblb.getYSpeed(), 0.8, 1e-6);
	CHECK_NEAR(tblb.getMotionSpeed(), 1.0, 1e-5);
	CHECK_NEAR(tblb.getMotionAccel(), 0.5, 1e-6);
	CHECK_EQ(tblb.getTuioState(), TUIO_ACCELERATING);
}

TEST(container_stop_remove) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	tobj.update(TuioTime(1,0), 0.5f, 0.0f, 0.0f);
	CHECK(tobj.isMoving());
	tobj.stop(TuioTime(2,0)); // performs a zero-motion update -> decelerating
	CHECK_EQ(tobj.getTuioState(), TUIO_DECELERATING);
	tobj.update(TuioTime(3,0), 0.5f, 0.0f, 0.0f);
	CHECK_EQ(tobj.getTuioState(), TUIO_STOPPED);
	tobj.remove(TuioTime(4,0));
	CHECK_EQ(tobj.getTuioState(), TUIO_REMOVED);
	CHECK(tobj.getTuioTime()==TuioTime(4,0));
}

TEST(container_source) {
	TuioCursor tcur(1, 0, 0.0f, 0.0f);
	CHECK_EQ(tcur.getTuioSourceID(), 0);
	tcur.setTuioSource(3, "reacTIVision", "192.168.0.1");
	CHECK_EQ(tcur.getTuioSourceID(), 3);
	CHECK_EQ(std::string(tcur.getTuioSourceName()), "reacTIVision");
	CHECK_EQ(std::string(tcur.getTuioSourceAddress()), "192.168.0.1");
}

TEST(container_copy_lastpoint_repoint) {
	// regression: lastPoint must point into the copy's path, not the source's
	TuioCursor orig(TuioTime(0,0), 1, 0, 0.0f, 0.0f);
	orig.update(TuioTime(1,0), 0.2f, 0.0f);
	orig.update(TuioTime(2,0), 0.4f, 0.0f);
	CHECK_EQ((int)orig.getPath().size(), 3);

	TuioCursor copy(orig);
	CHECK_EQ((int)copy.getPath().size(), 3);
	// a subsequent update on the copy must stay consistent
	// (would dangle into the original's list if lastPoint were not repointed)
	copy.update(TuioTime(3,0), 0.6f, 0.0f);
	CHECK_NEAR(copy.getX(), 0.6, 1e-6);
	CHECK_EQ((int)copy.getPath().size(), 4);
	CHECK(std::isfinite(copy.getXSpeed()));
	// original untouched
	CHECK_EQ((int)orig.getPath().size(), 3);

	// same via assignment
	TuioCursor asg(9, 0, 0.0f, 0.0f);
	asg = orig;
	asg.update(TuioTime(3,0), 0.8f, 0.0f);
	CHECK_NEAR(asg.getX(), 0.8, 1e-6);
	CHECK(std::isfinite(asg.getXSpeed()));
}

TEST(container_predict_position) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	tobj.update(TuioTime(1,0), 0.5f, 0.0f, 0.0f); // x_speed = 0.5/s, x_accel = 0.5/s^2
	TuioPoint pred = tobj.predictPosition();
	// x + v*dt + 0.5*a*dt^2 = 0.5 + 0.5*1 + 0.5*0.5*1 = 1.25
	CHECK_NEAR(pred.getX(), 1.25, 1e-5);
	CHECK_NEAR(pred.getY(), 0.0, 1e-6);
}

TEST(container_path_capped) {
	TuioCursor tcur(TuioTime(0,0), 1, 0, 0.0f, 0.0f);
	for (int i = 1; i <= 260; i++)
		tcur.update(TuioTime(i,0), (float)i/1000.0f, 0.0f);
	CHECK((int)tcur.getPath().size() <= 256); // MAX_PATH_SIZE
}

#include "test_util.h"
#include "TuioObject.h"
using namespace TUIO;

TEST(tuioobject_symbol_angle) {
	TuioObject tobj(TuioTime(0,0), 1, 42, 0.5f, 0.5f, (float)M_PI/2);
	CHECK_EQ(tobj.getSymbolID(), 42);
	CHECK_NEAR(tobj.getAngle(), M_PI/2, 1e-5);
	CHECK_NEAR(tobj.getAngleDegrees(), 90.0, 1e-3);

	tobj.setSymbolID(7);
	CHECK_EQ(tobj.getSymbolID(), 7);
}

TEST(tuioobject_rotation) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	// rotate by pi/2 over 1 second
	tobj.update(TuioTime(1,0), 0.0f, 0.0f, (float)M_PI/2);
	CHECK_NEAR(tobj.getAngle(), M_PI/2, 1e-4);
	// rotation_speed is in turns per second: 0.25 turns/s
	CHECK_NEAR(tobj.getRotationSpeed(), 0.25, 1e-4);
	CHECK(tobj.getRotationAccel() != 0.0f);
	CHECK(tobj.isMoving()); // TUIO_ROTATING counts as moving
}

TEST(tuioobject_rotation_wrap) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.1f);
	// rotate past the 2pi boundary wraps into [0,2pi)
	tobj.update(TuioTime(1,0), 0.0f, 0.0f, (float)(2*M_PI + 0.2));
	CHECK(tobj.getAngle() >= 0.0f && tobj.getAngle() <= 2*M_PI);
}

TEST(tuioobject_angle_threshold) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	tobj.addAngleThreshold(0.1f);
	// change below the threshold is suppressed
	tobj.update(TuioTime(1,0), 0.0f, 0.0f, 0.05f);
	CHECK_NEAR(tobj.getAngle(), 0.0, 1e-6);
	// change above it is applied
	tobj.update(TuioTime(2,0), 0.0f, 0.0f, 0.5f);
	CHECK_NEAR(tobj.getAngle(), 0.5, 1e-5);
	tobj.removeAngleThreshold();
}

TEST(tuioobject_angle_filter_copy) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	tobj.addAngleFilter(1.0f, 0.1f);
	tobj.update(TuioTime(1,0), 0.0f, 0.0f, 0.4f);

	// deep copy: independent filter state
	TuioObject copy(tobj);
	copy.update(TuioTime(2,0), 0.0f, 0.0f, 3.0f);
	tobj.update(TuioTime(2,0), 0.0f, 0.0f, 0.5f);
	CHECK(std::isfinite(tobj.getAngle()));
	CHECK(std::isfinite(copy.getAngle()));
	CHECK(tobj.getAngle() != copy.getAngle());

	tobj.removeAngleFilter();
	tobj.update(TuioTime(3,0), 0.0f, 0.0f, 1.0f);
	CHECK_NEAR(tobj.getAngle(), 1.0, 1e-5);
}

TEST(tuioobject_update_full) {
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	tobj.update(0.2f, 0.3f, 0.4f, 1.0f, 0.0f, 0.25f, 0.5f, 0.1f);
	CHECK_NEAR(tobj.getX(), 0.2, 1e-6);
	CHECK_NEAR(tobj.getAngle(), 0.4, 1e-6);
	CHECK_NEAR(tobj.getXSpeed(), 1.0, 1e-6);
	CHECK_NEAR(tobj.getRotationSpeed(), 0.25, 1e-6);
}

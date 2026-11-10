#include "test_util.h"
#include "TuioBlob.h"
using namespace TUIO;

TEST(tuioblob_fields) {
	TuioBlob tblb(TuioTime(0,0), 7, 4, 0.5f, 0.6f, 0.3f, 0.2f, 0.1f, 0.02f);
	CHECK_EQ(tblb.getSessionID(), 7);
	CHECK_EQ(tblb.getBlobID(), 4);
	CHECK_NEAR(tblb.getWidth(), 0.2, 1e-6);
	CHECK_NEAR(tblb.getHeight(), 0.1, 1e-6);
	CHECK_NEAR(tblb.getArea(), 0.02, 1e-6);
	CHECK_NEAR(tblb.getAngle(), 0.3, 1e-6);

	tblb.setBlobID(11);
	CHECK_EQ(tblb.getBlobID(), 11);
	CHECK_EQ(tblb.getScreenWidth(1000), 200);
	CHECK_EQ(tblb.getScreenHeight(1000), 100);
}

TEST(tuioblob_update) {
	TuioBlob tblb(TuioTime(0,0), 1, 0, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.01f);
	tblb.update(TuioTime(1,0), 0.3f, 0.4f, 0.5f, 0.2f, 0.2f, 0.04f);
	CHECK_NEAR(tblb.getX(), 0.3, 1e-6);
	CHECK_NEAR(tblb.getAngle(), 0.5, 1e-4);
	CHECK_NEAR(tblb.getWidth(), 0.2, 1e-6);
	CHECK_NEAR(tblb.getArea(), 0.04, 1e-6);
	CHECK(tblb.getRotationSpeed() != 0.0f);
}

TEST(tuioblob_update_full) {
	TuioBlob tblb(TuioTime(0,0), 1, 0, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.01f);
	tblb.update(0.2f, 0.3f, 0.4f, 0.15f, 0.15f, 0.0225f, 1.0f, 0.5f, 0.25f, 0.5f, 0.1f);
	CHECK_NEAR(tblb.getXSpeed(), 1.0, 1e-6);
	CHECK_NEAR(tblb.getRotationSpeed(), 0.25, 1e-6);
	CHECK_NEAR(tblb.getWidth(), 0.15, 1e-6);
}

TEST(tuioblob_thresholds_filters) {
	TuioBlob tblb(TuioTime(0,0), 1, 0, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.01f);

	tblb.addAngleThreshold(0.2f);
	tblb.addSizeThreshold(0.2f);
	tblb.update(TuioTime(1,0), 0.0f, 0.0f, 0.1f, 0.12f, 0.1f, 0.01f);
	CHECK_NEAR(tblb.getAngle(), 0.0, 1e-6);   // below angle threshold
	CHECK_NEAR(tblb.getWidth(), 0.1, 1e-6);   // below size threshold
	tblb.removeAngleThreshold();
	tblb.removeSizeThreshold();

	tblb.addAngleFilter(1.0f, 0.1f);
	tblb.addSizeFilter(1.0f, 0.1f);
	tblb.update(TuioTime(2,0), 0.0f, 0.0f, 1.0f, 0.3f, 0.3f, 0.09f);
	CHECK(std::isfinite(tblb.getAngle()));
	CHECK(std::isfinite(tblb.getWidth()));

	// deep copy keeps independent filters
	TuioBlob copy(tblb);
	copy.update(TuioTime(3,0), 0.0f, 0.0f, 5.0f, 0.9f, 0.9f, 0.8f);
	tblb.update(TuioTime(3,0), 0.0f, 0.0f, 0.6f, 0.4f, 0.4f, 0.16f);
	CHECK(tblb.getAngle() != copy.getAngle());
	CHECK(std::isfinite(tblb.getWidth()));

	tblb.removeAngleFilter();
	tblb.removeSizeFilter();
}

TEST(tuioblob_copy) {
	TuioBlob tblb(TuioTime(0,0), 2, 3, 0.4f, 0.5f, 0.6f, 0.1f, 0.2f, 0.02f);
	tblb.update(TuioTime(1,0), 0.5f, 0.5f, 0.7f, 0.2f, 0.2f, 0.04f);
	TuioBlob copy(tblb);
	CHECK_EQ(copy.getBlobID(), 3);
	CHECK_NEAR(copy.getAngle(), tblb.getAngle(), 1e-6);
	CHECK_EQ((int)copy.getPath().size(), (int)tblb.getPath().size());
}

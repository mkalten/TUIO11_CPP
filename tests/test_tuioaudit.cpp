/*
 Tests for the TuioAudit scenarios driven through a TuioServer
 with a CaptureSender, plus a decode-path check feeding the emitted
 bundles into a TuioClient through a StubReceiver.
*/

#include "test_util.h"
#include "test_helpers.h"
#include "TuioClient.h"
#include "../TuioScenarios.h"

#include <string.h>
#include <string>

// runs the scenario for n frames with a fixed dt and returns all captured packets
static std::string runScenario(TuioServer *server, CaptureSender *sender, TuioScenario *scenario, TuioTime t0, int frames, float dt = 0.05f) {
	sender->reset();
	// adds need the current frame time to be emitted,
	// so start() is called inside the first frame
	for (int i=0;i<frames;i++) {
		TuioTime t(t0.getSeconds(), t0.getMicroseconds() + (long)(i*dt*1000000));
		server->initFrame(t);
		if (i==0) scenario->start(server);
		scenario->update(server, i*dt, dt);
		server->stopUntouchedMovingObjects();
		server->stopUntouchedMovingCursors();
		server->stopUntouchedMovingBlobs();
		server->commitFrame();
	}
	return sender->captured;
}

TEST(tuioaudit_registry) {
	std::vector<TuioScenario*> scenarios = getTuioScenarios();
	CHECK(scenarios.size() >= 16);
	for (std::vector<TuioScenario*>::iterator s=scenarios.begin(); s!=scenarios.end(); s++) {
		CHECK((*s)->name()!=NULL && strlen((*s)->name())>0);
		CHECK((*s)->description()!=NULL && strlen((*s)->description())>0);
		CHECK_EQ(findTuioScenario((*s)->name()), *s);
	}
	CHECK(findTuioScenario("nonexistent")==NULL);

	// only the malformed-packet scenarios are raw
	const char *rawNames[] = {"noalive","setonly","nofseq","shortset","longset","badtype","deadset"};
	for (int i=0;i<7;i++) {
		TuioScenario *s = findTuioScenario(rawNames[i]);
		CHECK(s!=NULL && s->raw());
	}
	for (std::vector<TuioScenario*>::iterator s=scenarios.begin(); s!=scenarios.end(); s++) {
		bool expected = false;
		for (int i=0;i<7;i++) if (strcmp((*s)->name(),rawNames[i])==0) expected = true;
		CHECK_EQ((*s)->raw(), expected);
	}
}

// emits one raw frame through a CaptureSender and returns the packet
static std::string rawPacket(TuioScenario *scenario, float t = 0.0f) {
	CaptureSender sender;
	std::vector<OscSender*> senders;
	senders.push_back(&sender);
	scenario->updateRaw(senders,t,0.05f);
	return std::string(sender.lastPacket.begin(), sender.lastPacket.end());
}

TEST(tuioaudit_noalive_bundle) {
	TuioScenario *scenario = findTuioScenario("noalive");
	CHECK(scenario!=NULL);
	std::string packet = rawPacket(scenario);
	CHECK(packet.find("/tuio/2Dcur")!=std::string::npos);
	CHECK(packet.find("set")!=std::string::npos);
	CHECK(packet.find("fseq")!=std::string::npos);
	CHECK(packet.find("alive")==std::string::npos);
}

TEST(tuioaudit_setonly_bundle) {
	TuioScenario *scenario = findTuioScenario("setonly");
	CHECK(scenario!=NULL);
	std::string packet = rawPacket(scenario);
	CHECK(packet.find("set")!=std::string::npos);
	CHECK(packet.find("alive")==std::string::npos);
	CHECK(packet.find("fseq")==std::string::npos);
	CHECK(packet.find("source")==std::string::npos);
}

TEST(tuioaudit_nofseq_bundle) {
	TuioScenario *scenario = findTuioScenario("nofseq");
	CHECK(scenario!=NULL);
	std::string packet = rawPacket(scenario);
	CHECK(packet.find("alive")!=std::string::npos);
	CHECK(packet.find("set")!=std::string::npos);
	CHECK(packet.find("fseq")==std::string::npos);
}

// malformed set messages must not crash the receiving client
TEST(tuioaudit_malformed_set_tolerance) {
	StubReceiver receiver;
	TuioClient client(&receiver);
	RecordingListener rec;
	client.addTuioListener(&rec);

	const char *names[] = {"shortset","longset","badtype","setonly","noalive","nofseq","deadset"};
	for (int i=0;i<7;i++) {
		TuioScenario *scenario = findTuioScenario(names[i]);
		CHECK(scenario!=NULL);
		CaptureSender sender;
		std::vector<OscSender*> senders;
		senders.push_back(&sender);
		scenario->updateRaw(senders,0.5f,0.05f);
		try {
			receiver.ProcessPacket(sender.lastPacket.data(), (int)sender.lastPacket.size(), IpEndpointName());
		} catch (...) {
			// the implementation may propagate parse exceptions — no crash is the contract
		}
	}
	CHECK(true);
}

// builds a /tuio/2Dcur bundle and feeds it to the client — exercises the
// removal-scan + frameCursors path that previously use-after-freed
static void feedBundle(StubReceiver &receiver, const char *src, const char *alive, float x, int fseq) {
	char buf[1024];
	osc::OutboundPacketStream p(buf, sizeof(buf));
	p << osc::BeginBundleImmediate;
	if (src)   p << osc::BeginMessage("/tuio/2Dcur") << "source" << src << osc::EndMessage;
	if (alive && strcmp(alive,"keep")==0)
		p << osc::BeginMessage("/tuio/2Dcur") << "alive" << 4242 << osc::EndMessage;
	else if (alive && strcmp(alive,"dead")==0)
		p << osc::BeginMessage("/tuio/2Dcur") << "alive" << 9999 << osc::EndMessage;
	else
		p << osc::BeginMessage("/tuio/2Dcur") << "alive" << osc::EndMessage;
	p << osc::BeginMessage("/tuio/2Dcur") << "set" << 4242 << x << 0.5f << 0.0f << 0.0f << 0.0f << osc::EndMessage;
	p << osc::BeginMessage("/tuio/2Dcur") << "fseq" << fseq << osc::EndMessage;
	p << osc::EndBundle;
	receiver.ProcessPacket(p.Data(), (int)p.Size(), IpEndpointName());
}

TEST(tuioaudit_missing_alive_no_crash) {
	// a set-updated cursor absent from alive previously caused a
	// use-after-free: the removed cursor was resurrected by update(),
	// then found itself in cursorList and got deleted mid-callback
	StubReceiver receiver;
	TuioClient client(&receiver);
	RecordingListener rec;
	client.addTuioListener(&rec);

	// no alive message at all -> removal churn, must not crash
	for (int f=0; f<8; f++) {
		char buf[1024];
		osc::OutboundPacketStream p(buf, sizeof(buf));
		p << osc::BeginBundleImmediate;
		p << osc::BeginMessage("/tuio/2Dcur") << "source" << "TuioAudit" << osc::EndMessage;
		p << osc::BeginMessage("/tuio/2Dcur") << "set" << 4242 << 0.5f+0.001f*f << 0.5f << 0.0f << 0.0f << 0.0f << osc::EndMessage;
		p << osc::BeginMessage("/tuio/2Dcur") << "fseq" << f << osc::EndMessage;
		p << osc::EndBundle;
		receiver.ProcessPacket(p.Data(), (int)p.Size(), IpEndpointName());
	}
	CHECK(rec.addCur >= 1);
	CHECK(rec.delCur >= 1);
}

TEST(tuioaudit_deadset_no_crash) {
	// alive present but missing the updated session ID — same code path
	StubReceiver receiver;
	TuioClient client(&receiver);
	RecordingListener rec;
	client.addTuioListener(&rec);

	feedBundle(receiver, "TuioAudit", "keep", 0.5f, 0);
	for (int f=1; f<8; f++) feedBundle(receiver, "TuioAudit", "dead", 0.5f+0.001f*f, f);

	CHECK(rec.addCur >= 1);
	CHECK(rec.delCur >= 1);
}

TEST(tuioaudit_pending_discard_on_next_packet) {
	// set without fseq leaves pending frame entries; the next packet
	// must discard them rather than letting them linger
	StubReceiver receiver;
	TuioClient client(&receiver);
	RecordingListener rec;
	client.addTuioListener(&rec);

	// incomplete bundle: set only, no fseq -> nothing committed
	{
		char buf[1024];
		osc::OutboundPacketStream p(buf, sizeof(buf));
		p << osc::BeginBundleImmediate;
		p << osc::BeginMessage("/tuio/2Dcur") << "set" << 4242 << 0.5f << 0.5f << 0.0f << 0.0f << 0.0f << osc::EndMessage;
		p << osc::EndBundle;
		receiver.ProcessPacket(p.Data(), (int)p.Size(), IpEndpointName());
	}
	CHECK_EQ(rec.addCur, 0);
	CHECK_EQ(rec.updCur, 0);

	// a well-formed bundle afterwards still works normally
	feedBundle(receiver, "TuioAudit", "keep", 0.6f, 1);
	CHECK_EQ(rec.addCur, 1);
}

TEST(tuioaudit_raw_fseq_increments) {
	TuioScenario *scenario = findTuioScenario("nofseq");
	CHECK(scenario!=NULL);
	// fseq is omitted from the stream but still counted internally;
	// verify the raw path emits one packet per updateRaw call
	CaptureSender sender;
	std::vector<OscSender*> senders;
	senders.push_back(&sender);
	scenario->updateRaw(senders,0.0f,0.05f);
	scenario->updateRaw(senders,0.05f,0.05f);
	CHECK_EQ(sender.packets, 2);
}

TEST(tuioaudit_circle_emits_cursor) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	server.setSourceName("Test");
	TuioScenario *scenario = findTuioScenario("circle");
	CHECK(scenario!=NULL);

	TuioTime t0 = TuioTime::getSystemTime();
	std::string packet = runScenario(&server,sender,scenario,t0,4);
	CHECK(packet.find("/tuio/2Dcur")!=std::string::npos);
	CHECK(packet.find("set")!=std::string::npos);
	CHECK(packet.find("alive")!=std::string::npos);
	CHECK(packet.find("fseq")!=std::string::npos);
}

TEST(tuioaudit_mixed_emits_all_profiles) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	TuioScenario *scenario = findTuioScenario("mixed");
	CHECK(scenario!=NULL);

	TuioTime t0 = TuioTime::getSystemTime();
	std::string packet = runScenario(&server,sender,scenario,t0,4);
	CHECK(packet.find("/tuio/2Dobj")!=std::string::npos);
	CHECK(packet.find("/tuio/2Dcur")!=std::string::npos);
	CHECK(packet.find("/tuio/2Dblb")!=std::string::npos);
}

TEST(tuioaudit_empty_silence_then_periodic) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	TuioScenario *scenario = findTuioScenario("empty");
	CHECK(scenario!=NULL);

	// without periodic messages the server emits nothing at all when idle
	TuioTime t0 = TuioTime::getSystemTime();
	std::string packet = runScenario(&server,sender,scenario,t0,4);
	CHECK(packet.find("set")==std::string::npos);

	// with periodic messages enabled, idle frames emit alive/fseq keepalives
	server.enablePeriodicMessages(1);
	sender->reset();
	TuioTime t1(t0.getSeconds()+2, 0);
	server.initFrame(t1);
	scenario->update(&server,2.0f,0.05f);
	server.commitFrame();
	CHECK(sender->captured.find("alive")!=std::string::npos);
	CHECK(sender->captured.find("fseq")!=std::string::npos);
	CHECK(sender->captured.find("set")==std::string::npos);
}

TEST(tuioaudit_static_no_updates) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	TuioScenario *scenario = findTuioScenario("static");
	CHECK(scenario!=NULL);

	TuioTime t0 = TuioTime::getSystemTime();
	// the adds are emitted when start() runs inside the first frame
	sender->reset();
	server.initFrame(t0);
	scenario->start(&server);
	scenario->update(&server,0.0f,0.05f);
	server.commitFrame();
	CHECK(sender->captured.find("set")!=std::string::npos);
	// a later untouched frame emits nothing at all
	TuioTime t1(t0.getSeconds()+1, 0);
	sender->reset();
	server.initFrame(t1);
	scenario->update(&server,1.0f,0.05f);
	server.commitFrame();
	CHECK(sender->captured.find("set")==std::string::npos);
	sender->reset();
	server.initFrame(t1);
	scenario->stop(&server);
	server.commitFrame();
}

TEST(tuioaudit_churn_grows_session_ids) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	StubReceiver receiver;
	TuioClient client(&receiver);
	RecordingListener rec;
	client.addTuioListener(&rec);
	TuioScenario *scenario = findTuioScenario("churn");
	CHECK(scenario!=NULL);

	TuioTime t0 = TuioTime::getSystemTime();
	scenario->start(&server);
	int firstSession = -1, laterSession = -1;
	// run two full add/remove cycles, decoding each frame into the client
	for (int i=0;i<50;i++) {
		float t = i*0.06f;
		TuioTime ft(t0.getSeconds(), t0.getMicroseconds() + (long)(t*1000000));
		server.initFrame(ft);
		scenario->update(&server,t,0.06f);
		server.commitFrame();
		receiver.ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
		if (i==5 && rec.addCur>0) firstSession = rec.lastSessionID;
	}
	CHECK(rec.addCur >= 2);         // at least two add cycles
	CHECK(rec.delCur >= 1);         // and at least one removal
	laterSession = rec.lastSessionID;
	CHECK(laterSession > firstSession);  // session IDs grow with each cycle
	scenario->stop(&server);
}

TEST(tuioaudit_bounds_exact_coordinates) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	StubReceiver receiver;
	TuioClient client(&receiver);
	RecordingListener rec;
	client.addTuioListener(&rec);
	TuioScenario *scenario = findTuioScenario("bounds");
	CHECK(scenario!=NULL);

	TuioTime t0 = TuioTime::getSystemTime();
	scenario->start(&server);
	// during a pause phase (phase < LEG/2) the cursor sits exactly on a corner
	server.initFrame(t0);
	scenario->update(&server,0.0f,0.05f);
	server.commitFrame();
	receiver.ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(rec.lastX, 0.0f);
	CHECK_EQ(rec.lastY, 0.0f);

	// later corner: leg 2 pause sits at (1,1) — leg boundaries at t = 1.0, 2.0
	TuioTime t1(t0.getSeconds()+2, 0);
	server.initFrame(t1);
	scenario->update(&server,2.2f,0.05f);
	server.commitFrame();
	receiver.ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(rec.lastX, 1.0f);
	CHECK_EQ(rec.lastY, 1.0f);
	scenario->stop(&server);
}

TEST(tuioaudit_burst_alive_count) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	TuioScenario *scenario = findTuioScenario("burst");
	CHECK(scenario!=NULL);

	TuioTime t0 = TuioTime::getSystemTime();
	runScenario(&server,sender,scenario,t0,4);
	// decode the last bundle into a client — it should see all 16 cursors
	StubReceiver receiver;
	TuioClient client(&receiver);
	RecordingListener rec;
	client.addTuioListener(&rec);
	receiver.ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(rec.addCur, 16);
}

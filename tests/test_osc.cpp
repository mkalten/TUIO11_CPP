#include "test_helpers.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "osc/OscOutboundPacketStream.h"
using namespace TUIO;
using namespace osc;

// OscSender base surface via concrete senders
TEST(oscsender_surface) {
	UdpSender udp("127.0.0.1", 13460);
	CHECK(udp.isConnected());
	CHECK((int)udp.getBufferSize() > 0);
	CHECK_EQ(std::string(udp.tuio_type()), "TUIO/UDP");

	TcpSender tcp(13461);
	CHECK_EQ(std::string(tcp.tuio_type()), "TUIO/TCP");

	CaptureSender cap;
	CHECK(cap.isLocal());
	CHECK_EQ(cap.getBufferSize(), 8192);
	CHECK(cap.isConnected());
}

// OscReceiver lifecycle on the stub
TEST(oscreceiver_lifecycle) {
	StubReceiver recv;
	CHECK(!recv.isConnected());
	recv.connect();
	CHECK(recv.isConnected());
	recv.disconnect();
	CHECK(!recv.isConnected());
}

// deterministic full path: TuioServer -> CaptureSender -> StubReceiver -> TuioClient
TEST(osc_server_to_client_inprocess) {
	CaptureSender *sender = new CaptureSender();
	TuioServer server(sender);
	server.setSourceName("testsrc","127.0.0.1");

	StubReceiver *stub = new StubReceiver();
	TuioClient client(stub); // attaches itself via addTuioClient
	RecordingListener listener;
	client.addTuioListener(&listener);

	server.addTuioCursor(0.4f, 0.6f);
	server.commitFrame();
	CHECK(sender->packets > 0);

	// feed the captured bundle to the receiver -> client decodes it
	stub->ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(listener.addCur, 1);
	CHECK_NEAR(listener.lastX, 0.4, 1e-5);
	CHECK_NEAR(listener.lastY, 0.6, 1e-5);
	CHECK_EQ((int)client.getTuioCursors().size(), 1);

	// update propagates
	server.updateTuioCursor(server.getTuioCursors().front(), 0.5f, 0.6f);
	server.commitFrame();
	stub->ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(listener.updCur, 1);
	CHECK_NEAR(listener.lastX, 0.5, 1e-5);

	// removal propagates
	server.removeTuioCursor(server.getTuioCursors().front());
	server.commitFrame();
	stub->ProcessPacket(sender->lastPacket.data(), (int)sender->lastPacket.size(), IpEndpointName());
	CHECK_EQ(listener.delCur, 1);
	CHECK_EQ((int)client.getTuioCursors().size(), 0);

	client.removeAllTuioListeners();
}

// hand-crafted flat TUIO bundle fed directly to the receiver
TEST(oscreceiver_manual_bundle) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, 4096);
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio/2Dcur") << "source" << "t@a" << EndMessage
		<< BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)0 << EndMessage
		<< BeginMessage("/tuio/2Dcur") << "set" << (int32_t)0 << 0.5f << 0.5f << 0.0f << 0.0f << 0.0f << EndMessage
		<< BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << EndMessage
	<< EndBundle;

	// the bundle decodes all messages: set creates the cursor, fseq fires it and refresh()
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size(), IpEndpointName());
	CHECK_EQ(listener.addCur, 1);
	CHECK_EQ(listener.refreshCount, 1);
	CHECK_EQ((int)client.getTuioCursors().size(), 1);
}

// malformed packets must not crash the receiver
TEST(oscreceiver_malformed) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char garbage[32];
	memset(garbage, 0xAB, sizeof(garbage));
	// NOTE: ProcessPacket only catches MalformedBundleException — a malformed
	// *message* propagates out as a different oscpack exception
	try {
		stub.ProcessPacket(garbage, sizeof(garbage), IpEndpointName());
	} catch (...) { /* documents current behavior */ }
	try {
		stub.ProcessPacket(garbage, 0, IpEndpointName());
	} catch (...) {}
	CHECK_EQ(listener.addCur + listener.addObj + listener.addBlb, 0);
}

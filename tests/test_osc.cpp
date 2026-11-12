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

// malformed packets must be caught and discarded, not propagated
TEST(oscreceiver_malformed) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char garbage[32];
	memset(garbage, 0xAB, sizeof(garbage));

	// message-level garbage, empty packet, and NULL data must not escape or crash
	stub.ProcessPacket(garbage, sizeof(garbage), IpEndpointName());
	stub.ProcessPacket(garbage, 0, IpEndpointName());
	stub.ProcessPacket(NULL, 8, IpEndpointName());

	// a truncated bundle (size not a multiple of four)
	char buffer[4096];
	OutboundPacketStream bundle(buffer, 4096);
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << EndMessage
	<< EndBundle;
	stub.ProcessPacket(bundle.Data(), (int)bundle.Size()-1, IpEndpointName());

	CHECK_EQ(listener.addCur + listener.addObj + listener.addBlb, 0);
}

// appends one size-prefixed big-endian element to a hand-built bundle
static char* appendElement(char *p, const char *data, int size) {
	p[0]=(char)((size>>24)&0xFF); p[1]=(char)((size>>16)&0xFF);
	p[2]=(char)((size>>8)&0xFF);  p[3]=(char)(size&0xFF);
	memcpy(p+4, data, size);
	return p+4+size;
}

// a malformed element must be skipped without aborting its valid siblings
TEST(oscreceiver_bad_element_continues) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char setbuf[256], fseqbuf[256];
	OutboundPacketStream set(setbuf, sizeof(setbuf));
	set << BeginMessage("/tuio/2Dcur") << "set" << (int32_t)1 << 0.5f << 0.5f << 0.0f << 0.0f << 0.0f << EndMessage;
	OutboundPacketStream fseq(fseqbuf, sizeof(fseqbuf));
	fseq << BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << EndMessage;

	// corrupt element: valid size field, content has no terminated address pattern
	char corrupt[8];
	memset(corrupt, 0xFF, sizeof(corrupt));

	char buf[1024], *p = buf;
	memcpy(p, "#bundle\0", 8); p += 8;
	memset(p, 0, 8); p += 8; // timetag
	p = appendElement(p, setbuf, (int)set.Size());
	p = appendElement(p, corrupt, (int)sizeof(corrupt));
	p = appendElement(p, fseqbuf, (int)fseq.Size());

	stub.ProcessPacket(buf, (int)(p-buf), IpEndpointName());
	// set and fseq on both sides of the corrupt element were still dispatched
	CHECK_EQ(listener.addCur, 1);
	CHECK_EQ(listener.refreshCount, 1);
}

// a bundle with a corrupt element size table is rejected as a whole
TEST(oscreceiver_corrupt_bundle_rejected) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	OutboundPacketStream bundle(buffer, 4096);
	bundle << BeginBundleImmediate
		<< BeginMessage("/tuio/2Dcur") << "set" << (int32_t)1 << 0.5f << 0.5f << 0.0f << 0.0f << 0.0f << EndMessage
		<< BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << EndMessage
	<< EndBundle;

	// first element size at offset 16: not a multiple of four -> bundle invalid
	buffer[19] = 7;
	stub.ProcessPacket(buffer, (int)bundle.Size(), IpEndpointName());
	CHECK_EQ(listener.addCur, 0);
	CHECK_EQ(listener.refreshCount, 0);

	// element size exceeding the bundle remainder is rejected the same way
	OutboundPacketStream bundle2(buffer, 4096);
	bundle2 << BeginBundleImmediate
		<< BeginMessage("/tuio/2Dcur") << "set" << (int32_t)1 << 0.5f << 0.5f << 0.0f << 0.0f << 0.0f << EndMessage
		<< BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << EndMessage
	<< EndBundle;
	buffer[16] = 0x0F; buffer[17] = 0xFF; buffer[18] = 0xFF; buffer[19] = 0xFC;
	stub.ProcessPacket(buffer, (int)bundle2.Size(), IpEndpointName());
	CHECK_EQ(listener.addCur, 0);
	CHECK_EQ(listener.refreshCount, 0);
}

// messages inside a nested bundle are dispatched like top-level messages
TEST(oscreceiver_nested_bundle) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char setbuf[256], fseqbuf[256];
	OutboundPacketStream set(setbuf, sizeof(setbuf));
	set << BeginMessage("/tuio/2Dcur") << "set" << (int32_t)1 << 0.5f << 0.5f << 0.0f << 0.0f << 0.0f << EndMessage;
	OutboundPacketStream fseq(fseqbuf, sizeof(fseqbuf));
	fseq << BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << EndMessage;

	// inner bundle holding only the set message
	char inner[512], *ip = inner;
	memcpy(ip, "#bundle\0", 8); ip += 8;
	memset(ip, 0, 8); ip += 8;
	ip = appendElement(ip, setbuf, (int)set.Size());

	char buf[1024], *p = buf;
	memcpy(p, "#bundle\0", 8); p += 8;
	memset(p, 0, 8); p += 8;
	p = appendElement(p, inner, (int)(ip-inner));
	p = appendElement(p, fseqbuf, (int)fseq.Size());

	stub.ProcessPacket(buf, (int)(p-buf), IpEndpointName());
	CHECK_EQ(listener.addCur, 1);
	CHECK_EQ(listener.refreshCount, 1);
}

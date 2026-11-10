#include "test_helpers.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "UdpSender.h"
#include "UdpReceiver.h"
#include "TcpSender.h"
#include "TcpReceiver.h"
using namespace TUIO;

TEST(client_server_udp_loopback) {
	TuioClient client(13401);
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();
	CHECK(client.isConnected());

	TuioServer server(new UdpSender("127.0.0.1", 13401));
	server.setSourceName("utest","127.0.0.1");
	server.initFrame(TuioTime::getSessionTime());
	server.addTuioCursor(0.4f, 0.6f);
	server.addTuioObject(5, 0.3f, 0.3f, 0.5f);
	server.addTuioBlob(0.5f, 0.5f, 0.2f, 0.1f, 0.1f, 0.01f);
	server.commitFrame();

	WAIT_FOR(listener.addCur >= 1, 2000);
	WAIT_FOR(listener.addObj >= 1, 2000);
	WAIT_FOR(listener.addBlb >= 1, 2000);
	CHECK_NEAR(listener.lastX, 0.5, 1e-4);
	CHECK_EQ(listener.lastSymbolID, 5);
	WAIT_FOR(listener.refreshCount >= 1, 2000);

	// client-side getters reflect the received state
	CHECK_EQ(client.getTuioCursorCount(), 1);
	CHECK_EQ(client.getTuioObjectCount(), 1);
	CHECK_EQ(client.getTuioBlobCount(), 1);

	// update propagates over the wire
	TuioCursor *tcur = server.getTuioCursors().front();
	server.initFrame(TuioTime::getSessionTime());
	server.updateTuioCursor(tcur, 0.7f, 0.7f);
	server.commitFrame();
	WAIT_FOR(listener.updCur >= 1, 2000);

	// removal propagates
	server.initFrame(TuioTime::getSessionTime());
	server.removeTuioCursor(tcur);
	server.commitFrame();
	WAIT_FOR(listener.delCur >= 1, 2000);

	client.disconnect();
	CHECK(!client.isConnected());
}

TEST(client_server_tcp_loopback) {
	// listen-mode sender + connect-mode receiver
	TcpSender *sender = new TcpSender(13405);
	TuioServer server(sender);
	server.setSourceName("ttest","127.0.0.1");
	TuioClient client(new TcpReceiver("127.0.0.1", 13405));
	RecordingListener listener;
	client.addTuioListener(&listener);
	client.connect();

	// wait for the accept before sending — packets sent to an empty
	// client list are silently dropped
	WAIT_FOR(sender->isConnected(), 2000);

	server.initFrame(TuioTime::getSessionTime());
	server.addTuioCursor(0.2f, 0.8f);
	server.commitFrame();

	WAIT_FOR(listener.addCur >= 1, 3000);
	CHECK_NEAR(listener.lastX, 0.2, 1e-4);
	CHECK_NEAR(listener.lastY, 0.8, 1e-4);

	client.disconnect();
}

TEST(sender_packet_bounds) {
	char buf[128];
	// a packet stream with no data must be rejected
	osc::OutboundPacketStream empty(buf, sizeof(buf));
	CHECK_EQ((int)empty.Size(), 0);

	UdpSender udp("127.0.0.1", 13411);
	CHECK(udp.isConnected());
	CHECK(!udp.sendOscPacket(&empty)); // Size()==0 -> false

	osc::OutboundPacketStream msg(buf, sizeof(buf));
	msg << osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << osc::EndMessage;
	CHECK(udp.sendOscPacket(&msg));

	// a packet larger than the sender's buffer size is rejected
	std::vector<char> bigbuf(5000);
	std::vector<char> blob(4100, 'x');
	osc::OutboundPacketStream oversized(&bigbuf[0], bigbuf.size());
	oversized << osc::BeginMessage("/t") << osc::Blob(&blob[0], 4100) << osc::EndMessage;
	CHECK((int)oversized.Size() > 4096);
	CHECK(!udp.sendOscPacket(&oversized));
}

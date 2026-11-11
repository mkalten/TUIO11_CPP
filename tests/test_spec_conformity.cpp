/*
 TUIO 1.1 specification conformity tests.

 Decodes the wire bundles emitted by TuioServer and verifies the OSC
 message layout of the 2D profiles against the TUIO 1.1 specification
 (https://www.tuio.org/?specification):

   /tuio/2Dobj set s i x y a X Y A m r
   /tuio/2Dcur set s x y X Y m
   /tuio/2Dblb set s x y a w h f X Y A m r

 with s (Session ID) and i (Class ID) encoded as OSC int32 and all
 other parameters as OSC float32, and bundles ordered
 source -> alive -> set* -> fseq.
*/

#include "test_util.h"
#include "test_helpers.h"
#include "TuioClient.h"
#include "TuioServer.h"
#include "osc/OscOutboundPacketStream.h"
#include <type_traits>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace TUIO;
using namespace osc;

// CaptureSender that additionally retains every packet of a frame,
// so tests can inspect multi-packet frames of a split bundle
class PacketCollector : public CaptureSender {
public:
	std::vector< std::vector<char> > allPackets;
	PacketCollector(int size=8192) { buffer_size = size; }
	bool sendOscPacket(osc::OutboundPacketStream *bundle) {
		CaptureSender::sendOscPacket(bundle);
		allPackets.push_back(lastPacket);
		return true;
	}
	void reset() { CaptureSender::reset(); allPackets.clear(); }
};

// decodes a captured OSC packet into its top-level messages
static std::vector<osc::ReceivedMessage> decodeMessages(const std::vector<char> &packet) {
	std::vector<osc::ReceivedMessage> msgs;
	if (packet.empty()) return msgs;
	osc::ReceivedPacket p(&packet[0], packet.size());
	if (p.IsBundle()) {
		osc::ReceivedBundle b(p);
		for (osc::ReceivedBundle::const_iterator i = b.ElementsBegin(); i != b.ElementsEnd(); ++i)
			if (i->IsMessage()) msgs.push_back(osc::ReceivedMessage(*i));
	} else {
		msgs.push_back(osc::ReceivedMessage(p));
	}
	return msgs;
}

// collects the arguments of a message for indexed access
static std::vector<osc::ReceivedMessageArgument> msgArgs(const osc::ReceivedMessage &m) {
	std::vector<osc::ReceivedMessageArgument> args;
	for (osc::ReceivedMessage::const_iterator a = m.ArgumentsBegin(); a != m.ArgumentsEnd(); ++a)
		args.push_back(*a);
	return args;
}

// the first argument of every TUIO message is the command string
static std::string msgCmd(const osc::ReceivedMessage &m) {
	osc::ReceivedMessage::const_iterator a = m.ArgumentsBegin();
	if (a==m.ArgumentsEnd() || !(*a).IsString()) return "";
	return (*a).AsString();
}

static std::string msgTags(const osc::ReceivedMessage &m) {
	return m.TypeTags() ? m.TypeTags() : "";
}

static int32_t msgIntArg(const osc::ReceivedMessage &m, unsigned int index) {
	osc::ReceivedMessage::const_iterator a = m.ArgumentsBegin();
	for (unsigned int i=0;i<index;i++) ++a;
	return (*a).AsInt32();
}


// ------------------------- wire format: server -> spec -------------------------

TEST(spec_wire_2Dobj_set_layout) {
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);
	server.setSourceName("utest","127.0.0.1");
	sender->reset();

	server.initFrame(TuioTime(0,0));
	TuioObject *tobj = server.addTuioObject(5, 0.3f, 0.4f, 0.5f);
	server.commitFrame();
	server.initFrame(TuioTime(1,0));
	server.updateTuioObject(tobj, 0.8f, 0.4f, 0.5f+(float)M_PI);
	server.commitFrame();

	std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->lastPacket);
	if (msgs.size()!=4) { CHECK_EQ(msgs.size(), 4u); return; }

	// s i x y a X Y A m r -> "set" + int32 + int32 + 8 float32
	CHECK_EQ(std::string(msgs[2].AddressPattern()), "/tuio/2Dobj");
	CHECK_EQ(msgCmd(msgs[2]), "set");
	CHECK_EQ(msgTags(msgs[2]), "siiffffffff");
	CHECK_EQ((int)msgs[2].ArgumentCount(), 11);

	std::vector<osc::ReceivedMessageArgument> args = msgArgs(msgs[2]);
	CHECK(args[1].IsInt32() && args[2].IsInt32());
	for (int i=3;i<11;i++) CHECK(args[i].IsFloat());
	CHECK_EQ(args[1].AsInt32(), tobj->getSessionID());
	CHECK_EQ(args[2].AsInt32(), 5);
	CHECK_NEAR(args[3].AsFloat(), 0.8, 1e-6);
	CHECK_NEAR(args[4].AsFloat(), 0.4, 1e-6);
	CHECK_NEAR(args[5].AsFloat(), 0.5+M_PI, 1e-5);
	// moved half the axis in 1 s -> X=0.5; rotated pi in 1 s -> A=0.5 turns/s
	CHECK_NEAR(args[6].AsFloat(), 0.5, 1e-5);
	CHECK_NEAR(args[7].AsFloat(), 0.0, 1e-6);
	CHECK_NEAR(args[8].AsFloat(), 0.5, 1e-5);
	CHECK_NEAR(args[9].AsFloat(), 0.5, 1e-5);
	CHECK_NEAR(args[10].AsFloat(), 0.5, 1e-5);
}

TEST(spec_wire_2Dcur_set_layout) {
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);
	sender->reset();

	server.initFrame(TuioTime(0,0));
	TuioCursor *tcur = server.addTuioCursor(0.2f, 0.8f);
	server.commitFrame();
	server.initFrame(TuioTime(1,0));
	server.updateTuioCursor(tcur, 0.7f, 0.8f);
	server.commitFrame();

	std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->lastPacket);
	if (msgs.size()!=3) { CHECK_EQ(msgs.size(), 3u); return; } // no source: alive,set,fseq

	// s x y X Y m -> "set" + int32 + 5 float32
	CHECK_EQ(std::string(msgs[1].AddressPattern()), "/tuio/2Dcur");
	CHECK_EQ(msgCmd(msgs[1]), "set");
	CHECK_EQ(msgTags(msgs[1]), "sifffff");
	CHECK_EQ((int)msgs[1].ArgumentCount(), 7);

	std::vector<osc::ReceivedMessageArgument> args = msgArgs(msgs[1]);
	CHECK(args[1].IsInt32());
	for (int i=2;i<7;i++) CHECK(args[i].IsFloat());
	CHECK_EQ(args[1].AsInt32(), tcur->getSessionID());
	CHECK_NEAR(args[2].AsFloat(), 0.7, 1e-6);
	CHECK_NEAR(args[3].AsFloat(), 0.8, 1e-6);
	CHECK_NEAR(args[4].AsFloat(), 0.5, 1e-5);  // X
	CHECK_NEAR(args[5].AsFloat(), 0.0, 1e-6);  // Y
	CHECK_NEAR(args[6].AsFloat(), 0.5, 1e-5);  // m
}

TEST(spec_wire_2Dblb_set_layout) {
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);
	sender->reset();

	server.initFrame(TuioTime(0,0));
	TuioBlob *tblb = server.addTuioBlob(0.2f, 0.3f, 0.1f, 0.2f, 0.15f, 0.03f);
	server.commitFrame();
	server.initFrame(TuioTime(1,0));
	server.updateTuioBlob(tblb, 0.7f, 0.3f, 0.1f+(float)M_PI, 0.4f, 0.15f, 0.06f);
	server.commitFrame();

	std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->lastPacket);
	if (msgs.size()!=3) { CHECK_EQ(msgs.size(), 3u); return; }

	// s x y a w h f X Y A m r -> "set" + int32 + 11 float32
	CHECK_EQ(std::string(msgs[1].AddressPattern()), "/tuio/2Dblb");
	CHECK_EQ(msgCmd(msgs[1]), "set");
	CHECK_EQ(msgTags(msgs[1]), "sifffffffffff");
	CHECK_EQ((int)msgs[1].ArgumentCount(), 13);

	std::vector<osc::ReceivedMessageArgument> args = msgArgs(msgs[1]);
	CHECK(args[1].IsInt32());
	for (int i=2;i<13;i++) CHECK(args[i].IsFloat());
	CHECK_EQ(args[1].AsInt32(), tblb->getSessionID());
	CHECK_NEAR(args[2].AsFloat(), 0.7, 1e-6);
	CHECK_NEAR(args[3].AsFloat(), 0.3, 1e-6);
	CHECK_NEAR(args[4].AsFloat(), 0.1+M_PI, 1e-5);
	CHECK_NEAR(args[5].AsFloat(), 0.4, 1e-6);   // w
	CHECK_NEAR(args[6].AsFloat(), 0.15, 1e-6);  // h
	CHECK_NEAR(args[7].AsFloat(), 0.06, 1e-6);  // f (area)
	CHECK_NEAR(args[8].AsFloat(), 0.5, 1e-5);   // X
	CHECK_NEAR(args[9].AsFloat(), 0.0, 1e-6);   // Y
	CHECK_NEAR(args[10].AsFloat(), 0.5, 1e-5);  // A
	CHECK_NEAR(args[11].AsFloat(), 0.5, 1e-5);  // m
	CHECK_NEAR(args[12].AsFloat(), 0.5, 1e-5);  // r
}

TEST(spec_wire_bundle_order) {
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);
	server.setSourceName("order","127.0.0.1");
	sender->reset();

	server.initFrame(TuioTime(0,0));
	server.addTuioObject(1, 0.5f, 0.5f, 0.0f);
	server.addTuioObject(2, 0.6f, 0.6f, 0.0f);
	server.addTuioCursor(0.4f, 0.4f);
	server.addTuioBlob(0.5f, 0.5f, 0.0f, 0.1f, 0.1f, 0.01f);
	server.commitFrame();

	// one bundle per profile, each ordered source -> alive -> set* -> fseq
	CHECK_EQ(sender->allPackets.size(), 3u);
	const char *profiles[] = { "/tuio/2Dobj", "/tuio/2Dcur", "/tuio/2Dblb" };
	for (unsigned int i=0;i<3 && i<sender->allPackets.size();i++) {
		std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->allPackets[i]);
		if (msgs.size()<3) { CHECK(msgs.size()>=3); continue; }
		for (std::vector<osc::ReceivedMessage>::iterator m=msgs.begin(); m!=msgs.end(); ++m)
			CHECK_EQ(std::string(m->AddressPattern()), profiles[i]);
		CHECK_EQ(msgCmd(msgs.front()), "source");
		CHECK_EQ(msgCmd(msgs[1]), "alive");
		CHECK_EQ(msgCmd(msgs.back()), "fseq");
		for (unsigned int j=2;j<msgs.size()-1;j++)
			CHECK_EQ(msgCmd(msgs[j]), "set");
	}
	// the object bundle carries two set messages
	std::vector<osc::ReceivedMessage> objMsgs = decodeMessages(sender->allPackets[0]);
	CHECK_EQ(objMsgs.size(), 5u);
}

TEST(spec_wire_alive_lists_all) {
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);
	sender->reset();

	server.initFrame(TuioTime(0,0));
	for (int i=0;i<4;i++) server.addTuioCursor(i/4.0f, 0.5f);
	server.commitFrame();

	std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->lastPacket);
	if (msgs.size()<2) { CHECK(msgs.size()>=2); return; }

	// the alive message is a list of int32 session IDs of all active components
	CHECK_EQ(msgCmd(msgs[0]), "alive");
	CHECK_EQ(msgTags(msgs[0]), "siiii");
	std::vector<osc::ReceivedMessageArgument> args = msgArgs(msgs[0]);
	CHECK_EQ((int)args.size(), 5);
	std::vector<int> aliveIds;
	for (unsigned int i=1;i<args.size();i++) {
		CHECK(args[i].IsInt32());
		aliveIds.push_back(args[i].AsInt32());
	}
	std::list<TuioCursor*> serverCursors = server.getTuioCursors();
	for (std::list<TuioCursor*>::iterator c=serverCursors.begin(); c!=serverCursors.end(); ++c)
		CHECK(std::find(aliveIds.begin(), aliveIds.end(), (*c)->getSessionID()) != aliveIds.end());
}

TEST(spec_wire_fseq_semantics) {
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);

	// fseq is int32 and increments with each committed frame
	sender->reset();
	server.initFrame(TuioTime(0,0));
	TuioCursor *tcur = server.addTuioCursor(0.2f, 0.5f);
	server.commitFrame();
	std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->lastPacket);
	CHECK_EQ(msgCmd(msgs.back()), "fseq");
	CHECK_EQ(msgTags(msgs.back()), "si");
	CHECK_EQ(msgIntArg(msgs.back(), 1), 0);

	server.initFrame(TuioTime(1,0));
	server.updateTuioCursor(tcur, 0.6f, 0.5f);
	server.commitFrame();
	msgs = decodeMessages(sender->lastPacket);
	CHECK_EQ(msgIntArg(msgs.back(), 1), 1);

	// redundant bundles of sendFullMessages are flagged with fseq -1
	sender->reset();
	server.sendFullMessages();
	CHECK_EQ(sender->allPackets.size(), 3u);
	for (unsigned int i=0;i<sender->allPackets.size();i++) {
		std::vector<osc::ReceivedMessage> fm = decodeMessages(sender->allPackets[i]);
		if (fm.size()<2) { CHECK(fm.size()>=2); continue; }
		CHECK_EQ(msgCmd(fm.back()), "fseq");
		CHECK_EQ(msgIntArg(fm.back(), 1), -1);
	}
}

TEST(spec_wire_split_same_fseq) {
	// a small packet size forces the frame into several packets that
	// all carry the same fseq and each repeat the alive message
	PacketCollector *sender = new PacketCollector(576);
	TuioServer server(sender);
	server.setSourceName("split","127.0.0.1");
	sender->reset();

	server.initFrame(TuioTime(0,0));
	for (int i=0;i<40;i++) server.addTuioCursor(i/40.0f, 0.5f);
	server.commitFrame();

	CHECK(sender->allPackets.size() > 1);
	for (unsigned int i=0;i<sender->allPackets.size();i++) {
		std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->allPackets[i]);
		if (msgs.size()<3) { CHECK(msgs.size()>=3); continue; }
		CHECK_EQ(msgCmd(msgs[0]), "source");
		CHECK_EQ(msgCmd(msgs[1]), "alive");
		CHECK_EQ(msgCmd(msgs.back()), "fseq");
		CHECK_EQ(msgIntArg(msgs.back(), 1), 0);
	}
}

TEST(spec_wire_source_format) {
	// without a configured source the bundle starts with the alive message
	PacketCollector *s1 = new PacketCollector();
	TuioServer srv1(s1);
	srv1.initFrame(TuioTime(0,0));
	srv1.addTuioCursor(0.5f, 0.5f);
	srv1.commitFrame();
	std::vector<osc::ReceivedMessage> msgs = decodeMessages(s1->lastPacket);
	if (msgs.size()<1) { CHECK(msgs.size()>=1); return; }
	CHECK_EQ(msgCmd(msgs[0]), "alive");

	// with setSourceName the bundle starts with a "name@address" string
	PacketCollector *s2 = new PacketCollector();
	TuioServer srv2(s2);
	srv2.setSourceName("myapp","10.0.0.1");
	srv2.initFrame(TuioTime(0,0));
	srv2.addTuioCursor(0.5f, 0.5f);
	srv2.commitFrame();
	msgs = decodeMessages(s2->lastPacket);
	if (msgs.size()<1) { CHECK(msgs.size()>=1); return; }
	CHECK_EQ(msgCmd(msgs[0]), "source");
	CHECK_EQ(msgTags(msgs[0]), "ss");
	std::vector<osc::ReceivedMessageArgument> args = msgArgs(msgs[0]);
	if (args.size()<2) { CHECK(args.size()>=2); return; }
	CHECK(args[1].IsString());
	CHECK_EQ(std::string(args[1].AsString()), "myapp@10.0.0.1");
}

TEST(spec_wire_empty_bundle_minimal) {
	// a minimal TUIO bundle contains only the compulsory alive and fseq
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);

	CHECK_EQ(sender->allPackets.size(), 3u);
	const char *profiles[] = { "/tuio/2Dcur", "/tuio/2Dobj", "/tuio/2Dblb" };
	for (unsigned int i=0;i<3 && i<sender->allPackets.size();i++) {
		std::vector<osc::ReceivedMessage> msgs = decodeMessages(sender->allPackets[i]);
		if (msgs.size()!=2) { CHECK_EQ(msgs.size(), 2u); continue; }
		CHECK_EQ(std::string(msgs[0].AddressPattern()), profiles[i]);
		CHECK_EQ(msgCmd(msgs[0]), "alive");
		CHECK_EQ(msgTags(msgs[0]), "s"); // empty alive list
		CHECK_EQ(msgCmd(msgs[1]), "fseq");
		CHECK_EQ(msgIntArg(msgs[1], 1), -1); // redundant bundle marker
	}
}


// ------------------------- client decode: spec -> client -------------------------

TEST(spec_client_decodes_2Dobj_fields) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	// frame 1: object is added with its primary attributes
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dobj") << "alive" << (int32_t)42 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dobj") << "set" << (int32_t)42 << (int32_t)7 << 0.11f << 0.22f << 0.33f << 1.0f << 2.0f << 0.25f << 0.5f << 0.125f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dobj") << "fseq" << (int32_t)1 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addObj, 1);
	TuioObject *tobj = client.getTuioObject(42);
	CHECK(tobj!=NULL);
	if (tobj==NULL) return;
	CHECK_EQ(tobj->getSessionID(), 42);
	CHECK_EQ(tobj->getSymbolID(), 7);
	CHECK_NEAR(tobj->getX(), 0.11, 1e-6);
	CHECK_NEAR(tobj->getY(), 0.22, 1e-6);
	CHECK_NEAR(tobj->getAngle(), 0.33, 1e-6);

	// frame 2: the dynamic attributes of the set message are applied
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dobj") << "alive" << (int32_t)42 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dobj") << "set" << (int32_t)42 << (int32_t)7 << 0.31f << 0.32f << 0.43f << 1.0f << 2.0f << 0.25f << 0.5f << 0.125f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dobj") << "fseq" << (int32_t)2 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.updObj, 1);
	CHECK_NEAR(tobj->getX(), 0.31, 1e-6);
	CHECK_NEAR(tobj->getY(), 0.32, 1e-6);
	CHECK_NEAR(tobj->getAngle(), 0.43, 1e-6);
	CHECK_NEAR(tobj->getXSpeed(), 1.0, 1e-6);
	CHECK_NEAR(tobj->getYSpeed(), 2.0, 1e-6);
	CHECK_NEAR(tobj->getRotationSpeed(), 0.25, 1e-6);
	CHECK_NEAR(tobj->getMotionAccel(), 0.5, 1e-6);
	CHECK_NEAR(tobj->getRotationAccel(), 0.125, 1e-6);

	client.removeAllTuioListeners();
}

TEST(spec_client_decodes_2Dcur_fields) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)10 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)10 << 0.1f << 0.2f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 1);
	TuioCursor *tcur = client.getTuioCursor(10);
	CHECK(tcur!=NULL);
	if (tcur==NULL) return;
	CHECK_EQ(tcur->getSessionID(), 10);
	CHECK_EQ(tcur->getCursorID(), 0);
	CHECK_NEAR(tcur->getX(), 0.1, 1e-6);
	CHECK_NEAR(tcur->getY(), 0.2, 1e-6);

	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)10 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)10 << 0.3f << 0.4f << 1.0f << 2.0f << 0.5f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)2 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.updCur, 1);
	CHECK_NEAR(tcur->getX(), 0.3, 1e-6);
	CHECK_NEAR(tcur->getY(), 0.4, 1e-6);
	CHECK_NEAR(tcur->getXSpeed(), 1.0, 1e-6);
	CHECK_NEAR(tcur->getYSpeed(), 2.0, 1e-6);
	CHECK_NEAR(tcur->getMotionAccel(), 0.5, 1e-6);

	client.removeAllTuioListeners();
}

TEST(spec_client_decodes_2Dblb_fields) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dblb") << "alive" << (int32_t)8 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "set" << (int32_t)8 << 0.1f << 0.2f << 0.3f << 0.2f << 0.15f << 0.05f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "fseq" << (int32_t)1 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addBlb, 1);
	TuioBlob *tblb = client.getTuioBlob(8);
	CHECK(tblb!=NULL);
	if (tblb==NULL) return;
	CHECK_EQ(tblb->getSessionID(), 8);
	CHECK_EQ(tblb->getBlobID(), 0);
	CHECK_NEAR(tblb->getX(), 0.1, 1e-6);
	CHECK_NEAR(tblb->getAngle(), 0.3, 1e-6);
	CHECK_NEAR(tblb->getWidth(), 0.2, 1e-6);
	CHECK_NEAR(tblb->getHeight(), 0.15, 1e-6);
	CHECK_NEAR(tblb->getArea(), 0.05, 1e-6);

	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dblb") << "alive" << (int32_t)8 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "set" << (int32_t)8 << 0.4f << 0.5f << 0.6f << 0.3f << 0.25f << 0.07f << 1.0f << 2.0f << 0.25f << 0.5f << 0.125f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "fseq" << (int32_t)2 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.updBlb, 1);
	CHECK_NEAR(tblb->getX(), 0.4, 1e-6);
	CHECK_NEAR(tblb->getAngle(), 0.6, 1e-6);
	CHECK_NEAR(tblb->getWidth(), 0.3, 1e-6);
	CHECK_NEAR(tblb->getHeight(), 0.25, 1e-6);
	CHECK_NEAR(tblb->getArea(), 0.07, 1e-6);
	CHECK_NEAR(tblb->getXSpeed(), 1.0, 1e-6);
	CHECK_NEAR(tblb->getYSpeed(), 2.0, 1e-6);
	CHECK_NEAR(tblb->getRotationSpeed(), 0.25, 1e-6);
	CHECK_NEAR(tblb->getMotionAccel(), 0.5, 1e-6);
	CHECK_NEAR(tblb->getRotationAccel(), 0.125, 1e-6);

	client.removeAllTuioListeners();
}

TEST(spec_client_wrong_typetags_ignored) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];

	// a set message with a float session ID (int32 expected) is skipped
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)5 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << 5.0f << 0.1f << 0.1f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 0);
	CHECK_EQ(listener.refreshCount, 1); // the frame still completes

	// a truncated set message (missing arguments) is skipped
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)6 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)6 << 0.1f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)2 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 0);
	CHECK_EQ(listener.refreshCount, 2);

	// an int32 where a float is expected is skipped as well
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)7 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)7 << (int32_t)1 << 0.1f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)3 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 0);
	CHECK_EQ(listener.refreshCount, 3);

	// an unknown command is ignored
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "bogus" << (int32_t)9 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)4 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 0);

	// a set message with surplus arguments is tolerated
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)8 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)8 << 0.1f << 0.2f << 0.0f << 0.0f << 0.0f << 3.14f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)5 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 1);

	client.removeAllTuioListeners();
}

TEST(spec_client_fseq_handling) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];

	// frame 5 adds cursor 10
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)10 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)10 << 0.1f << 0.1f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)5 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 1);
	CHECK_EQ(listener.refreshCount, 1);

	// a second packet with the same fseq (split bundle) is still accepted
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)10 << (int32_t)11 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)11 << 0.2f << 0.2f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)5 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 2);
	CHECK_EQ(listener.refreshCount, 2);

	// an older fseq is dropped entirely
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)10 << (int32_t)11 << (int32_t)12 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)12 << 0.3f << 0.3f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)3 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 2);
	CHECK_EQ(listener.refreshCount, 2); // no refresh on dropped frames
	CHECK(client.getTuioCursor(12)==NULL);

	// fseq -1 marks a redundant bundle and is processed normally
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)10 << (int32_t)11 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)-1 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.refreshCount, 3);
	CHECK_EQ(client.getTuioCursorCount(), 2);

	client.removeAllTuioListeners();
}

TEST(spec_client_source_multiplexing) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];

	// two sources emitting the same session ID produce distinct components
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "source" << "cam1@10.0.0.1" << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)5 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)5 << 0.1f << 0.1f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)1 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "source" << "cam2@10.0.0.2" << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)5 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)5 << 0.9f << 0.9f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)2 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addCur, 2);
	CHECK_EQ(client.getTuioCursorCount(), 2);

	TuioCursor *c1 = client.getTuioCursor(0, 5);
	TuioCursor *c2 = client.getTuioCursor(1, 5);
	CHECK(c1!=NULL && c2!=NULL);
	if (c1==NULL || c2==NULL) return;
	CHECK_EQ(std::string(c1->getTuioSourceName()), "cam1");
	CHECK_EQ(std::string(c1->getTuioSourceAddress()), "10.0.0.1");
	CHECK_EQ(std::string(c2->getTuioSourceName()), "cam2");
	CHECK_EQ(std::string(c2->getTuioSourceAddress()), "10.0.0.2");
	// cursor IDs are assigned per source
	CHECK_EQ(c1->getCursorID(), 0);
	CHECK_EQ(c2->getCursorID(), 0);

	// a source string without '@' implicitly comes from localhost
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dcur") << "source" << "localapp" << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "alive" << (int32_t)9 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "set" << (int32_t)9 << 0.5f << 0.5f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dcur") << "fseq" << (int32_t)3 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	TuioCursor *c3 = client.getTuioCursor(2, 9);
	CHECK(c3!=NULL);
	if (c3==NULL) return;
	CHECK_EQ(std::string(c3->getTuioSourceName()), "localapp");
	CHECK_EQ(std::string(c3->getTuioSourceAddress()), "localhost");

	client.removeAllTuioListeners();
}

// regression for the blob dedup fix: a set message that only changes
// the rotation speed/accel must still trigger a blob update
TEST(spec_client_blob_rotation_update) {
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);

	char buffer[4096];
	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dblb") << "alive" << (int32_t)8 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "set" << (int32_t)8 << 0.3f << 0.4f << 0.5f << 0.2f << 0.15f << 0.03f << 0.0f << 0.0f << 0.0f << 0.0f << 0.0f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "fseq" << (int32_t)1 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.addBlb, 1);
	TuioBlob *tblb = client.getTuioBlob(8);
	CHECK(tblb!=NULL);
	if (tblb==NULL) return;

	{
		osc::OutboundPacketStream b(buffer, 4096);
		b << osc::BeginBundleImmediate
			<< osc::BeginMessage("/tuio/2Dblb") << "alive" << (int32_t)8 << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "set" << (int32_t)8 << 0.3f << 0.4f << 0.5f << 0.2f << 0.15f << 0.03f << 0.0f << 0.0f << 0.5f << 0.0f << 0.1f << osc::EndMessage
			<< osc::BeginMessage("/tuio/2Dblb") << "fseq" << (int32_t)2 << osc::EndMessage
		<< osc::EndBundle;
		stub.ProcessPacket(b.Data(), (int)b.Size(), IpEndpointName());
	}
	CHECK_EQ(listener.updBlb, 1);
	CHECK_NEAR(tblb->getRotationSpeed(), 0.5, 1e-6);
	CHECK_NEAR(tblb->getRotationAccel(), 0.1, 1e-6);

	client.removeAllTuioListeners();
}


// ------------------------- component types & semantics -------------------------

TEST(spec_component_types) {
	// spec data types: session/class/component IDs are int32 on the wire
	// (int in the API), all geometric and kinetic attributes are float
	CHECK((std::is_same<decltype(((TuioContainer*)0)->getSessionID()), int>::value));
	CHECK((std::is_same<decltype(((TuioObject*)0)->getSymbolID()), int>::value));
	CHECK((std::is_same<decltype(((TuioCursor*)0)->getCursorID()), int>::value));
	CHECK((std::is_same<decltype(((TuioBlob*)0)->getBlobID()), int>::value));
	CHECK((std::is_same<decltype(((TuioPoint*)0)->getX()), float>::value));
	CHECK((std::is_same<decltype(((TuioPoint*)0)->getY()), float>::value));
	CHECK((std::is_same<decltype(((TuioObject*)0)->getAngle()), float>::value));
	CHECK((std::is_same<decltype(((TuioBlob*)0)->getAngle()), float>::value));
	CHECK((std::is_same<decltype(((TuioBlob*)0)->getWidth()), float>::value));
	CHECK((std::is_same<decltype(((TuioBlob*)0)->getHeight()), float>::value));
	CHECK((std::is_same<decltype(((TuioBlob*)0)->getArea()), float>::value));
	CHECK((std::is_same<decltype(((TuioContainer*)0)->getXSpeed()), float>::value));
	CHECK((std::is_same<decltype(((TuioContainer*)0)->getYSpeed()), float>::value));
	CHECK((std::is_same<decltype(((TuioContainer*)0)->getMotionSpeed()), float>::value));
	CHECK((std::is_same<decltype(((TuioContainer*)0)->getMotionAccel()), float>::value));
	CHECK((std::is_same<decltype(((TuioObject*)0)->getRotationSpeed()), float>::value));
	CHECK((std::is_same<decltype(((TuioObject*)0)->getRotationAccel()), float>::value));
}

TEST(spec_angle_normalized) {
	// spec: angle range 0..2PI; the server-side update keeps it there
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.2f);
	tobj.update(TuioTime(1,0), 0.0f, 0.0f, -0.3f); // crossing 0 wraps up
	CHECK(tobj.getAngle()>=0.0f && tobj.getAngle()<=2*M_PI);
	CHECK_NEAR(tobj.getAngle(), 2*M_PI-0.3, 1e-5);
	tobj.update(TuioTime(2,0), 0.0f, 0.0f, (float)(2*M_PI+0.3));
	CHECK(tobj.getAngle()>=0.0f && tobj.getAngle()<=2*M_PI);
	CHECK_NEAR(tobj.getAngle(), 0.3, 1e-4);
}

TEST(spec_speed_units) {
	// spec: velocity unit is one full axis length per second
	TuioCursor tcur(TuioTime(0,0), 1, 0, 0.0f, 0.0f);
	tcur.update(TuioTime(1,0), 1.0f, 0.0f); // full axis in one second
	CHECK_NEAR(tcur.getXSpeed(), 1.0, 1e-5);
	CHECK_NEAR(tcur.getMotionSpeed(), 1.0, 1e-5);

	// acceleration = speed change / dt
	tcur.update(TuioTime(2,0), 1.5f, 0.0f);
	CHECK_NEAR(tcur.getXSpeed(), 0.5, 1e-5);
	CHECK_NEAR(tcur.getMotionAccel(), -0.5, 1e-5);

	// spec: rotation velocity unit is one full rotation per second
	TuioObject tobj(TuioTime(0,0), 1, 1, 0.0f, 0.0f, 0.0f);
	tobj.update(TuioTime(1,0), 0.0f, 0.0f, (float)M_PI); // half a turn in 1 s
	CHECK_NEAR(tobj.getRotationSpeed(), 0.5, 1e-4);
}

TEST(spec_roundtrip_field_fidelity) {
	PacketCollector *sender = new PacketCollector();
	TuioServer server(sender);
	server.setSourceName("rt","127.0.0.1");
	StubReceiver stub;
	TuioClient client(&stub);
	RecordingListener listener;
	client.addTuioListener(&listener);
	sender->reset();

	server.initFrame(TuioTime(0,0));
	TuioObject *sobj = server.addTuioObject(9, 0.11f, 0.22f, 0.33f);
	TuioCursor *scur = server.addTuioCursor(0.44f, 0.55f);
	TuioBlob *sblb = server.addTuioBlob(0.66f, 0.77f, 0.88f, 0.12f, 0.34f, 0.05f);
	server.commitFrame();
	CHECK_EQ(sender->allPackets.size(), 3u);
	for (unsigned int i=0;i<sender->allPackets.size();i++)
		stub.ProcessPacket(sender->allPackets[i].data(), (int)sender->allPackets[i].size(), IpEndpointName());

	TuioObject *cobj = client.getTuioObject(sobj->getSessionID());
	TuioCursor *ccur = client.getTuioCursor(scur->getSessionID());
	TuioBlob *cblb = client.getTuioBlob(sblb->getSessionID());
	CHECK(cobj!=NULL && ccur!=NULL && cblb!=NULL);
	if (cobj==NULL || ccur==NULL || cblb==NULL) return;
	CHECK_EQ(cobj->getSymbolID(), 9);
	CHECK_EQ(cobj->getX(), 0.11f);
	CHECK_EQ(cobj->getY(), 0.22f);
	CHECK_EQ(cobj->getAngle(), 0.33f);
	CHECK_EQ(ccur->getX(), 0.44f);
	CHECK_EQ(ccur->getY(), 0.55f);
	CHECK_EQ(cblb->getX(), 0.66f);
	CHECK_EQ(cblb->getY(), 0.77f);
	CHECK_EQ(cblb->getAngle(), 0.88f);
	CHECK_EQ(cblb->getWidth(), 0.12f);
	CHECK_EQ(cblb->getHeight(), 0.34f);
	CHECK_EQ(cblb->getArea(), 0.05f);

	// dynamic attributes travel bit-exact through the wire format as well
	sender->reset();
	server.initFrame(TuioTime(1,0));
	server.updateTuioObject(sobj, 0.61f, 0.22f, 0.33f);
	server.updateTuioCursor(scur, 0.94f, 0.55f);
	server.updateTuioBlob(sblb, 0.96f, 0.77f, 0.88f, 0.12f, 0.34f, 0.05f);
	server.commitFrame();
	for (unsigned int i=0;i<sender->allPackets.size();i++)
		stub.ProcessPacket(sender->allPackets[i].data(), (int)sender->allPackets[i].size(), IpEndpointName());

	CHECK_EQ(cobj->getXSpeed(), sobj->getXSpeed());
	CHECK_EQ(cobj->getYSpeed(), sobj->getYSpeed());
	CHECK_EQ(cobj->getRotationSpeed(), sobj->getRotationSpeed());
	CHECK_EQ(cobj->getMotionAccel(), sobj->getMotionAccel());
	CHECK_EQ(cobj->getRotationAccel(), sobj->getRotationAccel());
	CHECK_EQ(ccur->getXSpeed(), scur->getXSpeed());
	CHECK_EQ(ccur->getMotionAccel(), scur->getMotionAccel());
	CHECK_EQ(cblb->getXSpeed(), sblb->getXSpeed());
	CHECK_EQ(cblb->getRotationSpeed(), sblb->getRotationSpeed());
	CHECK_EQ(cblb->getRotationAccel(), sblb->getRotationAccel());

	client.removeAllTuioListeners();
}

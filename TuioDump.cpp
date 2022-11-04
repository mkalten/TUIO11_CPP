/*
	TUIO C++ Example - part of the reacTIVision project
	http://reactivision.sourceforge.net/

	Copyright (c)

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	Any person wishing to distribute modifications to the Software is
	requested to send the modifications to the original developer so that
	they can be incorporated into the canonical version.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "TuioDump.h"

static std::string _address("localhost");
static bool _udp = true;
static int _port = 3333;


void TuioDump::addTuioObject(TuioObject *tobj) {
	std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/"<<  tobj->getTuioSourceID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;
}

void TuioDump::updateTuioObject(TuioObject *tobj) {
	std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/"<<  tobj->getTuioSourceID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() 
		<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
}

void TuioDump::removeTuioObject(TuioObject *tobj) {
	std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/"<<  tobj->getTuioSourceID() << ")" << std::endl;
}

void TuioDump::addTuioCursor(TuioCursor *tcur) {
	std::cout << "add cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << "/"<<  tcur->getTuioSourceID() << ") " << tcur->getX() << " " << tcur->getY() << std::endl;
}

void TuioDump::updateTuioCursor(TuioCursor *tcur) {
	std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << "/"<<  tcur->getTuioSourceID() << ") " << tcur->getX() << " " << tcur->getY() 
				<< " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
}

void TuioDump::removeTuioCursor(TuioCursor *tcur) {
	std::cout << "del cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << "/"<<  tcur->getTuioSourceID() << ")" << std::endl;
}

void TuioDump::addTuioBlob(TuioBlob *tblb) {
	std::cout << "add blb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/"<<  tblb->getTuioSourceID() << ") "<< tblb->getX() << " " << tblb->getY() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea() << std::endl;
}

void TuioDump::updateTuioBlob(TuioBlob *tblb) {
	std::cout << "set blb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/"<<  tblb->getTuioSourceID() << ") "<< tblb->getX() << " " << tblb->getY() << " " << tblb->getAngle() << " "<< tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea() 
	<< " " << tblb->getMotionSpeed() << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << std::endl;
}

void TuioDump::removeTuioBlob(TuioBlob *tblb) {
	std::cout << "del blb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/"<<  tblb->getTuioSourceID() << ")" << std::endl;
}



void TuioDump::addTuioObject25D(TuioObject25D *tobj) {
	std::cout << "add 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/" << tobj->getTuioSourceID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getAngle() << std::endl;
}

void TuioDump::updateTuioObject25D(TuioObject25D *tobj) {
	std::cout << "set 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/" << tobj->getTuioSourceID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getAngle()
		<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
}

void TuioDump::removeTuioObject25D(TuioObject25D *tobj) {
	std::cout << "del 25Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/" << tobj->getTuioSourceID() << ")" << std::endl;
}

void TuioDump::addTuioCursor25D(TuioCursor25D *tcur) {
	std::cout << "add 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << "/" << tcur->getTuioSourceID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ() << std::endl;
}

void TuioDump::updateTuioCursor25D(TuioCursor25D *tcur) {
	std::cout << "set 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << "/" << tcur->getTuioSourceID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
		<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
}

void TuioDump::removeTuioCursor25D(TuioCursor25D *tcur) {
	std::cout << "del 25Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << "/" << tcur->getTuioSourceID() << ")" << std::endl;
}

void TuioDump::addTuioBlob25D(TuioBlob25D *tblb) {
	std::cout << "add 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/" << tblb->getTuioSourceID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea() << std::endl;
}

void TuioDump::updateTuioBlob25D(TuioBlob25D *tblb) {
	std::cout << "set 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/" << tblb->getTuioSourceID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getAngle() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getArea()
		<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRotationSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << std::endl;
}

void TuioDump::removeTuioBlob25D(TuioBlob25D *tblb) {
	std::cout << "del 25Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/" << tblb->getTuioSourceID() << ")" << std::endl;
}




void TuioDump::addTuioObject3D(TuioObject3D *tobj) {
	std::cout << "add 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/" << tobj->getTuioSourceID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getRoll() << " " << tobj->getPitch() << " " << tobj->getYaw() << std::endl;
}

void TuioDump::updateTuioObject3D(TuioObject3D *tobj) {
	std::cout << "set 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/" << tobj->getTuioSourceID() << ") " << tobj->getX() << " " << tobj->getY() << " " << tobj->getZ() << " " << tobj->getRoll() << " " << tobj->getPitch() << " " << tobj->getYaw()
		<< " " << tobj->getXSpeed() << " " << tobj->getYSpeed() << " " << tobj->getZSpeed() << " " << tobj->getRollSpeed() << " " << tobj->getPitchSpeed() << " " << tobj->getYawSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;
}

void TuioDump::removeTuioObject3D(TuioObject3D *tobj) {
	std::cout << "del 3Dobj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/" << tobj->getTuioSourceID() << ")" << std::endl;
}

void TuioDump::addTuioCursor3D(TuioCursor3D *tcur) {
	std::cout << "add 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << "/" << tcur->getTuioSourceID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ() << std::endl;
}

void TuioDump::updateTuioCursor3D(TuioCursor3D *tcur) {
	std::cout << "set 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << "/" << tcur->getTuioSourceID() << ") " << tcur->getX() << " " << tcur->getY() << " " << tcur->getZ()
		<< " " << tcur->getXSpeed() << " " << tcur->getYSpeed() << " " << tcur->getZSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
}

void TuioDump::removeTuioCursor3D(TuioCursor3D *tcur) {
	std::cout << "del 3Dcur " << tcur->getCursorID() << " (" << tcur->getSessionID() << "/" << tcur->getTuioSourceID() << ")" << std::endl;
}

void TuioDump::addTuioBlob3D(TuioBlob3D *tblb) {
	std::cout << "add 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/" << tblb->getTuioSourceID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getRoll() << " " << tblb->getPitch() << " " << tblb->getYaw() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getDepth() << " " << tblb->getVolume() << std::endl;
}

void TuioDump::updateTuioBlob3D(TuioBlob3D *tblb) {
	std::cout << "set 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/" << tblb->getTuioSourceID() << ") " << tblb->getX() << " " << tblb->getY() << " " << tblb->getZ() << " " << tblb->getRoll() << " " << tblb->getPitch() << " " << tblb->getYaw() << " " << tblb->getWidth() << " " << tblb->getHeight() << " " << tblb->getDepth() << " " << tblb->getVolume()
		<< " " << tblb->getXSpeed() << " " << tblb->getYSpeed() << " " << tblb->getZSpeed() << " " << tblb->getRollSpeed() << " " << tblb->getPitchSpeed() << " " << tblb->getYawSpeed() << " " << tblb->getMotionAccel() << " " << tblb->getRotationAccel() << std::endl;
}

void TuioDump::removeTuioBlob3D(TuioBlob3D *tblb) {
	std::cout << "del 3Dblb " << tblb->getBlobID() << " (" << tblb->getSessionID() << "/" << tblb->getTuioSourceID() << ")" << std::endl;
}






void  TuioDump::refresh(TuioTime frameTime) {
	//std::cout << "refresh " << frameTime.getTotalMilliseconds() << std::endl;
}


static void show_help() {
	std::cout << "Usage: TuioDump -p [port] -t -a [address]" << std::endl;
	std::cout << "        -p [port] for alternative port number" << std::endl;
	std::cout << "        -t for TUIO/TCP (dedault is TUIO/UDP)" << std::endl;
	std::cout << "        -a [address] for remote TUIO/TCP server" << std::endl;
	std::cout << "           use 'incoming' for TUIO/TCP socket" << std::endl;
	std::cout << "        -h show this help" << std::endl;
}



static void init(int argc, char** argv) {
	char c;
	
#ifndef WIN32
	while ((c = getopt(argc, argv, "p:a:th")) != -1) {
		switch (c) {
			case 't':
				_udp = false;
				break;
			case 'a':
				_address = std::string(optarg);
				break;
			case 'p':
				_port = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(0);
			default:
				show_help();
				exit(1);
		}
	}
#else

	int index = 1;

	while (index < argc)
	{
		c = argv[index][1];

		switch (c) {
			case 't':
				_udp = false;
				index++;
				break;
			case 'a':
				index++;
				_address = argv[index];
				index++;
				break;
			case 'p':
				index++;
				_port = atoi(argv[index]);
				index++;
				break;
			case 'h':
				show_help();
				exit(0);
			default:
				show_help();
				exit(1);
		}

	}

#endif
}

int main(int argc, char* argv[])
{
	init(argc, argv);
	
	OscReceiver *osc_receiver;
	if (_udp) osc_receiver = new UdpReceiver(_port);
	else {
		if (_address=="incoming") osc_receiver = new TcpReceiver(_port);
		else osc_receiver = new TcpReceiver(_address.c_str(), _port);
	}
	TuioDump dump;
	TuioClient client(osc_receiver);
	client.addTuioListener(&dump);
	client.connect(true);

	delete osc_receiver;
	return 0;
}



/*
	TUIO C++ Audit - part of the reacTIVision project
	http://reactivision.sourceforge.net/

	Copyright (c) 2005-2026 Martin Kaltenbrunner <martin@tuio.org>

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

#include "TuioScenarios.h"
#include "UdpSender.h"
#include "TcpSender.h"
#include "WebSockSender.h"
#include "TcpUtils.h"

#include <signal.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#ifndef WIN32
#include <unistd.h>
#endif

static std::string _address("localhost");
static std::string _scenario("mixed");
static bool _udp = true, _tcp = false, _web = false, _verbose = false;
static int _port = 3333, _web_port = 8080, _fps = 60, _cycle = 0, _periodic = 0;

static bool running = true;
static void sighandler(int) { running = false; }

static void show_scenarios() {
	std::vector<TuioScenario*> scenarios = getTuioScenarios();
	int width = 0;
	for (std::vector<TuioScenario*>::iterator s=scenarios.begin(); s!=scenarios.end(); s++)
		if ((int)strlen((*s)->name())>width) width = (int)strlen((*s)->name());

	std::cout << "available scenarios:" << std::endl;
	for (std::vector<TuioScenario*>::iterator s=scenarios.begin(); s!=scenarios.end(); s++) {
		if ((*s)->raw()) continue;
		std::cout << "  " << std::left << std::setw(width) << (*s)->name() << "  " << (*s)->description() << std::endl;
	}
	std::cout << "malformed scenarios:" << std::endl;
	for (std::vector<TuioScenario*>::iterator s=scenarios.begin(); s!=scenarios.end(); s++) {
		if (!(*s)->raw()) continue;
		std::cout << "  " << std::left << std::setw(width) << (*s)->name() << "  " << (*s)->description() << std::endl;
	}
}

static void show_help() {
	std::cout << "Usage: TuioAudit -s [scenario] -p [port] -a [address] -t -w -u -r [fps] -c [sec] -v" << std::endl;
	std::cout << "        -s [scenario] select the test scenario (default is 'mixed')" << std::endl;
	std::cout << "        -l list the available scenarios" << std::endl;
	std::cout << "        -c [seconds] cycle through all scenarios" << std::endl;
	std::cout << "        -r [fps] frame rate (default is 60)" << std::endl;
	std::cout << "        -p [port] TUIO/UDP and TUIO/TCP port (default is 3333)" << std::endl;
	std::cout << "        -a [address] TUIO/UDP destination host (default is localhost)" << std::endl;
	std::cout << "        -u disable TUIO/UDP delivery" << std::endl;
	std::cout << "        -t enable an additional TUIO/TCP socket on the -p port" << std::endl;
	std::cout << "        -w enable an additional TUIO/WEB socket on port 8080" << std::endl;
	std::cout << "        -m [seconds] emit periodic alive/fseq messages (default off)" << std::endl;
	std::cout << "        -v verbose console output" << std::endl;
	std::cout << "        -h show this help" << std::endl;
}

static bool init(int argc, char** argv) {
#ifdef WIN32
	(void)argc; (void)argv;
	return true;
#else
	char c;
	while ((c = getopt(argc, argv, "s:lc:r:p:a:utm:wvh")) != -1) {
		switch (c) {
			case 's':
				_scenario = std::string(optarg);
				break;
			case 'l':
				show_scenarios();
				exit(0);
			case 'c':
				_cycle = atoi(optarg);
				if (_cycle<1) _cycle = 1;
				break;
			case 'r':
				_fps = atoi(optarg);
				if (_fps<1) _fps = 1;
				break;
			case 'p':
				_port = atoi(optarg);
				break;
			case 'a':
				_address = std::string(optarg);
				break;
			case 'u':
				_udp = false;
				break;
			case 't':
				_tcp = true;
				break;
			case 'w':
				_web = true;
				break;
			case 'm':
				_periodic = atoi(optarg);
				if (_periodic<1) _periodic = 1;
				break;
			case 'v':
				_verbose = true;
				break;
			case 'h':
				show_help();
				exit(0);
			default:
				show_help();
				return false;
		}
	}
	return true;
#endif
}

int main(int argc, char* argv[])
{
	if (!init(argc,argv)) return 1;

	if (!_udp && !_tcp && !_web) {
		std::cerr << "no transport enabled, use -t or -w together with -u" << std::endl;
		return 1;
	}

	// the first enabled sender becomes the primary one
	// the app keeps its own sender list for raw scenario delivery
	std::vector<OscSender*> senders;
	OscSender *sender = NULL;
	if (_udp) sender = new UdpSender(_address.c_str(),_port);
	else if (_tcp) {
		try { sender = new TcpSender(_port); }
		catch (std::exception e) { sender = NULL; }
	} else {
		try { sender = new WebSockSender(_web_port); }
		catch (std::exception e) { sender = NULL; }
	}
	if (!sender) {
		std::cerr << "could not create the OSC sender" << std::endl;
		return 1;
	}
	senders.push_back(sender);

	TuioServer *server = new TuioServer(sender);
	server->setSourceName("TuioAudit");
	server->setVerbose(_verbose);
	if (_periodic>0) server->enablePeriodicMessages(_periodic);

	// additional TUIO/TCP sender (unless TCP is already the primary sender)
	if (_tcp && !(!_udp && _tcp)) {
		OscSender *tcp_sender = NULL;
		try { tcp_sender = new TcpSender(_port); }
		catch (std::exception e) { tcp_sender = NULL; }
		if (tcp_sender) { server->addOscSender(tcp_sender); senders.push_back(tcp_sender); }
		else std::cerr << "could not open the TUIO/TCP socket on port " << _port << std::endl;
	}

	// additional TUIO/WEB sender (unless WebSocket is already the primary sender)
	if (_web && (_udp || _tcp)) {
		OscSender *web_sender = NULL;
		try { web_sender = new WebSockSender(_web_port); }
		catch (std::exception e) { web_sender = NULL; }
		if (web_sender) { server->addOscSender(web_sender); senders.push_back(web_sender); }
		else std::cerr << "could not open the TUIO/WEB socket on port " << _web_port << std::endl;
	}

	std::vector<TuioScenario*> scenarios = getTuioScenarios();
	TuioScenario *scenario = findTuioScenario(_scenario.c_str());
	if (!scenario) {
		// printed to stdout so it stays ordered with the scenario list
		std::cout << "unknown scenario '" << _scenario << "'" << std::endl;
		show_scenarios();
		delete server;
		return 1;
	}

	signal(SIGINT,sighandler);

	TuioTime::initSession();
	std::cout << "TuioAudit scenario '" << scenario->name() << "' at " << _fps << " fps" << std::endl;

	std::vector<TuioScenario*>::iterator current = scenarios.begin();
	float scenario_t = 0.0f, cycle_t = 0.0f;
	bool start_pending = true;

	while (running) {
		TuioTime frameTime = TuioTime::getSessionTime();
		server->initFrame(frameTime);

		float dt = 1.0f/_fps;

		// adds and removals need the current frame time to be emitted,
		// so scenario transitions happen inside the frame
		if (start_pending) {
			scenario->start(server);
			start_pending = false;
		} else if (_cycle>0 && cycle_t>=_cycle) {
			scenario->stop(server);
			current++;
			if (current==scenarios.end()) current = scenarios.begin();
			scenario = *current;
			std::cout << "scenario '" << scenario->name() << "'" << std::endl;
			scenario->start(server);
			cycle_t = 0.0f;
			scenario_t = 0.0f;
		}

		// raw scenarios emit hand-crafted packets through the senders
		// directly, bypassing the TuioServer's well-formed bundles
		if (scenario->raw()) scenario->updateRaw(senders,scenario_t,dt);
		else scenario->update(server,scenario_t,dt);
		scenario_t += dt;
		cycle_t += dt;

		server->stopUntouchedMovingObjects();
		server->stopUntouchedMovingCursors();
		server->stopUntouchedMovingBlobs();
		server->commitFrame();

		// maintain the frame rate compensating the previous processing time
		int delay = (int)(1000.0f/_fps) - (TuioTime::getSessionTime().getTotalMilliseconds() - frameTime.getTotalMilliseconds());
		if (delay>0) TUIO::sleepMs(delay);
	}

	delete server;
	return 0;
}

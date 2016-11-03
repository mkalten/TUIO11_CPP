PLATFORM=$(shell uname)
ENDIANESS=OSC_HOST_LITTLE_ENDIAN

LD_FLAGS = -lGL -lGLU -lglut

TUIO_DEMO = TuioDemo
TUIO_DUMP = TuioDump
SIMPLE_SIMULATOR = SimpleSimulator
TUIO_STATIC  = libTUIO.a
TUIO_SHARED  = libTUIO.so

SDL_LDFLAGS := $(shell sdl2-config --libs)
SDL_CFLAGS  := $(shell sdl2-config --cflags)

INCLUDES = -I./TUIO -I./oscpack
#CFLAGS  = -g -Wall -O3 -fPIC $(SDL_CFLAGS)
CFLAGS  = -w -O3 -fPIC $(SDL_CFLAGS)
CXXFLAGS = $(CFLAGS) $(INCLUDES) -D$(ENDIANESS)
SHARED_OPTIONS = -shared -Wl,-soname,$(TUIO_SHARED)

ifeq ($(PLATFORM), Darwin)
	CC = gcc
	CXX = g++ -stdlib=libstdc++
	CFLAGS += -mmacosx-version-min=10.6 -arch=i386  -arch x86_64
	CXXFLAGS += -mmacosx-version-min=10.6 -arch=i386 -arch x86_64
#	CXX = g++ -stdlib=libc++
#	CFLAGS += -mmacosx-version-min=10.9 -arch x86_64
#	CXXFLAGS += -mmacosx-version-min=10.9 -arch x86_64
	TUIO_SHARED  = libTUIO.dylib
	LD_FLAGS =  -framework OpenGL -framework GLUT -framework SDL2 -framework Cocoa
 	SHARED_OPTIONS = -dynamiclib -Wl,-dylib_install_name,$(TUIO_SHARED)
	SDL_LDFLAGS =
endif

%.o: %.cpp
	@echo [CXX] $@
	@ $(CXX) $(CXXFLAGS) -o $@ -c $<
%.o: %.m
	@echo [CC] $@
	@ $(CC) $(CFLAGS) -o $@ -c $<

DEMO_SOURCES = TuioDemo.cpp
DEMO_OBJECTS = TuioDemo.o
DUMP_SOURCES = TuioDump.cpp
DUMP_OBJECTS = TuioDump.o
SIMULATOR_SOURCES = SimpleSimulator.cpp
SIMULATOR_OBJECTS = SimpleSimulator.o

COMMON_TUIO_SOURCES = ./TUIO/TuioTime.cpp ./TUIO/TuioPoint.cpp ./TUIO/TuioContainer.cpp ./TUIO/TuioObject.cpp ./TUIO/TuioCursor.cpp ./TUIO/TuioBlob.cpp ./TUIO/TuioDispatcher.cpp ./TUIO/TuioManager.cpp  ./TUIO/OneEuroFilter.cpp
SERVER_TUIO_SOURCES = ./TUIO/TuioServer.cpp ./TUIO/UdpSender.cpp ./TUIO/TcpSender.cpp ./TUIO/WebSockSender.cpp ./TUIO/FlashSender.cpp
CLIENT_TUIO_SOURCES = ./TUIO/TuioClient.cpp ./TUIO/OscReceiver.cpp ./TUIO/UdpReceiver.cpp ./TUIO/TcpReceiver.cpp
OSC_SOURCES = ./oscpack/osc/OscTypes.cpp ./oscpack/osc/OscOutboundPacketStream.cpp ./oscpack/osc/OscReceivedElements.cpp ./oscpack/osc/OscPrintReceivedElements.cpp ./oscpack/ip/posix/NetworkingUtils.cpp ./oscpack/ip/posix/UdpSocket.cpp

COMMON_TUIO_OBJECTS = $(COMMON_TUIO_SOURCES:.cpp=.o)
SERVER_TUIO_OBJECTS = $(SERVER_TUIO_SOURCES:.cpp=.o)
CLIENT_TUIO_OBJECTS = $(CLIENT_TUIO_SOURCES:.cpp=.o)
OSC_OBJECTS = $(OSC_SOURCES:.cpp=.o)

all: $(TUIO_DUMP) $(TUIO_DEMO) $(SIMPLE_SIMULATOR) $(TUIO_STATIC) $(TUIO_SHARED)

$(TUIO_STATIC):	$(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS)
	@echo [LD] $(TUIO_STATIC)
	@ ar rcs $@ $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS)

$(TUIO_SHARED): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS)
	@echo [LD] $(TUIO_SHARED)
	@ $(CXX) -o $@ $+ -lpthread $(SHARED_OPTIONS)

$(TUIO_DUMP): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(OSC_OBJECTS) $(DUMP_OBJECTS)
	@echo [LD] $(TUIO_DUMP)
	@ $(CXX) -o $@ $+ -lpthread

$(TUIO_DEMO): $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(OSC_OBJECTS) $(DEMO_OBJECTS)
	@echo [LD] $(TUIO_DEMO)
	@ $(CXX) -o $@ $+ -lpthread $(SDL_LDFLAGS) $(LD_FLAGS)

$(SIMPLE_SIMULATOR): $(COMMON_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS) $(SIMULATOR_OBJECTS)
	@echo [LD] $(SIMPLE_SIMULATOR)
	@ $(CXX) -o $@ $+ -lpthread $(SDL_LDFLAGS) $(LD_FLAGS)

clean:
	@echo [CLEAN] $(TUIO_DUMP) $(TUIO_DEMO) $(SIMPLE_SIMULATOR) $(TUIO_STATIC) $(TUIO_SHARED)
	@ rm -f $(TUIO_DUMP) $(TUIO_DEMO) $(SIMPLE_SIMULATOR) $(TUIO_STATIC) $(TUIO_SHARED)
	@ rm -f $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS) $(DUMP_OBJECTS) $(DEMO_OBJECTS) $(SIMULATOR_OBJECTS)

cmake_minimum_required (VERSION 2.6)
set(CMAKE_CONFIGURATION_TYPES Release Debug)
project (TUIO)

include_directories(oscpack)
include_directories(TUIO)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
endif()

set(WITH_TUIO_DUMP OFF CACHE BOOL "With the tuio dump console")
set(WITH_TUIO_DEMO OFF CACHE BOOL "With the tuio demo windows (not working yet)")
set(WITH_TUIO_SIMULATOR OFF CACHE BOOL "With the tuio simple simulator (not working yet)")

file(GLOB TUIO_SOURCES TUIO/*.cpp)
file(GLOB TUIO_HEADERS TUIO/*.h)
file(GLOB OSC_SOURCES 	oscpack/osc/*.cpp
						oscpack/ip/*.cpp)
file(GLOB_RECURSE OSC_HEADERS oscpack/osc/*.h)

# No ZERO_CHECK
set(CMAKE_SUPPRESS_REGENERATION true)

add_definitions(-DLIB_EXPORT)

IF(WIN32 OR WIN64)
  file(GLOB OSC_SOURCES_2 oscpack/ip/win32/*.cpp)
  add_definitions(-D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -DOSC_HOST_LITTLE_ENDIAN)
  #-D_DEBUG
ELSE()
	file(GLOB OSC_SOURCES_2	oscpack/ip/posix/*.cpp)
ENDIF()


add_library(libTUIO SHARED ${TUIO_SOURCES} ${OSC_SOURCES} ${OSC_SOURCES_2} ${TUIO_HEADERS} ${OSC_HEADERS})

if(WIN32 OR WIN64)
	target_link_libraries(libTUIO ws2_32 winmm)
endif()

IF(WITH_TUIO_DUMP)
    file(GLOB TUIO_DUMP TuioDump.cpp 
                    TuioDump.h)

    add_executable(TuioDump ${TUIO_DUMP} ${TUIO_HEADERS} ${OSC_HEADERS})

    target_link_libraries(TuioDump libTUIO)
ENDIF()


IF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    SET(PLATFORM x64)
ELSE("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
    SET(PLATFORM Win32)
ENDIF()

IF(WITH_TUIO_DEMO)
    file(GLOB TUIO_DEMO TuioDemo.cpp 
                    TuioDemo.h)
    IF(WIN32)
        file(GLOB_RECURSE TUIO_SDL_SOURCES windows/sdl/*.h)
        include_directories(windows/sdl/include)

        find_library(LIB_SDL2 SDL2 ./windows/sdl/${PLATFORM})
        find_library(LIB_FREEGLUT freeglut ./windows/sdl/${PLATFORM})
        find_library(LIB_SDL2MAIN SDL2main ./windows/sdl/${PLATFORM})
        set(LIB_GL opengl32)
        set(LIB_GLU glu32)
    ELSE()
        # i think there is a bit to fix for unix and darwin systems
        set(LIB_GL GL)
        set(LIB_FREEGLUT glut)
        set(LIB_GLU GLU)
    ENDIF()
    
    CONFIGURE_FILE(TuioDemo.vcxproj.user.init TuioDemo.vcxproj.user @ONLY)
    add_executable(TuioDemo ${TUIO_DEMO} ${TUIO_HEADERS} ${OSC_HEADERS} ${TUIO_SDL_SOURCES})

    target_link_libraries(TuioDemo libTUIO ${LIB_SDL2} ${LIB_FREEGLUT} ${LIB_SDL2MAIN} ${LIB_GL} ${LIB_GLU})
ENDIF()


#not checked, i think it will not work
IF(WITH_TUIO_SIMULATOR)
    file(GLOB TUIO_SIMULATOR} SimpleSimulator.cpp 
                    SimpleSimulator.h)
    add_executable(SimpleSimulator ${TUIO_SIMULATOR} ${TUIO_HEADERS} ${OSC_HEADERS})
    target_link_libraries(SimpleSimulator libTUIO)
ENDIF()


#set_target_properties(libTUIO PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
    
    


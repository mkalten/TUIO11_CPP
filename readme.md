# TUIO C++ Reference Implementation and Demo Applications

Copyright (c) 2005-2017 [Martin Kaltenbrunner](http://modin.yuri.at/). 
This software is part of 
[reacTIVision](http://reactivision.sourceforge.net/), an open source 
fiducial tracking and multi-touch framework based on computer vision. 
For further information on the TUIO protocol and framework, please visit 
[TUIO.org](http://www.tuio.org/)

## Demo Applications:

This package contains two demo applications which are able to receive 
TUIO messages from any TUIO enabled multitouch or object tracking 
application.

*   **TuioDump**: prints the TUIO events directly to the console
*   **TuioDemo**: graphically displays the object and cursor states on 
the screen
*   **SimpleSimulator**: a simple TUIO simulator that demonstrates the 
use of the TuioServer class

You can use these TUIO client and server demo applications for debugging 
purposes, and using them as a starting point for the development of you 
own open source C++ applications implementing the TUIO protocol.

Pressing F1 will toggle FullScreen mode within the TuioDemo,  
pressing ESC or closing the Window will end the application.  
Hitting the V key will print the received TUIO events to the console.

Dragging the mouse will generate TUIO cursor events within the 
SimpleSimulator.  
Pressing the SHIFT key while clicking, will produce "sticky" cursors.  
Pressing the CTRL key while clicking, will create "joint" cursors.  
Hitting the V key will print the generated TUIO events to the console.  
Hitting the R key will reset the SimpleSimulator.

You can switch to other TUIO types by hitting number :
0 : TuioCursor;
1 : TuioObject;
2 : TuioBlob;
3 : TuioCursor25D;
4 : TuioObject25D;
5 : TuioBlob25D;
6 : TuioCursor3D;
7 : TuioObject3D;
8 : TuioBlob3D.

As the SimpleSimulatro demo is a 2D application, is it diffcult to test rotations, z, width ... You can modify them by scrolling in different control mode. Each control mode change a different parameter. You can switch control mode by hitting number on the NumPad :
0 : Z;
1 : Roll;
2 : Pitch;
3 : Yaw;
4 : Width;
5 : Height;
6 : Depth;

Keep in mind to make your graphics scalable for the varying screen and 
window resolutions. A reasonable TUIO application will run in fullscreen 
mode, although the windowed mode might be useful for debugging purposes 
or when working with the Simulator.

## Application Programming Interface:

First you need to create an instance of **TuioClient**. This class is 
listening to TUIO messages on the specified port and generates higher 
level messages based on the object events. The method **connect(true)** 
will start the TuioClient in a blocking mode, simply calling 
**connect()** will start the TuioClient in the background. Call 
**disconnect()** in order to stop listening to incoming TUIO messages.

Your application needs to implement the **TuioListener** interface, and 
has to be added to the TuioClient in order to receive TUIO callback 
messages.

A TuioListener needs to implement the following methods:

*   **addTuioObject(TuioObject \*tobj)** this is called when an object 
becomes visible
*   **removeTuioObject(TuioObject \*tobj)** an object was removed from 
the surface
*   **updateTuioObject(TuioObject \*tobj)** an object was moved on the 
table surface

*   **addTuioCursor(TuioCursor \*tcur)** this is called when a new cursor 
is detected
*   **removeTuioCursor(TuioCursor \*tcur)** a cursor was removed from the 
surface
*   **updateTuioCursor(TuioCursor \*tcur)** a cursor was moving on the 
table surface

*   **addTuioBlob(TuioBlob \*tblb)** this is called when a new blob is 
detected
*   **removeTuioBlob(TuioBlob \*tblb)** a blob was removed from the 
surface
*   **updateTuioBlob(TuioBlob \*tblb)** a cursor was moving on the table 
surface

*   **addTuioObject25D(TuioObject25D \*tobj)** this is called when a 25Dobject 
becomes visible
*   **removeTuioObject25D(TuioObject25D \*tobj)** a 25Dobject was removed from 
the surface
*   **updateTuioObject25D(TuioObject25D \*tobj)** a 25Dobject was moved on the 
table surface

*   **addTuioCursor25D(TuioCursor25D \*tcur)** this is called when a new 25Dcursor 
is detected
*   **removeTuioCursor25D(TuioCursor25D \*tcur)** a 25Dcursor was removed from the 
surface
*   **updateTuioCursor25D(TuioCursor25D \*tcur)** a 25Dcursor was moving on the 
table surface

*   **addTuioBlob25D(TuioBlob25D \*tblb)** this is called when a new 25Dblob is 
detected
*   **removeTuioBlob25D(TuioBlob25D \*tblb)** a 25Dblob was removed from the 
surface
*   **updateTuioBlob25D(TuioBlob25D \*tblb)** a 25Dcursor was moving on the table 
surface

*   **addTuioObject3D(TuioObject3D \*tobj)** this is called when a 3Dobject 
becomes visible
*   **removeTuioObject3D(TuioObject3D \*tobj)** a 3Dobject was removed from 
the surface
*   **updateTuioObject3D(TuioObject3D \*tobj)** a 3Dobject was moved on the 
table surface

*   **addTuioCursor3D(TuioCursor3D \*tcur)** this is called when a new 3Dcursor 
is detected
*   **removeTuioCursor3D(TuioCursor3D \*tcur)** a 3Dcursor was removed from the 
surface
*   **updateTuioCursor3D(TuioCursor3D \*tcur)** a 3Dcursor was moving on the 
table surface

*   **addTuioBlob3D(TuioBlob3D \*tblb)** this is called when a new 3Dblob is 
detected
*   **removeTuioBlob3D(TuioBlob3D \*tblb)** a 3Dblob was removed from the 
surface
*   **updateTuioBlob3D(TuioBlob3D \*tblb)** a 3Dblob was moving on the table 
surface

*   **refresh(TuioTime bundleTime)** this method is called after each 
bundle, use it to repaint your screen for example

Typically you will need just the following code to start with:

<pre>MyTuioListener listener; // defines a TuioListener
TuioClient client(port); // creates the TuioClient
client.addTuioListener(&listener); // registers the TuioListener
client.connect(); // starts the TuioClient
</pre>

Each object or cursor is identified with aunique session ID, that is 
maintained over its lifetime. Additionally each object carries fiducial 
ID that corresponds to its attached fiducial marker number. The finger 
ID of the cursor object is always a number in the range of all currently 
detected cursor blobs.

The **TuioObject**, **TuioCursor**, **TuioBlob**,**TuioObject25D**, **TuioCursor25D**, **TuioBlob25D**,**TuioObject3D**, **TuioCursor3D** and **TuioBlob3D** references are 
updated automatically by the TuioClient and are always referencing the 
same instance over the object lifetime. All the TuioObject, TuioCursor, TuioBlob, TuioObject25D, TuioCursor25D, TuioBlob25D, TuioObject3D, TuioCursor3D and TuioBlob3D attributes are encapsulated and can be accessed with 
methods such as **getX()**, **getY()** and **getAngle()** and so on. 
TuioObject, TuioCursor, TuioBlob, TuioObject25D, TuioCursor25D, TuioBlob25D, TuioObject3D, TuioCursor3D and TuioBlob3D also have some additional 
convenience methods for the calculation of distances and angles between 
objects. The **getPath()** method returns a Vector of TuioPoint 
representing the movement path of the object.

Alternatively the TuioClient class contains some methods for the polling 
of the current object and cursor state. There are methods which return 
either a list or individual object and cursor objects. The TuioObject, TuioCursor, TuioBlob, TuioObject25D, TuioCursor25D, TuioBlob25D, TuioObject3D, TuioCursor3D and TuioBlob3D classes have been added as a container which 
also can be usedby external classes.

*   **getTuioObjects()** returns a Vector<TuioObject*> of all currently 
present TuioObjects
*   **getTuioCursors()** returns a Vector<TuioCursor*> of all currently 
present TuioCursors
*   **getTuioBlobs()** returns a Vector<TuioBlob*> of all currently 
present TuioCursors

*   **getTuioObject(long s_id)** returns a TuioObject* or NULL depending 
on its presence
*   **getTuioCursor(long s_id)** returns a TuioCursor* or NULL depending 
on its presence
*   **getTuioBlob(long s_id)** returns a TuioBlob* or NULL depending on 
its presence


*   **getTuioObjects25D()** returns a Vector<TuioObject25D*> of all currently 
present TuioObjects25D
*   **getTuioCursors25D()** returns a Vector<TuioCursor25D*> of all currently 
present TuioCursors25D
*   **getTuioBlobs25D()** returns a Vector<TuioBlob25D*> of all currently 
present TuioCursors25D

*   **getTuioObject25D(long s_id)** returns a TuioObject25D* or NULL depending 
on its presence
*   **getTuioCursor25D(long s_id)** returns a TuioCursor25D* or NULL depending 
on its presence
*   **getTuioBlob25D(long s_id)** returns a TuioBlob25D* or NULL depending on 
its presence


*   **getTuioObjects3D()** returns a Vector<TuioObject3D*> of all currently 
present TuioObjects3D
*   **getTuioCursors3D()** returns a Vector<TuioCursor3D*> of all currently 
present TuioCursors3D
*   **getTuioBlobs3D()** returns a Vector<TuioBlob3D*> of all currently 
present TuioCursors3D

*   **getTuioObject3D(long s_id)** returns a TuioObject* or NULL depending 
on its presence
*   **getTuioCursor3D(long s_id)** returns a TuioCursor* or NULL depending 
on its presence
*   **getTuioBlob3D(long s_id)** returns a TuioBlob* or NULL depending on 
its presence


## Building the Examples:

This package includes project files for Visual Studio and XCode as well 
as a simple Linux Makefile for building the example applications. The 
Win32 project already include the necessary libraries, to build the GUI 
example on Linux make sure you have the SDL2, OpenGL and GLUT libraries 
and headers installed on your system, the latest version 2.0.5 is 
recommended. On Mac OS X you need to install the SDL2 Framework to 
/Library/Frameworks.

## License:

This library is free software; you can redistribute it and/or modify it 
under the terms of the GNU Lesser General Public License as published by 
the Free Software Foundation; either version 3.0 of the License, or (at 
your option) any later version.

This library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License 
along with this library; if not, write to the Free Software Foundation, 
Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA

## References:

This package uses the [oscpack](http://www.rossbencina.com/code/oscpack) 
OpenSound Control library, and the [SDL](http://www.libsdl.org/) 
graphics library.


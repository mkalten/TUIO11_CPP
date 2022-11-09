/*
 TUIO C++ Library
 Copyright (c) 2022 Nicolas Bremard <nicolas.bremard@laposte.net>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library.
*/

#ifndef INCLUDED_TUIOLISTENER_H
#define INCLUDED_TUIOLISTENER_H

#include "TuioObject.h"
#include "TuioCursor.h"
#include "TuioBlob.h"

#include "TuioObject25D.h"
#include "TuioCursor25D.h"
#include "TuioBlob25D.h"

#include "TuioObject3D.h"
#include "TuioCursor3D.h"
#include "TuioBlob3D.h"

#include "TuioCustom.h"

namespace TUIO {
	
	/**
	 * <p>The TuioListener interface provides a simple callback infrastructure which is used by the {@link TuioClient} class 
	 * to dispatch TUIO events to all registered instances of classes that implement the TuioListener interface defined here.</p> 
	 * <p>Any class that implements the TuioListener interface is required to implement all of the callback methods defined here.
	 * The {@link TuioClient} makes use of these interface methods in order to dispatch TUIO events to all registered TuioListener implementations.</p>
	 * <p><code>
	 * public class MyTuioListener implements TuioListener<br/>
	 * ...</code><p><code>
	 * MyTuioListener listener = new MyTuioListener();<br/>
	 * TuioClient client = new TuioClient();<br/>
	 * client.addTuioListener(listener);<br/>
	 * client.start();<br/>
	 * </code></p>
	 *
	 * @author Nicolas Bremard
	 * @version 1.1.7
	 */
	class LIBDECL TuioListener { 
		
	public:
		/**
		 * The destructor is doing nothing in particular. 
		 */
		virtual ~TuioListener(){};
		

#pragma region region2D

		/**
		 * This callback method is invoked by the TuioClient when a new TuioObject is added to the session.   
		 *
		 * @param  tobj  the TuioObject reference associated to the addTuioObject event
		 */
		virtual void addTuioObject(TuioObject *tobj){}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioObject is updated during the session.   
		 *
		 * @param  tobj  the TuioObject reference associated to the updateTuioObject event
		 */
		virtual void updateTuioObject(TuioObject *tobj){}
		
		/**
		 * This callback method is invoked by the TuioClient when an existing TuioObject is removed from the session.   
		 *
		 * @param  tobj  the TuioObject reference associated to the removeTuioObject event
		 */
		virtual void removeTuioObject(TuioObject *tobj){}
		
		/**
		 * This callback method is invoked by the TuioClient when a new TuioCursor is added to the session.   
		 *
		 * @param  tcur  the TuioCursor reference associated to the addTuioCursor event
		 */
		virtual void addTuioCursor(TuioCursor *tcur){}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioCursor is updated during the session.   
		 *
		 * @param  tcur  the TuioCursor reference associated to the updateTuioCursor event
		 */
		virtual void updateTuioCursor(TuioCursor *tcur){}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioCursor is removed from the session.   
		 *
		 * @param  tcur  the TuioCursor reference associated to the removeTuioCursor event
		 */
		virtual void removeTuioCursor(TuioCursor *tcur){}

		/**
		 * This callback method is invoked by the TuioClient when a new TuioBlob is added to the session.   
		 *
		 * @param  tcur  the TuioBlob reference associated to the addTuioBlob event
		 */
		virtual void addTuioBlob(TuioBlob *tblb){}
		
		/**
		 * This callback method is invoked by the TuioClient when an existing TuioBlob is updated during the session.   
		 *
		 * @param  tblb  the TuioBlob reference associated to the updateTuioBlob event
		 */
		virtual void updateTuioBlob(TuioBlob *tblb){}
		
		/**
		 * This callback method is invoked by the TuioClient when an existing TuioBlob is removed from the session.   
		 *
		 * @param  tblb  the TuioBlob reference associated to the removeTuioBlob event
		 */
		virtual void removeTuioBlob(TuioBlob *tblb){}

#pragma endregion region2D

#pragma region region25D
			   
		/**
		 * This callback method is invoked by the TuioClient when a new TuioObject25D is added to the session.
		 *
		 * @param  tobj  the TuioObject25D reference associated to the addTuioObject25D event
		 */
		virtual void addTuioObject25D(TuioObject25D *tobj) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioObject25D is updated during the session.
		 *
		 * @param  tobj  the TuioObject25D reference associated to the updateTuioObject25D event
		 */
		virtual void updateTuioObject25D(TuioObject25D *tobj) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioObject25D is removed from the session.
		 *
		 * @param  tobj  the TuioObject25D reference associated to the removeTuioObject25D event
		 */
		virtual void removeTuioObject25D(TuioObject25D *tobj) {}

		/**
		 * This callback method is invoked by the TuioClient when a new TuioCursor25D is added to the session.
		 *
		 * @param  tcur  the TuioCursor25D reference associated to the addTuioCursor25D event
		 */
		virtual void addTuioCursor25D(TuioCursor25D *tcur) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioCursor25D is updated during the session.
		 *
		 * @param  tcur  the TuioCursor25D reference associated to the updateTuioCursor25D event
		 */
		virtual void updateTuioCursor25D(TuioCursor25D *tcur) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioCursor25D is removed from the session.
		 *
		 * @param  tcur  the TuioCursor25D reference associated to the removeTuioCursor25D event
		 */
		virtual void removeTuioCursor25D(TuioCursor25D *tcur) {}

		/**
		 * This callback method is invoked by the TuioClient when a new TuioBlob25D is added to the session.
		 *
		 * @param  tcur  the TuioBlob25D reference associated to the addTuioBlob25D event
		 */
		virtual void addTuioBlob25D(TuioBlob25D *tblb) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioBlob25D is updated during the session.
		 *
		 * @param  tblb  the TuioBlob25D reference associated to the updateTuioBlob25D event
		 */
		virtual void updateTuioBlob25D(TuioBlob25D *tblb) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioBlob25D is removed from the session.
		 *
		 * @param  tblb  the TuioBlob25D reference associated to the removeTuioBlob25D event
		 */
		virtual void removeTuioBlob25D(TuioBlob25D *tblb) {}
#pragma endregion region25D

#pragma region region3D

		/**
		 * This callback method is invoked by the TuioClient when a new TuioObject3D is added to the session.
		 *
		 * @param  tobj  the TuioObject3D reference associated to the addTuioObject3D event
		 */
		virtual void addTuioObject3D(TuioObject3D *tobj) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioObject3D is updated during the session.
		 *
		 * @param  tobj  the TuioObject3D reference associated to the updateTuioObject3D event
		 */
		virtual void updateTuioObject3D(TuioObject3D *tobj) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioObject3D is removed from the session.
		 *
		 * @param  tobj  the TuioObject3D reference associated to the removeTuioObject3D event
		 */
		virtual void removeTuioObject3D(TuioObject3D *tobj) {}

		/**
		 * This callback method is invoked by the TuioClient when a new TuioCursor3D is added to the session.
		 *
		 * @param  tcur  the TuioCursor3D reference associated to the addTuioCursor3D event
		 */
		virtual void addTuioCursor3D(TuioCursor3D *tcur) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioCursor3D is updated during the session.
		 *
		 * @param  tcur  the TuioCursor3D reference associated to the updateTuioCursor3D event
		 */
		virtual void updateTuioCursor3D(TuioCursor3D *tcur) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioCursor3D is removed from the session.
		 *
		 * @param  tcur  the TuioCursor3D reference associated to the removeTuioCursor3D event
		 */
		virtual void removeTuioCursor3D(TuioCursor3D *tcur) {}

		/**
		 * This callback method is invoked by the TuioClient when a new TuioBlob3D is added to the session.
		 *
		 * @param  tcur  the TuioBlob3D reference associated to the addTuioBlob3D event
		 */
		virtual void addTuioBlob3D(TuioBlob3D *tblb) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioBlob3D is updated during the session.
		 *
		 * @param  tblb  the TuioBlob3D reference associated to the updateTuioBlob3D event
		 */
		virtual void updateTuioBlob3D(TuioBlob3D *tblb) {}

		/**
		 * This callback method is invoked by the TuioClient when an existing TuioBlob3D is removed from the session.
		 *
		 * @param  tblb  the TuioBlob3D reference associated to the removeTuioBlob3D event
		 */
		virtual void removeTuioBlob3D(TuioBlob3D *tblb) {}
#pragma endregion region3D

		/**
		 * This callback method is invoked by the TuioClient when a new TuioObject is added to the session.
		 *
		 * @param  tobj  the TuioObject reference associated to the addTuioObject event
		 */
		virtual void customMessage(TuioCustom *tcus) {}

		
		/**
		 * This callback method is invoked by the TuioClient to mark the end of a received TUIO message bundle.   
		 *
		 * @param  ftime  the TuioTime associated to the current TUIO message bundle
		 */
		virtual void refresh(TuioTime ftime){}
	};
}
#endif /* INCLUDED_TUIOLISTENER_H */

/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    The event queue interface
 *
*/




#ifndef __MSHWEVENTQUEUE_H__
#define __MSHWEVENTQUEUE_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MShwEvent;

// CLASS DECLARATION

/**
 * MShwEventQueue
 * Event queue, provides the API to send events to Event Observers
 */
class MShwEventQueue
	{
	protected:

		/**
		 * Destructor. Dont allow deleting objects through this interface
		 */
		virtual ~MShwEventQueue() {};

	public: // the API

		/**
		 * Send event to the event observers.
		 * @param aEvent, the event object that describes the incident
		 */
		virtual void SendEventL( MShwEvent* aEvent ) = 0;

	};

#endif // __MSHWEVENTQUEUE_H__

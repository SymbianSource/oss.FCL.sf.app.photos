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
* Description:    The event observer interface
 *
*/




#ifndef __MSHWEVENTOBSERVER_H__
#define __MSHWEVENTOBSERVER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// Forward declarations
class MShwEvent;

// CLASS DECLARATION

/**
 *  MShwEventObserver
 */
class MShwEventObserver
	{
	protected:
	
		/**
		 * Destructor. Dont allow deleting objects through this interface.
		 */
		virtual ~MShwEventObserver() {};

	public: // the API

		/**
		 * Get notification that an event has occurred.
		 * This method may leave, it is event queues task to handle errors.
		 * @param aEvent, the event object describing the insident.
		 */
		virtual void NotifyL( MShwEvent* aEvent ) = 0;

	};

#endif // __MShwEventObserver_H__

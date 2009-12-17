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
* Description:    The base class for event publishers
 *
*/




#ifndef __CSHWEVENTPUBLISHERBASE_H__
#define __CSHWEVENTPUBLISHERBASE_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "shweventpublisher.h"

// Forward declarations
class MShwEvent;
class MShwEventQueue;

// CLASS DECLARATION

/**
 * CShwEventPublisherBase
 * @author Kimmo Hoikka
 */
NONSHARABLE_CLASS( CShwEventPublisherBase ) : public CBase, public MShwEventPublisher
	{
	public: // Constructors and destructor

		/**
		 * Constructor
		 */
		CShwEventPublisherBase();

		/**
		 * Destructor.
		 */
		~CShwEventPublisherBase();

	public: // New API

		/**
		 * This method can be used by the deriving classes to send events to the queue
		 * @param aEvent, the event to be sent to the queue
		 */
		void SendEventL( MShwEvent* aEvent );
	
	public: // From MShwEventPublisher

		/** @see MShwEventPublisher::SetEventQueue */
		void SetEventQueue( MShwEventQueue* aQueue );

	private:

		/// Ref: The event queue
		MShwEventQueue* iQueue;

	};

#endif // __CSHWEVENTPUBLISHERBASE_H__

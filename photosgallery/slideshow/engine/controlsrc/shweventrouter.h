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
* Description:    The event router for the slideshow engine
 *
*/




#ifndef __CSHWEVENTROUTER_H__
#define __CSHWEVENTROUTER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "shweventqueue.h"

// FORWARD DECLARATIONS
class MShwEventObserver;
class MShwEventPublisher;
class MShwEvent;

// CLASS DECLARATION

/**
 *  CShwEventRouter
 *
 * @lib shwslideshowengine.lib
 */
NONSHARABLE_CLASS( CShwEventRouter ) : public CBase, public MShwEventQueue
	{
	public: // Constructors and destructor

		/**
		 * Constructor
		 */
		static CShwEventRouter* NewL();

		/**
		 * Destructor.
		 */
		~CShwEventRouter();

	private:

		/**
		 * Constructor
		 */
		CShwEventRouter();
		
		/**
		 * 2nd stage constructor
		 */
		void ConstructL();

	public: // the API

		/**
		 * Add to the list of observers. Conveniency overload
		 * @param aObservers, list containing the observers to add.
		 */
		void AddObserversL( TArray<MShwEventObserver*> aObservers );

		/**
		 * Add to the list of observers.
		 * @param aObserver, the observer to add.
		 */
		void AddObserverL( MShwEventObserver* aObserver );

		/**
		 * Add a list of publisher to the system. Conveniency overload
		 * @param aPublisher, list contanining the producers to add
		 */
		void AddProducers( TArray< MShwEventPublisher* > aPublishers );

		/**
		 * Add a publisher to the system.
		 * @param aPublisher, the producer to add
		 */
		void AddProducer( MShwEventPublisher* aPublisher );

	public: // From MShwEventQueue
		
		/** @ref MShwEventQueue::SendEvent */
		void SendEventL( MShwEvent* aEvent );
		
	private:

		/// Own: The container for the event observers
		RPointerArray< MShwEventObserver > iObservers;

		/// Own: The queue for the events
		RArray< MShwEvent* > iEvents;
		
		/// Own: Flag to tell when an event is beeing served
		TBool iInsideEventLoop;

	};

#endif // __CSHWEVENTROUTER_H__

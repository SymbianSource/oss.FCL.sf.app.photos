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
* Description:    The scheduler for the slideshow
 *
*/




#include "shweventrouter.h"
#include "shweventobserver.h"
#include "shweventpublisher.h"
#include "shwevent.h"
#include "shwslideshowenginepanic.h"

#include <glxlog.h>
#include <glxtracer.h>

// constants
namespace
	{
	// the typical amount of observers. In current design six, 
	// increase if there is more observers in future
	const TInt KEventObserverGranularity = 7;
	const TInt KMaxNumberOfConcurrentEvents = 10;
	}

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwEventRouter::CShwEventRouter()
	: iObservers( KEventObserverGranularity ), 
	iEvents( KMaxNumberOfConcurrentEvents )
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwEventRouter* CShwEventRouter::NewL()
	{
	TRACER("CShwEventRouter::NewL()");
	CShwEventRouter* self = new( ELeave ) CShwEventRouter;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwEventRouter::~CShwEventRouter()
	{
	TRACER("CShwEventRouter::~CShwEventRouter");
	GLX_LOG_INFO("CShwEventRouter::~CShwEventRouter");
	// need to cleanup cloned events if we did get run down
	if( iEvents.Count() > 0 )
		{
		for( int i = 0; i < iEvents.Count(); i++ )
			{
			// call the destructor
			delete iEvents[ i ];
			}
		}
	// close the event array itself, this frees the memory
	iEvents.Close();
	// close the observer array itself, this frees the memory
	// NOTE! we did not have ownership of the observers
	iObservers.Close();
	}

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwEventRouter::ConstructL()
	{
	TRACER("CShwEventRouter::ConstructL");
	GLX_LOG_INFO("CShwEventRouter::ConstructL");
	// reserve space for the events so we should never leave in event handling
	iEvents.ReserveL( KMaxNumberOfConcurrentEvents );
	iInsideEventLoop = EFalse;
	}

// -----------------------------------------------------------------------------
// AddObserversL.
// -----------------------------------------------------------------------------
void CShwEventRouter::AddObserversL( TArray<MShwEventObserver*> aObservers )
	{
	TRACER("CShwEventRouter::AddObserversL");
    GLX_LOG_INFO( "CShwEventRouter::AddObserversL" );
	// add each observer
	for( TInt i = 0; i < aObservers.Count(); i++ )
		{
		AddObserverL( aObservers[ i ] );
		}
	}

// -----------------------------------------------------------------------------
// AddObserverL.
// -----------------------------------------------------------------------------
void CShwEventRouter::AddObserverL( MShwEventObserver* aObserver )
	{
	TRACER("CShwEventRouter::AddObserverL( MShwEventObserver* aObserver )");
    GLX_LOG_INFO("CShwEventRouter::AddObserverL(MShwEventObserver* aObserver)");
	__ASSERT_DEBUG( aObserver, User::Panic( _L("Null observer not allowed"), 1 ) );
	iObservers.AppendL( aObserver );
	}

// -----------------------------------------------------------------------------
// AddProducers.
// -----------------------------------------------------------------------------
void CShwEventRouter::AddProducers( TArray< MShwEventPublisher* > aPublishers )
	{
	TRACER("CShwEventRouter::AddProducers");
    GLX_LOG_INFO( "CShwEventRouter::AddProducers" );
	// add each publisher
	for( TInt i = 0; i < aPublishers.Count(); i++ )
		{
		AddProducer( aPublishers[ i ] );
		}
	}

// -----------------------------------------------------------------------------
// AddProducer.
// -----------------------------------------------------------------------------
void CShwEventRouter::AddProducer( MShwEventPublisher* aPublisher )
	{
	TRACER("CShwEventRouter::AddProducers(MShwEventPublisher* aPublisher)");
    GLX_LOG_INFO( "CShwEventRouter::AddProducer" );
	// give the publisher the event queue so they can start calling us
	aPublisher->SetEventQueue( this );
	}

// -----------------------------------------------------------------------------
// SendEventL. The method that handles the event dispatching.
// The events are dispatched in same order as they are sent, so if an event 
// is sent while previous is beeing dispatched we queue the event and 
// send it after the previous is dispatched to all observers
// -----------------------------------------------------------------------------
void CShwEventRouter::SendEventL( MShwEvent* aEvent )
	{
	TRACER("CShwEventRouter::SendEventL( MShwEvent* aEvent )");
    GLX_LOG_INFO( "CShwEventRouter::SendEvent" );
	__ASSERT_DEBUG( aEvent, User::Panic( _L("Null event not allowed"), 2 ) );

	// add the event to the queue
	// need to clone the event as the calling code may get out of scope
	// before we have time to deliver the event
	// this should never leave as we have reserved space in the array
	MShwEvent* clone = aEvent->CloneLC();
	iEvents.AppendL( clone );
	// we take ownership of the clone so pop it off the stack
	CleanupStack::Pop( clone );
	
	// are we inside event loop: when an event is beeing sent we may be 
	// delivering an earlier event
	if( iInsideEventLoop )
		{
		// there is an event beeing served so queue the event and return
		// the event will be served once previous one is served
		return;
		}
	// ok, dwell into the event serving loop
	// when we fall here we are just about to serve the one event
	// we may get new events while processing the old ones however
	iInsideEventLoop = ETrue;
	do
		{
		// take the first event
		MShwEvent* event = iEvents[ 0 ];
		TInt i = 0;
		// notify all observers
		for( ; i < iObservers.Count(); i++ )
			{
			iObservers[ i ]->NotifyL( event );
			}
		// this event is served, remove it and delete it
		iEvents.Remove( 0 );
		// delete the clone
		delete event;
		// if there are new events, deliver them as well
		}
	while( iEvents.Count() > 0 );
	// done processing events, safe to enter the loop again
	iInsideEventLoop = EFalse;
	}


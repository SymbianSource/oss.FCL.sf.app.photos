/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Test for scheduler for the slideshow
 *
*/




//  CLASS HEADER
#include "T_CShwEventRouter.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>

//  INTERNAL INCLUDES
#include "shweventrouter.h"
#include "shwevent.h"
#include "shwslideshowenginepanic.h"

// test state flags
TBool gT_TestEventDestructorCalled = EFalse;
TBool gT_TestEventCloneLCCalled = EFalse;
TBool gT_PanicCalled = EFalse;

// A test event class
class T_TestEvent : public MShwEvent
	{
	public:
		~T_TestEvent()
			{
			gT_TestEventDestructorCalled = ETrue;
			}
		MShwEvent* CloneLC()
			{
			gT_TestEventCloneLCCalled = ETrue;
			MShwEvent* tmp = new( ELeave ) T_TestEvent;
			CleanupStack::PushL( tmp );
			return tmp;
			}

		/**
		 * Helper to check that the event is of this class type
		 */
		TBool IsOfSameClass( MShwEvent* aLhs )
			{
			// here we have to compare agains NULL as TBool cannot be converted from a pointer
			return dynamic_cast<T_TestEvent*>( aLhs ) != NULL;
			}
	};

// CONSTRUCTION
T_CShwEventRouter* T_CShwEventRouter::NewL()
	{
	T_CShwEventRouter* self = T_CShwEventRouter::NewLC();
	CleanupStack::Pop( self );
	
	return self;
	}

T_CShwEventRouter* T_CShwEventRouter::NewLC()
	{
	T_CShwEventRouter* self = new( ELeave ) T_CShwEventRouter();
	CleanupStack::PushL( self );
	
	self->ConstructL();
	
	return self;
	}

// Destructor (virtual by CBase)
T_CShwEventRouter::~T_CShwEventRouter()
	{
	}

// Default constructor
T_CShwEventRouter::T_CShwEventRouter()
	{
	}

// Second phase construct
void T_CShwEventRouter::ConstructL()
	{
	// The ConstructL from the base class CEUnitTestSuiteClass must be called.
	// It generates the test case table.
	CEUnitTestSuiteClass::ConstructL();
	}

// METHODS

void T_CShwEventRouter::NotifyL( MShwEvent* aEvent )
	{
	// we got the event
	iEventReceiveCount++;
	// assert that the event was correct one
	EUNIT_ASSERT_DESC( iEventToReceive->IsOfSameClass( aEvent ), "Check that the event class is correct" );
	// for OOM testing, lets make a memory allocation
	TInt* intti = new( ELeave ) TInt;
	delete intti;
	// did the test want us to leave
	if( iLeaveInNotify )
		{
		User::Leave( KErrCorrupt );
		}
	// did the test want us to send new events
	if( iSendNewEventCount-- > 0 )
		{
		// allocate new event
		T_TestEvent testEvent2;
		// set the event object pointer so that the notify can verify the event object
		iEventToReceive = &testEvent2;
		// send the event,
		iQueue->SendEventL( &testEvent2 );
		}
	}
	
void T_CShwEventRouter::SetEventQueue( MShwEventQueue* aQueue )
	{
	iQueue = aQueue;
	}

void T_CShwEventRouter::SetupL()
	{
	// reset information flags
	gT_TestEventDestructorCalled = EFalse;
	gT_TestEventCloneLCCalled = EFalse;
	gT_PanicCalled = EFalse;
	// reset test state
	iLeaveInNotify = EFalse;
	iSendNewEventCount = 0;
	iEventReceiveCount = 0;
	// construct the router
	iRouter = CShwEventRouter::NewL();
	}

void T_CShwEventRouter::Teardown()
	{
	delete iRouter;
	iRouter = NULL;
	}

void T_CShwEventRouter::TestBasicRoutingL()
	{
	// add us as observer and producer
	iRouter->AddObserverL( this );
	iRouter->AddProducer( this );

	T_TestEvent testEvent;
	// set the event object pointer so that the notify can verify the event object
	iEventToReceive = &testEvent;
	// send the event,
	iQueue->SendEventL( &testEvent );
	// check we got an event
	EUNIT_ASSERT_EQUALS_DESC( iEventReceiveCount, 1, "one event expected" );
	// check that our event class methods were correctly called
	EUNIT_ASSERT_DESC( gT_TestEventCloneLCCalled, "check that clone was called" );
	EUNIT_ASSERT_DESC( gT_TestEventDestructorCalled, "destructor was called" );
	// there was no panic call
	EUNIT_ASSERT( !gT_PanicCalled );
	}

void T_CShwEventRouter::TestNestedEventsL()
	{
	// we want the notify to add 2 events during the notify calls so 3 events overall
	iSendNewEventCount = 2;
	// add us as observer and producer
	iRouter->AddObserverL( this );
	iRouter->AddProducer( this );

	T_TestEvent testEvent;
	// set the event object pointer so that the notify can verify the event object
	iEventToReceive = &testEvent;
	// send the event,
	iQueue->SendEventL( &testEvent );
	// check we got events
	EUNIT_ASSERT_EQUALS_DESC( iEventReceiveCount, 3, "three events expected" );
	// check that our event class methods were correctly called
	EUNIT_ASSERT_DESC( gT_TestEventCloneLCCalled, "check that clone was called" );
	EUNIT_ASSERT_DESC( gT_TestEventDestructorCalled, "destructor was called" );
	// there was no panic call
	EUNIT_ASSERT( !gT_PanicCalled );
	}

void T_CShwEventRouter::TestNestedEventsAndMultipleObserversL()
	{
	// we want the notify to add 2 events during the notify calls so 3 events overall
	iSendNewEventCount = 2;
	// add us as observer twice
	RPointerArray<MShwEventObserver> observers;
	CleanupClosePushL( observers );
	observers.AppendL( this );
	observers.AppendL( this );
	// add the array of observers
	iRouter->AddObserversL( observers.Array() );
	CleanupStack::PopAndDestroy( observers );
	// add us as publisher
	RPointerArray<MShwEventPublisher> publisher;
	CleanupClosePushL( publisher );
	publisher.AppendL( this );
	iRouter->AddProducers( publisher.Array() );
	CleanupStack::PopAndDestroy( publisher );

	T_TestEvent testEvent;
	// set the event object pointer so that the notify can verify the event object
	iEventToReceive = &testEvent;
	// send the event,
	iQueue->SendEventL( &testEvent );
	// check we got events
	EUNIT_ASSERT_EQUALS_DESC( iEventReceiveCount, 6, "six events expected" );
	// check that our event class methods were correctly called
	EUNIT_ASSERT_DESC( gT_TestEventCloneLCCalled, "check that clone was called" );
	EUNIT_ASSERT_DESC( gT_TestEventDestructorCalled, "destructor was called" );
	// there was no panic call
	EUNIT_ASSERT( !gT_PanicCalled );
	}

void T_CShwEventRouter::TestLeaveInNotifyL()
	{
	// we want the notify to leave
	iLeaveInNotify = ETrue;
	// add us as observer and producer
	iRouter->AddObserverL( this );
	iRouter->AddProducer( this );
	
	T_TestEvent testEvent;
	// set the event object pointer so that the notify can verify the event object
	iEventToReceive = &testEvent;
	// send the event, TRAP since we need to get out in the event
	EUNIT_TRAP_EXCEPT_ALLOC_D( error, 
		{
		iQueue->SendEventL( &testEvent );
		} );
	// there was no panic call
	EUNIT_ASSERT( !gT_PanicCalled );
	EUNIT_ASSERT_EQUALS_DESC( error, KErrCorrupt, "test that leave was correct code" );

	// check we got an event
	EUNIT_ASSERT_EQUALS_DESC( iEventReceiveCount, 1, "one event expected" );
	EUNIT_ASSERT_DESC( gT_TestEventCloneLCCalled, "check that clone was called" );
	EUNIT_ASSERT_DESC( !gT_TestEventDestructorCalled, "destructor was not yet called" );
	
	// delete router
	Teardown();
	// now the event gets deleted
	EUNIT_ASSERT_DESC( gT_TestEventDestructorCalled, "destructor was called" );
	}

void T_CShwEventRouter::TestOOML()
	{
	// this test case is supposed to be run under alloc decorator
	// check that we got alloc decorator on
	if( !CEUnitTestCaseDecorator::ActiveTestCaseDecorator( KEUnitAllocTestCaseDecoratorName ) )
		{
		EUNIT_FAIL_TEST( "Test configuration failure, alloc decorator not on" );
		}
	
	// add us as observer and producer
	iRouter->AddObserverL( this );
	iRouter->AddProducer( this );
	
	T_TestEvent testEvent;
	// set the event object pointer so that the notify can verify the event object
	iEventToReceive = &testEvent;
	// send the event
	iQueue->SendEventL( &testEvent );
	
	// get the alloc failure count from the alloc decorator
	// in the first two alloc failures we dont get to the nofity method
	EUNIT_GET_ALLOC_DECORATOR_FAILCOUNT_D( failurecount );
	if( failurecount > 2 )
		{
		// ok we should get the event
		// check we got an event
		EUNIT_ASSERT_EQUALS_DESC( iEventReceiveCount, 1, "one event expected" );
		// check that if clone was called, then also destructor is called
		EUNIT_ASSERT_EQUALS( gT_TestEventDestructorCalled, gT_TestEventCloneLCCalled );
		}
	}
	
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    T_CShwEventRouter,
    "CShwEventRouter unit tests",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Testing basic functionality",
    "CShwEventRouter",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestBasicRoutingL, Teardown)

EUNIT_ALLOC_TEST(
    "Testing nested events",
    "CShwEventRouter",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestNestedEventsL, Teardown)

EUNIT_ALLOC_TEST(
    "Testing multiple observers",
    "CShwEventRouter",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestNestedEventsAndMultipleObserversL, Teardown)

EUNIT_ALLOC_TEST(
    "Testing error handling",
    "CShwEventRouter",
    "SendEvent",
    "ERRORHANDLING",
    SetupL, TestLeaveInNotifyL, Teardown)

EUNIT_ALLOC_TEST(
    "Testing error handling in OOM",
    "CShwEventRouter",
    "SendEvent",
    "ERRORHANDLING",
    SetupL, TestOOML, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE

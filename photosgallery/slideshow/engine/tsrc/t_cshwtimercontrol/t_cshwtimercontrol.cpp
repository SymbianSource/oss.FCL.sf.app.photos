/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Test for timer control for the slideshow
 *
*/



//  CLASS HEADER
#include "t_cshwtimercontrol.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>
#include <digia/eunit/EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "shwevent.h"
#include "shwautoptr.h"
#include "shwcallback.h"

// CONSTRUCTION
T_CShwTimerControl* T_CShwTimerControl::NewL()
    {
    T_CShwTimerControl* self = T_CShwTimerControl::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

T_CShwTimerControl* T_CShwTimerControl::NewLC()
    {
    T_CShwTimerControl* self = new( ELeave ) T_CShwTimerControl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
T_CShwTimerControl::~T_CShwTimerControl()
    {
    }

// Default constructor
T_CShwTimerControl::T_CShwTimerControl()
    {
    }

// Second phase construct
void T_CShwTimerControl::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void T_CShwTimerControl::SendEventL( MShwEvent* aEvent )
	{
	// need to clone the event since the caller goes out of scope
	iEvent = aEvent->CloneLC();
	CleanupStack::Pop( iEvent );
	
	// stop the scheduler loop if its started
	if( iWait.IsStarted() )
		{
		iWait.AsyncStop();
		}
	}

TInt T_CShwTimerControl::TimerCallBack()
	{
	iStoppedForTimer = ETrue;
	iWait.AsyncStop();
	return 0;
	}

void T_CShwTimerControl::SetupL()
    {
    iCShwTimerControl = CShwTimerControl::NewL();
	// set the event queue
	iCShwTimerControl->SetEventQueue( this );
    }

void T_CShwTimerControl::Teardown()
    {
    delete iCShwTimerControl;
    iCShwTimerControl = NULL;
    
    delete iEvent;
    iEvent = NULL;
    }

void T_CShwTimerControl::Empty()
	{
	}

void T_CShwTimerControl::TestConstructL()
	{
	// create timer
    iCShwTimerControl = CShwTimerControl::NewL();
	// set the event queue
	iCShwTimerControl->SetEventQueue( this );
	// test that object exists
	EUNIT_ASSERT_DESC( iCShwTimerControl, "object created" );
	// delete timer
    delete iCShwTimerControl;
    iCShwTimerControl = NULL;
    // EUnit checks for memory leaks
	}

void T_CShwTimerControl::T_TimeTickLL()
	{
	// call timetick
	iCShwTimerControl->SendTimerBeatL();
	// check that we got the event
	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventTimerBeat* timer = dynamic_cast<TShwEventTimerBeat*>( iEvent );
	EUNIT_ASSERT_DESC( timer, "event was TShwEventTimerBeat" );
	}

void T_CShwTimerControl::T_NotifyLL()
    {
    // if should evaluate true: else if( eventStartView )
    TShwEventStartView start( 123 );
    iCShwTimerControl->NotifyL( &start );

	// Need to let the scheduler loop
	iWait.Start();

	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventTimerBeat* timer = dynamic_cast<TShwEventTimerBeat*>( iEvent );
	EUNIT_ASSERT_DESC( timer, "event was TShwEventTimerBeat" );
    }
    
void T_CShwTimerControl::T_NotifyL2L()
    {
    // if should evaluate false: else if( eventStartView )
    MShwEvent* iParam1 = NULL;
    iCShwTimerControl->NotifyL( iParam1 );

	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }
    
void T_CShwTimerControl::T_PauseL()
	{
    // should evaluate true: else if( eventStartView )
    // NOTE timer multiplies this with 1000 so this is milliseconds
    TShwEventStartView start( 123 );
    iCShwTimerControl->NotifyL( &start );

	// then send Pause
	// should evaluate true: if ( eventPause )
	TShwEventPause pause;
    iCShwTimerControl->NotifyL( &pause );
	// send pause again, its a no-op
    iCShwTimerControl->NotifyL( &pause );

	// create timer to give us callback
	TShwAutoPtr< CPeriodic > timer = CPeriodic::NewL( CActive::EPriorityStandard );
	// start a timer to stop the asynch loop since we dont expect the 
	// iCShwTimerControl to send any event
	// start asynch wait for 1 second
	iStoppedForTimer = EFalse;
	timer->Start( 
		1 * 1000000, 
		1 * 1000000, 
		TShwCallBack< T_CShwTimerControl, TimerCallBack >( this ) );

	// Need to let the scheduler loop
	iWait.Start();
	timer->Cancel();
	
	EUNIT_ASSERT_DESC( iStoppedForTimer, "stopped in timer, SendEventL not called" );
	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );

	// send resume
	// should evaluate true: else if( eventResume )
	TShwEventResume resume;
    iCShwTimerControl->NotifyL( &resume );

	// start the timer again
	iStoppedForTimer = EFalse;
	timer->Start( 
		1 * 1000000, 
		1 * 1000000, 
		TShwCallBack< T_CShwTimerControl, TimerCallBack >( this ) );

	// Need to let the scheduler loop
	iWait.Start();
	timer->Cancel();

	EUNIT_ASSERT_DESC( !iStoppedForTimer, "SendEventL was called" );
	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTimerBeat*>( iEvent ), "event was timer beat" );
	// release the event
	delete iEvent; iEvent = NULL;

	// send resume again
	// should evaluate true: else if( eventResume )
	TShwEventResume resume2;
    iCShwTimerControl->NotifyL( &resume2 );
	// start the timer again
	iStoppedForTimer = EFalse;
	timer->Start( 
		1 * 1000000, 
		1 * 1000000, 
		TShwCallBack< T_CShwTimerControl, TimerCallBack >( this ) );
	// Need to let the scheduler loop
	iWait.Start();
	timer->Cancel();
	EUNIT_ASSERT_DESC( iStoppedForTimer, "stopped in timer, SendEventL not called" );
	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwTimerControl,
    "Test suite for CShwTimerControl",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Constructor test",
    "CShwTimerControl",
    "NewL",
    "FUNCTIONALITY",
    Empty, TestConstructL, Empty )

EUNIT_ALLOC_TEST(
    "TimeTickL - test0",
    "CShwTimerControl",
    "TimeTickL",
    "ERRORHANDLING",
    SetupL, T_TimeTickLL, Teardown)

// these cant be decorated as the EUnit
// scheduler does not implement Error method
EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test0",
    "CShwTimerControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyLL, Teardown)
    
EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test1",
    "CShwTimerControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyL2L, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "Pause test",
    "CShwTimerControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_PauseL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

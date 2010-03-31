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
* Description:   Test for view control for the slideshow
 *
*/




//  CLASS HEADER
#include "t_cshwviewcontrol.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>
#include <digia/eunit/EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "shwviewcontrol.h"
#include "shwevent.h"
#include "shwslideshowenginepanic.h"

TBool gPanicCalled;
namespace NShwEngine
	{
	extern void Panic( TShwEnginePanic aPanic )
	    {
	    gPanicCalled = ETrue;
	    // in test situation we just leave
	    User::Leave( aPanic );
	    }
	}

// CONSTRUCTION
T_CShwViewControl* T_CShwViewControl::NewL()
    {
    T_CShwViewControl* self = T_CShwViewControl::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

T_CShwViewControl* T_CShwViewControl::NewLC()
    {
    T_CShwViewControl* self = new( ELeave ) T_CShwViewControl();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CShwViewControl::~T_CShwViewControl()
    {
    Teardown();
    }

// Default constructor
T_CShwViewControl::T_CShwViewControl()
    {
    }

// Second phase construct
void T_CShwViewControl::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

void T_CShwViewControl::SendEventL( MShwEvent* aEvent )
	{
	// need to clone the event since the caller goes out of scope
	iEvent = aEvent->CloneLC();
	CleanupStack::Pop( iEvent );
	}

const TInt KTestTransitionDuration = 987;
const TInt KTestViewDuration = 678;

//  METHODS
TBool gMediaListCalled = EFalse;
void T_CShwViewControl::MGlxMediaList_MethodCalled( TMGlxMediaListMethodId /*aMethodId*/ )
	{
	// media list was called
	gMediaListCalled = ETrue;
	}

void T_CShwViewControl::SetupL()
	{
	iStubMediaList = new( ELeave ) TMGlxMediaList_Stub( this );

	// create viewcontrol
	iCShwViewControl = 
		CShwViewControl::NewL( 
			*iStubMediaList, KTestTransitionDuration, KTestViewDuration );
	// set the event queue
	iCShwViewControl->SetEventQueue( this );
	
	// reset flags
	gPanicCalled = EFalse;
	gMediaListCalled = EFalse;
	}

void T_CShwViewControl::Teardown()
	{
	// delete the object
	delete iCShwViewControl; 
	iCShwViewControl = NULL;
	
	delete iStubMediaList;
	iStubMediaList = NULL;
	
	// delete the event
	delete iEvent;
	iEvent = NULL;
	}

void T_CShwViewControl::TestNewlL()
	{
	}

void T_CShwViewControl::TestNotifyL1L()
	{
	// Path
	// true: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	TShwEventStart start;
	// call notify
	iCShwViewControl->NotifyL( &start );

	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventStartView* startview = dynamic_cast<TShwEventStartView*>( iEvent );
	EUNIT_ASSERT_DESC( startview, "event was TShwEventStartView" );
	EUNIT_ASSERT_EQUALS_DESC(
		startview->Parameter(), KTestViewDuration, "event had correct parameter" );
	}

void T_CShwViewControl::TestNotifyL2L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: if( iTransitionReadyReceived || iUserNavigated )
    TShwEventReadyToView ready2view;
    iCShwViewControl->NotifyL( &ready2view );

	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }

void T_CShwViewControl::TestNotifyL3L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	//  true: if( iTransitionReadyReceived || iUserNavigated )
    TShwEventTransitionReady trans;
    iCShwViewControl->NotifyL( &trans );

    TShwEventReadyToView ready2view;
    iCShwViewControl->NotifyL( &ready2view );

	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventStartView* startview = dynamic_cast<TShwEventStartView*>( iEvent );
	EUNIT_ASSERT_DESC( startview, "event was TShwEventStartView" );
	EUNIT_ASSERT_EQUALS_DESC( 
		startview->Parameter(), KTestViewDuration, "event had correct parameter" );
    }

void T_CShwViewControl::TestNotifyL4L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	// false: if( iReadyToViewReceived )
    TShwEventTransitionReady trans;
    iCShwViewControl->NotifyL( &trans );

	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }
    
void T_CShwViewControl::TestNotifyL5L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	//  true: if( iReadyToViewReceived )
    TShwEventReadyToView ready2view;
    iCShwViewControl->NotifyL( &ready2view );

    TShwEventTransitionReady trans;
    iCShwViewControl->NotifyL( &trans );

	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventStartView* startview = dynamic_cast<TShwEventStartView*>( iEvent );
	EUNIT_ASSERT_DESC( startview, "event was TShwEventStartView" );
	EUNIT_ASSERT_EQUALS_DESC( 
		startview->Parameter(), KTestViewDuration, "event had correct parameter" );
    }

void T_CShwViewControl::TestNotifyL6L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	//  true: 	 if( iTimerReceived && iReadyToAdvanceReceived && (!iPaused) && (iList.Count() > 1) )
	// call notify first with view ready
	TShwEventReadyToAdvance viewReady;
    iCShwViewControl->NotifyL( &viewReady );
	// then send timer beat
    TShwEventTimerBeat beat;
    iCShwViewControl->NotifyL( &beat );
	// validate
	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventStartTransition* startview = dynamic_cast<TShwEventStartTransition*>( iEvent );
	EUNIT_ASSERT_DESC( startview, "event was TShwEventStartTransition" );
	EUNIT_ASSERT_EQUALS_DESC( 
		startview->Parameter(), KTestTransitionDuration, "event had correct parameter" );
    }
    
void T_CShwViewControl::TestNotifyL7L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	//  false: 	 if( iTimerReceived && iReadyToAdvanceReceived && (!iPaused) && (iList.Count() > 1) )
	// send timer beat
    TShwEventTimerBeat beat;
    iCShwViewControl->NotifyL( &beat );
	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }

void T_CShwViewControl::TestNotifyL8L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	//  false: 	 if( iTimerReceived && iReadyToAdvanceReceived && (!iPaused) && (iList.Count() > 1) )
    // test iPaused
	// call notify first with view ready
	TShwEventReadyToAdvance viewReady;
    iCShwViewControl->NotifyL( &viewReady );
	// send pause
	TShwEventPause pause;
    iCShwViewControl->NotifyL( &pause );
	// then send timer beat
    TShwEventTimerBeat beat;
    iCShwViewControl->NotifyL( &beat );
	// validate
	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }
    
void T_CShwViewControl::TestNotifyL9L()
    {
	// Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	//  false: 	 if( iTimerReceived && iReadyToAdvanceReceived && (!iPaused) && (iList.Count() > 1) )
	// test iList.Count()
	// call notify first with view ready
	TShwEventReadyToAdvance viewReady;
    iCShwViewControl->NotifyL( &viewReady );
    // set list count to be one
    iStubMediaList->iCount = 1;
	// then send timer beat
    TShwEventTimerBeat beat;
    iCShwViewControl->NotifyL( &beat );
	// validate
	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }
    
void T_CShwViewControl::TestNotifyL10L()
    {
	// Path
	// all top level ifs are false
    iCShwViewControl->NotifyL( NULL );

	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }

void T_CShwViewControl::TestNotifyL11L()
    {
    // Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventReadyToAdvance* >( aEvent ) )
	//  true:	if( iTimerReceived && iReadyToAdvanceReceived && (!iPaused) && (iList.Count() > 1) )
    // iTimerReceived value should be ETrue
	// send timerbeat first
	TShwEventTimerBeat beat;
    iCShwViewControl->NotifyL( &beat );
    // then send the ready to advance
    TShwEventReadyToAdvance view_ready;
    iCShwViewControl->NotifyL( &view_ready );
	// validate
	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventStartTransition* start = dynamic_cast<TShwEventStartTransition*>( iEvent );
	EUNIT_ASSERT_DESC( start, "event was TShwEventStartTransition" );
	EUNIT_ASSERT_EQUALS_DESC(
		start->Parameter(), KTestTransitionDuration, "event had correct parameter" );
    }
    
void T_CShwViewControl::TestNotifyL12L()
    {
    // Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventReadyToAdvance* >( aEvent ) )
	//  true:	if( iTimerReceived && iReadyToAdvanceReceived && (!iPaused) && (iList.Count() > 1) )
    // iTimerReceived value should be EFalse
    // send the view ready
    TShwEventReadyToAdvance view_ready;
    iCShwViewControl->NotifyL( &view_ready );

	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }

void T_CShwViewControl::TestNotifyL13L()
    {
    // Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	//  true: else if( dynamic_cast< TShwEventReadyToAdvance* >( aEvent ) )
	//  true:	if( iTimerReceived && iReadyToAdvanceReceived && (!iPaused) && (iList.Count() > 1) )
    // iTimerReceived value should be ETrue, iPaused should be false
	// send timerbeat first
	TShwEventTimerBeat beat;
    iCShwViewControl->NotifyL( &beat );
	// send pause
	TShwEventPause pause;
    iCShwViewControl->NotifyL( &pause );
    // send the view ready
    TShwEventReadyToAdvance view_ready;
    iCShwViewControl->NotifyL( &view_ready );

	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );
    }

void T_CShwViewControl::TestNotifyL14L()
    {
    // test pause - usernavigate - resume
    // Path
	// false: if( dynamic_cast< TShwEventStart* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
	// false: else if( dynamic_cast< TShwEventReadyToAdvance* >( aEvent ) )
	//  true: else if ( dynamic_cast< TShwEventNextImage* >( aEvent )

	// send pause
	TShwEventPause pause;
    iCShwViewControl->NotifyL( &pause );

    // send the previous image
    TShwEventPreviousImage previous;
    iCShwViewControl->NotifyL( &previous );

	// validate that start view was not yet sent
	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );

	// send ready to view
	TShwEventReadyToView readyToView;
    iCShwViewControl->NotifyL( &readyToView );

	// validate that start view was sent
	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	TShwEventStartView* startview = dynamic_cast<TShwEventStartView*>( iEvent );
	EUNIT_ASSERT_DESC( startview, "event was TShwEventStartView" );
	EUNIT_ASSERT_EQUALS_DESC( startview->Parameter(), 
		KTestViewDuration, "event had correct parameter" );
	// release event
	delete iEvent;
	iEvent = NULL;

	// send resume
	TShwEventResume resume;
    iCShwViewControl->NotifyL( &resume );

	// validate that start view was sent again
	EUNIT_ASSERT_DESC( iEvent, "event was received" );
	startview = dynamic_cast<TShwEventStartView*>( iEvent );
	EUNIT_ASSERT_DESC( startview, "event was TShwEventStartView" );
	EUNIT_ASSERT_EQUALS_DESC( startview->Parameter(), 
		KTestViewDuration, "event had correct parameter" );
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwViewControl,
    "CShwViewControl test suite",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Constructor test",
    "CShwViewControl",
    "NewL",
    "FUNCTIONALITY",
    SetupL, TestNewlL, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 1",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL1L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 2",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL2L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 3",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL3L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 4",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL4L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 5",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL5L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 6",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL6L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 7",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL7L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 8",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL8L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 9",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL9L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 10",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL10L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 11",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL11L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 12",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL12L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 13",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL13L, Teardown )

EUNIT_ALLOC_TEST(
    "NotifyL - path 14",
    "CShwViewControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, TestNotifyL14L, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE

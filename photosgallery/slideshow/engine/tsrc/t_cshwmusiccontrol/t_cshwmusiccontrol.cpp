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
* Description:  Test for music control for the slideshow
 *
*/



//  CLASS HEADER
#include "t_cshwmusiccontrol.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "shwevent.h"

namespace
	{
	_LIT(KFileName, "c:\\knightrider.mp3");	
	}

// CONSTRUCTION
T_CShwMusicControl* T_CShwMusicControl::NewL()
    {
    T_CShwMusicControl* self = T_CShwMusicControl::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CShwMusicControl* T_CShwMusicControl::NewLC()
    {
    T_CShwMusicControl* self = new( ELeave ) T_CShwMusicControl;
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }



// Destructor (virtual by CBase)
T_CShwMusicControl::~T_CShwMusicControl()
    {
    }



// Default constructor
T_CShwMusicControl::T_CShwMusicControl() 
				   :iMusicOn(EFalse),
				    iCurrentVolume(KErrNotFound),
				    iMaxVolume(KErrNotFound),
				    iPrevVolume(KErrNotFound)
    {
    }

// Second phase construct
void T_CShwMusicControl::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

void T_CShwMusicControl::SendEventL(MShwEvent* aEvent)
	{
	iEvent = aEvent->CloneLC();
	CleanupStack::Pop();
	}

TBool gMusicOnCalled = EFalse;
void T_CShwMusicControl::MusicOnL()
	{
	EUNIT_PRINT(_L("Music is ON"));
	iMusicOn = ETrue;
	gMusicOnCalled = ETrue;
	// leave if error code set
    User::LeaveIfError( iLeaveInObserver );
	}
	
TBool gMusicOffCalled = EFalse;
void T_CShwMusicControl::MusicOff()
	{
	EUNIT_PRINT(_L("Music is OFF"));
	iMusicOn = EFalse;
	gMusicOffCalled = ETrue;
	}


void T_CShwMusicControl::MusicVolume(TInt aCurrentVolume, TInt aMaxVolume)
	{
	iPrevVolume    = iCurrentVolume;
	iCurrentVolume = aCurrentVolume;
	iMaxVolume     = aMaxVolume;
	EUNIT_PRINT(_L("current volume %d"),aCurrentVolume);
	if(iWait.IsStarted())
		{
		iWait.AsyncStop();
		}
	}

void T_CShwMusicControl::ErrorWithTrackL( TInt aErrorCode )
	{
	iErrorCode = aErrorCode;
	}

void T_CShwMusicControl::Empty()
	{
	}

void T_CShwMusicControl::SetupL()
    {
    // reset current and max volume
    iCurrentVolume = KErrNotFound;
    iMaxVolume = KErrNotFound;
    // reset error code
    iErrorCode = KErrNone;
    // reset leave flag
    iLeaveInObserver = KErrNone;
    // reset state flags
    gMusicOnCalled = EFalse;
    gMusicOffCalled = EFalse;
    
    iCShwMusicControl = CShwMusicControl::NewL( *this, KFileName );
	// set the event queue - inherited from parent CShwEventPublisherBase
	iCShwMusicControl->SetEventQueue(this);
    }


void T_CShwMusicControl::Teardown()
    {
    delete iCShwMusicControl;
    iCShwMusicControl = NULL;
    
    delete iEvent;
    iEvent = NULL;
    }

void T_CShwMusicControl::TestConstructL()
	{
	EUNIT_PRINT(_L("CShwMusicControl::NewL"));
	// create 
    iCShwMusicControl = CShwMusicControl::NewL(*this, KFileName);
	// set the event queue
	iCShwMusicControl->SetEventQueue(this);
	// test that object exists
	EUNIT_ASSERT_DESC(iCShwMusicControl, "object created");
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");
	// delete music control
    delete iCShwMusicControl;
    iCShwMusicControl = NULL;
    // EUnit checks for memory leaks
	}

void T_CShwMusicControl::T_NotifyLL()
    {
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");
    // send start event
    TShwEventStart start;
    iCShwMusicControl->NotifyL(&start);
	// Need to let the scheduler loop to get first volume callback
	iWait.Start(); // Wait for notification of volume
	EUNIT_ASSERT(iMusicOn == ETrue);
    EUNIT_ASSERT(iCurrentVolume != KErrNotFound && iMaxVolume != KErrNotFound);
    }

void T_CShwMusicControl::T_NotifyL1L()
    {
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");
    // if should evaluate false: if (event)
    MShwEvent* iParam1 = NULL;
    iCShwMusicControl->NotifyL(iParam1);

	EUNIT_ASSERT_DESC( !iEvent, "event was not received" );

    // test also HandlePropertyL with unsupported property
    iCShwMusicControl->HandlePropertyL( EPbPropertyMute, 0, 0 );
    // test that volume was not send
    EUNIT_ASSERT( iCurrentVolume == KErrNotFound && iMaxVolume == KErrNotFound );
    }
    
void T_CShwMusicControl::T_NotifyL2L()
    {
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");
    // send start event
    TShwEventStart start;
    iCShwMusicControl->NotifyL( &start );
	// Need to let the scheduler loop to get first volume callback
	iWait.Start();
	// check that we got the volume
    EUNIT_ASSERT(iCurrentVolume != KErrNotFound && iMaxVolume != KErrNotFound);

    // send pause
    TShwEventPause pause;
    iCShwMusicControl->NotifyL( &pause );
	EUNIT_ASSERT( iMusicOn == EFalse );

    // send resume
    TShwEventResume resume;
    iCShwMusicControl->NotifyL( &resume );

	EUNIT_ASSERT(iMusicOn != EFalse );
	}

void T_CShwMusicControl::T_NotifyL3L()
    {
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");
    // send resume before start
    TShwEventResume resume;
    iCShwMusicControl->NotifyL( &resume );
	EUNIT_ASSERT_DESC( iMusicOn == ETrue, "music is on" );

    // Send pause before start, does not pause
    TShwEventPause pause;
    iCShwMusicControl->NotifyL( &pause );
	EUNIT_ASSERT_DESC( iMusicOn == EFalse, "music is off" );

    // send start event
    TShwEventStart start;
    iCShwMusicControl->NotifyL( &start );
	// Need to let the scheduler loop to get first volume callback
	iWait.Start();

    // Second subsequent pause request 
    iCShwMusicControl->NotifyL( &pause );
	EUNIT_ASSERT_DESC( iMusicOn == EFalse, "music is now paused" );
	}

void T_CShwMusicControl::T_NotifyL4L()
    {
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");
    // send start event
    TShwEventStart start;
    iCShwMusicControl->NotifyL(&start);
	// Need to let the scheduler loop to get first volume callback
	iWait.Start();
	
    // if should evaluate false: if (event)
    TShwEventVolumeDown vol;
    iCShwMusicControl->NotifyL(&vol);

	// Need to let the scheduler loop to get another volume callback
	iWait.Start();
	EUNIT_ASSERT_GREATER_DESC( iPrevVolume, iCurrentVolume, "volume should decrease" );
	EUNIT_ASSERT_NOT_EQUALS( KErrNotFound, iMaxVolume );
	 
	iMaxVolume = KErrNotFound;
    }
    
void T_CShwMusicControl::T_NotifyL5L()
    {
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");
    // send start event
    TShwEventStart start;
    iCShwMusicControl->NotifyL(&start);
	// Need to let the scheduler loop to get first volume callback
	iWait.Start();
	
    // if should evaluate false: if (event)
    TShwEventVolumeUp vol;
    iCShwMusicControl->NotifyL(&vol);

	// Need to let the scheduler loop to get another volume callback
	iWait.Start();
	EUNIT_ASSERT_GREATER_DESC( iCurrentVolume, iPrevVolume, "volume should increase" );
	EUNIT_ASSERT_NOT_EQUALS( KErrNotFound, iMaxVolume );
	iMaxVolume = KErrNotFound;
	}
        
void T_CShwMusicControl::T_ExtendedPlayL()
    {
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNone, iErrorCode, "if there was error, make sure track file exists");

    TShwEventStart start;
    iCShwMusicControl->NotifyL( &start );
	
	if (!iTimer)
		{
		iTimer = CPeriodic::NewL(EPriorityNormal);
		}

    // play for 10 seconds
	const TInt KPlayPeriod = 10 * 1000000;    // Microseconds
	TCallBack callBack(StopPlaying, this);
	iTimer->Start(KPlayPeriod, KPlayPeriod, callBack);		
	
	iPlayWait.Start();
	
	delete iTimer;
    iTimer = NULL;
	
    TShwEventPause pause;
    iCShwMusicControl->NotifyL(&pause);
	EUNIT_ASSERT(iMusicOn == EFalse);

	}   

TInt T_CShwMusicControl::StopPlaying(TAny* aMusicControl)
    {
    T_CShwMusicControl* self = reinterpret_cast<T_CShwMusicControl*>(aMusicControl);

	if(self->iPlayWait.IsStarted())
		{
		self->iPlayWait.AsyncStop();
		}

    return KErrNone;
    }    

void T_CShwMusicControl::T_ErrorInFileL()
    {
    // need to reset state as the setup is empty
    gMusicOnCalled = EFalse;
    // file that does not exist
    _LIT( KErrorFileName, "C:\\juubaduuba.mp123" );
	// create 
    iCShwMusicControl = CShwMusicControl::NewL( *this, KErrorFileName );
	// set the event queue
	iCShwMusicControl->SetEventQueue( this );
	// test that object exists
	EUNIT_ASSERT_DESC(iCShwMusicControl, "object created");
	
	// test that error was called
	// test that no error
	EUNIT_ASSERT_EQUALS_DESC( 
	    KErrNotFound, iErrorCode, "track should not exist");

    // test that start is a no-op
    TShwEventStart start;
    iCShwMusicControl->NotifyL( &start );
	
	// test that music is off
    EUNIT_ASSERT_DESC( gMusicOnCalled == EFalse, "music on was not called" );

	// delete music control
    delete iCShwMusicControl;
    iCShwMusicControl = NULL;
    // EUnit checks for memory leaks
	}   

void T_CShwMusicControl::T_LeaveInObserverL()
    {
    // make observer leave
    iLeaveInObserver = KErrCorrupt;

    // send start event
    TShwEventStart start;
    iCShwMusicControl->NotifyL( &start );
    
	// test that music is off
	EUNIT_ASSERT_DESC( iMusicOn == EFalse, "music is off");
    EUNIT_ASSERT_DESC( gMusicOnCalled == ETrue, "music on was called (and it did leave)" );
    EUNIT_ASSERT_DESC( gMusicOffCalled == ETrue, "music off was called" );
    // reset flags
    gMusicOnCalled = EFalse;
    gMusicOffCalled = EFalse;

    // make observer not leave
    iLeaveInObserver = KErrNone;

    // resend start event
    iCShwMusicControl->NotifyL( &start );
	// test that music is on
	EUNIT_ASSERT_DESC( iMusicOn == ETrue, "music is on");
    EUNIT_ASSERT_DESC( gMusicOnCalled == ETrue, "music on was called" );
    EUNIT_ASSERT_DESC( gMusicOffCalled == EFalse, "music off was not called" );
    // reset flags
    gMusicOnCalled = EFalse;
    gMusicOffCalled = EFalse;

    // send pause
    TShwEventPause pause;
    iCShwMusicControl->NotifyL( &pause );
	EUNIT_ASSERT_DESC( iMusicOn == EFalse, "music is off" );
    EUNIT_ASSERT_DESC( gMusicOnCalled == EFalse, "music on was not called" );
    EUNIT_ASSERT_DESC( gMusicOffCalled == ETrue, "music off was called" );
    // reset flags
    gMusicOnCalled = EFalse;
    gMusicOffCalled = EFalse;

    // make observer leave
    iLeaveInObserver = KErrCorrupt;

    // send resume
    TShwEventResume resume;
    iCShwMusicControl->NotifyL( &resume );
	EUNIT_ASSERT_DESC( iMusicOn == EFalse, "music is off" );
    EUNIT_ASSERT_DESC( gMusicOnCalled == ETrue, "music on was called (and it did leave)" );
    EUNIT_ASSERT_DESC( gMusicOffCalled == ETrue, "music off was called" );
    // reset flags
    gMusicOnCalled = EFalse;
    gMusicOffCalled = EFalse;

    // send volume up
    TShwEventVolumeUp vol;
    iCShwMusicControl->NotifyL( &vol );
    EUNIT_ASSERT_DESC( gMusicOnCalled == EFalse, "music on was not called" );
    EUNIT_ASSERT_DESC( gMusicOffCalled == EFalse, "music off was not called" );
    // reset flags
    gMusicOnCalled = EFalse;
    gMusicOffCalled = EFalse;

    // make observer not leave
    iLeaveInObserver = KErrNone;

    // send resume
    iCShwMusicControl->NotifyL( &resume );
	EUNIT_ASSERT_DESC( iMusicOn == ETrue, "music is on" );
    EUNIT_ASSERT_DESC( gMusicOnCalled == ETrue, "music on was called" );
    EUNIT_ASSERT_DESC( gMusicOffCalled == EFalse, "music off was not called" );
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwMusicControl,
    "Test suite for CShwMusicControl",
    "UNIT" )

/*
 Commented out as MPX crashes with Kern-exec 0
EUNIT_ALLOC_TEST(
    "Constructor test",
    "CShwMusicControl",
    "NewL",
    "FUNCTIONALITY",
    Empty, TestConstructL, Teardown )
*/
// these cant be decorated as the EUnit
// scheduler does not implement Error method
EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test 0",
    "CShwMusicControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyLL, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test 1",
    "CShwMusicControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyL1L, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test 2",
    "CShwMusicControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyL2L, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test 3",
    "CShwMusicControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyL3L, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test 4",
    "CShwMusicControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyL4L, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "NotifyL - test 5",
    "CShwMusicControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_NotifyL5L, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "Test error in file",
    "CShwMusicControl",
    "ConstructL, NotifyL",
    "FUNCTIONALITY",
    Empty, T_ErrorInFileL, Teardown)

EUNIT_NOT_DECORATED_TEST(
    "Test leave in observer",
    "CShwMusicControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, T_LeaveInObserverL, Teardown )

/*
commented out as no added value for the time beeing
EUNIT_NOT_DECORATED_TEST(
    "Play for multitple seconds",
    "CShwMusicControl",
    "NotifyL - test 6",
    "FUNCTIONALITY",
    SetupL, T_ExtendedPlayL, Teardown)
*/
    
    
EUNIT_END_TEST_TABLE

//  END OF FILE

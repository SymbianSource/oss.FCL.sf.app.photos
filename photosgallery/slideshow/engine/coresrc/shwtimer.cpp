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
* Description:    Slideshow specific timer class
 *
*/




// INCLUDES
#include "shwtimer.h"

#include <glxlog.h>
#include <glxtracer.h>

// LOCAL CONSTANTS
namespace
	{
	// how many milliseconds go to microsecond
	const TInt KMultiplyMilli2Micro = 1000;
	}

// DEPENDENCIES

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
inline CShwTimer::CShwTimer()
	: iPaused( EFalse ) // not paused by default
	{
	}
	
// -----------------------------------------------------------------------------
// NewL.
// -----------------------------------------------------------------------------
CShwTimer* CShwTimer::NewL( TInt aPriority )
	{
	TRACER("CShwTimer::NewL");
	GLX_LOG_INFO( "CShwTimer::NewL" );
	CShwTimer* self = new (ELeave) CShwTimer;
	CleanupStack::PushL( self );
	self->ConstructL( aPriority );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwTimer::ConstructL( TInt aPriority )
	{
	TRACER("CShwTimer::ConstructL");
	GLX_LOG_INFO( "CShwTimer::ConstructL" );
	iTimer = CPeriodic::NewL( aPriority );
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwTimer::~CShwTimer()
	{
	TRACER("CShwTimer::~CShwTimer()");
    GLX_LOG_INFO( "CShwTimer::~CShwTimer" );
	Cancel();
	delete iTimer;
	}

// -----------------------------------------------------------------------------
// Start.
// -----------------------------------------------------------------------------
void CShwTimer::Start( TInt aDelay, TInt aInterval, TCallBack aCallback )
	{
	TRACER("CShwTimer::Start");
    GLX_LOG_INFO( "CShwTimer::Start" );

    // convert millisecond to microseconds
    TTimeIntervalMicroSeconds32 delay = aDelay * KMultiplyMilli2Micro;
    TTimeIntervalMicroSeconds32 interval = aInterval * KMultiplyMilli2Micro;
    
	// remember the callback
	iCallback = aCallback;
	// remember the interval
	iInterval = interval;
	// reset the time left in case start is called during pause
	iTimeLeft = delay;

	// No harm in cancelling the timer before start
	iTimer->Cancel();

	// if not paused, start the timer
	if( !iPaused )
		{
		// Get the current time
		iTimeTimerExpires.HomeTime();
		// Add the delay to determine the expiry time
		iTimeTimerExpires += iTimeLeft;
		// Start the timer
		iTimer->Start( iTimeLeft, iInterval, iCallback );
		}
	}

// -----------------------------------------------------------------------------
// Cancel.
// -----------------------------------------------------------------------------
void CShwTimer::Cancel()
	{
	TRACER("CShwTimer::Cancel");
    GLX_LOG_INFO( "CShwTimer::Cancel" );
	if ( iTimer )
		{
		iTimer->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// Pause
// -----------------------------------------------------------------------------
void CShwTimer::Pause()
	{
	TRACER("CShwTimer::Pause");
    GLX_LOG_INFO( "CShwTimer::Pause" );
    // pause only if not yet paused
	if( !iPaused )
		{
		iPaused = ETrue;
		// take time left
		iTimeLeft = TimeLeft();
		//if the iTimeTimerExpires has already elapsed then we get -ve time left.In this case make it zero
		if(iTimeLeft.Int() <= 0)
		    {
		    iTimeLeft = 0;		    
		    }
        GLX_LOG_INFO1( "CShwTimer::Pause timeleft %d", iTimeLeft.Int() );
		// and cancel the timer
		iTimer->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// Resume.
// -----------------------------------------------------------------------------
void CShwTimer::Resume()
	{
	TRACER("CShwTimer::Resume");
    GLX_LOG_INFO( "CShwTimer::Resume" );
    // resume only if paused
    if( iPaused )
    	{
		iPaused = EFalse;
		// if we had time left
		if( 0 <= iTimeLeft.Int() )		
			{
            GLX_LOG_INFO1( "CShwTimer::Resume timeleft %d", iTimeLeft.Int() );
			// start again, first for time left and after that with interval
			// NOTE! cant use Start as it takes time as milliseconds in a TInt
			// Get the current time
			iTimeTimerExpires.HomeTime();
			// Add the delay to determine the expiry time
			iTimeTimerExpires += iTimeLeft;
			// Start the timer
			iTimer->Start( iTimeLeft, iInterval, iCallback );
			}
    	}
	}

// -----------------------------------------------------------------------------
// TimeLeft.
// -----------------------------------------------------------------------------
TTimeIntervalMicroSeconds32 CShwTimer::TimeLeft()
	{
	TRACER("CShwTimer::TimeLeft");
    GLX_LOG_INFO( "CShwTimer::TimeLeft" );

	TTime timeNow;
	timeNow.HomeTime();

	// Difference between time now and the time at which timer should expire
	TTimeIntervalMicroSeconds remainingTime;
	remainingTime = iTimeTimerExpires.MicroSecondsFrom( timeNow );

	TTimeIntervalMicroSeconds32 timeLeft = I64INT( remainingTime.Int64() );
	
	// In case that 64 bit value is larger than 32 bits, just have the 
	// maximum 32 bit value
	if ( remainingTime > KMaxTInt32 )
		{
		timeLeft = TTimeIntervalMicroSeconds32( KMaxTInt32 );
		}
	
	return timeLeft;
	}

// -----------------------------------------------------------------------------
// IsActive.
// -----------------------------------------------------------------------------
TBool CShwTimer::IsActive()
	{
	TRACER("CShwTimer::IsActive");
	return iTimer->IsActive();
	}

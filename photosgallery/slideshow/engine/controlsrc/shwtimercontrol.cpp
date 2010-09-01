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
* Description:    The timer control for the slideshow
 *
*/




#include "shwtimercontrol.h"
#include "shwevent.h"
#include "shwcallback.h"
#include "shwtimer.h"

#include <glxlog.h>
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwTimerControl::CShwTimerControl()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwTimerControl* CShwTimerControl::NewL()
	{
	TRACER("CShwTimerControl::NewL");
	GLX_LOG_INFO( "CShwTimerControl::NewL" );
	CShwTimerControl* self = new( ELeave ) CShwTimerControl;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwTimerControl::~CShwTimerControl()
	{
	TRACER("CShwTimerControl::~CShwTimerControl");
    GLX_LOG_INFO( "CShwTimerControl::~CShwTimerControl" );
	// just delete, it cancels the timer
	delete iTimer;
	}

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwTimerControl::ConstructL()
	{
	TRACER("CShwTimerControl::ConstructL");
	// create the timer
	iTimer = CShwTimer::NewL( CActive::EPriorityStandard ); // neutral priority
	}

// -----------------------------------------------------------------------------
// SendTimerBeatL.
// -----------------------------------------------------------------------------
TInt CShwTimerControl::SendTimerBeatL()
	{
	TRACER("CShwTimerControl::SendTimerBeatL");
    GLX_LOG_INFO( "CShwTimerControl::SendTimerBeatL" );
	// cancel the timer
	iTimer->Cancel();
	// send the timerbeat
	TShwEventTimerBeat timerbeat;
	SendEventL( &timerbeat );
	// return KErrNone, return value is ignored by CPeriodic
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// NotifyL.
// -----------------------------------------------------------------------------
void CShwTimerControl::NotifyL( MShwEvent* aEvent )
	{
	TRACER("CShwTimerControl::NotifyL");
    GLX_LOG_INFO( "CShwTimerControl::NotifyL" );
	TShwEventStartView* eventStartView = 
		dynamic_cast<TShwEventStartView*>( aEvent );
    // was it start view
	if( eventStartView )
		{
		// event is start view
		TInt interval = eventStartView->Parameter();
		// get the relevant delay and interval values
	    GLX_LOG_INFO1( 
	        "CShwTimerControl::NotifyL - TShwEventStartView %d", interval );

		// Start the timer with the appropriate values for the event
        iTimer->Start(interval, interval, TShwCallBack<CShwTimerControl,
                &CShwTimerControl::SendTimerBeatL> (this));     
		}
    // was it start pause
	else if ( dynamic_cast<TShwEventPause*>( aEvent ) )
		{
		GLX_LOG_INFO( "CShwTimerControl::NotifyL - TShwEventPause" );
		// pause the timer
		iTimer->Pause();
		}
    // was it start resume
	else if( dynamic_cast<TShwEventResume*>( aEvent ) )
		{
		// event is resume
	    GLX_LOG_INFO( "CShwTimerControl::NotifyL - TShwEventResume" );
		// resume the timer
		iTimer->Resume();
		}
	else if ( dynamic_cast< TShwEventNextImage* >( aEvent ) || 
			  dynamic_cast< TShwEventPreviousImage* >( aEvent ) )
	    { // it was next or previous image
	    GLX_LOG_INFO( "CShwTimerControl::NotifyL - TShwEventNext/PreviousImage" );
        // cancel the timer as we have moved to another image
        iTimer->Cancel();
	    }
	else if ( dynamic_cast< TShwEventToggleControlUi* >( aEvent ))
        {
        GLX_LOG_INFO( "CShwTimerControl::NotifyL - TShwEventToggleControlUi" );
        // Have to impliment if need comes
        }
	}

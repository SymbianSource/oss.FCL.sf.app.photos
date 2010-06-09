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
* Description:    The view control for the slideshow
 *
*/




#include "shwviewcontrol.h"

#include "shwevent.h"
#include "shwslideshowenginepanic.h"

#include <mglxmedialist.h>
#include <glxlog.h>
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwViewControl::CShwViewControl( MGlxMediaList& aList )
	: iList( aList ),
	iReadyToViewReceived( EFalse ),
	iTransitionReadyReceived( EFalse ),
	iReadyToAdvanceReceived( EFalse ),
	iTimerReceived( EFalse ),
	iHDMIDecoded(ETrue),
	iUserNavigated( EFalse ),
	iPaused( EFalse ),
	iUserNavigatedWhilePaused( EFalse )
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwViewControl* CShwViewControl::NewL( 
	MGlxMediaList& aList, TInt aTransitionDuration, TInt aViewDuration )
	{
	TRACER("CShwViewControl::NewL");
	GLX_LOG_INFO( "CShwViewControl::NewL" );
	CShwViewControl* self = new( ELeave ) CShwViewControl( aList );
	CleanupStack::PushL( self );

	// set the durations
	self->iTransitionDuration = aTransitionDuration;
	self->iViewDuration = aViewDuration;
	self->ConstructL();

	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwViewControl::~CShwViewControl()
	{
	TRACER("CShwViewControl::~CShwViewControl");
    GLX_LOG_INFO( "CShwViewControl::~CShwViewControl" );
	}

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwViewControl::ConstructL()
	{
	TRACER("CShwViewControl::ConstructL");
	GLX_LOG_INFO( "CShwViewControl::ConstructL" );
	}

// -----------------------------------------------------------------------------
// NotifyL.
// -----------------------------------------------------------------------------
void CShwViewControl::NotifyL( MShwEvent* aEvent )
	{
	TRACER("CShwViewControl::NotifyL");
	GLX_LOG_INFO( "CShwViewControl::NotifyL" );
	// we got an event, was it start slideshow 
	if( dynamic_cast< TShwEventStart* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventStart" );
		// start view immediately
		// as we do not get transition ready before the first slide
		TShwEventStartView startView( iViewDuration );
		SendEventL( &startView );
		}
	// was it ready to view
	else if( dynamic_cast< TShwEventReadyToView* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventReadyToView" );
		// set ready to view flag
		iReadyToViewReceived = ETrue;
		// is transition ready or user did navigate
		if( iTransitionReadyReceived || iUserNavigated )
		    {
		    // reset user navigate flag
		    iUserNavigated = EFalse;
		    // send start view
		    SendStartViewL();
		    }
		}
	// was it transition ready
	else if( dynamic_cast< TShwEventTransitionReady* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventTransitionReady" );
		// set transition ready flag
		iTransitionReadyReceived = ETrue;
		// are we ready to start view
		if( iReadyToViewReceived )
		    {
		    // send start view
		    SendStartViewL();
		    }
		}
	// was it timer beat
	else if( dynamic_cast< TShwEventTimerBeat* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventTimerBeat" );
		// we got the timer
		iTimerReceived = ETrue;
		// check if we are ok to go to start transition
        CheckAndSendStartTransitionL();
		}
	// was it ready to advance
	else if( dynamic_cast< TShwEventReadyToAdvance* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventReadyToAdvance" );
		// we got ready to advance
		iReadyToAdvanceReceived = ETrue;
		// check if we are ok to go to start transition
        CheckAndSendStartTransitionL();
		}
	else if ( dynamic_cast< TShwEventNextImage* >( aEvent ) || 
			  dynamic_cast< TShwEventPreviousImage* >( aEvent ) )
		{// it was next or previous image
		// user did navigate, reset the state flags
		iReadyToAdvanceReceived = EFalse;
		iReadyToViewReceived = EFalse;
		// user did navigate
		iUserNavigated = ETrue;
		iUserNavigatedWhilePaused = iPaused;
		}
	else if ( dynamic_cast< TShwEventPause* >( aEvent ) )
		{ // pause event
		// we are paused
		iPaused = ETrue;
		iUserNavigatedWhilePaused = EFalse;
		}
	else if ( dynamic_cast< TShwEventResume* >( aEvent ) )
		{ // resume event
		// we are not paused
		iPaused = EFalse;
		// Check if user did navigate
		if( iUserNavigatedWhilePaused )
			{
			// reset the flag
			iUserNavigatedWhilePaused = EFalse;
			// user navigated while paused so need to reset view mode 
			// to restart the effect and timer, while on pause it was not
			// completely started
		    SendStartViewL();
			}
		}
	else if ( dynamic_cast< TShwEventToggleControlUi* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventToggleControlUi" );
        // Have to impliment if need comes
        }	 
	else if( dynamic_cast< TShwEventHDMIDisConnected* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventHDMIDisConnected" );
		iHDMIDecoded = ETrue ;
		// check if we are ok to go to start transition
        CheckAndSendStartTransitionL();
		}
	else if( dynamic_cast< TShwEventHDMIImageDecodingStarted* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventHDMIImageDecodingStarted" );
		iHDMIDecoded = EFalse ; 
		}
	else if( dynamic_cast< TShwEventHDMIImageDecodingCompleted* >( aEvent ) )
		{
		GLX_LOG_INFO( "CShwViewControl::NotifyL - TShwEventHDMIImageDecodingCompleted" );
		iHDMIDecoded = ETrue ;
		// check if we are ok to go to start transition
        CheckAndSendStartTransitionL();
		}
	}

// -----------------------------------------------------------------------------
// CheckAndSendStartTransitionL.
// -----------------------------------------------------------------------------
void CShwViewControl::CheckAndSendStartTransitionL()
    {
    TRACER("CShwViewControl::CheckAndSendStartTransitionL");
    GLX_LOG_INFO( "CShwViewControl::CheckAndSendStartTransitionL" );
	// check if we got timer and ready to advance and 
	// we are not paused and there is more than one item
	if (iTimerReceived && iReadyToAdvanceReceived && iHDMIDecoded && (!iPaused)
			&& (iList.Count() > 1))
	    {
        GLX_LOG_INFO( "CShwViewControl::Sending TShwEventStartTransition" );
        // reset timer and viewready flags
        iReadyToAdvanceReceived = EFalse;
        iTimerReceived = EFalse;
        // send start transition event
        TShwEventStartTransition startTransition( iTransitionDuration );
        SendEventL( &startTransition );
        // reset also view flags so that we wait for both before starting view
        iReadyToViewReceived = EFalse;
        iTransitionReadyReceived = EFalse;
	    }
    }

// -----------------------------------------------------------------------------
// SendStartViewL.
// -----------------------------------------------------------------------------
void CShwViewControl::SendStartViewL()
    {
    TRACER("CShwViewControl::SendStartViewL");
    GLX_LOG_INFO( "CShwViewControl::SendStartViewL" );
    // reset ready to view and transition ready flags
	iReadyToViewReceived = EFalse;
	iTransitionReadyReceived = EFalse;
	// send start view event
	TShwEventStartView startView( iViewDuration );
	SendEventL( &startView );
	// reset also transition start flags so that we wait for both 
	// before starting transition
    iTimerReceived = EFalse;
	iReadyToAdvanceReceived = EFalse;
    }


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
* Description:    The pause handling control for the slideshow
 *
*/




//  CLASS HEADER
#include "shwslideshowpausehandler.h"

//  EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "shwslideshowengine.h"

#include <glxlog.h>
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// C++ Constructor. inlined to save a few bits of ROM
// -----------------------------------------------------------------------------
inline CShwSlideShowPauseHandler::CShwSlideShowPauseHandler(
        CShwSlideshowEngine& aEngine )
    : iEngine( aEngine ),
    iInCall( EFalse ),  // set to false just to clarify
    iUserPaused( EFalse ), // set to false to clarify
    iOnForeground( ETrue ) // we are on foreground by default
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwSlideShowPauseHandler* CShwSlideShowPauseHandler::NewL(
        CShwSlideshowEngine& aEngine )
    {
    TRACER("CShwSlideShowPauseHandler::OfferKeyEventL");
    GLX_LOG_INFO( "CShwSlideShowPauseHandler::NewL" );
    CShwSlideShowPauseHandler* self = 
        new( ELeave ) CShwSlideShowPauseHandler( aEngine );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwSlideShowPauseHandler::~CShwSlideShowPauseHandler()
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// UserToggledControlsVisibiltyL.
// -----------------------------------------------------------------------------
void CShwSlideShowPauseHandler::ToggleUiControlsVisibiltyL()
    {
        iEngine.ToggleUiControlsVisibiltyL();
    }
// -----------------------------------------------------------------------------
// UserToggledPauseL.
// -----------------------------------------------------------------------------
void CShwSlideShowPauseHandler::UserToggledPauseL()
    {
    TRACER("CShwSlideShowPauseHandler::UserToggledPauseL");
    GLX_LOG_INFO( 
        "CShwSlideShowPauseHandler::UserToggledPauseL" );
    // get the engine state unless we are on background
    if( iOnForeground )
        {
        CShwSlideshowEngine::TShwState state = iEngine.State();
        if ( CShwSlideshowEngine::EShwStateRunning == state )
            {
            // user paused us
            iUserPaused = ETrue;
            // Calling PauseL on the engine results in the view receiving an
            // paused event from engine observer.
            iEngine.PauseL();
            }
        else if ( CShwSlideshowEngine::EShwStatePaused == state )
            {
            // user resumed us
            iUserPaused = EFalse;
            // if engine is paused, resume
            iEngine.ResumeL(); 
            }
        }
    }

// -----------------------------------------------------------------------------
// PhoneCallStartedL.
// -----------------------------------------------------------------------------
void CShwSlideShowPauseHandler::PhoneCallStartedL()
    {
    TRACER("CShwSlideShowPauseHandler::PhoneCallStartedL");
    GLX_LOG_INFO( 
        "CShwSlideShowPauseHandler::PhoneCallStartedL" );
    // set state to in call
    iInCall = ETrue;
    // get the engine state
    CShwSlideshowEngine::TShwState state = iEngine.State();
    // if engine is running
    if ( CShwSlideshowEngine::EShwStateRunning == state )
        {
        // Calling PauseL on the engine results in the view receiving an
        // paused event from engine observer.
        iEngine.PauseL();
        }
    }

// -----------------------------------------------------------------------------
// PhoneCallEndedL.
// -----------------------------------------------------------------------------
void CShwSlideShowPauseHandler::PhoneCallEndedL()
    {
    TRACER("CShwSlideShowPauseHandler::PhoneCallEndedL");
    GLX_LOG_INFO( 
        "CShwSlideShowPauseHandler::PhoneCallEndedL" );
    // set state to not in call
    iInCall = EFalse;
    // only resume when we are on foreground
    if( iOnForeground )
        {
        // get the engine state
        CShwSlideshowEngine::TShwState state = iEngine.State();
        // if not paused by user and engine is paused, resume
        if( ( !iUserPaused )&&
            ( CShwSlideshowEngine::EShwStatePaused == state ) )
            {
            iEngine.ResumeL(); 
            }
        }
    }

// -----------------------------------------------------------------------------
// SwitchToBackgroundL.
// -----------------------------------------------------------------------------
void CShwSlideShowPauseHandler::SwitchToBackgroundL()
    {
    TRACER("CShwSlideShowPauseHandler::SwitchToBackgroundL");
    GLX_LOG_INFO( 
        "CShwSlideShowPauseHandler::SwitchToBackgroundL" );
    // we are now not on foreground
    iOnForeground = EFalse;
    // get the engine state
    CShwSlideshowEngine::TShwState state = iEngine.State();
    // if engine is running
    if ( CShwSlideshowEngine::EShwStateRunning == state )
        {
        // Calling PauseL on the engine results in the view receiving an
        // paused event from engine observer.
        iEngine.PauseL();
        }
    }

// -----------------------------------------------------------------------------
// SwitchToForegroundL.
// -----------------------------------------------------------------------------
void CShwSlideShowPauseHandler::SwitchToForegroundL()
    {
    TRACER("CShwSlideShowPauseHandler::SwitchToForegroundL");
    GLX_LOG_INFO( 
        "CShwSlideShowPauseHandler::SwitchToForegroundL" );
    // we are now on foreground
    iOnForeground = ETrue;
    // get the engine state
    CShwSlideshowEngine::TShwState state = iEngine.State();
    // if we are not paused by user, not in call and engine is paused, resume
    if( ( !iUserPaused )&&
        ( !iInCall )&&
        ( CShwSlideshowEngine::EShwStatePaused == state ) )
        {
        iEngine.ResumeL(); 
        }
    }
// -----------------------------------------------------------------------------
// IsSlideShowEngineStatePaused
// -----------------------------------------------------------------------------
TBool CShwSlideShowPauseHandler::IsSlideShowEngineStatePaused()
	{
	TRACER("CShwSlideShowPauseHandler::IsSlideShowEngineStatePaused()");
	return iUserPaused;
	}


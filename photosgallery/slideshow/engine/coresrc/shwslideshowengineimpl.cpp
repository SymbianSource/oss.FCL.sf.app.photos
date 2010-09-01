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
* Description:    The implementation of the engine API
 *
*/




// CLASS HEADER
#include "shwslideshowengineimpl.h"

// EXTERNAL INCLUDES
#include <alf/alfimagevisual.h>
#include <glxvisuallistmanager.h>
#include <mglxvisuallist.h>
#include <glxlog.h>
#include <glxtracer.h>

// INTERNAL INCLUDES
#include "shwengineobserver.h"
#include "shwplaybackfactory.h"
#include "shweventrouter.h"
#include "shwevent.h"
#include "shwslideshowenginepanic.h"
#include "shwhuiutility.h"

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwSlideshowEngineImpl::CShwSlideshowEngineImpl( 
    MShwEngineObserver& aObserver )
    : iObserver( aObserver )
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwSlideshowEngineImpl* CShwSlideshowEngineImpl::NewL(
	MShwEngineObserver& aObserver )
	{
	TRACER("CShwSlideshowEngineImpl::NewL");
	GLX_LOG_INFO("CShwSlideshowEngineImpl::NewL");
	CShwSlideshowEngineImpl* self = 
	    new( ELeave ) CShwSlideshowEngineImpl( aObserver );
	CleanupStack::PushL( self );
	// perform 2nd phase
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwSlideshowEngineImpl::~CShwSlideshowEngineImpl()
	{
	TRACER("CShwSlideshowEngineImpl::~CShwSlideshowEngineImpl");
	GLX_LOG_INFO("CShwSlideshowEngineImpl::~CShwSlideshowEngineImpl");
	// delete event router, it should not own any external objects
	delete iEventRouter;
	// delete the playback factory, it deletes the objects it owns
	// and they may require media or visual list
	delete iFactory;
	// clean the visual list (it has reference to media list)
	// Check that we got a manager
	if( iVisualListManager )
		{
		// Check if we got a list
		if( iVisualList )
			{
			// remove the list
			iVisualListManager->ReleaseList( iVisualList );
			}
		iVisualListManager->Close();
		}
	}

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::ConstructL()
	{
	TRACER("CShwSlideshowEngineImpl::ConstructL");
	GLX_LOG_INFO("CShwSlideshowEngineImpl::ConstructL");
	// get the visual list manager
	iVisualListManager = CGlxVisualListManager::ManagerL();
	}

// -----------------------------------------------------------------------------
// StartL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::StartL(
	CAlfEnv& aEnv, CAlfDisplay& aDisplay, MGlxMediaList& aItemsToShow,
	MShwMusicObserver& aMusicObserver, TSize aScreenSize )
	{
	TRACER("CShwSlideshowEngineImpl::StartL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::Start(Init)L" );

	// check that we've not already been started
	__ASSERT_ALWAYS( !iState, 
		NShwEngine::Panic( NShwEngine::EEngineStartLCalledInWrongState ) );

	// remember the display
	iDisplay = &aDisplay;

	// the media list is already ordered correctly for use
	iMediaList = &aItemsToShow;

	// construct the visual list
	iVisualList = iVisualListManager->AllocListL( 
		*iMediaList, aEnv, aDisplay, CAlfImageVisual::EScaleFitInside );

	// set the default icons to the list
	iVisualList->SetDefaultIconBehaviourL( ETrue );

	// construct the factory
	iFactory = CShwPlaybackFactory::NewL(
		&aEnv, 
		iVisualList, 
		iMediaList, 
		aMusicObserver,
		aScreenSize );

	// construct the event router
	iEventRouter = CShwEventRouter::NewL();
	// get the observers from factory
	// add them to router
	iEventRouter->AddObserversL( iFactory->EventObservers().Array() );
	// add ourself as observer
	iEventRouter->AddObserverL( this );

	// get the producers from factory
	// set the event producers to the router
	iEventRouter->AddProducers( iFactory->EventPublishers().Array() );
	// add ourself as publisher
	iEventRouter->AddProducer( this );

    // now all producers are there but they may need to do some
    // initialisation, send initialize event to allow that
	TShwEventInitialize initEvent;
	SendEventL( &initEvent );

	// set state now. We will get the event call (Notify)
	// once the first thumbnail is loaded and then we really start
	iState = CShwSlideshowEngine::EShwStateInitialized;

	GLX_LOG_INFO( "CShwSlideshowEngineImpl::Start(Init)L - End" );
	}

// -----------------------------------------------------------------------------
// NextItemL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::NextItemL()
	{
	TRACER("CShwSlideshowEngineImpl::NextItemL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::NextItemL" );

	if( CShwSlideshowEngine::EShwStateRunning == iState ||
		CShwSlideshowEngine::EShwStatePaused == iState )
		{
		// only change images if paused or playing
		TShwEventNextImage nextItemEvent;
		SendEventL( &nextItemEvent );
		}
	}

// -----------------------------------------------------------------------------
// PreviousItemL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::PreviousItemL()
	{
	TRACER("CShwSlideshowEngineImpl::PreviousItemL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::PreviousItemL" );
	
	if( CShwSlideshowEngine::EShwStateRunning == iState ||
		CShwSlideshowEngine::EShwStatePaused == iState )
		{
		// only change images if paused or playing
		TShwEventPreviousImage previousItemEvent;
		SendEventL( &previousItemEvent );
		}
	}

// -----------------------------------------------------------------------------
// PauseL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::PauseL()
	{
	TRACER("CShwSlideshowEngineImpl::PauseL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::PauseL" );
	// check state, if not running, don't bother pausing
	if( CShwSlideshowEngine::EShwStateRunning == iState )
		{
		// send the pause event to the router
		TShwEventPause pauseEvent;
		SendEventL( &pauseEvent );
				
		// update the state
		iState = CShwSlideshowEngine::EShwStatePaused;
		    
		// tell the observer that we paused the show
		iObserver.EnginePausedL();
		}
	}
// -----------------------------------------------------------------------------
// LSKPressedL
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::LSKPressedL()
    {
    TRACER("CShwSlideshowEngineImpl::LSKPressedL");
    GLX_LOG_INFO( "CShwSlideshowEngineImpl::LSKPressedL" );
    iObserver.EngineLSKPressedL();
        
    }
// -----------------------------------------------------------------------------
// ToggleControlsVisibiltyL., LSKPressedL
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::ToggleUiControlsVisibiltyL()
    {
    TRACER("CShwSlideshowEngineImpl::ToggleUiControlsVisibiltyL");
    GLX_LOG_INFO( "CShwSlideshowEngineImpl::ToggleUiControlsVisibiltyL" );
    
        // send the toggleUi event to the router
        TShwEventToggleControlUi toggleUi;
        SendEventL( &toggleUi );
        // tell the observer that we toggled softkey visibility by 
        // pressing selection key
        iObserver.EngineToggleUiStateL();
        
    }

// -----------------------------------------------------------------------------
// VolumeUpL
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::VolumeUpL()
	{
	TRACER("CShwSlideshowEngineImpl::VolumeUpL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::VolumeUpL" );

	// check state and send the resume event to the router
	if( CShwSlideshowEngine::EShwStateInitialized != iState )
		{
		TShwEventVolumeUp volumeUp;
		SendEventL( &volumeUp );
		}
	}

// -----------------------------------------------------------------------------
// VolumeDownL
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::VolumeDownL()
	{
	TRACER("CShwSlideshowEngineImpl::VolumeDownL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::VolumeDownL" );

	// check state and send the resume event to the router
	if( CShwSlideshowEngine::EShwStateInitialized != iState )
		{
		TShwEventVolumeDown volumeDown;
		SendEventL( &volumeDown );
		}
	}

// -----------------------------------------------------------------------------
// GetMusicVolumeL
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::GetMusicVolumeL()
    {
    TRACER("CShwSlideshowEngineImpl::GetMusicVolumeL");
    if(iFactory)
        {
        iFactory->GetMusicVolumeL();
        }
    }

// -----------------------------------------------------------------------------
// ResumeL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::ResumeL()
	{
	TRACER("CShwSlideshowEngineImpl::ResumeL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::ResumeL" );

	// check state and send the resume event to the router
	if ( iState == CShwSlideshowEngine::EShwStatePaused )
		{
		// only valid to resume if we're currently paused
		TShwEventResume resumeEvent;
		SendEventL( &resumeEvent );

		// update the state
		iState = CShwSlideshowEngine::EShwStateRunning;
		
		// tell the observer that the show restarted
		iObserver.EngineResumedL();
		}
	}

// -----------------------------------------------------------------------------
// NotifyL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::NotifyL( MShwEvent* aEvent )
	{
	TRACER("CShwSlideshowEngineImpl::NotifyL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::NotifyL" );

	// we got an event, was it ready to view and we are not yet running
	if( ( CShwSlideshowEngine::EShwStateInitialized == iState ) &&
	    ( dynamic_cast< TShwEventReadyToView* >( aEvent ) ) )
		{
    	GLX_LOG_INFO( "CShwSlideshowEngineImpl::send TShwEventStart" );
		// start the playback
		TShwEventStart start;
		SendEventL( &start );
		
		// set state
		iState = CShwSlideshowEngine::EShwStateRunning;
		
		// tell the observer that we are running the show
		iObserver.EngineStartedL();

		// show the control group on the display.
		ShwUiUtility::ShowVisualListL( iDisplay, iVisualList );
		}
	else if( dynamic_cast< TShwEventFatalError* >( aEvent ) )
	    {
    	GLX_LOG_INFO( "CShwSlideshowEngineImpl fatal error, exit slideshow" );
        // let the observer close down slideshow
        iObserver.ErrorDuringSlideshowL();
	    }
	else if( dynamic_cast< TShwEventToggleControlUi* >( aEvent ) )
        {
        GLX_LOG_INFO( "CShwSlideshowEngineImpl:NotifyL - TShwEventToggleControlUi" );
        // Have to impliment if need comes
        }
	}

// -----------------------------------------------------------------------------
// State.
// -----------------------------------------------------------------------------	
CShwSlideshowEngine::TShwState CShwSlideshowEngineImpl::State() const
	{
	TRACER("CShwSlideshowEngineImpl::State()");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::State()" );
	return iState;
	}

// -----------------------------------------------------------------------------
// AvailableEffectsL.
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::AvailableEffectsL(
	RArray<TShwEffectInfo>& aEffects )
	{
	TRACER("CShwSlideshowEngineImpl::AvailableEffectsL");
	GLX_LOG_INFO( "CShwSlideshowEngineImpl::AvailableEffectsL" );
	// This should be queried from the playback factory
	CShwPlaybackFactory::AvailableEffectsL( aEffects );
	}

// -----------------------------------------------------------------------------
// HandleHDMIDecodingEventL
// -----------------------------------------------------------------------------
void CShwSlideshowEngineImpl::HandleHDMIDecodingEventL(
        THdmiDecodingStatus aStatus)
    {
    TRACER("CShwSlideshowEngineImpl::HandleHDMIDecodingEventL");
    GLX_LOG_INFO1("CShwSlideshowEngineImpl::HandleHDMIDecodingEventL: %d",
            aStatus);

    switch (aStatus)
        {
        case EHdmiDecodingStarted:
            {
            TShwEventHDMIImageDecodingStarted decodingStarted;
            SendEventL(&decodingStarted);
            break;
            }
        case EHdmiDecodingCompleted:
            {
            TShwEventHDMIImageDecodingCompleted decodingCompleted;
            SendEventL(&decodingCompleted);
            break;
            }
		case EHdmiDecodingFirstCompleted:
			{
			TShwEventHDMIFirstImageDecodingCompleted decodingFirstCompleted;
			SendEventL( &decodingFirstCompleted );
			break;
			}						
        case EHdmiDisconnected:
		default:
            {
            TShwEventHDMIDisConnected hdmiDisconnected;
            SendEventL(&hdmiDisconnected);
            break;
            }
        }
    }


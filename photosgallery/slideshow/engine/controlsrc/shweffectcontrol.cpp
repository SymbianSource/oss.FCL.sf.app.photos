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
* Description:    The effect control for the slideshow
 *
*/




// CLASS HEADER
#include "shweffectcontrol.h"

// EXTERNAL INCLUDES
#include <mglxvisuallist.h>
#include <mglxmedialist.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <AknUtils.h>
// INTERNAL INCLUDES
#include "shweffectmanager.h"
#include "shweffect.h"
#include "shwthumbnailloader.h"
#include "shwslideshowenginepanic.h"
#include "shwcallback.h"
#include "shwtimer.h"
#include "shwconstants.h"

using namespace NShwSlideshow;

// LOCAL NAMESPACE
namespace
    {
    // local constants, the visual list range
    const TInt KRangeLeftFromFocus = -1;
    const TInt KRangeRightFromFocus = 1;
    }

// LOCAL FUNCTIONS AND CONSTANTS
// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwEffectControl::CShwEffectControl(
        MShwEffectManager& aEffectManager, 
        MGlxVisualList& aVisualList,
        MGlxMediaList& aMediaList,
        TSize aScreenSize )
    : iEffectManager( aEffectManager ),
    iVisualList( aVisualList ),
    iMediaList( aMediaList ),
	iEffectLoopRestarted( EFalse ),
	iFailedThumbnailIndex( KErrNotFound )
    {
    // initialize the default layout chain
    iDefaultIconLayout.SetOpacity( KMaxOpacity, 0 );
    iDefaultIconLayout.SetSize( aScreenSize.iWidth, aScreenSize.iHeight, 0 );
    // initialise the not visible layout
    iNotVisibleLayout.SetOpacity(KMinOpacity,0);
    iNotVisibleLayout.SetSize( 0, 0, 0 );
   iSplitter.SetDefaultLayout(&iNotVisibleLayout);
    // initialise the paused layout
    iPausedLayout.SetOpacity( KMaxOpacity, 0);
    iPausedLayout.SetSize( aScreenSize.iWidth, aScreenSize.iHeight, 0 );    
    }

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwEffectControl* CShwEffectControl::NewL(
    MShwEffectManager& aEffectManager, 
    MGlxVisualList& aVisualList,
    MGlxMediaList& aMediaList,
    TSize aScreenSize )
    {
    TRACER("CShwEffectControl* CShwEffectControl::NewL");
    GLX_LOG_INFO( "CShwEffectControl::NewL" );
    CShwEffectControl* self = 
        new( ELeave ) CShwEffectControl(
            aEffectManager, 
            aVisualList,
            aMediaList,
            aScreenSize );

    // 2nd phase construct
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwEffectControl::~CShwEffectControl()
    {
    TRACER("CShwEffectControl::~CShwEffectControl()");
    GLX_LOG_INFO( "CShwEffectControl::~CShwEffectControl" );

    iPausedVisuals.Close();   

    // stop observing the visual list
    iVisualList.RemoveObserver( this );
    // delete timer, it cancels itself
    delete iTransitionTimer;
    // remove viewing context
    iVisualList.RemoveContext( iContextId );
    // delete the thumbnail loader
    delete iThumbnailLoader;
    }

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwEffectControl::ConstructL()
    {
    TRACER("CShwEffectControl::ConstructL()");
    GLX_LOG_INFO( "CShwEffectControl::ConstructL" );

    // create thumbnail loader for the medialist
    iThumbnailLoader  = CShwThumbnailLoader::NewL( iMediaList, *this );

    // create the timer to end the transition phase
    iTransitionTimer = CShwTimer::NewL( CActive::EPriorityStandard );

    // add us as visual list observer
    iVisualList.AddObserverL( this );

    // specify viewing range for the visual list
    iContextId = iVisualList.AddContextL(
        KRangeLeftFromFocus, KRangeRightFromFocus );

    // initialize splitter
    iSplitter.SetVisualListL( &iVisualList );
    // add the splitter layout on the bottom
    //subhasis commented 1 line
	//API depricated in visual list manager
    iVisualList.AddLayoutL( &iSplitter );
    }

// -----------------------------------------------------------------------------
// SendTransitionReadyL.
// -----------------------------------------------------------------------------
TInt CShwEffectControl::SendTransitionReadyL()
    {
    TRACER("CShwEffectControl::SendTransitionReadyL()");
    GLX_LOG_INFO( "CShwEffectControl::SendTransitionReadyL" );
    // cancel the timer
    iTransitionTimer->Cancel();
    // we need to remove the high quality thumbnails we dont plan
    // to show in near future, so focus minus range to left 
    // is the first to unload
    TInt indexToUnloadBackwards = NextListIndex( KRangeLeftFromFocus );
    // check that this index is not in focus or next from focus
    // this can happen if the list to play is really short
    TInt focusIndex = iMediaList.FocusIndex();
    if( ( indexToUnloadBackwards != focusIndex )&&
        ( indexToUnloadBackwards != NextListIndex( KNavigateForwards ) ) )
        {
        // ok, not visible so can unload
        iThumbnailLoader->Unload( indexToUnloadBackwards );
        }
    // send the event
    TShwEventTransitionReady transitionReady;
    SendEventL( &transitionReady );
    // return KErrNone, return value is ignored by CPeriodic
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// NotifyL.
// -----------------------------------------------------------------------------
void CShwEffectControl::NotifyL( MShwEvent* aEvent )
    {
    TRACER("CShwEffectControl::NotifyL");
    // we got an event
    // is it initialize
    TShwEventInitialize* init = 
        dynamic_cast<TShwEventInitialize*>( aEvent );
    // is it start view
    TShwEventStartView* viewEvent = 
        dynamic_cast<TShwEventStartView*>( aEvent );
    // is it start transition
    TShwEventStartTransition* transEvent = 
        dynamic_cast<TShwEventStartTransition*>( aEvent );
    // was it a pause or resume event
    TShwEventPause* pauseEvent = 
        dynamic_cast<TShwEventPause*>( aEvent );
    TShwEventResume* resumeEvent = 
        dynamic_cast<TShwEventResume*>( aEvent );
    // or was it a navigation event
    TShwEventNextImage* nextImageEvent = 
        dynamic_cast< TShwEventNextImage* >( aEvent );
    TShwEventPreviousImage* previousImageEvent =
        dynamic_cast< TShwEventPreviousImage* >( aEvent );
    TShwEventToggleControlUi* toggleControlUiEvent = 
        dynamic_cast< TShwEventToggleControlUi* >( aEvent );

    // process init event
    if( init )
        {
        // prepare the first effect and load the correct size thumbnail
        // thumbnail loader ensures that the size attribute is available
        PrepareAndLoadImageToViewL();
        }
    // process start view event
    else if( viewEvent )
        {
        HandleStartViewEventL( *viewEvent );
        }
    // process start transition event
    else if( transEvent )
        {
        HandleStartTransitionEventL( *transEvent );
        }
    // process pause event
    else if( pauseEvent )
        {
        HandlePauseEventL();
        }
    // process resume event
    else if( resumeEvent )
        {
        HandleResumeEventL();
        }
    // process user navigate to next
    else if( nextImageEvent )
        {
        HandleNextImageEventL();
        }
    // process toggling UI by pressing selection key
    else if( toggleControlUiEvent )
        {
        HandleToggleControlUiEventL();
        }
    // process user navigate to previous
    else if ( previousImageEvent )
        {
        HandlePreviousImageEventL();
        }
    // no other events are interesting for us
    }

// -----------------------------------------------------------------------------
// HandleFocusChangedL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleFocusChangedL(
    TInt aFocusIndex, TReal32 /*aItemsPerSecond*/, MGlxVisualList* /*aList*/,
    NGlxListDefs::TFocusChangeType /*aType*/ )
    {
    TRACER("CShwEffectControl::HandleFocusChangedL");
    GLX_LOG_INFO1( "CShwEffectControl::HandleFocusChangedL %d", aFocusIndex );
    // set current as previous
    iPreviousVisual = iCurrentVisual;
    // take new current
    iCurrentVisual = iVisualList.Visual( aFocusIndex );
    // we need to remove the high quality thumbnails we dont plan
    // to show in near future, so focus minus range to left minus one
    // is the first to unload
    TInt indexToUnloadBackwards = NextListIndex( KRangeLeftFromFocus - 1 );
    // check that this index is not in focus or next from focus
    // this can happen if the list to play is really short
    if( ( indexToUnloadBackwards != aFocusIndex )&&
        ( indexToUnloadBackwards != NextListIndex( KNavigateForwards ) ) )
        {
        // ok, not visible so can unload
        iThumbnailLoader->Unload( indexToUnloadBackwards );
        }
    // unload also range + 1 as user might navigate backwards fast
    TInt indexToUnloadForwards = NextListIndex( KRangeRightFromFocus + 1 );
    // check that this index is not in focus or next from focus
    // this can happen if the list to play is really short
    if( ( indexToUnloadForwards != aFocusIndex )&&
        ( indexToUnloadForwards != NextListIndex( KNavigateForwards ) ) )
        {
        // ok, not visible so can unload
        iThumbnailLoader->Unload( indexToUnloadForwards );
        }
    }

// -----------------------------------------------------------------------------
// HandleSizeChanged.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleSizeChanged(
    const TSize& /*aSize*/, MGlxVisualList* /*aList*/ )
    {
    // nothing to be done
    }
// -----------------------------------------------------------------------------
// HandleToggleControlUiEventL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleToggleControlUiEventL()
    {
    // nothing to be done
    }
 
// -----------------------------------------------------------------------------
// HandleVisualRemoved.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleVisualRemoved(
    const CAlfVisual* aVisual, MGlxVisualList* /*aList*/ )
    {
    TRACER("CShwEffectControl::HandleVisualRemoved");
    GLX_LOG_INFO( "CShwEffectControl::HandleVisualRemoved" );
    // remove the layout for this visual just to make sure we dont use it
    //subhasis commented 1 line
	//API depricated in visual list manager
    iSplitter.RemoveLayout( aVisual );
    }

// -----------------------------------------------------------------------------
// HandleVisualAddedL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleVisualAddedL(
    CAlfVisual* /*aVisual*/, TInt /*aIndex*/, MGlxVisualList* /*aList*/ )
    {
    // nothing to be done
    }

// -----------------------------------------------------------------------------
// HandleThumbnailLoadedL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleThumbnailLoadedL( TInt aIndex )
    {
    TRACER("CShwEffectControl::HandleThumbnailLoadedL");
    TInt focus = iVisualList.FocusIndex();
    GLX_LOG_INFO2( 
        "CShwEffectControl::HandleThumbnailLoadedL %d, focus %d", aIndex, focus );
    // check the index
    // was it on focus, we cannot check the state as it can be either
    // view or transition because of user navigation
    if( aIndex == focus )
        {
        GLX_LOG_INFO( 
            "HandleThumbnailLoadedL - sending TShwEventReadyToView" );
        // its focus index so lets send ready to view
        // send ready to view event
        TShwEventReadyToView readyToView;
        SendEventL( &readyToView );
        }
    // is it next from focus
    else if( aIndex == NextListIndex( KNavigateForwards ) )
        {
        GLX_LOG_INFO( 
            "HandleThumbnailLoadedL - sending TShwEventReadyToAdvance" );
        // send ready to advance command, 
        // it tells the observers that we are ready to advance to next image
        TShwEventReadyToAdvance readyToAdvance;
        SendEventL( &readyToAdvance );
        }
    }

// -----------------------------------------------------------------------------
// HandleThumbnailLoadFailureL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleThumbnailLoadFailureL( TInt aIndex )
    {
    TRACER("CShwEffectControl::HandleThumbnailLoadFailureL");
    GLX_LOG_INFO1( "CShwEffectControl::HandleThumbnailLoadFailureL %d", aIndex );
    // We cant remove the errornous item from the medialist so 
    // at the moment we just proceed as normal
    // user will then see a broken icon for a while
    // we need to remember this index however as we dont want to start the
    // effect for it
    iFailedThumbnailIndex = aIndex;
    HandleThumbnailLoadedL( aIndex );
    }

// -----------------------------------------------------------------------------
// HandleMediaListEmpty
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleMediaListEmpty()
    {
    TRACER("CShwEffectControl::HandleMediaListEmpty");
    GLX_LOG_INFO( "CShwEffectControl::HandleMediaListEmpty" );
    // need to remove the contexts from medialist as closing down will 
    // take a while
    // stop observing the visual list
    iVisualList.RemoveObserver( this );
    // delete the thumbnail loader
    delete iThumbnailLoader;
    // set to NULL to prevent double delete
    iThumbnailLoader = NULL;
    // send fatal error message
    TShwEventFatalError errmessage;
    // need to TRAP as we cannot leave
    TRAPD( error, SendEventL( &errmessage ) );
    // was there an error
    if( error != KErrNone )
        {
        // panic engine as nothing else can be done
        NShwEngine::Panic( NShwEngine::EEngineFatalError );
        }
    }

// -----------------------------------------------------------------------------
// PrepareAndLoadImageToViewL
// -----------------------------------------------------------------------------
void CShwEffectControl::PrepareAndLoadImageToViewL()
    {
    TRACER("CShwEffectControl::PrepareAndLoadImageToViewL");
    // get current index
    TInt currentIndex = iVisualList.FocusIndex();
    // get current effect
    MShwEffect* currentEffect = iEffectManager.CurrentEffect();
    __ASSERT_ALWAYS( 
        currentEffect, NShwEngine::Panic( NShwEngine::ENullCurrentEffect ) );
    // set the current visual, since on first time there has not been navigation
    iCurrentVisual = iVisualList.Visual( currentIndex );
    /// get the image size from thumbnail loader
    TSize imgSize = iThumbnailLoader->ImageSizeL( currentIndex );
    // tell the effect to prepare for viewing current visual
    // thumbnail size is returned
    TSize thumbnailSize = 
        currentEffect->PrepareViewL( iCurrentVisual, imgSize );
    // ask the thumbnail loader to load the thumbnail
    // once the thumbnail is loaded, we get a callback
    iThumbnailLoader->LoadAndNotifyL(
        currentIndex, thumbnailSize );
    }

// -----------------------------------------------------------------------------
// HandleStartViewEventL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleStartViewEventL( TShwEventStartView& aEvent )
    {
    TRACER("CShwEffectControl::HandleStartViewEventL");
    GLX_LOG_INFO( "CShwEffectControl::HandleStartViewEventL" );

    // remember the current view length, it is packaged into the event
    iViewDuration = aEvent.Parameter();

    // update the state
    iState = EShwEffectStateInView;
    
	//following code commented to remove layout dependency
	//! to be refactored
	 
	// get the focus index
	TInt currentIndex = iVisualList.FocusIndex();
    // tell effect to exit transition
    // if we got view event and loop is already started
   if( iEffectLoopRestarted )
        {
        // exit transition for the previous effect and visual
        // this is done first since the next effect may be same
        // as previous
        iPreviousEffect->ExitTransition( iPreviousVisual );
        // remove layout from splitter
        iSplitter.RemoveLayout( iPreviousVisual );
        }
    else
        {
        // get the current effect
        MShwEffect* currentEffect = iEffectManager.CurrentEffect();
        // remove the old layout
        iSplitter.RemoveLayout( iCurrentVisual );
        // check if this thumbnail was succesfully loaded
		if ( iFailedThumbnailIndex == currentIndex )
		    {
        	// set the temporary layout for default icon
        	iSplitter.SetLayoutL( &iDefaultIconLayout, iCurrentVisual );
		    }
		else
		    {
	            // enter the view mode for visual
	            // ask the layout chain and set that to the current visual
	            // set also effect fade-in
	        		iSplitter.SetLayoutL( 
	    			currentEffect->EnterViewL( 
	    				iCurrentVisual, 
	    				iViewDuration, 
	    				KFirstEffectFadeInDuration ), 
	    			iCurrentVisual );
		    }
		// bring the focus visual to the front
		iCurrentVisual->MoveToFront();    
		}
	//parts of the code commented to remove layout dependency
	//! to be refactored
    // get next index forwards
    TInt nextIndex = NextListIndex( KNavigateForwards );
    // need to tell the next effect to prepare for the view
    // get next effect
    MShwEffect* nextEffect = iEffectManager.Effect( KNavigateForwards );
    // get next visual
    CAlfVisual* nextVisual = iVisualList.Visual( nextIndex );
    // get the real size of the image
    TSize imgSize = iThumbnailLoader->ImageSizeL( nextIndex );
    // tell the effect to prepare to view the next visual
    // thumbnail size is returned
    TSize thumbnailSize = 
       nextEffect->PrepareViewL( nextVisual, imgSize );
    // ask the thumbnail loader to load next thumbnail
    iThumbnailLoader->LoadAndNotifyL( nextIndex, thumbnailSize );
    }

// -----------------------------------------------------------------------------
// HandleStartTransitionEventL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleStartTransitionEventL( 
    TShwEventStartTransition& aEvent )
    {
    TRACER("CShwEffectControl::HandleStartTransitionEventL");
    GLX_LOG_INFO( "CShwEffectControl::HandleStartTransitionEventL" );
	//following code was commented to remove layout dependency
	//! to be refactored
    // update the state
    iState = EShwEffectStateInTransition;

    // duration is in event parameters
    TInt duration = aEvent.Parameter();
    // enter current effects transition mode for current visual
    // get current effect
    MShwEffect* currentEffect = iEffectManager.CurrentEffect();
    // inform the effect that view mode ended
    currentEffect->ExitView( iCurrentVisual );
    // remove the view layout
    iSplitter.RemoveLayout( iCurrentVisual );
    // add the layout chain to the current visual
    iSplitter.SetLayoutL(
        currentEffect->EnterTransitionL(
            iCurrentVisual, duration ), iCurrentVisual );
    // ok, now we have the loop started so next time we get view_event 
    // we stop this transition
    iEffectLoopRestarted = ETrue;

    // enter next effects view mode for next visual
    // get next effect, given the direction stored in the event
    MShwEffect* nextEffect = iEffectManager.Effect( KNavigateForwards );
    // get next list index forwards
    TInt nextIndex = NextListIndex( KNavigateForwards );
    GLX_LOG_INFO1( "CShwEffectControl::NextVisual, %d", nextIndex );
    // get next visual, focus plus 1 modulo itemcount
    CAlfVisual* nextVisual = iVisualList.Visual( nextIndex );

    // remove old layout
    iSplitter.RemoveLayout( nextVisual );
    // Check if the next thumbnail was succesfully loaded
	if ( iFailedThumbnailIndex == nextIndex )
	    {
		TAlfTimedValue opacity(KMaxOpacity,0);
		// Set Opacity to make it visible
		nextVisual->SetOpacity(opacity);
    	// set the temporary layout for default icon
    	iSplitter.SetLayoutL( &iDefaultIconLayout, nextVisual );
	    }
	else
	    {
        // enter view mode for the effect
        // add the layout chain to the current visual
        iSplitter.SetLayoutL(
            nextEffect->EnterViewL( nextVisual, iViewDuration, duration ), 
                nextVisual );
	    }


    // proceed on the list, 1 step forward
    NavigateListL( KNavigateForwards );

    
	// fix for EVTY-7H8BV5
	// When transition is happening from current to next visual, we are seeing next to next visual 
	// which is not needed so making next to next visual to invisible
    TInt nextToNextIndex = NextListIndex( KNavigateForwards );
    CAlfVisual* nextToNextVisual = iVisualList.Visual( nextToNextIndex );
	if(nextToNextVisual != iPreviousVisual)    
		{
		TAlfTimedValue opacity(KMinOpacity,0);
		nextToNextVisual->SetOpacity(opacity);	    				
		}

    // need to send ready to view as in the normal case we already are 
    // (the image is already there)
    TShwEventReadyToView readyToView;
    SendEventL( &readyToView );

    // need to start timer to send the transition ready
    iTransitionTimer->Start(
        duration, duration, 
        TShwCallBack< CShwEffectControl, SendTransitionReadyL >( this ) );
    }

// -----------------------------------------------------------------------------
// HandlePauseEventL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandlePauseEventL()
    {
    TRACER("CShwEffectControl::HandlePauseEventL");
    GLX_LOG_INFO( "CShwEffectControl::HandlePauseEventL" );

    // Need to know if user navigates during pause
    iUserNavigated = EFalse;

    // if we are in transition, pause the timer
    if ( iState == EShwEffectStateInTransition )
        {
        iTransitionTimer->Pause();
        iPausedVisuals.AppendL(iPreviousVisual);
        }
    // Store the current visual
    iPausedVisuals.AppendL(iCurrentVisual);
	//subhasis commented 1 line
	//API depricated in visual list manager
    iSplitter.RemoveLayout( iCurrentVisual );
    iSplitter.SetLayoutL( &iPausedLayout, iCurrentVisual );
    // we need to pause all effects as user may navigate while we are in pause
    // and user navigation may change the current and next effect
    RPointerArray< MShwEffect > effects;
    // need to close the array in case of a leave
    CleanupClosePushL( effects );
    iEffectManager.GetActiveEffectsL( effects );
    // loop through all active ones
    for( TInt i = 0; i < effects.Count(); ++i )
        {
        // get the effect and pause it
        effects[ i ]->PauseL();
        }
    CleanupStack::PopAndDestroy( &effects );
    }

// -----------------------------------------------------------------------------
// HandleResumeEventL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleResumeEventL()
    {
    TRACER("CShwEffectControl::HandleResumeEventL");
    GLX_LOG_INFO( "CShwEffectControl::HandleResumeEventL" );
    // we need to resume all effects as user may navigate while we are in pause
    // and user navigation may change the current and next effect
    RPointerArray< MShwEffect > effects;
    // need to close the array in case of a leave
    CleanupClosePushL( effects );
    iEffectManager.GetActiveEffectsL( effects );
    // loop through all active ones
    for( TInt i = 0; i < effects.Count(); ++i )
        {
        // get the effect and pause it
        effects[ i ]->Resume();
        }
    CleanupStack::PopAndDestroy( &effects );

    if( iUserNavigated )
        { // user navigated while paused
        // resume and cancel the transition timer for its next use
        // if user navigates while pause 
        // we never continue from transition so we need
        // to resume the timer, otherwise it stays paused forever
        iTransitionTimer->Resume();
        iTransitionTimer->Cancel();
        }
    // if we are in transition, resume the timer
    else if ( iState == EShwEffectStateInTransition )
        {
        // restart timer to continue the transition
        iTransitionTimer->Resume();
	    TInt count = iPausedVisuals.Count();
	    if (count > 0)    
	        {
            while (count > 0)
		        {
		        //remove the visuals
		        iPausedVisuals.Remove(--count);
		        }
	        }
        }
    }

// -----------------------------------------------------------------------------
// HandleNextImageEventL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandleNextImageEventL()
    {
    TRACER("CShwEffectControl::HandleNextImageEventL");
    GLX_LOG_INFO( "CShwEffectControl::HandleNextImageEventL" );

    // reset the flag so that we do prepare and start view for the visual
    iEffectLoopRestarted = EFalse;
    // set user navigate flag so that we do ExitView in resume
    iUserNavigated = ETrue;

    // get current effect
    MShwEffect* currentEffect = iEffectManager.CurrentEffect();
    // inform the effect that view mode ended
    currentEffect->ExitView( iCurrentVisual );
    // remove the view layout
    //subhasis commented 1 line
	//API depricated in visual list manager
    iSplitter.RemoveLayout( iCurrentVisual );
    // were we in transition
    if ( iState == EShwEffectStateInTransition )
        {
        // we're in transition so the navigation is complete but we
        // want to view to the current visual immediately
        // in transition two effects exist, previous and current
        // and we're moving from the previous visual to the current visual

        // remove the layout from previous
        //subhasis commented 1 line
		//API depricated in visual list manager
        iSplitter.RemoveLayout( iPreviousVisual );
        // cancel the timer to prevent a duplicate start view event
        iTransitionTimer->Cancel();
        }
    else
        {
        // we're in the view state
        // so we want to stop viewing the current visual
        // and show the next one

        // proceed on the list, 1 step forward
        NavigateListL( KNavigateForwards );

        // set the temporary layout for default icon
        iSplitter.SetLayoutL( &iDefaultIconLayout, iCurrentVisual );
        }

    // call prepareview for the effect and load the image
    PrepareAndLoadImageToViewL();
    }

// -----------------------------------------------------------------------------
// HandlePreviousImageEventL.
// -----------------------------------------------------------------------------
void CShwEffectControl::HandlePreviousImageEventL()
    {
    TRACER("CShwEffectControl::HandlePreviousImageEventL");
    GLX_LOG_INFO( "CShwEffectControl::HandlePreviousImageEventL" );

    // reset the flag so that we do prepare and start view for the visual
    iEffectLoopRestarted = EFalse;
    // set user navigate flag so that we do EnterView in next StartView
    iUserNavigated = ETrue;

    // ExitView - current
    // get current effect
    MShwEffect* currentEffect = iEffectManager.CurrentEffect();
    // inform the effect that view mode ended
    currentEffect->ExitView( iCurrentVisual );
    // remove the current layout
    //subhasis commented 1 line
	//API depricated in visual list manager
    iSplitter.RemoveLayout( iCurrentVisual );
    // remove the previous layout as well, its no-op if there wasnt one
    iSplitter.RemoveLayout( iPreviousVisual );
    // proceed on the list, 1 step backwards
    NavigateListL( KNavigateBackwards );

    // were we in transition
    if ( iState == EShwEffectStateInTransition )
        {
        // as we're in transition, the focus index has already been
        // updated to the next item so we need to step back

        // cancel the timer to prevent a duplicate start view event
        iTransitionTimer->Cancel();
        }
    else
        {
        // EnterView - previous
        // set the temporary layout for default icon
        iSplitter.SetLayoutL( &iDefaultIconLayout, iCurrentVisual );
        }

    // call prepareview for the effect and load the image
    PrepareAndLoadImageToViewL();
    }

// -----------------------------------------------------------------------------
// NavigateListL.
// -----------------------------------------------------------------------------
void CShwEffectControl::NavigateListL( TInt aDirection )
    {
    TRACER("CShwEffectControl::NavigateListL");
	// reset the failed index value as the failure might be temporary
	iFailedThumbnailIndex = KErrNotFound;
    // navigate the visual list
    iVisualList.NavigateL( aDirection );
    // store current effect as previous
    iPreviousEffect = iEffectManager.CurrentEffect();
    // navigate to the next effect
    iEffectManager.ProceedToEffect( aDirection );
    }

// -----------------------------------------------------------------------------
// NextListIndex.
// -----------------------------------------------------------------------------
TInt CShwEffectControl::NextListIndex( TInt aDirection )
    {
    TRACER("CShwEffectControl::NextListIndex");
    // next index in list is focus plus direction and then wrap around
    TInt newIndex = 
        ( iVisualList.FocusIndex() + aDirection ) % iVisualList.ItemCount();
    if( newIndex < 0 )
        {
        // navigating left
        // sign for modulo is the same as left side so need to add itemcount
        newIndex += iVisualList.ItemCount();
        }
    return newIndex;
    }

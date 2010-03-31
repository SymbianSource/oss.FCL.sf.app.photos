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
* Description:    The view key handling control for the slideshow
 *
*/




//  CLASS HEADER
#include "shwslideshowkeyhandler.h"

//  EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include <glxlog.h>
#include <glxtracer.h>
#include <glxuiutility.h>
#include "shwslideshowengine.h"
#include "shwslideshowpausehandler.h"
#include "shwslideshowview.h"
// -----------------------------------------------------------------------------
// C++ Constructor. 
// -----------------------------------------------------------------------------
inline CShwSlideShowKeyHandler::CShwSlideShowKeyHandler(
        CShwSlideshowEngine& aEngine, 
        CShwSlideShowPauseHandler& aPauseHandler,
        TInt& aState ) 
    : iEngine( aEngine ),iPauseHandler( aPauseHandler ), iState( aState )
    {
    TRACER("CShwSlideShowKeyHandler::CShwSlideShowKeyHandler");
    GLX_LOG_INFO("CShwSlideShowKeyHandler::CShwSlideShowKeyHandler");
    // No implementation required
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CShwSlideShowKeyHandler::ConstructL()
    {
    TRACER("CShwSlideShowKeyHandler::ConstructL");
    GLX_LOG_INFO("CShwSlideShowKeyHandler::ConstructL");
    iUiUtility = CGlxUiUtility::UtilityL();
    }

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwSlideShowKeyHandler* CShwSlideShowKeyHandler::NewL(
        CShwSlideshowEngine& aEngine,
        CShwSlideShowPauseHandler& aPauseHandler,
        TInt& aState )
    {
    TRACER("CShwSlideShowKeyHandler::NewL");
    GLX_LOG_INFO("CShwSlideShowKeyHandler::NewL");
    CShwSlideShowKeyHandler* self = 
        new( ELeave ) CShwSlideShowKeyHandler( aEngine, aPauseHandler,
                aState );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwSlideShowKeyHandler::~CShwSlideShowKeyHandler()
    {
    TRACER("CShwSlideShowKeyHandler::~CShwSlideShowKeyHandler");
    GLX_LOG_INFO("CShwSlideShowKeyHandler::~CShwSlideShowKeyHandler");
    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
    // No implementation required
    }

// -----------------------------------------------------------------------------
// OfferKeyEventL.
// -----------------------------------------------------------------------------
TKeyResponse CShwSlideShowKeyHandler::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TRACER("CShwSlideShowKeyHandler::OfferKeyEventL");
    GLX_LOG_INFO( "CShwSlideShowKeyHandler::OfferKeyEventL" );
    TKeyResponse retVal = EKeyWasNotConsumed;
    
    if( aType == EEventKeyDown )
        {
        switch( aKeyEvent.iScanCode )
            {
            case EStdKeyIncVolume : // volume up
                {
                iEngine.VolumeUpL();
                retVal = EKeyWasConsumed;
                break;
                }
            case EStdKeyDecVolume : // volume down
                {
                iEngine.VolumeDownL();
                retVal = EKeyWasConsumed;
                break;
                }
            case EStdKeyLeftArrow :	// this gets rotated in landscape
                {
                // show previous image
                iEngine.PreviousItemL();
                retVal = EKeyWasConsumed;
                break;
                }
            case EStdKeyRightArrow : // this gets rotated in landscape
                {
                iEngine.NextItemL();
                retVal = EKeyWasConsumed;
                break;
                }
            case EStdKeyDevice0 : // left soft key
                {
                iEngine.LSKPressedL();
                // key was consumed
                retVal = EKeyWasConsumed;
                break;                
                }
            case EStdKeyDevice3 : // navi select
                {
                iPauseHandler.ToggleUiControlsVisibiltyL();
                // key was consumed
                retVal = EKeyWasConsumed;
                break;
                }
            case EStdKeyDevice1 : // right soft key
                {
                // Exiting so revert to showing the system skin
                // Note: this has to be done here, not in the view deactivation
                // as when we come to deactivate, the screen furniture will
                // match the view id to the active view (e.g. the tile view)
                //@todo-check
                //iScreenFurniture.DisableSystemSkinL( iViewId.iUid,
                //    CGlxScreenFurniture::EGlxSfItemLeftSoftkey, EFalse )
                //iScreenFurniture.DisableSystemSkinL( iViewId.iUid,
                //    CGlxScreenFurniture::EGlxSfItemRightSoftkey, EFalse )
                // Note that in practice it doesn't matter whether we return
                // EKeyWasConsumed or EKeyWasNotConsumed as the result is
                // the same (we still get back to the last view). This is due to
                // HUI's key handling. So, for safety's sake, if in future there 
                // are changes to HUI to cease processing the key events when 
                // EKeyWasConsumed is returned, then we return EKeyWasNotConsumed to
                // ensure that the command is forwarded to the back command handler.
                break;                
                }
            default:	
                {
                retVal = EKeyWasNotConsumed;
                break;
                }
            }// end switch
        }// end if
    // if its a system generated exit event
    // set the state 
    else if (aType == EEventKey && aKeyEvent.iScanCode == EStdKeyNo)
    	{
    	iUiUtility->SetExitingState(ETrue);
    	} // end if else

    return retVal; 
    }

/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Media item list observer interface 
*
*/




#include "glxtoolbarcontroller.h"
#include <aknbutton.h>
#include <akntoolbar.h>                     // For Toolbar
#include <glxcommandhandlers.hrh>           // For Command ids
#include <glxtracer.h>                         // For Logs
#include <glxlog.h>                         // For Logs
#include <mglxmedialist.h>

#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>
#include <centralrepository.h>              // for checking the ShareOnline version
#include <glxuiutility.h>


//----------------------------------------------------------------------------------
// NewL
//----------------------------------------------------------------------------------
//
CGlxToolbarController* CGlxToolbarController::NewL()
    {
    TRACER("CGlxToolbarController::NewL");
    
    CGlxToolbarController *self = new( ELeave ) CGlxToolbarController ();
    return self;
    }

//----------------------------------------------------------------------------------
// Default Constructor
//----------------------------------------------------------------------------------
//
CGlxToolbarController::CGlxToolbarController( )
    {
    TRACER("CGlxToolbarController::Default constructor");
    
    }
    
//----------------------------------------------------------------------------------
// AddToObserver
//----------------------------------------------------------------------------------
//
void CGlxToolbarController::AddToObserverL (MGlxMediaList& aList, CAknToolbar* aToolbar)
    {
    TRACER("CGlxToolbarController::AddToObserverL");

    iToolbar = aToolbar;
    iAttributeAvailable = EFalse;
    aList.AddMediaListObserverL ( this );
    }

//----------------------------------------------------------------------------------
// RemoveFromObserver 
//----------------------------------------------------------------------------------
//
void CGlxToolbarController::RemoveFromObserver (MGlxMediaList& aList)
    {
    TRACER("CGlxToolbarController::RemoveFromObserver");
    
    if( aList.SelectionCount()<= 0 )
        {
        EnableLatch( EGlxCmdStartMultipleMarking, EFalse );
        }

    aList.RemoveMediaListObserver ( this );
    }

//----------------------------------------------------------------------------
// HandleItemAddedL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxToolbarController::HandleItemAddedL");
    
    }

//----------------------------------------------------------------------------
// HandleMediaL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxToolbarController::HandleMediaL");
    
    }

//----------------------------------------------------------------------------
// HandleItemRemovedL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleItemRemovedL(TInt /*aStartIndex*/, 
        TInt /*aEndIndex*/, MGlxMediaList* aList)
    {
    TRACER("CGlxToolbarController::HandleItemRemovedL");
    
    if( aList->Count() <= 0 )
        {
         SetToolbarItemsDimmed(ETrue);
        }
    }

//----------------------------------------------------------------------------
// HandleItemModifiedL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxToolbarController::HandleItemModifiedL");
    
    }

//----------------------------------------------------------------------------
// HandleAttributesAvailableL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleAttributesAvailableL(TInt aItemIndex, 
        const RArray<TMPXAttribute>& /*aAttributes*/, 
        MGlxMediaList* aList)
    {
    TRACER("CGlxToolbarController::HandleAttributesAvailableL");
    
    if( aItemIndex == aList->FocusIndex() )
        {        
        iAttributeAvailable = ETrue;
        SetStatusL(aList);
        }
    }

//----------------------------------------------------------------------------
// HandleFocusChangedL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/, 
        TInt /*aOldIndex*/, MGlxMediaList* aList)
    {  
    TRACER("CGlxToolbarController::HandleFocusChangedL");
    if (aList->Count() <= 0)
        {
        SetToolbarItemsDimmed(ETrue);
        }
    }

//----------------------------------------------------------------------------
// HandleItemSelectedL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, 
        MGlxMediaList* aList)
    {
    TRACER("CGlxToolbarController::HandleItemSelectedL");
    GLX_DEBUG2("CGlxToolbarController::HandleItemSelectedL()"
            " aList->SelectionCount()=%d", aList->SelectionCount());

    // If atleast 1 item is marked, or if Mark All is called, then Latch the mark button.
    if ((aList->SelectionCount() == 1) || (aList->SelectionCount()
            == aList->Count()))
        {
        EnableLatch(EGlxCmdStartMultipleMarking, ETrue);
        }
    }

//----------------------------------------------------------------------------
// HandleMessageL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleMessageL(const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxToolbarController::HandleMessageL");
    
    }

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
//
CGlxToolbarController::~CGlxToolbarController()
    {
    TRACER("CGlxToolbarController Destructor");
    
    }

//----------------------------------------------------------------------------
// SetStatusOnViewActivationL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::SetStatusOnViewActivationL( MGlxMediaList* aList )
     {
    TRACER("CGlxToolbarController::SetStatusOnViewActivationL");
    
    CGlxNavigationalState* navigationalState =
            CGlxNavigationalState::InstanceL();
    CleanupClosePushL(*navigationalState);
    if (navigationalState->ViewingMode() == NGlxNavigationalState::EBrowse)
        {
        iToolbar->SetItemDimmed(EGlxCmdSlideshowPlay, EFalse, ETrue);
        iToolbar->SetItemDimmed(EGlxCmdStartMultipleMarking, EFalse, ETrue);
        TBool dimmed = aList->SelectionCount() ? EFalse : ETrue;
        iToolbar->SetItemDimmed(EGlxCmdSend, dimmed, ETrue);
        iToolbar->SetItemDimmed(EGlxCmdUpload, dimmed, ETrue);
        }
    else if (navigationalState->ViewingMode() == NGlxNavigationalState::EView)
        {
        SetToolbarItemsDimmed(EFalse);
        }
    CleanupStack::PopAndDestroy(navigationalState);

    // When going back from fullscreen to grid, when the attributes are already 
    // available in the cache, there is no HandleAttributeAvailable callback. Hence,
    // checking for medialist count.
    if (!iAttributeAvailable)
        {
        SetStatusL(aList);
        }
    }

//----------------------------------------------------------------------------
//SetStatusL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::SetStatusL(MGlxMediaList* aList)
	{
    TRACER("CGlxToolbarController::SetStatusL");

	GLX_LOG_INFO1("CGlxToolbarController::SetStatusL(%d)", aList->Count());
    // When going back from fullscreen to grid, when the attributes are already 
    // available in the cache, there is no HandleAttributeAvailable callback. Hence,
    // checking for medialist count and backward navigation
	CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
	CleanupClosePushL(*uiUtility);
	if (aList->Count() <= 0 && (uiUtility->ViewNavigationDirection()
			== EGlxNavigationBackwards))
		{
		SetToolbarItemsDimmed(ETrue);
		}
	else if (KErrNotFound != aList->FocusIndex())
		{
		CGlxNavigationalState* navigationalState =
				CGlxNavigationalState::InstanceL();
		CleanupClosePushL(*navigationalState);
		if (navigationalState->ViewingMode() == NGlxNavigationalState::EView)
			{
			SetToolbarItemsDimmed(EFalse);
			}
		else if (navigationalState->ViewingMode()
				== NGlxNavigationalState::EBrowse)
			{
			TBool dimmed = aList->SelectionCount() ? EFalse : ETrue;
			iToolbar->SetItemDimmed(EGlxCmdSend, dimmed, ETrue);
			iToolbar->SetItemDimmed(EGlxCmdUpload, dimmed, ETrue);
			}
		CleanupStack::PopAndDestroy(navigationalState);
		}
	CleanupStack::PopAndDestroy(uiUtility);
	}

//----------------------------------------------------------------------------
// EnableLatch
//----------------------------------------------------------------------------
//
void CGlxToolbarController::EnableLatch( TInt aCommandId, TInt aLatched )
    {
    TRACER("CGlxToolbarController::EnableLatch()");
    GLX_DEBUG2("CGlxToolbarController::EnableLatch() aLatched=%d", aLatched);

    CAknButton* toolbarButton =
            static_cast<CAknButton*> (iToolbar->ControlOrNull(aCommandId));

    if( toolbarButton )
        {
        toolbarButton->SetCurrentState(aLatched, ETrue);
        }
    }

// ----------------------------------------------------------------------------
// HandlePopulatedL
// ----------------------------------------------------------------------------
//
void CGlxToolbarController::HandlePopulatedL( MGlxMediaList* aList )
    {
    TRACER("CGlxToolbarController::HandlePopulatedL()");
    if (aList->Count() <= 0)
        {
        SetToolbarItemsDimmed(ETrue);
        }
    if (!iToolbar->IsVisible())
        {
        GLX_DEBUG1("CGlxToolbarController::HandlePopulatedL()"
                " - SetToolbarVisibility(ETrue)");       
        iToolbar->SetToolbarVisibility(ETrue);
        } 
    }

// ----------------------------------------------------------------------------
// SetToolbarItemsDimmed
// ----------------------------------------------------------------------------
//
void CGlxToolbarController::SetToolbarItemsDimmed(TBool aDimmed)
    {
    TRACER("CGlxToolbarController::SetToolbarItemsDimmed()");
    if (iToolbar)
        {
        iToolbar->SetItemDimmed(EGlxCmdSlideshowPlay, aDimmed, ETrue);
        iToolbar->SetItemDimmed(EGlxCmdStartMultipleMarking, aDimmed, ETrue);
        iToolbar->SetItemDimmed(EGlxCmdSend, aDimmed, ETrue);
        iToolbar->SetItemDimmed(EGlxCmdUpload, aDimmed, ETrue);
        }
    }
//end of file
	

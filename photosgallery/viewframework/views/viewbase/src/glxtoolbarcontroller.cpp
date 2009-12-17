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
#include <mglxmedialist.h>

#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>
const TInt KGlxToolbarButtonUnLatched = 0;   // As per the states in resource file
const TInt KGlxToolbarButtonLatched = 1 ;

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
    iToolbar->SetDimmed(ETrue);
    iToolbar->DrawNow();
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
        iToolbar->SetDimmed(ETrue);
        iToolbar->DrawNow();
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
void CGlxToolbarController::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt aNewIndex, TInt /*aOldIndex*/, 
        MGlxMediaList* aList)
    {  
    TRACER("CGlxToolbarController::HandleFocusChangedL");
    
    // If new index is not equal to -1 (i.e., if there are some items present), 
    // then check the media item
    if( (aNewIndex == aList->FocusIndex()) && (KErrNotFound != aNewIndex) )
        {
        //Get the current media item from medialist
        const TGlxMedia& mediaItem = aList->Item(aList->FocusIndex());
        //Check whether media item is a system item, for example Favourites album
        TBool isSystemItem = EFalse;
        mediaItem.GetSystemItem(isSystemItem);
        if( mediaItem.IsStatic() )
            {
            iToolbar->SetDimmed(ETrue);  
            }
        else if( iToolbar->IsDimmed() )
            {
            iToolbar->SetDimmed(EFalse);
            }
        if( isSystemItem )
             {
             iToolbar->SetItemDimmed( EGlxCmdRename, ETrue, ETrue );
             }
        else
             {
             iToolbar->SetItemDimmed( EGlxCmdRename, EFalse, ETrue );   
            }
        }
    //DrawNow must be called since SetDimmed does not redraw the toolbar
    iToolbar->DrawNow();     
    }

//----------------------------------------------------------------------------
// HandleItemSelectedL
//----------------------------------------------------------------------------
//
void CGlxToolbarController::HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, 
        MGlxMediaList* aList)
    {
    TRACER("CGlxToolbarController::HandleItemSelectedL");
    
    // If atleast 1 item is marked, or if Mark All is called, then Latch the mark button.
    if((aList->SelectionCount() == 1 ) || (aList->SelectionCount() == aList->Count()))
        {
        EnableLatch( EGlxCmdStartMultipleMarking, ETrue );
        }
	/* single clk chngs
	 * check the current view status.
	 * need to disable upload button in toolbar for grid by default
	 */
	if(!IsFullScreenView())
        {
        if(aList->SelectionCount() >= 1 )
            {
            iToolbar->SetItemDimmed(EGlxCmdUpload,EFalse , ETrue);
            }
        else
            {
            iToolbar->SetItemDimmed(EGlxCmdUpload,ETrue , ETrue);
            }    
        }
    
    //DrawNow must be called since SetDimmed does not redraw the toolbar
    iToolbar->DrawNow();    
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
     	
     // When going back from fullscreen to grid, when the attributes are already 
     // available in the cache, there is no HandleAttributeAvailable callback. Hence,
     // checking for medialist count.
     if( !iAttributeAvailable && (aList->Count(NGlxListDefs::ECountNonStatic) > 0))
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
	if(KErrNotFound != aList->FocusIndex())
		{
		//Get the current media item from medialist
		const TGlxMedia& mediaItem = aList->Item(aList->FocusIndex());      

		//Check whether media item is a system item, for example Favourites album
		TBool isSystemItem = EFalse;    
		mediaItem.GetSystemItem(isSystemItem);

		if(!mediaItem.IsStatic() && iToolbar->IsDimmed())
			{
			//Activate (Undim) the toolbar if item in focus is not static
			iToolbar->SetDimmed(EFalse);            
			}    


		if( isSystemItem )
			{
			iToolbar->SetItemDimmed( EGlxCmdRename, ETrue, ETrue );
			}
		else
			{
			iToolbar->SetItemDimmed( EGlxCmdRename, EFalse, ETrue );
			}    
		}    
	/* single clk chngs
	 * enable/disable upload button as per selection count, only in grid view
	 */
	if(aList->SelectionCount()== 0 && !IsFullScreenView())
	    {
	    iToolbar->SetItemDimmed(EGlxCmdUpload,ETrue , ETrue);
	    }
	
	//DrawNow must be called since SetDimmed does not redraw the toolbar
	iToolbar->DrawNow();
	}

//----------------------------------------------------------------------------
// EnableLatch
//----------------------------------------------------------------------------
//
void CGlxToolbarController::EnableLatch( TInt aCommandId, TInt aLatched )
    {
    CAknButton* toolbarButton = static_cast<CAknButton*>
                                (iToolbar->ControlOrNull( aCommandId ));

    if( toolbarButton )
        {           
        if(aLatched)
            {
            toolbarButton->SetCurrentState( KGlxToolbarButtonLatched, ETrue );
            }
        else
            {
            toolbarButton->SetCurrentState( KGlxToolbarButtonUnLatched, ETrue );
            }
        }
    }
//----------------------------------------------------------------------------
// Check for current view mode .Grid/fullscreen/imgviewer
//----------------------------------------------------------------------------
//
TBool CGlxToolbarController::IsFullScreenView()
    {
    TBool fullscreenViewingMode = EFalse;
             
     CGlxNavigationalState* aNavigationalState = CGlxNavigationalState::InstanceL();
     CMPXCollectionPath* naviState = aNavigationalState->StateLC();
     
     if ( naviState->Levels() >= 1)
         {
         if (aNavigationalState->ViewingMode() == NGlxNavigationalState::EBrowse) 
             {
             // For image viewer collection, goto view mode
             if (naviState->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
                 {
                 // current view mode is img vwr
                 fullscreenViewingMode = ETrue;
                 }
             else
                 {
                 //current view mode is Grid 
                 fullscreenViewingMode = EFalse;
                 }
             } 
         else 
             {
             //current view mode is Fullscreen
             fullscreenViewingMode = ETrue;
             }                
         }
     CleanupStack::PopAndDestroy( naviState );
     aNavigationalState->Close();
     return fullscreenViewingMode;
    }
//end os file
	

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




#include "glxmskcontroller.h"
#include <glxtracer.h>                         // For Logs
#include <mglxmedialist.h>
#include <glxlistviewplugin.rsg>
#include "glxcommandhandlers.hrh"
#include <avkon.rsg>
#include <glxviewbase.rsg>
#include <StringLoader.h>


//----------------------------------------------------------------------------------
// NewL
//----------------------------------------------------------------------------------
//
CGlxMSKController* CGlxMSKController::NewL()
    {
    TRACER("CGlxToolbarController::NewL");
    
    CGlxMSKController *self = new( ELeave ) CGlxMSKController ();
    return self;
    }

//----------------------------------------------------------------------------------
// Default Constructor
//----------------------------------------------------------------------------------
//
CGlxMSKController::CGlxMSKController()
                    
    {
    TRACER("CGlxMSKController::CGlxMSKController");
    iUiUtility = CGlxUiUtility::UtilityL();
    }
    
//----------------------------------------------------------------------------------
// AddToObserver
//----------------------------------------------------------------------------------
//
void CGlxMSKController::AddToObserverL (MGlxMediaList& aList, CEikButtonGroupContainer* aCba)
    {
    TRACER("CGlxMSKController::AddToObserverL");

    iCba = aCba;
    iAttributeAvailable = EFalse;
    aList.AddMediaListObserverL ( this );
    }

//----------------------------------------------------------------------------------
// RemoveFromObserver 
//----------------------------------------------------------------------------------
//
void CGlxMSKController::RemoveFromObserver (MGlxMediaList& aList)
    {
    TRACER("CGlxMSKController::RemoveFromObserver");

    aList.RemoveMediaListObserver ( this );
    }

//----------------------------------------------------------------------------
// HandleItemAddedL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMSKController::HandleItemAddedL");
    // no implementation
    }

//----------------------------------------------------------------------------
// HandleMediaL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMSKController::HandleMediaL");
    // no implementation
    }

//----------------------------------------------------------------------------
// HandleItemRemovedL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleItemRemovedL(TInt /*aStartIndex*/, 
        TInt /*aEndIndex*/, MGlxMediaList* aList)
    {
    TRACER("CGlxMSKController::HandleItemRemovedL");
    
    if( aList->Count() <= 0 )
        {
        iCba->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_BACK);
        iCba->DrawNow();
        }
    }

//----------------------------------------------------------------------------
// HandleItemModifiedL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMSKController::HandleItemModifiedL");
    // no implementation
    }

//----------------------------------------------------------------------------
// HandleAttributesAvailableL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleAttributesAvailableL(TInt aItemIndex, 
        const RArray<TMPXAttribute>& /*aAttributes*/, 
        MGlxMediaList* aList)
    {
    TRACER("CGlxMSKController::HandleAttributesAvailableL");
     
    if( aItemIndex == aList->FocusIndex() )
        {        
        iAttributeAvailable = ETrue;
        SetStatusL(aList);
        }
    const TGlxMedia& mediaItem = aList->Item(aList->FocusIndex());      

    if( mediaItem.IsStatic() && aList->SelectionCount() <= 0 )
        {
        HBufC* openbuf =  StringLoader::LoadLC(R_GLX_MSK_OPEN);
        TPtr textopenptr = openbuf->Des();
               
        iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                EAknCmdOpen, textopenptr );
        iCba->DrawNow();
        CleanupStack::PopAndDestroy(openbuf);
        }
    }

//----------------------------------------------------------------------------
// HandleFocusChangedL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt aNewIndex, TInt /*aOldIndex*/, 
        MGlxMediaList* aList)
    {  
    TRACER("CGlxMSKController::HandleFocusChangedL");

    // If new index is not equal to -1 (i.e., if there are items present), 
    // then check the media item

    if( (KErrNotFound != aNewIndex) && (aNewIndex == aList->FocusIndex()) )
        {
        SetStatusL(aList);
        }
    }

//----------------------------------------------------------------------------
// HandleItemSelectedL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleItemSelectedL(TInt /*aIndex*/, TBool aSelected, 
        MGlxMediaList* aList)
    {
    TRACER("CGlxMSKController::HandleItemSelectedL");
    
    if( !aSelected )
        {
        SetStatusL(aList);
        }
    }

//----------------------------------------------------------------------------
// HandleMessageL
//----------------------------------------------------------------------------
//
void CGlxMSKController::HandleMessageL(const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMSKController::HandleMessageL");
    // no implementation
    }

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
//
CGlxMSKController::~CGlxMSKController()
    {
    TRACER("CGlxMSKController::~CGlxMSKController");
    if ( iUiUtility )
        {
        iUiUtility->Close();
        iUiUtility = NULL;
        }
    }

//----------------------------------------------------------------------------
// SetStatusOnViewActivationL
//----------------------------------------------------------------------------
//
void CGlxMSKController::SetStatusOnViewActivationL( MGlxMediaList* aList )
    {
    TRACER("CGlxMSKController::SetStatusOnViewActivationL");

    // When going back from fullscreen to grid, when the attributes are already 
    // available in the cache, there is no HandleAttributeAvailable callback. Hence,
    // checking for medialist count.
    if( !iAttributeAvailable && (aList->Count(NGlxListDefs::ECountNonStatic) > 0))
        {
        SetStatusL(aList);
        }
    if( aList->Count() == 0 )
        {
        iCba->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_BACK);
        iCba->DrawNow();
        }
    }

//----------------------------------------------------------------------------
//SetStatusL
//----------------------------------------------------------------------------
//
void CGlxMSKController::SetStatusL(MGlxMediaList* aList)
    {
    TRACER("CGlxMSKController::SetStatusL");
   
    if( !aList->SelectionCount() > 0 )
        {
        //Get the current media item from medialist
        const TGlxMedia& mediaItem = aList->Item(aList->FocusIndex());      
        TBool isSystemItem = EFalse;    
        mediaItem.GetSystemItem(isSystemItem);

        HBufC* openbuf =  StringLoader::LoadLC(R_GLX_MSK_OPEN);
        TPtr textopen = openbuf->Des();

        HBufC* playbuf =  StringLoader::LoadLC(R_GLX_MSK_PLAY);
        TPtr textplay = playbuf->Des();
        
        // Check whether media item is a system item, for example Favourites album 
        // or a static item
        if( isSystemItem || mediaItem.IsStatic() )
            {
            iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    EAknCmdOpen, textopen );
            iCba->DrawNow();
            }

        else
            {
            switch (mediaItem.Category())
                {
                case EMPXImage:
                    {
                     if (!iUiUtility->IsExitingState())
			            {
			             // When Marking is done and Exit key is pressed, we do not need to set
			             // MSK, as the application is exiting.
		                iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
			                            EAknCmdOpen, textopen );
			            iCba->DrawNow();
			            }
                    break;
                    }
                case EMPXVideo:
                    {
                     // When Marking is done and Exit key is pressed, we do not need to set
			         // MSK, as the application is exiting.
                     if (!iUiUtility->IsExitingState())
			            {
					    iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
			                            EAknCmdOpen, textplay );
			            iCba->DrawNow();
			            }
	                break;
                    }
                case EMPXMonth:
                    {
                    iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                            EAknCmdOpen, textopen );
                    iCba->DrawNow();
                    break;
                    }
                case EMPXAlbum:
                    {
                    iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                            EAknCmdOpen, textopen );
                    iCba->DrawNow();
                    break;
                    }
                case EMPXTag:
                    {
                    iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                            EAknCmdOpen, textopen );
                    iCba->DrawNow();
                    break;
                    }
                default:
                    {
                    iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                            EAknCmdOpen, textopen );
                    iCba->DrawNow();
                    break;
                    }                   
                }
            }
        CleanupStack::PopAndDestroy(playbuf);
        CleanupStack::PopAndDestroy(openbuf);
        }
    }

//----------------------------------------------------------------------------
//SetMainStatusL
//----------------------------------------------------------------------------
//
void CGlxMSKController::SetMainStatusL()
    {
    HBufC* openbuf =  StringLoader::LoadLC(R_GLX_MSK_OPEN);
    TPtr textopen = openbuf->Des();
        
    iCba->SetCommandL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknCmdOpen, textopen );
    iCba->DrawNow();
    CleanupStack::PopAndDestroy(openbuf);
    }
    

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
* Description:    Marking command handler
*
*/




/**
 *  @internal reviewed 12/06/2007 by Alex Birkett
 */
 
#include "glxcommandhandlermarking.h"

#include <mglxmedialist.h>

#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include <glxcommandhandlermarking.rsg>
#include <glxuiutility.h>
#include <glxscreenfurniture.h>
#include <glxtracer.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxvisuallistmanager.h>

#include <StringLoader.h>

#include "glxcommandhandlers.hrh"
#include "glxmarkediconmanager.h"
#include <glxicons.mbg>


_LIT(KGlxMarkingCmdHandlerRes,"glxcommandhandlermarking.rsc");
_LIT(KGlxBlankString," ");

            
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerMarking::CGlxCommandHandlerMarking(
                MGlxMediaListProvider* aMediaListProvider,
                TBool aHasToolbarItem /*,TBool aUpdateMiddleSoftkey*/ )
    : CGlxMediaListCommandHandler( aMediaListProvider, aHasToolbarItem )/*,
        iKeyHandler( aKeyHandler ), iUpdateMiddleSoftkey( aUpdateMiddleSoftkey )*/
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerMarking::ConstructL()
    {
    TRACER("CGlxCommandHandlerMarking::ConstructL");
    // Load the view's resources
    TFileName resourceFile(KDC_APP_RESOURCE_DIR);
    resourceFile.Append(KGlxMarkingCmdHandlerRes); 
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);
    
    // Add supported commands
    // mark
    TCommandInfo markCmd( EAknCmdMark );
    // Filter out static items
    markCmd.iMinSelectionLength = 1;
    AddCommandL( markCmd );
    
    // unmark
    TCommandInfo unMarkCmd( EAknCmdUnmark );
    // Filter out static items
    unMarkCmd.iMinSelectionLength = 1;
    AddCommandL( unMarkCmd );
    
    // mark all
    AddCommandL( TCommandInfo( EAknMarkAll ) );
    
    // unmark all
    TCommandInfo unMarkAllCmd( EAknUnmarkAll );
    // Filter out static items
    unMarkAllCmd.iMinSelectionLength = 1;
    AddCommandL( unMarkAllCmd );    
    
    // marking sub-menu
    AddCommandL( TCommandInfo( EGlxCmdMarkingSubmenu ) );
    
    // start multiple marking
    AddCommandL( TCommandInfo( EGlxCmdStartMultipleMarking ) );

    // end multiple marking
    AddCommandL( TCommandInfo( EGlxCmdEndMultipleMarking ) );

    // Add view state dummy commands
    AddCommandL( TCommandInfo( EGlxCmdStateView ) );
    AddCommandL( TCommandInfo( EGlxCmdStateBrowse ) );      
    
    iLeftSoftKeyStatus = EGlxLSKUndefined;
    
    // get pointer to screen furniture
    iUiUtility = CGlxUiUtility::UtilityL();
	if(!iUiUtility->IsPenSupported())
		{
	    iScreenFurniture = iUiUtility->ScreenFurniture();
	    iRskTextCancel = StringLoader::LoadL( R_GLX_SOFTKEY_CANCEL );
	    iRskTextBack = StringLoader::LoadL( R_GLX_SOFTKEY_BACK );
	    iMskTextMark = StringLoader::LoadL( R_GLX_MARKING_MARK );
	    iMskTextUnMark = StringLoader::LoadL( R_GLX_MARKING_UNMARK );        
	    iMskTextOpen = StringLoader::LoadL( R_GLX_SOFTKEY_OPEN );
		}
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerMarking* CGlxCommandHandlerMarking::NewL(
                                MGlxMediaListProvider* aMediaListProvider,
                                TBool aHasToolbarItem/*,
                                TBool aUpdateMiddleSoftkey */)
    {
    CGlxCommandHandlerMarking* self = 
        new (ELeave) CGlxCommandHandlerMarking( aMediaListProvider, aHasToolbarItem 
    								/*, aUpdateMiddleSoftkey */); 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerMarking::~CGlxCommandHandlerMarking()
    {
    /// if (but do test this assumption!)
    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
	if(!iUiUtility->IsPenSupported())		
		{
		delete iRskTextCancel;
	    delete iRskTextBack;
	    delete iMskTextMark;
	    delete iMskTextUnMark;
	    delete iMskTextOpen;
		}

    if(iUiUtility)
        {
        iUiUtility->Close();
        }
    }

// ---------------------------------------------------------------------------
// DoExecuteL
// ---------------------------------------------------------------------------
//
TBool CGlxCommandHandlerMarking::DoExecuteL(TInt aCommand, 
                                            MGlxMediaList& aList)
    {
    TRACER("CGlxCommandHandlerMarking::DoExecuteL");
    TBool consumed = ETrue;
    switch( aCommand)
        {
        case EAknCmdMark:
           {
		   //This is Checking Marking Mode is On or Not 
           if(iMultipleMarking)
               {
               if(!aList.Item(aList.FocusIndex()).IsStatic())
                   {
            		aList.SetSelectedL(aList.FocusIndex(), ETrue);          
                   }
                }
            else 
                {
			    EnterMultipleMarkingModeL();
            	}
            //@ fix for EIZU-7RE43S && ELWU-7RA7NX    
            consumed = EFalse;  // This Command Should goto View For Further Processing    
            break;
            }
        case EAknCmdUnmark:
            {
            if(!aList.Item(aList.FocusIndex()).IsStatic())
                {
                aList.SetSelectedL(aList.FocusIndex(), EFalse);  
				if(!iUiUtility->IsPenSupported())
					{
                	iScreenFurniture->ModifySoftkeyIdL(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition, EAknCmdMark, 0, *iMskTextMark);
					}
				/*commented out this coz, while select/unselect on a perticular item
				 * some events were getting lost. -- sourav                
				 */
				/*if(aList.SelectionCount() <= 0)
		            {
		            iMultipleMarking = EFalse;
		            }*/
		        //@ fix for EIZU-7RE43S && ELWU-7RA7NX    
		        consumed = EFalse; // This Command Should goto View For Further Processing                    
                }
            break;
            }
        case EAknMarkAll:
            {
            if(iMultipleMarking)
                {
            	SelectAllL( ETrue );     
                } 
            else 
                {
                EnterMultipleMarkingModeL();
                }
			//@ fix for EIZU-7RE43S             
            consumed = EFalse; // This Command Should goto View For Further Processing                    
            break;
            }
        case EAknUnmarkAll:
            {
            SelectAllL( EFalse );  
            ExitMultipleMarkingModeL();
            consumed = EFalse;
            break;
            }
		// Its For Handling Commands from ToolBar
        case EGlxCmdStartMultipleMarking:
            {
            if(!iMultipleMarking)
                {
            EnterMultipleMarkingModeL();
            }
            consumed = EFalse;
            break;
            }            
        case EGlxCmdEndMultipleMarking:
            {
            if(iMultipleMarking)
                {
                SelectAllL( EFalse);
                }            
            ExitMultipleMarkingModeL();
            consumed = EFalse;
            break;
            }
       /*     
        case EGlxCmdStateView:
            {
            iInFullScreen = ETrue;
            consumed = EFalse;
            }
            break;

        case EGlxCmdStateBrowse:
            {
            iInFullScreen = EFalse;
            iMskTextResourceId = 0;
            UpdateMiddleSoftkeyContentsL();
            UpdateMiddleSoftkeyStatus();
            consumed = EFalse;
            }
            break;*/
            
        default:
            {
            consumed = EFalse;
            break;
        }
        }
    return consumed;
    }

// ---------------------------------------------------------------------------
// DoIsDisabled
// ---------------------------------------------------------------------------
//
TBool CGlxCommandHandlerMarking::DoIsDisabled(TInt aCommandId, 
                                                MGlxMediaList& aList) const
    {
    TBool disabled = EFalse;
    if( ViewingState() == TCommandInfo::EViewingStateView )
    	{
    	return ETrue;
    	}
    switch (aCommandId)
        {
        case EAknCmdMark:
            {
            // Base class has checked for min selection, so no need to check
            /// for Count() > 0
            disabled = ( aList.Item( aList.FocusIndex() ).IsStatic() ) ||
                    aList.IsSelected( aList.FocusIndex() );
            break;
            }
        case EAknCmdUnmark:
            {
            // disable if current item is not selected
            disabled = !( aList.IsSelected(aList.FocusIndex() ) );
            break;
            }
        case EAknMarkAll:
            {
            // disable if all items are marked
            TInt count = aList.Count( NGlxListDefs::ECountNonStatic );
            TInt selCount = aList.SelectionCount();           
            disabled = ( selCount >= count );
            break;
            }
        case EAknUnmarkAll:
            {
            // set disabled if no items selected
            disabled = ( aList.SelectionCount() == 0 );
            break;
            }
        case EGlxCmdStartMultipleMarking:
            {
            // set disabled if no static items present
            disabled = ( aList.Count( NGlxListDefs::ECountNonStatic ) == 0 );
            break;    
            }
        case EGlxCmdMarkingSubmenu:
            {
            // set disabled if no  static items present
            disabled = ( aList.Count( NGlxListDefs::ECountNonStatic ) == 0 );
            break;    
            }
        default:
            break;
        }
    
    return disabled;
    }

// ---------------------------------------------------------------------------
// DoDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerMarking::DoDynInitMenuPaneL(TInt /*aResourceId*/, 
                                                CEikMenuPane* /*aMenuPane*/)
    {          
    }
  

    
// ---------------------------------------------------------------------------
// DoActivateL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerMarking::DoActivateL(TInt aViewId)
    {
    // store view id
    iViewId = aViewId;
    
    
    // get media list from provider
    MGlxMediaList& mlist = MediaList();
    mlist.AddMediaListObserverL( this );
    }

// ---------------------------------------------------------------------------
// Deactivate
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerMarking::Deactivate()
    {

    MediaList().RemoveMediaListObserver( this );
    if(iMultipleMarking)
        {
        iMultipleMarking = EFalse;
        }
    // set selection to unmarked
    TRAP_IGNORE( SelectAllL( EFalse ) );
    }

// ---------------------------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CGlxCommandHandlerMarking::OfferKeyEventL(const TKeyEvent& 
                                                    aKeyEvent, TEventCode aType)
    {
    
    // Consume zoom key events in multiple marking mode
    // to prevent entering full screen view
    if ( iMultipleMarking && ( EStdKeyIncVolume == aKeyEvent.iScanCode
                            || EStdKeyDecVolume == aKeyEvent.iScanCode) )
        {
        return EKeyWasConsumed;
        }

    TBool shiftKeyPressed = ( aKeyEvent.iModifiers & EModifierShift );
    
    // marking not available in full-screen mode
    if( !iInFullScreen )
        {
        // get media list from provider
        MGlxMediaList& mediaList = MediaList();
        if ( shiftKeyPressed && aType == EEventKeyDown )
            {
            if ( !iBasicMarking )
                {   
                iBasicMarking = ETrue;
                // toggle marking mode based on current marked state
                if ( mediaList.IsSelected( mediaList.FocusIndex() ) )
                    {
                    iMarkingMode=EUnmarking;
                    }
                else
                    {
                    iMarkingMode=EMarking;
                    }
                }
            }
        else if( shiftKeyPressed && aType == EEventKey )
            {
            // handle navi-key select   
            if( aKeyEvent.iScanCode==EStdKeyDevice3 )
                {         
                TInt focIdx = mediaList.FocusIndex();
                
                /// is already in DoExecuteL:
                /// Call ExecuteL with EAknCmdMark or EAknCmdUnmark to avoid duplication.
                /// Replace with following code with 
                /// (void) ExecuteL( mlist.IsSelected( mlist.FocusIndex() ) ? 
                ///     EAknCmdUnmark : EAknCmdMark );
                if( focIdx != KErrNotFound )
                    {
                    if ( mediaList.IsSelected( focIdx ) )
                        {
                        // item is marked so unmark
                        mediaList.SetSelectedL(focIdx, EFalse);
                        }
                    else
                        {
                        // if not static item set selected
                        if( !mediaList.Item(focIdx).IsStatic() )
                            {
                            mediaList.SetSelectedL( focIdx, ETrue ); 
                            }
                        }    
                    }
                              
                return EKeyWasConsumed;
                }
            }
        else if( !shiftKeyPressed && aType == EEventKeyUp )
            {
            // if in basic marking mode exit basic marking mode
            if(iBasicMarking)
                {
                iBasicMarking = EFalse;
				if(!iUiUtility->IsPenSupported())
					{
	                // Change RSK
	                iScreenFurniture->ModifySoftkeyIdL(
	                        CEikButtonGroupContainer::ERightSoftkeyPosition, 
	                        EAknSoftkeyCancel, 0, *iRskTextCancel);

	                // Change MSK
	                iScreenFurniture->ModifySoftkeyIdL(
	                        CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
	                        EAknCmdMark, 0, *iMskTextMark);  
					}  
                }
            }
        else
            {
            } 
        }
  
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleItemAddedL(TInt /*aStartIndex*/, 
                                    TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // No Implementation
    }
    
    
// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleMediaL(TInt /*aListIndex*/, 
                                     MGlxMediaList* /*aList*/)
    {
    
    }

// ---------------------------------------------------------------------------
// HandleItemRemoved
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleItemRemovedL(TInt /*aStartIndex*/, 
                                    TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    //UpdateMiddleSoftkeyStatus();
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleItemModifiedL(const RArray<TInt>& 
                                    /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* aList)
    {
	TRACER("CGlxCommandHandlerMarking::HandleAttributesAvailableL");
	
	if(!iUiUtility->IsPenSupported())
		{
	    if( aItemIndex == aList->FocusIndex() && iMultipleMarking)
	        {
	        if(aList->Item(aItemIndex).IsStatic())
	            {
	            iScreenFurniture->ModifySoftkeyIdL(
	                    CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
	                    EAknSoftkeyEmpty, 0, KGlxBlankString);    
	            }
	        else
	            {
	            if (aList->IsSelected(aItemIndex))
	                {
	                iScreenFurniture->ModifySoftkeyIdL(
	                        CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
	                        EAknCmdUnmark, 0, *iMskTextUnMark);    
	                }
	            else
	                {
	                iScreenFurniture->ModifySoftkeyIdL(
	                        CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
	                            EAknCmdMark, 0, *iMskTextMark);    
	                }
	            	}
	        	}
			}
    }

// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, 
                        TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList)
    {
    TRACER("CGlxCommandHandlerMarking::HandleFocusChangedL");
    if( iBasicMarking )
        {
        TInt startIdx = 0;
        TInt endIdx = 0;
        
        TInt count = aList->Count();
        
        switch (aType)
            {
            case NGlxListDefs::EForward:
                {
                startIdx = aOldIndex;
                endIdx = aNewIndex;
                
                // if need to go off end of list add count to end index
                // so we can iterate sequentally through items
                if( aNewIndex < aOldIndex )
                    {
                    endIdx += count;   
                    }            
                }
                break;
            case NGlxListDefs::EBackward:
                {
                startIdx = aNewIndex;
                endIdx = aOldIndex;
                
                // if need to go off end of list add count to end index
                // so we can iterate sequentally through items
                if( aOldIndex < aNewIndex )
                    {
                    endIdx += count;  

                    }          
                }
                break;
            // do nothing if focus type is unknown
            case NGlxListDefs::EUnknown:
            // fallthrough
            default:
                break;
            }
        
        // set items in range to selected    
        if( startIdx != endIdx )
            {
            TInt iterator = startIdx;
            
            while( iterator <= endIdx )
                {
                // use modulus of count to take account
                // of iterator range going off end of list
                TInt listIndex = iterator % count;
                SetSelectedL(aList, listIndex);    
                iterator++;
                }
            }
        }
    else
        {
		if(!iUiUtility->IsPenSupported())
			{
	        if (aNewIndex >=0)
	        {
	            // if in multiple marking mode update softkey for current
	            // focussed item
	            // Update MSK
	            if(aList->Item(aNewIndex).IsStatic())
	                {
	                iScreenFurniture->ModifySoftkeyIdL(
	                        CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
	                        EAknSoftkeyEmpty, 0, KGlxBlankString);    
	                }
	            else
	                {
	                if (aList->IsSelected(aNewIndex))
	                    {
	                    iScreenFurniture->ModifySoftkeyIdL(
	                            CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
	                            EAknCmdUnmark, 0, *iMskTextUnMark);    
	                    }
	                else
	                    {
	                    iScreenFurniture->ModifySoftkeyIdL(
	                            CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
	                                EAknCmdMark, 0, *iMskTextMark);    
						if(!iMultipleMarking && (0 == aList->SelectionCount()))	                                
							{
							ExitMultipleMarkingModeL();
							}
	                    }
	                }
	            }
			}
        }
    }

// ---------------------------------------------------------------------------
// HandleItemSelected
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleItemSelectedL(TInt aIndex, 
                                TBool /*aSelected*/, MGlxMediaList* aList)
    {
    TRACER("CGlxCommandHandlerMarking::HandleItemSelectedL");
    /**
    If in the multiple marking mode, and the item selected is the focussed 
    item, HandleItemSelected causes the left softkey to be updated to 
    correspond to the marked state of the focussed item. If the item is 
    marked the softkey is changed to "Unmark" and vice versa.
    */
    
	if(!iUiUtility->IsPenSupported())
		{
    	if( aIndex == aList->FocusIndex() )
        	{
        	// Update MSK
        	iScreenFurniture->ModifySoftkeyIdL(
                CEikButtonGroupContainer::EMiddleSoftkeyPosition, EAknCmdUnmark, 0, *iMskTextUnMark);    

        	} 
		}
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
void CGlxCommandHandlerMarking::HandleMessageL(const CMPXMessage& /*aMessage*/, 
                                                    MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// ClearSelectionL
// ---------------------------------------------------------------------------
//    
void CGlxCommandHandlerMarking::SelectAllL( TBool aSelect)
    {
    TRACER("CGlxCommandHandlerMarking::SelectAllL");
    MGlxMediaList& mlist = MediaList();
    TInt mcount = mlist.Count();
    for ( TInt i=0; i< mcount; i++ )
        {
        mlist.SetSelectedL( i, aSelect );
        }
    }

// ---------------------------------------------------------------------------
// EnterMultipleMarkingModeL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerMarking::EnterMultipleMarkingModeL()
    {
    TRACER("CGlxCommandHandlerMarking::EnterMultipleMarkingModeL");
    
    iMultipleMarking = ETrue;
	
	if(!iUiUtility->IsPenSupported())
		{
	    // change softkeys
	    // Change RSK
	    iScreenFurniture->ModifySoftkeyIdL(
	            CEikButtonGroupContainer::ERightSoftkeyPosition, 
	                EAknSoftkeyCancel, 0, *iRskTextCancel);

	    // Change MSK
	    iScreenFurniture->ModifySoftkeyIdL(
	            CEikButtonGroupContainer::EMiddleSoftkeyPosition, EAknCmdMark, 0, *iMskTextMark);    
		}
    }

// ---------------------------------------------------------------------------
// ExitMultipleMarkingModeL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerMarking::ExitMultipleMarkingModeL()
    {
    TRACER("CGlxCommandHandlerMarking::ExitMultipleMarkingModeL");
    
    iMultipleMarking = EFalse;

	if(!iUiUtility->IsPenSupported())
		{
	    // rsetore softkeys
	    // Change RSK
	    iScreenFurniture->ModifySoftkeyIdL(
	            CEikButtonGroupContainer::ERightSoftkeyPosition,
	                EAknSoftkeyBack, 0, *iRskTextBack);

	    iScreenFurniture->ModifySoftkeyIdL(
	            CEikButtonGroupContainer::EMiddleSoftkeyPosition,
	            EAknCmdOpen, 0, *iMskTextOpen);
		}
    // set basic marking flag to false
    iBasicMarking = EFalse;
    }

// ---------------------------------------------------------------------------
// UpdateMarkedSelection
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerMarking::SetSelectedL(MGlxMediaList* aList, 
                                                                TInt aIndex)
    {
    TRACER("CGlxCommandHandlerMarking::UpdateMarkedSelection");
    
    TBool mark = (iMarkingMode==EMarking);
    aList->SetSelectedL( aIndex, mark );
    }
    
// ---------------------------------------------------------------------------
// PopulateToolbar
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerMarking::PopulateToolbarL()
	{
	TRACER("CGlxCommandHandlerMarking::PopulateToolbar");
	
	}


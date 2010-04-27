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
* Description:    Media List Command Handler
*
*/




#include <e32base.h>

#include "glxmedialistcommandhandler.h"

#include <eikprogi.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessageprogressdefs.h>
#include <StringLoader.h>

#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxfetchcontextremover.h>
#include <glxgeneraluiutilities.h>
#include <glxuiutility.h>
#include <glxmediageneraldefs.h>
#include <glxuistd.h>
#include <mglxmedialist.h>
#include <glxcommandhandlers.hrh>

#include "mglxmedialistprovider.h"

// -----------------------------------------------------------------------------
// Constructor for command info
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMediaListCommandHandler::TCommandInfo::TCommandInfo(TInt aCommandId) 
    {
    // Set command id 
    iCommandId = aCommandId;

    // Do not stop animation before execution
    iStopAnimationForExecution = EFalse;

    // Disable for system items
    iDisallowSystemItems = EFalse;

    // Disable for DRM protected items
    iDisallowDRM = EFalse;

    // Set selection length requirement so that it does not filter
    iMinSelectionLength = 0;
    iMaxSelectionLength = KMaxTInt;

    // default minimum item count
    iMinSlideshowPlayableContainedItemCount = 0;

    // Disable for animated GIFs
    iDisallowAnimatedGIFs = EFalse;

    // Default item category is "none"
    iCategoryFilter = EMPXNoCategory;    

    // Don't apply category filter
    iCategoryRule = EIgnore;
    
    // Set the default viewing state
    iViewingState = EViewingStateUndefined;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMediaListCommandHandler::CGlxMediaListCommandHandler(
	MGlxMediaListProvider* aMediaListProvider,
	TBool aHasToolbarItem):CGlxCommandHandler(aHasToolbarItem)
	{
	iMediaListProvider = aMediaListProvider;
	iCurrentViewingState = TCommandInfo::EViewingStateUndefined;
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMediaListCommandHandler::~CGlxMediaListCommandHandler()
	{
	iCommandInfoArray.Reset();
	}

// -----------------------------------------------------------------------------
// ExecuteL
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMediaListCommandHandler::ExecuteL(TInt aCommandId)
    {
    TBool consume = EFalse;
    
    switch (aCommandId)
        {
        case EGlxCmdStateBrowse:
            iCurrentViewingState = TCommandInfo::EViewingStateBrowse;
            break;
            
        case EGlxCmdStateView:
            iCurrentViewingState = TCommandInfo::EViewingStateView;
            break;

        default:                    // Do nothing
            break;
        }
    /// @todo Consume the state command. This requires changing derived classes so that they do not 
    /// receive the state commands, but use the TCommandInfo filtering for state. Too risky change
    /// to do for inc6.1.
        
    if ( BypassFiltersForExecute() )
        {
        MGlxMediaList& list = MediaList();
        
        consume = DoExecuteL(aCommandId, list);
        }
    else
        {
        // Is this command handled by this command handler
        if ( IsSupported( aCommandId ) )
            {
            MGlxMediaList& list = MediaList();
            
            // Do not consume if the command is disabled
            if ( !IsDisabledL(aCommandId, list) )
                {
                // Ask deriving class to execute
                consume = DoExecuteL(aCommandId, list);
                }
            }
        }
        
    return consume;
    }

// -----------------------------------------------------------------------------
// DynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListCommandHandler::DynInitMenuPaneL(TInt aResourceId, 
        CEikMenuPane* aMenuPane, TBool aIsBrowseMode)
	{
	if ( aMenuPane )
	    {
	    if ( BypassFiltersForMenu() )
	        {
			DoDynInitMenuPaneL(aResourceId, aMenuPane);
	        }
	    else
	        {
    		TInt num_items = aMenuPane->NumberOfItemsInPane();
    		TBool atLeastOneEnabledSupportedItem = EFalse;
            
    		// Iterate through menu pane
    		for ( TInt i = 0; i < num_items; i++)
    			{
    			CEikMenuPaneItem::SData& item = aMenuPane->ItemDataByIndexL(i);

        	    // Check if the menu command is know to this command handler
        	    if ( IsSupported( item.iCommandId ) )
        	        { 
                    TBool isDisabled = EFalse;
					//If we are in grid view check whether to disable the item
					//with a light weight API, since the API IsDisabledL( )
					//takes time to execute randomly over the time, which is
					//not predictable. This is not be required for list view
					//since it has only 2-3 commands and there are no AIW 
					//menu.                        
                    if(aIsBrowseMode)
                        {
                        TBool isContextItem = EFalse;
    					//Check whether its a context menu item.
                        if(item.iFlags & EEikMenuItemSpecific)
                            {
                            isContextItem = ETrue;
                            }
							
                        isDisabled = CheckDisabledForBrowseModeL(item.iCommandId, 
                                MediaList(), isContextItem);
                        }
                    else
                        {
                        isDisabled = IsDisabledL(item.iCommandId, MediaList());
                        }
                    
        	        // Check visibility of the menu item
    				aMenuPane->SetItemDimmed( item.iCommandId, isDisabled);
    				
    				atLeastOneEnabledSupportedItem = atLeastOneEnabledSupportedItem || (!isDisabled);
        	        }
    			}
    			
    	    if ( atLeastOneEnabledSupportedItem )
    	        {
    			DoDynInitMenuPaneL(aResourceId, aMenuPane);
    			}
    	    }
	    }
	}

// ---------------------------------------------------------------------------
// ActivateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxMediaListCommandHandler::DoActivateL(TInt /*aViewId*/)
    {
    
    }
	
// ---------------------------------------------------------------------------
// Default implementation does nothing
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxMediaListCommandHandler::Deactivate()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CGlxMediaListCommandHandler::OfferKeyEventL(
        const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }
    
// ---------------------------------------------------------------------------
// PreDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxMediaListCommandHandler::PreDynInitMenuPaneL( TInt /*aResourceId*/ )
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// CGlxMediaListCommandHandler::GetRequiredAttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxMediaListCommandHandler::GetRequiredAttributesL(
		                            RArray< TMPXAttribute >& aAttributes, 
            		                TBool aFilterUsingSelection, 
            		                TBool aFilterUsingCommandId,
                                    TInt aCommandId) const
    {
    CleanupClosePushL(aAttributes);
    if (!aFilterUsingCommandId || IsSupported(aCommandId))
    	{
    	GetRequiredAttributesL(aAttributes, aFilterUsingSelection);
    	}
    CleanupStack::Pop(&aAttributes);    
    }
    
// -----------------------------------------------------------------------------
// Check if command is currently disabled
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMediaListCommandHandler::IsDisabledL(TInt aCommandId, 
        MGlxMediaList& aMediaList) const 
    {
    __ASSERT_DEBUG(IsSupported(aCommandId), Panic(EGlxPanicIllegalArgument));
    const TCommandInfo& info = CommandInfo(aCommandId);
    
    // Perform expensive checks only if necessary
    TBool isDisabled = IsDisabled(info);
    
    if (!isDisabled)
        {
        	/** 
            * The disabling check must be done so that we go through the selection 
            * and check each item separately against all filter rules.
            *
            * There can for example be a filter that does not allow system items, 
            * anim gifs or videos. Now if the selection contains 10 items and 3 of 
            * them are system items, 4 are anim gifs and 3 are videos we need to disable the 
            * command. If that selection contained 1 "normal" image we must enable
            * the command accordingly.
            */
	        // assume this item is not disabled
	        
    
	        // Check through all selected items, if no selection then the focused item
            TGlxSelectionIterator iterator;
            iterator.SetToFirst( &aMediaList );
            // Loop until iterator does not give any more indexes 
            TInt index = KErrNotFound;
            while ( KErrNotFound != (index = iterator++ ) )
                {
                // get the media item
                const TGlxMedia& item = aMediaList.Item( index );

                // check category rule first
                TMPXGeneralCategory cat = item.Category();
                if( ( TCommandInfo::EIgnore != info.iCategoryRule )&&
                    ( cat != EMPXNoCategory ) )
                    {
                    // get category attribute
                    // did we want to deny all items of this type
                    if( TCommandInfo::EForbidAll == info.iCategoryRule )
                        {
                        // disable if the category was same
                        isDisabled = (info.iCategoryFilter == cat);
                        }
                    else
                        {
                        // assign the value
                        isDisabled = info.iCategoryFilter != cat;
                        // disable if the category was not same
                        if( isDisabled )
                            {
                            // we found first that is not same so end the loop
                            break;
                            }
                        }
                    }
                // Check system item if not yet disabled by previous rules
                TBool isSystemItem = EFalse;
                if( ( !isDisabled )&&( info.iDisallowSystemItems )&&
                    ( item.GetSystemItem(isSystemItem) ) )
                    {
                    // disable if this was system item
                    isDisabled = isSystemItem;
                    }
                // Check DRM if not yet disabled by previous rules
                if( (!isDisabled )&&( info.iDisallowDRM ) )
                    {
                    // disable if DRM protected
                    isDisabled = item.IsDrmProtected();
                    }
                // Check animated GIF if not yet disabled by previous rules
                TInt frameCount(0);
                if( ( !isDisabled )&&( info.iDisallowAnimatedGIFs )&&
                    ( item.GetFrameCount(frameCount) ) )
                    {
                    // disable if framecount is greater than one
                    isDisabled = ( 1 < frameCount );
                    }
                // Check minimum item count if not yet disabled by previous rules
                TInt count(0);
                if( ( !isDisabled )&&( info.iMinSlideshowPlayableContainedItemCount )&&
                    ( item.GetSlideshowPlayableContainedItemCount(count) ) )
                    {
                    // disable if less than required amount of items in container (ignore non-containers, i.e. -1)
                    if( count != -1 )
                        {
                        isDisabled = ( count < info.iMinSlideshowPlayableContainedItemCount );
                        }
                    }
                // when we find the first item for which the command is not disabled
                // we can end the loop unless the category rule states we need to check them all
                if( ( !isDisabled )&&( TCommandInfo::ERequireAll != info.iCategoryRule ) )
                    {
                    // this is the exit of the loop in case there is an item to enable for
                    break;
                    }
                }

            // Check deriving class if we are still not disabled
            if ( !isDisabled )
                {
                isDisabled = DoIsDisabled( aCommandId, aMediaList );
                }
            }
    return isDisabled;
    }

// -----------------------------------------------------------------------------
// MediaList
// -----------------------------------------------------------------------------
//	
EXPORT_C MGlxMediaList& CGlxMediaListCommandHandler::MediaList()
	{
	return iMediaListProvider->MediaList();
	}
	
// -----------------------------------------------------------------------------
// MediaList
// -----------------------------------------------------------------------------
//	
EXPORT_C const MGlxMediaList& CGlxMediaListCommandHandler::MediaList() const
	{
	return const_cast<CGlxMediaListCommandHandler*>(this)->MediaList();
	}

// -----------------------------------------------------------------------------
// Default implementation of elaborate filtering returns EFalse for "not disabled"
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxMediaListCommandHandler::DoIsDisabled(TInt /*aCommandId*/, 
        MGlxMediaList& /*aList*/) const
    {
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// Return selection length. Length is zero if static item is focused
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxMediaListCommandHandler::SelectionLength() const
    {
    const MGlxMediaList& ml = MediaList();
    
    TInt selectionLength = ml.SelectionCount();

    // If there is no selection, treat the focused item as selected, unless
    // it is a static item
    if ( 0 == selectionLength )
        {
        if (ml.Count() > 0) 
            {
            // see if item is a static item
            // get focussed item
            const TGlxMedia&  item = ml.Item(ml.FocusIndex());
            if(!item.IsStatic())
                {
                // not static item - set selection length to 1
                selectionLength = 1;
                }
            }
        }

    return selectionLength;
    }
    
// -----------------------------------------------------------------------------
// Add the command to command array
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListCommandHandler::AddCommandL(
        const CGlxMediaListCommandHandler::TCommandInfo& aCommand)
	{
	// Make sure command not already added
	__ASSERT_DEBUG(!IsSupported(aCommand.iCommandId), Panic(EGlxPanicAlreadyAdded));

	// This function is rarely called, so copying the object should be ok
	iCommandInfoArray.AppendL(aCommand);
	}

// -----------------------------------------------------------------------------
// Return index of command info by command id
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CGlxMediaListCommandHandler::CommandInfoIndex(TInt aCommandId) const
	{
	// Find the index of the command
	TInt index = KErrNotFound;
    TInt count = iCommandInfoArray.Count();
    for (TInt i = 0; i < count; i++)
        {
        if (iCommandInfoArray[i].iCommandId == aCommandId)
            {
            index = i;
            break;
            }
        }
        
    return index;
	}


// -----------------------------------------------------------------------------
// DoDynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListCommandHandler::DoDynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
    {
    // No implementation by default
    }


// -----------------------------------------------------------------------------
// Return command info by id
// -----------------------------------------------------------------------------
//	 
EXPORT_C CGlxMediaListCommandHandler::TCommandInfo& 
        CGlxMediaListCommandHandler::CommandInfo(TInt aCommandId)  
    {
    // Assuming the deriving class knows the command ids it supports
    __ASSERT_DEBUG(IsSupported(aCommandId), Panic(EGlxPanicIllegalArgument));
    return iCommandInfoArray[CommandInfoIndex(aCommandId)];
    }

// -----------------------------------------------------------------------------
// Return command info by id
// -----------------------------------------------------------------------------
//	 
const CGlxMediaListCommandHandler::TCommandInfo& 
        CGlxMediaListCommandHandler::CommandInfo(TInt aCommandId) const
    {
    // Assuming the deriving class knows the command ids it supports
    __ASSERT_DEBUG(IsSupported(aCommandId), Panic(EGlxPanicIllegalArgument));
    return iCommandInfoArray[CommandInfoIndex(aCommandId)];
    }
    
// -----------------------------------------------------------------------------
// return ETrue if command is handled by this command handler
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMediaListCommandHandler::IsSupported(TInt aCommandId) const
    {
    return CommandInfoIndex(aCommandId) != KErrNotFound;
    }

// -----------------------------------------------------------------------------
// BypassFiltersForExecute
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMediaListCommandHandler::BypassFiltersForExecute() const
    {
    // Return false by default: should only return true in special circumstances
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CGlxMediaListCommandHandler::DoGetRequiredAttributesL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListCommandHandler::
        DoGetRequiredAttributesL( RArray<TMPXAttribute>& /*aAttributes*/, TBool /*aFilterUsingSelection*/) const
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// BypassFiltersForMenu
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMediaListCommandHandler::BypassFiltersForMenu() const
    {
    // Return false by default: should only return true in special circumstances
    return EFalse;
    }
            		                
// -----------------------------------------------------------------------------
// ViewingState
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMediaListCommandHandler::TCommandInfo::TViewingState
						CGlxMediaListCommandHandler::ViewingState() const
    {
    return iCurrentViewingState;
    }

// -----------------------------------------------------------------------------
// IsViewable
// -----------------------------------------------------------------------------
//	
TBool CGlxMediaListCommandHandler::IsViewable(const TCommandInfo& aInfo) const
    {
    return aInfo.iViewingState & iCurrentViewingState;
    }

// -----------------------------------------------------------------------------
// CGlxMediaListCommandHandler::IsDisabled
// -----------------------------------------------------------------------------
//	
TBool CGlxMediaListCommandHandler::IsDisabled(const TCommandInfo& aInfo) const
	{
    // Check selection length validitity first, because it is a very fast check,
    // and covers so many situations
    TInt selectionLength = SelectionLength();
    return (selectionLength < aInfo.iMinSelectionLength ||
                       selectionLength > aInfo.iMaxSelectionLength) ||
                       !IsViewable(aInfo);

	}

// ---------------------------------------------------------------------------
// CGlxMediaListCommandHandler::GetRequiredAttributesL
// ---------------------------------------------------------------------------
//
void CGlxMediaListCommandHandler::GetRequiredAttributesL(
		                            RArray< TMPXAttribute >& aAttributes, 
            		                TBool aFilterUsingSelection) const
	{
    // Check selection length validitity first, because it is a very fast check,
    // and covers so many situations

    
    TInt commandCount = iCommandInfoArray.Count();
    
    TBool requireMediaGeneralCategory = EFalse;
    TBool requireMediaDrmProtected = EFalse;
    TBool requireMediaGeneralSystemItem = EFalse;
    TBool requireMediaGeneralFramecount = EFalse;
    TBool requireMediaGeneralSlideshowableContent = EFalse;
    
    for (TInt i = 0; i < commandCount; i++)
        {
        
        if (aFilterUsingSelection && IsDisabled(iCommandInfoArray[i]))
        	{
        	continue; // No need to add attributes if we already know that the command handler is disabled.
        	}
        
        // request category if matters
        if (TCommandInfo::EIgnore != iCommandInfoArray[i].iCategoryRule)
        	{
        	requireMediaGeneralCategory = ETrue;
        	}
        
        // request drm info if matters
        if (iCommandInfoArray[i].iDisallowDRM)
            {
            requireMediaDrmProtected = ETrue;
            }
    	
        // request system item info if matters
        if (iCommandInfoArray[i].iDisallowSystemItems)
            {
            requireMediaGeneralSystemItem = ETrue;
            }
        
        // request animated GIFs if matters
        if (iCommandInfoArray[i].iDisallowAnimatedGIFs )
            {
            requireMediaGeneralFramecount = ETrue;
            }

        // request item count if this filter is specified
        if (iCommandInfoArray[i].iMinSlideshowPlayableContainedItemCount )
            {
            requireMediaGeneralSlideshowableContent = ETrue;
            }
        }
    
    if (requireMediaGeneralCategory)
        {
        aAttributes.AppendL(KMPXMediaGeneralCategory);
        }
    
    if (requireMediaDrmProtected)
        {
        aAttributes.AppendL(KMPXMediaDrmProtected);
        }
    
    if (requireMediaGeneralSystemItem)
        {
        aAttributes.AppendL(KGlxMediaGeneralSystemItem);
        }
    
    if (requireMediaGeneralFramecount)
        {
        aAttributes.AppendL(KGlxMediaGeneralFramecount);
        }
    
    if (requireMediaGeneralSlideshowableContent)
        {
        aAttributes.AppendL(KGlxMediaGeneralSlideshowableContent);
        }
    	
    DoGetRequiredAttributesL(aAttributes, aFilterUsingSelection);
	}

// -----------------------------------------------------------------------------
// Check if the command should be disabled/enabled for grid view
// -----------------------------------------------------------------------------
//  
TBool CGlxMediaListCommandHandler::CheckDisabledForBrowseModeL(TInt aCommandId, 
        MGlxMediaList& aMediaList, TBool aIsContextItem)
        
    {    
    TInt mlCount = aMediaList.Count();
    TInt selectionCount = aMediaList.SelectionCount();
    
    if(mlCount<=0)
        {
		//If no items are present then enable Help and Exit in options
        if(aCommandId != EAknCmdHelp &&
                aCommandId != EAknCmdExit)
            {
            return ETrue;
            }        
        }
		//If count is >0 and if its context menu item then enable them
		//always, Since all the items are static it would not affect
		//the normal behaviour present earlier.
    else if(aIsContextItem)
        {
        return EFalse;
        }
		//If selectionCount is 0 then enable only slideshow, markall
		//help and exit as present currently.		
    else if((selectionCount == 0) && 
            (aCommandId != EAknCmdHelp &&
            aCommandId != EAknCmdExit &&
            aCommandId != EGlxCmdSlideshow &&
            aCommandId != EAknMarkAll &&
            aCommandId != EGlxCmdSlideshowPlay &&
            aCommandId != EGlxCmdSlideshowSettings))
        {
        return ETrue;
        }
		//If selectionCount is same as media count then disable
		//Markall and Details view. If only one item is present
		//then only disable MarkAll.				
    else if((selectionCount == mlCount) && 
            (aCommandId == EAknMarkAll ||
            (aCommandId == EGlxCmdDetails && mlCount>1)))
        {
        return ETrue;
        }             
		//If selectionCount > 1 then disable Details view.
    else if((selectionCount > 1) &&
            (aCommandId == EGlxCmdDetails))
        {
        return ETrue;
        }       
           
    return EFalse;
    }

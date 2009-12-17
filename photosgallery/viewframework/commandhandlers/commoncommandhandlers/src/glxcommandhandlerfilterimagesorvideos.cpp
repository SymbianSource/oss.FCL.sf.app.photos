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
* Description:    Show commmand handler
*
*/




#include "glxcommandhandlerfilterimagesorvideos.h"

#include <eikmenup.h>

#include <glxcommandhandlers.hrh>
#include <glxtracer.h>
#include <glxuiutility.h>
#include <mglxmedialist.h>
#include <mglxmedialistprovider.h>
#include <glxfilterfactory.h>
#include <bldvariant.hrh>   // for feature flags

/**
 * @internal reviewed 08/02/2008 by Rhodri Byles
 */

// ---------------------------------------------------------------------------
// C++ default constructor. Inlined to save a bit of rom, note that for inline
// it needs to be before its use in NewL
// ---------------------------------------------------------------------------
inline 
CGlxCommandHandlerFilterImagesOrVideos::CGlxCommandHandlerFilterImagesOrVideos(
        MGlxMediaListProvider* aMediaListProvider, 
        CGlxUiUtility& aUiUtility )
    : CGlxMediaListCommandHandler( aMediaListProvider ),
    iUiUtility( aUiUtility )
    {
    // No implentation
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerFilterImagesOrVideos*
        CGlxCommandHandlerFilterImagesOrVideos::NewL(
                    MGlxMediaListProvider* aMediaListProvider )
	{
    // get Hui utility, if it fails we leave the whole construction
    CGlxUiUtility* utility = CGlxUiUtility::UtilityL();
    // put utility to cleanupstack in case allocating self fails
    CleanupClosePushL( *utility );
    // we have HUI utility so create the class next, give the
    // dereferenced hui utility for it so self takes ownership
    CGlxCommandHandlerFilterImagesOrVideos* self =
		new( ELeave ) CGlxCommandHandlerFilterImagesOrVideos(
		    aMediaListProvider,
		    *utility );
    // take utility out of stack as ownership transferred to this class
    CleanupStack::PopAndDestroy( utility );
    // perform 2nd phase
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerFilterImagesOrVideos::~CGlxCommandHandlerFilterImagesOrVideos()
	{
	TRACER("CGlxCommandHandlerFilterImagesOrVideos::~CGlxCommandHandlerFilterImagesOrVideos");
	// remember to close the instance of huiutility
    iUiUtility.Close();
	}

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CGlxCommandHandlerFilterImagesOrVideos::ConstructL()
    {
    // create the commands
    TCommandInfo info( EGlxCmdShowAll );
    // show only in Grid view
    info.iViewingState = TCommandInfo::EViewingStateBrowse;
    // Enable for any item count
    info.iMinSelectionLength = 0;
    AddCommandL( info );

    info.iCommandId = EGlxCmdShowImages;
    AddCommandL( info );

    info.iCommandId = EGlxCmdShowVideos;
    AddCommandL( info );

    info.iCommandId = EGlxCmdShow;
    AddCommandL( info );
    }

// -----------------------------------------------------------------------------
// DynInitMenuPaneL
// -----------------------------------------------------------------------------
void CGlxCommandHandlerFilterImagesOrVideos::DynInitMenuPaneL(
    TInt /*aResourceId*/, CEikMenuPane* aMenuPane )
	{
	if( iInFullScreen )
	    {
	    // We are is Full screen so dim/hide the "Show" menu
	    // this dims/hides the cascade as well
        if( aMenuPane )
            {
            TInt pos;
            if ( aMenuPane->MenuItemExists( EGlxCmdShow, pos ) )
                {
                aMenuPane->SetItemDimmed( EGlxCmdShow, ETrue );
                }
            }
	    }
	else
	    {
	    // We are in the Grid View so Filter out the menu options accordingly
	    
	    // Get the orginal filter used by the media list
        CMPXFilter* orgFilter = MediaList().Filter();
    	TGlxFilterItemType currentItemType = EGlxFilterVideoAndImages;
    	
    	// If a filter is used get the current itemtype
    	// If no filter is used the everything is displayed which is the same
    	// as EGlxFilterVideoAndImages
    	if( orgFilter )
    	    {
            if( orgFilter->IsSupported( KGlxFilterGeneralItemType ) )
                {
                currentItemType = *(orgFilter->Value<TGlxFilterItemType>(
                                                KGlxFilterGeneralItemType ) );
                }
    	    }
        // This will contain the Id of the command to dim/hide
    	TInt commandIdToDim = 0;
    	switch( currentItemType )
    	    {
            case EGlxFilterVideoAndImages:
                {
                // As the current filter type is EGlxFilterVideoAndImages we 
                // should remove the "All" option from the menu
                commandIdToDim = EGlxCmdShowAll;
                break;
                }
            case EGlxFilterImage:
                {
                // As the current filter type is EGlxFilterImage we 
                // should remove the "Images" option from the menu
                commandIdToDim = EGlxCmdShowImages;
                break;
                }
            case EGlxFilterVideo:
                {
                // As the current filter type is EGlxFilterVideo we 
                // should remove the "Videos" option from the menu
                commandIdToDim = EGlxCmdShowVideos;
                break;
                }
            default:
                {
                // This case does not happen in real life.
                // If it does then all three options get displayed on the menu
                break;
                }
    	    }

        // Now dim/hide the required menu option
        if (commandIdToDim && aMenuPane)
            {
            TInt pos;
            if ( aMenuPane->MenuItemExists(commandIdToDim, pos) )
                {
                aMenuPane->SetItemDimmed(commandIdToDim, ETrue);
                }
            }
	    }
	}

// -----------------------------------------------------------------------------
// DoActivateL
// -----------------------------------------------------------------------------
void CGlxCommandHandlerFilterImagesOrVideos::DoActivateL( TInt /*aViewId*/ )
	{
	// If the Direction is forwards then we have come from the list view
	// In which case we make sure the filter is displaying both Images and Videos
    TGlxNavigationDirection navDir = iUiUtility.ViewNavigationDirection();
    if( EGlxNavigationForwards == navDir )
        {
        SetItemTypeInMediaListL( EGlxFilterVideoAndImages );
        }
	}

// -----------------------------------------------------------------------------
// ExecuteL
// -----------------------------------------------------------------------------
TBool CGlxCommandHandlerFilterImagesOrVideos::DoExecuteL(
    TInt aCommandId, MGlxMediaList& /*aList*/ )
    {
    TRACER("CGlxCommandHandlerFilterImagesOrVideos::ExecuteL");

    TBool consume = EFalse;
    // Create an Item Type and set it to something invalid (within this context)
    TGlxFilterItemType itemType = EGlxFilterMediaTypeSuppliedInFilter;
    
    switch( aCommandId )
        {
        case EGlxCmdShowAll:
            {
            // Set ItemType to Display All
            itemType = EGlxFilterVideoAndImages;
            consume = ETrue;
            break;
            }
        case EGlxCmdShowImages:
            {
            // Set ItemType to Display Images
            itemType = EGlxFilterImage;
            consume = ETrue;
            break;
            }
        case EGlxCmdShowVideos:
            {
            // Set ItemType to Display Videos
            itemType = EGlxFilterVideo;
            consume = ETrue;
            break;
            }
        case EGlxCmdStateView:
            {
            // We are now in Full Screen mode
            iInFullScreen = ETrue;
            consume = ETrue;
            break;
            }
        case EGlxCmdStateBrowse:
            {
            // We are now in the Grid view
            iInFullScreen = EFalse;
            consume = ETrue;
            break;
            }
        default:    // No need to do anything
            {
            break;        
            }
        }

    // Has the item type been set    
    if( EGlxFilterMediaTypeSuppliedInFilter != itemType )
        {
        // Inform media list of ItemType change
        SetItemTypeInMediaListL( itemType );
        }

    return consume;
    }

// -----------------------------------------------------------------------------
// SetItemTypeInMediaListL
// -----------------------------------------------------------------------------
void CGlxCommandHandlerFilterImagesOrVideos::SetItemTypeInMediaListL(
                                                TGlxFilterItemType aItemType)
    {
	CMPXFilter* currentFilter = MediaList().Filter();
	if( currentFilter )
	    {
	    // Examine the item type used
        if( currentFilter->IsSupported( KGlxFilterGeneralItemType ) )
            {
            TGlxFilterItemType itemType = EGlxFilterVideoAndImages;
            itemType = *(currentFilter->Value<TGlxFilterItemType>(
                                                KGlxFilterGeneralItemType));

            // Is the itemType already set to what we want.
            if( aItemType != itemType )
                {
                // No it's not so change it.
                // Create a TGlxFilterProperties with the correct itemType
                TGlxFilterProperties filterProperties;
                filterProperties.iItemType = aItemType;

                // Create a Filter based on the current filter but with the
                // new itemType
                CMPXFilter* newFilter = TGlxFilterFactory::CreateCombinedFilterL(
                                                                filterProperties, 
                                                                currentFilter, 
                                                                ETrue);
                CleanupStack::PushL(newFilter);

                // Set this filter in the media list
                MediaList().SetFilterL( newFilter );
                CleanupStack::PopAndDestroy( newFilter );
                }
            }
	    }
    else
        {
        // There is no filter set. So everything is displayed
        // Check if this is what we want.
        // If no filter is set and we need to display everything then there is
        // no need to do anything.
        if( EGlxFilterVideoAndImages != aItemType )
            {
            // We do not want all so change it.
            // Create a TGlxFilterProperties with the correct itemType
            TGlxFilterProperties filterProperties;
            filterProperties.iItemType = aItemType;
            
            // Create a Filter with the new itemType
            CMPXFilter* newFilter =
                TGlxFilterFactory::CreateCombinedFilterL( filterProperties );
            CleanupStack::PushL( newFilter );

            // Set this filter in the media list
            MediaList().SetFilterL( newFilter );
            CleanupStack::PopAndDestroy( newFilter );
            }
        }
    }
	
	
//End of file

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
* Description:    Show on map command handler
*
*/




#include "glxcommandhandleraiwshowmap.h"

#include <e32math.h>
#include <glxattributecontext.h>
#include <glxcommandhandlers.hrh>
#include <glxmedia.h>
#include <glxuiutilities.rsg>
#include <glxmediageneraldefs.h>
#include <glxtracer.h>
#include <mglxmedialist.h>
#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>


// For LocationFW
#include <EPos_CPosLandmark.h>
#include <epos_poslandmarkserialization.h>
#include <lbsposition.h>

#include "glxaiwservicehandler.h"

const TInt KGlxAiwShowMapCommandSpace = 0x00000400;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerAiwShowMap* CGlxCommandHandlerAiwShowMap::NewL(
    MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource )
    {
    TRACER("CGlxCommandHandlerAiwShowMap::NewL");
    CGlxCommandHandlerAiwShowMap* self = new ( ELeave )
        CGlxCommandHandlerAiwShowMap( aMediaListProvider, aMenuResource );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxCommandHandlerAiwShowMap::~CGlxCommandHandlerAiwShowMap()
    {
    iBufferArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxCommandHandlerAiwShowMap::CGlxCommandHandlerAiwShowMap(
    MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource )
        : CGlxCommandHandlerAiwBase( aMediaListProvider, aMenuResource )
    {
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwShowMap::DoGetRequiredAttributesL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwShowMap::DoGetRequiredAttributesL
                    (RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const
    {
	if (!aFilterUsingSelection || SelectionLength() <= MaxSelectedItems())
		{
		aAttributes.AppendL(KGlxMediaGeneralLocation);
		}
    }

// -----------------------------------------------------------------------------
// AiwDoDynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwShowMap::AiwDoDynInitMenuPaneL(TInt /*aResourceId*/,
        CEikMenuPane* aMenuPane)
    {
	TInt num_items = aMenuPane->NumberOfItemsInPane();

	// Iterate through menu pane
	TInt i = 0;
	while (i < num_items)
		{
		CEikMenuPaneItem::SData& item = aMenuPane->ItemDataByIndexL(i);

        // "Show on map" cannot be dimmed as AIW resets the flags to not dimmed
        // Have to delete the menu item in this case.
        if (EGlxCmdAiwShowMap == item.iCommandId)
            {
            //Check visibility of the menu item
            //Fix for  ESLM-7XFF9L:Must always be disabled in all cases.
            TBool isDisabled = ETrue;

            // Should it be disabled
            if ( (isDisabled) ||
                 (MediaList().SelectionCount() != 1 &&
                 !IsInFullScreenViewingModeL()) )
                {
                // if so delete it.
                aMenuPane->DeleteMenuItem(EGlxCmdAiwShowMap);
                }
            break;
            }
        ++i;
		}
    }

// -----------------------------------------------------------------------------
// CommandId
// -----------------------------------------------------------------------------
//
TInt CGlxCommandHandlerAiwShowMap::CommandId() const
    {
    TRACER("CGlxCommandHandlerAiwShowMap::CommandId");
    return EGlxCmdAiwShowMap;
    }

// -----------------------------------------------------------------------------
// AiwCommandId
// -----------------------------------------------------------------------------
//
TInt CGlxCommandHandlerAiwShowMap::AiwCommandId() const
    {
    TRACER("CGlxCommandHandlerAiwShowMap::AiwCommandId");
    return KAiwCmdMnShowMap;
    }

// -----------------------------------------------------------------------------
// AiwInterestResource
// -----------------------------------------------------------------------------
//
TInt CGlxCommandHandlerAiwShowMap::AiwInterestResource() const
    {
    TRACER("CGlxCommandHandlerAiwShowMap::AiwInterestResource");
    return R_GLX_AIW_SHOWMAP_INTEREST;
    }

// -----------------------------------------------------------------------------
// AppendAiwParameterL
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAiwShowMap::AppendAiwParameterL(const TGlxMedia& aItem,
                                     CGlxAiwServiceHandler& aAiwServiceHandler)
    {
    TRACER("CGlxCommandHandlerAiwShowMap::AppendAiwParameterL");
    TBool success = EFalse;

    // Cleanup before execution
    iBufferArray.ResetAndDestroy();

    TCoordinate coordinate;
    if (aItem.GetCoordinate(coordinate))
        {
        // Create a landmark from the coordinate
        CPosLandmark* landmark = CPosLandmark::NewLC();
        TRealX noNumber;
        noNumber.SetNaN();   // Not a Number

        TLocality locality( coordinate, noNumber );

        // locality must have valid (ie not NaN) values
        landmark->SetPositionL( locality );

        // Serialize buffer to landmark and append it to AIW parameter
        const HBufC8* landmarkBuf =
            PosLandmarkSerialization::PackL( *landmark );
        // we have ownership of landmarkBuf so add to array
        // for cleanup on our destruction
        iBufferArray.AppendL( landmarkBuf );

         // Append location parameter to the AIW param list
        TAiwVariant landmarkVariant( *landmarkBuf );
        TAiwGenericParam landmarkParam( EGenericParamLandmark, landmarkVariant );
        aAiwServiceHandler.AddParamL(aItem.Id(), landmarkParam );

        success = ETrue;
        CleanupStack::PopAndDestroy( landmark );
        }

    return success;
    }

// -----------------------------------------------------------------------------
// CommandSpace
// -----------------------------------------------------------------------------
//
TInt CGlxCommandHandlerAiwShowMap::CommandSpace() const
    {
    TRACER("CGlxCommandHandlerAiwShowMap::CommandSpace");
    return KGlxAiwShowMapCommandSpace;
    }

// -----------------------------------------------------------------------------
// DoIsDisabled
// -----------------------------------------------------------------------------
//
//
TBool CGlxCommandHandlerAiwShowMap::IsItemWithLocationInfoSelected(MGlxMediaList& aList )
    {
    TRACER("CGlxCommandHandlerAiwShowMap::DoIsDisabled");
    // Disable the "Show on map" menu item if the selection (marked list,
    // or focused item if none marked) does not contain any items
    // that have location data
    TBool isSupported = EFalse;

    // Check through all, if any, selected items, otherwise the focused item
    TGlxSelectionIterator iterator;
    iterator.SetToFirst( &aList );

    // Loop until a supported item is found or until there are no
    // more indexes to process
    TInt index = KErrNotFound;
    while ( KErrNotFound != (index = iterator++ ) && !isSupported )
        {
        // get the media item
        const TGlxMedia& media = aList.Item( index );

        // Test to see if the coordinate is present
        TCoordinate coordinate;
        isSupported = media.GetCoordinate(coordinate);
        }

    // Don't disable the menu item if the property's supported
    return !isSupported;
    }

// -----------------------------------------------------------------------------
// PreDynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwShowMap::PreDynInitMenuPaneL(TInt aResourceId)
	{
	CGlxCommandHandlerAiwBase::PreDynInitMenuPaneL(aResourceId);
	}

// -----------------------------------------------------------------------------
// IsInFullScreenViewingModeL
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAiwShowMap::IsInFullScreenViewingModeL()
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
                //it means we are in img viewer
                fullscreenViewingMode = ETrue;
                }
            }
        else
            {
            //it means we are in Fullscreen
            fullscreenViewingMode = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( naviState );
    aNavigationalState->Close();
    return fullscreenViewingMode;
    }

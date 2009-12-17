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
* Description:    Manager of visual lists
*
*/




/**
 * @internal reviewed 12/06/2007 by Alex Birkett
 */

#include "glxmarkediconmanager.h"

#include <mglxvisuallist.h>
#include <mglxmedialist.h>

#include <alf/alftexture.h>
#include <alf/alfvisual.h>
#include <alf/alfbrush.h>

#include <glxuiutility.h>
#include <glxtracer.h>
#include <glxtexturemanager.h>

#include <aknconsts.h>
#include <avkon.mbg>


const TInt KGlxMarkedIconSize = 13;
const TInt KGlxMarkedIconBorder = 2;

const TInt KGlxMarkingOpaque = 127;

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
CGlxMarkedIconManager* CGlxMarkedIconManager::NewL(MGlxMediaList& aMediaList, 
                                                    MGlxVisualList& aVisualList)
    {
    CGlxMarkedIconManager* self = 
            new(ELeave)CGlxMarkedIconManager(aMediaList, aVisualList);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//  
CGlxMarkedIconManager::~CGlxMarkedIconManager()
    {   
    iMediaList.RemoveMediaListObserver(this);
    iVisualList.RemoveObserver(this);
	if( iBlackTexture )
	    {
	    // call texture manager to remove the texture
	    // Ui Utility is owned by base 
	    if(iUiUtility)
		    {
	        iUiUtility->GlxTextureManager().RemoveTexture( *iBlackTexture );
		    }
	    }
    }

// ---------------------------------------------------------------------------
// SetMultipleMarkingModeIndicatorsL
// ---------------------------------------------------------------------------
//  
void CGlxMarkedIconManager::SetMultipleMarkingModeIndicatorsL(TBool 
                                                    aMultipleModeEnabled)
    {
    TRACER("CGlxMarkedIconManager::SetMultipleMarkingModeIndicatorsL");
    iMultipleMarkingEnabled = aMultipleModeEnabled;
    
    // set/remove opaque overlay on visible unmarked items
    TInt count = iMediaList.Count();
    for(TInt i = 0; i<count; i++)
        {
        if(!iMediaList.IsSelected(i))
            {
            if(iMultipleMarkingEnabled)
                {
                // attempt to add overlay icon to unmarked item
                iVisualList.AddIconL( i, *iBlackTexture, 
                   NGlxIconMgrDefs::EGlxIconTopLeft, ETrue, ETrue, 0 );
                }
            else
                {
                // attempt to remove overlay
                iVisualList.RemoveIcon( i, *iBlackTexture );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//
void CGlxMarkedIconManager::HandleItemAddedL(TInt /*aStartIndex*/,      
                                TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation required   
    }


// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleMediaL(TInt /*aListIndex*/, 
                                                    MGlxMediaList* /*aList*/)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleItemRemovedL(TInt /*aStartIndex*/, 
                                TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleItemModifiedL(const RArray<TInt>& 
                                    /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    // No implementation required
    }
    
// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleAttributesAvailableL(TInt /*aItemIndex*/,     
    const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
            TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleItemSelectedL(TInt aIndex, TBool aSelected, 
                                                    MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMarkedIconManager::HandleItemSelectedL");
    // if selected add tick icon else remove tick item 
    // (assuming item is already marked)
    if(aSelected)
        {
        iVisualList.AddIconL( aIndex, *iMarkTexture, 
                                NGlxIconMgrDefs::EGlxIconTopRight, 
                                ETrue, EFalse, KGlxMarkedIconBorder );

        if(iMultipleMarkingEnabled)
            {
            // attempt to remove overlay
            iVisualList.RemoveIcon( aIndex, *iBlackTexture );
            }
        
        }
    else
        {
        // only 1 icon to choose from
        iVisualList.RemoveIcon( aIndex, *iMarkTexture );

        
        if(iMultipleMarkingEnabled)
            {
            // attempt to add overlay icon to unmarked item
            iVisualList.AddIconL( aIndex, *iBlackTexture, 
                   NGlxIconMgrDefs::EGlxIconTopLeft, ETrue, ETrue, 0 );

            }
        }

    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleMessageL(const CMPXMessage& /*aMessage*/, 
                                            MGlxMediaList* /*aList*/)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleFocusChangedL( TInt /*aFocusIndex*/, 
                    TReal32 /*aItemsPerSecond*/, MGlxVisualList* /*aList*/,
                    NGlxListDefs::TFocusChangeType /*aType*/ )    
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleSizeChanged
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleSizeChanged( const TSize& /*aSize*/, 
                                                MGlxVisualList* /*aList*/ )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleVisualRemoved
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleVisualRemoved( const CAlfVisual* /*aVisual*/, 
                                                 MGlxVisualList* /*aList*/ )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
void CGlxMarkedIconManager::HandleVisualAddedL( CAlfVisual* /*aVisual*/, 
                                    TInt aIndex, MGlxVisualList* /*aList*/ )
    {
    TRACER("CGlxMarkedIconManager::HandleVisualAddedL");
    // add brushes to any new visuals if required
    // aIndex should be index to media list
    if(iMediaList.IsSelected(aIndex))
        {
        iVisualList.AddIconL( aIndex, *iMarkTexture, 
                   NGlxIconMgrDefs::EGlxIconTopRight, 
                   ETrue, EFalse, KGlxMarkedIconBorder );
        }
    else
        {
        // if in multiple marking mode
        // ensure non-selected items have overlay
        if(iMultipleMarkingEnabled)
            {
            iVisualList.AddIconL( aIndex, *iBlackTexture, 
                   NGlxIconMgrDefs::EGlxIconTopLeft, ETrue, ETrue, 0 );
            }
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//  
CGlxMarkedIconManager::CGlxMarkedIconManager(MGlxMediaList& aMediaList, 
                                                MGlxVisualList& aVisualList)
    : CGlxIconManager(aMediaList, aVisualList)
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//  
void CGlxMarkedIconManager::ConstructL()
    {
    TRACER("CGlxMarkedIconManager::ConstructL");
    BaseConstructL();
    
    // create 'tick' icon
    TFileName resFile(KAvkonBitmapFile);
  
    // get texture manager from UI utility (owned by base class)
    CGlxTextureManager& tm = iUiUtility->GlxTextureManager();
    // get mark texture frim texture manager
    iMarkTexture = &(tm.CreateIconTextureL( EMbmAvkonQgn_indi_marked_grid_add, 
                            resFile, TSize( KGlxMarkedIconSize, KGlxMarkedIconSize) ) );
                                    
    
    TRgb overlayColour( KRgbBlack );
    overlayColour.SetAlpha( KGlxMarkingOpaque );
    
    iBlackTexture = &( tm.CreateFlatColourTextureL( overlayColour ) );
        
    // add as observers
    iMediaList.AddMediaListObserverL(this); 
    iVisualList.AddObserverL(this);
    }
    

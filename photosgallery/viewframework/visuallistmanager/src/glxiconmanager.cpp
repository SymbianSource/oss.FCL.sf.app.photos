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
* Description:    Icon manager base class
*
*/




/**
 * @internal reviewed 31/07/2007 by Rowland Cook
 */
 
#include "glxiconmanager.h"

#include "mglxmedialistobserver.h"
#include "mglxvisuallistobserver.h"
#include "mglxvisuallist.h"

#include <eikappui.h>
#include <eikenv.h>
#include <glxlog.h>
#include <glxtracer.h>

#include <glxuiutility.h>
#include "glxtexturemanager.h"
#include <mglxmedialist.h>

#include <alf/alftexture.h>
#include <alf/alfvisual.h>
#include <alf/alfimage.h>
#include <alf/alfimagebrush.h>
#include <alf/alfbrusharray.h>

// -----------------------------------------------------------------------------
// Parameters for icon/brush creation
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxIconManagerParams::TGlxIconManagerParams(TReal32 aTlX, 
                                TReal32 aTlY, TReal32 aBrX, TReal32 aBrY,
                                TReal32 aOpacity)
	:iTlX(aTlX), iTlY(aTlY), iBrX(aBrX), iBrY(aBrY), iOpacity(aOpacity)
	{
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxIconManager::~CGlxIconManager()
	{
	TRACER("CGlxIconManager::~CGlxIconManager");
	GLX_LOG_INFO("CGlxIconManager::~CGlxIconManager");

	if(iUiUtility)
		{
        iUiUtility->Close();
        }
    
    // remove brushes we've added to visuals in visual list
    TInt mcount = iMediaList.Count();
    TInt brushCount = iImageBrushArray.Count();

    for(TInt visIdx =0; visIdx<mcount; visIdx++)
        {
        for(TInt brIdx =0; brIdx<brushCount; brIdx++)
            {
            RemoveFromItem(visIdx, brIdx);
            }
        }
		
	iImageBrushArray.ResetAndDestroy();
	iImageBrushArray.Close();
	}

// -----------------------------------------------------------------------------
// BaseConstructL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::BaseConstructL()
	{
	TRACER("CGlxIconManager::BaseConstructL");
	GLX_LOG_INFO("CGlxIconManager::BaseConstructL");
    iUiUtility = CGlxUiUtility::UtilityL();
	}

// -----------------------------------------------------------------------------
// CreateVisualFromIconL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::CreateVisualFromIconL(TInt aIconResourceId, 
                                                            TDesC& aFilename)
	{
	TRACER("CGlxIconManager::CreateVisualFromIconL");
	GLX_LOG_INFO("CGlxIconManager::CreateVisualFromIconL");
	// create image brush for our icon
	CAlfTexture& iconTexture = iUiUtility->GlxTextureManager().
              CreateIconTextureL(aIconResourceId, aFilename);

    CreateVisualFromTextureL( iconTexture );
	}
	
// -----------------------------------------------------------------------------
// CreateVisualFromIconL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::CreateVisualFromIconL(TInt aIconResourceId, 
                                TDesC& aFilename, TGlxIconManagerParams& aParams)
	{
	TRACER("CGlxIconManager::CreateVisualFromIconL");
	GLX_LOG_INFO("CGlxIconManager::CreateVisualFromIconL");
	// create image brush for our icon
	CAlfTexture& iconTexture = iUiUtility->GlxTextureManager().
              CreateIconTextureL(aIconResourceId, aFilename);

    CreateVisualFromTextureL( iconTexture, aParams );
	}

// -----------------------------------------------------------------------------
// CreateVisualFromTextureL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxIconManager::CreateVisualFromTextureL( CAlfTexture& aTexture )
	{
	TRACER("CGlxIconManager::CreateVisualFromTextureL( CAlfTexture& aTexture )");
	GLX_LOG_INFO("CGlxIconManager::CreateVisualFromTextureL");
    TAlfImage img( aTexture );
    
    CAlfImageBrush* imageBrush = 
        CAlfImageBrush::NewL( *( iUiUtility->Env() ), img );
    CleanupStack::PushL(imageBrush);
    
    imageBrush->SetLayer(EAlfBrushLayerForeground);
    imageBrush->SetBorders(-1,-1,-1,-1);
    iImageBrushArray.AppendL(imageBrush);
    CleanupStack::Pop(imageBrush);
	}

// -----------------------------------------------------------------------------
// CreateVisualFromTextureL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::CreateVisualFromTextureL(
        CAlfTexture& aTexture, TGlxIconManagerParams& aParams )
	{
	TRACER("CGlxIconManager::CreateVisualFromTextureL 2");
	GLX_LOG_INFO("CGlxIconManager::CreateVisualFromTextureL 2");
    TAlfImage img( aTexture );
    img.SetTexCoords(aParams.iTlX, aParams.iTlY, aParams.iBrX, aParams.iBrY );
    
    CAlfImageBrush* imageBrush = 
        CAlfImageBrush::NewL( *iUiUtility->Env(), img );
    CleanupStack::PushL(imageBrush);
    
    imageBrush->SetLayer( EAlfBrushLayerForeground );
    imageBrush->SetBorders(-1,-1,-1,-1);
    
    TAlfTimedValue opacity( aParams.iOpacity, 0 ); 
    imageBrush->SetOpacity( opacity );
    
    iImageBrushArray.AppendL(imageBrush);
    CleanupStack::Pop(imageBrush);
	}

// -----------------------------------------------------------------------------
// AddIconToItemL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::AddIconToItemL(TInt aIndex, TInt aBrushIndex)
	{
	TRACER("CGlxIconManager::AddIconToItemL");
	GLX_LOG_INFO("CGlxIconManager::AddIconToItemL");
	CAlfVisual* vis = iVisualList.Visual(aIndex);
	AddIconToItemL(vis, aBrushIndex);
	}
	
// -----------------------------------------------------------------------------
// AddIconToItemL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::AddIconToItemL(CAlfVisual* aVisual, TInt aBrushIndex)
	{
	TRACER("CGlxIconManager::AddIconToItemL 2");
	GLX_LOG_INFO("CGlxIconManager::AddIconToItemL 2");
	if(aVisual)
		{
		// only add if not already present
		if(BrushPositionInVisual(aVisual, aBrushIndex) == KErrNotFound)
		    {
		    aVisual->EnableBrushesL();	
	        aVisual->Brushes()->AppendL(iImageBrushArray[aBrushIndex],
	                                        EAlfDoesNotHaveOwnership );	
		    }
		}
	}

// -----------------------------------------------------------------------------
// RemoveFromItemL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::RemoveFromItem(TInt aIndex, TInt aIconIndex)
	{
	TRACER("CGlxIconManager::RemoveFromItem");
	GLX_LOG_INFO("CGlxIconManager::RemoveFromItem");
	CAlfVisual* vis = iVisualList.Visual(aIndex);
	RemoveFromItem(vis, aIconIndex);
	}
	
// -----------------------------------------------------------------------------
// RemoveFromItemL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxIconManager::RemoveFromItem(CAlfVisual* aVisual, 
                                                        TInt aIconIndex)
	{
	TRACER("CGlxIconManager::RemoveFromItem 2");
	GLX_LOG_INFO("CGlxIconManager::RemoveFromItem 2");
	if(aVisual && aIconIndex < iImageBrushArray.Count())
		{
		CAlfBrushArray* brushArray = aVisual->Brushes();
		if ( brushArray )
		    {
    		TInt brushCount = brushArray->Count();
    		for(TInt i = 0; i<brushCount;i++)
    			{
    			CAlfBrush* brush = &(brushArray->At(i));
    			if(brush==iImageBrushArray[aIconIndex])
    				{
    				brushArray->Remove(i);
    				break;
    				}
    			}
    	    }
		}
	}

// -----------------------------------------------------------------------------
// BrushPositionInVisual
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CGlxIconManager::BrushPositionInVisual(TInt aListIndex, 
	                                                TInt aBrushIndex)
    {
    TRACER("CGlxIconManager::BrushPositionInVisual");
    GLX_LOG_INFO("CGlxIconManager::BrushPositionInVisual");
    CAlfVisual* vis = iVisualList.Visual( aListIndex );
    return BrushPositionInVisual(vis, aBrushIndex );
    }

// -----------------------------------------------------------------------------
// BrushPositionInVisual
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxIconManager::BrushPositionInVisual( CAlfVisual* aVisual, 
	                                                   TInt aBrushIndex)
    { 
    TRACER("CGlxIconManager::BrushPositionInVisual 2");
    GLX_LOG_INFO("CGlxIconManager::BrushPositionInVisual 2");
    TInt pos = KErrNotFound;
    
	if(aVisual && aBrushIndex < iImageBrushArray.Count())
		{
		CAlfBrushArray* brushArray = aVisual->Brushes();
		if ( brushArray )
		    {
    		TInt brushCount = brushArray->Count();
    		for(TInt i = 0; i<brushCount;i++)
    			{
    			CAlfBrush* brush = &(brushArray->At(i));
    			if(brush==iImageBrushArray[aBrushIndex])
    				{
    			    pos = i;
    				break;
    				}
    			}
			}
		}
	return pos;
    }

// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//	
void CGlxIconManager::HandleItemAddedL(TInt /*StartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleMediaL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleItemRemovedL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleItemModifiedL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}
    
// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleAttributesAvailableL(TInt /*aItemIndex*/,     
    const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}
        
// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleItemSelectedL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleMessageL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleMessageL(const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/)
	{
	// No implementation
	}


// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleFocusChangedL(TInt /*aFocusIndex*/, TReal32 /*aItemsPerSecond*/, MGlxVisualList* /*aList*/, NGlxListDefs::TFocusChangeType /*aType*/)
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleSizeChanged
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleSizeChanged( const TSize& /*aSize*/, MGlxVisualList* /*aList*/)
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleVisualRemoved
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleVisualRemoved( const CAlfVisual* /*aVisual*/,  MGlxVisualList* /*aList*/ )
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// HandleVisualAddedL
// -----------------------------------------------------------------------------
//
void CGlxIconManager::HandleVisualAddedL(CAlfVisual* /*aVisual*/, TInt /*aIndex*/,MGlxVisualList* /*aList*/ )
	{
	// No implementation
	}
	
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxIconManager::CGlxIconManager(MGlxMediaList& aMediaList, MGlxVisualList& aVisList)
    : iMediaList(aMediaList), iVisualList(aVisList)
	{
	// No implementation
	}



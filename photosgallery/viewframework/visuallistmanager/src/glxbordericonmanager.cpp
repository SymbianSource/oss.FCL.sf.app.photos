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
* Description:    Adds a border to icons in visual lists
*
*/




/**
 * @internal reviewed 31/07/2007 by Rowland Cook
 */

#include "glxbordericonmanager.h"
//#include "mglxmedialistobserver.h"
#include "mglxvisuallist.h"

#include <AknsUtils.h>
#include <alf/alfvisual.h>
#include <alf/alfbrush.h>
#include <alf/alfbrusharray.h>
#include <glxuiutility.h>
#include <alf/alfborderbrush.h>

const TInt KGlxBorderIconWidth = 1;
const TInt KGlxBorderIconOffset = 1;

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxBorderIconManager* CGlxBorderIconManager::NewL(
                           MGlxMediaList& aMediaList, MGlxVisualList& aVisualList)
	{
	CGlxBorderIconManager* self = 
	                new(ELeave)CGlxBorderIconManager(aMediaList, aVisualList);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CGlxBorderIconManager::~CGlxBorderIconManager()
	{
	iVisualList.RemoveObserver(this);
	delete iBorderBrush;
	}
	
// ---------------------------------------------------------------------------
// HandleVisualAddedL
// ---------------------------------------------------------------------------
// 
void CGlxBorderIconManager::HandleVisualAddedL( CAlfVisual* aVisual, 
    TInt /*aIndex*/, MGlxVisualList* /*aList*/ )
	{
	AddBorderBrushL(aVisual);
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CGlxBorderIconManager::CGlxBorderIconManager(MGlxMediaList& aMediaList, 
                                            MGlxVisualList& aVisualList)
	: CGlxIconManager(aMediaList, aVisualList)
	{
	// No  implementation
	}

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//	
void CGlxBorderIconManager::ConstructL()
	{
	BaseConstructL();
	
	// add as observer
	iVisualList.AddObserverL(this);
	
    TRgb brushColor;
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), brushColor, 
        KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG12);

    iBorderBrush = 
        CAlfBorderBrush::NewL( 
            *( iUiUtility->Env() ),
            KGlxBorderIconWidth, KGlxBorderIconWidth, 
            KGlxBorderIconOffset, KGlxBorderIconOffset );
                                
    iBorderBrush->SetLayer(EAlfBrushLayerForeground);
    iBorderBrush->SetColor(brushColor);
	
	// check for any visual already present in list
	TInt itemCount = iVisualList.ItemCount();
	for(TInt i = 0; i < itemCount; i++)
	    {
	    AddBorderBrushL(iVisualList.Visual(i));
	    }
	}

    
// ---------------------------------------------------------------------------
// AddBorderBrushL
// ---------------------------------------------------------------------------
//	
void CGlxBorderIconManager::AddBorderBrushL(CAlfVisual* aVisual)
    {
	if ( aVisual )
	    {
    	aVisual->EnableBrushesL();
        	
    	aVisual->Brushes()->AppendL( iBorderBrush, EAlfDoesNotHaveOwnership );
    	}
    }
    
// ---------------------------------------------------------------------------
// SetThumbnailBorderColor
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxBorderIconManager::SetThumbnailBorderColor()
	{
	TRgb brushColor;
    TInt errCode = AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), brushColor, 
        KAknsIIDQsnOtherColors, EAknsCIQsnOtherColorsCG12);
        
	if( KErrNone == errCode )
		{
		iBorderBrush->SetColor( brushColor );	
		}
  }

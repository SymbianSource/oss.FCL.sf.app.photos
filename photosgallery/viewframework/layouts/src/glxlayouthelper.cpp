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
* Description:    Layout to mapping function adapter class
*
*/




#include "glxlayouthelper.h"

#include <alf/alfvisual.h>  // CAlfVisual
#include <glxlayout.h> // MGlxLayout
#include <glxpanic.h> // Panic codes
#include <alf/alftimedvalue.h> // for TAlfRealPoint
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxLayoutHelper::TGlxLayoutHelper( CAlfVisual& aVisual )
	: iLayoutInfo( &aVisual ),
	iVisual( aVisual ),
	iLayout( NULL )
	{
	// drawing origo is in top left corner of the image
	// Alf screen origo is in top left corner so this enables us to use 0,0
	// to get a full screen image onto the center of the screen
	iVisual.SetOrigin( EAlfVisualHOriginCenter, EAlfVisualVOriginCenter );
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayoutHelper::~TGlxLayoutHelper()
    {
    // nothing to do
    }

// -----------------------------------------------------------------------------
// LayoutVisual
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutHelper::LayoutVisual( TInt aIndex, TSize aScreenSize )
	{
	// assert that we have a layout before we do laying out of visual
	if( iLayout )
		{
		// reset the layout info
		iLayoutInfo.Reset( aIndex );
		iLayoutInfo.SetScreenSize( aScreenSize );
		// ask for the layout to update new values
		iLayout->SetLayoutValues( iLayoutInfo );
		}
	}
// -----------------------------------------------------------------------------
// LayoutVisual
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutHelper::LayoutPanVisual( TInt aIndex, TSize aScreenSize )
    {
    // assert that we have a layout before we do laying out of visual
    if( iLayout )
        {
        // reset the layout info
        iLayoutInfo.Reset( aIndex );
        iLayoutInfo.SetScreenSize( aScreenSize );
        }
    }
// -----------------------------------------------------------------------------
// SetLayout
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutHelper::SetLayout( MGlxLayout* aLayout )
	{
	iLayout = aLayout;
	}
// -----------------------------------------------------------------------------
// GetUpdatedVisualSize
// -----------------------------------------------------------------------------
EXPORT_C TSize TGlxLayoutHelper::GetUpdatedVisualSize(  )
	{
	return iLayoutInfo.iVisualSize ;
	}


// -----------------------------------------------------------------------------
// GetUpdatedRectSize
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayoutInfo& TGlxLayoutHelper::GetUpdatedLayoutInfo(  )
    {
    return iLayoutInfo;
    }
//End of File  

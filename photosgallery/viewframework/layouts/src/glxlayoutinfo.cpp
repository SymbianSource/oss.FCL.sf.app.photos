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
* Description:    Layout base classes
*
*/




#include "glxlayoutinfo.h"

// -----------------------------------------------------------------------------
// TGlxLayoutInfo
// -----------------------------------------------------------------------------
TGlxLayoutInfo::TGlxLayoutInfo( CAlfVisual* aVisual )
	: iMappedIndex( 0 ),
	iIndex( 0 ),
	iVisual( aVisual )
	{
	}

// -----------------------------------------------------------------------------
// TGlxLayoutInfoResetter
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayoutInfoResetter::TGlxLayoutInfoResetter()
	: TGlxLayoutInfo( NULL )
	{
	// reset the object
	Reset( 0 );
	}

// -----------------------------------------------------------------------------
// TGlxLayoutInfoResetter
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayoutInfoResetter::TGlxLayoutInfoResetter( CAlfVisual* aVisual )
	: TGlxLayoutInfo( aVisual )  
	{
	// reset the object
	Reset( 0 );
	}

// -----------------------------------------------------------------------------
// SetVisual
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutInfoResetter::SetVisual( CAlfVisual* aVisual )
	{
	iVisual = aVisual;
	}
// -----------------------------------------------------------------------------
// SetScreenSize
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutInfoResetter::SetScreenSize( TSize aSize )
	{
	iScreenSize = aSize;
	}

// -----------------------------------------------------------------------------
// Reset
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutInfoResetter::Reset( TInt aIndex )
	{
	// reset index
	iIndex = aIndex;
	iMappedIndex = iIndex;
	}

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
* Description:    Layout base class
*
*/




#include "glxlayout.h"
#include <glxpanic.h> // Panic codes

// -----------------------------------------------------------------------------
// TGlxLayout class
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayout::TGlxLayout()
    : iNextLayout( NULL ) // no CBase so clear value
    {
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayout::~TGlxLayout()
    {
    }

// -----------------------------------------------------------------------------
// SetNext
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayout::SetNext( MGlxLayout* aLayout )
	{
	iNextLayout = aLayout;
	}

// -----------------------------------------------------------------------------
// Next
// -----------------------------------------------------------------------------
EXPORT_C MGlxLayout* TGlxLayout::Next() const
	{
	return iNextLayout;
	}

// -----------------------------------------------------------------------------
// SetLayoutValues
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayout::SetLayoutValues( TGlxLayoutInfo& aInfo )
	{
	// first set our own values
	DoSetLayoutValues( aInfo );
	
	// do we have a successor
	if( iNextLayout )
		{
		// tell it to set values
		iNextLayout->SetLayoutValues( aInfo );
		}
	}

// -----------------------------------------------------------------------------
// DoMapValue
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayout::DoSetLayoutValues( TGlxLayoutInfo& /*aInfo*/ )
	{
	// by default do nothing
	}

// -----------------------------------------------------------------------------
// CGlxLayout class
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C CGlxLayout::CGlxLayout()
    {
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C CGlxLayout::~CGlxLayout()
    {
    }

// -----------------------------------------------------------------------------
// SetNext
// -----------------------------------------------------------------------------
EXPORT_C void CGlxLayout::SetNext( MGlxLayout* aLayout )
	{
	iLayoutImpl.SetNext( aLayout );
	}

// -----------------------------------------------------------------------------
// Next
// -----------------------------------------------------------------------------
EXPORT_C MGlxLayout* CGlxLayout::Next() const
	{
	return iLayoutImpl.Next();
	}

// -----------------------------------------------------------------------------
// SetLayoutValues
// -----------------------------------------------------------------------------
EXPORT_C void CGlxLayout::SetLayoutValues( TGlxLayoutInfo& aInfo )
	{
	// first call our own method
	DoSetLayoutValues( aInfo );
	// then the rest
	iLayoutImpl.SetLayoutValues( aInfo );
	}

// -----------------------------------------------------------------------------
// DoMapValue
// -----------------------------------------------------------------------------
EXPORT_C void CGlxLayout::DoSetLayoutValues( TGlxLayoutInfo& /*aInfo*/ )
	{
	// by default do nothing
	}


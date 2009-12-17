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
* Description:    Set coefficient layout
*
*/




#include "glxsetcoefficientlayout.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxSetCoefficientLayout::TGlxSetCoefficientLayout()
	{
	__DEBUG_ONLY( _iName = _L( "TGlxSetCoefficientLayout" ) );

	// by default set all coefficients
	iType = TGlxLayoutInfo::ELastEnum;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxSetCoefficientLayout::~TGlxSetCoefficientLayout()
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// SetType
// -----------------------------------------------------------------------------
EXPORT_C void TGlxSetCoefficientLayout::SetType( TGlxLayoutInfo::TLayoutType aType )
	{
	iType = aType;
	}

// -----------------------------------------------------------------------------
// SetCoefficient
// -----------------------------------------------------------------------------
EXPORT_C void TGlxSetCoefficientLayout::SetCoefficient( 
	TReal32 aCoefficient, TInt aMilliseconds)
	{
//	iCoefficient.Set( aCoefficient, aMilliseconds );
	}

// -----------------------------------------------------------------------------
// DoMapValue
// -----------------------------------------------------------------------------
void TGlxSetCoefficientLayout::DoSetLayoutValues( TGlxLayoutInfo& aInfo )
	{
	// set coefficient
	if( iType == TGlxLayoutInfo::ELastEnum )
		{
		// set them all
		for( TInt i = 0; i < TGlxLayoutInfo::ELastEnum; i++ )
			{
			// need to cast the index to enum type for the compiler to be happy
//			aInfo.SetCoefficient( 
//				(TGlxLayoutInfo::TLayoutType)i, iCoefficient.Now() );
			}
		}
	else
		{
		// just set the one specified
//		aInfo.SetCoefficient( iType, iCoefficient.Now() );
		}
	}
	
// -----------------------------------------------------------------------------
// DoChanged
// -----------------------------------------------------------------------------
TBool TGlxSetCoefficientLayout::DoChanged() const
	{
//	return iCoefficient.Changed();
	}
	
// -----------------------------------------------------------------------------
// DoClearChanged
// -----------------------------------------------------------------------------
void TGlxSetCoefficientLayout::DoClearChanged() 
	{
//	iCoefficient.ClearChanged();
	}
	

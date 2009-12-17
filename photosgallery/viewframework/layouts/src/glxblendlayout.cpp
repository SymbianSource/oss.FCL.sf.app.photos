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
* Description:    Layout blender
*
*/




#include "glxblendlayout.h" 
#include <glxpanic.h> // Panic codes

// LOCAL METHODS
namespace
	{
	// helper method to calculate coefficiency for given field
	TReal32 GetCoefficiency(
		TGlxLayoutInfo& aBaseInfo, 
		TGlxLayoutInfo& aDeltaInfo, 
		TGlxLayoutInfo::TLayoutType aType, 
		TInt aIndex )
		{
		/*
		// position, get coefficients
		//TReal32 new_coeff = aDeltaInfo.Coefficient( aType );
		//TReal32 combined_coeff = aBaseInfo.Coefficient( aType );
		// check if this is the first layout chain in the blender
		// and full coefficiency has not been yet reached
		if( aIndex == 0 && combined_coeff < 1.0 )
			{
			// increase the coefficient to reach 1.0
			new_coeff = 1.0 - combined_coeff;
			aBaseInfo.SetCoefficient( aType, 1.0 );
			}
		// if the coefficient was bigger than zero
		// and full coefficiency has not been yet reached
		else if( new_coeff > 0.0 && combined_coeff < 1.0 )
			{
			// dont allow coefficient to exceed 1.0
			if( new_coeff > ( 1.0 - combined_coeff ) )
				{
				new_coeff = 1.0 - combined_coeff;
				}
			// increase the coefficient
			aBaseInfo.SetCoefficient( aType, combined_coeff + new_coeff );
			}
		else
			{
			// coefficient over or underflow
			new_coeff = 0;
			}
		return new_coeff;*/
		} 
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxBlendLayout::TGlxBlendLayout()
	: iChanged( EFalse )	// blender not is changed by default
	{
	__DEBUG_ONLY( _iName = _L( "TGlxBlendLayout" ) );
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxBlendLayout::~TGlxBlendLayout () 
	{
	iLayouts.Close();
	}
	
// -----------------------------------------------------------------------------
// AddLayoutL
// -----------------------------------------------------------------------------
EXPORT_C void TGlxBlendLayout::AddLayoutL( MGlxLayout* aLayout )
	{
	// layout must not be null
	__ASSERT_DEBUG( aLayout, Panic( EGlxPanicNullLayout ) );
	// create layout struct
	TLayout cache;
	cache.iLayout = aLayout;
	__ASSERT_DEBUG( 
		iLayouts.Find( cache, TLayout::Match ) == KErrNotFound, 
		Panic( EGlxPanicAlreadyAdded ) );

	// cant set anything sane to layout info at this point
	iLayouts.AppendL( cache );
	iChanged = ETrue;
	}

// -----------------------------------------------------------------------------
// RemoveLayout
// -----------------------------------------------------------------------------
EXPORT_C void TGlxBlendLayout::RemoveLayout( const MGlxLayout* aLayout )
	{
	TInt count = iLayouts.Count();
	for( TInt i = 0; i < count; i++ )
		{
		if( iLayouts[ i ].iLayout == aLayout )
			{
			// Found it, remove
			iLayouts.Remove( i );
			iChanged = ETrue;
			return;
			}
		}
	// its ok to remove a layout that was already gone, no harm done there
	}

// -----------------------------------------------------------------------------
// RemoveAllLayouts
// -----------------------------------------------------------------------------
EXPORT_C void TGlxBlendLayout::RemoveAllLayouts()
	{
	iLayouts.Close();
	}

// -----------------------------------------------------------------------------
// MapValue
// -----------------------------------------------------------------------------
void TGlxBlendLayout::DoSetLayoutValues( TGlxLayoutInfo& aInfo )
	{
	/*
	TInt count = iLayouts.Count();
	// first set each layouts info fields correctly
	for( TInt i = 0; i < count; i++ )
		{
		// reset the values for the layout info
		//iLayouts[ i ].iLayoutInfo.Reset( aInfo.Index() );
		//iLayouts[ i ].iLayoutInfo.SetVisual( aInfo.Visual() );
		//iLayouts[ i ].iLayoutInfo.SetScreenInfo( aInfo.ScreenInfo() );
		}

	// process each layout
	for( TInt i = 0; i < count; i++ )
		{
		// call the layout (chain) to perform the processing for their own info
		iLayouts[ i ].iLayout->SetLayoutValues( iLayouts[ i ].iLayoutInfo );
		}

	// reset completely the aInfo fields and its coefficients
	// we use the coefficient fields to know when we have enough values for
	// certain field (coefficient reaches 1.0)
	// we need to reset the base values as the cached values in layout chains
	// had default values (which may not ne 0 for all fields)
	aInfo.iPosition.iX = 0;
	aInfo.iPosition.iY = 0;
	aInfo.SetCoefficient( TGlxLayoutInfo::EPosition, 0 );
	aInfo.iSize.iX = 0;
	aInfo.iSize.iY = 0;
	aInfo.SetCoefficient( TGlxLayoutInfo::ESize, 0 );
	aInfo.iOpacity = 0;
	aInfo.SetCoefficient( TGlxLayoutInfo::EOpacity, 0 );
	aInfo.iRotation = 0;
	aInfo.SetCoefficient( TGlxLayoutInfo::ERotation, 0 );

	// then merge the values according to coefficients
	// need to process in reverse order since layout in index 0
	// has the strongest effect
	for( TInt i = count - 1; i >= 0; i-- )
		{
		// position, get coefficient
		TReal32 new_coeff = GetCoefficiency( 
				aInfo, iLayouts[ i ].iLayoutInfo, 
				TGlxLayoutInfo::EPosition, i );
		// if the coefficient was bigger than zero
		if( new_coeff > 0.0 )
			{
			// add new position, multiplied with the coefficient
			aInfo.iPosition.iX += 
				iLayouts[ i ].iLayoutInfo.iPosition.iX * new_coeff;
			aInfo.iPosition.iY += 
				iLayouts[ i ].iLayoutInfo.iPosition.iY * new_coeff;
			}

		// size, get coefficient
		new_coeff = GetCoefficiency( 
				aInfo, iLayouts[ i ].iLayoutInfo, 
				TGlxLayoutInfo::ESize, i );
		// if the coefficient was bigger than zero
		if( new_coeff > 0.0 )
			{
			// add new size, multiplied with the coefficient
			aInfo.iSize.iX += 
				iLayouts[ i ].iLayoutInfo.iSize.iX * new_coeff;
			aInfo.iSize.iY += 
				iLayouts[ i ].iLayoutInfo.iSize.iY * new_coeff;
			}

		// opacity, get coefficient
		new_coeff = GetCoefficiency( 
				aInfo, iLayouts[ i ].iLayoutInfo, 
				TGlxLayoutInfo::EOpacity, i );
		if( new_coeff > 0.0 )
			{
			// add new opacity, multiplied with the coefficient
			aInfo.iOpacity += 
				iLayouts[ i ].iLayoutInfo.iOpacity * new_coeff;
			}

		// rotation, get coefficient
		new_coeff = GetCoefficiency( 
				aInfo, iLayouts[ i ].iLayoutInfo, 
				TGlxLayoutInfo::ERotation, i );
		if( new_coeff > 0.0 )
			{
			// add new rotation, multiplied with the coefficient
			aInfo.iRotation += 
				iLayouts[ i ].iLayoutInfo.iRotation * new_coeff;
			}
		} */
	}

// -----------------------------------------------------------------------------
// DoChanged
// -----------------------------------------------------------------------------
TBool TGlxBlendLayout::DoChanged() const
	{
	// See if even one layout has changed
	/// @todo consider adding the index of current visual to the Changed call
/*	TInt count = iLayouts.Count();
	for( TInt i = 0; i < count; i++ )
		{
		if( iLayouts[ i ].iLayout->Changed() )
			{
			return ETrue;
			}
		}

	return iChanged;
*/	}
	
// -----------------------------------------------------------------------------
// DoClearChanged
// -----------------------------------------------------------------------------
void TGlxBlendLayout::DoClearChanged() 
	{
	TInt count = iLayouts.Count();
	for( TInt i = 0; i < count; i++ )
		{
//		iLayouts[ i ].iLayout->ClearChanged();
		}
		
	iChanged = EFalse;
	}

// -----------------------------------------------------------------------------
// TLayout::Match
// -----------------------------------------------------------------------------
TBool TGlxBlendLayout::TLayout::Match( const TLayout& aLhs, const TLayout& aRhs )
	{
	return aLhs.iLayout == aRhs.iLayout;
	}

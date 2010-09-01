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
* Description:    Splits layout chain by visual
*
*/




#include "glxlayoutsplitter.h"

#include <alf/alfvisual.h>
#include <mglxvisuallist.h>
#include <glxpanic.h> // Panic codes

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayoutSplitter::TGlxLayoutSplitter() 
	: iDefaultLayout( NULL ),
	iVisualList( NULL )
	{
	__DEBUG_ONLY( _iName = _L("TGlxLayoutSplitter") );
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxLayoutSplitter::~TGlxLayoutSplitter()
	{
	Reset();
	}

// -----------------------------------------------------------------------------
// Tells the splitter what is the visual list object
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutSplitter::SetVisualListL(
	MGlxVisualList* aVisualList )
	{
	__ASSERT_DEBUG( 
		aVisualList, Panic( EGlxPanicNullVisualList ) );
	iVisualList = aVisualList;
	iVisualList->AddObserverL( this );
	}

// -----------------------------------------------------------------------------
// SetLayoutL
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutSplitter::SetLayoutL(
	MGlxLayout* aLayout, const CAlfVisual* aVisual )
	{
	// Call SetVisualListL first
	__ASSERT_DEBUG( iVisualList, Panic( EGlxPanicNullVisualList ) ); 
	// search if there already is a layout for this visual
	__DEBUG_ONLY( TLayout temp;temp.iVisual = aVisual; );
	__ASSERT_DEBUG( 
		iLayouts.Find( temp, TLayout::Match ) == KErrNotFound, 
		Panic( EGlxPanicAlreadyAdded ) );
	
	// create layout struct
	TLayout newLayoutStruct;
	newLayoutStruct.iLayout = aLayout;
	newLayoutStruct.iVisual = aVisual;
	// append to the array
	iLayouts.AppendL( newLayoutStruct );
	}
	
// -----------------------------------------------------------------------------
// Removes an existing layout 
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutSplitter::RemoveLayout(
	const CAlfVisual* aVisual )
	{
	TInt index = LayoutIndex( aVisual );
	if( index != KErrNotFound )
		{
		iLayouts.Remove( index );
		}
	// NOTE: it is ok to try to remove a layout from visual when there was none
	}
	
// -----------------------------------------------------------------------------
// Sets layout to be used when for visuals which don't have a specific layout
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutSplitter::SetDefaultLayout(
	MGlxLayout* aLayout )
	{
	iDefaultLayout = aLayout;
	}

// -----------------------------------------------------------------------------
// Reset
// Remove all layouts and remove association with visual list.
// -----------------------------------------------------------------------------
EXPORT_C void TGlxLayoutSplitter::Reset()
    {
    // Remove all layouts
    iLayouts.Close();

    if( iVisualList )
        {
        iVisualList->RemoveObserver( this );
        iVisualList = NULL;
        }
    }

// -----------------------------------------------------------------------------
// DoSetLayoutValues
// -----------------------------------------------------------------------------
void TGlxLayoutSplitter::DoSetLayoutValues(
	TGlxLayoutInfo& aInfo )
	{
	// Call SetVisualListL to avoid this assert
	__ASSERT_DEBUG( iVisualList, Panic( EGlxPanicNullVisualList ) ); 

	// Does a layout exist for the visual
	// If yes, use the specific layout. If not, use default layout.
	MGlxLayout* layout = Layout( &aInfo.Visual() );
	// if layout was found
	if( layout )
		{
		// return its value
		return layout->SetLayoutValues( aInfo );
		}
	// otherwise return the default layout value
	else if( iDefaultLayout )
		{
		return iDefaultLayout->SetLayoutValues( aInfo );
		}
	// nothing to do, no layout for the visual nor default
	}

// -----------------------------------------------------------------------------
// HandleVisualAddedL
// -----------------------------------------------------------------------------
void TGlxLayoutSplitter::HandleVisualAddedL(
	CAlfVisual* /*aVisual*/, TInt /*aIndex*/, MGlxVisualList* /*aList*/)
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// HandleVisualRemoved
// Remove the layout associated with the visual, if any.
// -----------------------------------------------------------------------------
void TGlxLayoutSplitter::HandleVisualRemoved(
	const CAlfVisual* aVisual, MGlxVisualList* /*aList*/ )
	{
	// Try to find a layout struct with the provided visual, and remove
	// it if found
	TInt index = LayoutIndex( aVisual );
	if( index != KErrNotFound )
		{
		iLayouts.Remove( index );
		}
	}

// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
void TGlxLayoutSplitter::HandleFocusChangedL(
	TInt /*aFocusIndex*/, 
	TReal32 /*aItemsPerSecond*/, 
	MGlxVisualList* /*aList*/,
	NGlxListDefs::TFocusChangeType /*aType*/ )
	{
	// Do nothing
	}
	
// -----------------------------------------------------------------------------
// HandleSizeChanged
// -----------------------------------------------------------------------------
void TGlxLayoutSplitter::HandleSizeChanged(
	const TSize& /*aSize*/, MGlxVisualList* /*aList*/ )
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// Returns the layout associated with the aInIndex, or NULL if not found
// -----------------------------------------------------------------------------
MGlxLayout* TGlxLayoutSplitter::Layout(
	const CAlfVisual* aVisual ) const
	{
	// Find the layout that is intended for this visual
	TInt index = LayoutIndex( aVisual );
	if( index != KErrNotFound )
		{
		return iLayouts[ index ].iLayout;
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// Returns layout struct index or KErrNotFound
// -----------------------------------------------------------------------------
TInt TGlxLayoutSplitter::LayoutIndex(
	const CAlfVisual* aVisual ) const  
	{
	// search if there already is a layout for this visual
	TLayout temp;
	temp.iVisual = aVisual;
	return iLayouts.Find( temp, TLayout::Match );
	}

// -----------------------------------------------------------------------------
// TGlxLayoutSplitter::Match
// -----------------------------------------------------------------------------
TBool TGlxLayoutSplitter::TLayout::Match( 
	const TLayout& aLhs, const TLayout& aRhs )
	{
	// return true if the visuals are same
	return aLhs.iVisual == aRhs.iVisual;
	}


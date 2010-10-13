/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Stub file for image transaction in slideshow
 *
*/




#include "stub_tglxlayoutsplitter.h"

extern TBool gSplitterAddLayoutLCalled = EFalse;
extern TBool gSplitterRemoveLayoutLCalled = EFalse;

// -----------------------------------------------------------------------------
// Stub for TGlxLayout -->
// -----------------------------------------------------------------------------
TGlxLayout::TGlxLayout()
    {
    }
TGlxLayout::~TGlxLayout()
    {
    }
void TGlxLayout::SetNext( MGlxLayout* /*aLayout*/ )
	{
	}
MGlxLayout* TGlxLayout::Next() const
	{
	return NULL;
	}
void TGlxLayout::Insert(MGlxLayout* /*aLayout*/)
    {
    }
void TGlxLayout::Remove(MGlxLayout* /*aLayout*/)
    {
    }
void TGlxLayout::SetLayoutValues( TGlxLayoutInfo& /*aInfo*/ )
	{
	}
TBool TGlxLayout::Changed() const
	{
	return ETrue;
	}
void TGlxLayout::ClearChanged()
	{
	}
void TGlxLayout::DoSetLayoutValues( TGlxLayoutInfo& /*aInfo*/ )
	{
	}
TBool TGlxLayout::DoChanged() const
	{
	return ETrue;
	}
void TGlxLayout::DoClearChanged()
	{
	}
// -----------------------------------------------------------------------------
// <-- Stub for TGlxLayout
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for layout splitter -->
// -----------------------------------------------------------------------------
TGlxLayoutSplitter::TGlxLayoutSplitter()
	{
	gSplitterAddLayoutLCalled = EFalse;
	gSplitterRemoveLayoutLCalled = EFalse;
	}
TGlxLayoutSplitter::~TGlxLayoutSplitter()
	{
	}
void TGlxLayoutSplitter::SetVisualListL(MGlxVisualList* /*aVisualList*/)
	{
	}

void TGlxLayoutSplitter::AddLayoutL(MGlxLayout* /*aLayout*/, const CHuiVisual* /*aVisual*/)
	{
	gSplitterAddLayoutLCalled = ETrue;
	}

void TGlxLayoutSplitter::RemoveLayout(const CHuiVisual* /*aVisual*/)
	{
	gSplitterRemoveLayoutLCalled = ETrue;
	}
void TGlxLayoutSplitter::SetDefaultLayout(MGlxLayout* /*aLayout*/)
	{
	}

void TGlxLayoutSplitter::HandleFocusChangedL(
	TInt /*aFocusIndex*/, TReal32 /*aItemsPerSecond*/, MGlxVisualList* /*aList*/,
	NGlxListDefs::TFocusChangeType /*aType*/ )
	{
	
	}
void TGlxLayoutSplitter::HandleSizeChanged( const TSize& /*aSize*/, MGlxVisualList* /*aList*/ )
	{
	
	}
void TGlxLayoutSplitter::HandleVisualRemoved(
	const CHuiVisual* /*aVisual*/, MGlxVisualList* /*aList*/ )
	{
	
	}
void TGlxLayoutSplitter::HandleVisualAddedL(
	CHuiVisual* /*aVisual*/, TInt /*aIndex*/, MGlxVisualList* /*aList*/ )
	{
	
	}
void TGlxLayoutSplitter::DoSetLayoutValues( TGlxLayoutInfo& /*aInfo*/ )
	{
	
	}
TBool TGlxLayoutSplitter::DoChanged() const
	{
	return ETrue;	
	}
void TGlxLayoutSplitter::DoClearChanged()
	{
	
	}
// -----------------------------------------------------------------------------
// <-- Stub for layout splitter
// -----------------------------------------------------------------------------

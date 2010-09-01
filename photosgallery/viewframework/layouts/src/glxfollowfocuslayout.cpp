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
* Description:    Layout mapping index to distance from focus
*
*/




#include "glxfollowfocuslayout.h" 

#include <mglxvisuallist.h>	// MGlxVisualList
#include "glxpanic.h" // Panic codes
#include <mglxanimation.h>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxFollowFocusLayout::TGlxFollowFocusLayout()
	: iVisualList( NULL )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxFollowFocusLayout::~TGlxFollowFocusLayout() 
    {
    }

// -----------------------------------------------------------------------------
// SetVisualListL
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxFollowFocusLayout::SetVisualListL(
	MGlxVisualList* aVisualList )
    {
    __ASSERT_DEBUG( aVisualList, Panic( EGlxPanicNullVisualList ) );

    iVisualList = aVisualList;
    }

// -----------------------------------------------------------------------------
// DoSetLayoutValues
// -----------------------------------------------------------------------------
void TGlxFollowFocusLayout::DoSetLayoutValues( TGlxLayoutInfo& aInfo )
    {
    __ASSERT_DEBUG( iVisualList, Panic( EGlxPanicNullVisualList ) );

    // change the mappedindex to be relative to focus but wrapped around
    // so if the list items are
    //  0, 1, 2, 3, 4, 5, 6
    // -2,-1, 0, 1, 2, 3,-3 == focus in 2
    // -3,-2,-1, 0, 1, 2, 3 == focus in 3
    //  1, 2, 3,-3,-2,-1, 0 == focus in 6
    //  0, 1, 2, 3,-3,-2,-1 == focus in 0

    // calculate the current index - focus
    TInt index = aInfo.Index() - iVisualList->FocusIndex();
    
    // get item count
    TInt itemCount = iVisualList->ItemCount();
    TInt maxDist = itemCount / 2;

    // if index is smaller than smallest possible
    if ( index < -maxDist )
        {
        // wrap around
        index += itemCount;
        }
    // if index is larger than biggest possible
    else if ( index >= maxDist )
        {
        // wrap around
        index -= itemCount;
        }
    // assign the mapped index
    aInfo.iMappedIndex = index;
    }

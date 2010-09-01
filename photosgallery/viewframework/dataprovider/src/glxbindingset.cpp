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
* Description:    Implemention of bindings required to populate the visual item
*
*/




#include "glxbindingset.h"     // holds the bindings required to populate the visual item
#include <glxlog.h>            // Logging
#include <glxtracer.h>
// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//
CGlxBindingSet* CGlxBindingSet::NewLC( MGlxBindingObserver& aObserver )
    {
    TRACER("CGlxBindingSet::NewLC");
    CGlxBindingSet* set = new ( ELeave ) CGlxBindingSet( aObserver );
    CleanupStack::PushL( set );
    return set;
    }

// ----------------------------------------------------------------------------
// constructor
// ----------------------------------------------------------------------------
//
CGlxBindingSet::CGlxBindingSet( MGlxBindingObserver& aObserver )
        : iObserver( aObserver )
    {
    }
    
// ----------------------------------------------------------------------------
// destructor
// ----------------------------------------------------------------------------
//
CGlxBindingSet::~CGlxBindingSet() 
    {
    TRACER("CGlxBindingSet::~CGlxBindingSet");
    iBindings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// AddBindingL
// ----------------------------------------------------------------------------
//
void CGlxBindingSet::AddBindingL( CGlxBinding* aBinding )
    {
    TRACER("CGlxBindingSet::AddBindingL");
    __ASSERT_DEBUG( aBinding, User::Invariant() ); // null binding not accepted
    iBindings.AppendL( aBinding );
    aBinding->SetObserver( *this );
    }

// ----------------------------------------------------------------------------
// PopulateT
// ----------------------------------------------------------------------------
//
void CGlxBindingSet::PopulateT( Alf::MulVisualItem& aItem, 
        const TGlxMedia& aMedia, TBool aIsFocused, TInt aTextureId) const
    {
    TRACER("CGlxBindingSet::PopulateT");
    const TInt count = iBindings.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iBindings[i]->PopulateT( aItem, aMedia, aIsFocused, aTextureId );
        }
    }

// ----------------------------------------------------------------------------
// HasRelevantAttributes
// ----------------------------------------------------------------------------
//
TBool CGlxBindingSet::HasRelevantAttributes( const RArray< TMPXAttribute >& 
    aAttributes ) const
    {
    TRACER("CGlxBindingSet::HasRelevantAttributes");
    TBool isRelevant = EFalse;
    TInt count = iBindings.Count();
    TInt i = -1;
    while( ++i < count && !isRelevant )
        {
        isRelevant = iBindings[i]->HasRelevantAttributes( aAttributes );
        }
    GLX_LOG_INFO1("HasRelevantAttributes::isRelevant %d", isRelevant);
    return isRelevant;
    }

// ----------------------------------------------------------------------------
// HandleFocusChanged
// ----------------------------------------------------------------------------
//	
CGlxBinding::TResponse CGlxBindingSet::HandleFocusChanged( TBool aIsGained) 
    {
    TRACER("CGlxBindingSet::HandleFocusChanged");
    TResponse response = ENoUpdateNeeded;
    TInt count = iBindings.Count();
    TInt i = -1;
    while( ++i < count )
        {
        TResponse singleResponse = iBindings[i]->HandleFocusChanged( aIsGained );
        if ( ENoUpdateNeeded != singleResponse )
            {
            response = singleResponse;
            }
        }
    return response;
    }  
    
 // ----------------------------------------------------------------------------
// HandleItemChanged
// ----------------------------------------------------------------------------
//   
void CGlxBindingSet::HandleItemChangedL(const CMPXCollectionPath& aPath )
    {
    TRACER("CGlxBindingSet::HandleItemChangedL");
    TInt count = iBindings.Count();
    TInt i = -1;
    while( ++i < count )
        {
    
         iBindings[i]->HandleItemChangedL(aPath);
        }
    }
// ----------------------------------------------------------------------------
// HandleBindingChanged
// ----------------------------------------------------------------------------
//	
void CGlxBindingSet::HandleBindingChanged( const CGlxBinding& /*aBinding*/ )
    {
    TRACER("CGlxBindingSet::HandleBindingChanged");
    iObserver.HandleBindingChanged( *this );
    }

// ----------------------------------------------------------------------------
// AddRequirementsL
// ----------------------------------------------------------------------------
//	
void CGlxBindingSet::AddRequirementsL(  CGlxAttributeRequirements& 
    aAttributeRequirement,TSize aSize )
    {
    TRACER("CGlxBindingSet::AddRequirementsL");
    for(TInt i = 0; i < iBindings.Count(); i++ )
        {
        iBindings.operator[](i)->AddRequestL( aAttributeRequirement,aSize);
        }
    }


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
* Description:    Binding Set Factory ;Base for all Binding Set
*
*/




#include <glxlog.h>                      // Logging
#include <glxtracer.h>
#include "glxmulbindingsetfactory.h"     // CreateBindingSetL creates instance of Binding set
#include "glxbindingset.h"               // This basically holds the bindings required to populate the visual item
#include "glxpreviewthumbnailbinding.h"  // Implementation of preview thumbnail binding for the list view
#include "glxcontainerlistbinding.h"     // This class holds the list of tags/albums associated with a media item

/**
 * Constructor
  */
EXPORT_C TGlxMulBindingSetFactory::TGlxMulBindingSetFactory()
        : iBindingSet( NULL )
    {
    }

/**
 * Destructor
  */
EXPORT_C TGlxMulBindingSetFactory::~TGlxMulBindingSetFactory()
    {
    }

/**
 * CreateBindingSetL
  */
CGlxBindingSet* TGlxMulBindingSetFactory::CreateBindingSetL( 
        MGlxBindingObserver& aObserver ) const
    {
    TRACER("TGlxMulBindingSetFactory::CreateBindingSetL");
    CGlxBindingSet* set = CGlxBindingSet::NewLC( aObserver );
    iBindingSet = set; // does not take ownershop
    // allow deriving class to add the bindings to the set
    AddBindingsL();
    iBindingSet = NULL; // not necessary, but clear for maintenance safety
    CleanupStack::Pop( set );
    return set;
    }

/**
 * AddStringBindingL
  */
EXPORT_C void TGlxMulBindingSetFactory::AddStringBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
        TInt aStringId ) const
    {
    TRACER("TGlxMulBindingSetFactory::AddStringBindingL");
    iBindingSet->AddBindingL( CGlxStringBinding::NewL( aMulTag, aStringId ) );
    }

/**
 * AddMpxAttributeBindingL
  */
EXPORT_C void TGlxMulBindingSetFactory::AddMpxAttributeBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
        const TMPXAttribute& aAttribute, TInt aFormatStringId ) const
    {
    TRACER("TGlxMulBindingSetFactory::AddMpxAttributeBindingL");
    iBindingSet->AddBindingL( CGlxMpxAttributeBinding::NewL( aMulTag, aAttribute, aFormatStringId ) );
    }

/**
 * AddThumbnailBindingL
  */    
EXPORT_C void TGlxMulBindingSetFactory::AddThumbnailBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
	const TSize& aSize  ) const
    {
    TRACER("TGlxMulBindingSetFactory::AddThumbnailBindingL");
    iBindingSet->AddBindingL( CGlxThumbnailBinding::NewL( aMulTag, aSize ) );
    }
/**
 * AddFullScreenThumbnailBindingL
  */     
EXPORT_C void TGlxMulBindingSetFactory::AddFullScreenThumbnailBindingL(const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/)const
    {
    TRACER("TGlxMulBindingSetFactory::AddFullScreenThumbnailBindingL"); 
     iBindingSet->AddBindingL( CGlxFullScreenThumbnailBinding::NewL( aMulTag ) );
    }
/**
 * AddIconBindingL
 */    
EXPORT_C void TGlxMulBindingSetFactory::AddIconBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
    const CGlxMulIconProvider* aMulIcon, const TSize& aSize ) const
    {
    TRACER("TGlxMulBindingSetFactory::AddIconBindingL");
    iBindingSet->AddBindingL( CGlxIconBinding::NewL( aMulTag, aMulIcon, aSize ) );
    }
/**
 * AddContainerListBindingL
  */    
EXPORT_C void TGlxMulBindingSetFactory::AddContainerListBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
        const CMPXCollectionPath& aPath ) const
    {
     TRACER("TGlxMulBindingSetFactory::AddContainerListBindingL");
    iBindingSet->AddBindingL( CGlxContainerListBinding::NewL( aMulTag, aPath ) );
    }

/**
 * AddCommandBindingL
 */    
EXPORT_C void TGlxMulBindingSetFactory::AddCommandBindingL( MGlxBoundCommand* aCommand ) const
    {
    TRACER("TGlxMulBindingSetFactory::AddCommandBindingL");
	iBindingSet->AddBindingL( CGlxCommandBinding::NewL( aCommand ) );
    }

/**
 * AddTemplateBindingL
 */    
EXPORT_C void TGlxMulBindingSetFactory::AddTemplateBindingL(
    Alf::mulwidget::TLogicalTemplate aTemplate, 
	Alf::mulwidget::TLogicalTemplate aTemplateWhenFocused) const
    {
    TRACER("TGlxMulBindingSetFactory::AddTemplateBindingL");
    iBindingSet->AddBindingL( CGlxTemplateBinding::NewL( aTemplate, aTemplateWhenFocused ) );
    }

 /**
 * AddPreviewThumbnailBindingL
 */   
EXPORT_C void TGlxMulBindingSetFactory::AddPreviewThumbnailBindingL() const
    {
    TRACER("TGlxMulBindingSetFactory::AddPreviewThumbnailBindingL");
    iBindingSet->AddBindingL( CGlxPreviewThumbnailBinding::NewL() );//@todo
    }


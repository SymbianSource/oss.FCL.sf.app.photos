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
* Description:    Implementation of fULL-SCREEN view
*
*/





#include "glxfullscreenbindingsetfactory.h"
#include "glxmuliconprovider.h"
#include "glxuiutility.h"
#include <mpxmediageneraldefs.h>
#include <alf/ialfviewwidget.h>
#include <mul/imulwidget.h>
#include <avkon.rsg>
#include <mul/mulvisualitem.h>
// const that holds default icon size
const TSize KDefaultIconSize = TSize(50, 50);

//-----------------------------------------------------------------------------
// TGlxMulFullScreenBindingSetFactory 
//-----------------------------------------------------------------------------

/**
 * Constructor
 */
EXPORT_C TGlxMulFullScreenBindingSetFactory::TGlxMulFullScreenBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFullScreenBindingSetFactory::~TGlxMulFullScreenBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFullScreenBindingSetFactory::AddBindingsL() const
    {
	AddFullScreenThumbnailBindingL( Alf::mulvisualitem::KMulIcon1 );
	
    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL( *uiUtility );
    TSize videoIconSize = uiUtility->GetGridIconSize();
    CleanupStack::PopAndDestroy( uiUtility );
	AddIconBindingL( Alf::mulvisualitem::KMulIndicator2, 
	        CGlxMulVideoIconProvider::NewL(ETrue), videoIconSize);
	}
	
//-----------------------------------------------------------------------------
// TGlxMulFSMPTitleBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulFSMPTitleBindingSetFactory::TGlxMulFSMPTitleBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFSMPTitleBindingSetFactory::~TGlxMulFSMPTitleBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFSMPTitleBindingSetFactory::AddBindingsL() const
    {
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulTitle1 , KMPXMediaGeneralTitle );
	AddTemplateBindingL(Alf::mulwidget::KTemplate5);
    }

//-----------------------------------------------------------------------------
// TGlxMulFSMPDateBindingSetFactory 
//-----------------------------------------------------------------------------
/**
 * Constructor
 */
EXPORT_C TGlxMulFSMPDateBindingSetFactory::TGlxMulFSMPDateBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFSMPDateBindingSetFactory::~TGlxMulFSMPDateBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFSMPDateBindingSetFactory::AddBindingsL() const
    {
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaGeneralDate,
	    R_QTN_DATE_USUAL_WITH_ZERO );
	AddTemplateBindingL(Alf::mulwidget::KTemplate3);
	}


//-----------------------------------------------------------------------------
// TGlxMulFSMPTimeBindingSetFactory 
//-----------------------------------------------------------------------------
/**
 * Constructor
 */
EXPORT_C TGlxMulFSMPTimeBindingSetFactory::TGlxMulFSMPTimeBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFSMPTimeBindingSetFactory::~TGlxMulFSMPTimeBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFSMPTimeBindingSetFactory::AddBindingsL() const
    {
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaGeneralDate,
	    R_QTN_TIME_USUAL_WITH_ZERO );
	AddTemplateBindingL(Alf::mulwidget::KTemplate3);
	}

//-----------------------------------------------------------------------------
// TGlxMulFSMPMemoryBindingSetFactory 
//-----------------------------------------------------------------------------
/**
 * Constructor
 */
EXPORT_C TGlxMulFSMPMemoryBindingSetFactory::TGlxMulFSMPMemoryBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFSMPMemoryBindingSetFactory::~TGlxMulFSMPMemoryBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFSMPMemoryBindingSetFactory::AddBindingsL() const
    {
	AddIconBindingL( Alf::mulvisualitem::KMulIcon1, 
	        CGlxMulDriveIconProvider::NewL(), KDefaultIconSize );
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaGeneralSize );
	AddTemplateBindingL(Alf::mulwidget::KTemplate4);
	}
	
//-----------------------------------------------------------------------------
// TGlxMulFSMPAlbumBindingSetFactory 
//-----------------------------------------------------------------------------

/**
 * Constructor
 */
EXPORT_C TGlxMulFSMPAlbumBindingSetFactory::TGlxMulFSMPAlbumBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFSMPAlbumBindingSetFactory::~TGlxMulFSMPAlbumBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFSMPAlbumBindingSetFactory::AddBindingsL() const
    {
	AddTemplateBindingL(Alf::mulwidget::KTemplate4);
	}
	
//-----------------------------------------------------------------------------
// TGlxMulFSMPLocationBindingSetFactory 
//-----------------------------------------------------------------------------

/**
 * Constructor
 */
EXPORT_C TGlxMulFSMPLocationBindingSetFactory::TGlxMulFSMPLocationBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFSMPLocationBindingSetFactory::~TGlxMulFSMPLocationBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFSMPLocationBindingSetFactory::AddBindingsL() const
    {
	AddIconBindingL( Alf::mulvisualitem::KMulIcon1, 
	        CGlxMulLocationIconProvider::NewL(), KDefaultIconSize );
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KGlxMediaGeneralLocation );//@todo
	AddIconBindingL( Alf::mulvisualitem::KMulIcon2, 
	        CGlxMulLocationIconProvider::NewL(), KDefaultIconSize );
	AddTemplateBindingL(Alf::mulwidget::KTemplate6);
	}
	
//-----------------------------------------------------------------------------
// TGlxMulFSMPTagsBindingSetFactory 
//-----------------------------------------------------------------------------

/**
 * Constructor
 */
EXPORT_C TGlxMulFSMPTagsBindingSetFactory::TGlxMulFSMPTagsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFSMPTagsBindingSetFactory::~TGlxMulFSMPTagsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFSMPTagsBindingSetFactory::AddBindingsL() const
    {
	AddTemplateBindingL(Alf::mulwidget::KTemplate4);
	}


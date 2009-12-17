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
* Description:   Implementation of Metadata view
*
*/


#include "glxmetadatabindingsetfactory.h"
#include "glxmuliconprovider.h"
#include "glxdetailsboundcommand.h"
#include "glxuiutility.h"
#include <alf/ialfviewwidget.h>
#include <mul/imulwidget.h>
#include <avkon.rsg>
#include <glxmetadataview.rsg>
#include <mpxcollectionpath.h>
#include <glxcollectionplugintags.hrh>      // tag collection plugin uid
#include <glxcollectionpluginalbums.hrh>    // album collection plugin uid
#include <glxtracer.h>

//-----------------------------------------------------------------------------
// TGlxMulTitleDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulTitleDetailsBindingSetFactory::TGlxMulTitleDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulTitleDetailsBindingSetFactory::~TGlxMulTitleDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulTitleDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulTitleDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate6, Alf::mulwidget::KTemplate6);
	AddStringBindingL( Alf::mulvisualitem::KMulTitle, R_GLX_METADATA_VIEW_TITLE_NSERIES );
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaGeneralTitle );

	CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
	CleanupClosePushL( *uiUtility );
	AddThumbnailBindingL( Alf::mulvisualitem::KMulIcon1, uiUtility->GetGridIconSize() );
	CleanupStack::PopAndDestroy( uiUtility );

//	AddThumbnailBindingL( Alf::mulvisualitem::KMulIndicator1 );//qgn_lgal_details_editable_item
	AddCommandBindingL(CGlxTitleBoundCommand::NewL());
    }

//-----------------------------------------------------------------------------
// TGlxMulDateAndTimeDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulDateAndTimeDetailsBindingSetFactory::TGlxMulDateAndTimeDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulDateAndTimeDetailsBindingSetFactory::~TGlxMulDateAndTimeDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulDateAndTimeDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulDateAndTimeDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
	AddStringBindingL( Alf::mulvisualitem::KMulTitle,R_GLX_METADATA_VIEW_DATE_TIME_NSERIES );
	AddCommandBindingL(CGlxDateAndTimeBoundCommand::NewL());
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , 
	                    KMPXMediaGeneralDate, R_QTN_DATE_USUAL_WITH_ZERO );
    }

//-----------------------------------------------------------------------------
// TGlxMulDescriptionDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulDescriptionDetailsBindingSetFactory::TGlxMulDescriptionDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulDescriptionDetailsBindingSetFactory::~TGlxMulDescriptionDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulDescriptionDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulDescriptionDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
	AddStringBindingL( Alf::mulvisualitem::KMulTitle,R_GLX_METADATA_VIEW_DESCRIPTION_NSERIES );
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaGeneralComment );
//	AddThumbnailBindingL( Alf::mulvisualitem::KMulIndicator1 );//qgn_lgal_details_editable_item
	AddCommandBindingL(CGlxDescriptionBoundCommand::NewL());
    }

//-----------------------------------------------------------------------------
// TGlxMulTagsDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulTagsDetailsBindingSetFactory::TGlxMulTagsDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulTagsDetailsBindingSetFactory::~TGlxMulTagsDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulTagsDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulTagsDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
	AddStringBindingL( Alf::mulvisualitem::KMulTitle, R_GLX_METADATA_VIEW_TAGS_NSERIES );
	
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KGlxTagCollectionPluginImplementationUid );
    AddContainerListBindingL( Alf::mulvisualitem::KMulDetail, *path ); 
    CleanupStack::PopAndDestroy( path );
    
//	AddThumbnailBindingL( Alf::mulvisualitem::KMulIndicator1 );//qgn_lgal_details_editable_item
	AddCommandBindingL(CGlxTagsBoundCommand::NewL());
    }
    
//-----------------------------------------------------------------------------
// TGlxMulAlbumsDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulAlbumsDetailsBindingSetFactory::TGlxMulAlbumsDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulAlbumsDetailsBindingSetFactory::~TGlxMulAlbumsDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulAlbumsDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulAlbumsDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
 	AddStringBindingL( Alf::mulvisualitem::KMulTitle, R_GLX_METADATA_VIEW_ALBUMS_NSERIES );

	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KGlxCollectionPluginAlbumsImplementationUid );
    AddContainerListBindingL( Alf::mulvisualitem::KMulDetail, *path ); 
    CleanupStack::PopAndDestroy( path );
    
	//AddThumbnailBindingL( Alf::mulvisualitem::KMulIndicator1 );//qgn_lgal_details_editable_item
	AddCommandBindingL(CGlxAlbumsBoundCommand::NewL());

    }

//-----------------------------------------------------------------------------
// TGlxMulLocationDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulLocationDetailsBindingSetFactory::TGlxMulLocationDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulLocationDetailsBindingSetFactory::~TGlxMulLocationDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulLocationDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulLocationDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
 	AddStringBindingL( Alf::mulvisualitem::KMulTitle, R_GLX_METADATA_VIEW_LOCATION_NSERIES );
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KGlxMediaGeneralLocation ) ;//@todo
//	AddThumbnailBindingL( Alf::mulvisualitem::KMulIndicator1 );//qgn_lgal_details_editable_item
	AddCommandBindingL(CGlxLocationBoundCommand::NewL());
    }

//-----------------------------------------------------------------------------
// TGlxMulFileSizeDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulFileSizeDetailsBindingSetFactory::TGlxMulFileSizeDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulFileSizeDetailsBindingSetFactory::~TGlxMulFileSizeDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulFileSizeDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulFileSizeDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
	AddStringBindingL( Alf::mulvisualitem::KMulTitle,R_GLX_METADATA_VIEW_FILE_SIZE_NSERIES );
	AddCommandBindingL(CGlxFileSizeBoundCommand::NewL());
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaGeneralSize );
    }
    
//-----------------------------------------------------------------------------
// TGlxMulResolutionDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulResolutionDetailsBindingSetFactory::TGlxMulResolutionDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulResolutionDetailsBindingSetFactory::~TGlxMulResolutionDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulResolutionDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulResolutionDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
	AddStringBindingL( Alf::mulvisualitem::KMulTitle, R_GLX_METADATA_VIEW_RESOLUTION_NSERIES );
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KGlxMediaGeneralDimensions );
	AddCommandBindingL(CGlxResolutionBoundCommand::NewL());
    }

//-----------------------------------------------------------------------------
// TGlxMulDurationDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

 /**
 * Constructor
 */
EXPORT_C TGlxMulDurationDetailsBindingSetFactory::TGlxMulDurationDetailsBindingSetFactory()
    {
    }

/**
 * Destructor
 */
EXPORT_C TGlxMulDurationDetailsBindingSetFactory::~TGlxMulDurationDetailsBindingSetFactory()
    {
    }

 /**
 * AddBindingsL
 */
void TGlxMulDurationDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulDurationDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
 	AddStringBindingL( Alf::mulvisualitem::KMulTitle, R_GLX_METADATA_VIEW_DURATION_NSERIES);
	AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaGeneralDuration );
	AddCommandBindingL(CGlxDurationBoundCommand::NewL());
    }    
    
    
//-----------------------------------------------------------------------------
// TGlxMulUsageRightsDetailsBindingSetFactory 
//-----------------------------------------------------------------------------

/**
* Constructor
*/
EXPORT_C TGlxMulUsageRightsDetailsBindingSetFactory::TGlxMulUsageRightsDetailsBindingSetFactory()
    {
    }

/**
* Destructor
*/
EXPORT_C TGlxMulUsageRightsDetailsBindingSetFactory::~TGlxMulUsageRightsDetailsBindingSetFactory()
    {
    }

/**
* AddBindingsL
*/
void TGlxMulUsageRightsDetailsBindingSetFactory::AddBindingsL() const
    {
	TRACER("TGlxMulUsageRightsDetailsBindingSetFactory::AddBindingsL");
    
    AddTemplateBindingL(Alf::mulwidget::KTemplate5, Alf::mulwidget::KTemplate5);
 	AddStringBindingL( Alf::mulvisualitem::KMulTitle, R_GLX_METADATA_VIEW_DRM_NSERIES );
	//AddMpxAttributeBindingL( Alf::mulvisualitem::KMulDetail , KMPXMediaNullAttribute  );//@todo
 	AddCommandBindingL(CGlxUsageRightsBoundCommand::NewL());
    }

    
    

    

    
    


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
 * Description:    Updates CPS storage
 *
 */


// INCLUDE FILES
#include <e32base.h>
#include <featmgr.h>
#include <apgtask.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <liwservicehandler.h>
#include <glxuistd.h>     // KGlxFetchontextPriorityNormal and KGlxIconsFilename  
#include <AknIconUtils.h> 
#include <data_caging_path_literals.hrh>

#include <glxattributecontext.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <mglxmedialist.h>       // for MGlxMediaList
#include <glxcollectiongeneraldefs.h>
#include <glxgallery.hrh>               // for KGlxGalleryApplicationUid
#include <glxfilterfactory.h>
#include <glxuistd.h>
#include <glxthumbnailcontext.h>
#include <glxmedialistiterator.h>
#include <mglxcache.h>
#include <glxthumbnailattributeinfo.h>

#include "glxcontentharvesterplugin.h"
#include "glxcontentharvesterpluginbase.h"
#include "glxcontentharvesterplugin.hrh"
#include "glxmapconstants.h"
#include "glxcontentharvesterpluginalbums.h"

// Matrixmenu uid - needed to check if photo suite is in foreground
const TInt KMatrixMenuUid = 0x101F4CD2;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginBase::CGlxContentHarvesterPluginBase( )
    {
    TRACER( "CGlxContentHarvesterPluginBase::CGlxContentHarvesterPluginBase" );
    // No Implementation
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::ConstructL(TInt aDefaultIconId )
    {
    SetGridIconSizeL();
    TSize gridIconSize = GetGridIconSize();
    TFileName resFile(KDC_APP_BITMAP_DIR);
    resFile.Append(KGlxIconsFilename);
    iBitmap = AknIconUtils::CreateIconL(resFile,aDefaultIconId);
    AknIconUtils::SetSize(iBitmap, gridIconSize);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxContentHarvesterPluginBase::~CGlxContentHarvesterPluginBase()
    {
    TRACER( "CGlxContentHarvesterPluginBase::~CGlxContentHarvesterPluginBase" );
    if ( iCPSNotificationInterface )
        {
        iCPSNotificationInterface->Close();
        }
  
    delete iServiceHandler;
    
    if(iBitmap)
        {
         delete iBitmap;
         iBitmap=NULL;
        }
    }

// ---------------------------------------------------------------------------
// SetGridIconSizeL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::SetGridIconSizeL()
    {
    TRACER("CGlxContentHarvesterPluginBase::SetGridIconSizeL()");
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();

    if(FeatureManager::FeatureSupported( KFeatureIdLayout640_360_Touch ) || FeatureManager::FeatureSupported( KFeatureIdLayout360_640_Touch ))
        {
        iGridIconSize = TSize(111,83);
        }
    else if(FeatureManager::FeatureSupported(KFeatureIdLayout640_480_Touch) || FeatureManager::FeatureSupported(KFeatureIdLayout480_640_Touch) || 
            FeatureManager::FeatureSupported(KFeatureIdLayout640_480) || FeatureManager::FeatureSupported(KFeatureIdLayout480_640))
        {
        iGridIconSize = TSize(146,110);
        }
    else
        {
        iGridIconSize = TSize(146,110);
        }
    // Frees the TLS. Must be done after FeatureManager is used.
    FeatureManager::UnInitializeLib(); 
    }

// ---------------------------------------------------------------------------
// GetGridIconSize
// ---------------------------------------------------------------------------
//
TSize CGlxContentHarvesterPluginBase::GetGridIconSize()
    {
    TRACER("CGlxContentHarvesterPluginBase::GetGridIconSize()");
    return iGridIconSize;
    }

// ---------------------------------------------------------------------------
// GetBitmapHandle
// ---------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginBase::GetBitmapHandle() const
    {
    TRACER("CGlxContentHarvesterPluginBase::GetBitmapHandle");
    return iBitmap->Handle();
    }

// ---------------------------------------------------------------------------
// GetInterfaceForNotificationL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::GetInterfaceForNotificationL()
    {
    TRACER( "CGlxContentHarvesterPluginBase::GetInterfaceForNotificationL" );
    // create Service Handler and keep as long as access to Service needed
    iServiceHandler = CLiwServiceHandler::NewL();
    //keep for convenience, not owned
    iInParamList = &iServiceHandler->InParamListL();
    iOutParamList = &iServiceHandler->OutParamListL();

    RCriteriaArray criteriaArray;
    CleanupResetAndDestroyPushL( criteriaArray );

    // create Liw criteria
    CLiwCriteriaItem* criterion = 
    CLiwCriteriaItem::NewLC( KLiwCmdAsStr, KCPInterface, KCPService ); 
    criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );

    criteriaArray.AppendL( criterion );
    CleanupStack::Pop( criterion );

    // attach Liw criteria
    iServiceHandler->AttachL( criteriaArray );
    // get Service interface
    iServiceHandler->ExecuteServiceCmdL( *criterion, 
            *iInParamList, 
            *iOutParamList );

    CleanupStack::PopAndDestroy( &criteriaArray );

    // extract interface from output params
    TInt pos( 0 );
    iOutParamList->FindFirst( pos, KCPInterface );
    if( pos != KErrNotFound )
        {
        iCPSNotificationInterface = 
        (*iOutParamList)[pos].Value().AsInterface();    
        }

    iInParamList->Reset();
    iOutParamList->Reset();

    if ( !iCPSNotificationInterface )
        {
        // handle no Service
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// RequestCpsNotificationL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::RequestCpsNotificationL(TInt aSuiteItem)
    {
    TRACER( "CGlxContentHarvesterPluginBase::RequestCpsNotificationL" );

    TLiwGenericParam cptype( KType, TLiwVariant( KPublisherId ));
    iInParamList->AppendL( cptype );

    //create filter
    CLiwDefaultMap* filterMap = CLiwDefaultMap::NewLC();
    filterMap->InsertL( KPublisherId, TLiwVariant( KSuitePublisher ));

    switch (aSuiteItem)
        {
        case KItemIndexMonths:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationMonths ) );
            break;
        case KItemIndexAlbums:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationAlbums ) );
            break;
        case KItemIndexTags:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationTags ) );
            break;
        case KItemIndexAll:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationAll ) );
            break;
        }

    filterMap->InsertL( KContentId, TLiwVariant( KContentDefault ) );

    //we are only interested in update events
    filterMap->InsertL( KOperation, TLiwVariant( KOperationUpdate ) );

    TLiwGenericParam filter( KFilter, TLiwVariant( filterMap ));          
    iInParamList->AppendL( filter );

    iCPSNotificationInterface->ExecuteCmdL( KRequestNotification, 
            *iInParamList, 
            *iOutParamList,
            KLiwOptASyncronous,
            this );

    CleanupStack::PopAndDestroy( filterMap );
    filter.Reset();
    cptype.Reset();

    iInParamList->Reset();
    iOutParamList->Reset();
    }

// ---------------------------------------------------------------------------
// SetupPublisherL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::SetupPublisherL(TInt aSuiteItem)
    {
    TRACER( "CGlxContentHarvesterPluginBase::SetupPublisherL" );

    //add mandatory type param
    TLiwGenericParam cptype( KType, TLiwVariant( KPublisherId ));
    iInParamList->AppendL( cptype );

    //create publisher definition
    CLiwDefaultMap* filterMap = CLiwDefaultMap::NewLC();
    filterMap->InsertL( KPublisherId, TLiwVariant( KSuitePublisher ));
    switch (aSuiteItem)
        {
        case KItemIndexMonths:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationMonths ) );
            break;
        case KItemIndexAlbums:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationAlbums ) );
            break;
        case KItemIndexTags:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationTags ) );
            break;
        case KItemIndexAll:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationAll ) );
            break;
        }
    filterMap->InsertL( KContentId, TLiwVariant( KContentDefault ) );
    // initial value for activation flag
    filterMap->InsertL( KFlag, TLiwVariant( TUint(0) ) );


    // prepare Access Control List for the publisher
    CLiwDefaultMap* cpsAclMap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* cpsAclWriteMap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* cpsAclReadMap = CLiwDefaultMap::NewLC();

    // write policy - allow only MatrixMenu,
    // Matrix has 2 UIDs:
    //     - 0x20012474 is used if it is compiled as a standalone application
    //     - 0x101F4CD2 is used if it should replace AppShell
    // add both for convenience
    cpsAclWriteMap->InsertL( KUIDs, TLiwVariant( _L("0x101F4CD2:0x20012474") ) );

    // read policy - allow only the Content Harvester plug-in,
    // this plug-in runs inside Content Harvester server - UID 0x10282E5A
    cpsAclReadMap->InsertL( KUIDs, TLiwVariant( _L("0x10282E5A") ) );

    cpsAclMap->InsertL( KACLWrite, TLiwVariant( cpsAclWriteMap ) );
    cpsAclMap->InsertL( KACLRead, TLiwVariant( cpsAclReadMap ) );

    filterMap->InsertL( KAccessList, TLiwVariant( cpsAclMap ) );

    // create a dummy data map,
    // mandatory for adding publisher data - can be left empty
    CLiwDefaultMap* cpsDummyDataMap = CLiwDefaultMap::NewLC();
    filterMap->InsertL( KDataMap, TLiwVariant( cpsDummyDataMap ) );

    TLiwGenericParam cpsItemParam( KItem, TLiwVariant( filterMap ) );       
    iInParamList->AppendL( cpsItemParam );


    iCPSNotificationInterface->ExecuteCmdL( KAdd, 
            *iInParamList, 
            *iOutParamList,
            0,
            NULL );
    CleanupStack::PopAndDestroy( cpsDummyDataMap );
    CleanupStack::PopAndDestroy( cpsAclReadMap );
    CleanupStack::PopAndDestroy( cpsAclWriteMap );
    CleanupStack::PopAndDestroy( cpsAclMap );
    CleanupStack::PopAndDestroy( filterMap );
    cpsItemParam.Reset();
    // filter.Reset();
    cptype.Reset();

    iInParamList->Reset();
    iOutParamList->Reset();
    }

// ---------------------------------------------------------------------------
// HandleStateChangeL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::HandleStateChangeL(TInt aSuiteItem)
    {
    TRACER( "CGlxContentHarvesterPluginBase::HandleStateChangeL" );

    TLiwGenericParam cptype( KType, TLiwVariant( KPublisher ));
    iInParamList->AppendL( cptype );

    CLiwDefaultMap* filterMap = CLiwDefaultMap::NewLC();
    filterMap->InsertL( KPublisherId, TLiwVariant( KSuitePublisher ));
    switch (aSuiteItem)
        {
        case KItemIndexMonths:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationMonths ) );
            break;

        case KItemIndexAlbums:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationAlbums ) );
            break;

        case KItemIndexTags:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationTags ) );
            break;

        case KItemIndexAll:
            filterMap->InsertL( KContentType, TLiwVariant( KContentActivationAll ) );
            break;
        }

    TLiwGenericParam filter( KFilter, TLiwVariant( filterMap ));      
    iInParamList->AppendL( filter );


    iCPSNotificationInterface->ExecuteCmdL( KGetList, 
            *iInParamList, 
            *iOutParamList,
            0,
            NULL );

    CleanupStack::PopAndDestroy( filterMap );

    filter.Reset();
    cptype.Reset();

    TInt posStat( 0 );
    iOutParamList->FindFirst( posStat, KResults );
    if( posStat != KErrNotFound )
        {
        // status info present - extract and return
        CLiwIterable* results = (*iOutParamList)[posStat].Value().AsIterable();
        TLiwVariant cpdata;
        if ( results->NextL( cpdata ) )
            {
            const CLiwMap* map = cpdata.AsMap();
            TLiwVariant dataMapVar;
            if ( map->FindL( _L8("flag"), dataMapVar ) )
                {
                ActivateL(dataMapVar.AsTInt32());
                }
            dataMapVar.Reset();
            }
        cpdata.Reset();
        }
    else
        {
        // no return value
        // this will happen if suite was never activated/deactivated before
        // handle gracefuly - ignore and wait for action ;]
        }

    iInParamList->Reset();
    iOutParamList->Reset();

    }


// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginBase::FillInputListWithDataL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::FillInputListWithDataL( 
        CLiwGenericParamList* aInParamList,
        const TDesC& aPublisherId, const TDesC& aContentType, 
        const TDesC& aContentId, TInt aHandle)
    {
    TRACER( "CGlxContentHarvesterPluginBase::FillInputListWithDataL" );

    aInParamList->Reset();

    TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
    aInParamList->AppendL( cptype );

    CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

    if(aHandle)
        {
        map->InsertL( KFirstIcon, TLiwVariant( (TInt32)aHandle) );
        }

    cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
    cpdatamap->InsertL( KContentType, TLiwVariant( aContentType )); 
    cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
    cpdatamap->InsertL( KDataMap, TLiwVariant(map) );

    TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));        
    aInParamList->AppendL( item );
    CleanupStack::PopAndDestroy( map );
    CleanupStack::PopAndDestroy( cpdatamap );
    item.Reset();
    cptype.Reset(); 

    }

// ---------------------------------------------------------------------------
// CreateMedialistAndAttributeContextL
// ---------------------------------------------------------------------------
//
MGlxMediaList* CGlxContentHarvesterPluginBase::CreateMedialistAndAttributeContextL(
					const TGlxMediaId& aPluginId,
					CGlxAttributeContext* aUriAttributeContext, 
					CGlxAttributeContext* aThumbnailAttributeContext) const    		
    		{
            TRACER( "CGlxContentHarvesterPluginBase::CreateMedialistAndThumbnailContextL" );        
                 
            CMPXCollectionPath* path = CMPXCollectionPath::NewL();
            CleanupStack::PushL( path );
            path->AppendL( aPluginId.Value() );               

            CMPXFilter* filter = NULL;
            filter = TGlxFilterFactory::CreatePreviewFilterL(); 
            CleanupStack::PushL( filter );

            MGlxMediaList* mediaList = MGlxMediaList::InstanceL( *path, KGlxIdNone , filter);
            CleanupStack::PopAndDestroy( filter );
            CleanupStack::PopAndDestroy( path );  
            
            // Two different contexts are added. One for URI with high priority
            // and for Thumbnail with low priority. Because URI attribute should be
            // fetched first before placing thumbnail fetch request
            
            aUriAttributeContext->AddAttributeL(KMPXMediaGeneralUri);  
     
            TMPXAttribute attr( KGlxMediaIdThumbnail,
								GlxFullThumbnailAttributeId(ETrue,
									iGridIconSize.iWidth,iGridIconSize.iHeight) );
            
            aThumbnailAttributeContext->SetDefaultSpec(iGridIconSize.iWidth,
            											iGridIconSize.iHeight);
            
            aThumbnailAttributeContext->AddAttributeL(attr);    
         
            return mediaList;
            }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginBase::IsFocused
// ---------------------------------------------------------------------------
//
TBool CGlxContentHarvesterPluginBase::IsFocused() const
    {
    TRACER( "CGlxContentHarvesterPluginBase::IsFocused" );
    GLX_LOG_INFO1("CGlxContentHarvesterPluginBase::IsFocused,isFocused=%d",iIsFocused);
    return iIsFocused;
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginBase::Focused
// ---------------------------------------------------------------------------
//
TBool CGlxContentHarvesterPluginBase::Focused()
    {
    TRACER( "CGlxContentHarvesterPluginBase::Focused" );
    GLX_LOG_INFO1("CGlxContentHarvesterPluginBase::Focused(%d)", iIsFocused);
    return iIsFocused;
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginBase::SetFocus
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::SetFocus(TBool aFocus )
    {
    TRACER( "CGlxContentHarvesterPluginBase::SetFocus" );
    iIsFocused=aFocus;
    }

// ---------------------------------------------------------------------------
// ContainerCacheCleanupL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::ContainerCacheCleanupL(MGlxMediaList* aMediaList)
    {
    TRACER( "CGlxContentHarvesterPluginBase::ContainerCacheCleanupL" ); 
      
    if ( IsMatrixMenuInForegroundL() )
        {
        MGlxCache* cacheManager = MGlxCache::InstanceL();   
        for(TInt i=0 ; i< aMediaList->Count();i++)
            {  
            GLX_LOG_INFO1("CGlxContentHarvesterPluginBase::ForceCleanupMedia(%d)", i);
            cacheManager->ForceCleanupMedia(aMediaList->IdSpaceId(i),
                                            aMediaList->Item(i).Id());
            }
        cacheManager->Close();  
        }    
    }

// ---------------------------------------------------------------------------
// IsMatrixMenuInForegroundL
// ---------------------------------------------------------------------------
//
TBool CGlxContentHarvesterPluginBase::IsMatrixMenuInForegroundL()
    {
    TRACER( "CGlxContentHarvesterPluginBase::IsMatrixMenuInForegroundL" );
    
    RWsSession wssession;
    User::LeaveIfError(wssession.Connect());
    CleanupClosePushL(wssession);
        
    TApaTaskList taskList( wssession );       
    TApaTask task = taskList.FindApp( TUid::Uid( KMatrixMenuUid ) );
    // get fopreground app
    TApaTask taskForeGround = taskList.FindByPos(0); 
   
    if ( task.Exists() && task.ThreadId() == taskForeGround.ThreadId() )
        {
        CleanupStack::PopAndDestroy( &wssession ); 
        return ETrue;
        } 
    CleanupStack::PopAndDestroy( &wssession ); 
    return EFalse;
    }

// ---------------------------------------------------------------------------
// SetCHPlugin
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginBase::SetCHPlugin(MGlxContentHarvesterPlugin* aCHplugin)
	{
	iCHplugin = aCHplugin;
	}

// ---------------------------------------------------------------------------
// GetCHPlugin
// ---------------------------------------------------------------------------
//
MGlxContentHarvesterPlugin* CGlxContentHarvesterPluginBase::GetCHPlugin()
	{
	return iCHplugin;
	}

// EOF

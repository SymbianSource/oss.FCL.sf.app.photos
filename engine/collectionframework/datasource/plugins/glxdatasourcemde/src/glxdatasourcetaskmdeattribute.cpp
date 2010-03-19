/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   
*
*/



#include "glxdatasourcetaskmdeattribute.h"

#include <glxcollectionpluginCamera.hrh>
#include <glxcollectionpluginDownloads.hrh>
#include <glxcollectionpluginMonths.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxerrors.h>
#include <glxfilterfactory.h>
#include <glxgetrequest.h>
#include <glxgetrequest.h>
#include <glxidlistrequest.h>
#include <glxlog.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxmediageneraldefs.h>
#include <glxrequest.h>
#include <glxthumbnailattributeinfo.h>
#include <glxthumbnailrequest.h>
#include <glxtndatabase.h>
#include <glxtnfileinfo.h>
#include <glxtnthumbnailcreator.h>
#include <imageconversion.h>
#include <lbsposition.h>
#include <mdeconstants.h>
#include <mdelogiccondition.h>
#include <mdeobject.h>
#include <mdeobjectdef.h>
#include <mdeobjectquery.h>
#include <mdepropertydef.h>
#include <mderelationdef.h>
#include <mdesession.h>
#include <mdetextproperty.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h> 
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

#include "glxdatasourcemde.h"
#include "glxdatasourcemde.h"
#include "glxdatasourcemde.hrh"

// CONSTANTS

_LIT(KPropertyDefNameAlbumType, "Type");
_LIT(KPropertyDefNameComment, "Comment");
_LIT(KPropertyDefNameCreationDate, "CreationDate");
_LIT(KPropertyDefNameDRM, "DRM");
_LIT(KPropertyDefNameDuration, "Duration");
_LIT(KPropertyDefNameFrameCount, "FrameCount");
_LIT(KPropertyDefNameHeight, "Height");
_LIT(KPropertyDefNameItemType, "ItemType");
_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameLatitude, "Latitude");
_LIT(KPropertyDefNameLongitude, "Longitude");
_LIT(KPropertyDefNameSize, "Size");
_LIT(KPropertyDefNameTitle, "Title");
_LIT(KPropertyDefNameUsageCount, "UsageCount");
_LIT(KPropertyDefNameWidth, "Width");

// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------
//  
CGlxDataSourceTaskMdeAttribute::CGlxDataSourceTaskMdeAttribute(CGlxGetRequest* aRequest, MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource)
    : CGlxDataSourceTaskMde(aRequest, aObserver, aDataSource)
    {
    GLX_LOG_ENTRY_EXIT("CGlxDataSourceTaskMdeAttribute::CGlxDataSourceTaskMdeAttribute()");
    iFilterProperties = TGlxFilterFactory::ExtractAttributes(iRequest->Filter());
    }

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------
//  
CGlxDataSourceTaskMdeAttribute::~CGlxDataSourceTaskMdeAttribute()
	{
    GLX_LOG_ENTRY_EXIT("CGlxDataSourceTaskMdeAttribute::~CGlxDataSourceTaskMdeAttribute()");
    delete iMediaArray;
    iQueryAttributes.Close();
	}

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeAttribute::ExecuteRequestL
// ----------------------------------------------------------------------------
//      
void CGlxDataSourceTaskMdeAttribute::ExecuteRequestL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::ExecuteRequestL()");
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
        
    __ASSERT_DEBUG(request->MediaIds().Count() > 0, User::Invariant());
    
    if (request->MediaIds().Count() > 1)
        {
        iMediaArray = CMPXMediaArray::NewL();
        }
    
    if (request->MediaIds()[0] == KGlxCollectionRootId)
    	{
        __ASSERT_DEBUG(request->MediaIds().Count() == 1, User::Invariant());
        AddCollectionAttributesL(iResponse);
    	}
    else
    	{
        
        __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
        CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);   

        const RArray<TItemId>& mediaIds = reinterpret_cast<const RArray<TItemId>&>(request->MediaIds());
        
        switch(iFilterProperties.iItemType)
            {
            case EGlxFilterVideoAndImages:
            case EGlxFilterImage:
            case EGlxFilterVideo:
                {
            	QueueImageVideoObjectQueriesL(mediaIds, iFilterProperties);
        	    if (LocationAttributeRequested())
        	        {
        	        QueueLocaitonQueryL();
        	        }
                break;
                }
            case EGlxFilterAlbum:
                {
            	QueueAlbumObjectQueryL(mediaIds);
                break;
                }
            case EGlxFilterTag:
                {
                QueueTagObjectQueryL(mediaIds);
                break;
                }
            case EGlxFilterMonth:
                {
                QueueMonthObjectQueryL(mediaIds);
                break;
                }
            }
        
    	}

    DoNextQueryL();
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::DoHandleQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::DoHandleQueryCompletedL(CMdEQuery& /*aQuery*/)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::DoHandleQueryCompletedL()");   
    switch (iQueryTypes[0])
        {
        case ELocationAttributeQuery:
            DoHandleLocationQueryCompletedL();
        break;
        case EAttributeQuery:
            DoHandleAttributeQueryCompletedL();
        break;
        case EImageVideoQuery:
            DoHandleImageVideoQueryCompletedL();
        break;
        case EContainerFirstItemQuery:
            // Fall through to panic, DoHandleQueryCompletedL can't handle this query type.
        default:
            Panic(EGlxPanicLogicError);
        break;
        }  
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeAttribute::DoNextQueryL
// ----------------------------------------------------------------------------
//    
void CGlxDataSourceTaskMdeAttribute::DoNextQueryL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::DoNextQueryL()");
    if (iQueries.Count())
        {
        ExecuteQueryL();
        }
    else
    	{
    	DoHandleAttributeResponseCompleteL();
    	}
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeAttribute::DoHandleAttributeResponseCompleteL
// ----------------------------------------------------------------------------
//     
void CGlxDataSourceTaskMdeAttribute::DoHandleAttributeResponseCompleteL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::DoHandleAttributeResponseCompleteL()");
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
    if(request->MediaIds().Count() > 1)
        {
        iResponse->SetCObjectValueL(KMPXMediaArrayContents, iMediaArray);
        iResponse->SetTObjectValueL(KMPXMediaArrayCount, iMediaArray->Count());
        delete iMediaArray;
        iMediaArray = NULL;
        }
    HandleRequestComplete(KErrNone);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddAttributesLC
// ----------------------------------------------------------------------------
//      
void CGlxDataSourceTaskMdeAttribute::AddAttributesL(CMdEObject& aObject, CMPXMedia* aEntry)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::AddAttributesL(CMdEObject& aObject, CMPXMedia* aEntry)");
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    
    CGlxDataSource::TContainerType containerType = DataSource()->ContainerType(&aObject);
    if( containerType != CGlxDataSource::EContainerTypeNotAContainer )
        {
        AddContainerAttributesL(aEntry, &aObject, containerType);
        }
    else 
        {
        CGlxDataSource::TItemType itemType = DataSource()->ItemType(&aObject);
        if ( itemType != CGlxDataSource::EItemTypeNotAnItem )
            {
            AddItemAttributesL(aEntry, &aObject, itemType);
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }       
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddCollectionAttributesL
// ----------------------------------------------------------------------------
//     
void CGlxDataSourceTaskMdeAttribute::AddCollectionAttributesL(CMPXMedia* aEntry)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::AddCollectionAttributesL(CMPXMedia* aEntry)");
    __ASSERT_DEBUG(aEntry, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
    
    for ( TInt i = 0; i < request->Attributes().Count(); i++ )
        {
        if ( request->Attributes()[i] == KMPXMediaGeneralId )
            {
            aEntry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, (TMPXItemId)request->CollectionPluginUid().iUid);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralType )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralType, EMPXNoType);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCategory )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXCollection);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralTitle )
            {
            request->AppendCpiAttributeL(KMPXMediaGeneralTitle);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSystemItem )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSystemItem, ETrue);
            }
            
        
 #ifdef GLX_SUB_TITLE_REL8
            
        //Attributes to get the Count of Images in Container                
        else if	( request->Attributes()[i] == KGlxMediaItemTypeImage )    
        	{
        	TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
            CMdEObjectDef* objectDef = &DataSource()->ObjectDef();
            TBool isContent = ETrue;
            TGlxFilterProperties filterProperties = iFilterProperties;
            
            switch(iRequest->CollectionPluginUid().iUid)
	            {
	        	case KGlxCollectionPluginCameraImplementationUid:
	        		{        		
	                 container = DataSource()->CameraAlbumId();
	                 objectDef = &DataSource()->AlbumDef();
	                 filterProperties.iItemType = EGlxFilterImage;
	                 QueueObjectQueryL(*objectDef, isContent, EAttributeQuery, EQueryResultModeCount, container, request->Attributes()[i], aEntry,  filterProperties);
	                 break;                    	
	        		}
	        	default:
	        		{
	        		break;	
	        		}
	        	 }
        	}           	
		
		
		//Attributes to get the Count of Videos in Container                    	
         else if ( request->Attributes()[i] == KGlxMediaItemTypeVideo )    
        	{
        	TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
            CMdEObjectDef* objectDef = &DataSource()->ObjectDef();
            TBool isContent = ETrue;
            TGlxFilterProperties filterProperties = iFilterProperties;
            
            switch(iRequest->CollectionPluginUid().iUid)
	            {
	        	case KGlxCollectionPluginCameraImplementationUid:
	        		{        		
	                 container = DataSource()->CameraAlbumId();
	                 objectDef = &DataSource()->AlbumDef();
	                 filterProperties.iItemType = EGlxFilterVideo;
	                 QueueObjectQueryL(*objectDef, isContent, EAttributeQuery, EQueryResultModeCount, container, request->Attributes()[i], aEntry,  filterProperties);
	                 break;                    	
	        		}
	        	default:
	        		{
	        		break;	
	        		}
	        	 }
        	}
        	
#endif

        	
        else if ( ( request->Attributes()[i] == KMPXMediaGeneralCount ) || ( request->Attributes()[i] == KGlxMediaCollectionInternalUsageCount ) )
            {
            TGlxMediaId container = TGlxMediaId(KGlxCollectionRootId);
            CMdEObjectDef* objectDef = &DataSource()->ObjectDef();
            TBool isContent = ETrue;
            TGlxFilterProperties filterProperties = iFilterProperties;
            
            switch(iRequest->CollectionPluginUid().iUid)
                {

                case KGlxCollectionPluginAlbumsImplementationUid:
                    {
                    isContent = EFalse;
                    objectDef = &DataSource()->AlbumDef();
                    break;
                    }
                case KGlxCollectionPluginCameraImplementationUid:
                    {
                    container = DataSource()->CameraAlbumId();
                    objectDef = &DataSource()->AlbumDef();
                    break;
                    }
                case KGlxTagCollectionPluginImplementationUid: 
                    {
                    isContent = EFalse;
                    objectDef = &DataSource()->TagDef();
                    break;
                    }
                case KGlxCollectionPluginMonthsImplementationUid:
                    {
                    container = DataSource()->CameraAlbumId();
                    objectDef = &DataSource()->AlbumDef();
                    break;
                    }
                default:
                    {
                    // default gallery query returns all objects as per filter
                    break;
                    }
                }
            QueueObjectQueryL(*objectDef, isContent, EAttributeQuery, EQueryResultModeCount, container, request->Attributes()[i], aEntry,  filterProperties);
            }
        else if ( request->Attributes()[i] == KMPXMediaColDetailSpaceId )
            {
            aEntry->SetTObjectValueL(KMPXMediaColDetailSpaceId, KGlxDataSourceMdeImplementationUid);
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalStartDate )
            {
            TGlxFilterProperties filterProperties = iFilterProperties;
            filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
            filterProperties.iSortDirection = EGlxFilterSortDirectionAscending;

            QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery, EQueryResultModeObjectWithoutFreetexts, DataSource()->CameraAlbumId(), KGlxMediaCollectionInternalStartDate, aEntry, filterProperties);
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalEndDate )
            {
            // not necessary to be requested, returned when StartDate requested
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSlideshowableContent )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSlideshowableContent, 0);
            }
        else if ( request->Attributes()[i] == KMPXMediaDrmProtected )
            {
            // collection itself is not protected
            aEntry->SetTObjectValueL(KMPXMediaDrmProtected, EFalse); 
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }


    
// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddContainerAttributesL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::AddContainerAttributesL(CMPXMedia* aEntry, CMdEObject* aContainer, CGlxDataSource::TContainerType aType)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::AddContainerAttributesL()");
    __ASSERT_DEBUG(aEntry, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(aContainer, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);

    /// @todo check property defs are valid and type is correct
    for ( TInt i = 0; i < request->Attributes().Count(); i++ )
        {
        if ( request->Attributes()[i] == KMPXMediaGeneralId )
            {
            aEntry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, (TMPXItemId)aContainer->Id());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralType )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralType, EMPXGroup);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCategory )
            {
            if( CGlxDataSource::EContainerTypeAlbum == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXAlbum);
                }
            else if( CGlxDataSource::EContainerTypeTag == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXTag);
                }
            else if( CGlxDataSource::EContainerTypeMonth == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXMonth);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralUri )
            {
            aEntry->SetTextValueL(KMPXMediaGeneralUri, aContainer->Uri());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralTitle )
            {
            CMdEProperty* title;
            CMdEPropertyDef* titleProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameTitle);
            TInt titleIndex = aContainer->Property(*titleProperty, title);
            if(titleIndex == KErrNotFound)
                {
                TParsePtrC parser(aContainer->Uri());
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, parser.Name());
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, static_cast<CMdETextProperty*>(title)->Value());
                }
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                    {
                    CMdEProperty* albumType;
                    CMdEPropertyDef* albumTypeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameAlbumType);
                    TInt albumTypeIndex = aContainer->Property(*albumTypeProperty, albumType);
                    if( KErrNotFound != albumTypeIndex )
                        {
                        TUint16 albumTypeValue = static_cast<CMdEUint16Property*>(albumType)->Value();
                        if ( albumTypeValue != MdeConstants::Album::EAlbumUser ) 
                            {
                            request->AppendCpiAttributeL(KMPXMediaGeneralTitle);
                            aEntry->SetTObjectValueL(KGlxMediaCollectionInternalSystemItemType, albumTypeValue);
                            }
                        }
                    break;
                    }
                case CGlxDataSource::EContainerTypeTag:
                    {
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    request->AppendCpiAttributeL(KMPXMediaGeneralTitle);
                    CMdEProperty* time;
                    CMdEPropertyDef* timeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameCreationDate);
                    TInt timeIndex = aContainer->Property(*timeProperty, time);
                    if( KErrNotFound == timeIndex )
                        {
                        User::Leave(KErrCorrupt);
                        }

                    aEntry->SetTObjectValueL(KGlxMediaCollectionInternalStartDate, static_cast<CMdETimeProperty*>(time)->Value());
                    break;
                    }
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef* timeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameCreationDate);
            TInt timeIndex = aContainer->Property(*timeProperty, time);
            if( KErrNotFound == timeIndex )
                {
                User::Leave(KErrCorrupt);
                }

            aEntry->SetTObjectValueL(KMPXMediaGeneralDate, static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralSize )
            {
            CMdEProperty* size;
            CMdEPropertyDef* sizeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameSize);
            TInt sizeIndex = aContainer->Property(*sizeProperty, size);
            TInt sizeValue;
            if(sizeIndex == KErrNotFound)
                {
                sizeValue = 0;
                }
            else
                {
                sizeValue = static_cast<CMdEInt32Property*>(size)->Value();
                }
            aEntry->SetTObjectValueL(KMPXMediaGeneralSize, sizeValue);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDrive )
            {
            TParsePtrC parser(aContainer->Uri());
            aEntry->SetTextValueL(KMPXMediaGeneralDrive, parser.Drive());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralMimeType )
            {
            CMdEProperty* mimeType;
            CMdEPropertyDef* mimeTypeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameItemType);
            TInt mimeTypeIndex = aContainer->Property(*mimeTypeProperty, mimeType);
            if( KErrNotFound == mimeTypeIndex)
                {
                aEntry->SetTextValueL(KMPXMediaGeneralMimeType, KNullDesC); 
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralMimeType, static_cast<CMdETextProperty*>(mimeType)->Value());
                }
            }
        else if ( ( request->Attributes()[i] == KMPXMediaGeneralCount ) || ( request->Attributes()[i] == KGlxMediaCollectionInternalUsageCount ) )
            {
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                case CGlxDataSource::EContainerTypeTag:
                    {
                    CMdEProperty* countType;
                    CMdEPropertyDef* countTypeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameUsageCount);
                    TInt countTypeIndex = aContainer->Property(*countTypeProperty, countType);
                    if( KErrNotFound != countTypeIndex )
                        {
                        TUint count = static_cast<CMdEUint32Property*>(countType)->Value();
                        aEntry->SetTObjectValueL(request->Attributes()[i], count);
                        }
                    else
                        {
                        aEntry->SetTObjectValueL(request->Attributes()[i], 0);
                        }
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    TGlxFilterProperties filterProperties = iFilterProperties;
                    AddMonthFilterL(aContainer, filterProperties);
                    QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery, EQueryResultModeCount, DataSource()->CameraAlbumId(), request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
                }
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSystemItem )
            {
            TBool systemItem = EFalse;
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                    {
                    CMdEProperty* albumType;
                    CMdEPropertyDef* albumTypeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameAlbumType);
                    TInt albumTypeIndex = aContainer->Property(*albumTypeProperty, albumType);
                    if( KErrNotFound != albumTypeIndex )
                        {
                        TInt albumTypeValue = static_cast<CMdEUint16Property*>(albumType)->Value();
                        if( (albumTypeValue == MdeConstants::Album::EAlbumSystemCamera) || (albumTypeValue == MdeConstants::Album::EAlbumSystemFavourite ) )
                            {
                            systemItem = ETrue;
                            }
                        }
                    break;
                    }
                case CGlxDataSource::EContainerTypeTag:
                    {
                    systemItem = EFalse;
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    systemItem = ETrue;
                    break;
                    }
                }
            aEntry->SetTObjectValueL(KGlxMediaGeneralSystemItem, systemItem);
            }
            
         //Attributes to get the Count of Images in Container                
        else if	( request->Attributes()[i] == KGlxMediaItemTypeImage )    
        	{
            switch (aType)
	            {
	        	case CGlxDataSource::EContainerTypeMonth:
	        		{
                    TGlxFilterProperties filterProperties = iFilterProperties;
                    AddMonthFilterL(aContainer, filterProperties);
                    filterProperties.iItemType = EGlxFilterImage;
                    QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery, EQueryResultModeCount, DataSource()->CameraAlbumId(), request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
	        	default:
	        		{
	        		break;	
	        		}
	        	 }
        	}           	
		
		// Attributes to get the Count of Videos in Container 		
						                   	
         else if ( request->Attributes()[i] == KGlxMediaItemTypeVideo )    
        	{
            switch (aType)
	            {
	        	case CGlxDataSource::EContainerTypeMonth:
	        		{
                    TGlxFilterProperties filterProperties = iFilterProperties;
                    AddMonthFilterL(aContainer, filterProperties);
                    filterProperties.iItemType = EGlxFilterVideo;
                    QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery, EQueryResultModeCount, DataSource()->CameraAlbumId(), request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
	        	default:
	        		{
	        		break;	
	        		}
	        	 }
        	}
            
        else if ( request->Attributes()[i] == KMPXMediaColDetailSpaceId )
            {
            aEntry->SetTObjectValueL(KMPXMediaColDetailSpaceId, KGlxDataSourceMdeImplementationUid);
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalStartDate )
            {
            // nothing returned for container but is valid for CPI to request
            }
        else if ( request->Attributes()[i] == KGlxMediaCollectionInternalEndDate )
            {
            // nothing returned for container but is valid for CPI to request
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralDimensions )
            {
            // return zero size for container (thumbnail only used in preview list)
            aEntry->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(0,0));
            }
        else if ( request->Attributes()[i] == KMPXMediaDrmProtected )
            {
            // container itself is not protected
            aEntry->SetTObjectValueL(KMPXMediaDrmProtected, EFalse); 
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralFramecount )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralFramecount, 1);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSlideshowableContent )
            {
            TGlxFilterProperties filterProperties = iFilterProperties;
            filterProperties.iItemType = EGlxFilterImage;
            filterProperties.iNoDRM = ETrue;
            filterProperties.iExcludeAnimation = ETrue;
            switch (aType)
                {
                case CGlxDataSource::EContainerTypeAlbum:
                case CGlxDataSource::EContainerTypeTag:
                    {
                    QueueObjectQueryL(aContainer->Def(), ETrue, EAttributeQuery, EQueryResultModeCount, TGlxMediaId(aContainer->Id()), request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
                case CGlxDataSource::EContainerTypeMonth:
                    {
                    AddMonthFilterL(aContainer, filterProperties);
                    QueueObjectQueryL(DataSource()->AlbumDef(), ETrue, EAttributeQuery, EQueryResultModeCount, DataSource()->CameraAlbumId(), request->Attributes()[i], aEntry, filterProperties);
                    break;
                    }
                }
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralLastModifiedDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef* timeProperty = aContainer->Def().GetPropertyDefL(KPropertyDefNameLastModifiedDate);
            TInt timeIndex = aContainer->Property(*timeProperty, time);
            if( KErrNotFound == timeIndex) 
                {
                User::Leave(KErrCorrupt);
                }
            aEntry->SetTObjectValueL(KGlxMediaGeneralLastModifiedDate, static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralDimensions )
            {
            TSize dimensions(0,0);
            
            aEntry->SetTObjectValueL(KGlxMediaGeneralDimensions, dimensions);
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::AddItemAttributesL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::AddItemAttributesL(CMPXMedia* aEntry, CMdEObject* aItem, CGlxDataSource::TItemType aType)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::AddItemAttributesL(CMPXMedia* aEntry, CMdEObject* aItem, CGlxDataSource::TItemType aType)");
    __ASSERT_DEBUG(aEntry, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(aItem, Panic(EGlxPanicLogicError));
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);

    /// @todo check property defs are valid and type is correct
    for ( TInt i = 0; i < request->Attributes().Count(); i++ )
        {   
        if ( request->Attributes()[i] == KMPXMediaGeneralId )
            {
            aEntry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, (TMPXItemId)aItem->Id());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralType )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCategory )
            {
            if( CGlxDataSource::EItemTypeImage == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
                }
            else if( CGlxDataSource::EItemTypeVideo == aType)
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXVideo);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralUri )
            {
            aEntry->SetTextValueL(KMPXMediaGeneralUri, aItem->Uri());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralTitle )
            {
            CMdEProperty* title;
            CMdEPropertyDef* titleProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameTitle);
            TInt titleIndex = aItem->Property(*titleProperty, title);
            if( KErrNotFound == titleIndex )
                {
                TParsePtrC parser(aItem->Uri());
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, parser.Name());
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralTitle, static_cast<CMdETextProperty*>(title)->Value());
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef* timeProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameCreationDate);
            TInt timeIndex = aItem->Property(*timeProperty, time);
            if( KErrNotFound == timeIndex) 
                {
                User::Leave(KErrCorrupt);
                }
            aEntry->SetTObjectValueL(KMPXMediaGeneralDate, static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralLastModifiedDate )
            {
            CMdEProperty* time;
            CMdEPropertyDef* timeProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameLastModifiedDate);
            TInt timeIndex = aItem->Property(*timeProperty, time);
            if( KErrNotFound == timeIndex) 
                {
                User::Leave(KErrCorrupt);
                }
            aEntry->SetTObjectValueL(KGlxMediaGeneralLastModifiedDate, static_cast<CMdETimeProperty*>(time)->Value());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralSize )
            {
            CMdEProperty* size;
            CMdEPropertyDef* sizeProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameSize);
            TInt sizeIndex = aItem->Property(*sizeProperty, size);
            User::LeaveIfError(sizeIndex);
            
            aEntry->SetTObjectValueL(KMPXMediaGeneralSize, static_cast<CMdEInt32Property*>(size)->Value());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDrive )
            {
            TParsePtrC parser(aItem->Uri());
            aEntry->SetTextValueL(KMPXMediaGeneralDrive, parser.Drive());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralMimeType )
            {
            CMdEProperty* mimeType;
            CMdEPropertyDef* mimeTypeProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameItemType);
            TInt mimeTypeIndex = aItem->Property(*mimeTypeProperty, mimeType);

            User::LeaveIfError(mimeTypeIndex); 
            aEntry->SetTextValueL(KMPXMediaGeneralMimeType, static_cast<CMdETextProperty*>(mimeType)->Value());
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralDuration )
            {
            if( CGlxDataSource::EItemTypeImage == aType )
                {
                aEntry->SetTObjectValueL(KMPXMediaGeneralDuration, 0);
                }
            else
                {
                CMdEProperty* duration;
                CMdEPropertyDef* durationProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameDuration);
                TInt durationIndex = aItem->Property(*durationProperty, duration);
                User::LeaveIfError(durationIndex); 
                
                aEntry->SetTObjectValueL(KMPXMediaGeneralDuration, static_cast<CMdEUint32Property*>(duration)->Value());
                }
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSystemItem)
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSystemItem, EFalse);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralDimensions )
            {
            TSize dimensions(0,0);
            
            CMdEProperty* xDim;
            CMdEPropertyDef* xDimProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameWidth);
            TInt xDimIndex = aItem->Property(*xDimProperty, xDim);
            if( KErrNotFound == xDimIndex )
                {
                //User::Leave(KErrCorrupt);
                }
            else
                {
                dimensions.iWidth = static_cast<CMdEUint16Property*>(xDim)->Value();
                }
            
            CMdEProperty* yDim;
            CMdEPropertyDef* yDimProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameHeight);
            TInt yDimIndex = aItem->Property(*yDimProperty, yDim);
            if( KErrNotFound == yDimIndex )
                {
                //User::Leave(KErrCorrupt);
                }
            else
                {
                dimensions.iHeight = static_cast<CMdEUint16Property*>(yDim)->Value();
                }
            if ( ( dimensions.iWidth == 0 ) || ( dimensions.iHeight == 0 ) )
                {
                if( CGlxDataSource::EItemTypeImage == aType)
                    {
                    // EXIF header is corrupt, must read size from file.
                    CImageDecoder* decoder = NULL;

                    TRAPD(err, decoder = CImageDecoder::FileNewL( DataSource()->FileServerSession(), aItem->Uri(), CImageDecoder::EOptionNone ));
                    if (err == KErrNone)
                    	{
                    	dimensions = decoder->FrameInfo().iOverallSizeInPixels;
                    	}
                    delete decoder;
                    }
                }
            
            aEntry->SetTObjectValueL(KGlxMediaGeneralDimensions, dimensions);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralFramecount )
            {
            CMdEProperty* framecount;
            TInt fcount = 1;
            CMdEPropertyDef* framecountProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameFrameCount);
            if( framecountProperty )
                {
                TInt framecountIndex = aItem->Property(*framecountProperty, framecount);
                if( KErrNotFound != framecountIndex )
                    {
                    fcount = static_cast<CMdEUint32Property*>(framecount)->Value();
                    }
                }
             aEntry->SetTObjectValueL(KGlxMediaGeneralFramecount, fcount);
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralComment )
            {
            CMdEProperty* comment;
            CMdEPropertyDef* commentProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameComment); /// @todo using Exif Comment field for comment as spec is not clear
            TInt commentIndex = aItem->Property(*commentProperty, comment);
            if( KErrNotFound == commentIndex)
                {
                aEntry->SetTextValueL(KMPXMediaGeneralComment, KNullDesC); 
                }
            else
                {
                aEntry->SetTextValueL(KMPXMediaGeneralComment, static_cast<CMdETextProperty*>(comment)->Value());
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaDrmProtected )
            {
            CMdEProperty* drmProtected;
            CMdEPropertyDef* drmProtectedProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameDRM); 
            TInt drmProtectedIndex = aItem->Property(*drmProtectedProperty, drmProtected);
            if( KErrNotFound == drmProtectedIndex)
                {
                aEntry->SetTObjectValueL(KMPXMediaDrmProtected, EFalse); 
                }
            else
                {
                aEntry->SetTObjectValueL(KMPXMediaDrmProtected, static_cast<CMdEBoolProperty*>(drmProtected)->Value());
                }
            }
        else if ( request->Attributes()[i] == KMPXMediaGeneralCount )
            {
            aEntry->SetTObjectValueL(KMPXMediaGeneralCount, 1);
            }
        else if ( request->Attributes()[i] == KMPXMediaColDetailSpaceId )
            {
            aEntry->SetTObjectValueL(KMPXMediaColDetailSpaceId, KGlxDataSourceMdeImplementationUid);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralSlideshowableContent )
            {
            aEntry->SetTObjectValueL(KGlxMediaGeneralSlideshowableContent, -1);
            }
        else if ( request->Attributes()[i] == KGlxMediaGeneralLocation)
            {
            // Set the attribute to a TCoordinate initialised to NaN. 
            aEntry->SetTObjectValueL(KGlxMediaGeneralLocation, TCoordinate());
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::AddLocationAttributeToMediaL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::AddLocationAttributeToMediaL(CMPXMedia& aMedia, const TItemId& aLocationId)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::AddLocationAttributeToMediaL(CMPXMedia& aMedia, const TItemId& aLocationId)");
    CMdEObject* location = DataSource()->Session().GetObjectL(aLocationId, DataSource()->LocationDef()); 
    if(!location)
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PushL(location);
    CMdEProperty* longitude = NULL;
    CMdEPropertyDef* longitudePropertyDef = location->Def().GetPropertyDefL(KPropertyDefNameLongitude);
    TInt longitudeIndex = location->Property(*longitudePropertyDef, longitude);
    CMdEProperty* latitude = NULL;
    CMdEPropertyDef* latitudePropertyDef = location->Def().GetPropertyDefL(KPropertyDefNameLatitude);
    TInt latitudeIndex = location->Property(*latitudePropertyDef, latitude);

    if (longitudeIndex > KErrNotFound && latitudeIndex > KErrNotFound)
        {
        TCoordinate coordinate(static_cast< CMdEReal64Property *>(latitude)->Value(),static_cast< CMdEReal64Property *>(longitude)->Value());
        aMedia.SetTObjectValueL(KGlxMediaGeneralLocation, coordinate); 
        }
    
    CleanupStack::PopAndDestroy(location);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::DoHandleAttributeQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::DoHandleAttributeQueryCompletedL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::DoHandleAttributeQueryCompletedL()");
    __ASSERT_DEBUG(iQueryAttributes.Count(), Panic(EGlxPanicIllegalState));
    CMdEQuery* query =  iQueries[0];
    
    if( EQueryResultModeObjectWithoutFreetexts == query->ResultMode() )
        {
        __ASSERT_DEBUG(( iQueryAttributes[0].iAttribute == KGlxMediaCollectionInternalStartDate ), Panic(EGlxPanicIllegalState));
    	CMdEPropertyDef* creationDateDef = DataSource()->ObjectDef().GetPropertyDefL(KPropertyDefNameCreationDate);
        if (!creationDateDef  || creationDateDef->PropertyType() != EPropertyTime)
        	{
        	User::Leave(KErrCorrupt);
        	}
        TTime startMonth(0);	
        TTime endMonth(0);	
        if(iQueries[0]->Count() )
            {
            CMdEProperty* startTime;
            CMdEObject& startObject = (CMdEObject&)query->ResultItem(0);
            TInt timeIndex = startObject.Property(*creationDateDef, startTime);
            if( KErrNotFound == timeIndex )
                {
                User::Leave(KErrCorrupt);
                }
            startMonth = static_cast<CMdETimeProperty*>(startTime)->Value();
            CMdEProperty* endTime;
            CMdEObject& endObject = (CMdEObject&)query->ResultItem(query->Count()-1);
            timeIndex = endObject.Property(*creationDateDef, endTime);
            if( KErrNotFound == timeIndex )
                {
                User::Leave(KErrCorrupt);
                }
            endMonth = static_cast<CMdETimeProperty*>(endTime)->Value();
            }
        iQueryAttributes[0].iMedia->SetTObjectValueL(KGlxMediaCollectionInternalStartDate, startMonth);
        iQueryAttributes[0].iMedia->SetTObjectValueL(KGlxMediaCollectionInternalEndDate, endMonth);
        }
    else if( EQueryResultModeCount == query->ResultMode() )
        {
        iQueryAttributes[0].iMedia->SetTObjectValueL(iQueryAttributes[0].iAttribute, query->Count());
        }
    else
        {
        Panic(EGlxPanicIllegalState);
        }
    iQueryAttributes.Remove(0);
    }       

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::DoHandleLocationQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::DoHandleLocationQueryCompletedL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::DoHandleLocationQueryCompletedL()");
    TInt queryResultsCount = iQueries[0]->Count();
    
    for (TInt queryResultsPos = 0; queryResultsPos < queryResultsCount; queryResultsPos++)
        {
        CMdERelation& relation = static_cast<CMdERelation&>(iQueries[0]->ResultItem(queryResultsPos));
        CMPXMedia* targetMedia = NULL;
        if (iMediaArray)
            {
            TInt mediaArrayCount = iMediaArray->Count();
            for (TInt mediaArrayPos = 0; mediaArrayPos < mediaArrayCount; mediaArrayPos++)
                {
                CMPXMedia* media = (*iMediaArray)[mediaArrayPos];
                TMPXItemId id =  media->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                if (id.iId1 == relation.LeftObjectId())
                    {
                    targetMedia = media;
                    break;
                    }
                }
            }
        else
            {
            targetMedia = iResponse;
#ifdef _DEBUG
            TMPXItemId id =  targetMedia->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
            __ASSERT_DEBUG(id.iId1 == relation.LeftObjectId(), Panic(EGlxPanicLogicError));
#endif
            }
        
            __ASSERT_DEBUG(targetMedia, Panic(EGlxPanicLogicError));
            AddLocationAttributeToMediaL(*targetMedia, relation.RightObjectId());
          
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::DoHandleImageVideoQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::DoHandleImageVideoQueryCompletedL()
	{
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::DoHandleImageVideoQueryCompletedL()");
	CMdEQuery* query = iQueries[0];
	
    TInt queryResultsCount = query->Count();
    
    if( ( queryResultsCount == 1 ) && ( !iMediaArray ) )
        {
        CMdEObject& object = static_cast<CMdEObject&>(query->ResultItem(0));
        
        delete iResponse;
        // setiing iResponse to NULL to remove CodeScanner warning
        iResponse = NULL;
        iResponse = CMPXMedia::NewL();
        AddAttributesL(object, iResponse);
        }
    else
        {    
        for (TInt i = 0; i < queryResultsCount; i++)
            {
            CMdEObject& object = static_cast<CMdEObject&>(query->ResultItem(i));
            
            CMPXMedia* entry = CMPXMedia::NewL();
            CleanupStack::PushL(entry);
            iMediaArray->AppendL(entry);
            CleanupStack::Pop(entry);
            AddAttributesL(object, (*iMediaArray)[iMediaArray->Count() - 1]);
            }
        }    
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::QueueLocaitonQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::QueueLocaitonQueryL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::QueueLocaitonQueryL()");
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
        
    CMdEQuery* query = DataSource()->Session().NewRelationQueryL(*DataSource()->NamespaceDef(), this); 
    CleanupStack::PushL(query);
    
    CMdELogicCondition& rootCondition = query->Conditions();
    CMdERelationCondition& containerRelationCondition = rootCondition.AddRelationConditionL(DataSource()->ContainsDef(), ERelationConditionSideRight);
    CMdELogicCondition& itemLogicCondition  = containerRelationCondition.LeftL();
    CMdELogicCondition&  locationLogicCondition = containerRelationCondition.RightL();
    locationLogicCondition.AddObjectConditionL(DataSource()->LocationDef());
    locationLogicCondition.SetOperator(ELogicConditionOperatorOr);
    itemLogicCondition.SetOperator(ELogicConditionOperatorOr);
    
    TInt mediaIdCount = request->MediaIds().Count();
    
    RArray<TItemId> mediaIdArray;
    CleanupClosePushL(mediaIdArray);
    
    for (TInt i = 0; i < mediaIdCount; i++)
        {
        mediaIdArray.AppendL(request->MediaIds()[i].Value());
        }
    
    itemLogicCondition.AddObjectConditionL(mediaIdArray);
    CleanupStack::PopAndDestroy(&mediaIdArray);

    query->SetResultMode(EQueryResultModeItem);     
    
    CleanupStack::Pop(query);
    AppendQueryL(query, ELocationAttributeQuery);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::ExecuteQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeAttribute::ExecuteQueryL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::ExecuteQueryL()");
    switch (iQueryTypes[0])
        {
        case EAttributeQuery:
            {
            iQueries[0]->FindL(MaxQueryResultsCount(iQueryAttributes[0].iFilterProperties));
            }
        break;
        case EImageVideoQuery: // fall through
        case ELocationAttributeQuery:
            {
            iQueries[0]->FindL();
            }
        break;
        default:
            {
            Panic(EGlxPanicUnsupportedQueryType);
            }
        break;
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeAttribute::LocationAttributeRequested
// ----------------------------------------------------------------------------
//
TBool CGlxDataSourceTaskMdeAttribute::LocationAttributeRequested()
    {
    GLX_LOG_ENTRY_EXIT("TBool CGlxDataSourceTaskMdeAttribute::LocationAttributeRequested()");
    __ASSERT_DEBUG(dynamic_cast<CGlxGetRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxGetRequest* request = static_cast<CGlxGetRequest*>(iRequest);
    
    TInt attributeArrayCount = request->Attributes().Count();
    for (TInt i = 0; i < attributeArrayCount; i++)
        {
        if (request->Attributes()[i] == KGlxMediaGeneralLocation)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::QueueObjectQueryL
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeAttribute::QueueObjectQueryL(CMdEObjectDef& aObjectDef, TBool aIsContent, TGlxQueryType aQueryType, 
        TQueryResultMode aResultMode, const TGlxMediaId& aContainerId, TMPXAttribute aAttribute, CMPXMedia* aEntry, 
        const TGlxFilterProperties& aFilterProperties)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeAttribute::QueueObjectQueryL()");   
    CMdEObjectDef* queryBaseObject = &aObjectDef;
    if( aIsContent )
        {
        switch(aFilterProperties.iItemType)
            {
            case EGlxFilterImage: 
                {
                queryBaseObject = &DataSource()->ImageDef();
                break;
                }
            case EGlxFilterVideo:
                {
                queryBaseObject = &DataSource()->VideoDef();
                break;
                }
            default:
                {
                queryBaseObject = &DataSource()->ObjectDef();
                break;
                }
            }
        }

    CMdEQuery* query = DataSource()->Session().NewObjectQueryL(*DataSource()->NamespaceDef(), *queryBaseObject, this);
    CleanupStack::PushL(query);
    SetQueryConditionsL(*query, aFilterProperties, aContainerId, aObjectDef);
    query->SetResultMode(aResultMode);
  
    iQueryAttributes.AppendL(TGlxQueryAttribute(aAttribute, aEntry, aFilterProperties));

    CleanupStack::Pop(query);
    
   	TRAPD(err, AppendQueryL(query,aQueryType));
   	
   	if (err != KErrNone)
   		{
   		iQueryAttributes.Remove(iQueryAttributes.Count() - 1);
   		}
    }

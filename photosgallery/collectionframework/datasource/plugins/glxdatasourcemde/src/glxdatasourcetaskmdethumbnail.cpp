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
* Description:    The Data Source Task MDE THumbnail Class
*
*/




#include "glxdatasourcetaskmdethumbnail.h"

#include <caf/content.h>
#include <glxcollectionpluginCamera.hrh>
#include <glxcollectionpluginDownloads.hrh>
#include <glxcollectionpluginMonths.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxerrors.h>
#include <glxfilterfactory.h>
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
#include "glxdatasourcemde.hrh"

// CONSTANTS

_LIT(KPropertyDefNameDRM, "DRM");
_LIT(KPropertyDefNameItemType, "ItemType");
_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameSize, "Size");
_LIT(KUnsupportedItemType,  "video/x-pn-realvideo");
_LIT(KUnsupportedItemType1, "video/x-pn-multirate-realvideo");
_LIT(KUnsupportedItemType2, "video/vnd.rn-realvideo");
_LIT(KUnsupportedItemType3, "video/x-ms-wmv");

// ----------------------------------------------------------------------------
//  IsUnsupportedL
//  Tests to see if an item type is unsupported
// ----------------------------------------------------------------------------
//  
TBool IsUnsupportedL(CMdESession& aSession, TItemId aItemId)
	{
    GLX_LOG_ENTRY_EXIT("IsUnsupportedL()");
	TBool isUnsupported = EFalse;
    CMdEObject* item = aSession.GetObjectL(aItemId);
    
    if (!item)
    	{
    	User::Leave(KErrNotFound);
    	}
    
    CleanupStack::PushL(item);
    CMdEProperty* itemType = NULL;
    CMdEPropertyDef* itemTypeProperty = item->Def().GetPropertyDefL(KPropertyDefNameItemType);
    
    TInt itemTypeIndex = item->Property(*itemTypeProperty, itemType);
    if (itemTypeIndex > KErrNotFound)
    	{
    	const TDesC& des = static_cast< CMdETextProperty *>(itemType)->Value();
    	if (des.Compare(KUnsupportedItemType) == 0 || 
    	    des.Compare(KUnsupportedItemType1) == 0 || 
    	    des.Compare(KUnsupportedItemType2) == 0 ||
    	    des.Compare(KUnsupportedItemType3) == 0)
    		{
    		// The item is unsupported
    		isUnsupported = ETrue;
    		}
    	}
    CleanupStack::PopAndDestroy(item);

    return isUnsupported;
	}

// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------
// 	
CGlxDataSourceTaskMdeThumbnail:: CGlxDataSourceTaskMdeThumbnail(CGlxThumbnailRequest* aRequest, 
		                       MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource)
    : CGlxDataSourceTaskMde(aRequest, aObserver, aDataSource)
    {
    GLX_LOG_ENTRY_EXIT("CGlxDataSourceTaskMdeThumbnail::CGlxDataSourceTaskMdeThumbnail()");
    iFilterProperties = TGlxFilterFactory::ExtractAttributes(iRequest->Filter());
    }

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------
//  	
CGlxDataSourceTaskMdeThumbnail::~CGlxDataSourceTaskMdeThumbnail()
    {
    GLX_LOG_ENTRY_EXIT("CGlxDataSourceTaskMdeThumbnail::~CGlxDataSourceTaskMdeThumbnail()");    
#ifdef USE_S60_TNM
    DataSource()->CancelFetchThumbnail();
    delete iTnFileInfo;
    iTnFileInfo = NULL;
#else
    if( iTnRequestStatus )
        {
        CancelFetchUri(TGlxMediaId());
        }
    if( iTnRequestInProgress )
        {
        CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    
        TGlxThumbnailRequest tnReq;
        request->ThumbnailRequest(tnReq);
        DataSource()->ThumbnailCreator().CancelRequest(tnReq.iId);
        }
#endif        
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::ExecuteRequest
// ----------------------------------------------------------------------------
//          
void CGlxDataSourceTaskMdeThumbnail::ExecuteRequestL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::ExecuteRequestL()"); 
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    
    TGlxThumbnailRequest tnReq;
    request->ThumbnailRequest(tnReq);
#ifdef USE_S60_TNM
	if(request->ThumbnailInfo())
		{
		if (tnReq.iSizeClass.iWidth > 0 && tnReq.iSizeClass.iHeight > 0 )
			{
			DataSource()->FetchThumbnailL(iRequest, *this);
			}
		else
			{
			ThumbnailFetchComplete(KErrNone);
			}
		}
	else
		{
	   	FetchFileInfoL();
		}
#else
    iTnRequestInProgress = ETrue;
    DataSource()->ThumbnailCreator().FetchThumbnailL(tnReq, *this);
#endif
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::HandleRequestComplete
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeThumbnail::HandleRequestComplete(TInt aError)
	{
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::HandleRequestComplete()");
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    if(request && !(aError == KErrNone || aError == KErrNoMemory))
    	{
    	// An error has occured.
    	TBool isUnsupported = EFalse;
    	// If the item type is unsupported the we want to display the default
    	// icon rather than the corrupt icon.
    	TRAPD(err, isUnsupported = IsUnsupportedL(DataSource()->Session(), 
    			                          TItemId(request->ItemId().Value())));
    	if (isUnsupported)
    		{
    		aError = KErrNotSupported;
    		}
    	else if (err != KErrNone)
    		{
    		aError = err;
    		}
    	}
	CGlxDataSourceTask::HandleRequestComplete(aError);
	}

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeThumbnail::DoHandleQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeThumbnail::DoHandleQueryCompletedL(CMdEQuery& /*aQuery*/)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::DoHandleQueryCompletedL()");
    if (EContainerFirstItemQuery == iQueryTypes[0])
        {
        TRAPD(err, DoHandleContainerFirstItemQueryCompletedL());
        
        if (err != KErrNone)
            {
            CompleteThumbnailRequest(err);
            }
        }
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL
// ----------------------------------------------------------------------------
//   
void CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL(const TGlxMediaId& aId,
        TGlxThumbnailQuality aQuality)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::HandleThumbnailFetchCompleteL()");
    __ASSERT_DEBUG(dynamic_cast<CGlxThumbnailRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxThumbnailRequest* req = static_cast<CGlxThumbnailRequest*>(iRequest);
    delete iResponse;
    iResponse = NULL;
    iResponse = CMPXMedia::NewL();
    CGlxThumbnailAttribute* tnAttribute = new (ELeave) CGlxThumbnailAttribute;
    CleanupStack::PushL(tnAttribute);
    TGlxThumbnailRequest tnRequest;
    req->ThumbnailRequest(tnRequest); 
    TSize size(tnRequest.iSizeClass);
    tnAttribute->iDimensions = size;
    tnAttribute->iCroppingRect = tnRequest.iCroppingRect;
    tnAttribute->iThumbnailQuality = aQuality;

    TUint attributeId = req->AttributeId();
    if ( GlxIsFullThumbnailAttribute(attributeId) )
        {
        TBool quality = (EGlxThumbnailQualityHigh == aQuality);
        attributeId = GlxFullThumbnailAttributeId(quality,
                                                size.iWidth, size.iHeight);
        }

    iResponse->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, aId.Value());
    iResponse->SetNoNewLCObjectL(
                TMPXAttribute(KGlxMediaIdThumbnail, attributeId), tnAttribute);
    CleanupStack::PopAndDestroy(tnAttribute);
    }

#ifdef USE_S60_TNM
void CGlxDataSourceTaskMdeThumbnail::ThumbnailFetchComplete(TInt aError)
	{
    GLX_LOG_ENTRY_EXIT("CGlxDataSourceTaskMdeThumbnail::ThumbnailFetchComplete(TNM)");
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    TGlxThumbnailRequest tnRequest;
    request->ThumbnailRequest(tnRequest); 
    TInt err = aError;
    if(!err)
        {
        TRAP(err, HandleThumbnailFetchCompleteL(tnRequest.iId, EGlxThumbnailQualityHigh));
        }
    HandleRequestComplete(err);
	}
#else
// -----------------------------------------------------------------------------
// ThumbnailFetchComplete
// Notifies that a thumbnail for a given item is available, or that
// thumbnail generation failed.
// -----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeThumbnail::ThumbnailFetchComplete(
    const TGlxMediaId& aItemId, TGlxThumbnailQuality aQuality, TInt aErrorCode)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::ThumbnailFetchComplete()");
    iTnRequestInProgress = EFalse;
    TInt err = aErrorCode;
    if(!err)
        {
        TRAP(err, HandleThumbnailFetchCompleteL(aItemId, aQuality));
        }
    HandleRequestComplete(err);
    }


// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::ThumbnailDeletionComplete
// ----------------------------------------------------------------------------
//   
void CGlxDataSourceTaskMdeThumbnail::ThumbnailDeletionComplete(
                                const TGlxMediaId& /*aItemId*/, TInt /*aErrorCode*/)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::ThumbnailDeletionComplete()");
    }


// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::FilterAvailableComplete
// ----------------------------------------------------------------------------
//   
void CGlxDataSourceTaskMdeThumbnail::FilterAvailableComplete(
                                const RArray<TGlxMediaId>& /*aIdArray*/, TInt /*aErrorCode*/)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::FilterAvailableComplete()");
    // No implementation
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL(CGlxtnFileInfo* aInfo,
                        const TGlxMediaId& aItemId, TRequestStatus* aStatus)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL()");
    __ASSERT_DEBUG(dynamic_cast<CGlxThumbnailRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    if(request->ThumbnailInfo())
        {
        aInfo->CopyInfoL(*request->ThumbnailInfo());
        *aStatus = KRequestPending;
        User::RequestComplete(aStatus, KErrNone);
        return;
        }

    CMdEObject* item = DataSource()->Session().GetObjectL((TMdEItemId)aItemId.Value());
    if(!item)
        {
        User::Leave(KErrNotFound);
        }
        
    CleanupStack::PushL(item);
    iTnRequestStatus = aStatus;
    iTnFileInfo = aInfo;
    *iTnRequestStatus = KRequestPending;
    
    CGlxDataSource::TContainerType containerType = DataSource()->ContainerType(item);
    if( CGlxDataSource::EContainerTypeNotAContainer != containerType )
        {
        iTnFileInfo->iTemporary = ETrue;
        
        TGlxMediaId container = aItemId;
        CMdEObjectDef* objectDef = &item->Def();
        
        /// @todo: use default filter so we can ensure we always get correct first item if filters change
        iFilterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
        iFilterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
        iFilterProperties.iLastCaptureDate = ETrue;

        if( CGlxDataSource::EContainerTypeMonth == containerType )
            {
            AddMonthFilterL(item, iFilterProperties);
            container = DataSource()->CameraAlbumId();
            objectDef = &DataSource()->AlbumDef();
            }
            
        DoQueryL(*objectDef, ETrue, EContainerFirstItemQuery, EQueryResultModeObjectWithoutFreetexts, container);        
        }
    else
        {
        CompleteFetchFileInfoL(item);
        }

    CleanupStack::PopAndDestroy(item);
    }


// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::CancelFetchUri
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeThumbnail::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::CancelFetchUri()");
    CompleteThumbnailRequest(KErrCancel);
    }


// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::ThumbnailStorage
// ----------------------------------------------------------------------------
//  
MGlxtnThumbnailStorage* CGlxDataSourceTaskMdeThumbnail::ThumbnailStorage()
    {
    GLX_LOG_ENTRY_EXIT("MGlxtnThumbnailStorage* CGlxDataSourceTaskMdeThumbnail::ThumbnailStorage()");
    return &DataSource()->ThumbnailDatabase();
    }
#endif

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::CompleteFetchFileInfoL
// ----------------------------------------------------------------------------
//  	    
void CGlxDataSourceTaskMdeThumbnail::CompleteFetchFileInfoL(CMdEObject* aItem)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::CompleteFetchFileInfoL()");
    TParsePtrC parser(aItem->Uri());
    iTnFileInfo->SetFilePathL(parser.FullName());

    CMdEProperty* size;
    CMdEPropertyDef* sizeProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameSize);
    TInt sizeIndex = aItem->Property(*sizeProperty, size); 
    if( KErrNotFound == sizeIndex )
        {
        iTnFileInfo->iFileSize = 0;
        }
    else
        {
        iTnFileInfo->iFileSize = static_cast< CMdEInt32Property *>(size)->Value();
        }

    CMdEProperty* lastModifiedDateProperty;
    CMdEPropertyDef* lastModifiedDatePropertyDef = aItem->Def().GetPropertyDefL(KPropertyDefNameLastModifiedDate);

#ifdef _DEBUG
    TInt index = // This variable is only required for the assert debug below. If it is not wrapped in an  
                 // #ifdef _DEBUG, it will cause a warning in non debug builds.
#endif    
    aItem->Property(*lastModifiedDatePropertyDef, lastModifiedDateProperty);
    __ASSERT_DEBUG(index > KErrNotFound, Panic(EGlxPanicRequiredPropertyNotFound));
    
    iTnFileInfo->iFileTime = static_cast<CMdETimeProperty*>(lastModifiedDateProperty)->Value();
    
    CGlxDataSource::TItemType itemType = DataSource()->ItemType(aItem);
    iTnFileInfo->iIsVideo = ( CGlxDataSource::EItemTypeVideo == itemType );
    
    CMdEProperty* drmProtected;
    CMdEPropertyDef* drmProtectedProperty = aItem->Def().GetPropertyDefL(KPropertyDefNameDRM); 
    TInt drmProtectedIndex = aItem->Property(*drmProtectedProperty, drmProtected);
    TInt err = KErrNone;
    if( KErrNotFound == drmProtectedIndex)
        {
        // should be present for all normally harvested images
        // so this should only be where we didn't pre-fetch the attributes
        // i.e. background thumbnail generation
        // so we get status from CAF to avoid forcing second stage harvest
        TRAP(err, 
            ContentAccess::CContent* content = ContentAccess::CContent::NewLC(iTnFileInfo->FilePath());
            content->GetAttribute(ContentAccess::EIsProtected, iTnFileInfo->iIsProtected);
            CleanupStack::PopAndDestroy(content);
            );
        }
    else
        {
        iTnFileInfo->iIsProtected =  static_cast<CMdEBoolProperty*>(drmProtected)->Value();
        }

    CompleteThumbnailRequest(err);
    iTnFileInfo = NULL;
    iTnRequestStatus = NULL;
    }


// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeThumbnail::DoHandleContainerFirstItemQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeThumbnail::DoHandleContainerFirstItemQueryCompletedL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxDataSourceTaskMdeThumbnail::DoHandleContainerFirstItemQueryCompletedL()");
    if( !iQueries[0]->Count() )
        {
        User::Leave(KErrGlxEmptyContainer); // leave with a specific Error code so UI can display empty folder icon
        }
    
    CMdEObject* item = static_cast<CMdEObject*>(iQueries[0]->TakeOwnershipOfResult(0));
    if(!item)
        {
        User::Leave(KErrNotFound);
        }
    
    CleanupStack::PushL(item);
    CompleteFetchFileInfoL(item);
    CleanupStack::PopAndDestroy(item);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest(TInt aError)
	{
    TRACER("CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest(TInt aError)")
    GLX_LOG_INFO1("CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest  aError=%d", aError);
#ifdef USE_S60_TNM
	if (aError != KErrNone)
		{
		ThumbnailFetchComplete(aError);
		}
	else
		{
	    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
        TGlxThumbnailRequest tnReq;
	   	request->ThumbnailRequest(tnReq);

		if (tnReq.iSizeClass.iWidth > 0 && tnReq.iSizeClass.iHeight > 0 )
			{
		    request->SetThumbnailInfo(iTnFileInfo);
			DataSource()->FetchThumbnailL(iRequest, *this);
			}
		else
			{
		    GLX_LOG_INFO("CGlxDataSourceTaskMdeThumbnail::CompleteThumbnailRequest  KErrArgument");
			ThumbnailFetchComplete(KErrArgument);
			}
		}
#else
    if (iTnRequestStatus)
        {
    	*iTnRequestStatus = KRequestPending;
        User::RequestComplete(iTnRequestStatus, aError);
        iTnFileInfo = NULL;
        iTnRequestStatus = NULL;
        }
#endif
	}

#ifdef USE_S60_TNM
// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL()
    {
    GLX_LOG_ENTRY_EXIT("CGlxDataSourceTaskMdeThumbnail::FetchFileInfoL()");
    __ASSERT_DEBUG(dynamic_cast<CGlxThumbnailRequest*>(iRequest), Panic(EGlxPanicLogicError));
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(iRequest);
    CMdEObject* item = DataSource()->Session().GetObjectL(request->ItemId().Value());
    if(!item)
        {
        User::Leave(KErrNotFound);
        }
        
    CleanupStack::PushL(item);
    iTnFileInfo = new (ELeave) CGlxtnFileInfo;
    
    CGlxDataSource::TContainerType containerType = DataSource()->ContainerType(item);
    if( CGlxDataSource::EContainerTypeNotAContainer != containerType )
        {
        iTnFileInfo->iTemporary = ETrue;
        
        TGlxMediaId container = request->ItemId();
        CMdEObjectDef* objectDef = &item->Def();
        
        /// @todo: use default filter so we can ensure we always get correct first item if filters change
        iFilterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
        iFilterProperties.iSortDirection = EGlxFilterSortDirectionAscending;
        iFilterProperties.iLastCaptureDate = ETrue;

        if( CGlxDataSource::EContainerTypeMonth == containerType )
            {
            iFilterProperties.iOrigin = EGlxFilterOriginCamera;
            AddMonthFilterL(item, iFilterProperties);
            container = TGlxMediaId(KGlxCollectionRootId);
            objectDef = &DataSource()->ObjectDef();
            }
            
        DoQueryL(*objectDef, ETrue, EContainerFirstItemQuery,  EQueryResultModeObjectWithoutFreetexts, container);        
        }
    else
        {
        CompleteFetchFileInfoL(item);
        }

    CleanupStack::PopAndDestroy(item);
    }

#endif

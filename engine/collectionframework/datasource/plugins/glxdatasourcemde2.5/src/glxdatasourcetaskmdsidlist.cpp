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
* Description:   
*
*/



/**
 * @internal reviewed 11/07/2007 by M Byrne
 */

#include "glxdatasourcetaskmdsidlist.h"

#include <glxcollectionplugincamera.hrh>
#include <glxcollectionplugindownloads.hrh>
#include <glxcollectionpluginmonths.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxcollectionpluginimageviewer.hrh>

#include <glxerrors.h>
#include <glxfilterfactory.h>
#include <glxgetrequest.h>
#include <glxidlistrequest.h>
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
#include <glxtracer.h>
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

#include "glxdatasourcemds.h"
#include "glxdatasourcemds.hrh"
#include "glxdatasourcemdsutility.h"

// CONSTANTS
const TInt KGlxAlbumPromotionPosition = 0;

_LIT(KPropertyDefNameCreationDate, "CreationDate");

// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------
//	
CGlxDataSourceTaskMdeIdList::CGlxDataSourceTaskMdeIdList(CGlxIdListRequest* aRequest, MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource)
    : CGlxDataSourceTaskMde(aRequest, aObserver, aDataSource)
    {
    TRACER("CGlxDataSourceTaskMdeIdList::CGlxDataSourceTaskMdeIdList()")
    iFilterProperties = TGlxFilterFactory::ExtractAttributes(iRequest->Filter());
    }

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------
//		
CGlxDataSourceTaskMdeIdList::~CGlxDataSourceTaskMdeIdList()
    {
    TRACER("CGlxDataSourceTaskMdeIdList::~CGlxDataSourceTaskMdeIdList()")
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::FilterAvailableComplete
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeIdList::FilterAvailableComplete(
                                const RArray<TGlxMediaId>& aIdArray, TInt aErrorCode)
    {
    TRACER("CGlxDataSourceTaskMdeIdList::FilterAvailableComplete()")
    DoPostFilterComplete(aIdArray, aErrorCode);
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeIdList::ExecuteRequestL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeIdList::ExecuteRequestL()
    {
    TRACER("CGlxDataSourceTaskMdeIdList::ExecuteRequestL()") 
    CGlxIdListRequest* request = static_cast<CGlxIdListRequest*>(iRequest);
    TGlxMediaId container = request->ContainerId();
    TGlxQueryType queryType = EIdListQuery;
    TQueryResultMode resultMode = EQueryResultModeId;
    CMdEObjectDef* objectDef = &DataSource()->ObjectDef();
    TBool isContent = ETrue;

    if( KGlxCollectionRootId == container.Value() )    // The root.
        {
        switch(request->CollectionPluginUid().iUid)
            {

            case KGlxCollectionPluginAlbumsImplementationUid:
                {
                isContent = EFalse;
                objectDef = &DataSource()->AlbumDef();
                break;
                }
            case KGlxCollectionPluginCameraImplementationUid:
                {
                iFilterProperties.iOrigin = EGlxFilterOriginCamera;
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
                iFilterProperties.iOrigin = EGlxFilterOriginCamera;
                container = TGlxMediaId(KGlxCollectionRootId);
                resultMode = EQueryResultModeObjectWithFreetexts;
                break;
                }
            case KGlxCollectionPluginImageViewerImplementationUid:
                {
                RArray<TGlxMediaId> list;
                CleanupClosePushL(list);
                list.AppendL(TGlxMediaId(request->CollectionPluginUid().iUid));
                DoPostFilterComplete(list, KErrNone);
                CleanupStack::PopAndDestroy(&list);
                return;
                }
            default:
                {
                // default gallery query returns all objects as per filter
                break;
                }
            }
        }
    else    // return the content of a container
        {
        switch(request->CollectionPluginUid().iUid)
            {
            case KGlxCollectionPluginAlbumsImplementationUid:
                {
                objectDef = &DataSource()->AlbumDef();
                break;
                }
            case KGlxTagCollectionPluginImplementationUid: 
                {
                objectDef = &DataSource()->TagDef();
                break;
                }
            case KGlxCollectionPluginMonthsImplementationUid: 
                {
				iFilterProperties.iOrigin = EGlxFilterOriginCamera;
                AddMonthFilterL(container, iFilterProperties);
                container = TGlxMediaId(KGlxCollectionRootId);
                break;
                }
            default:
                {
                User::Leave(KErrNotSupported);
                break;
                }
            }
        }
        
    DoQueryL(*objectDef, isContent, queryType, resultMode, container);  
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::DoHandleQueryCompletedL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeIdList::DoHandleQueryCompletedL(CMdEQuery& /*aQuery*/)
    {
    TRACER("CGlxDataSourceTaskMdeIdList::DoHandleQueryCompletedL()")
    __ASSERT_DEBUG(iQueryTypes[0] == EIdListQuery, Panic(EGlxPanicLogicError));
    DoHandleListQueryCompletedL();
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeIdList::DoHandleListQueryCompletedL
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMdeIdList::DoHandleListQueryCompletedL()
    {
    TRACER("CGlxDataSourceTaskMdeIdList::DoHandleListQueryCompletedL()")
    if(iQueries[0]->ResultMode() == EQueryResultModeObjectWithFreetexts)
        {
        DoMonthListCreationL(*iQueries[0], iFilterProperties);
        }
    else // only id or item supported
        {

        RArray<TGlxMediaId> localList;
        CleanupClosePushL(localList);
        NGlxDataSourceMdsUtility::CopyItemIdArrayL(localList,iQueries[0]->ResultIds());
        PostFilterL(localList, iFilterProperties);
        CleanupStack::PopAndDestroy(&localList);
        }
    }     

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMdeIdList::DoMonthListCreationL
// ----------------------------------------------------------------------------
//      
void CGlxDataSourceTaskMdeIdList::DoMonthListCreationL(CMdEQuery& aQuery,
                                  const TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMdeIdList::DoMonthListCreationL()")
    CMdEProperty* time;
    CMdEPropertyDef& creationDateDef = DataSource()->ObjectDef().GetPropertyDefL(KPropertyDefNameCreationDate);
    if (creationDateDef.PropertyType() != EPropertyTime)
        {
        User::Leave(KErrCorrupt);
        }
    RArray<TGlxMediaId> monthList;
    CleanupClosePushL(monthList);
    TTime lastMonth;
    TTime currentMonth;
    TInt count = aQuery.Count();
    for( TInt i = 0 ; i < count ; i++ )
        {
        CMdEObject& object = (CMdEObject&)aQuery.ResultItem(i);
        TInt timeIndex = object.Property(creationDateDef, time);
        if( KErrNotFound == timeIndex )
            {
            User::Leave(KErrCorrupt);
            }
        currentMonth = static_cast<CMdETimeProperty*>(time)->Value();
        if( !DataSource()->SameMonth(lastMonth, currentMonth) )
            {
            const TGlxMediaId monthId = DataSource()->GetMonthIdL(currentMonth);
            monthList.AppendL(monthId);
            lastMonth = currentMonth;
            }
        }
    PostFilterL(monthList, aFilterProperties);
    CleanupStack::PopAndDestroy(&monthList);
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMdeIdList::DoPostFilterComplete
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMdeIdList::DoPostFilterComplete(
                                const RArray<TGlxMediaId>& aIdArray, TInt aErrorCode)
    {
    TRACER("CGlxDataSourceTaskMdeIdList::DoPostFilterComplete()")
    if (aErrorCode == KErrNone)
    	{
    	TRAP(aErrorCode, ListToMediaL(aIdArray));
    	}
    
    HandleRequestComplete(aErrorCode);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::PostFilterL
// ----------------------------------------------------------------------------
//		
void CGlxDataSourceTaskMdeIdList::PostFilterL(const RArray<TGlxMediaId>& 
                   aFilteredList, const TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMdeIdList::PostFilterL()")

	if( aFilterProperties.iPromoteSystemItems )
		{
		RArray<TGlxMediaId> list = aFilteredList;
		/*
		TInt favoritesIndex = list.Find(DataSource()->FavoritesId());
		if( KErrNotFound != favoritesIndex )
			{
			list.Remove(favoritesIndex);
			list.Insert(DataSource()->FavoritesId(), KGlxAlbumPromotionPosition);
			}
		*/			
		DoPostFilterComplete(list, KErrNone);
		}
	else
            {
            DoPostFilterComplete(aFilteredList, KErrNone);
            }
    }
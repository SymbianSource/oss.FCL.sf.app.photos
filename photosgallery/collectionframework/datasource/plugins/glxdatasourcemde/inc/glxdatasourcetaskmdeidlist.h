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
* Description:    Data Source Task MDE ID list
*
*/




#ifndef GLXDATASOURCETASKMDEIDLIST_H_
#define GLXDATASOURCETASKMDEIDLIST_H_

/**
 * @author Simon Brooks, Alex Birkett
 */

// INCLUDES
#include <e32cmn.h>
#include <mdequery.h>

#include <glxdatasourcetask.h>
#include <glxdatasource.h>
#include <mdccommon.h>
#include <mglxtnthumbnailcreatorclient.h>
#include <mpxfilter.h>
#include <glxfilterproperties.h>
#include "glxdatasourcetaskmde.h"
// FORWARD DECLARATIONS
class CGlxDataSourceMde;
class CGlxIdListRequest;
class CGlxtnThumbnailCreator;
class CMdEObjectDef;
class CMdEObjectQuery;
class CMdESession;

/**
 *  CGlxDataSourceTaskMdeIdList class 
 *  Task to handle id list requests.
 */
NONSHARABLE_CLASS(CGlxDataSourceTaskMdeIdList) : public CGlxDataSourceTaskMde, public MGlxtnThumbnailCreatorClient

	{
public:
    /**
     * Constructor.
     * @param aRequest request to be executed.
     * @param aObserver observer to be informed when task has completed.
     * @param aDataSource data source to be used by this object.
     */
     CGlxDataSourceTaskMdeIdList(CGlxIdListRequest* aRequest, MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource);
        
     /**
      * Destructor.
      */
     ~CGlxDataSourceTaskMdeIdList();
    	
private: // from MGlxtnThumbnailCreatorClient
	/**
	 * Not used (pure virtual stubbed)
	 * See @ref MGlxtnThumbnailCreatorClient::ThumbnailFetchComplete
	 */ 
	void ThumbnailFetchComplete(const TGlxMediaId& /*aItemId*/,
            TGlxThumbnailQuality /*aQuality*/, TInt /*aErrorCode*/) {};
	 
	/**
	 * Not used (pure virtual stubbed)
	 * See @ref MGlxtnThumbnailCreatorClient::ThumbnailDeletionComplete
	 */ 
	 void ThumbnailDeletionComplete(const TGlxMediaId& /*aItemId*/,
             TInt /*aErrorCode*/) {};
	 
	/**
	 * See @ref MGlxtnThumbnailCreatorClient::FilterAvailableComplete
	 */ 
	 void FilterAvailableComplete(const RArray<TGlxMediaId>& aIdArray,
	                                         TInt aErrorCode);
	/**
	 * Not used (pure virtual stubbed)
	 * See @ref MGlxtnThumbnailCreatorClient::FetchFileInfoL
	 */ 
	 void FetchFileInfoL(CGlxtnFileInfo* /*aInfo*/,
	                 const TGlxMediaId& /*aItemId*/, TRequestStatus* /*aStatus*/) {};
	
    /**
	 * Not used (pure virtual stubbed)
	 * See @ref MGlxtnThumbnailCreatorClient::CancelFetchUri
	 */ 
	 void CancelFetchUri(const TGlxMediaId& /*aItemId*/) {};
	 
	/**
	 * Not used (pure virtual stubbed)
	 * See @ref MGlxtnThumbnailCreatorClient::ThumbnailStorage
	 */ 
	 MGlxtnThumbnailStorage* ThumbnailStorage() { return NULL; };

public: // From CGlxDataSourceTask    
    /**
     * See @ref CGlxDataSourceTask::ExecuteRequestL
     */ 
    void ExecuteRequestL();

private: // from CGlxDataSourceTaskMde
    /**
     * See @ref CGlxDataSourceTask::DoHandleQueryCompletedL
     */ 
    void DoHandleQueryCompletedL(CMdEQuery& aQuery);

private:
	void DoHandleListQueryCompletedL();
		
    void DoMonthListCreationL(CMdEQuery& aQuery, const TGlxFilterProperties& aFilterProperties);
        
    void DoPostFilterComplete(const RArray<TGlxMediaId>& aIdArray, TInt aErrorCode);	
    
    void PostFilterL(const RArray<TGlxMediaId>& aFilteredList, const TGlxFilterProperties& aFilterProperties);                                                                            
	};

#endif // GLXDATASOURCETASKMDEIDLIST_H_

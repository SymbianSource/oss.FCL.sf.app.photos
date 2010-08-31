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



#ifndef GLXDATASOURCETASKMDETHUMBNAIL_H_
#define GLXDATASOURCETASKMDETHUMBNAIL_H_

/**
 * @author Simon Brooks, Alex Birkett
 */

#include "glxdatasourcetaskmdeattribute.h"

#include <mglxtnthumbnailcreatorclient.h>
#include <glxthumbnailrequest.h>

/**
 *  CGlxDataSourceTaskMdeThumnail class 
 *  Services thumbnail requests
 */
NONSHARABLE_CLASS(CGlxDataSourceTaskMdeThumbnail) : public CGlxDataSourceTaskMde, 
									                public MGlxtnThumbnailCreatorClient
			                    	    
	{
public:
    /**
	 * Constructor
	 * @param aRequest thumbnail request to service
	 * @param aObserver observer
	 * @param aDataSource data source
	 */
	CGlxDataSourceTaskMdeThumbnail(CGlxThumbnailRequest* aRequest, 
			MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource);

    /**
     * Destructor
     */
    ~CGlxDataSourceTaskMdeThumbnail();

private: // From CGlxDataSourceTask     
    /**
     * See @ref CGlxDataSourceTask::ExecuteRequestL
     */ 
    void ExecuteRequestL();
    
    /**
     * See @ref CGlxDataSourceTask::HandleRequestComplete
     */ 
    void HandleRequestComplete(TInt aError);

private: // From CGlxDataSourceTaskMde
    /**
     * See @ref CGlxDataSourceTaskMde::DoHandleQueryCompletedL
     */ 
    void DoHandleQueryCompletedL(CMdEQuery& aQuery);

private: // From MGlxtnThumbnailCreatorClient
    /**
     * See @ref MGlxtnThumbnailCreatorClient::ThumbnailFetchComplete
     */ 
    void ThumbnailFetchComplete(const TGlxMediaId& aItemId,
                            TGlxThumbnailQuality aQuality, TInt aErrorCode);
            
    /**
     * See @ref MGlxtnThumbnailCreatorClient::ThumbnailDeletionComplete
     */
    void ThumbnailDeletionComplete(const TGlxMediaId& aItemId, TInt aErrorCode);

    /**
     * See @ref MGlxtnThumbnailCreatorClient::FilterAvailableComplete
     */
    void FilterAvailableComplete(const RArray<TGlxMediaId>& aIdArray, TInt aErrorCode);
    
    /**
     * See @ref MGlxtnThumbnailCreatorClient::FetchFileInfoL
     */
    void FetchFileInfoL(CGlxtnFileInfo* aInfo, const TGlxMediaId& aItemId,
                    TRequestStatus* aStatus);
      
    /**
     * See @ref MGlxtnThumbnailCreatorClient::CancelFetchUri
     */
    void CancelFetchUri(const TGlxMediaId& aItemId);
    
    /**
     * See @ref MGlxtnThumbnailCreatorClient::ThumbnailStorage
     */
    MGlxtnThumbnailStorage* ThumbnailStorage();
    			
 	void HandleThumbnailFetchCompleteL(const TGlxMediaId& aId,
		TGlxThumbnailQuality aQuality);	
	
 	void DoHandleContainerFirstItemQueryCompletedL();
    
 	void CompleteFetchFileInfoL(CMdEObject* aItem);
        
private:
	void CompleteThumbnailRequest(TInt aError);
	
private:
    CGlxtnFileInfo* iTnFileInfo;

    TRequestStatus* iTnRequestStatus;

    TBool iTnRequestInProgress;
	};

#endif // GLXDATASOURCETASKMDETHUMBNAIL_H_

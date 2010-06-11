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



#ifndef GLXDATASOURCETASKMDSTHUMBNAIL_H_
#define GLXDATASOURCETASKMDSTHUMBNAIL_H_

/**
 * @author Simon Brooks, Alex Birkett
 */

#include "glxdatasourcetaskmdsattribute.h"

#ifdef USE_S60_TNM
#include "mthumbnailfetchrequestobserver.h"
#endif

#include <mglxtnthumbnailcreatorclient.h>
#include <glxthumbnailrequest.h>

/**
 *  CGlxDataSourceTaskMdeThumnail class 
 *  Services thumbnail requests
 */
NONSHARABLE_CLASS(CGlxDataSourceTaskMdeThumbnail) : public CGlxDataSourceTaskMde 
#ifdef USE_S60_TNM
									                , public MThumbnailFetchRequestObserver
#else									                
									                , public MGlxtnThumbnailCreatorClient
#endif									                
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

#ifdef USE_S60_TNM
private: //MThumbnailFetchRequestObserver
    void ThumbnailFetchComplete(TInt aError, TBool aQuality);
    void FetchFileInfoL();
#endif

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
    			
private:
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
#ifdef _DEBUG
	TTime iStartTime;
	TTime iStopTime;
#endif
	};

#endif // GLXDATASOURCETASKMDSTHUMBNAIL_H_

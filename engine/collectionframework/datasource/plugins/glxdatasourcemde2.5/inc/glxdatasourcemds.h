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
 * @author Simon Brooks, Alex Birkett
 */

/**
 * @internal reviewed 10/07/2007 by M Byrne
 */

#ifndef _C_GLXDATASOURCEMDS_H_
#define _C_GLXDATASOURCEMDS_H_

// INCLUDES
#include <e32cmn.h>
#include <f32file.h>

#include <mdesession.h>
#include <mglxtnstorage.h>
#include <mpxcollectionmessagedefs.h>

#ifdef USE_S60_TNM
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include "mthumbnailfetchrequestobserver.h"
#endif

#include "glxdatasource.h"

// FORWARD DECLARATIONS
class CGlxRequest;

#ifndef USE_S60_TNM
class CGlxtnThumbnailCreator;
class CGlxtnThumbnailDatabase;
#endif

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxDataSourceMde class 
 *
 *  Data Source Mde is a Meta Data Data Source.
 *  @lib GlxDataSourceMde.lib
 *  @ingroup collection_component_design
 */
class CGlxDataSourceMde : public CGlxDataSource,
                          public MMdESessionObserver,
                          public MMdEObjectObserver,
                          public MMdEObjectPresentObserver,
                          public MMdERelationObserver,
                          public MMdERelationPresentObserver
#ifdef USE_S60_TNM
						, public MThumbnailManagerObserver
#else
                        , public MGlxtnThumbnailStorageObserver
#endif
    {
public:
    static CGlxDataSourceMde* NewL();

private:
    ~CGlxDataSourceMde();
    
    CGlxDataSourceMde();
    
    void ConstructL();

public:
    // from CGlxDataSource
    CGlxDataSourceTask* CreateTaskL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver);

#ifdef USE_S60_TNM
public:
	void FetchThumbnailL(CGlxRequest* aRequest, MThumbnailFetchRequestObserver& aObserver);
	TInt CancelFetchThumbnail();
#else
private: 
    // from MGlxtnThumbnailStorageObserver
    /**
    * Notify that a given thumbnail is available in storage.
    * @param aId Media item ID.
    * @param aSize Requested thumbnail size.
    */
    void ThumbnailAvailable(const TGlxMediaId& aId, const TSize& aSize);
    /**
    * Notify that a given thumbnail is available in storage.
    * @param aId Media item ID.
    * @param aSize Thumbnail size.
    */
    void BackgroundThumbnailError(const TGlxMediaId& aId, TInt aError);
#endif

private: 
    /**
    *  from MMdESessionObserver
    */
    void HandleSessionOpened(CMdESession& aSession, TInt aError);
    
    void HandleSessionError(CMdESession& aSession, TInt aError);

private: // MMdEObjectObserver
	/**
	 * See @ref MMdEObjectObserver::HandleObjectNotification
	 */
	void HandleObjectNotification(CMdESession& aSession, 
						TObserverNotificationType aType,
						const RArray<TItemId>& aObjectIdArray);

private: // MMdEObjectPresentObserver
	/**
	 * See @ref MMdEObjectPresentObserver::HandleObjectPresentNotification
	 */
	void HandleObjectPresentNotification(CMdESession& aSession, 
			TBool aPresent, const RArray<TItemId>& aObjectIdArray);

private: // MMdERelationObserver
	/**
	 * See @ref MMdERelationObserver::HandleRelationNotification
	 */
	void HandleRelationNotification(CMdESession& aSession, 
			TObserverNotificationType aType,
			const RArray<TItemId>& aRelationIdArray);
	    
private: //MMdERelationPresentObserver
	/**
	 * See @ref MMdERelationPresentObserver::HandleRelationPresentNotification
	 */
	void HandleRelationPresentNotification(CMdESession& aSession,
			TBool aPresent, const RArray<TItemId>& aRelationIdArray);

#ifdef USE_S60_TNM
    // from MThumbnailManagerObserver
    void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );
    
    void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );
#endif
		
private:
    void BackgroundThumbnailMessageL(const TGlxMediaId& aId, const TSize& aSize, TInt aError);

    void DoSessionInitL();

    void AddMdEObserversL();

    void ProcessUpdateArray(const RArray<TItemId>& aArray, TMPXChangeEventType aType, TBool aIsObject);

    void ProcessItemUpdateL();

    static TInt ProcessItemUpdate(TAny* aPtr);
    void CreateSessionL();

    static TInt CreateSession(TAny* aPtr);

    void PrepareMonthsL();
    void TaskCompletedL();

    void TaskStartedL();

public:
    inline CMdESession& Session() const;
    
#ifndef USE_S60_TNM
    inline CGlxtnThumbnailCreator& ThumbnailCreator() const;

    inline CGlxtnThumbnailDatabase& ThumbnailDatabase() const;
#endif

    inline RFs& FileServerSession();

    inline const TGlxMediaId& CameraAlbumId() const;

    inline const TGlxMediaId& FavoritesId() const;

    inline CMdENamespaceDef* NamespaceDef() const;

    inline CMdEObjectDef& ObjectDef() const;

    inline CMdEObjectDef& ImageDef() const;

    inline CMdEObjectDef& VideoDef() const;
    
    inline CMdEObjectDef& AlbumDef() const;
    
    inline CMdEObjectDef& MediaDef() const;
    inline CMdEObjectDef& TagDef() const;
    
    /**
     * Return object definition used to describe months.
     * @return object definition used to describe months.
     */
    inline CMdEObjectDef& MonthDef() const;
    
    inline CMdERelationDef& ContainsDef() const;
    
    inline CMdERelationDef& ContainsLocationDef() const;

    /**
     * Get the location definition.
     * @return location definition.
     */
    inline CMdEObjectDef& LocationDef() const;
    
    TContainerType ContainerType(CMdEObject* aObject);
    
    TContainerType ContainerType(CMdEObjectDef* aObjectDef);
    
    TItemType ItemType(CMdEObject* aObject);
 
    const TGlxMediaId GetMonthIdL(const TTime& aMonth);

    TBool SameMonth(const TTime& aOldDate, const TTime& aNewDate);
     
	TBool ContainerIsLeft(CMdEObjectDef& aObjectDef);    

private:
#ifdef USE_S60_TNM
	MThumbnailFetchRequestObserver* iTnFetchObserver; 
    CThumbnailManager* iTnEngine;
    CFbsBitmap* iTnThumbnail;
    CFbsBitmap* iThumbnail;
    TThumbnailRequestId iTnThumbnailCbId;
    TBool iTnRequestInProgress;
    TInt iTnHandle;
    TGlxMediaId iMediaId;
    
	TTime iStartTime;
	TTime iStopTime;
#else
    CGlxtnThumbnailCreator*   iThumbnailCreator;
    CGlxtnThumbnailDatabase*  iThumbnailDatabase;
#endif

    CMdESession*              iSession;
    RFs iFs;
    TBool                     iSessionOpen;
    TGlxMediaId               iCameraAlbumId;
    TGlxMediaId               iFavoritesId;
    
    CMdENamespaceDef* iNameSpaceDef;
    CMdEObjectDef* iObjectDef;
    CMdEObjectDef* iImageDef;
    CMdEObjectDef* iVideoDef;
    CMdEObjectDef* iAlbumDef;
    CMdEObjectDef* iMediaDef;
    CMdEObjectDef* iTagDef;
    CMdEObjectDef* iMonthDef;
    /**
     * Location object definition.
     */
    CMdEObjectDef* iLocationDef;
    CMdERelationDef* iContainsDef;
    CMdERelationDef* iContainsLocationDef;
    
    RArray<TTime> iMonthArray;
    RArray<TGlxMediaId> iMonthList;
    TTime iFirstMonth;
    TTime iLastMonth;
    
    CAsyncCallBack* iUpdateCallback;
    
    CAsyncCallBack* iCreateSessionCallback;
    
    struct TUpdateData
        {
        TItemId iId;
        TMPXChangeEventType iType;
        TBool iIsObject;
        };
    
    RArray<TUpdateData> iUpdateData;
	TBool iPauseUpdate;
    TInt iDeletedCount;
    };

#include "glxdatasourcemds.inl"

#endif //_C_GLXDATASOURCEMDE_H_

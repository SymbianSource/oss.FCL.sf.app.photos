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



/**
 * @author Simon Brooks, Alex Birkett
 */

/**
 * @internal reviewed 10/07/2007 by M Byrne
 */

#ifndef _C_GLXDATASOURCEMDE_H_
#define _C_GLXDATASOURCEMDE_H_

// INCLUDES
#include <e32cmn.h>
#include <f32file.h>

#include <mdesession.h>
#include <mglxtnstorage.h>
#include <mpxcollectionmessagedefs.h>

#include "glxdatasource.h"

// FORWARD DECLARATIONS
class CGlxRequest;
class CGlxtnThumbnailCreator;
class CGlxtnThumbnailDatabase;

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
                          public MGlxtnThumbnailStorageObserver,
                          public MMdEObjectObserver,
                          public MMdEObjectPresentObserver,
                          public MMdERelationObserver,
                          public MMdERelationPresentObserver
    {
public:
    static CGlxDataSourceMde* NewL();

private:
    ~CGlxDataSourceMde();
    
    CGlxDataSourceMde();
    
     void ConstructL();

public: // from CGlxDataSource
    CGlxDataSourceTask* CreateTaskL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver);

private: 
    /**
    *  from MMdESessionObserver
    */
    void HandleSessionOpened(CMdESession& aSession, TInt aError);
    
    void HandleSessionError(CMdESession& aSession, TInt aError);

private: // from MGlxtnThumbnailStorageObserver
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

private: // MMdEObjectObserver
    /**
     * Called to notify the observer that new objects has been added to
     * the metadata engine database.
     *
     * @param aSession  session
     * @param aObjectIdArray       IDs of added object
     */
    void HandleObjectAdded(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray);

    /**
     * Called to notify the observer that objects has been modified in
     * the metadata engine database.
     *
     * @param aSession  session
     * @param aObjectIdArray       IDs of modified object
     */
    void HandleObjectModified(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray);

    /**
     * Called to notify the observer that objects has been removed from
     * the metadata engine database.
     *
     * @param aSession  session
     * @param aObjectIdArray       IDs of removed object
     */
    void HandleObjectRemoved(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray);
    
private: // MMdEObjectPresentObserver
    /**
     * Called to notify the observer that objects has been set to present state in
     * the metadata engine database.
     *
     * @param aSession  session
     * @param aObjectIdArray   object IDs which are set to present state
     */
    void HandleObjectPresent(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray);

    /**
     * Called to notify the observer that objects has been set to not present state in
     * the metadata engine database.
     *
     * @param aSession  session
     * @param aObjectIdArray   object IDs which are set to not present state
     */
    void HandleObjectNotPresent(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray);

private: // MMdERelationObserver
    /**
     * Called to notify the observer that new relations has been added to
     * the metadata engine database.
     *
     * @param aSession   session
     * @param aRelationIdArray  IDs of added relations
     */
    void HandleRelationAdded(CMdESession& aSession, const RArray<TItemId>& aRelationIdArray);

    /**
     * Called to notify the observer that relations has been removed from
     * the metadata engine database.
     *
     * @param aSession  session
     * @param aRelationIdArray       IDs of removed relations
     */
    void HandleRelationRemoved(CMdESession& aSession, const RArray<TItemId>& aRelationIdArray);
    
    /**
     * Called to notify the observer that relations has been modified from
     * the metadata engine database.
     *
     * @param aSession  session
     * @param aRelationIdArray       IDs of modified relations
     */
    void HandleRelationModified(CMdESession& aSession, const RArray<TItemId>& aRelationIdArray);

private: //MMdERelationPresentObserver
	/**
	 * Called to notify the observer that objects has been set to present state in
	 * the metadata engine database.
	 *
	 * @param aSession  session
	 * @param aObjectIdArray   object IDs which are set to present state
	 */
	void HandleRelationPresent(CMdESession& aSession, const RArray<TItemId>& aRelationIdArray);

	/**
	 * Called to notify the observer that objects has been set to not present state in
	 * the metadata engine database.
	 *
	 * @param aSession  session
	 * @param aObjectIdArray   object IDs which are set to not present state
	 */
	void HandleRelationNotPresent(CMdESession& aSession, const RArray<TItemId>& aRelationIdArray);
	
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
    
    inline CGlxtnThumbnailCreator& ThumbnailCreator() const;
    
    inline CGlxtnThumbnailDatabase& ThumbnailDatabase() const;
    
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

    CGlxtnThumbnailCreator*   iThumbnailCreator;

    CGlxtnThumbnailDatabase*  iThumbnailDatabase;

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
    };

#include "glxdatasourcemde.inl"

#endif //_C_GLXDATASOURCEMDE_H_

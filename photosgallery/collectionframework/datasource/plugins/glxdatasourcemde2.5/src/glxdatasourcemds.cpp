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
* Description:    Data Source MDS Class
*
*/




// INCLUDE FILES
#include "glxdatasourcemds.h"

#include <e32base.h>
#include <fbs.h>
#include <glxbackgroundtnmessagedefs.h>
#include <glxcollectionmessagedefs.h>
#include <glxcommandrequest.h>
#include <glxgetrequest.h>
#include <glxidlistrequest.h>
#include <glxrequest.h>
#include <glxthumbnailrequest.h>

#ifndef USE_S60_TNM
#include <glxtndatabase.h>
#include <glxtnthumbnailcreator.h>
#endif

#include <glxtracer.h>
#include <glxlog.h>
#include <mdeobjectcondition.h>
#include <mderelationcondition.h>
#include <mderelationdef.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>

#include "glxdatasourcetaskmds.h"
#include "glxdatasourcetaskmdsattribute.h"
#include "glxdatasourcetaskmdscommand.h"
#include "glxdatasourcetaskmdsidlist.h"
#include "glxdatasourcetaskmdsthumbnail.h"

#ifdef USE_S60_TNM
const TInt KMaxGridThumbnailWidth = 200;
#else
const TInt KGlxThumbnailCleanupAfterDeletions = 200;

_LIT(KGlxMdeDataSourceThumbnailDatabase, "glxmdstn");
#endif

_LIT(KObjectDefLocation, "Location");
_LIT(KObjectDefNameAlbum, "Album");
_LIT(KObjectDefNameImage, "Image");
_LIT(KObjectDefNameMedia, "MediaObject");
_LIT(KObjectDefNameObject, "Object");
_LIT(KObjectDefNameTag, "Tag");
_LIT(KObjectDefNameVideo, "Video");
_LIT(KRelationDefNameContains, "Contains");
_LIT(KRelationDefNameContainsLocation, "ContainsLocation");

_LIT(KGlxMdeCameraAlbumUri, "defaultalbum_captured");
_LIT(KGlxMdeFavoritesUri, "defaultalbum_favourites");

#undef __USING_INTELLIGENT_UPDATE_FILTERING

const TInt KHarvestUpdateChunkSize = 1000;

/**
 * Wait interval to recover from session creation error, in microseconds
 * This is to allow MdS to do something for us!
 */
const TInt KSessionCreationWaitInterval = 100000;

/**
 * Maximun number of retries to get default album(s) id
 */
const TInt KMaxNoOfRetry = 3;

// ---------------------------------------------------------------------------
// MPXChangeEventType
// Helper method
// ---------------------------------------------------------------------------
//
TMPXChangeEventType MPXChangeEventType(const TObserverNotificationType& aType)
	{
	TMPXChangeEventType type = EMPXItemInserted;
	
	switch (aType)
		{
		case ENotifyAdd:
			type = EMPXItemInserted;
		break;
		case ENotifyModify:
			type = EMPXItemModified;
		break;
		case ENotifyRemove:
			type = EMPXItemDeleted;
		break;
		}
	return type;
	}


// ---------------------------------------------------------------------------
// CGlxMDSShutdownObserver::NewL()
// ---------------------------------------------------------------------------
//
CGlxMDSShutdownObserver* CGlxMDSShutdownObserver::NewL( MGlxMDSShutdownObserver& aObserver,
                                                const TUid& aKeyCategory,
                                                const TInt aPropertyKey,
                                                TBool aDefineKey)
    {
    TRACER("CGlxMDSShutdownObserver* CGlxMDSShutdownObserver::NewL");
    CGlxMDSShutdownObserver* self = new( ELeave )CGlxMDSShutdownObserver( aObserver, 
                                                                  aKeyCategory,
                                                                  aPropertyKey,
                                                                  aDefineKey);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CGlxMDSShutdownObserver::CGlxMDSShutdownObserver()
// ---------------------------------------------------------------------------
//
CGlxMDSShutdownObserver::CGlxMDSShutdownObserver( MGlxMDSShutdownObserver& aObserver,
                                          const TUid& aKeyCategory,
                                          const TInt aPropertyKey,
                                          TBool aDefineKey)
    : CActive( CActive::EPriorityStandard ), iObserver( aObserver ),
      iKeyCategory( aKeyCategory ), iPropertyKey(aPropertyKey), iDefineKey( aDefineKey )
    {   
    TRACER("CGlxMDSShutdownObserver::CGlxMDSShutdownObserver()");
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CGlxMDSShutdownObserver::ConstructL()
// ---------------------------------------------------------------------------
//
void CGlxMDSShutdownObserver::ConstructL()
    { 
    TRACER("void CGlxMDSShutdownObserver::ConstructL()");
    // define P&S property types
    if (iDefineKey)
        {
        RProperty::Define(iKeyCategory,iPropertyKey,
                          RProperty::EInt,KAllowAllPolicy,KPowerMgmtPolicy);
        }
    
    // attach to the property
    TInt err = iProperty.Attach(iKeyCategory,iPropertyKey,EOwnerThread);
    User::LeaveIfError(err);
    
    // wait for the previously attached property to be updated
    iProperty.Subscribe(iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CGlxMDSShutdownObserver::~CGlxMDSShutdownObserver()
// ---------------------------------------------------------------------------
//
CGlxMDSShutdownObserver::~CGlxMDSShutdownObserver()
    {
    TRACER("CGlxMDSShutdownObserver::~CGlxMDSShutdownObserver()");
    Cancel();
    iProperty.Close();
    }

// ---------------------------------------------------------------------------
// CGlxMDSShutdownObserver::RunL()
// ---------------------------------------------------------------------------
//
void CGlxMDSShutdownObserver::RunL()
    {
    TRACER("void CGlxMDSShutdownObserver::RunL()");

    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();
    
    // retrieve the value
    TInt value = 0;
    TInt err = iProperty.Get(value);
    GLX_DEBUG2("CGlxMDSShutdownObserver::RunL(): iProperty.Get(value); returns %d", err);
    
    User::LeaveIfError(err);

    iObserver.ShutdownNotification(value);
    }

// ---------------------------------------------------------------------------
// CGlxMDSShutdownObserver::DoCancel()
// ---------------------------------------------------------------------------
//
void CGlxMDSShutdownObserver::DoCancel()
    {
    TRACER("void CGlxMDSShutdownObserver::DoCancel()");
    iProperty.Cancel();
    }



// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// NewL()
// ---------------------------------------------------------------------------
//
CGlxDataSourceMde* CGlxDataSourceMde::NewL()
	{
    TRACER("CGlxDataSourceMde* CGlxDataSourceMde::NewL()");
	CGlxDataSourceMde* ds = new (ELeave) CGlxDataSourceMde();
	CleanupStack::PushL(ds);
	ds->ConstructL();
	CleanupStack::Pop(ds); 
	return ds;
	}

// ---------------------------------------------------------------------------
// ~CGlxDataSourceMde()
// ---------------------------------------------------------------------------
//

CGlxDataSourceMde::~CGlxDataSourceMde()
	{
    TRACER("CGlxDataSourceMde::~CGlxDataSourceMde()");
    delete iSession;
    
#ifdef USE_S60_TNM
    delete iTnThumbnail;
    iTnThumbnail = NULL;    

    delete iThumbnail;
    iThumbnail = NULL;    

    delete iTnEngine;
    iTnEngine = NULL;    
#else 
     if (iThumbnailCreator)
    	{
    	iThumbnailCreator->Close(iThumbnailDatabase);
    	}
    delete iThumbnailDatabase;
#endif
    iFs.Close();
    iHC.Close();
    RFbsSession::Disconnect();
    iUpdateData.Close();
    iAddedItems.Reset();
    iAddedItems.Close();
    delete iUpdateCallback;
    delete iCreateSessionCallback;
    delete iMDSShutdownObserver ;
	}

// ---------------------------------------------------------------------------
// CGlxDataSourceMde()
// ---------------------------------------------------------------------------
//
CGlxDataSourceMde::CGlxDataSourceMde()
	{
    TRACER("CGlxDataSourceMde::CGlxDataSourceMde()");
    //No Implementation
	}

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::ConstructL()
	{
    TRACER("CGlxDataSourceMde::ConstructL()");
    
    iNoOfRetry = 0;
	iDataSourceReady = EFalse;			
    User::LeaveIfError(iFs.Connect());
	iSession = CMdESession::NewL( *this );
            
    User::LeaveIfError(RFbsSession::Connect());

#ifdef USE_S60_TNM
    iTnEngine = CThumbnailManager::NewL( *this);
    iTnEngine->SetDisplayModeL( EColor16M );
    iTnRequestInProgress = EFalse;
#else
	iThumbnailCreator = CGlxtnThumbnailCreator::InstanceL();
	iThumbnailDatabase = CGlxtnThumbnailDatabase::NewL(
            	                        KGlxMdeDataSourceThumbnailDatabase, this);
#endif
            	                        
    iCreateSessionCallback = new ( ELeave )
	    CAsyncCallBack( TCallBack( CreateSession, this ), CActive::EPriorityHigh );
    
    iMDSShutdownObserver = CGlxMDSShutdownObserver::NewL( *this, KHarvesterPSShutdown, KMdSShutdown, EFalse );
    
    iUpdateCallback = new ( ELeave )
	    CAsyncCallBack( TCallBack( ProcessItemUpdate, this ), CActive::EPriorityLow );
    iUpdateData.Reserve(100); // ignore if it fails
    
    User::LeaveIfError(iHC.Connect());
    iHC.AddHarvesterEventObserver(*this, EHEObserverTypePlaceholder, KHarvestUpdateChunkSize);

    iHarvestingOngoing = EFalse;
	}
	
// ----------------------------------------------------------------------------
// from MMdESessionObserver
// CMPXCollectionMdEPlugin::HandleSessionOpened
// ----------------------------------------------------------------------------
//    
void CGlxDataSourceMde::HandleSessionOpened( CMdESession& aSession, TInt aError )    
    {
    TRACER("CGlxDataSourceMde::HandleSessionOpened(CMdESession& aSession, TInt aError)");
    GLX_DEBUG2("CGlxDataSourceMde::HandleSessionOpened() aError(%d)", aError);   
    if( KErrNone != aError )
        {
        HandleSessionError(aSession, aError);
        }
    TRAPD(err, DoSessionInitL());
    if (KErrNone == err)
        {
        iNoOfRetry = 0;
        iSessionOpen = ETrue;
        iDataSourceReady = ETrue;
        TryStartTask(ETrue);
        }
    else
        {
        HandleSessionError(aSession, err);
        }
    }
    
// ----------------------------------------------------------------------------
// from MMdESessionObserver
// CMPXCollectionMdEPlugin::HandleSessionError
// ----------------------------------------------------------------------------
//     
void CGlxDataSourceMde::HandleSessionError(CMdESession& /*aSession*/, TInt aError )    
    {
    TRACER("CGlxDataSourceMde::HandleSessionError(CMdESession& /*aSession*/, TInt aError)")
    GLX_DEBUG2("void CGlxDataSourceMde::HandleSessionError() aError(%d)", aError);

    iDataSourceReady = EFalse;
    iSessionOpen = EFalse;

    //Wait for 'KSessionCreationWaitInterval' before again requesting for 
    //MDS session in case of failure.
    User::After(KSessionCreationWaitInterval);

    // We wait till MDS restarts before starting the session if the current session is locked.
    // that is handled separately by the MDS Shutdown PUB SUB Framework.   
    // for everything else we use the generic method and continue.
    if ( (KErrLocked != aError) && ( KErrServerTerminated != aError) )
        {
        iNoOfRetry++;
        GLX_DEBUG2("CGlxDataSourceMde::HandleSessionError() iNoOfRetry(%d)", iNoOfRetry);
        if (iNoOfRetry <= KMaxNoOfRetry)
            {
            GLX_DEBUG1("CGlxDataSourceMde::DoSessionInitL() Retry to create session with MdS");
            iCreateSessionCallback->CallBack();
            }
        }
    }

// ---------------------------------------------------------------------------
// CreateTaskL
// ---------------------------------------------------------------------------
//
CGlxDataSourceTask* CGlxDataSourceMde::CreateTaskL(CGlxRequest* aRequest, 
        MGlxDataSourceRequestObserver& aObserver)
	{
    TRACER("CGlxDataSourceTask* CGlxDataSourceMde::CreateTaskL(CGlxRequest* aRequest,MGlxDataSourceRequestObserver& aObserver)")	;
	if(dynamic_cast<CGlxCommandRequest*>(aRequest))
		{
        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeCommand* task = new (ELeave) 
        CGlxDataSourceTaskMdeCommand(static_cast<CGlxCommandRequest*>(aRequest),
                aObserver, this);
		CleanupStack::Pop(aRequest); // now owned by task
        CleanupStack::PushL(task);
        task->ConstructL();
		CleanupStack::Pop(task);
		return task;
		}
	else if (dynamic_cast< CGlxGetRequest *>(aRequest))
		{
        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeAttributeMde* task = new (ELeave) 
        CGlxDataSourceTaskMdeAttributeMde(static_cast<CGlxGetRequest*>(aRequest),
                aObserver, this);
		CleanupStack::Pop(aRequest); // now owned by task
        CleanupStack::PushL(task);
        task->ConstructL();
		CleanupStack::Pop(task);
		return task;
		}
	else if (dynamic_cast< CGlxIdListRequest *>(aRequest))
		{	
        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeIdList* task = new (ELeave) 
        CGlxDataSourceTaskMdeIdList(static_cast<CGlxIdListRequest*>(aRequest), 
                aObserver, this);
        CleanupStack::Pop(aRequest); // now owned by task
        CleanupStack::PushL(task); 
        task->ConstructL();
        CleanupStack::Pop(task);
        return task;
		}
	else if (dynamic_cast< CGlxThumbnailRequest *>(aRequest))
		{	
        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeThumbnail* task = new (ELeave) 
        CGlxDataSourceTaskMdeThumbnail(static_cast<CGlxThumbnailRequest*>(aRequest), 
                aObserver, this);
        CleanupStack::Pop(aRequest); // now owned by task
        CleanupStack::PushL(task); 
        task->ConstructL();
        CleanupStack::Pop(task);
        return task;
		}
	else
		{
		User::Leave(KErrNotSupported);
		return NULL; // stops compiler warning
		}
	}

#ifndef USE_S60_TNM
// ---------------------------------------------------------------------------
// ThumbnailAvailable
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::ThumbnailAvailable(const TGlxMediaId& 
        /*aId*/, const TSize& /*aSize*/)
	{
    TRACER("CGlxDataSourceMde::ThumbnailAvailable(const TGlxMediaId& /*aId*/, const TSize& /*aSize*/)");
	//No implementation
	}

// ---------------------------------------------------------------------------
// BackgroundThumbnailError
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::BackgroundThumbnailError(const TGlxMediaId& aId, TInt aError)
	{
    TRACER("CGlxDataSourceMde::BackgroundThumbnailError(const TGlxMediaId& aId, TInt aError)");
	TSize size(0, 0);
	TRAP_IGNORE(BackgroundThumbnailMessageL(aId, size, aError));
	}
#endif

// ---------------------------------------------------------------------------
// BackgroundThumbnailMessageL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::BackgroundThumbnailMessageL(const TGlxMediaId& aId, 
        const TSize& aSize, TInt aError)
	{
    TRACER("CGlxDataSourceMde::BackgroundThumbnailMessageL(const TGlxMediaId& aId, const TSize& aSize, TInt aError)");
	CMPXMessage* message = CMPXMessage::NewL();
	CleanupStack::PushL(message);
	message->SetTObjectValueL(KMPXMessageGeneralId, KGlxMessageIdBackgroundThumbnail);
	message->SetTObjectValueL<TMPXItemId>(KGlxBackgroundThumbnailMediaId, aId.Value());
	message->SetTObjectValueL(KGlxBackgroundThumbnailSize, aSize);
	message->SetTObjectValueL(KGlxBackgroundThumbnailError, aError);
	BroadcastMessage(*message);
	CleanupStack::PopAndDestroy(message);
	}

// ---------------------------------------------------------------------------
// DoSessionInitL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::DoSessionInitL()
	{
    TRACER("CGlxDataSourceMde::DoSessionInitL()");
	/// @todo check schema version number
    iNameSpaceDef = &iSession->GetDefaultNamespaceDefL();
    
    TBool retry = EFalse;
    CMdEObject* cameraAlbum = iSession->GetObjectL(KGlxMdeCameraAlbumUri);
    if (cameraAlbum)
        {
        iCameraAlbumId = (TGlxMediaId) cameraAlbum->Id();
        delete cameraAlbum;
        }
    else
        {
        GLX_DEBUG1("CGlxDataSourceMde::DoSessionInitL() failed CameraAlbum");
        iCameraAlbumId = TGlxMediaId(KErrNone);
        retry = ETrue;
        }

    CMdEObject* favorites = iSession->GetObjectL(KGlxMdeFavoritesUri);
    if (favorites)
        {
        iFavoritesId = (TGlxMediaId) favorites->Id();
        delete favorites;
        }
    else
        {
        GLX_DEBUG1("CGlxDataSourceMde::DoSessionInitL() failed Favorites");
        iFavoritesId = TGlxMediaId(KErrNone);
        retry = ETrue;
        }

    GLX_DEBUG3("CGlxDataSourceMde::DoSessionInitL() retry(%d), iNoOfRetry(%d)", retry, iNoOfRetry);
    if (retry && iNoOfRetry < KMaxNoOfRetry)
        {
        GLX_DEBUG1("CGlxDataSourceMde::DoSessionInitL() Retry to get default album(s)");
        User::Leave(KErrNotFound);
        }
	
    iContainsDef = &iNameSpaceDef->GetRelationDefL(KRelationDefNameContains);
    iContainsLocationDef = &iNameSpaceDef->GetRelationDefL(KRelationDefNameContainsLocation);
    
    iObjectDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameObject);
    iImageDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameImage);
    iVideoDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameVideo);
    iMediaDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameMedia);
    iAlbumDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameAlbum);
    iTagDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameTag);
    iLocationDef = &iNameSpaceDef->GetObjectDefL(KObjectDefLocation);
	
	AddMdEObserversL();
	}

// ---------------------------------------------------------------------------
// AddMdEObserversL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::AddMdEObserversL()
    {
    TRACER("CGlxDataSourceMde::AddMdEObserversL()");
	iSession->AddRelationObserverL(*this);
	iSession->AddRelationPresentObserverL(*this);
	
	//when setting observing conditions,
	//add filters for all images, videos, Albums & Tags
	CMdELogicCondition* addCondition = CMdELogicCondition::NewLC( ELogicConditionOperatorOr );
	addCondition->AddObjectConditionL( *iImageDef );
	addCondition->AddObjectConditionL( *iVideoDef );
	addCondition->AddObjectConditionL( *iAlbumDef );
	addCondition->AddObjectConditionL( *iTagDef );
	
	iSession->AddObjectObserverL(*this, addCondition );
	iSession->AddObjectPresentObserverL(*this );
		
	// This addCondition should only be popped. 
	// As the ownership is transferred, the same will be destroyed by MdS.
	CleanupStack::Pop( addCondition ); 
    }

// ---------------------------------------------------------------------------
// CGlxDataSourceMde::HandleObjectNotification
// ---------------------------------------------------------------------------
//
///@todo AB test this
void CGlxDataSourceMde::HandleObjectNotification(CMdESession& /*aSession*/, 
					TObserverNotificationType aType,
					const RArray<TItemId>& aObjectIdArray)
	{
    TRACER("CGlxDataSourceMde::HandleObjectNotification()");
    GLX_LOG_INFO3("CGlxDataSourceMde::HandleObjectNotification() aType=%d, aObjectIdArray.Count()=%d, iHarvestingOngoing=%d", 
															  aType, aObjectIdArray.Count(),
															  iHarvestingOngoing);
   	if (ENotifyAdd == aType)
		{
	    for ( TInt i = 0; i < aObjectIdArray.Count(); i++ )
	        {
            TInt ret = iAddedItems.Append(aObjectIdArray[i]);
            if (ret != KErrNone)
                {
                GLX_DEBUG2("ENotifyAdd-iAddedItems.Append() failed i(%d)", i);
                }
	        }
	    GLX_DEBUG2("ENotifyAdd - iAddedItems.Count()=%d", iAddedItems.Count());
		}
    
   	if (ENotifyModify == aType)
		{
	    for ( TInt i = 0; i < aObjectIdArray.Count(); i++ )
	        {
	        if (iAddedItems.Find(aObjectIdArray[i]) != KErrNotFound)
	        	{
		        if (!iHarvestingOngoing)
		        	{
		        	GLX_DEBUG1("ENotifyModify - Harvesting Completed - "
                            "Reset iAddedItems array");
					iAddedItems.Reset();
					break;
		        	}
                GLX_DEBUG1("ENotifyModify - Id found in iAddedItems array, DO NOT PROCESS");
	        	return;
	        	}
	        }
        }

   	GLX_DEBUG1("HandleObjectNotification - ProcessUpdateArray");
	ProcessUpdateArray(aObjectIdArray,  MPXChangeEventType(aType), ETrue);
#ifndef USE_S60_TNM
	if(MPXChangeEventType(aType) == EMPXItemDeleted )
		{			
		TInt count = aObjectIdArray.Count();
		iDeletedCount += count;
		GLX_DEBUG3("EMPXItemDeleted - aObjectIdArray.Count()=%d, iDeletedCount=%d", 
		        count, iDeletedCount);
		if(iDeletedCount > KGlxThumbnailCleanupAfterDeletions)
		    {
	    	TRAPD(err, ThumbnailCreator().CleanupThumbnailsL(iThumbnailDatabase));
	    	if(!err)
	    	    {
	    	    iDeletedCount = 0;
	    	    }
		    }
		}

	if(MPXChangeEventType(aType) == EMPXItemModified )
	    {
	    GLX_DEBUG1("HandleObjectNotification - EMPXItemModified");
	    TRAP_IGNORE(ThumbnailCreator().CleanupThumbnailsL(iThumbnailDatabase));
		}
#endif		
	}

// ---------------------------------------------------------------------------
// CGlxDataSourceMde::HandleObjectPresentNotification
// ---------------------------------------------------------------------------
//
///@todo AB test this
void CGlxDataSourceMde::HandleObjectPresentNotification(CMdESession& /*aSession*/, 
		TBool aPresent, const RArray<TItemId>& aObjectIdArray)
	{
    TRACER("CGlxDataSourceMde::HandleObjectPresentNotification()");
	if (aPresent)
		{
		ProcessUpdateArray(aObjectIdArray, EMPXItemInserted, ETrue);
		}
	else
		{
		ProcessUpdateArray(aObjectIdArray,  EMPXItemDeleted, ETrue);
		}
	}

// ---------------------------------------------------------------------------
// CGlxDataSourceMde::HandleRelationNotification
// ---------------------------------------------------------------------------
//
///@todo AB test this
void CGlxDataSourceMde::HandleRelationNotification(CMdESession& /*aSession*/, 
			TObserverNotificationType aType,
			const RArray<TItemId>& aRelationIdArray)
	{
    TRACER("CGlxDataSourceMde::HandleRelationNotification()");
	ProcessUpdateArray(aRelationIdArray, MPXChangeEventType(aType), EFalse);
	}

// ---------------------------------------------------------------------------
// CGlxDataSourceMde::HandleRelationPresentNotification
// ---------------------------------------------------------------------------
//
///@todo AB test this
void CGlxDataSourceMde::HandleRelationPresentNotification(CMdESession& /*aSession*/,
			TBool aPresent, const RArray<TItemId>& aRelationIdArray)
	{
    TRACER("CGlxDataSourceMde::HandleRelationPresentNotification()");
	if (aPresent)
		{
        ProcessUpdateArray(aRelationIdArray, EMPXItemInserted, EFalse);
		}
	else
		{
		ProcessUpdateArray(aRelationIdArray, EMPXItemDeleted, EFalse);
		}
	}

// ---------------------------------------------------------------------------
// ProcessUpdateArray
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::ProcessUpdateArray(const RArray<TItemId>& aArray, 
        TMPXChangeEventType aType, TBool aIsObject)
	{
    TRACER("CGlxDataSourceMde::ProcessUpdateArray(const RArray<TItemId>& aArray,TMPXChangeEventType aType, TBool aIsObject)");
    // only need one message so process first item
    TUpdateData update;
    update.iId = aArray[0];
    update.iType = aType;
    update.iIsObject = aIsObject;
    if( iUpdateData.Count() )
        {
        if( ( iUpdateData[0].iType == aType ) && ( iUpdateData[0].iIsObject ) )
            {
            return;
            }
        }
    if( iUpdateData.Append(update) == KErrNone ) // if we can't allocate space for the update, ignore it
        {
        iUpdateCallback->CallBack();
        }
	}
	
// ---------------------------------------------------------------------------
// CreateSession
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::CreateSession()
    {
    TRACER("CGlxDataSourceMde::CreateSession()")
    TRAP_IGNORE(CreateSessionL());
    }
    
// ---------------------------------------------------------------------------
// CreateSession
// ---------------------------------------------------------------------------
//
TInt CGlxDataSourceMde::CreateSession(TAny* aPtr)
    {
    TRACER("CGlxDataSourceMde::CreateSession(TAny* aPtr)");
    CGlxDataSourceMde* self
                    = reinterpret_cast<CGlxDataSourceMde*>( aPtr );
    TRAP_IGNORE(self->CreateSessionL());
    return 0;
    }
    
// ---------------------------------------------------------------------------
// CreateSessionL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::CreateSessionL()
    {
    TRACER("CGlxDataSourceMde::CreateSessionL()");
    delete iSession;
    iSession = NULL;
	iSession = CMdESession::NewL( *this );
    }

// ---------------------------------------------------------------------------
// ProcessItemUpdate
// ---------------------------------------------------------------------------
//
TInt CGlxDataSourceMde::ProcessItemUpdate(TAny* aPtr)
    {
    TRACER("CGlxDataSourceMde::ProcessItemUpdate(TAny* aPtr)");
    CGlxDataSourceMde* self
                    = reinterpret_cast<CGlxDataSourceMde*>( aPtr );
    TRAP_IGNORE(self->ProcessItemUpdateL());
    return 0;
    }
    
// ---------------------------------------------------------------------------
// ProcessItemUpdateL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::ProcessItemUpdateL()
    {
    TRACER("CGlxDataSourceMde::ProcessItemUpdateL()");
	if ( !iUpdateData.Count() || iPauseUpdate )
        {
        return;
        }
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL(message);
    message->SetTObjectValueL<TInt>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
    message->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType,
            iUpdateData[0].iType);
    TMPXGeneralCategory category = EMPXNoCategory;
	TMPXItemId id = iUpdateData[0].iId;
	
#ifdef __USING_INTELLIGENT_UPDATE_FILTERING
	if ( !iUpdateData[0].iIsObject )
		{
		TMPXGeneralCategory containerCategory = EMPXNoCategory;
		TMPXItemId containerId;
		
		CMdERelation* relation = iSession->GetRelationL(id);
		if( relation )
			{
			TItemId rightId = relation->RightObjectId();
			TItemId leftId = relation->LeftObjectId();
    		delete relation;
    		
    		CMdEObject* contObject = iSession->GetObjectL(leftId);
   		    __ASSERT_DEBUG(contObject, Panic(EGlxPanicIllegalState));
    		TContainerType container = ContainerType(contObject);
    		delete contObject;
	    	__ASSERT_DEBUG(( EContainerTypeTag != container), Panic(EGlxPanicIllegalState));
    		if( EContainerTypeNotAContainer == container )
    			{
    			CMdEObject* rightObject = iSession->GetObjectL(rightId);
    		    __ASSERT_DEBUG(rightObject, Panic(EGlxPanicIllegalState));
    			TContainerType rightContainer = ContainerType(rightObject);
    			delete rightObject;
   		    	__ASSERT_DEBUG(( EContainerTypeAlbum != rightContainer), 
   		    	        Panic(EGlxPanicIllegalState));
    			if( EContainerTypeTag == rightContainer )
    				{
        			id = leftId;
        			containerId = rightId;
    		    	containerCategory = EMPXTag;
    				}
    			else if( EContainerTypeNotAContainer == rightContainer )
    				{
    				User::Leave(KErrNotSupported); // Not a gallery relation.
    				}
    			}
    		else if( EContainerTypeAlbum == container)
    	    	{
    			id = rightId;
    			containerId = leftId;
    	    	containerCategory = EMPXAlbum;
    	    	}
    		message->SetTObjectValueL<TMPXGeneralCategory>(KGlxCollectionMessageContainerCategory,
    		        containerCategory);
    		message->SetTObjectValueL<TMPXItemId>(KGlxCollectionMessageContainerId, 
    		        containerId);
			}
	    else
	        {
  	        // use id 0 to identify to ML that we don't know what was deleted
	        id = 0;
	        }
		}

	if ( id != 0 )
	    {
	    CMdEObject* object = iSession->GetObjectL(id);
    	if( object )
    		{
        	TContainerType container = ContainerType(object);
            if( EContainerTypeAlbum == container)
            	{
            	category = EMPXAlbum;
            	}
            else if( EContainerTypeTag == container)
            	{
            	category = EMPXTag;
            	}
            else
            	{
            	TItemType item = ItemType(object);
                if( EItemTypeImage == item)
                	{
                	category = EMPXImage;
                	}
                else if( EItemTypeVideo == item)
                	{
                	category = EMPXVideo;
                	}
            	}
        	delete object;
    		}
	    }
#endif // __USING_INTELLIGENT_UPDATE_FILTERING
	message->SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory,
	        category);
   	message->SetTObjectValueL<TMPXItemId>(KMPXMessageMediaGeneralId, id);
    BroadcastMessage(*message); 
    CleanupStack::PopAndDestroy(message);
    iUpdateData.Remove(0);
    }

// ---------------------------------------------------------------------------
// ContainerType
// ---------------------------------------------------------------------------
//
CGlxDataSource::TContainerType CGlxDataSourceMde::ContainerType(CMdEObject* aObject)
	{
    TRACER("CGlxDataSourceMde::ContainerType(CMdEObject* aObject)");
	TContainerType containerType = EContainerTypeNotAContainer;
 	
	if( 0 == aObject->Def().Compare(*iAlbumDef) )
		{
		containerType = EContainerTypeAlbum;
		}
	else if( 0 == aObject->Def().Compare(*iTagDef) )
		{
		containerType = EContainerTypeTag;
		}

	return containerType;
	}


// ---------------------------------------------------------------------------
// ContainerType
// ---------------------------------------------------------------------------
//	
CGlxDataSource::TContainerType CGlxDataSourceMde::ContainerType(CMdEObjectDef* 
        aObjectDef)
	{
    TRACER("CGlxDataSourceMde::ContainerType()");
	TContainerType containerType = EContainerTypeNotAContainer;
 	
	if( 0 == aObjectDef->Compare(*iAlbumDef) )
		{
		containerType = EContainerTypeAlbum;
		}
	else if( 0 == aObjectDef->Compare(*iTagDef) )
		{
		containerType = EContainerTypeTag;
		}

	return containerType;
	}
	
// ---------------------------------------------------------------------------
// ItemType()
// ---------------------------------------------------------------------------
//
CGlxDataSource::TItemType CGlxDataSourceMde::ItemType(CMdEObject* aObject)
	{
    TRACER("CGlxDataSourceMde::ItemType(CMdEObject* aObject)");
	TItemType itemType = EItemTypeNotAnItem;
	
	if( 0 == aObject->Def().Compare(*iImageDef) )
		{
		itemType = EItemTypeImage;
		}
	else if(0 == aObject->Def().Compare(*iVideoDef) )
		{
		itemType = EItemTypeVideo;
		}
	
	return itemType;
	} 

// ---------------------------------------------------------------------------
// ContainerIsLeft
// ---------------------------------------------------------------------------
//
TBool CGlxDataSourceMde::ContainerIsLeft(CMdEObjectDef& aObjectDef)
    {
    TRACER("CGlxDataSourceMde::ContainerIsLeft(CMdEObjectDef& aObjectDef)")
    TBool containerLeft = EFalse;
    if ( 0 == aObjectDef.Compare(AlbumDef()) )
        {
        containerLeft = ETrue;
        }
    return containerLeft;
    }
    
// ---------------------------------------------------------------------------
// CGlxDataSource
// TaskCompletedL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::TaskCompletedL()
    {
	iPauseUpdate = EFalse;
    iUpdateCallback->CallBack();
    }
    
// ---------------------------------------------------------------------------
// CGlxDataSource
// TaskStartedL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::TaskStartedL()
    {
    iPauseUpdate = ETrue;
    }	
    
#ifdef USE_S60_TNM
void CGlxDataSourceMde::FetchThumbnailL(CGlxRequest* aRequest, 
        MThumbnailFetchRequestObserver& aObserver)
	{
    TRACER("CGlxDataSourceMde::FetchThumbnailL()");
#ifdef _DEBUG
    iStartTime.HomeTime(); // Get home time
#endif
    
	iTnFetchObserver = &aObserver;
	
    CGlxThumbnailRequest* request = static_cast<CGlxThumbnailRequest*>(aRequest);
    
    TGlxThumbnailRequest tnReq;
    request->ThumbnailRequest(tnReq);
	User::LeaveIfNull(request->ThumbnailInfo());

	iTnHandle = tnReq.iBitmapHandle;
	iMediaId = tnReq.iId;
    if (tnReq.iSizeClass.iWidth < KMaxGridThumbnailWidth)
    	{
   		iTnEngine->SetFlagsL(CThumbnailManager::ECropToAspectRatio);
	    iTnEngine->SetThumbnailSizeL(EGridThumbnailSize);
	    GLX_LOG_INFO("CGlxDataSourceMde::FetchThumbnailL() - Fetch TN attrib -"
	            " EGridThumbnailSize");
    	}
    else
    	{
   		iTnEngine->SetFlagsL(CThumbnailManager::EDefaultFlags);
    	iTnEngine->SetThumbnailSizeL(EFullScreenThumbnailSize);
    	GLX_LOG_INFO("CGlxDataSourceMde::FetchThumbnailL() - Fetch TN attrib - "
    	        "EFullScreenThumbnailSize");
    	}

    if (tnReq.iPriorityMode == TGlxThumbnailRequest::EPrioritizeQuality)
        {
        iTnEngine->SetQualityPreferenceL(CThumbnailManager::EOptimizeForQuality);
        GLX_LOG_INFO("CGlxDataSourceMde::FetchThumbnailL() - Fetch TN attrib -"
                " EOptimizeForQuality");
        }
    else
        {
        iTnEngine->SetQualityPreferenceL(CThumbnailManager::EOptimizeForQualityWithPreview);
        GLX_LOG_INFO("CGlxDataSourceMde::FetchThumbnailL() - Fetch TN attrib -"
                " EOptimizeForQualityWithPreview");
        }
    
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
                                     request->ThumbnailInfo()->FilePath(), 0);
    iTnThumbnailCbId = iTnEngine->GetThumbnailL(*source);
    CleanupStack::PopAndDestroy(source);

    iTnRequestInProgress = ETrue;
	}

TInt CGlxDataSourceMde::CancelFetchThumbnail()
	{
    TRACER("CGlxDataSourceMde::CancelFetchThumbnail");
	TInt ret = KErrNone;
	if (iTnRequestInProgress)
		{
		ret = iTnEngine->CancelRequest(iTnThumbnailCbId);
		iTnRequestInProgress = EFalse;
        iTnFetchObserver->ThumbnailFetchComplete(KErrCancel,EFalse);
		}
	return ret;
	}

// Called when preview thumbnail is created
void CGlxDataSourceMde::ThumbnailPreviewReady(MThumbnailData& aThumbnail, 
                                              TThumbnailRequestId aId)
    {
    TRACER("CGlxDataSourceMde::ThumbnailPreviewReady()");
    
    TInt error = KErrNotSupported;
    if ( aThumbnail.Bitmap() )
         {
         GLX_DEBUG1("CGlxDataSourceMde::ThumbnailPreviewReady preview aval");
         error = KErrNone;
         }
    //This function is called number of times as a callback ,
    //hence not trapping the leaving function which costs time and memory.
    //Ignoring this for code scanner warnings - Leaving functions called in non-leaving functions.
    ThumbnailReadyL(error, aThumbnail, aId, EFalse);
    }

// Called when real thumbnail is created
void CGlxDataSourceMde::ThumbnailReady(TInt aError,
        MThumbnailData& aThumbnail, TThumbnailRequestId aId)
	{
	TRACER("CGlxDataSourceMde::ThumbnailReady");
	GLX_DEBUG2("GlxDataSourceMde::ThumbnailReady aError=%d", aError);
	//This function is called number of times as a callback ,
    //hence not trapping the leaving function which costs time and memory.
    //Ignoring this for code scanner warnings - Leaving functions called in non-leaving functions.
	ThumbnailReadyL(aError,aThumbnail, aId, ETrue);
	}

// ---------------------------------------------------------------------------
// ThumbnailReadyL
// ---------------------------------------------------------------------------
//  
void CGlxDataSourceMde::ThumbnailReadyL(TInt aError,
        MThumbnailData& aThumbnail, TThumbnailRequestId aId, TBool aQuality)
    {
    TRACER("CGlxDataSourceMde::ThumbnailReadyL()");
    GLX_DEBUG3("CGlxDataSourceMde::ThumbnailReadyL aError=%d, aQuality=%d",
                                aError, aQuality);
#ifdef _DEBUG
    iStopTime.HomeTime(); // Get home time
    GLX_DEBUG2("=>CGlxDataSourceMde::ThumbnailReadyL - TN Fetch took <%d> us",
                        (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif
    
    if (iTnThumbnailCbId == aId)
        {
        if (aError == KErrNone && iTnHandle)
            {
            if (iTnHandle == KGlxMessageIdBackgroundThumbnail)
                {
                BackgroundThumbnailMessageL(iMediaId, TSize(), aError);
                }
            else
                {
                delete iTnThumbnail;
                iTnThumbnail = NULL;
                iTnThumbnail = aThumbnail.DetachBitmap();

                delete iThumbnail;
                iThumbnail = NULL;
                iThumbnail = new (ELeave) CFbsBitmap();
                User::LeaveIfError( iThumbnail->Duplicate(iTnHandle));
                User::LeaveIfError(iThumbnail->Resize(iTnThumbnail->SizeInPixels()));
                CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(iThumbnail);
                CleanupStack::PushL(device );
                CFbsBitGc* context = NULL;
                User::LeaveIfError(device->CreateContext(context));
                CleanupStack::PushL(context);
                context->BitBlt( TPoint(), iTnThumbnail);
                CleanupStack::PopAndDestroy(context); 
                CleanupStack::PopAndDestroy(device);
                }
            }
        }
    
    if (iTnFetchObserver && iTnRequestInProgress)
        {
        iTnFetchObserver->ThumbnailFetchComplete(aError, aQuality);
        iTnHandle = KErrNone;
        iTnRequestInProgress = EFalse;
        }
    }
#endif

// ---------------------------------------------------------------------------
// CGlxDataSourceMde
// HarvestingUpdated
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::HarvestingUpdated( 
            HarvesterEventObserverType /*aHEObserverType*/, 
            HarvesterEventState aHarvesterEventState,
            TInt /*aItemsLeft*/)
    {
    TRACER("void CGlxDataSourceMde::HarvestingUpdated()");
    GLX_LOG_INFO1("CGlxDataSourceMde::HarvestingUpdated() aHarvesterEventState=%d",
            aHarvesterEventState);
    
    switch(aHarvesterEventState)
        {
        case EHEStateStarted:
            GLX_LOG_INFO("CGlxDataSourceMde::HarvestingUpdated() - EHEStateStarted");
        case EHEStateResumed:
        case EHEStateHarvesting:
        	 {
             iHarvestingOngoing = ETrue;
	         }
             break;
        case EHEStatePaused:
        case EHEStateFinished:
        	 {
	         iHarvestingOngoing = EFalse;
	         GLX_LOG_INFO("CGlxDataSourceMde::HarvestingUpdated() - EHEStateFinished");
        	 }
             break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// ShutdownNotification
// ---------------------------------------------------------------------------
//  
void CGlxDataSourceMde::ShutdownNotification(TInt aShutdownState)
    {
    TRACER("void CGlxDataSourceMde::ShutdownNotification(TInt aShutdownState)")
    GLX_DEBUG2("CGlxDataSourceMde::ShutdownNotification(aShutdownState=%d)", 
            aShutdownState);

    if (!iDataSourceReady && 0 == aShutdownState)
        {
        GLX_DEBUG1("Photos MdS ShutdownNotification - MdS Server restarted!");
        iNoOfRetry = 0;
        CreateSession();
        }

    if (iDataSourceReady && 1 == aShutdownState)
        {
        GLX_DEBUG1("Photos MdS ShutdownNotification - MdS Server Shutdown!");
        HandleSessionError(*iSession, KErrServerTerminated);
        }
    }

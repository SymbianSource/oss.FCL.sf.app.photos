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



// INCLUDE FILES
#include "glxdatasourcemds.h"

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

#ifndef USE_S60_TNM
const TInt KGlxThumbnailCleanupAfterDeletions = 200;

_LIT(KGlxMdeDataSourceThumbnailDatabase, "glxmdstn");
#endif

const TInt KMaxGridThumbnailWidth = 200;

_LIT(KObjectDefLocation, "Location");
_LIT(KObjectDefNameAlbum, "Album");
_LIT(KObjectDefNameImage, "Image");
_LIT(KObjectDefNameMedia, "MediaObject");
_LIT(KObjectDefNameObject, "Object");
_LIT(KObjectDefNameTag, "Tag");
_LIT(KObjectDefNameVideo, "Video");
_LIT(KPropertyDefNameCreationDate, "CreationDate");
_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameSize, "Size");
_LIT(KPropertyDefNameTitle, "Title");
_LIT(KRelationDefNameContains, "Contains");
_LIT(KRelationDefNameContainsLocation, "ContainsLocation");

_LIT(KObjectDefNameMonth, "MediaObject");/// @todo nasty hack remove and use base object

_LIT(KGlxMdeCameraAlbumUri, "defaultalbum_captured");
_LIT(KGlxMdeFavoritesUri, "defaultalbum_favourites");

#undef __USING_INTELLIGENT_UPDATE_FILTERING

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
    RFbsSession::Disconnect();
    iMonthArray.Close();
    iMonthList.Close();
    iUpdateData.Close();
    delete iUpdateCallback;
    delete iCreateSessionCallback;
	}

// ---------------------------------------------------------------------------
// CGlxDataSourceMde()
// ---------------------------------------------------------------------------
//
CGlxDataSourceMde::CGlxDataSourceMde()
	{
    TRACER("CGlxDataSourceMde::CGlxDataSourceMde()")
    //No Implementation
	}

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::ConstructL()
	{
    TRACER("CGlxDataSourceMde::ConstructL()")
    
	iDataSourceReady = EFalse;			
    User::LeaveIfError(iFs.Connect());
	iSession = CMdESession::NewL( *this );
            
    User::LeaveIfError(RFbsSession::Connect());

#ifdef USE_S60_TNM
    iTnEngine = CThumbnailManager::NewL( *this);
    iTnEngine->SetFlagsL(CThumbnailManager::EAllowAnySize);
    iTnEngine->SetDisplayModeL( EColor64K );
    iTnRequestInProgress = EFalse;
#else
	iThumbnailCreator = CGlxtnThumbnailCreator::InstanceL();
	iThumbnailDatabase = CGlxtnThumbnailDatabase::NewL(
            	                        KGlxMdeDataSourceThumbnailDatabase, this);
#endif
         	                        
    iCreateSessionCallback = new ( ELeave )
	    CAsyncCallBack( TCallBack( CreateSession, this ), CActive::EPriorityHigh );
    iUpdateCallback = new ( ELeave )
	    CAsyncCallBack( TCallBack( ProcessItemUpdate, this ), CActive::EPriorityLow );
    iUpdateData.Reserve(100); // ignore if it fails
	}
	
// ----------------------------------------------------------------------------
// from MMdESessionObserver
// CMPXCollectionMdEPlugin::HandleSessionOpened
// ----------------------------------------------------------------------------
//    
void CGlxDataSourceMde::HandleSessionOpened( CMdESession& aSession, TInt aError )    
    {
    TRACER("CGlxDataSourceMde::HandleSessionOpened(CMdESession& aSession, TInt aError)")
    if( KErrNone != aError )
        {
        HandleSessionError(aSession, aError);
        }
    TRAPD(err, DoSessionInitL());
    if( KErrNone != err )
        {
        HandleSessionError(aSession, err);
        }
    
    iSessionOpen = ETrue;
    iDataSourceReady = ETrue;
	TryStartTask(ETrue);
    }
    
// ----------------------------------------------------------------------------
// from MMdESessionObserver
// CMPXCollectionMdEPlugin::HandleSessionError
// ----------------------------------------------------------------------------
//     
void CGlxDataSourceMde::HandleSessionError(CMdESession& /*aSession*/, TInt /*aError*/ )    
    {
    TRACER("CGlxDataSourceMde::HandleSessionError(CMdESession& /*aSession*/, TInt /*aError*/)")
    iSession = NULL;
    iDataSourceReady = EFalse;
    iSessionOpen = EFalse;
    iCreateSessionCallback->CallBack();
    }


// ---------------------------------------------------------------------------
// CreateTaskL
// ---------------------------------------------------------------------------
//
CGlxDataSourceTask* CGlxDataSourceMde::CreateTaskL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver)
	{
    TRACER("CGlxDataSourceTask* CGlxDataSourceMde::CreateTaskL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver)")	
	if(dynamic_cast<CGlxCommandRequest*>(aRequest))
		{
        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeCommand* task = new (ELeave) CGlxDataSourceTaskMdeCommand(static_cast<CGlxCommandRequest*>(aRequest), aObserver, this);
		CleanupStack::Pop(aRequest); // now owned by task
        CleanupStack::PushL(task);
        task->ConstructL();
		CleanupStack::Pop(task);
		return task;
		}
	else if (dynamic_cast< CGlxGetRequest *>(aRequest))
		{
	    _LIT( KFormatTimeStamp, "[%H:%T:%S.%*C5]");
	    RDebug::Print(_L("==> CGlxDataSourceMde::CreateTaskL - CGlxDataSourceTaskMdeAttributeMde+"));
	    TTime time;
	    time.HomeTime(); // Get home time
	    TBuf<32> timeStampBuf;
	    time.FormatL(timeStampBuf, KFormatTimeStamp);
	    RDebug::Print(_L("%S"), &timeStampBuf);    
		
        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeAttributeMde* task = new (ELeave) CGlxDataSourceTaskMdeAttributeMde(static_cast<CGlxGetRequest*>(aRequest), aObserver, this);
		CleanupStack::Pop(aRequest); // now owned by task
        CleanupStack::PushL(task);
        task->ConstructL();
		CleanupStack::Pop(task);
		return task;
		}
	else if (dynamic_cast< CGlxIdListRequest *>(aRequest))
		{	
        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeIdList* task = new (ELeave) CGlxDataSourceTaskMdeIdList(static_cast<CGlxIdListRequest*>(aRequest), aObserver, this);
        CleanupStack::Pop(aRequest); // now owned by task
        CleanupStack::PushL(task); 
        task->ConstructL();
        CleanupStack::Pop(task);
        return task;
		}
	else if (dynamic_cast< CGlxThumbnailRequest *>(aRequest))
		{	
	    _LIT( KFormatTimeStamp, "[%H:%T:%S.%*C5]");
	    RDebug::Print(_L("==> CGlxDataSourceMde::CreateTaskL - CGlxDataSourceTaskMdeThumbnail+"));
	    TTime time;
	    time.HomeTime(); // Get home time
	    TBuf<32> timeStampBuf;
	    time.FormatL(timeStampBuf, KFormatTimeStamp);
	    RDebug::Print(_L("%S"), &timeStampBuf);    

        CleanupStack::PushL(aRequest);
        CGlxDataSourceTaskMdeThumbnail* task = new (ELeave) CGlxDataSourceTaskMdeThumbnail(static_cast<CGlxThumbnailRequest*>(aRequest), aObserver, this);
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
void CGlxDataSourceMde::ThumbnailAvailable(const TGlxMediaId& /*aId*/, const TSize& /*aSize*/)
	{
    TRACER("CGlxDataSourceMde::ThumbnailAvailable(const TGlxMediaId& /*aId*/, const TSize& /*aSize*/)")
	//No implementation
	}

// ---------------------------------------------------------------------------
// BackgroundThumbnailError
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::BackgroundThumbnailError(const TGlxMediaId& aId, TInt aError)
	{
    TRACER("CGlxDataSourceMde::BackgroundThumbnailError(const TGlxMediaId& aId, TInt aError)")
	TSize size(0, 0);
	TRAP_IGNORE(BackgroundThumbnailMessageL(aId, size, aError));
	}
#endif

// ---------------------------------------------------------------------------
// BackgroundThumbnailMessageL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::BackgroundThumbnailMessageL(const TGlxMediaId& aId, const TSize& aSize, TInt aError)
	{
    TRACER("CGlxDataSourceMde::BackgroundThumbnailMessageL(const TGlxMediaId& aId, const TSize& aSize, TInt aError)")
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
    TRACER("CGlxDataSourceMde::DoSessionInitL()")
	/// @todo check schema version number
    iNameSpaceDef = &iSession->GetDefaultNamespaceDefL();
    
	CMdEObject* cameraAlbum = iSession->GetObjectL(KGlxMdeCameraAlbumUri);
	if ( !cameraAlbum )
		{
		User::Leave(KErrCorrupt);
		}
	iCameraAlbumId = (TGlxMediaId)cameraAlbum->Id();
	delete cameraAlbum;

    CMdEObject* favorites = iSession->GetObjectL(KGlxMdeFavoritesUri);
	if ( !favorites )
		{
		User::Leave(KErrCorrupt);
		}
	iFavoritesId = (TGlxMediaId)favorites->Id();
	delete favorites;
    
	
    iContainsDef = &iNameSpaceDef->GetRelationDefL(KRelationDefNameContains);
    iContainsLocationDef = &iNameSpaceDef->GetRelationDefL(KRelationDefNameContainsLocation);
    
    iObjectDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameObject);
    iImageDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameImage);
    iVideoDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameVideo);
    iMediaDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameMedia);
    iAlbumDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameAlbum);
    iTagDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameTag);
    iMonthDef = &iNameSpaceDef->GetObjectDefL(KObjectDefNameMonth);
    iLocationDef = &iNameSpaceDef->GetObjectDefL(KObjectDefLocation);
	
	AddMdEObserversL();
	
	PrepareMonthsL();
	}

// ---------------------------------------------------------------------------
// AddMdEObserversL
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::AddMdEObserversL()
    {
    TRACER("CGlxDataSourceMde::AddMdEObserversL()")
	iSession->AddRelationObserverL(*this);
	iSession->AddRelationPresentObserverL(*this);
	
	iSession->AddObjectObserverL(*this);
	iSession->AddObjectPresentObserverL(*this);
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
    TRACER("CGlxDataSourceMde::HandleObjectNotification()")
	ProcessUpdateArray(aObjectIdArray,  MPXChangeEventType(aType), ETrue);
#ifndef USE_S60_TNM
	if(MPXChangeEventType(aType) == EMPXItemDeleted )
		{			
		TInt count = aObjectIdArray.Count();
		iDeletedCount += count;
		if(iDeletedCount > KGlxThumbnailCleanupAfterDeletions)
		    {
	    	TRAPD(err, ThumbnailCreator().CleanupThumbnailsL(iThumbnailDatabase));
	    	if(!err)
	    	    {
	    	    iDeletedCount = 0;
	    	    }
		    }
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
    TRACER("CGlxDataSourceMde::HandleObjectPresentNotification()")
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
    TRACER("CGlxDataSourceMde::HandleRelationNotification()")
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
    TRACER("CGlxDataSourceMde::HandleRelationPresentNotification()")
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
void CGlxDataSourceMde::ProcessUpdateArray(const RArray<TItemId>& aArray, TMPXChangeEventType aType, TBool aIsObject)
	{
    TRACER("CGlxDataSourceMde::ProcessUpdateArray(const RArray<TItemId>& aArray, TMPXChangeEventType aType, TBool aIsObject)")
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
// MPXChangeEventType
// ---------------------------------------------------------------------------
//
TInt CGlxDataSourceMde::CreateSession(TAny* aPtr)
    {
    TRACER("CGlxDataSourceMde::CreateSession(TAny* aPtr)")
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
    TRACER("CGlxDataSourceMde::CreateSessionL()")
	iSession = CMdESession::NewL( *this );
    }
            

// ---------------------------------------------------------------------------
// ProcessItemUpdate
// ---------------------------------------------------------------------------
//
TInt CGlxDataSourceMde::ProcessItemUpdate(TAny* aPtr)
    {
    TRACER("CGlxDataSourceMde::ProcessItemUpdate(TAny* aPtr)")
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
    TRACER("CGlxDataSourceMde::ProcessItemUpdateL()")
    //__ASSERT_DEBUG(iUpdateData.Count(), Panic(EGlxPanicIllegalState));
	if ( !iUpdateData.Count() || iPauseUpdate )
        {
        return;
        }
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL(message);
    message->SetTObjectValueL<TInt>(KMPXMessageGeneralId, KMPXMessageIdItemChanged);
    message->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, iUpdateData[0].iType);
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
   		    	__ASSERT_DEBUG(( EContainerTypeAlbum != rightContainer), Panic(EGlxPanicIllegalState));
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
    		message->SetTObjectValueL<TMPXGeneralCategory>(KGlxCollectionMessageContainerCategory, containerCategory);
    		message->SetTObjectValueL<TMPXItemId>(KGlxCollectionMessageContainerId, containerId);
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
	message->SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory, category);
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
    TRACER("CGlxDataSourceMde::ContainerType(CMdEObject* aObject)")
	TContainerType containerType = EContainerTypeNotAContainer;
 	
	if( 0 == aObject->Def().Compare(*iAlbumDef) )
		{
		containerType = EContainerTypeAlbum;
		}
	else if( 0 == aObject->Def().Compare(*iTagDef) )
		{
		containerType = EContainerTypeTag;
		}
	else if( 0 == aObject->Def().Compare(*iMonthDef) )
	    {
		containerType = EContainerTypeMonth;
	    }

	return containerType;
	}


// ---------------------------------------------------------------------------
// ContainerType
// ---------------------------------------------------------------------------
//	
CGlxDataSource::TContainerType CGlxDataSourceMde::ContainerType(CMdEObjectDef* aObjectDef)
	{
    TRACER("CGlxDataSourceMde::ContainerType(CMdEObjectDef* aObjectDef)")
	TContainerType containerType = EContainerTypeNotAContainer;
 	
	if( 0 == aObjectDef->Compare(*iAlbumDef) )
		{
		containerType = EContainerTypeAlbum;
		}
	else if( 0 == aObjectDef->Compare(*iTagDef) )
		{
		containerType = EContainerTypeTag;
		}
	else if( 0 == aObjectDef->Compare(*iMonthDef) )
	    {
		containerType = EContainerTypeMonth;
	    }

	return containerType;
	}
	
// ---------------------------------------------------------------------------
// ItemType()
// ---------------------------------------------------------------------------
//
CGlxDataSource::TItemType CGlxDataSourceMde::ItemType(CMdEObject* aObject)
	{
    TRACER("CGlxDataSourceMde::ItemType(CMdEObject* aObject)")
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
// PrepareMonthsL()
// ---------------------------------------------------------------------------
//
void CGlxDataSourceMde::PrepareMonthsL()
    {
    TRACER("CGlxDataSourceMde::PrepareMonthsL()")
    TTime month(0);
    iFirstMonth = month;
    }
    
// ---------------------------------------------------------------------------
// GetMonthIdL()
// ---------------------------------------------------------------------------
//
const TGlxMediaId CGlxDataSourceMde::GetMonthIdL(const TTime& aMonth)
    {
    TRACER("CGlxDataSourceMde::GetMonthIdL(const TTime& aMonth)")
    TTime monthStart = iFirstMonth + aMonth.MonthsFrom(iFirstMonth);
    const TTimeIntervalMonths KGlxOneMonth = 1;
    const TTimeIntervalMicroSeconds KGlxOneMicrosecond = 1;

    TGlxMediaId monthId;    
    TInt monthIndex = iMonthArray.Find(monthStart);
    if( monthIndex != KErrNotFound )
        {
        monthId = iMonthList[monthIndex];
        }
    else
        {
        _LIT(KGlxMonthTitleFormat, "%F%Y%M%D:");
        const TInt KGlxMonthTitleLength = 12;
        TBuf<KGlxMonthTitleLength> title;
        monthStart.FormatL(title, KGlxMonthTitleFormat);
        
        CMdEObject* month = iSession->GetObjectL(title);
        if( month )
            {
            monthId = (TGlxMediaId)month->Id();
            iMonthArray.AppendL(monthStart);
            iMonthList.AppendL(monthId);
            delete month;
            }
        else
            {
            TTime monthEnd = monthStart + KGlxOneMonth - KGlxOneMicrosecond;
            month = iSession->NewObjectLC(*iMonthDef, title); 
            
            // A title property def of type text is required.
            CMdEPropertyDef& titlePropertyDef = iObjectDef->GetPropertyDefL(KPropertyDefNameTitle);
            if (titlePropertyDef.PropertyType() != EPropertyText)
            	{
            	User::Leave(KErrCorrupt);
            	}
            // Set the object title.
            month->AddTextPropertyL (titlePropertyDef, title);

            // A size property is required.
            CMdEPropertyDef& sizePropertyDef = iObjectDef->GetPropertyDefL(KPropertyDefNameSize);
            if (sizePropertyDef.PropertyType() != EPropertyUint32)
            	{
            	User::Leave(KErrCorrupt);
            	}
            month->AddUint32PropertyL(sizePropertyDef,0);

            
            // A creation date property is required.
        	CMdEPropertyDef& creationDateDef = iObjectDef->GetPropertyDefL(KPropertyDefNameCreationDate);
            if (creationDateDef.PropertyType() != EPropertyTime)
            	{
            	User::Leave(KErrCorrupt);
            	}
        	month->AddTimePropertyL(creationDateDef, monthStart);

            // A last modified date property is required.
        	CMdEPropertyDef& lmDateDef = iObjectDef->GetPropertyDefL(KPropertyDefNameLastModifiedDate);
            if (lmDateDef.PropertyType() != EPropertyTime)
            	{
            	User::Leave(KErrCorrupt);
            	}
            
        	month->AddTimePropertyL(lmDateDef, monthEnd);
        	
            monthId = (TGlxMediaId)iSession->AddObjectL(*month);
            CleanupStack::PopAndDestroy(month);
            iMonthArray.AppendL(monthStart);
            iMonthList.AppendL(monthId);
            }
        }
    return monthId;
    }
    
// ---------------------------------------------------------------------------
// SameMonth
// ---------------------------------------------------------------------------
//
TBool CGlxDataSourceMde::SameMonth(const TTime& aOldDate, const TTime& aNewDate)
    {
    TRACER("CGlxDataSourceMde::SameMonth(const TTime& aOldDate, const TTime& aNewDate)")
    return ( aOldDate.MonthsFrom(iFirstMonth) == aNewDate.MonthsFrom(iFirstMonth) );
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
void CGlxDataSourceMde::FetchThumbnailL(CGlxRequest* aRequest, MThumbnailFetchRequestObserver& aObserver)
	{
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
	    GLX_LOG_INFO("CGlxDataSourceMde::FetchThumbnailL() - Fetch TN attrib - EGridThumbnailSize");
	}
    else
	{
	     iTnEngine->SetFlagsL(CThumbnailManager::EDefaultFlags);
	     iTnEngine->SetThumbnailSizeL(EFullScreenThumbnailSize);
	     GLX_LOG_INFO("CGlxDataSourceMde::FetchThumbnailL() - Fetch TN attrib - EFullScreenThumbnailSize");
	}
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(request->ThumbnailInfo()->FilePath(), 0);
	iStartTime.UniversalTime();
    iTnThumbnailCbId = iTnEngine->GetThumbnailL(*source);
    iTnRequestInProgress = ETrue;
    CleanupStack::PopAndDestroy();
	}

TInt CGlxDataSourceMde::CancelFetchThumbnail()
	{
	TInt ret = KErrNone;
	if (iTnRequestInProgress)
		{
		ret = iTnEngine->CancelRequest(iTnThumbnailCbId);
		}
	return ret;
	}

void CGlxDataSourceMde::ThumbnailPreviewReady(MThumbnailData& /*aThumbnail*/, 
                                                    TThumbnailRequestId /*aId*/)
    {
    }

// Called when real thumbnail is created
void CGlxDataSourceMde::ThumbnailReady(TInt aError,
        MThumbnailData& aThumbnail, TThumbnailRequestId aId)
	{
	if (iTnThumbnailCbId == aId)
		{
		iTnRequestInProgress = EFalse;
		iStopTime.UniversalTime();
		RDebug::Print(_L("==> S60 TNMan fetch took <%d> us"), (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
	    
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

		if (iTnFetchObserver)
			{
			iTnFetchObserver->ThumbnailFetchComplete(aError);
			iTnHandle = KErrNone;
			}
		}
	}
#endif

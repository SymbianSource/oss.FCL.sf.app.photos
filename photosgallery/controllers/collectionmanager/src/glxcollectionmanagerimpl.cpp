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
* Description:    Implementation of collection manager API for external use
*
*/




#include "glxcollectionmanagerimpl.h"

#include <glxcommandfactory.h>
#include <glxpanic.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>

#include "glxcollectioninfo.h"

#include <glxcollectionplugincamera.hrh>
#include <glxcollectionpluginalbums.hrh>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxCollectionManagerImpl::CGlxCollectionManagerImpl()
    {
    }
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxCollectionManagerImpl::~CGlxCollectionManagerImpl()
    {
    CloseCollection();
    delete iUri;
    }
// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::AddToContainerL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::AddToContainerL(const TDesC& aUri, 
		CGlxCollectionManager::TCollection aContainer, TRequestStatus& aRequestStatus)
    {
    __ASSERT_DEBUG(CGlxCollectionManager::ECollectionCameraAlbum == aContainer, Panic(EGlxPanicUnsupportedCollection));
    
    iCollectionId.iUid = KGlxCollectionPluginCameraImplementationUid;
    iContainerId = 0; // A zero container id indicates to the data source that we wish to the 
    // 'special album' related to the collection id. e.g. the camera album.
    
    AddToContainerL(aUri,aRequestStatus);
    }
    
// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::AddToContainerL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::AddToContainerL(const TDesC& aUri, 
                        TUint32 aCollectionId, TRequestStatus& aRequestStatus)
    {
    iCollectionId.iUid = KGlxCollectionPluginAlbumsImplementationUid;
    iContainerId = aCollectionId;
    
    AddToContainerL(aUri,aRequestStatus);
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::CollectionInfoL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::CollectionInfoL(CGlxCollectionInfo& aInfo,
                        TRequestStatus& aRequestStatus)
    {
    __ASSERT_DEBUG(iState == EClosed || iState == EOpen, Panic(EGlxPanicIllegalState));
    
    iCollectionInfo = &aInfo;
    InitializeRequest(aRequestStatus);
    
    if (iState == EOpen)
    	{
    	CloseCollection(); // Close and re-open the collection to ensure we have the
    	                   // latest changes.
    	}

    iState = EOpeningForCollectionInfo;
    OpenCollectionL();
    
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::Cancel
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::Cancel()
    {	
    ///@todo test canceling
    
    switch (iState)
    	{
    	case EClosed:
    	case EOpen:
    	case EOpening:
    		{
    		// no request outstanding
    		break;
    		}
    	case EOpeningForAdd:
    	case EOpeningForCollectionInfo:
    		{
    		iState = EOpening; // Complete open if we can.
    		break;		
    		}
    	case EAdding:
    	case ERequestingMedia:
    		{
    		iState = EOpen;
    		iCollectionUtility->Collection().CancelRequest();
    		}
    	}
    CompleteRequest(KErrCancel);
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::HandleCollectionMessageL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::HandleCollectionMessageL(const TMPXCollectionMessage& /*aMessage*/)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::HandleCollectionMessageL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::HandleCollectionMessageL(const CMPXMessage& aMsg)
    {
    if (aMsg.IsSupported(KMPXMessageGeneralEvent))
    	{
	    switch (aMsg.ValueTObjectL<TInt>(KMPXMessageGeneralEvent))
	    	{
	    	case TMPXCollectionMessage::EPathChanged:
	    		{
	            __ASSERT_DEBUG(iState == EOpeningForAdd || iState == EOpeningForCollectionInfo || iState == EOpening || iState == EOpen, Panic(EGlxPanicIllegalState));
	    		
	            TState oldState = iState;
	            iState = EOpen;
	            
	            switch(oldState)
	            {
	            case EOpeningForAdd:
	            	{
	            	iState = EOpen;
	            	DoAddToContainerL();
	            	break;
	            	}
	            case EOpeningForCollectionInfo:
	            	{
	        		iState = EOpen;
	            	__ASSERT_DEBUG(iCollectionInfo, Panic(EGlxPanicNullPointer));
		 
	        		DoCollectionInfoL();
	        		break;
	            	}
	            case EOpening:
	            	{
	            	// don't do anything
	            	break;
	            	}
		    		}
	    		}
	    	break;
	    	default:
	    	break;
	    	}
    	}
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::HandleOpenL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::HandleOpenL(const CMPXMedia& /*aEntries*/,
                         TInt /*aIndex*/,TBool /*aComplete*/,TInt aError)
    { 
    // Handle open only gets called when an error occurs
    ///@todo check that this method works
    
    __ASSERT_DEBUG(iState == EOpening || iState == EOpeningForAdd || iState == EOpeningForCollectionInfo, Panic(EGlxPanicIllegalState));
    	
    if (aError != KErrNone)
    	{
    	CompleteRequest(aError);
    	iState = EClosed;
    	}  	
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::HandleOpenL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::HandleOpenL(const CMPXCollectionPlaylist& /*aPlaylist*/ ,TInt /*aError*/)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::HandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::HandleCollectionMediaL(const CMPXMedia& aMedia, TInt aError)
    {
    iState = EOpen;    	
    
    if (aError == KErrNone && iCollectionInfo)
    	{
        iCollectionInfo->SetExists(ETrue);
        iCollectionInfo->SetTitleL(aMedia.ValueText(KMPXMediaGeneralTitle));
    	}
  
    CompleteRequest(aError);
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::HandleCommandComplete
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::HandleCommandComplete(CMPXCommand* /*aCommandResult*/, TInt aError)
    {
    CompleteRequest(aError);
    iState = EOpen;
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::CompleteRequest
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::CompleteRequest(TInt aError)
    {
    if (iRequestStatus)
        {
        User::RequestComplete(iRequestStatus, aError);
        iRequestStatus = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::InitializeRequest
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::InitializeRequest(TRequestStatus& aRequestStatus)
    {
    __ASSERT_DEBUG(!iRequestStatus, Panic(EGlxPanicCollectionManagerOutstandingRequest));
    iRequestStatus = &aRequestStatus;
    *iRequestStatus = KRequestPending;
    }

/// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::OpenCollectionL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::OpenCollectionL()
    {
    if (iCollectionUtility)
    	{
    	iCollectionUtility->Close(); // deletes the collection utility's this pointer. 
    	}
    iCollectionUtility = NULL;
    iCollectionUtility = MMPXCollectionUtility::NewL(this);
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
	path->AppendL(KGlxCollectionPluginAlbumsImplementationUid); //Open the albums collection plugin
	// We can use this plugin to add things to the camera album.
	iCollectionUtility->Collection().OpenL(*path);
	CleanupStack::PopAndDestroy(path);
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::CloseCollection
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::CloseCollection()
	{
	if (iCollectionUtility)
		{
		iCollectionUtility->Close();
		iCollectionUtility = NULL;
		}
	iState = EClosed;
	}

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::DoAddToContainerL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::DoAddToContainerL()
	{
	__ASSERT_DEBUG(iState == EOpen, Panic(EGlxPanicIllegalState));
	CMPXCollectionPath* targetContainerPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL(targetContainerPath);
    targetContainerPath->AppendL(iCollectionId.iUid);

    targetContainerPath->AppendL(iContainerId);
    CMPXCommand* command = TGlxCommandFactory::AddToContainerCommandLC(*iUri, *targetContainerPath);
    CleanupStack::Pop(command);
    CleanupStack::PopAndDestroy(targetContainerPath);
    CleanupStack::PushL(command);
    iCollectionUtility->Collection().CommandL(*command);
    CleanupStack::PopAndDestroy(command);
    iState = EAdding;
	}

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::AddToContainerL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::AddToContainerL(const TDesC& aUri, TRequestStatus& aRequestStatus)
	{
    __ASSERT_DEBUG(iState == EClosed || iState == EOpen,  Panic(EGlxPanicIllegalState));
    
    InitializeRequest(aRequestStatus);
    
	delete iUri;
    iUri = NULL;
    iUri = aUri.AllocL();
    
    if (iState == EClosed)
    	{
    	iState = EOpeningForAdd;
    	OpenCollectionL();
    	}
    else
    	{
    	DoAddToContainerL();
    	}
	}

// -----------------------------------------------------------------------------
// CGlxCollectionManagerImpl::DoCollectionInfoL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerImpl::DoCollectionInfoL()
	{
    __ASSERT_DEBUG(iState == EOpen,  Panic(EGlxPanicIllegalState));
	CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
	CleanupStack::PushL(path);
	
	TInt index = path->IndexOfId(iCollectionInfo->Id());
	if (index == KErrNotFound)
		{
		iCollectionInfo->SetExists(EFalse);
		CompleteRequest(KErrNone);
		}
	else if(index < 0) // Index is an error other than KErrNotFound
		{
		CompleteRequest(index);
		}
	else
		{
		// the index is good get the title
		path->Set(index);
		
		RArray<TMPXAttribute> attributeArray;
		CleanupClosePushL(attributeArray);
		attributeArray.AppendL(KMPXMediaGeneralTitle);
		iCollectionUtility->Collection().MediaL(*path, attributeArray.Array());
		CleanupStack::PopAndDestroy(&attributeArray);
		iState = ERequestingMedia;
		}
	CleanupStack::PopAndDestroy(path);
	}


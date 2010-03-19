/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectionpath.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessageprogressdefs.h>

#include <glxfilterfactory.h>
#include <glxcollectionpluginall.hrh>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxmpxcommandhandler.h>
#include <QDebug>

GlxMpxCommandHandler::GlxMpxCommandHandler()
{
    iMediaList = NULL;
}

GlxMpxCommandHandler::~GlxMpxCommandHandler()
{
    //close the existing instance of Media List
    if ( iMediaList ) { 
        iMediaList->Close();
        iMediaList = NULL;
    }
}

void GlxMpxCommandHandler::executeCommand(int commandId,int collectionId)
{
    qDebug("GlxMpxCommandHandler::executeCommand ");
    int aHierarchyId = 0; 
    TGlxFilterItemType aFilterType = EGlxFilterImage;

	//CreateMediaListL(aCollectionId, aHierarchyId,aFilterType);
	if(collectionId != KGlxAlbumsMediaId)
		{
        qDebug("GlxMpxCommandHandler::executeCommand ::CreateMediaListL");
		CreateMediaListL(collectionId, aHierarchyId,aFilterType);
		}
	else
		{
        qDebug("GlxMpxCommandHandler::executeCommand ::CreateMediaListAlbumItemL");
		//for creating Medial List for Albums Media path Items
		CreateMediaListAlbumItemL(collectionId, aHierarchyId,aFilterType);
		}

    TBool consume = ETrue;
	//Execute Command 
	DoExecuteCommandL(commandId,*iMediaList,consume);
	//Create MPX command if any
    CMPXCommand* command = CreateCommandL(commandId,*iMediaList,consume);
    if(command){
        command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, static_cast<TAny*>(this));
        iMediaList->AddMediaListObserverL(this);
        iMediaList->CommandL(*command);
    }
}

// ---------------------------------------------------------------------------
// CreateMediaListL()
// Creates a collection path
// Create a filter as requested filtertype
// Creates the medialist
// ---------------------------------------------------------------------------
void GlxMpxCommandHandler::CreateMediaListL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType) 
{
   	Q_UNUSED(aHierarchyId);
   	Q_UNUSED(aFilterType);	
   	
	//close the existing instance of Media List
	if ( iMediaList ) { 
	    iMediaList->Close();
	    iMediaList = NULL;
	}

	// Create path to the list of images and videos
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	path->AppendL(aCollectionId);
	// Create filter to filter out either images or videos
	CMPXFilter* filter = TGlxFilterFactory::CreateItemTypeFilterL(EGlxFilterImage);   //todo take actual filter type
	CleanupStack::PushL(filter);
	
	// Create the media list
	iMediaList =  MGlxMediaList::InstanceL( *path, 
	                  TGlxHierarchyId(0), filter );  //todo take actual hierarchy 
	CleanupStack::PopAndDestroy( filter );
	
	CleanupStack::PopAndDestroy(path);
}
    
// ---------------------------------------------------------------------------
// CreateMediaListAlbumItemL()
// Creates the media list for the album Item 
// ---------------------------------------------------------------------------
void GlxMpxCommandHandler::CreateMediaListAlbumItemL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType)
	{
	Q_UNUSED(aCollectionId);
	Q_UNUSED(aHierarchyId);
	//retrieve the instance of Media list for ALBUMS LIST which will be saved in iMediaList
	CreateMediaListL(KGlxCollectionPluginAlbumsImplementationUid,0,EGlxFilterAlbum);
	//retrieve the path of the focussed item of the MediaList
	CMPXCollectionPath* path = iMediaList->PathLC(NGlxListDefs:: EPathFocusOrSelection);
	//close the existing instance of Media List
	iMediaList->Close();
	iMediaList = NULL;
	//create new media list with the derived path
	CMPXFilter* filter = TGlxFilterFactory::CreateItemTypeFilterL(aFilterType);   //todo take actual filter type
	CleanupStack::PushL(filter);
	iMediaList = MGlxMediaList::InstanceL( *path, 
	                  TGlxHierarchyId(0), filter );  //todo take actual hierarchy
	CleanupStack::PopAndDestroy( filter );
	
	CleanupStack::PopAndDestroy(path);
	}
// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//  
void GlxMpxCommandHandler::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // Do nothing
    qDebug("New item added ");
    }

// -----------------------------------------------------------------------------
// HandleMediaL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}
	
// -----------------------------------------------------------------------------
// HandleItemRemovedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}
	
// -----------------------------------------------------------------------------
// HandleItemModifiedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}	

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleAttributesAvailableL(TInt /*aItemIndex*/, 	
	const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}
		
// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/) 
	{
	// Do nothing
	}
	
// -----------------------------------------------------------------------------
// HandleItemSelectedL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// HandleMessageL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleMessageL(const CMPXMessage& aMessage, 
        MGlxMediaList* aList)
{
    if ((aMessage.IsSupported(KMPXMessageGeneralId) && (aMessage.IsSupported(KMPXCommandGeneralSessionId))))
    {
        TInt msgId = aMessage.ValueTObjectL<TInt>(KMPXMessageGeneralId);
        TAny* sessionId = aMessage.ValueTObjectL<TAny*>(KMPXCommandGeneralSessionId);

        // Check if this is a progress message and intended for this object
    }
}

// -----------------------------------------------------------------------------
// HandleCommandCompleteL
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::HandleCommandCompleteL(TAny* aSessionId, CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList)
    {

    MGlxMediaList::UnmarkAllL(*iMediaList);
    
    DoHandleCommandCompleteL(aSessionId, aCommandResult, aError, aList);
    
    iMediaList->RemoveMediaListObserver(this);
    }
    
// -----------------------------------------------------------------------------
// Default implementation of advanced command handling does nothing
// -----------------------------------------------------------------------------
//	
void GlxMpxCommandHandler::DoHandleCommandCompleteL(TAny* /*aSessionId*/, 
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/, MGlxMediaList* /*aList*/)
    {
    }
 
// -----------------------------------------------------------------------------
// Default implementation of advanced command handling does nothing
// -----------------------------------------------------------------------------
//

void GlxMpxCommandHandler::DoExecuteCommandL(TInt /*aCommandId*/, MGlxMediaList& /*aMediaList*/, TBool& /*aConsume*/)
{

}

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
* Description:    Add to container command handler
*
*/




/**
 * @internal reviewed 05/06/2007 by Dave Schofield
 */

#include "glxcommandhandleraddtocontainer.h"

#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <eikmenup.h> 
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxcommandfactory.h>
#include <glxcommandhandlers.hrh>
#include <glxfilterfactory.h>
#include <glxuiutility.h>
#include <glxuistd.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxuiutilities.rsg>
#include <mglxmedialist.h>

#include <mpxcollectioncommanddefs.h>
#include <mpxcollectionpath.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <StringLoader.h>

#include "glxmediaselectionpopup.h"
#include <glxicons.mbg>
#include <glxtracer.h>

TInt CGlxCommandHandlerAddToContainer::iSelectionCount = 0;

// ---------------------------------------------------------------------------
// Return add to album command handler
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerAddToContainer*
CGlxCommandHandlerAddToContainer::NewAddToAlbumCommandHandlerL(
		MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem,
		const TDesC& aFileName)
	{
	TRACER("CGlxCommandHandlerAddToContainer::NewAddToAlbumCommandHandlerL");
	return CGlxCommandHandlerAddToContainer::NewL(aMediaListProvider,
			EGlxCmdAddToAlbum, aHasToolbarItem, aFileName);
	}

// ---------------------------------------------------------------------------
// Return add (to) tags command handler
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerAddToContainer*
CGlxCommandHandlerAddToContainer::NewAddToTagCommandHandlerL(
		MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem,
		const TDesC& aFileName)
	{
	TRACER("CGlxCommandHandlerAddToContainer::NewAddToTagCommandHandlerL");
	return CGlxCommandHandlerAddToContainer::NewL(aMediaListProvider,
			EGlxCmdAddTag, aHasToolbarItem, aFileName);
	}

// ---------------------------------------------------------------------------
// Return add (to) Favourites command handler
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerAddToContainer*
CGlxCommandHandlerAddToContainer::NewAddToFavCommandHandlerL(
		MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem,
		const TDesC& aFileName)
	{
	TRACER("CGlxCommandHandlerAddToContainer::NewAddToFavCommandHandlerL");
	return CGlxCommandHandlerAddToContainer::NewL(aMediaListProvider,
			EGlxCmdAddToFavourites, aHasToolbarItem, aFileName);
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerAddToContainer* CGlxCommandHandlerAddToContainer::NewL(
		MGlxMediaListProvider* aMediaListProvider, TInt aCommandId,
		TBool aHasToolbarItem, const TDesC& aFileName)
	{
	TRACER("CGlxCommandHandlerAddToContainer::NewL");
	CGlxCommandHandlerAddToContainer* self =
			new (ELeave) CGlxCommandHandlerAddToContainer(aMediaListProvider,
					aHasToolbarItem);
	CleanupStack::PushL(self);
	self->ConstructL(aCommandId, aFileName);
	CleanupStack::Pop(self);
	return self;
	}
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerAddToContainer::CGlxCommandHandlerAddToContainer(
    MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem)
        : CGlxMpxCommandCommandHandler( aMediaListProvider, aHasToolbarItem )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerAddToContainer::ConstructL(TInt aCommandId,
		const TDesC& aFileName)
	{
	TRACER("CGlxCommandHandlerAddToContainer::ConstructL");
	// Load resource
	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(aFileName);
	// Reset the Flag inorder to allow for a new selection pop-up to be created.
	iIsProcessOngoing = EFalse;
	// Add supported command
	TCommandInfo info(aCommandId);

	// Filter out static items - user must have selected at least one item to
	// enable the command handler    
	const TInt KGlxCommandHandlerMinSelectionAllowSingle = 1;
	info.iMinSelectionLength = KGlxCommandHandlerMinSelectionAllowSingle;

	AddCommandL(info);
	iCommandId = aCommandId;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerAddToContainer::~CGlxCommandHandlerAddToContainer()
    {
    TRACER("CGlxCommandHandlerAddToContainer::~CGlxCommandHandlerAddToContainer");
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
	// Reset the Flag inorder to allow for a new selection pop-up to be created.
    iIsProcessOngoing = EFalse;
    }

// ---------------------------------------------------------------------------
// Create an add to container command
// ---------------------------------------------------------------------------
//
CMPXCommand* CGlxCommandHandlerAddToContainer::CreateCommandL(TInt aCommandId, 
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    TRACER("CGlxCommandHandlerAddToContainer::CreateCommandL");
	// Ignore the current command in case the previous command has still not been completed
	if(iIsProcessOngoing)
	    {
	    return NULL;
	    }
    iSelectionCount = 0;
	// Set the Flag inorder to block any new subsequent selection pop-ups from getting created.
	iIsProcessOngoing = ETrue;
    CMPXCollectionPath* targetCollection = CMPXCollectionPath::NewL();
    CleanupStack::PushL(targetCollection);
    TBool enableMultipleSelection = EFalse;
    TBool enablePopup = EFalse;
    CMPXFilter* filter = NULL;
    switch(aCommandId)
        {
        case EGlxCmdAddToFavourites:
            {
            targetCollection->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
            // The target collection has also to be appeneded with the the relation id.
            // appending another level into the albums to get favourites and 1 is the relation id of albums
            targetCollection->AppendL( TMPXItemId(1) );
            targetCollection->Set( 0 );
            break;
            }
        case EGlxCmdAddToAlbum:
            {
            enablePopup = ETrue;
            targetCollection->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
	        filter = TGlxFilterFactory::CreateCameraAlbumExclusionFilterL();
	        CleanupStack::PushL(filter);
            break;
            }
        case EGlxCmdAddTag:
            {
            enableMultipleSelection = ETrue;
            enablePopup = ETrue;
            targetCollection->AppendL(KGlxTagCollectionPluginImplementationUid);
            break;
            }
        }
               
    CMPXCollectionPath* sourceItems = aMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
    CMPXCommand* command = NULL;
    if (enablePopup )
        {
        CGlxMediaSelectionPopup* popup = new (ELeave) CGlxMediaSelectionPopup;
        
        TBool accepted = EFalse;
        CMPXCollectionPath* targetContainers = 
            popup->ExecuteLD(*targetCollection, accepted, enableMultipleSelection, ETrue, filter);

        if (accepted)
            {        
            // Set the container selection count to give correct completion text
            TArray<TInt> selection = targetContainers->Selection();
            iSelectionCount = selection.Count();
            
            CleanupStack::PushL(targetContainers);
            command = TGlxCommandFactory::AddToContainerCommandLC(
                *sourceItems, *targetContainers);
            CleanupStack::Pop(command); 
            CleanupStack::PopAndDestroy(targetContainers);
            }
        else
	    	{
    		// Reset the Flag inorder to allow any new selection pop-up to be created.
	    	iIsProcessOngoing = EFalse;
	    	}
        }
    else
        {
        command = TGlxCommandFactory::AddToContainerCommandLC(
            *sourceItems, *targetCollection);
        CleanupStack::Pop(command); 
        }
    CleanupStack::PopAndDestroy(sourceItems);
    if (filter)
        {
        CleanupStack::PopAndDestroy(filter);
        }
    CleanupStack::PopAndDestroy(targetCollection);
	return command;
    } 

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAddToContainer::CompletionTextL
// -----------------------------------------------------------------------------
//	
HBufC* CGlxCommandHandlerAddToContainer::CompletionTextL() const
    {
    TRACER("CGlxCommandHandlerAddToContainer::CompletionTextL()");
    if (iCommandId == EGlxCmdAddToAlbum)
    	{
   		if (iSelectionCount > 1)
   			{
	   		return StringLoader::LoadL(R_GLX_COMPLETION_ADD_TO_CONTAINER_MULTI_ITEM_ALBUM);
   			}
   		return StringLoader::LoadL(R_GLX_COMPLETION_ADD_TO_CONTAINER_ONE_ITEM_ALBUM);
    	}
   	else if (iCommandId == EGlxCmdAddTag)
   		{
   		if (iSelectionCount > 1)
   			{
	   		return StringLoader::LoadL(R_GLX_COMPLETION_ADD_TO_CONTAINER_MULTI_ITEM_TAG);
   			}
		return StringLoader::LoadL(R_GLX_COMPLETION_ADD_TO_CONTAINER_ONE_ITEM_TAG);	
   		}
    else // considered as favourites 
        {
        return StringLoader::LoadL(R_GLX_COMPLETION_ADD_TO_FAVOURITES);
        }
    }


TBool CGlxCommandHandlerAddToContainer::OkToExit() const
        {
        return ETrue;
        }
// -----------------------------------------------------------------------------
// CGlxCommandHandlerAddToContainer::DoHandleCommandCompleteL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAddToContainer::DoHandleCommandCompleteL(TAny* /*aSessionId*/,
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/, MGlxMediaList* /*aList*/)
      {
      TRACER("CGlxCommandHandlerAddToContainer::DoHandleCommandCompleteL()");
	  // Reset the Flag inorder to allow any new selection pop-up to be created.
      iIsProcessOngoing = EFalse;
      }



// -----------------------------------------------------------------------------
// DialogDismissedL
// -----------------------------------------------------------------------------
//  
void CGlxCommandHandlerAddToContainer::DialogDismissedL(TInt aButtonId)
    {
    TRACER("CGlxCommandHandlerAddToContainer::DialogDismissedL");
    iIsProcessOngoing = EFalse;
    CGlxMpxCommandCommandHandler::DialogDismissedL(aButtonId);
    }



// ---------------------------------------------------------------------------
// CGlxCommandHandlerAddToContainer::DoActivateL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerAddToContainer::DoActivateL(TInt /*aViewId*/)
	{

 	}

void CGlxCommandHandlerAddToContainer::PopulateToolbarL()
	{

	}

// ---------------------------------------------------------------------------
// CGlxCommandHandlerAddToContainer::DoIsDisabled
// ---------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAddToContainer::DoIsDisabled(TInt aCommandId, 
                                                 MGlxMediaList& aList) const
    {
	TRACER("CGlxCommandHandlerAddToContainer::DoIsDisabled");
	if (EGlxCmdAddToAlbum==aCommandId || EGlxCmdAddTag == aCommandId)
        {
        return EFalse;
        }
    return ETrue;
    }
// End of File

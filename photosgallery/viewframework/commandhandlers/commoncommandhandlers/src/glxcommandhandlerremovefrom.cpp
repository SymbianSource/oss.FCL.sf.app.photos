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
* Description:    Delete command handler
*
*/




#include "glxcommandhandlerremovefrom.h"

#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>

#include <AknUtils.h>
#include <bautils.h>
#include <glxuiutility.h>
#include <glxicons.mbg>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxuistd.h>
#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <mpxcollectionpath.h>
#include <StringLoader.h>

#include "glxcommandfactory.h"

// ---------------------------------------------------------------------------
// Return add (to) tags command handler
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerRemoveFrom* 
    CGlxCommandHandlerRemoveFrom::NewRemFromFavCommandHandlerL(
        MGlxMediaListProvider* aMediaListProvider) 
    {
    return CGlxCommandHandlerRemoveFrom::NewL(aMediaListProvider, 
                                EMPXAlbum, EGlxCmdRemoveFromFavourites);
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerRemoveFrom* CGlxCommandHandlerRemoveFrom::NewL(
        MGlxMediaListProvider* aMediaListProvider, TMPXGeneralCategory aContainerType, 
                TInt aCommandId)
    {
    CGlxCommandHandlerRemoveFrom* self = new (ELeave) CGlxCommandHandlerRemoveFrom(
            aMediaListProvider, aContainerType);
    CleanupStack::PushL(self);
    self->ConstructL(aCommandId);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerRemoveFrom::CGlxCommandHandlerRemoveFrom(MGlxMediaListProvider* 
        aMediaListProvider, TMPXGeneralCategory aContainerType): 
        CGlxMpxCommandCommandHandler(aMediaListProvider), iContainerType(aContainerType)
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRemoveFrom::ConstructL(TInt aCommandId)
    {
    // Load resource file
	TParse parse;
    parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
   	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);

   	// Add supported command
   	TCommandInfo info(EGlxCmdRemoveFrom);
   	// Filter out static items
    info.iMinSelectionLength = 1;
    info.iMaxSelectionLength = KMaxTInt;
   	AddCommandL(info);

    // Add Remove From Favourites Command
    TCommandInfo removeFav(EGlxCmdRemoveFromFavourites);
    removeFav.iMinSelectionLength = 1;
    removeFav.iMaxSelectionLength = 1;
    AddCommandL(removeFav);

    // Store this value to show the correct Text
   	iCommandId = aCommandId;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerRemoveFrom::~CGlxCommandHandlerRemoveFrom()
    {
    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
    }

// ---------------------------------------------------------------------------
// SetContainerId
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxCommandHandlerRemoveFrom::SetContainerId(TGlxMediaId aContainerId)
	{
	iRemoveFromContainerId = aContainerId;
	}

// ---------------------------------------------------------------------------
// Create an add to container command
// ---------------------------------------------------------------------------
//
CMPXCommand* CGlxCommandHandlerRemoveFrom::CreateCommandL(TInt aCommandId, 
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    CMPXCollectionPath* path = aMediaList.PathLC();
    CMPXCommand* command = NULL;
    if (aCommandId == EGlxCmdRemoveFromFavourites)
        {
        // Hardcoding this to the favourites
        TGlxMediaId favId(1);

        command = TGlxCommandFactory::RemoveFromContainerCommandLC(favId, *path);
        }
    else
        {
        if (iRemoveFromContainerId == KGlxIdNone)
            {
            command = TGlxCommandFactory::RemoveFromContainerCommandLC(*path);
            }
        else
            {
            command = TGlxCommandFactory::RemoveFromContainerCommandLC(iRemoveFromContainerId, *path);
            }
        }
    CleanupStack::Pop(command);
    CleanupStack::PopAndDestroy(path);
    return command;
    } 
    
// -----------------------------------------------------------------------------
// CGlxCommandHandlerAddToFavourites::CompletionTextL
// -----------------------------------------------------------------------------
//  
HBufC* CGlxCommandHandlerRemoveFrom::CompletionTextL() const
    {
    HBufC* text = NULL;
    if (iCommandId == EGlxCmdRemoveFromFavourites)
        {
        text = StringLoader::LoadL(R_GLX_COMPLETION_REM_FROM_FAVOURITES);            
        }
    return text;
    }
    

// -----------------------------------------------------------------------------
// Return confirmation string
// -----------------------------------------------------------------------------
//	
HBufC* CGlxCommandHandlerRemoveFrom::ConfirmationTextL(TInt aCommandId, 
        TBool aMultiSelection) const
    {
    HBufC* text = NULL;
    if (EGlxCmdRemoveFromFavourites != aCommandId)
        {
        switch (iContainerType)
            {
            case EMPXAlbum:
    	if (aMultiSelection)
    		{
    		text = StringLoader::LoadL(R_GLX_CONFIRMATION_REMOVE_ITEMS_ALBUM);
    		}
    	else
    		{
    		text = StringLoader::LoadL(R_GLX_CONFIRMATION_REMOVE_NAME_ALBUM);
    		}
    	break;
    	case EMPXTag:
    	if (aMultiSelection)
    		{
    		text = StringLoader::LoadL(R_GLX_CONFIRMATION_REMOVE_ITEMS_TAG);
    		}
    	else
    		{
    		text = StringLoader::LoadL(R_GLX_CONFIRMATION_REMOVE_NAME_TAG);
    		}
    	break;
            default: break; // return NULL
            }
        }
    return text;
    }
    
// ---------------------------------------------------------------------------
// CGlxCommandHandlerRemoveFrom::DoActivateL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRemoveFrom::DoActivateL(TInt /*aViewId*/)
	{
		
 	}

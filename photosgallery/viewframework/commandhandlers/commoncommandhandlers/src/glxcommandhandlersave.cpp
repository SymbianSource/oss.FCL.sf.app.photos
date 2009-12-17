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




#include "glxcommandhandlersave.h"

#include <AknUtils.h>
#include <AknCommonDialogsDynMem.h>
#include <CAknCommonDialogsBase.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>
#include <glxuiutility.h>
#include <glxuistd.h>
#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxscreenfurniture.h>
#include <mpxcollectionpath.h>
#include <StringLoader.h>
#include <f32file.h>
#include <glxappui.h>
#include <aknViewAppUi.h>
#include <glxicons.mbg>
#include <glxtracer.h>
#include <glxtextentrypopup.h>
#include <DocumentHandler.h>
#include <apmstd.h>
#include <glximageviewermanager.h>
#include "glxcommandfactory.h"




// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSave* CGlxCommandHandlerSave::NewL(
        MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem)
    {
    TRACER("CGlxCommandHandlerSave::NewL");
    CGlxCommandHandlerSave* self = new (ELeave) CGlxCommandHandlerSave(aMediaListProvider, aHasToolbarItem);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerSave::CGlxCommandHandlerSave(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem)
        : CGlxMpxCommandCommandHandler( aMediaListProvider, aHasToolbarItem )
    {
    // Don't do anything.
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSave::ConstructL()
    {
    TRACER("CGlxCommandHandlerSave::ConstructL");    
    iUiUtility = CGlxUiUtility::UtilityL();
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();
    // Load resource file
	TParse parse;
    parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
   	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);

   	// Add supported command
   	TCommandInfo saveInfo( EGlxCmdSave );
   	// Filter out static items
    saveInfo.iMinSelectionLength = 1;
   	AddCommandL(saveInfo);
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSave::~CGlxCommandHandlerSave()
    {
    TRACER("CGlxCommandHandlerSave::~CGlxCommandHandlerSave");
    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
    if(iUiUtility)
        {
        iUiUtility->Close();
        }
    if ( NULL != iImageViewerInstance)
        {
        iImageViewerInstance->DeleteInstance();
        }
    }

// ---------------------------------------------------------------------------
// Create an add to container command
// ---------------------------------------------------------------------------
//
CMPXCommand* CGlxCommandHandlerSave::CreateCommandL(TInt /*aCommandId*/, 
        MGlxMediaList& /*aMediaList*/, TBool& /*aConsume*/) const
    {
    TRACER("CGlxCommandHandlerSave::CreateCommandL");
    CMPXCommand* command = NULL;
    TDataType nullType;
    CDocumentHandler* handler = CDocumentHandler::NewLC( NULL );
    HBufC* imagePath = iImageViewerInstance->ImageUri();
    TInt err = handler->CopyL( *imagePath, *imagePath, nullType, NULL );
    CleanupStack::PopAndDestroy(handler);    
    return command;
    } 
// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSave::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* aMenuPane)
    {
    TRACER("CGlxCommandHandlerSave::DynInitMenuPaneL");
    if ( aMenuPane )
        {
        TInt pos;
        if ( aMenuPane->MenuItemExists(EGlxCmdSave, pos) )
            {            
            if(!iImageViewerInstance->IsPrivate())
                {
                aMenuPane->DeleteMenuItem(EGlxCmdSave);
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// DoActivateL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSave::DoActivateL(TInt /*aViewId*/)
	{

	}

// ----------------------------------------------------------------------------
// OfferKeyEventL
// ----------------------------------------------------------------------------
TKeyResponse CGlxCommandHandlerSave::OfferKeyEventL(const TKeyEvent& aKeyEvent, 
															TEventCode aType)
	{
    TKeyResponse response = EKeyWasNotConsumed;
    // Is the key event from the Cancel (Backspace) key
	if (aKeyEvent.iCode == EKeyBackspace && aType == EEventKey)
		{
		// try to execute the delete command
		if(ExecuteL( EGlxCmdSave ))
		    {
    		response = EKeyWasConsumed;
		    }
		}
	return response;	
	}

// ----------------------------------------------------------------------------
// PopulateToolbar
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerSave::PopulateToolbarL()
	{
	TRACER( "CGlxCommandHandlerRename::PopulateToolbar" );
	
	iUiUtility->ScreenFurniture()->SetTooltipL( EGlxCmdSave, CAknButton::EPositionLeft );
	}



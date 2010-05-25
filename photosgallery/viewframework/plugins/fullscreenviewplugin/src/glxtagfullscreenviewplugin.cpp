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
* Description:    Implementation 
*
*/




// INCLUDE FILES

#include "glxtagfullscreenviewplugin.h"
#include "glxfullscreenviewplugin.hrh"                        // KGlxTagTileViewPluginImplementationUid

#include <glxfullscreenviewdata.rsg>

#include <bautils.h>									//BaflUtils
#include <data_caging_path_literals.hrh>				//KDC_APP_RESOURCE_DIR			
#include <glxcommandhandlerremovefrom.h>				//CGlxCommandHandlerRemoveFrom
#include <glxcommandhandlerhelp.h>
#include <glxtracer.h>
#include <glxlog.h>

#include <glxresourceutilities.h>                       // for CGlxResourceUtilities
#include <glxcommandhandlervideoplayback.h>

_LIT(KGlxTileResource,"glxfullscreenviewdata.rsc");		//TagTileView Resource File

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
CGlxTagFullScreenViewPlugin::CGlxTagFullScreenViewPlugin()
    {
    TRACER("CGlxTagFullScreenViewPlugin::CGlxTagFullScreenViewPlugin()");
    
    iResourceIds.iViewId = R_GLX_TAGFULLSCREEN_VIEW;
    iResourceIds.iMenuId = R_GLX_TAGFULLSCREEN_MENU;
    iResourceIds.iCbaId = R_GLX_FULLSCREEN_SOFTKEYS;
    iViewUID = KGlxTagFullScreenViewPluginImplementationUid;    
    
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
void CGlxTagFullScreenViewPlugin::ConstructL()
    {
        TRACER("CGlxTagFullScreenViewPlugin::CGlxTagFullScreenViewPlugin()");
    // Do nothing
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
 CGlxTagFullScreenViewPlugin* CGlxTagFullScreenViewPlugin::NewL()
    {
    
    TRACER( "CGlxTagFullScreenViewPlugin::NewL");
    
    CGlxTagFullScreenViewPlugin* self = new ( ELeave ) CGlxTagFullScreenViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CGlxTagFullScreenViewPlugin::~CGlxTagFullScreenViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Adds Command Handlers to the view
//-----------------------------------------------------------------------------
void CGlxTagFullScreenViewPlugin::AddCommandHandlersL()
	{
	TRACER( "CGlxTagFullScreenViewPlugin::AddCommandHandlersL");
	
	//Fix for error ID EVTY-7M87LF
	//@ Registration of Video Playback Command handler has to before UPnP.
	//@ else UPnP will not get play command once video playback command is consumed.
	iFullScreenView->AddCommandHandlerL(
			CGlxCommandHandlerVideoPlayback::NewL(iFullScreenView));
		
	CGlxFullScreenViewPluginBase::AddCommandHandlersL();
	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());

	//For tag tile view
	GLX_LOG_INFO("Adding CGlxCommandHandlerRemoveFrom");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerRemoveFrom::NewL(
			iFullScreenView, EMPXTag, uiutilitiesrscfile));

	// set up help context info
	TGlxHelpContext helpInfo;
	helpInfo.iBrowseContext = LGAL_HLP_TAGS_GRID;
	helpInfo.iBrowseContext = LGAL_HLP_TAGS_FULLSCREEN;

	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
	}

//-----------------------------------------------------------------------------
// Get the Resource File Name
//-----------------------------------------------------------------------------

void CGlxTagFullScreenViewPlugin::GetResourceFilenameL(TFileName& aResFile)
	{
	TRACER( "CGlxTagFullScreenViewPlugin::GetResourceFilenameL"); //Logging 
    TParse parse;
    parse.Set(KGlxTileResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
	}

//End Of File

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

#include "glxmainfullscreenviewplugin.h"
#include "glxfullscreenviewplugin.hrh"

#include <data_caging_path_literals.hrh>

#include <glxfullscreenviewdata.rsg>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities

#include <glxcommandhandlerhelp.h>

#include <AknUtils.h>
#include <avkon.hrh>
#include <bautils.h>

#include <glxfullscreenview.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <glxcommandhandlervideoplayback.h>

_LIT(KGlxFullScreenResource,"glxfullscreenviewdata.rsc");


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxMainFullScreenViewPlugin::CGlxMainFullScreenViewPlugin()

    {
    TRACER("CGlxMainFullScreenViewPlugin::CGlxMainFullScreenViewPlugin()");
    iResourceIds.iMenuId = R_GLX_FULLSCREEN_MENU;
    iResourceIds.iViewId = R_GLX_FULLSCREEN_VIEW;
    //Load empty soft keys at initialization
    iResourceIds.iCbaId = R_GLX_FULLSCREEN_EMPTYSOFTKEYS;
    iViewUID = KGlxMainFullScreenViewImplementationId;
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxMainFullScreenViewPlugin::ConstructL()
    {
    TRACER("CGlxMainFullScreenViewPlugin::ConstructL()");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMainFullScreenViewPlugin* CGlxMainFullScreenViewPlugin::NewL()
    {
    TRACER("CGlxMainFullScreenViewPlugin::NewL()");
    CGlxMainFullScreenViewPlugin* self = 
        new ( ELeave ) CGlxMainFullScreenViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMainFullScreenViewPlugin::~CGlxMainFullScreenViewPlugin()
    {
    TRACER("CGlxMainFullScreenViewPlugin::~CGlxMainFullScreenViewPlugin()");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// AddCommandHandlersL
// ---------------------------------------------------------------------------
//
void CGlxMainFullScreenViewPlugin::AddCommandHandlersL()
    {
	//Fix for error ID EVTY-7M87LF
	//@ Registration of Video Playback Command handler has to before UPnP.
	//@ else UPnP will not get play command once video playback command is consumed.
	iFullScreenView->AddCommandHandlerL(
			CGlxCommandHandlerVideoPlayback::NewL(iFullScreenView));
	
    CGlxFullScreenViewPluginBase::AddCommandHandlersL();

    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_ALL_FULLSCREEN;
    
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
    }

// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
void CGlxMainFullScreenViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TRACER("CGlxMainFullScreenViewPlugin::GetResourceFilenameL()");
    TParse parse;
    parse.Set(KGlxFullScreenResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

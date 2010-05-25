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
* Description:    Implementation of download FullScreen view plugin 
*
*/





// INCLUDE FILES

#include "glxdownloadsfullscreenviewplugin.h"

#include "glxfullscreenviewplugin.hrh"
#include <avkon.hrh>
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>

#include <AknUtils.h>
#include <bautils.h>
#include <glxcollectiongeneraldefs.h>
#include <glxcommandhandlerhelp.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <glxmedia.h>
#include <glxmedialist.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxsettingsmodel.h>			              // For Cenrep Keys
#include <glxfullscreenview.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>
#include <StringLoader.h>

#include <glxfullscreenviewdata.rsg>
#include <glxicons.mbg> // icons 
#include <glxcommandhandlervideoplayback.h>

_LIT(KGlxFullScreenResource,"glxfullscreenviewdata.rsc");



// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxDownloadsFullScreenViewPlugin::CGlxDownloadsFullScreenViewPlugin()

    {
    iResourceIds.iMenuId = R_GLX_FULLSCREEN_MENU;
    iResourceIds.iViewId = R_GLX_FULLSCREEN_VIEW;
    iResourceIds.iCbaId = R_GLX_FULLSCREEN_SOFTKEYS;
    iViewUID = KGlxDownloadsFullScreenViewImplementationId;
    
    
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxDownloadsFullScreenViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxDownloadsFullScreenViewPlugin* CGlxDownloadsFullScreenViewPlugin::NewL()
    {
    CGlxDownloadsFullScreenViewPlugin* self = 
        new ( ELeave ) CGlxDownloadsFullScreenViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxDownloadsFullScreenViewPlugin::~CGlxDownloadsFullScreenViewPlugin()
    {
    if ( iResOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResOffset );
        }
    }

// ---------------------------------------------------------------------------
// AddCommandHandlersL
// ---------------------------------------------------------------------------
//
void CGlxDownloadsFullScreenViewPlugin::AddCommandHandlersL()
    {
    GLX_LOG_ENTRY_EXIT_LEAVE_L("CGlxDownloadsFullScreenViewPlugin::AddCommandHandlersL()");
    
    //Fix for error ID EVTY-7M87LF
	//@ Registration of Video Playback Command handler has to before UPnP.
	//@ else UPnP will not get play command once video playback command is consumed.
	iFullScreenView->AddCommandHandlerL(
			CGlxCommandHandlerVideoPlayback::NewL(iFullScreenView));
	
    CGlxFullScreenViewPluginBase::AddCommandHandlersL();
    
    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_DOWNLOADS_FULLSCREEN;
    
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
    }



// -----------------------------------------------------------------------------
// GetResourceFilenameL
// -----------------------------------------------------------------------------
//
void CGlxDownloadsFullScreenViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxFullScreenResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

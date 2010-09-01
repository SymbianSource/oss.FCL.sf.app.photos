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

#include "glximageviewerfullscreenviewplugin.h"

#include "glxfullscreenviewplugin.hrh"
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlerhelp.h>
#include <glxcommandhandlers.hrh>

#include <AknUtils.h>
#include <avkon.hrh>
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
#include <photos.hlp.hrh>
#include <glxfullscreenviewdata.rsg>
#include <glxicons.mbg> // icons 

_LIT(KGlxFullScreenResource,"glxfullscreenviewdata.rsc");



// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxImageViewerScreenViewPlugin::CGlxImageViewerScreenViewPlugin()

    {
    	
    TRACER("CGlxImageViewerScreenViewPlugin::CGlxImageViewerScreenViewPlugin()");	
    	
    iResourceIds.iMenuId = R_GLX_IMAGEVIEWER_FULLSCREEN_MENU;   //r_glx_imageviewer_fullscreen_menu
    iResourceIds.iViewId = R_GLX_IMAGEVIEWER_FULLSCREEN_VIEW;   //r_glx_imageviewer_fullscreen_view
    iResourceIds.iCbaId = R_GLX_FULLSCREEN_SOFTKEYS;
    iViewUID = KGlxImageViewerFullScreenViewImplementationId;    
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxImageViewerScreenViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxImageViewerScreenViewPlugin* CGlxImageViewerScreenViewPlugin::NewL()
    {
    TRACER("CGlxImageViewerScreenViewPlugin::::NewL()");	
    
    CGlxImageViewerScreenViewPlugin* self = 
        new ( ELeave ) CGlxImageViewerScreenViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxImageViewerScreenViewPlugin::~CGlxImageViewerScreenViewPlugin()
    {
    TRACER("CGlxImageViewerScreenViewPlugin::::~CGlxImageViewerScreenViewPlugin()");	
    	
    if ( iResOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResOffset );
        }
    }

// ---------------------------------------------------------------------------
// AddCommandHandlersL
// ---------------------------------------------------------------------------
//
void CGlxImageViewerScreenViewPlugin::AddCommandHandlersL()
    {
    TRACER("CGlxImageViewerScreenViewPlugin::AddCommandHandlersL()");	
    	
    GLX_LOG_ENTRY_EXIT_LEAVE_L("CGlxImageViewerScreenViewPlugin::AddCommandHandlersL()");
    CGlxFullScreenViewPluginBase::AddCommandHandlersL();

    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_IMAGE_VIEWER;
    
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
    }



// -----------------------------------------------------------------------------
// GetResourceFilenameL
// -----------------------------------------------------------------------------
//
void CGlxImageViewerScreenViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TRACER("CGlxImageViewerScreenViewPlugin::GetResourceFilenameL(TFileName& aResFile)");		
    	
    TParse parse;
    parse.Set(KGlxFullScreenResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

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

#include "glxalbumfullscreenviewplugin.h"

#include "glxfullscreenviewplugin.hrh"
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>

#include <AknUtils.h>
#include <avkon.hrh>
#include <bautils.h>
#include <glxcommandhandlerhelp.h>
#include <glxcommandhandlerremovefrom.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxfullscreenviewdata.rsg>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>

#include "glxcommandhandlerdetails.h"
#include "glxfullscreenview.h"

_LIT(KGlxFullScreenResource,"glxfullscreenviewdata.rsc");

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxAlbumFullScreenViewPlugin::CGlxAlbumFullScreenViewPlugin()

    {
    iResourceIds.iMenuId = R_GLX_ALBUM_FULLSCREEN_MENU;
    iResourceIds.iViewId = R_GLX_ALBUM_FULLSCREEN_VIEW;
    iResourceIds.iCbaId = R_GLX_FULLSCREEN_SOFTKEYS;
    iViewUID = KGlxAlbumFullScreenViewImplementationId;
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxAlbumFullScreenViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxAlbumFullScreenViewPlugin* CGlxAlbumFullScreenViewPlugin::NewL()
    {
    CGlxAlbumFullScreenViewPlugin* self = 
        new ( ELeave ) CGlxAlbumFullScreenViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxAlbumFullScreenViewPlugin::~CGlxAlbumFullScreenViewPlugin()
    {
    // Do nothing
    }

    
void CGlxAlbumFullScreenViewPlugin::AddCommandHandlersL()
	{
	CGlxFullScreenViewPluginBase::AddCommandHandlersL();
	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());
	// Remove from Album.
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerRemoveFrom::NewL(
			iFullScreenView, EMPXAlbum, uiutilitiesrscfile));

	TGlxHelpContext helpInfo;
	helpInfo.iBrowseContext = LGAL_HLP_ALBUM_FULLSCREEN;

	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
	}


void CGlxAlbumFullScreenViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxFullScreenResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

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

#include "glxalbumgridviewplugin.h"

#include "glxgridviewplugin.hrh"
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>

#include <AknUtils.h>
#include <avkon.hrh>
#include <bautils.h>
#include <glxcommandhandlerhelp.h>
#include <glxcommandhandlerremovefrom.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxgridviewdata.rsg>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>

#include "glxcommandhandlerdetails.h"
#include "glxgridview.h"

_LIT(KGlxGridResource,"glxgridviewdata.rsc");

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxAlbumGridViewPlugin::CGlxAlbumGridViewPlugin()

    {
    iResourceIds.iMenuId = R_GLX_ALBUM_GRID_MENU;
    iResourceIds.iViewId = R_GLX_ALBUM_GRID_VIEW;
    iResourceIds.iCbaId = R_GRID_SOFTKEYS;
    iResourceIds.iEmptyViewTextId = R_GRID_EMPTY_VIEW_TEXT;
    iResourceIds.iOkOptionsMenuId = R_GRID_OK_MENUBAR_ALBUM_GRIDVIEW;
    iViewUID = KGlxAlbumGridViewImplementationId;
    
      
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxAlbumGridViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxAlbumGridViewPlugin* CGlxAlbumGridViewPlugin::NewL()
    {
    CGlxAlbumGridViewPlugin* self = 
        new ( ELeave ) CGlxAlbumGridViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxAlbumGridViewPlugin::~CGlxAlbumGridViewPlugin()
    {
    // Do nothing
    }

    
void CGlxAlbumGridViewPlugin::AddCommandHandlersL()
	{
	CGlxGridViewPluginBase::AddCommandHandlersL();
	// Remove from Album
	///@todo this should only be added to the albums view plugin.
	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());

	iGridView->AddCommandHandlerL(CGlxCommandHandlerRemoveFrom::NewL(iGridView,
			EMPXAlbum, uiutilitiesrscfile));

	TGlxHelpContext helpInfo;
	helpInfo.iBrowseContext = LGAL_HLP_ALBUM_GRID;
	helpInfo.iViewContext = LGAL_HLP_ALBUM_FULLSCREEN;

	iGridView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));

	}



void CGlxAlbumGridViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxGridResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

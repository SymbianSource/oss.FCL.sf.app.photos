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

#include "glxcameraalbumgridviewplugin.h"

#include "glxgridviewplugin.hrh"
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>

#include <glxgridviewdata.rsg>


#include <AknUtils.h>
#include <avkon.hrh>
#include <bautils.h>
#include <glxcommandhandlerhelp.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>

#include "glxgridview.h"

_LIT(KGlxGridResource,"glxgridviewdata.rsc");

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCameraAlbumGridViewPlugin::CGlxCameraAlbumGridViewPlugin()

    {
    iResourceIds.iMenuId = R_GLX_GRID_MENU;
    iResourceIds.iViewId = R_GLX_CAPTURED_GRID_VIEW;
    iResourceIds.iCbaId = R_GRID_SOFTKEYS;
    iResourceIds.iEmptyViewTextId = R_GRID_EMPTY_VIEW_TEXT;
    iResourceIds.iOkOptionsMenuId = R_GRID_OK_MENUBAR;
    iViewUID = KGlxCameraAlbumGridViewImplementationId;
    
         
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCameraAlbumGridViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxCameraAlbumGridViewPlugin* CGlxCameraAlbumGridViewPlugin::NewL()
    {
    CGlxCameraAlbumGridViewPlugin* self = 
        new ( ELeave ) CGlxCameraAlbumGridViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxCameraAlbumGridViewPlugin::~CGlxCameraAlbumGridViewPlugin()
    {
    // Do nothing
    }

    
void CGlxCameraAlbumGridViewPlugin::AddCommandHandlersL()
    {
    CGlxGridViewPluginBase::AddCommandHandlersL();
      
    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_CAMERA_ALBUM_GRID;
    helpInfo.iViewContext = LGAL_HLP_CAMERA_ALBUM_FULLSCRN;
    iGridView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo)); 
    }

void CGlxCameraAlbumGridViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxGridResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File
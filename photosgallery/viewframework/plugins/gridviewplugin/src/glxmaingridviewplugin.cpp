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

#include "glxmaingridviewplugin.h"
#include "glxgridviewplugin.hrh"

#include <data_caging_path_literals.hrh>

#include <glxgridviewdata.rsg>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities

#include <glxcommandhandlerhelp.h>

#include <AknUtils.h>
#include <avkon.hrh>
#include <bautils.h>

#include <glxgridview.h>
#include <glxcommandhandlerslideshow.h>
#include <glxtracer.h>
#include <glxlog.h>


_LIT(KGlxGridResource,"glxGridviewdata.rsc");


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxMainGridViewPlugin::CGlxMainGridViewPlugin()

    {
    TRACER("CGlxMainGridViewPlugin::CGlxMainGridViewPlugin()");
    iResourceIds.iMenuId = R_GLX_GRID_MENU;
    iResourceIds.iViewId = R_GRID_VIEW;
    iResourceIds.iCbaId = R_GRID_SOFTKEYS;
    iResourceIds.iEmptyViewTextId = R_GRID_EMPTY_VIEW_TEXT;
    iResourceIds.iOkOptionsMenuId = R_GRID_OK_MENUBAR;
    iViewUID = KGlxMainGridViewImplementationId;
    
                
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxMainGridViewPlugin::ConstructL()
    {
    GLX_LOG_INFO("CGlxMainGridViewPlugin::ConstructL()");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMainGridViewPlugin* CGlxMainGridViewPlugin::NewL()
    {
    TRACER("CGlxMainGridViewPlugin::NewL()");
    CGlxMainGridViewPlugin* self = 
        new ( ELeave ) CGlxMainGridViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMainGridViewPlugin::~CGlxMainGridViewPlugin()
    {
    TRACER("CGlxMainGridViewPlugin::~CGlxMainGridViewPlugin()");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// AddCommandHandlersL
// ---------------------------------------------------------------------------
//
void CGlxMainGridViewPlugin::AddCommandHandlersL()
    {
    CGlxGridViewPluginBase::AddCommandHandlersL();

    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_ALL_GRID;
    helpInfo.iViewContext = LGAL_HLP_ALL_FULLSCREEN;
    GLX_LOG_INFO( "Adding CGlxCommandHandlerSlideshow" );
    iGridView->AddCommandHandlerL(
                    CGlxCommandHandlerSlideshow::NewL( iGridView, ETrue , EFalse ) );
    iGridView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
    }

// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
void CGlxMainGridViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TRACER("CGlxMainGridViewPlugin::GetResourceFilenameL()");
    TParse parse;
    parse.Set(KGlxGridResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

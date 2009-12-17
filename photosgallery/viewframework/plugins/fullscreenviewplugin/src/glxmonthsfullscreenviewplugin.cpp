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




// INCLUDE Files

#include "glxmonthsfullscreenviewplugin.h"
#include "glxfullscreenviewplugin.hrh"

#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>

#include <glxfullscreenviewdata.rsg>

#include <AknUtils.h>
#include <avkon.hrh>
#include <bautils.h>
#include <glxcommandhandlerhelp.h>
#include <glxcommandhandlerremovefrom.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>

#include "glxfullscreenview.h"

#include <glxcommandhandlers.hrh>

_LIT(KGlxFullScreenResource,"glxfullscreenviewdata.rsc");

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxMonthsFullScreenViewPlugin::CGlxMonthsFullScreenViewPlugin()

    {
    iResourceIds.iMenuId = R_GLX_FULLSCREEN_MENU;
    iResourceIds.iViewId = R_GLX_FULLSCREEN_VIEW;
    iResourceIds.iCbaId = R_GLX_FULLSCREEN_SOFTKEYS;
    iViewUID = KGlxMonthsFullScreenViewImplementationId;   
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxMonthsFullScreenViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMonthsFullScreenViewPlugin* CGlxMonthsFullScreenViewPlugin::NewL()
    {
    CGlxMonthsFullScreenViewPlugin* self = 
        new ( ELeave ) CGlxMonthsFullScreenViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMonthsFullScreenViewPlugin::~CGlxMonthsFullScreenViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// AddCommandHandlersL
// ---------------------------------------------------------------------------
//    
void CGlxMonthsFullScreenViewPlugin::AddCommandHandlersL()
    {
    CGlxFullScreenViewPluginBase::AddCommandHandlersL();
    
    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_MONTH_FULLSCREEN;
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo)); 
    }


// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
void CGlxMonthsFullScreenViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxFullScreenResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

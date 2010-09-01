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

#include "glxmonthsgridviewplugin.h"
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

#include <photos.hlp.hrh>

#include "glxgridview.h"

#include <glxcommandhandlers.hrh>

_LIT(KGlxGridResource,"glxgridviewdata.rsc");

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxMonthsGridViewPlugin::CGlxMonthsGridViewPlugin()

    {
    iResourceIds.iMenuId = R_GLX_GRID_MENU;
    iResourceIds.iViewId = R_GLX_MONTHS_GRID_VIEW;
    iResourceIds.iCbaId = R_GRID_SOFTKEYS;
    iResourceIds.iEmptyViewTextId = R_GRID_EMPTY_VIEW_TEXT;
    iResourceIds.iOkOptionsMenuId = R_GRID_OK_MENUBAR;
    iViewUID = KGlxMonthsGridViewImplementationId;
    
                    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxMonthsGridViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMonthsGridViewPlugin* CGlxMonthsGridViewPlugin::NewL()
    {
    CGlxMonthsGridViewPlugin* self = 
        new ( ELeave ) CGlxMonthsGridViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMonthsGridViewPlugin::~CGlxMonthsGridViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// AddCommandHandlersL
// ---------------------------------------------------------------------------
//    
void CGlxMonthsGridViewPlugin::AddCommandHandlersL()
    {
    CGlxGridViewPluginBase::AddCommandHandlersL();     

    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_MONTH_GRID;
    helpInfo.iViewContext = LGAL_HLP_MONTH_FULLSCREEN;
    iGridView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo)); 
    }


// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
void CGlxMonthsGridViewPlugin::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxGridResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
    }

//  End of File

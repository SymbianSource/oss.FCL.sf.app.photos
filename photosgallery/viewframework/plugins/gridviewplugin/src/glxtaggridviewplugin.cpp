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

#include "glxtaggridviewplugin.h"
#include "glxgridviewplugin.hrh"                        // KGlxTagGridViewPluginImplementationUid

#include <glxgridviewdata.rsg>

#include <bautils.h>									//BaflUtils
#include <data_caging_path_literals.hrh>				//KDC_APP_RESOURCE_DIR			
#include <glxcommandhandlerremovefrom.h>				//CGlxCommandHandlerRemoveFrom
#include <glxcommandhandlerhelp.h>
#include <glxtracer.h>
#include <glxlog.h>

#include <glxresourceutilities.h>                       // for CGlxResourceUtilities



_LIT(KGlxGridResource,"glxgridviewdata.rsc");		//TagGridView Resource File

/**
 * @internal reviewed 12/06/2007 by Alex Birkett
 */

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
CGlxTagGridViewPlugin::CGlxTagGridViewPlugin()
    {
    TRACER("CGlxTagGridViewPlugin::CGlxTagGridViewPlugin()");
    
    iResourceIds.iViewId = R_TAGGRID_VIEW;
    iResourceIds.iMenuId = R_TAGGRID_MENU;
    iResourceIds.iCbaId = R_GRID_SOFTKEYS;
    iResourceIds.iEmptyViewTextId = R_GRID_EMPTY_VIEW_TEXT;
    iResourceIds.iOkOptionsMenuId = R_GRID_OK_MENUBAR_TAG_GRIDVIEW;
    iViewUID = KGlxTagGridViewPluginImplementationUid;    
    
                        
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
void CGlxTagGridViewPlugin::ConstructL()
    {
        TRACER("CGlxTagGridViewPlugin::CGlxTagGridViewPlugin()");
    // Do nothing
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
 CGlxTagGridViewPlugin* CGlxTagGridViewPlugin::NewL()
    {
    
    TRACER( "CGlxTagGridViewPlugin::NewL");
    
    CGlxTagGridViewPlugin* self = new ( ELeave ) CGlxTagGridViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CGlxTagGridViewPlugin::~CGlxTagGridViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Adds Command Handlers to the view
//-----------------------------------------------------------------------------
void CGlxTagGridViewPlugin::AddCommandHandlersL()
   {
   TRACER( "CGlxTagGridViewPlugin::AddCommandHandlersL"); 
    
   CGlxGridViewPluginBase::AddCommandHandlersL();
    
   //For tag Grid view
   GLX_LOG_INFO("Adding CGlxCommandHandlerRemoveFrom");
   iGridView->AddCommandHandlerL(CGlxCommandHandlerRemoveFrom::NewL(iGridView, EMPXTag)); 

    // set up help context info
    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext = LGAL_HLP_TAGS_GRID;
    helpInfo.iViewContext = LGAL_HLP_TAGS_FULLSCREEN;
    
    iGridView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo)); 
 }

//-----------------------------------------------------------------------------
// Get the Resource File Name
//-----------------------------------------------------------------------------

void CGlxTagGridViewPlugin::GetResourceFilenameL(TFileName& aResFile)
	{
	TRACER( "CGlxTagGridViewPlugin::GetResourceFilenameL"); //Logging 
    TParse parse;
    parse.Set(KGlxGridResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(aResFile);  
	}

//End Of File

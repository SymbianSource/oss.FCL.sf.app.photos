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
* Description:    List view plugin implementation 
*
*/




// INCLUDE FILES
#include "glxmainlistviewplugin.h"

#include <StringLoader.h>						//StringLoader

#include <glxcommandhandlerhelp.h>				//Command Handler Help
#include <glxcommandhandlershowviaupnp.h>		//Command Handlers Show Via upnp
#include <glxlistview.h>
#include <glxlistviewplugin.rsg>

#include "glxlistviewplugin.hrh"

// ---------------------------------------------------------------------------
// Two-phased constructor for main list view plugin.
// ---------------------------------------------------------------------------
//
CGlxMainListViewPlugin* CGlxMainListViewPlugin::NewL()
    {
    CGlxMainListViewPlugin* self = new (ELeave) CGlxMainListViewPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxMainListViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CGlxMainListViewPlugin::CGlxMainListViewPlugin()
    {
    iResourceIds.iViewId = R_GLX_MAIN_LIST_VIEW;
    iResourceIds.iMenuId = R_GLX_MAIN_LIST_VIEW_MENU;
    iResourceIds.iCbaId = R_GLX_MAIN_LIST_VIEW_SOFTKEYS;
    iResourceIds.iEmptyTextId = R_GLX_LIST_VIEW_EMPTY_TEXT;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMainListViewPlugin::~CGlxMainListViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CGlxListViewPluginBase
// ---------------------------------------------------------------------------
//
CGlxListView* CGlxMainListViewPlugin::ConstructListViewLC(MGlxMediaListFactory* aMediaListFactory)
    {
    HBufC* mainTitle = StringLoader::LoadLC(R_GLX_MAIN_LIST_VIEW_TITLE);
		
    CGlxListView* listView = CGlxListView::NewL(aMediaListFactory, KGlxMainListViewImplementationUid, iResourceIds,*mainTitle);

    CleanupStack::PopAndDestroy(mainTitle);

    CleanupStack::PushL(listView);
    
    iListView = listView;

    return listView;
    }

// ---------------------------------------------------------------------------
// From CGlxListViewPluginBase
// ---------------------------------------------------------------------------
//
void CGlxMainListViewPlugin::AddCommandHandlersL()
    {
    // add help command handler
    TGlxHelpContext helpInfo;
    helpInfo.iBrowseContext =  LGAL_HLP_VIEW_LIST;
    iListView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo)); 
    //add ShowViaUpnp Command Handler for stop showing
    iListView->AddCommandHandlerL(CGlxCommandHandlerShowViaUpnp::NewL(iListView, EFalse)); 
    }

//  End of File

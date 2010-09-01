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
#include "glxalbumlistviewplugin.h"

#include <glxlistview.h>
#include <glxcommandhandlerdelete.h>
#include <glxcommandhandlernewmedia.h>
#include <glxcommandhandlerrename.h>
#include <glxcommandhandlerhelp.h>
#include <glxcommandhandlerslideshow.h>
#include <glxlistviewplugin.rsg>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include "glxlistviewplugin.hrh"

// ---------------------------------------------------------------------------
// Two-phased constructor for album list view plugin.
// ---------------------------------------------------------------------------
//
CGlxAlbumListViewPlugin* CGlxAlbumListViewPlugin::NewL()
    {
    CGlxAlbumListViewPlugin* self = new (ELeave) CGlxAlbumListViewPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxAlbumListViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CGlxAlbumListViewPlugin::CGlxAlbumListViewPlugin()
    {
    iResourceIds.iViewId = R_GLX_ALBUM_LIST_VIEW;
    iResourceIds.iMenuId = R_GLX_ALBUM_LIST_VIEW_MENU;
    iResourceIds.iCbaId = R_GLX_LIST_VIEW_SOFTKEYS;
    iResourceIds.iEmptyTextId = R_GLX_LIST_VIEW_EMPTY_TEXT;
    
    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxAlbumListViewPlugin::~CGlxAlbumListViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CGlxListViewPluginBase
// ---------------------------------------------------------------------------
//
CGlxListView* CGlxAlbumListViewPlugin::ConstructListViewLC(MGlxMediaListFactory* aMediaListFactory)
    {
    return CGlxListView::NewLC(aMediaListFactory, KGlxAlbumListViewImplementationUid, iResourceIds);
    }

// ---------------------------------------------------------------------------
// From CGlxListViewPluginBase
// ---------------------------------------------------------------------------
//
void CGlxAlbumListViewPlugin::AddCommandHandlersL()
	{
	CGlxListViewPluginBase::AddCommandHandlersL();
	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());

	iListView->AddCommandHandlerL(CGlxCommandHandlerSlideshow::NewL(iListView,
			EFalse, ETrue, uiutilitiesrscfile));
	iListView->AddCommandHandlerL(CGlxCommandHandlerDelete::NewL(iListView,
			ETrue, EFalse, uiutilitiesrscfile));
	iListView->AddCommandHandlerL(CGlxCommandHandlerNewMedia::NewL(iListView,
			uiutilitiesrscfile));
	iListView->AddCommandHandlerL(CGlxCommandHandlerRename::NewL(iListView,
			ETrue, uiutilitiesrscfile));
	// add help command handler

	// set up help context info
	TGlxHelpContext helpInfo;
	helpInfo.iBrowseContext = LGAL_HLP_ALBUM_LIST;

	iListView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
	}

//  End of File

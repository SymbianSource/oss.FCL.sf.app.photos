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
#include "glxmonthlistviewplugin.h"

#include <glxcommandhandlerhelp.h>
#include <glxcommandhandlerslideshow.h>
#include <glxlistview.h>
#include <glxresourceutilities.h>
#include <glxlistviewplugin.rsg>


#include "glxlistviewplugin.hrh"

// ---------------------------------------------------------------------------
// Two-phased constructor for month list view plugin.
// ---------------------------------------------------------------------------
//
CGlxMonthListViewPlugin* CGlxMonthListViewPlugin::NewL()
    {
    CGlxMonthListViewPlugin* self = new (ELeave) CGlxMonthListViewPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxMonthListViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CGlxMonthListViewPlugin::CGlxMonthListViewPlugin()
    {
    iResourceIds.iViewId = R_GLX_MONTH_LIST_VIEW;
    iResourceIds.iMenuId = R_GLX_MONTH_LIST_VIEW_MENU;
    iResourceIds.iCbaId = R_GLX_LIST_VIEW_SOFTKEYS;
    iResourceIds.iEmptyTextId = R_GLX_LIST_VIEW_EMPTY_TEXT;
    
    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMonthListViewPlugin::~CGlxMonthListViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CGlxListViewPluginBase
// ---------------------------------------------------------------------------
//
CGlxListView* CGlxMonthListViewPlugin::ConstructListViewLC(MGlxMediaListFactory* aMediaListFactory)
    {
    	
    return CGlxListView::NewLC(aMediaListFactory, KGlxMonthListViewImplementationUid, iResourceIds);
    }

// ---------------------------------------------------------------------------
// From CGlxListViewPluginBase
// ---------------------------------------------------------------------------
//
void CGlxMonthListViewPlugin::AddCommandHandlersL()
	{
	CGlxListViewPluginBase::AddCommandHandlersL();
	// set up help context info
	TGlxHelpContext helpInfo;
	helpInfo.iBrowseContext = LGAL_HLP_MONTH_LIST;
	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());
	iListView->AddCommandHandlerL(CGlxCommandHandlerSlideshow::NewL(iListView,
			EFalse, ETrue, uiutilitiesrscfile));
	iListView->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo));
	}

//  End of File

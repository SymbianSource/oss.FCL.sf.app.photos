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
* Description:    Tag browser view plugin implementation 
 *
*/




// INCLUDE FILES

#include "glxtagsbrowserviewplugin.h"  //class header

#include <glxtracer.h>        
#include <glxlog.h>                             // For Logs
#include <mpxcollectionutility.h>               // For MMPXCollectionUtility
#include <glxtagsbrowserview.rsg>               // For resources
#include <glxcloudview.h>                       // For CGlxSimpleListView
#include <glxcommandhandlerback.h>              // For CGlxCommandHandlerBack
#include <glxcommandhandlershowviaupnp.h>	//Command Handlers stop showing
#include <glxcommandhandlersortorder.h>         // For CGlxCommandHandlerSortOrder
#include <glxcommandhandlerhelp.h>
#include <glxfilterfactory.h>			              // For TGlxFilterFactory
#include <glxsettingsmodel.h>			              // For Cenrep Keys
#include <glxcommandhandlerslideshow.h>         // for slide show command handler
#include <glxcommandhandlerdelete.h>
#include <glxcommandhandlerrename.h>
#include <glxcommandhandlerdetails.h>        //For tag manager launch
#include <glxmedialist.h>

#include "glxtagsbrowserviewplugin.hrh"	// For Plugin Uids

// Resource File
_LIT(KResourceFile, "glxtagsbrowserview.rsc");


// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CGlxTagsBrowserViewPlugin::CGlxTagsBrowserViewPlugin()
{
   TRACER("CGlxTagsBrowserViewPlugin::CGlxTagsBrowserViewPlugin");
}

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxTagsBrowserViewPlugin::ConstructL()
{
   TRACER("CGlxTagsBrowserViewPlugin::ConstructL");
}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxTagsBrowserViewPlugin *CGlxTagsBrowserViewPlugin::NewL()
{
   TRACER("CGlxTagsBrowserViewPlugin::NewL");

  CGlxTagsBrowserViewPlugin *self = new(ELeave)CGlxTagsBrowserViewPlugin();
  CleanupStack::PushL(self);
  self->ConstructL();
  CleanupStack::Pop(self);
  return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxTagsBrowserViewPlugin::~CGlxTagsBrowserViewPlugin()
{
   TRACER("CGlxTagsBrowserViewPlugin::~CGlxTagsBrowserViewPlugin ");
 
}

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView *CGlxTagsBrowserViewPlugin::ConstructViewLC()
{
   TRACER("CGlxTagsBrowserViewPlugin::ConstructViewLC");

  GLX_LOG_INFO("CGlxTagsBrowserViewPlugin::ConstructViewLC - Create the view");
  CGlxCloudView *view = CGlxCloudView::NewLC(this, KResourceFile(),  
    // Resource file name
  R_TAGSBROWSER_VIEW,  // View resource
  R_TAGSBROWSER_EMPTY_TEXT,  // Empty list text
  R_TAGSBROWSER_SOFTKEYS_OPTIONS_BACK_SELECT, //softket resource id
  R_TAGSBROWSER_SOFTKEYS_MSK_DISABLED //msk disabled resource id
  );

	// Add commnad handler
  view->AddCommandHandlerL(CGlxCommandHandlerSlideshow::NewL(view,EFalse,ETrue));
  view->AddCommandHandlerL(CGlxCommandHandlerBack::NewBackCommandHandlerL());
  view->AddCommandHandlerL(CGlxCommandHandlerShowViaUpnp::NewL(view, EFalse));
  view->AddCommandHandlerL(CGlxCommandHandlerSortOrder::NewL(view,KGlxSortOrderTagBrowser));
  view->AddCommandHandlerL(CGlxCommandHandlerDetails::NewL(view));
  GLX_LOG_INFO("Adding CGlxCommandHandlerDelete");
  view->AddCommandHandlerL(CGlxCommandHandlerDelete::
                              NewL(view, ETrue, ETrue));
  
  TGlxHelpContext helpInfo;
  helpInfo.iBrowseContext = LGAL_HLP_TAGS_BROWSER;
  view->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo)); 
  view->AddCommandHandlerL(CGlxCommandHandlerRename::NewL(view,ETrue));

  return view;
}

// -----------------------------------------------------------------------------
// MediaListL
// -----------------------------------------------------------------------------
//	
MGlxMediaList& CGlxTagsBrowserViewPlugin::CreateMediaListL(MMPXCollectionUtility& aCollectionUtility) const
	{
	MGlxMediaList* mediaList = NULL;
	
    // Create media list
    CMPXCollectionPath* path = aCollectionUtility.Collection().PathL();
    CleanupStack::PushL(path);
    path->Back();

	// Create a filter
	GLX_LOG_INFO("CGlxTagsBrowserViewPlugin::MediaListL - Create a new filter");
    // Getting sort order from cenrep setting  
	TGlxFilterSortOrder sortOrder = static_cast<TGlxFilterSortOrder> ( CGlxSettingsModel::ValueL<TInt>( KGlxSortOrderTagBrowser ));
	
	// If sort order is alphbetical or no order is specified the create alphbetical filter
	CMPXFilter* filter = NULL;
    if( sortOrder == EGlxFilterSortOrderAlphabetical || sortOrder == EGlxFilterSortOrderNotUsed )
    	{
    	filter = TGlxFilterFactory::CreateAlphabeticSortFilterL();    	
    	}
	else
		{
		filter = TGlxFilterFactory::CreateItemCountSortFilterL();    	
		}
	CleanupStack::PushL(filter);

    mediaList = MGlxMediaList::InstanceL(*path, KGlxIdNone, filter);
    GLX_LOG_INFO1("CGlxTagsBrowserViewPlugin::MediaListL  - Path level = %d", path->Levels());
    
	CleanupStack::PopAndDestroy(filter);		// filter
    CleanupStack::PopAndDestroy(path); // path

	return *mediaList;
	}

//  End of File

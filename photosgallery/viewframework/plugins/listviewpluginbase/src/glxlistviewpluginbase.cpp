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

//  CLASS HEADER
#include "glxlistviewpluginbase.h"

//  EXTERNAL INCLUDES
#include <data_caging_path_literals.hrh>

//  INTERNAL INCLUDES
#include <glxlistview.h>
#include <glxcommandhandlerslideshow.h>
#include <glxcommandhandlerback.h>
#include <glxcommandhandlershowviaupnp.h>		// Command Handlers Stop Showing(upnp)
#include <glxresourceutilities.h>               // for GetResourceFilenameL


namespace
    {
    _LIT(KGlxListViewPluginResource, "glxlistviewplugin.rsc");
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxListViewPluginBase::CGlxListViewPluginBase()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxListViewPluginBase::~CGlxListViewPluginBase()
    {
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
EXPORT_C CAknView* CGlxListViewPluginBase::ConstructViewLC()
    {
    TFileName resFile;
    GetResourceFilenameL(resFile);

    if(resFile.Length() > 0)
        {
        iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resFile);
        }

    CGlxListView* listView = ConstructListViewLC(this);

    iListView = listView;

    AddCommandHandlersL();

    // Return on cleanup stack with ownership passed
    return listView;
    }


// ---------------------------------------------------------------------------
// AddCommandHandlersL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxListViewPluginBase::AddCommandHandlersL()
    {
    // we dont want to step back in path for slide show in this case
    
    iListView->AddCommandHandlerL(
                              CGlxCommandHandlerBack::NewBackCommandHandlerL());
    iListView->AddCommandHandlerL(
                              CGlxCommandHandlerShowViaUpnp::NewL(iListView, EFalse));
    }


// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxListViewPluginBase::GetResourceFilenameL(TFileName& aResFile)
    {
    TParse parse;
    parse.Set(KGlxListViewPluginResource, &KDC_APP_RESOURCE_DIR, NULL);
    aResFile.Append(parse.FullName()); 

    CGlxResourceUtilities::GetResourceFilenameL(aResFile);
    }

//  End of File

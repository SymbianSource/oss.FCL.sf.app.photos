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
*  Description : Implementation of metadata view plugin
*
*/



// INCLUDE FILES

#include "glxmetadataviewplugin.h"

#include <glxcommandhandlerback.h>                     // For CGlxCommandHandlerBack
#include <glxcommandhandlerhelp.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>

#include "glxmetadataview.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxMetadataViewPlugin::CGlxMetadataViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMetadataViewPlugin* CGlxMetadataViewPlugin::NewL()
    {
    CGlxMetadataViewPlugin* self = new ( ELeave ) CGlxMetadataViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMetadataViewPlugin::~CGlxMetadataViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CGlxMetadataViewPlugin::ConstructViewLC()
    {
    /*Create view it self medialist provider ? */   
	CGlxMetadataView* view = CGlxMetadataView::NewLC(NULL);    
	
	// add command handlers
	view->AddCommandHandlerL(CGlxCommandHandlerBack::NewPreviousViewCommandHandlerL(view));
	
	TGlxHelpContext helpInfo;
	helpInfo.iBrowseContext = LGAL_HLP_DETAILS_VIEW;
	view->AddCommandHandlerL(CGlxCommandHandlerHelp::NewL(helpInfo)); 
    
    return view;
    }

//  End of File

/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Full screen view plugin
*
*/





// INCLUDE FILES
#include "shwslideshowviewplugin.h"

#include <glxlog.h>
#include <glxtracer.h>
#include <glxcommandhandlerback.h>

#include "shwslideshowview.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CShwSlideshowViewPlugin::CShwSlideshowViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CShwSlideshowViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CShwSlideshowViewPlugin* CShwSlideshowViewPlugin::NewL()
    {
    TRACER("CShwSlideshowViewPlugin::NewL");
    GLX_LOG_INFO("CShwSlideshowViewPlugin::NewL" );
    CShwSlideshowViewPlugin* self = new ( ELeave ) CShwSlideshowViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CShwSlideshowViewPlugin::~CShwSlideshowViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CShwSlideshowViewPlugin::ConstructViewLC()
    {
    TRACER("CShwSlideshowViewPlugin::ConstructViewLC");
    GLX_LOG_INFO("CShwSlideshowViewPlugin::ConstructViewLC" );
	CShwSlideshowView* view = CShwSlideshowView::NewLC();

	// add the back command handler to get us to previous view
    view->AddCommandHandlerL( 
    	CGlxCommandHandlerBack::NewPreviousViewCommandHandlerL( ) );
	
    return view;
    }

//  End of File

/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The slideshow settings dialog ecom plugin
 *
*/




//  CLASS HEADER
#include "shwslideshowsettingsplugin.h"


//  EXTERNAL INCLUDES
#include <AknDialog.h>  	 			// for CAknDialog
#include <mpxcustomcommandobserver.h>	// for MMPXCustomCommandObserver
#include <e32std.h>
#include <ecom/implementationproxy.h>

//  INTERNAL INCLUDES
#include <glxlog.h>
#include <glxtracer.h>
#include "shwslideshowsettingsdialog.h"

#include <shwsettingsdialog.rsg>

// -----------------------------------------------------------------------------
// C++ Constructor. 
// -----------------------------------------------------------------------------
inline CShwSlideshowSettingsPlugin::CShwSlideshowSettingsPlugin() 
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwSlideshowSettingsPlugin* CShwSlideshowSettingsPlugin::NewL()
	{
	TRACER("CShwSlideshowSettingsPlugin::NewL");
	GLX_LOG_INFO("CShwSlideshowSettingsPlugin::NewL");
	CShwSlideshowSettingsPlugin* self = new(ELeave) CShwSlideshowSettingsPlugin;
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CShwSlideshowSettingsPlugin::~CShwSlideshowSettingsPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Construct Avkon dialog.
// ---------------------------------------------------------------------------
CAknDialog* CShwSlideshowSettingsPlugin::ConstructDialogL()
    {
    TRACER("CShwSlideshowSettingsPlugin::ConstructDialogL");
    GLX_LOG_INFO("CShwSlideshowSettingsPlugin::ConstructDialogL");
    return CShwSlideshowSettingsDialog::NewL();
    }
    
// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Resource ID.
// ---------------------------------------------------------------------------
TInt CShwSlideshowSettingsPlugin::ResourceId()
    {
    TRACER("CShwSlideshowSettingsPlugin::ResourceId");
    GLX_LOG_INFO("CShwSlideshowSettingsPlugin::ResourceId");
    return R_SHW_SETTINGS_DLG; // Resource Id of settings dialog
    }

//  End of File

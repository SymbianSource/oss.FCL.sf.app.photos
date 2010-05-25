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
#include "glxfullscreenviewpluginbase.h"

#include <glxcommandhandleraddtocontainer.h>
#include <glxcommandhandleraiwassign.h>
#include <glxcommandhandleraiwedit.h>
#include <glxcommandhandlersave.h>
#include <glxcommandhandleraiwprintpreview.h>
#include <glxcommandhandleraiwshowmap.h>
#include <glxcommandhandleraiwshareonovi.h>
#include <glxcommandhandlerback.h>
#include <glxcommandhandlerdelete.h>
#include <glxcommandhandlerdetails.h>
#include <glxcommandhandlerremovefrom.h>
#include <glxcommandhandlerrotate.h>
#include <glxcommandhandlerhideui.h>
#include <glxcommandhandlerslideshow.h>
#include <glxcommandhandlermarking.h>
#include <glxcommandhandlerback.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <glxcommandhandlerupload.h>
#include <glxcommandhandlermoreinfo.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>
#include <glxcommandhandlersend.h>
#include <glxcommandhandlershowviaupnp.h>				//Command Handlers Upnp
#include <glxcommandhandlerhelp.h>
#include <glxcommandhandleraiwshowonmaphardkey.h>

#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include "glxfullscreenview.h"

/**
 * Start Delay for the periodic timer, in microseconds
 */
const TInt KPeriodicStartDelay = 500000; 

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxFullScreenViewPluginBase::CGlxFullScreenViewPluginBase()
    {
    TRACER("CGlxFullScreenViewPluginBase::CGlxFullScreenViewPluginBase()");
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxFullScreenViewPluginBase::~CGlxFullScreenViewPluginBase()
    {
    TRACER("CGlxFullScreenViewPluginBase::~CGlxFullScreenViewPluginBase()");
    // Do nothing
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }

    if (iPeriodic)
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
    }
// -----------------------------------------------------------------------------
// Callback from periodic timer
// -----------------------------------------------------------------------------
//
TInt CGlxFullScreenViewPluginBase::PeriodicCallback(TAny* aPtr )
    {
    static_cast< CGlxFullScreenViewPluginBase* >( aPtr )->PeriodicCallback();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Callback from periodic timer, non-static
// -----------------------------------------------------------------------------
//

inline void CGlxFullScreenViewPluginBase::PeriodicCallback()
    {
    TRAP_IGNORE(AddCommandHandlersL());
    iPeriodic->Cancel();
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
EXPORT_C CAknView* CGlxFullScreenViewPluginBase::ConstructViewLC()
    {
    GLX_LOG_INFO("=>CGlxFullScreenViewPluginBase::ConstructViewLC");
    
    TFileName resFile;
    GetResourceFilenameL(resFile);
    
    if(resFile.Length() > 0)
        {
        iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resFile);
        }

    CGlxFullScreenView* fullScreenView = CGlxFullScreenView::NewLC(this,
            iResourceIds, iViewUID);
    
    iFullScreenView = fullScreenView;
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerBack");
    iFullScreenView->AddCommandHandlerL(
                  CGlxCommandHandlerBack::NewPreviousViewCommandHandlerL());
      
    if(!iPeriodic)
        {
        iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
        }

    if ( !iPeriodic->IsActive() )
        {
        iPeriodic->Start( KPeriodicStartDelay, KMaxTInt, TCallBack( &PeriodicCallback,
                static_cast<TAny*>( this ) ) );
        }
    
    GLX_LOG_INFO("<=CGlxFullScreenViewPluginBase::ConstructViewLC");
    // Return on cleanup stack with ownership passed
    return fullScreenView;
    }

// ---------------------------------------------------------------------------
// Add the command handlers
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxFullScreenViewPluginBase::AddCommandHandlersL()
	{
	TRACER("CGlxFullScreenViewPluginBase::AddCommandHandlersL()");

	// Note that the order in which the command handlers are added determines
	// the order in which the toolbar displays the icons

	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());
	GLX_LOG_INFO( "Adding CGlxCommandHandlerSave" );
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerSave::NewL());

	GLX_LOG_INFO( "Adding CGlxCommandHandlerSlideshow" );
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerSlideshow::NewL(
			iFullScreenView, ETrue, ETrue, uiutilitiesrscfile));

	GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer");
	iFullScreenView->AddCommandHandlerL(
			CGlxCommandHandlerAddToContainer::NewAddToAlbumCommandHandlerL(
					iFullScreenView, EFalse, uiutilitiesrscfile));

	GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer");
	iFullScreenView->AddCommandHandlerL(
			CGlxCommandHandlerAddToContainer::NewAddToFavCommandHandlerL(
					iFullScreenView, EFalse, uiutilitiesrscfile));

	GLX_LOG_INFO("Adding CGlxCommandHandlerDetails");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerDetails::NewL(
			iFullScreenView, uiutilitiesrscfile));

	GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer");
	iFullScreenView->AddCommandHandlerL(
			CGlxCommandHandlerAddToContainer::NewAddToTagCommandHandlerL(
					iFullScreenView, EFalse, uiutilitiesrscfile));
	// The AIW service handlers 
	GLX_LOG_INFO("Adding CGlxCommandHandlerAiwAssign");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwAssign::NewL(
			iFullScreenView, iResourceIds.iMenuId, uiutilitiesrscfile));
	GLX_LOG_INFO("Adding CGlxCommandHandlerAiwEdit");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwEdit::NewL(
			iFullScreenView));
	GLX_LOG_INFO("Adding CGlxCommandHandlerAiwPrintPreview");
	iFullScreenView->AddCommandHandlerL(
			CGlxCommandHandlerAiwPrintPreview::NewL(iFullScreenView,
					iResourceIds.iMenuId, uiutilitiesrscfile));
	GLX_LOG_INFO("Adding CGlxCommandHandlerAiwShareOnOvi");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwShareOnOvi::NewL(
			iFullScreenView, iResourceIds.iMenuId, uiutilitiesrscfile));
	// End of AIW service handlers                            

    //Toolbar commands for all Fullscreen views. May also be present in Options menu.
	GLX_LOG_INFO("Adding CGlxCommandHandlerSend");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerSend::NewL(
			iFullScreenView, ETrue, uiutilitiesrscfile));

	GLX_LOG_INFO("Adding CGlxCommandHandlerDelete");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerDelete::NewL(
			iFullScreenView, EFalse, ETrue, uiutilitiesrscfile));
	GLX_LOG_INFO("Adding CGlxCommandHandlerUpload");
	iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerUpload::NewL(
			iFullScreenView, ETrue));

	}

// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxFullScreenViewPluginBase::GetResourceFilenameL(TFileName& aResFile)
    {
    TRACER("CGlxFullScreenViewPluginBase::GetResourceFilenameL()");
    
    aResFile = KNullDesC;
    }

//  End of File

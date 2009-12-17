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
#include <glxcommandhandlervideoplayback.h>
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
    //GLX_ASSERT_DEBUG( aPtr != NULL, Panic( EGlxPanicLogicError ), 
    //    "Received null pointer for garbage collector" );
        
    // get "this" pointer
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
    
    //AddCommandHandlersL();
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
    
    GLX_LOG_INFO( "Adding CGlxCommandHandlerSave" );
            iFullScreenView->AddCommandHandlerL(
                            CGlxCommandHandlerSave::NewL( iFullScreenView, ETrue ) );
    
    GLX_LOG_INFO( "Adding CGlxCommandHandlerSlideshow" );
    iFullScreenView->AddCommandHandlerL(
                    CGlxCommandHandlerSlideshow::NewL( iFullScreenView, ETrue , ETrue ) );
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerDetails");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerDetails::
                                NewL(iFullScreenView));
    
//  GLX_LOG_INFO("Adding CGlxCommandHandlerRotate");                    
//  iFullScreenView->AddCommandHandlerL(                                           
//                  CGlxCommandHandlerRotate::NewL( iFullScreenView ) );
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAddToContainer::
            NewAddToAlbumCommandHandlerL(iFullScreenView, EFalse));                             

    GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAddToContainer::
            NewAddToFavCommandHandlerL(iFullScreenView, EFalse));                             

    GLX_LOG_INFO("Adding CGlxCommandHandlerDetails");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerDetails::
            NewL(iFullScreenView));

    GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAddToContainer::
                                NewAddToTagCommandHandlerL(iFullScreenView, EFalse));
    // The AIW service handlers 
    // ShowMap must be the first one                           
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwShowMap");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwShowMap::
                                NewL(iFullScreenView, iResourceIds.iMenuId));
    // Assign must be the second one
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwAssign");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwAssign::
                                NewL(iFullScreenView, iResourceIds.iMenuId));
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwEdit");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwEdit::
                                NewL(iFullScreenView, iResourceIds.iMenuId));
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwPrintPreview");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwPrintPreview::
                                NewL(iFullScreenView, iResourceIds.iMenuId));
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwShareOnOvi");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwShareOnOvi::
            NewL(iFullScreenView, iResourceIds.iMenuId));
    // End of AIW service handlers                            

    GLX_LOG_INFO("Adding CGlxCommandHandlerMoreInfo");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerMoreInfo::
                                NewL(iFullScreenView,EFalse));    				
    GLX_LOG_INFO("Adding CGlxCommandHandlerHideUi");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerHideUi::NewL());

	//Fix for error ID EVTY-7M87LF
	//@ Registration of Video Playback Command handler has to before UPnP.
	//@ else UPnP will not get play command once video playback command is consumed.
    GLX_LOG_INFO("Adding CGlxCommandHandlerVideoPlayback");
    iFullScreenView->AddCommandHandlerL(
                    CGlxCommandHandlerVideoPlayback::NewL( iFullScreenView ) );

	//Fix for error ID EVTY-7M87LF
	//@ Registration of UPnP Command handler has to be after video play back command
	//@ priorty of command handling is last registered in first server basis.
	//@ UPnP will get play command first everytime, where it'll check for UPnP active.
    //@ if UPnP is active it'll consume the event, else it'll allow other registered command to execute it.

    GLX_LOG_INFO("Adding CGlxCommandHandlerShowVisUpnp");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerShowViaUpnp::NewL(
            iFullScreenView,EFalse));

    //Toolbar commands for all Fullscreen views. May also be present in Options menu.
    GLX_LOG_INFO("Adding CGlxCommandHandlerSend");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerSend::NewL(
            iFullScreenView, ETrue));
    GLX_LOG_INFO("Adding CGlxCommandHandlerDelete");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerDelete::
            NewL(iFullScreenView, EFalse, ETrue));
    GLX_LOG_INFO("Adding CGlxCommandHandlerUpload");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerUpload::NewL(
            iFullScreenView, ETrue));

    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwShowMapHardKey");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerAiwShowMapHardKey::NewL(
            iFullScreenView, ETrue));

    GLX_LOG_INFO("Adding CGlxCommandHandlerRemoveFrom");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerRemoveFrom::
            NewL(iFullScreenView, EMPXTag));
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerRemoveFromFavourites");
    iFullScreenView->AddCommandHandlerL(CGlxCommandHandlerRemoveFrom::
            NewRemFromFavCommandHandlerL(iFullScreenView ));

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

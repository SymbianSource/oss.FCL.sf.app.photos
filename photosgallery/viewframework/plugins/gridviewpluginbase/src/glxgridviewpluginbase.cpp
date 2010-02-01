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
#include "glxgridviewpluginbase.h"

#include <bldvariant.hrh>
#include <glxcommandhandleraddtocontainer.h>
#include <glxcommandhandleraiwassign.h>
#include <glxcommandhandleraiwedit.h>
#include <glxcommandhandleraiwprintpreview.h>
#include <glxcommandhandleraiwshowmap.h>
#include <glxcommandhandleraiwshareonovi.h>
#include <glxcommandhandlerback.h>
#include <glxcommandhandlersend.h>
#include <glxcommandhandlercopytohomenetwork.h>
#include <glxcommandhandlerdelete.h>
#include <glxcommandhandlerhelp.h>
#include <glxcommandhandlerhideui.h>
#include <glxcommandhandlermarking.h>
#include <glxcommandhandlermoreinfo.h>
#include <glxcommandhandlerdetails.h>
#include <glxcommandhandlerremovefrom.h>
#include <glxcommandhandlershowviaupnp.h>				//Command Handlers Upnp
#include <glxcommandhandlerslideshow.h>
#include <glxcommandhandlerupload.h>
#include <glxcommandhandlervideoplayback.h>
#include <glxcommandhandleropen.h>
#include <glxtracer.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>
#include <glxcommandhandlerfilterimagesorvideos.h> // CGlxCommandHandlerFilterImagesOrVideos
#include <glxlog.h>
#include "glxgridview.h"

/**
 * @internal reviewed 08/02/2008 by Rhodri Byles
 */

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxGridViewPluginBase::CGlxGridViewPluginBase()
    {
    TRACER("CGlxGridViewPluginBase::CGlxGridViewPluginBase()");
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxGridViewPluginBase::~CGlxGridViewPluginBase()
    {
    TRACER("CGlxGridViewPluginBase::~CGlxGridViewPluginBase()");
    // Do nothing
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
EXPORT_C CAknView* CGlxGridViewPluginBase::ConstructViewLC()
    {
    GLX_LOG_INFO("=>CGlxGridViewPluginBase::ConstructViewLC");
    
    TFileName resFile;
    GetResourceFilenameL(resFile);
    
    if(resFile.Length() > 0)
        {
        iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resFile);
        }

    CGlxGridView* gridView = CGlxGridView::NewLC(this, iResourceIds, iViewUID);
    
    iGridView = gridView;
    
    AddCommandHandlersL();
    
    GLX_LOG_INFO("<=CGlxGridViewPluginBase::ConstructViewLC");
    // Return on cleanup stack with ownership passed
    return gridView;
    }

// ---------------------------------------------------------------------------
// Add the command handlers
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxGridViewPluginBase::AddCommandHandlersL()
    {
    TRACER("CGlxGridViewPluginBase::AddCommandHandlersL()");
#ifdef _DEBUG
    TTime startTime;
    GLX_LOG_INFO("CGlxGridViewPluginBase::AddCommandHandlersL()");	
    startTime.HomeTime();
#endif
        
    GLX_LOG_INFO( "Adding CGlxCommandHandlerSlideshow" );
    iGridView->AddCommandHandlerL(
                        CGlxCommandHandlerSlideshow::NewL( iGridView, ETrue , EFalse ) );
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerSend");
                iGridView->AddCommandHandlerL(CGlxCommandHandlerSend::NewL(iGridView, ETrue));
        
    GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer-Album");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAddToContainer::
                                NewAddToAlbumCommandHandlerL(iGridView, EFalse));   
								    
    GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer-Album SingleClick");
        iGridView->AddCommandHandlerL(CGlxCommandHandlerAddToContainer::
                                NewAddToAlbumSingleClickCommandHandlerL(iGridView, EFalse));  
    GLX_LOG_INFO("Adding CGlxCommandHandlerDetails");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerDetails::
                                NewL(iGridView));
                                
    GLX_LOG_INFO("Adding CGlxCommandHandlerOpen");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerOpen::
                                 NewL(iGridView));
                            
    GLX_LOG_INFO("Adding CGlxCommandHandlerDelete");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerDelete::
                                NewL(iGridView, EFalse, EFalse));
                                
    GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer-Tag");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAddToContainer::
                                NewAddToTagCommandHandlerL(iGridView, EFalse));
								
    GLX_LOG_INFO("Adding CGlxCommandHandlerAddToContainer-Tag Single Click");
        iGridView->AddCommandHandlerL(CGlxCommandHandlerAddToContainer::
                                NewAddToTagSingleClickCommandHandlerL(iGridView, EFalse));
    // The AIW service handlers 
    // ShowMap must be the first one                           
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwShowMap");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAiwShowMap::
                                NewL(iGridView, iResourceIds.iMenuId));
    // Assign must be the second one
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwAssign");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAiwAssign::
                                NewL(iGridView, iResourceIds.iMenuId));
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwEdit");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAiwEdit::
                                NewL(iGridView));
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwEdit-Single Click");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAiwEdit::
                                NewL(iGridView, ETrue));

    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwPrintPreview");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAiwPrintPreview::
                                NewL(iGridView, iResourceIds.iMenuId));
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwShareOnOvi");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerAiwShareOnOvi::
                                NewL(iGridView, iResourceIds.iMenuId));    
    // End of AIW service handlers                            
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerMarking");
    iGridView->AddCommandHandlerL( CGlxCommandHandlerMarking::NewL(
                                            iGridView, ETrue ) );
                                                            
    GLX_LOG_INFO("Adding CGlxCommandHandlerBack");
    iGridView->AddCommandHandlerL(
                    CGlxCommandHandlerBack::NewBackCommandHandlerL());
    
    GLX_LOG_INFO("Adding CGlxCommandHandlerMoreInfo");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerMoreInfo::
                                NewL(iGridView,EFalse));    				
    GLX_LOG_INFO("Adding CGlxCommandHandlerHideUi");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerHideUi::NewL());

#ifdef GLX_GRID_VIEW_IMAGES_OR_VIDEOS_ONLY_FILTERING
    iGridView->AddCommandHandlerL( CGlxCommandHandlerFilterImagesOrVideos::NewL( iGridView ) );
#endif
    GLX_LOG_INFO("Adding CGlxCommandHandlerCopyToHomeNetwork");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerCopyToHomeNetwork::NewL(*iGridView));
    
    //GLX_LOG_INFO("Adding CGlxCommandHandlerMarking");
    //iGridView->AddCommandHandlerL( CGlxCommandHandlerMarking::NewL(iGridView/*, *iGridView, ETrue*/ ) );
    GLX_LOG_INFO("Adding CGlxCommandHandlerUpload");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerUpload::NewL(iGridView, ETrue));    
    
	//Fix for error ID EVTY-7M87LF
	//@ Registration of Video Playback Command handler has to before UPnP.
	//@ else UPnP will not get play command once video playback command is consumed.
	GLX_LOG_INFO("Adding CGlxCommandHandlerVideoPlayback");
    iGridView->AddCommandHandlerL(
                        CGlxCommandHandlerVideoPlayback::NewL( iGridView ) );

	//Fix for error ID EVTY-7M87LF
	//@ Registration of UPnP Command handler has to be after video play back command
	//@ priorty of command handling is last registered in first server basis.
	//@ UPnP will get play command first everytime, where it'll check for UPnP active.
	//@ if UPnP is active it'll consume the event, else it'll allow other registered command to execute it.
    GLX_LOG_INFO("Adding CGlxCommandHandlerShowVisUpnp");
    iGridView->AddCommandHandlerL(CGlxCommandHandlerShowViaUpnp::NewL(iGridView, ETrue));

#ifdef _DEBUG
    TTime stopTime;
    stopTime.HomeTime();
    GLX_DEBUG2("CGlxGridViewPluginBase::AddCommandHandlersL() took <%d> us", 
                    (TInt)stopTime.MicroSecondsFrom(startTime).Int64());
#endif    
    }

// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxGridViewPluginBase::GetResourceFilenameL(TFileName& aResFile)
    {
    TRACER("CGlxGridViewPluginBase::GetResourceFilenameL()");
    
    aResFile = KNullDesC;
    }

//  End of File

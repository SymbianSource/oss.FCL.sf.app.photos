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
* Description:    Upnp Command Handlers.
*
*/




#include "glxcommandhandlershowviaupnp.h"

//Includes
#include <eikmenup.h>
#include <glxcommandhandlers.hrh>
#include <glxtracer.h>
#include <glxlog.h>

#include "glxupnprenderer.h"     
#include "glxupnprendererdefs.h"

//-----------------------------------------------------------------------------------------
// Two phased Constructor
//-----------------------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerShowViaUpnp* CGlxCommandHandlerShowViaUpnp::
    NewL(MGlxMediaListProvider* aMediaListProvider, TBool aUseHomeNetworkCascadeMenu)
    {  
    TRACER( "CGlxCommandHandlerShowViaUpnp::NewL");
    CGlxCommandHandlerShowViaUpnp* self =new(ELeave) 
              CGlxCommandHandlerShowViaUpnp(aMediaListProvider, aUseHomeNetworkCascadeMenu);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
  
//-----------------------------------------------------------------------------------------
//  First Phase Constructor
//-----------------------------------------------------------------------------------------  
CGlxCommandHandlerShowViaUpnp::CGlxCommandHandlerShowViaUpnp
    (MGlxMediaListProvider* aMediaListProvider, TBool aUseHomeNetworkCascadeMenu)
: CGlxMediaListCommandHandler(aMediaListProvider), iUseHomeNetworkCascadeMenu(aUseHomeNetworkCascadeMenu)
    {
    TRACER(" CGlxCommandHandlerShowViaUpnp::CGlxCommandHandlerShowViaUpnp");
    }
  
//-----------------------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerShowViaUpnp::~CGlxCommandHandlerShowViaUpnp()
    {
    //Do Nothing  
    }  
  
//-----------------------------------------------------------------------------------------
// ConstructL
//-----------------------------------------------------------------------------------------  
  
void CGlxCommandHandlerShowViaUpnp::ConstructL()  
    {
    TRACER(" CGlxCommandHandlerShowViaUpnp::ConstructL");
    AddUpnpCommandL();   
    }

//-----------------------------------------------------------------------------------------
// Add the UPnP commands to the CommandInfo
//-----------------------------------------------------------------------------------------  
void CGlxCommandHandlerShowViaUpnp::AddUpnpCommandL()
    {
    // Add the Show on Home Network Command        
    TCommandInfo infoStartShowing(EGlxShowViaUpnp);                          
    infoStartShowing.iMinSelectionLength = 1;    
    infoStartShowing.iMaxSelectionLength = 1;
    // Fix for ABAI-7C3J5K, the menu item should be enabled for DRM items
    infoStartShowing.iDisallowDRM = EFalse;    
    AddCommandL(infoStartShowing) ;

    // We need two versions of the start and stop showing command if iUseHomeNetworkCascadeMenu is true
    if (iUseHomeNetworkCascadeMenu)
        {
        //Add the home network cascaded menu (sub-menu)
        TCommandInfo infoHomeNetworkSubmenu(EGlxHomeNetworkSubmenu);
        infoHomeNetworkSubmenu.iMinSelectionLength = 1;
        infoHomeNetworkSubmenu.iMaxSelectionLength = KMaxTInt;
        // Show in Grid View
        infoHomeNetworkSubmenu.iViewingState = TCommandInfo::EViewingStateBrowse;
        AddCommandL(infoHomeNetworkSubmenu);
       
        // Add the Show on Home Network Command (submenu version)
        TCommandInfo infoStartShowingSubmenuVersion(EGlxShowViaUpnpSubmenuVersion);                          
        infoStartShowingSubmenuVersion.iMinSelectionLength = 1;    
        infoStartShowingSubmenuVersion.iMaxSelectionLength = 1;
        infoStartShowingSubmenuVersion.iDisallowDRM = ETrue;
        AddCommandL(infoStartShowingSubmenuVersion);
           
        //Add the Stop Showing Command (submenu version)
        TCommandInfo infoStopShowingSubmenuVersion(EGlxStopShowingSubmenuVersion); 
        AddCommandL(infoStopShowingSubmenuVersion);   
        }
    
    //Add the Stop Showing Command
    TCommandInfo infoStopShowing(EGlxStopShowing);
    //By default the MinSelectionLength and MaxSelectionLength will be
    //set to zero and KMaxTInt respectively    
    AddCommandL(infoStopShowing);
 
    // Get notified when we switch to 'browsing'
    TCommandInfo browseInfo(EGlxCmdStateBrowse);
    browseInfo.iMinSelectionLength = 0;
    browseInfo.iMaxSelectionLength = KMaxTInt;
    // Filter out static items
    AddCommandL(browseInfo);
    
    // Get notified when we switch to 'viewing' (fullscreen)
    TCommandInfo viewInfo(EGlxCmdStateView);
    viewInfo.iMinSelectionLength = 0;
    viewInfo.iMaxSelectionLength = KMaxTInt;
    // Filter out static items
    AddCommandL(viewInfo);
    
    //Add the Play command here as we need to add some filters    
    TCommandInfo infoplay(EGlxCmdPlay);              
    // filter out everything except videos
    infoplay.iCategoryFilter = EMPXVideo;
    infoplay.iCategoryRule = TCommandInfo::ERequireAll;                    
    infoplay.iMinSelectionLength = 1;
    infoplay.iMaxSelectionLength = 1;    
    AddCommandL(infoplay);       
    }

//-----------------------------------------------------------------------------------------
// Executing the command handler
//-----------------------------------------------------------------------------------------
  
TBool CGlxCommandHandlerShowViaUpnp::DoExecuteL(TInt aCommandId, MGlxMediaList& /*aList*/)
    {
    TRACER( "CGlxCommandHandlerShowViaUpnp::DoExecuteL");

    TBool consumed = ETrue;    
      
    switch(aCommandId)
        {
        case EGlxShowViaUpnpSubmenuVersion: // fallthrough
        case EGlxShowViaUpnp:
            {    
            GLX_LOG_INFO("DoExecuteL -ShowViaUPNP");  
            
            GlxUpnpRenderer:: StartShowingL();              
            } 
            break;
        case EGlxStopShowingSubmenuVersion: // fallthrough
        case EGlxStopShowing:
            {
            GLX_LOG_INFO("DoExecuteL -StopShowing");                      
            GlxUpnpRenderer::ShowStopL();
            GlxUpnpRenderer::StopShowingL();                  
            }
            break;
            
        case EGlxCmdPlay:
            {            
            GLX_LOG_INFO("DoExecuteL -UPnpPlay");  
			//@ Fix for EVTY-7M87LF
            if ( NGlxUpnpRenderer::EActive == GlxUpnpRenderer::Status() )
	            {
                GlxUpnpRenderer::ShowVideoL();
	            }
            else
	            {
        		consumed = EFalse;
	            }
            }
            break;           
        case EGlxCmdStateBrowse:
        case EGlxCmdStateView:
            {
            GLX_LOG_INFO("DoExecuteL -CmdStateBrowse / CmdStateView");  
            iFullScreen = (aCommandId == EGlxCmdStateView);
            consumed = EFalse;
            }
            break;
        default:
            {
            consumed = EFalse;  //Command is not Executed                        
            }
            break;
         }
     return consumed;     
     }

//-----------------------------------------------------------------------------------------
// Disable the command handler
//-----------------------------------------------------------------------------------------
TBool CGlxCommandHandlerShowViaUpnp::DoIsDisabled(TInt aCommandId, MGlxMediaList& /*aList*/) const
    {
    TRACER("CGlxCommandHandlerShowViaUpnp::DoIsDisabled");    

    TBool disabled = ETrue;
    NGlxUpnpRenderer::TStatus rendererStatus = GlxUpnpRenderer::Status();
    
    switch(aCommandId)
        {
        case EGlxShowViaUpnpSubmenuVersion:
            {
            GLX_LOG_INFO("DoIsDisabled -ShowViaUpnpSubmenuVersion");
          
            disabled = !(rendererStatus == NGlxUpnpRenderer::EAvailableNotActive && iUseHomeNetworkCascadeMenu);
            }
        break;
        case EGlxShowViaUpnp:
            {    
            GLX_LOG_INFO("DoIsDisabled -ShowViaUpnp");
                                       
            //If the access point is set and renderer is not active and we are in full screen or we are not using the cascaded menu
            disabled = !(rendererStatus == NGlxUpnpRenderer::EAvailableNotActive && (iFullScreen || !iUseHomeNetworkCascadeMenu));                             
            }                     
        break;
        case EGlxStopShowingSubmenuVersion:
            {
            GLX_LOG_INFO("DoIsDisabled - StopShowingSubmenuVersion");
            //If the renderer device is showing the image/video 
            disabled = !(rendererStatus == NGlxUpnpRenderer::EActive);   
            }
        break;
        case EGlxStopShowing:
            {
            GLX_LOG_INFO("DoIsDisabled -StopShowing");
            
            disabled = !(rendererStatus == NGlxUpnpRenderer::EActive && 
                    (!iUseHomeNetworkCascadeMenu || iFullScreen || (!iFullScreen && SelectionLength() == 0)));
            }
        break;  
        case EGlxCmdPlay:
            {
            GLX_LOG_INFO("DoIsDisabled -UPNPPlay");
			//@ Fix for EVTY-7M87LF
            // If the focussed item is video file,
			// Play command should be visible to play either in device or renderer if active.
            disabled = EFalse;
            }
        break;
        case EGlxHomeNetworkSubmenu:
            {
            GLX_LOG_INFO("DoIsDisabled - HomeNetworkSubmenu");
            
            disabled = (rendererStatus == NGlxUpnpRenderer::ENotAvailable && !iFullScreen);
            }
        break;
        case EGlxCmdStateBrowse:
        case EGlxCmdStateView:
            {
            GLX_LOG_INFO("DoIsDisabled - CmdStateBrowse / CmdStateView");
            disabled = EFalse;
            }
        default: 
            {            
            GLX_LOG_WARNING("DoIsDisabled -UnknownCommandHandler");
            }
        break;
        }
    return disabled;
    }

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
* Description:    Interface class
*
*/




//Includes

#include "glxupnprenderer.h"    

#include <glxassert.h>
#include <glxcommandhandlers.hrh>
#include <glxtracer.h>
#include <eikenv.h>
#include <eikappui.h>
#include "glxupnprendererimpl.h"


/// Smart pointer that stores the renderer pointer. 
/// Static data, so the object is destroyed when dll is unloaded.
/// The UPnP renderer needs to be stored as static data, to allow the state of the rendering
/// to be stored across multiple view switches

TGlxPointer<CGlxUpnpRendererImpl> gUpnpRendererImpl;
    
//-----------------------------------------------------------------------------------------
// Start Showing the Image/Video
//-----------------------------------------------------------------------------------------
    
void GlxUpnpRenderer::StartShowingL()
    {
    TRACER("CGlxUpnpRenderer::StartShowingL()");
    
    GLX_ASSERT_DEBUG ( !gUpnpRendererImpl.Ptr(), Panic( EGlxPanicIllegalState), "Start showing called when showing was already started" );
    gUpnpRendererImpl = CGlxUpnpRendererImpl::NewL(); // Smart pointer deletes existing instance if any  

    // Send state changed command
	ChangeCommandStateL();
    }

//-----------------------------------------------------------------------------------------
// Stop Showing the Image/Video
//-----------------------------------------------------------------------------------------

EXPORT_C void GlxUpnpRenderer:: StopShowingL()
    {
    TRACER("CGlxUpnpRenderer::StopShowingL()");
    if(gUpnpRendererImpl.Ptr()) 
      {
	    ChangeCommandStateL();
	  }
    }
//-----------------------------------------------------------------------------------------
//  Inform the clients that the UPnP State has changed
//-----------------------------------------------------------------------------------------

 void GlxUpnpRenderer::ChangeCommandStateL()
 	{
    TRACER("void GlxUpnpRenderer::ChangeCommandStateL()");
    // Send state changed command
    // This class does not have access to a CEikonEnv and hence 
    // pls ignore the code scanner warning - Using CEikonEnv::Static
    MEikCommandObserver* commandObserver = CEikonEnv::Static()->EikAppUi();
    commandObserver->ProcessCommandL( EGlxCmdShowViaUpnpStateChanged );    
    }
 
//-----------------------------------------------------------------------------------------
// Get the Status of the UPNP
//-----------------------------------------------------------------------------------------

EXPORT_C NGlxUpnpRenderer::TStatus GlxUpnpRenderer::Status() 
    {
    TRACER("CGlxUpnpRenderer::TStatus CGlxUpnpRenderer::UpnpStatus() ");
    
    NGlxUpnpRenderer::TStatus status =NGlxUpnpRenderer::ENotAvailable;    
    
    if(gUpnpRendererImpl.Ptr())
        {
        //renderer object is created, so it is active
        status = NGlxUpnpRenderer::EActive;
        NGlxUpnpRenderer::TErrorStatus upnpStatus = gUpnpRendererImpl->UpnpErrorStatus();
        
        if(upnpStatus == NGlxUpnpRenderer::EUpnpKErrDisconnected)
        	{
        	GLX_LOG_INFO("WLan disconnected");        	      	        	
        	status = NGlxUpnpRenderer::EAvailableNotActive;	
        	gUpnpRendererImpl = NULL;             	
        	}           	
        }      
    else
        {
        // renderer is either not available or available and not active
        // renderer is available but not active if it is supported
        // in the current device
        if ( CGlxUpnpRendererImpl::IsSupported() )
            {
            status = NGlxUpnpRenderer::EAvailableNotActive;
            }
        // else the renderer is not available
        }	
    return status;
    }     

//-----------------------------------------------------------------------------------------
// Show Video via UPNP
//-----------------------------------------------------------------------------------------

void GlxUpnpRenderer::ShowVideoL()
    {
    TRACER("CGlxUpnpRenderer::ShowVideoL()");
    //If the focussed item is video
    gUpnpRendererImpl->UpnpShowVideoL();
    }
//-----------------------------------------------------------------------------------------
// Stop the Show via UPNP
//-----------------------------------------------------------------------------------------

void GlxUpnpRenderer::ShowStopL()
{
TRACER("CGlxUpnpRenderer::ShowVideoL()");
if(gUpnpRendererImpl.Ptr())
 {
  gUpnpRendererImpl = NULL;     
 }
}


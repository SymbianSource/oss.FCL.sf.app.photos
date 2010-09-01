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




#ifndef GLXUPNPRENDERER_H__
#define GLXUPNPRENDERER_H__

#include "glxupnprendererdefs.h"

// INCLUDES
#include <e32def.h>
#include <e32base.h>

#include <glxpointer.h>

// CLASS DECLARATION

/**   
*  Renderer a fully static class, acts as an interface class 
*
*  @lib glxupnpcommandhandler.lib
*/
   
NONSHARABLE_CLASS( GlxUpnpRenderer ) 
    {
     
public:
    
    /**
    *  Show Video Via UPNP
    */    
    static void ShowVideoL();    
    
    /**
    * Start Showing the Image/Video
    */    
    static void StartShowingL();
    
    /**
    * Stop Showing the Image/Video
    */    
    IMPORT_C static void StopShowingL();  
     /**
    * Stop Showing the Image/Video
    */  
    static void ShowStopL();  
   
    /**
    * Check the Status of the Renderer
    * @return the renderer status
    */    
    IMPORT_C static NGlxUpnpRenderer::TStatus Status();     
     /**
     * Inform the clients that the UPnP State has changed   
     */
    static void ChangeCommandStateL(); 
       
    }; 
 
#endif


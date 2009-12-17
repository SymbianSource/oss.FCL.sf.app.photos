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
* Description:    Standard proxy of the ECOM plugin
*
*/





// INCLUDE FILES
#include <ecom/implementationproxy.h>
#include "glxfullscreenviewplugin.hrh"
#include "glxmainfullscreenviewplugin.h"
#include "glxalbumfullscreenviewplugin.h"
#include "glxcameraalbumfullscreenviewplugin.h"
#include "glxdownloadsfullscreenviewplugin.h"
#include "glxmonthsfullscreenviewplugin.h"
#include "glxtagfullscreenviewplugin.h"
#include "glximageviewerfullscreenviewplugin.h"

#if ( !defined IMPLEMENTATION_PROXY_ENTRY )
typedef TAny* TProxyNewLPtr;
#define IMPLEMENTATION_PROXY_ENTRY( aUid,aFuncPtr ) { { aUid }, ( TProxyNewLPtr )( aFuncPtr ) }
#endif


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// The list of implementations
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    { 
    IMPLEMENTATION_PROXY_ENTRY( 
        KGlxMainFullScreenViewImplementationId, 
        CGlxMainFullScreenViewPlugin::NewL ),

    IMPLEMENTATION_PROXY_ENTRY( 
        KGlxAlbumFullScreenViewImplementationId, 
        CGlxAlbumFullScreenViewPlugin::NewL ),
        
    IMPLEMENTATION_PROXY_ENTRY( 
        KGlxCameraAlbumFullScreenViewImplementationId, 
        CGlxCameraAlbumFullScreenViewPlugin::NewL ),
         
    IMPLEMENTATION_PROXY_ENTRY( 
        KGlxDownloadsFullScreenViewImplementationId, 
        CGlxDownloadsFullScreenViewPlugin::NewL ), 
        
    IMPLEMENTATION_PROXY_ENTRY( 
        KGlxMonthsFullScreenViewImplementationId, 
        CGlxMonthsFullScreenViewPlugin::NewL ), 
        
    IMPLEMENTATION_PROXY_ENTRY( 
        KGlxTagFullScreenViewPluginImplementationUid, 
        CGlxTagFullScreenViewPlugin::NewL ),
    
    IMPLEMENTATION_PROXY_ENTRY( 
        KGlxImageViewerFullScreenViewImplementationId, 
        CGlxImageViewerScreenViewPlugin::NewL ), 
    };

// ---------------------------------------------------------------------------
// The proxy of implementations
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = 
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of File

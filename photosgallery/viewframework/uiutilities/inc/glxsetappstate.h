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
* Description:    Update PCFW with current application state
*
*/




/**
 * @internal reviewed 03/07/2007 by David Holland
 */


#ifndef GLXSETAPPSTATE_H
#define GLXSETAPPSTATE_H

#include <e32base.h>

// DATA TYPES
/**
*   Supported application states
*/
enum TGlxAppState
    {
    EGlxAppStateUndefined,
    EGlxInCarouselView, 
    EGlxInFullScreenView,
    EGlxInZoomedView,
    EGlxInVideoPlaybackView,
    EGlxInSlideshowView,
    EGlxInListView,         
    EGlxInTagBrowserView,
    EGlxInMapBrowserView,
    EGlxInTextEntry
    };

/**
 *  GlxSetAppState
 *
 *  Update PCFW with current application state
 *
 *  @lib glxuiutilities.lib
 */
class GlxSetAppState
    {
public:
    /**
      * Set Gallery KPCFWAppStatePSUid with current app state
      * @param aAppState new app state
      */
    IMPORT_C static void SetState(TGlxAppState aAppState);
    
    /**
      * Get current value of Gallery KPCFWAppStatePSUid
      * @return aAppState new app state
      */
    IMPORT_C static TGlxAppState AppState();
    };

#endif // GLXSETAPPSTATE_H
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

#include "glxsetappstate.h"

#include <glxlog.h>

///@todo use 'real' header when available in SDK
#include <glxpsstatesourceappstatedomainpskeys.h>
#include <e32property.h>

// -----------------------------------------------------------------------------
// SetState
// -----------------------------------------------------------------------------
//	
EXPORT_C void GlxSetAppState::SetState(TGlxAppState aAppState)
    {
    GLX_LOG_INFO1("GlxSetAppState::SetState state (%d)", aAppState);
    
    KPCFWAppStatePSGalleryState appState;
    
    TBool validAppState = ETrue;
    
    switch(aAppState)
        {
        case EGlxInCarouselView:
            {
            appState = GalleryInCarouselView;      
            }
            break;
        case EGlxInFullScreenView:
            {
            appState = GalleryInFullScreenView;  
            }
            break;
        case EGlxInZoomedView:
            {
            appState = GalleryInZoomedView;  
            }
            break;
        case EGlxInVideoPlaybackView:
            {
            appState = GalleryInVideoPlaybackView;  
            }
            break;
        case EGlxInSlideshowView:
            {
            appState = GalleryInSlideshowView;  
            }
            break;
        case EGlxInListView:
            {
            appState = GalleryInListView;  
            }
            break;		
        case EGlxInTagBrowserView:
            {
            appState = GalleryInTagBrowserView;  
            }
            break;
        case EGlxInMapBrowserView:
            {
            appState = GalleryInMapBrowserView;  
            }
            break;
        case EGlxInTextEntry:
            {
            appState = GalleryInTextEntry;
            }
            break;
        default:
            {
            validAppState = EFalse; 
            }
            break;    
        }
         
    if(validAppState)
        {
        // ignore any error
        RProperty::Set(KPCFWAppStatePSUid, KPCFWAppStatePSGallery, appState);
        }
    }

// -----------------------------------------------------------------------------
// AppState
// -----------------------------------------------------------------------------
//    
EXPORT_C TGlxAppState GlxSetAppState::AppState()
    {
    TGlxAppState appState = EGlxAppStateUndefined;
    TInt pcfwState;

    TInt err = 
        RProperty::Get(KPCFWAppStatePSUid, KPCFWAppStatePSGallery, pcfwState);
    
    if(err == KErrNone)
        {
         switch(pcfwState)
            {
            case GalleryInCarouselView:
                {
                appState = EGlxInCarouselView;      
                }
                break;
            case GalleryInFullScreenView:
                {
                appState =  EGlxInFullScreenView;  
                }
                break;
            case GalleryInZoomedView:
                {
                appState = EGlxInZoomedView;  
                }
                break;
            case GalleryInVideoPlaybackView:
                {
                appState = EGlxInVideoPlaybackView;  
                }
                break;
            case GalleryInSlideshowView:
                {
                appState = EGlxInSlideshowView;  
                }
                break;
            case GalleryInListView:
                {
                appState = EGlxInListView;  
                }
                break;		
            case GalleryInTagBrowserView:
                {
                appState = EGlxInTagBrowserView;  
                }
                break;
            case GalleryInMapBrowserView:
                {
                appState = EGlxInMapBrowserView;  
                }
                break;
            case GalleryInTextEntry:
                {
                appState = EGlxInTextEntry;
                }
                break;
            default:

                break;    
            }
        }
    GLX_LOG_INFO1("GlxSetAppState::AppState state is (%d)", appState);
    return appState;
    }

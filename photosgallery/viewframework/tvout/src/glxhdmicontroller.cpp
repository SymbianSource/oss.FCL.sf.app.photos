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
* Description:    
*
*/

#include <w32std.h>
#include <alf/alfutil.h>
#include <glxtracer.h>
#include <glxlog.h>
#include "glxtv.h"                      // for CGlxTv
#include "glxhdmicontainer.h"
#include "glxhdmisurfaceupdater.h"

#include "glxhdmicontroller.h"

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
EXPORT_C CGlxHdmiController* CGlxHdmiController::NewL()
    {
    TRACER("CGlxHdmiController* CGlxHdmiController::NewL()");
    CGlxHdmiController* self = new (ELeave) CGlxHdmiController();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// destructor 
// -----------------------------------------------------------------------------
EXPORT_C CGlxHdmiController::~CGlxHdmiController()
    {
    TRACER("CGlxHdmiController::~CGlxHdmiController()");
    DestroySurfaceUpdater();
    DestroyContainer();
    delete iStoredImagePath;
    iStoredImagePath = NULL;
    if (iFsBitmap)
        {
        delete iFsBitmap;
        iFsBitmap = NULL;
        }
    if(iGlxTvOut)
		{
        delete iGlxTvOut;
		}
    }

// -----------------------------------------------------------------------------
// Setting an Image Path 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::SetImageL(const TDesC& aImageFile, CFbsBitmap* aFsBitmap,
                                             TBool aStore)
    {
    TRACER("CGlxHdmiController::SetImageL()");
    if (aFsBitmap == NULL || !aImageFile.Length())
        {
        GLX_LOG_INFO("CGlxHdmiController::SetImageL() - NULL Uri");
        return;
        }
    if (aStore)
        {
        iIsImageSupported = ETrue;
        StoreImageInfoL( aImageFile, aFsBitmap );
        }
    if (iGlxTvOut->IsHDMIConnected())
        {
        iPhotosForeground = ETrue;         // the image should be in posting mode
        GLX_LOG_INFO("CGlxHdmiController::SetImageL() - 2");
        // do not close the surface , use the same surface instead.
        // Call a function to pass imagefile
        if (!iHdmiContainer)
            {            
            CreateHdmiContainerL(); 
            }            
        if (!iSurfaceUpdater)
            {
            // This case would come when surface updater is not created at the first instance and also
            // it satisfies the 720p condition                
            CreateSurfaceUpdaterL(aImageFile);
            }
        else
            {
            GLX_LOG_INFO("CGlxHdmiController::SetImageL() - 3");
            iSurfaceUpdater->UpdateNewImageL(aImageFile, aFsBitmap);
            }
        iHdmiContainer->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// ItemNotSupported 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ItemNotSupported()
    {
    TRACER("CGlxHdmiController::IsVideo()");
    iIsImageSupported = EFalse;
    if (iGlxTvOut->IsHDMIConnected())
        {
        DestroySurfaceUpdater();
        }
    }


// -----------------------------------------------------------------------------
// ActivateZoom 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ActivateZoom(TBool aAutoZoomOut)
    {
    TRACER("CGlxHdmiController::ActivateZoom()");
    if (iGlxTvOut->IsHDMIConnected() && iSurfaceUpdater)
        {
        iSurfaceUpdater->ActivateZoom(aAutoZoomOut);
        }
    }

// -----------------------------------------------------------------------------
// DeactivateZoom 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::DeactivateZoom()
    {
    TRACER("CGlxHdmiController::DeactivateZoom()");
    if (iGlxTvOut->IsHDMIConnected() && iSurfaceUpdater)
        {
        iSurfaceUpdater->DeactivateZoom();
        }
    }

// -----------------------------------------------------------------------------
// ShiftToCloningMode 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ShiftToCloningMode()
    {
    TRACER("CGlxHdmiController::ShiftToCloningMode()");
    iPhotosForeground = EFalse;
    // Shift to cloning only if HDMI is connected and surfaceupdater available.
    if (iGlxTvOut->IsHDMIConnected() && iSurfaceUpdater)
        {
        iSurfaceUpdater->ShiftToCloningMode();
        }
    }

// -----------------------------------------------------------------------------
// ShiftToPostingMode 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ShiftToPostingMode()
    {
    TRACER("CGlxHdmiController::ShiftToPostingMode()");
    iPhotosForeground = ETrue;
    if (iGlxTvOut->IsHDMIConnected())
        {
        if (!iSurfaceUpdater)
            {
            GLX_LOG_INFO("CGlxHdmiController::ShiftToPostingMode() - 1");
            // This case would come when HDMI connected, TvOut /headphones being connected
            // and then it shows a popup of "microphone connected" 
            // thus Background - Foreground when headphones connected during HDMI connected
            if (iFsBitmap == NULL || !iStoredImagePath->Length())
                {
                GLX_LOG_INFO("CGlxHdmiController::ShiftToPostingMode() - NULL Uri");
                return;
                }
            SetImageL(iStoredImagePath->Des(), iFsBitmap, EFalse);
            }
        else
            {
            GLX_LOG_INFO("CGlxHdmiController::ShiftToPostingMode() - 2");
            iSurfaceUpdater->ShiftToPostingMode();
            }
        }
    }

// -----------------------------------------------------------------------------
// IsHDMIConnected 
// -----------------------------------------------------------------------------
EXPORT_C TBool CGlxHdmiController::IsHDMIConnected()
    {
    TRACER("CGlxHdmiController::IsHDMIConnected()");
    return iGlxTvOut->IsHDMIConnected(); 
    }


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CGlxHdmiController::CGlxHdmiController():
            iFsBitmap(NULL),
            iStoredImagePath(NULL),
            iPhotosForeground(EFalse)
    {
    TRACER("CGlxHdmiController::CGlxHdmiController()");
    // Implement nothing here
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::ConstructL()
    {
    TRACER("CGlxHdmiController::ConstructL()");
    iGlxTvOut = CGlxTv::NewL(*this);
    }

// -----------------------------------------------------------------------------
// DestroyContainer 
// -----------------------------------------------------------------------------
void CGlxHdmiController::DestroyContainer()
    {
    TRACER("CGlxHdmiController::DestroyContainer()");
    if (iHdmiContainer)
        {
        GLX_LOG_INFO("CGlxHdmiController::DestroyHdmi() - deleting iHdmiContainer 1");
        delete iHdmiContainer;
        iHdmiContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// DestroySurfaceUpdater 
// -----------------------------------------------------------------------------
void CGlxHdmiController::DestroySurfaceUpdater()
    {
    TRACER("CGlxHdmiController::DestroySurfaceUpdater()");
    if (iSurfaceUpdater)
        {
        delete iSurfaceUpdater;
        iSurfaceUpdater = NULL;
        }    
    }

// -----------------------------------------------------------------------------
// CreateHdmiContainerL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::CreateHdmiContainerL()
    {
    TRACER("CGlxHdmiController::CreateHdmiContainer()");
    TRect rect = AlfUtil::ScreenSize();
    iHdmiContainer = CGlxHdmiContainer::NewL(rect);
    }

// -----------------------------------------------------------------------------
// CreateSurfaceUpdaterL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::CreateSurfaceUpdaterL(const TDesC& aImageFile)
    {
    TRACER("CGlxHdmiController::CreateSurfaceUpdater()");
    RWindow* window = iHdmiContainer->GetWindow();
    iSurfaceUpdater = CGlxHdmiSurfaceUpdater::NewL(window, aImageFile, iFsBitmap,
                                                    iHdmiContainer);
    iHdmiContainer->DrawNow();
    }

// -----------------------------------------------------------------------------
// StoreImageInfoL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::StoreImageInfoL(const TDesC& aImageFile, CFbsBitmap* aFsBitmap)
    {
    TRACER("CGlxHdmiController::StoreImageInfoL()");
    if(iStoredImagePath)
        {
        delete iStoredImagePath;
        iStoredImagePath = NULL;
        }
    if (iFsBitmap)
        {
        delete iFsBitmap;
        iFsBitmap = NULL;
        }
    iStoredImagePath = aImageFile.AllocL();
    iFsBitmap = new (ELeave) CFbsBitmap;
    iFsBitmap->Duplicate(aFsBitmap->Handle());
    }

// -----------------------------------------------------------------------------
// HandleTvStatusChangedL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::HandleTvStatusChangedL( TTvChangeType aChangeType )
    {
    TRACER("CGlxHdmiController::HandleTvStatusChangedL()");
    if ( aChangeType == ETvConnectionChanged )          
        {
        if ( iGlxTvOut->IsHDMIConnected() && iGlxTvOut->IsConnected() && iSurfaceUpdater)
            {
            GLX_LOG_INFO("CGlxHdmiController::HandleTvStatusChangedL() - HDMI and TV Connected");
            // Do nothing , as this means HDMI is already connected and headset/tv cable connected
            // meaning we shouldnt destroy HDMI and neither have to create surface updater.
            return;
            }
        else if ( iGlxTvOut->IsHDMIConnected() && iIsImageSupported && iPhotosForeground)
            {
            GLX_LOG_INFO("CGlxHdmiController::HandleTvStatusChangedL() - HDMI Connected");
            // Calling SetImageL() with appropriate parameters
            SetImageL(iStoredImagePath->Des(), iFsBitmap, EFalse);
            }
        else
            {
            GLX_LOG_INFO3("CGlxHdmiController::HandleTvStatusChangedL() iIsImageSupported=%d, iGlxTvOut->IsHDMIConnected()=%d, iIsPostingMode=%d", 
                    iIsImageSupported,iGlxTvOut->IsHDMIConnected(),iPhotosForeground);
            // if it gets disconnected, destroy the surface 
            GLX_LOG_INFO("CGlxHdmiController::HandleTvStatusChangedL() - HDMI Not Connected");
            DestroySurfaceUpdater();
            }
        }
    }


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
EXPORT_C CGlxHdmiController* CGlxHdmiController::NewL(const TDesC& aImageFile)
    {
    TRACER("CGlxHdmiController* CGlxHdmiController::NewL()");
    CGlxHdmiController* self = new (ELeave) CGlxHdmiController(aImageFile);
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
    if(iGlxTvOut)
		{
        delete iGlxTvOut;
		}
    }

// -----------------------------------------------------------------------------
// Setting an Image Path 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::SetImageL(const TDesC& aImageFile,
        TSize aImageDimensions, TInt aFrameCount, TBool aStore)
    {
    TRACER("CGlxHdmiController::SetImageL()");
    if (aStore)
        {
        iImageSupported = ETrue;
        StoreImageInfoL(aImageFile, aImageDimensions, aFrameCount);
        }
    if (iGlxTvOut->IsHDMIConnected())
        {
            GLX_LOG_INFO("CGlxHdmiController::SetImageL() - 2");
            // do not close the surface , use the same surface instead.
            // Call a function to pass imagefile, imagedimension, framecount
            if (!iHdmiContainer)
                {            
                CreateHdmiContainerL(); 
                }            
            if (!iSurfaceUpdater)
                {
                // This case would come when surface updater is not created at the first instance and also
                // it satisfies the 720p condition                
                CreateSurfaceUpdaterL(aImageFile, aImageDimensions, aFrameCount);
                }
            else
                {
            GLX_LOG_INFO("CGlxHdmiController::SetImageL() - 3");
            iSurfaceUpdater->UpdateNewImageL(aImageFile, aFrameCount,aImageDimensions);
            }
        iHdmiContainer->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// IsVideo 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ItemNotSupported()
    {
    TRACER("CGlxHdmiController::IsVideo()");
    iImageSupported = EFalse;
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
    if (iGlxTvOut->IsHDMIConnected())
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
    if (iGlxTvOut->IsHDMIConnected())
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
    if (iGlxTvOut->IsHDMIConnected() && iSurfaceUpdater)
        {
        iSurfaceUpdater->ShiftToPostingMode();
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CGlxHdmiController::CGlxHdmiController(const TDesC& aImageFile):
        iImagePath(aImageFile)
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
void CGlxHdmiController::CreateSurfaceUpdaterL(const TDesC& aImageFile, 
        TSize aImageDimensions, TInt aFrameCount)
    {
    TRACER("CGlxHdmiController::CreateSurfaceUpdater()");
    RWindow* window = iHdmiContainer->GetWindow();
    iSurfaceUpdater = CGlxHdmiSurfaceUpdater::NewL(window, aImageFile, aImageDimensions, 
            aFrameCount, iHdmiContainer);
    iHdmiContainer->DrawNow();
    }

// -----------------------------------------------------------------------------
// StoreImageInfoL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::StoreImageInfoL(const TDesC& aImageFile,
        TSize aImageDimensions, TInt aFrameCount)
    {
    TRACER("CGlxHdmiController::StoreImageInfoL()");
    if(iStoredImagePath)
        {
        delete iStoredImagePath;
        iStoredImagePath = NULL;
        }
    iStoredImagePath = aImageFile.AllocL();
    iImageDimensions = aImageDimensions;
    iFrameCount = aFrameCount;
    }

// -----------------------------------------------------------------------------
// HandleTvStatusChangedL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::HandleTvStatusChangedL( TTvChangeType aChangeType )
    {
    TRACER("CGlxHdmiController::HandleTvStatusChangedL()");
    if ( aChangeType == ETvConnectionChanged )          
        {
        if ( iGlxTvOut->IsHDMIConnected() && iImageSupported)
            {
            GLX_LOG_INFO("CGlxHdmiController::HandleTvStatusChangedL() - HDMI Connected");
            // Calling SetImageL() with appropriate parameters
            SetImageL(iStoredImagePath->Des(), iImageDimensions, iFrameCount, EFalse);
            }
        else
            {
            // if it gets disconnected, destroy the surface 
            GLX_LOG_INFO("CGlxHdmiController::HandleTvStatusChangedL() - HDMI Not Connected");
            DestroySurfaceUpdater();
            }
        }
    }

// -----------------------------------------------------------------------------
// HandleTvStatusChangedL 
// -----------------------------------------------------------------------------
EXPORT_C TBool CGlxHdmiController::IsHDMIConnected()
    {
    TRACER("CGlxHdmiController::IsHDMIConnected()");
    return iGlxTvOut->IsHDMIConnected(); 
    }


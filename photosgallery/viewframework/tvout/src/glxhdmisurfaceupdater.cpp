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

#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>
#include <e32math.h>

#include <imageconversion.h> 
#include <fbs.h>
#include <glxtracer.h>
#include <glxlog.h>

#include "glxactivecallback.h"
#include "glxhdmisurfaceupdater.h"
#include "glxactivedecoder.h"

// 720p image size
const TInt KHdTvWidth = 1280;
const TInt KHdTvHeight = 720;
const TInt KMulFactorToCreateBitmap = 4;
const TInt KZoomDelay = 10000;
//100 , is decide for 20 steps of zooming , with each step being 5 pixels.
const TInt KMaxZoomLimit = 100;
//evey time we zoom , there is a increase in the ht amd width by 10 pixels.
const TInt KSingleStepForZoom = 10;
// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
CGlxHdmiSurfaceUpdater* CGlxHdmiSurfaceUpdater::NewL(RWindow* aWindow, const TDesC& aImageFile, 
        TSize aImageDimensions, TInt aFrameCount, MGlxGenCallback* aCallBack)
    {
    TRACER("CGlxHdmiSurfaceUpdater* CGlxHdmiSurfaceUpdater::NewL()");
    CGlxHdmiSurfaceUpdater* self = new (ELeave) CGlxHdmiSurfaceUpdater(aWindow, aImageFile,
            aImageDimensions, aFrameCount, aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL(aImageDimensions);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// destructor 
// -----------------------------------------------------------------------------
CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater()
    {
    TRACER("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater()");
    ReleaseContent();
    if(iTimer->IsActive())
         {
         iTimer->Cancel();
         }
     delete iTimer;   
    if (iGlxDecoderAO)
        {
        delete iGlxDecoderAO;
        }        
    iGlxDecoderAO = NULL;
    iFsSession.Close();
    if (iSurfManager)
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater() - Close"); 
        iSurfUpdateSession.Close();
        if (iSurfChunk)
            {
            iSurfChunk->Close();
            }            
        delete iSurfChunk;
        iSurfChunk = NULL;        
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater(). iSurfManager->CloseSurface()"); 
        iSurfManager->CloseSurface(iSurfId);
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::~CGlxHdmiSurfaceUpdater(). iSurfManager->Close()"); 
        iSurfManager->Close();
        delete iSurfManager;
        iSurfManager = NULL;       
        }
    }

// -----------------------------------------------------------------------------
// ReleaseContent 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ReleaseContent()
    {
    TRACER("void CGlxHdmiSurfaceUpdater::ReleaseContent()"); 
    
    iGlxDecoderAO->Cancel();    
    if(iDecodedBitmap)
        {
        delete iDecodedBitmap;
        iDecodedBitmap= NULL;
        }    
    if (iSurfBufferAO->IsActive())
        {
        iSurfBufferAO->Cancel();        
        }
    if(iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;    
        }
    iSurfUpdateSession.CancelAllUpdateNotifications();
    }

// -----------------------------------------------------------------------------
// CTor 
// -----------------------------------------------------------------------------
CGlxHdmiSurfaceUpdater::CGlxHdmiSurfaceUpdater(RWindow* aWindow, const TDesC& aImageFile, 
        TSize aOrigImageDimensions, TInt aFrameCount, MGlxGenCallback* aCallBack): 
        iWindow(aWindow), iImagePath(aImageFile), iOrigImageDimensions(aOrigImageDimensions),
        iFrameCount(aFrameCount ),iCallBack(aCallBack)
    {
    TRACER("CGlxHdmiSurfaceUpdater::CGlxHdmiSurfaceUpdater()");
    // Implement nothing here
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ConstructL(TSize /*aImageDimensions*/)
    {
    TRACER("CGlxHdmiSurfaceUpdater::ConstructL()");
    TInt error = iFsSession.Connect ();
    if ( KErrNone!= iFsSession.Connect () )
        {
        User::LeaveIfError(error);
        }
    
    // Create the active object
    iGlxDecoderAO = CGlxHdmiDecoderAO::NewL(this, iFrameCount);
    CreateImageDecoderL(iImagePath);
    CreateBitmapL();
    CreateHdmiL();
    error = iSurfUpdateSession.Connect();
    #ifdef _DEBUG
    iStartTime.HomeTime();
    #endif
    //to refresh the HD screen for the first time.
    iFirstTime = ETrue;
    //start decoding the image    
    iGlxDecoderAO->ConvertImageL(*iDecodedBitmap,0,iImageDecoder);
         
    if (KErrNone !=error)
        {
        GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ConstructL() Surface update Session Connect Failed with error = %d", error);
        User::LeaveIfError(error);
        }
    iLeftCornerForZoom.iX = 0; 
    iLeftCornerForZoom.iY = 0;
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iZoom = ETrue;
    }

// -----------------------------------------------------------------------------
// UpdateNewImageL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::UpdateNewImageL(const TDesC& aImageFile, 
        TInt /*aFrameCount*/)
    {
    TRACER("CGlxHdmiSurfaceUpdater::UpdateNewImageL()");
    ReleaseContent();   
    CreateImageDecoderL(aImageFile);    
    CreateBitmapL();
    CreateHdmiL(EFalse);
    #ifdef _DEBUG
    iStartTime.HomeTime();
    #endif
    //start decoding the image
    iGlxDecoderAO->ConvertImageL(*iDecodedBitmap,0,iImageDecoder);
    }

// -----------------------------------------------------------------------------
// CreateHDMI 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateHdmiL(TBool aCreateSurface)
    {
    TRACER("CGlxHdmiSurfaceUpdater::CreateHdmiL()");
    
    if (aCreateSurface)
        {
        TSize rectSz = iDecodedBitmap->SizeInPixels(); // For animation
        GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::CreateHdmiL() Size height = %d and width = %d", rectSz.iHeight, rectSz.iWidth);
        CreateSurfaceL(rectSz);        
        }
    
    // Active objects for double buffered draw signalling
    if(!iSurfBufferAO)
        {
        iSurfBufferAO = new(ELeave) CGlxActiveCallBack(TCallBack(SurfBuffer0Ready, this),
                    CActive::EPriorityStandard-1);
        CActiveScheduler::Add(iSurfBufferAO);    
        }
    
    //Set the background Surface
    iWindow->SetBackgroundSurface(iSurfId);
    }

// -----------------------------------------------------------------------------
// CreateSurfaceL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateSurfaceL(TSize aSize)
    {
    TRACER("CGlxHdmiSurfaceUpdater::CreateSurfaceL()");
       
    iSurfManager = new(ELeave) RSurfaceManager();
    TInt error = iSurfManager->Open();
    if (error != KErrNone)
        {
        GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::CreateSurfaceL Open Surface manager failed with error = %d", error);
        User::LeaveIfError(error);
        }
    RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
    attributes().iPixelFormat           = EUidPixelFormatARGB_8888;// EUidPixelFormatYUV_420Planar;
    attributes().iSize                  = aSize;
    
    attributes().iBuffers               = 1;
    attributes().iStride                = aSize.iWidth * KMulFactorToCreateBitmap;  
    attributes().iAlignment             = KMulFactorToCreateBitmap;
    attributes().iContiguous            = EFalse;
    attributes().iMappable              = ETrue;
        
    error = iSurfManager->CreateSurface(attributes, iSurfId);
    if(error)
        {
        GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::CreateSurfaceL, Creating surface failed with error : %d",error);
        User::LeaveIfError(error);
        }    
    //Map the surface and stire the surface info
    MapSurfaceL();
    }

void CGlxHdmiSurfaceUpdater::MapSurfaceL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::MapSurfaceL()");
    
    //Create chunk to map it to the surface ID.
    iSurfChunk = new(ELeave) RChunk();
    User::LeaveIfNull(iSurfChunk);    
    TInt error = iSurfManager->MapSurface(iSurfId, *iSurfChunk);
    if(error!=KErrNone)
        {
        GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::CreateSurfaceL(), MapSurface Failed wint error : %d",error);
        }  
    
    // Get the info from the surfaceManager
    // and store pointers to the pixel data
    RSurfaceManager::TInfoBuf info;
    error = iSurfManager->SurfaceInfo(iSurfId, info);    
    iSurfaceStride = info().iStride;
    User::LeaveIfError(error);  
    TInt offset = 0;
    iSurfManager->GetBufferOffset( iSurfId, 0,offset);
    iSurfBuffer = iSurfChunk->Base()+offset;
    }
// -----------------------------------------------------------------------------
// SurfBuffer0Ready 
// -----------------------------------------------------------------------------
TInt CGlxHdmiSurfaceUpdater::SurfBuffer0Ready(TAny* /*aObject*/)
    {
    TRACER("CGlxHdmiSurfaceUpdater::SurfBuffer0Ready()");
    return ETrue;
    }

// -----------------------------------------------------------------------------
// Refresh 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::Refresh()
    {
    TRACER("CGlxHdmiSurfaceUpdater::Refresh()");
    SwapBuffers();   
    iCallBack->DoGenCallback();       
    if(iFirstTime)  
	{
	iFirstTime = EFalse;
	iWindow->RemoveBackgroundSurface(ETrue);
	iWindow->SetBackgroundSurface(iSurfId);
	}
    }

// -----------------------------------------------------------------------------
// SwapBuffers
// This is used to sawp the buffers shown and to be shown 
// After this is done, a refresh to the window should be done to refresh the TV
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::SwapBuffers()
    {
    TRACER("CGlxHdmiSurfaceUpdater::SwapBuffers()"); 
      
    // Lock the heap so that subsequent call to dataaddress doesnt happen
    iDecodedBitmap->LockHeap();    
    
    // Data stride
    TUint fs = iDecodedBitmap->DataStride();    
    
    //Bitmap address from where the data has to be copied.
    TUint8* from = (TUint8*)iDecodedBitmap->DataAddress();
    
    //surface chunk address to where the bitmap data has to be copied.
    TUint8* to = (TUint8*)iSurfBuffer;    
    
    // To buffer (32 bit colors)
    TUint ts = iSurfaceStride;
    //No of bytes to be copied on to the surface.
    TUint bytes = iDecodedBitmap->SizeInPixels().iWidth * KMulFactorToCreateBitmap;
    
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::SwapBuffers() - decodeSize width = %d and height %d",
            iDecodedBitmap->SizeInPixels().iWidth, iDecodedBitmap->SizeInPixels().iHeight );
    
    // Copy the bitmap on to the surface.
    for (TInt y = iDecodedBitmap->SizeInPixels().iHeight; y >0; y--)
        {
        Mem::Copy(to, from, bytes);        
        to += ts;        
        from += fs;        
        }            
    iDecodedBitmap->UnlockHeap();
    }

// -----------------------------------------------------------------------------
// StartImageDecodeL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateBitmapL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::StartImageDecodeL()");
    TSize decodeSize;
    decodeSize.iHeight = (iOrigImageDimensions.iHeight>KHdTvHeight? KHdTvHeight: 
                                    iOrigImageDimensions.iHeight);
    decodeSize.iWidth = (iOrigImageDimensions.iWidth>KHdTvWidth? KHdTvWidth:
                                    iOrigImageDimensions.iWidth);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::StartImageDecodeL() - decodeSize width = %d and height %d",
            decodeSize.iWidth, decodeSize.iHeight );
    
    //create the bitmap for the required size
    iDecodedBitmap = new(ELeave) CFbsBitmap();
    
    TInt err = iDecodedBitmap->Create(decodeSize,EColor16MU );
    User::LeaveIfNull(iDecodedBitmap);
    }

// -----------------------------------------------------------------------------
// HandleRunL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::HandleRunL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::HandleRunL()");
    
    #ifdef _DEBUG
    iStopTime.HomeTime();
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::HandleRunL() ConvertImageL took us %d us",
            iStopTime.MicroSecondsFrom(iStartTime) );
    #endif
    
    iZoomRectSz = iDecodedBitmap->SizeInPixels(); 
    if (iSurfBufferAO->iStatus != KRequestPending && !iSurfBufferAO->IsActive())
           {
           Refresh();              
           iSurfBufferAO->iStatus = KRequestPending;
           iSurfBufferAO->SetActive();    
           iSurfUpdateSession.NotifyWhenAvailable(iSurfBufferAO->iStatus);
           TInt err = iSurfUpdateSession.SubmitUpdate(1, iSurfId, 0, NULL);       
           }
    }

// -----------------------------------------------------------------------------
// CreateImageDecoderL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateImageDecoderL(const TDesC& aImageFile)
    {
    TRACER("CGlxHdmiController::CreateImageDecoderL()");
     // Create a decoder for the image in the named file
    TRAPD(error,iImageDecoder = CImageDecoder::FileNewL(iFsSession, 
            aImageFile, CImageDecoder::EOptionNone, KNullUid));
    if (error!=KErrNone)
        {
        User::Leave(error);
        }
    }
// -----------------------------------------------------------------------------
// ActivateZoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ActivateZoom()
    {
    TRACER("CGlxHdmiSurfaceUpdater::ActivateZoom()");
    iConfig.SetSurfaceId(iSurfId);
    iZoom = ETrue;
    if(!iTimer->IsActive())
       {
       iTimer->Start(KZoomDelay,KZoomDelay,TCallBack( TimeOut,this ));
       }
    }
// -----------------------------------------------------------------------------
// DeactivateZoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::DeactivateZoom()
    {
    TRACER("CGlxHdmiSurfaceUpdater::DeactivateZoom()");
    if(iDecodedBitmap)
        {
        TSize bitmapsize = iDecodedBitmap->SizeInPixels(); 
        iConfig.SetViewport(TRect(0,0,bitmapsize.iWidth,bitmapsize.iHeight));
        iConfig.SetExtent(TRect(0,0,bitmapsize.iWidth,bitmapsize.iHeight));
        iWindow->SetBackgroundSurface(iConfig, ETrue);
        }
    
    }
// ---------------------------------------------------------------------------
// TimeOut
// ---------------------------------------------------------------------------
//  
TInt CGlxHdmiSurfaceUpdater::TimeOut(TAny* aSelf)
    {
    TRACER("CGlxHdmiSurfaceUpdater::TimeOut");
    if(aSelf)
        {
        CGlxHdmiSurfaceUpdater* self = static_cast <CGlxHdmiSurfaceUpdater*> (aSelf);
        if (self)
            {            
             self->Zoom(ETrue);
            }
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// Zoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::Zoom(TBool aZoom)
    {
    TRACER("CGlxHdmiSurfaceUpdater::Zoom()");
    if(iLeftCornerForZoom.iX == KMaxZoomLimit)
        {
        iZoom = EFalse;
        }    
    if(aZoom && iZoom)
        {
        iZoomRectSz.iWidth = TInt(iZoomRectSz.iWidth-KSingleStepForZoom);
        iZoomRectSz.iHeight = TInt(iZoomRectSz.iHeight-KSingleStepForZoom);
        iLeftCornerForZoom.iX =iLeftCornerForZoom.iX+KSingleStepForZoom/2;
        iLeftCornerForZoom.iY =iLeftCornerForZoom.iY+KSingleStepForZoom/2;           
        GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::Zoom()--- 2,iZoomRectSz.iWidth = %d, iZoomRectSz.iHeight = %d", iZoomRectSz.iWidth,iZoomRectSz.iHeight);
        iConfig.SetViewport(TRect(iLeftCornerForZoom.iX,iLeftCornerForZoom.iY,iZoomRectSz.iWidth,iZoomRectSz.iHeight));
        }
    else
        {
        iZoomRectSz.iWidth = TInt(iZoomRectSz.iWidth+KSingleStepForZoom);
        iZoomRectSz.iHeight = TInt(iZoomRectSz.iHeight+KSingleStepForZoom);
        iLeftCornerForZoom.iX =iLeftCornerForZoom.iX-KSingleStepForZoom/2;
        iLeftCornerForZoom.iY =iLeftCornerForZoom.iY-KSingleStepForZoom/2; 
        if(iLeftCornerForZoom.iX == 0)
            {
            iTimer->Cancel();
            iZoom = ETrue;
            }
        GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::Zoom()--- 4,iZoomRectSz.iWidth = %d, iZoomRectSz.iHeight = %d", iZoomRectSz.iWidth,iZoomRectSz.iHeight);
        iConfig.SetViewport(TRect(iLeftCornerForZoom.iX,iLeftCornerForZoom.iY,iZoomRectSz.iWidth,iZoomRectSz.iHeight));
        }
    TSize bitmapsize = iDecodedBitmap->SizeInPixels();
    iConfig.SetExtent(TRect(0,0,bitmapsize.iWidth,bitmapsize.iHeight));    
    iWindow->SetBackgroundSurface(iConfig, ETrue);   
    }

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
    if(iWindow)
        {
        iWindow->RemoveBackgroundSurface(ETrue);
        }
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
        if(iSurfSessionConnected)
            {
            iSurfUpdateSession.Close();
            }
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
    if ( iGlxDecoderAO )
        {
        iGlxDecoderAO->Cancel();
        }
    
    if ( iDecodedBitmap )
        {
        delete iDecodedBitmap;
        iDecodedBitmap= NULL;
        }
    
    if(iSurfBufferAO && iSurfBufferAO->IsActive())
        {
		iSurfBufferAO->Cancel();
        }
    
    if ( iImageDecoder )
        {
        delete iImageDecoder;
        iImageDecoder = NULL;    
        }
    
    if (iSurfSessionConnected &&  iSurfManager)
        {
        iSurfUpdateSession.CancelAllUpdateNotifications();
        }
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
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ConstructL() FsSession Connect error = %d", error);
    User::LeaveIfError(error);
    
    iBitmapReady = EFalse;
    // Create the active object
    iGlxDecoderAO = CGlxHdmiDecoderAO::NewL(this, iFrameCount);
    CreateImageDecoderL(iImagePath);
    CreateBitmapL();
    CreateHdmiL();
    error = iSurfUpdateSession.Connect();
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::ConstructL() Surface update Session Connect error = %d", error);
    User::LeaveIfError(error);
    iSurfSessionConnected = ETrue;
    
#ifdef _DEBUG
    iStartTime.HomeTime();
#endif
    //start decoding the image    
    iGlxDecoderAO->ConvertImageL(*iDecodedBitmap,0,iImageDecoder);    
    
    iLeftCornerForZoom.iX = 0; 
    iLeftCornerForZoom.iY = 0;
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iZoom = ETrue;
    }

// -----------------------------------------------------------------------------
// UpdateNewImageL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::UpdateNewImageL(const TDesC& aImageFile, 
        TInt /*aFrameCount*/,TSize aImageDimensions)
    {
    TRACER("CGlxHdmiSurfaceUpdater::UpdateNewImageL()");
	//Cancel the zoom timers if any
	if(iTimer->IsActive())
        {
		GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::UpdateNewImageL() - Cancel Timer");
        iTimer->Cancel();
        }
    iOrigImageDimensions = aImageDimensions;
	iBitmapReady = EFalse;
	iLeftCornerForZoom.iX = 0; 
	iLeftCornerForZoom.iY = 0;
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
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::CreateHdmiL() Create Surface");
        CreateSurfaceL();        
        }
    
    // Active objects for double buffered draw signalling
    if(!iSurfBufferAO)
        {
        iSurfBufferAO = new(ELeave) CGlxActiveCallBack(TCallBack(SurfBuffer0Ready, this),
                    CActive::EPriorityStandard-1);
        CActiveScheduler::Add(iSurfBufferAO);    
        }
    }

// -----------------------------------------------------------------------------
// CreateSurfaceL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::CreateSurfaceL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::CreateSurfaceL()");
    TSize surfaceSize = iWindow->Size();   // create surface of the screen size, i.e 1280x720
    iSurfManager = new(ELeave) RSurfaceManager();
    TInt error = iSurfManager->Open();
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::CreateSurfaceL Open Surface manager error = %d", error);
    User::LeaveIfError(error);
    
    RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
    attributes().iPixelFormat           = EUidPixelFormatARGB_8888;// EUidPixelFormatYUV_420Planar;
    attributes().iSize                  = surfaceSize;
    
    attributes().iBuffers               = 1;
    attributes().iStride                = surfaceSize.iWidth * KMulFactorToCreateBitmap;  
    attributes().iAlignment             = KMulFactorToCreateBitmap;
    attributes().iContiguous            = EFalse;
    attributes().iMappable              = ETrue;
        
    error = iSurfManager->CreateSurface(attributes, iSurfId);
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::CreateSurfaceL, Creating surface error : %d",error);
    User::LeaveIfError(error);
        
    //Map the surface and stride the surface info
    MapSurfaceL();
    // Set the Configuration to the surface ID when creating a surface
    iConfig.SetSurfaceId(iSurfId);
    }

// -----------------------------------------------------------------------------
// MapSurfaceL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::MapSurfaceL()
    {
    TRACER("CGlxHdmiSurfaceUpdater::MapSurfaceL()");
    
    //Create chunk to map it to the surface ID.
    iSurfChunk = new(ELeave) RChunk();
    User::LeaveIfNull(iSurfChunk);    
    TInt error = iSurfManager->MapSurface(iSurfId, *iSurfChunk);
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::MapSurfaceL(), MapSurface error : %d",error);
    User::LeaveIfError(error);
    
    // Get the info from the surfaceManager
    // and store pointers to the pixel data
    RSurfaceManager::TInfoBuf info;
    error = iSurfManager->SurfaceInfo(iSurfId, info);
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::MapSurfaceL(), SurfaceInfo error : %d",error);
    User::LeaveIfError(error);  
    
    iSurfaceStride = info().iStride;    
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
    // copy the decoded bitmap on to the surface
    SwapBuffers();   
    // Modify the surface position with respect to the buffer size 
	ModifySurfacePostion();
	// refresh the window
    iCallBack->DoGenCallback();       
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
    TSize scrnSize = iWindow->Size();
    TSize targetBitmapSize;
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::StartImageDecodeL() - bitmapsize=%d, %d",iOrigImageDimensions.iWidth,iOrigImageDimensions.iHeight);
    TReal32 scaleFactor = 0.0f;
    if (scrnSize.iWidth * iOrigImageDimensions.iHeight > scrnSize.iHeight
            * iOrigImageDimensions.iWidth)
        {
        scaleFactor = (TReal32) scrnSize.iHeight
                / (TReal32) iOrigImageDimensions.iHeight;
        }
    else
        {
        scaleFactor = (TReal32) scrnSize.iWidth
                / (TReal32) iOrigImageDimensions.iWidth;
        }
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::StartImageDecodeL() - scaleFactor=%f",scaleFactor);
    targetBitmapSize.iHeight = iOrigImageDimensions.iHeight * scaleFactor;
    targetBitmapSize.iWidth = iOrigImageDimensions.iWidth * scaleFactor;

    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::StartImageDecodeL() - targetBitmapSize=%d, %d",targetBitmapSize.iWidth,targetBitmapSize.iHeight);
    //create the bitmap for the required size
    iDecodedBitmap = new (ELeave) CFbsBitmap();

    TInt err = iDecodedBitmap->Create(targetBitmapSize, EColor16MU);
    User::LeaveIfNull(iDecodedBitmap);
    }

// -----------------------------------------------------------------------------
// HandleRunL 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::HandleRunL(TRequestStatus& aStatus)
    {
    TRACER("CGlxHdmiSurfaceUpdater::HandleRunL()");

#ifdef _DEBUG
    iStopTime.HomeTime();
    GLX_LOG_INFO1("CGlxHdmiSurfaceUpdater::HandleRunL() ConvertImageL took"
            " <%d> us", (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif
    if(aStatus.Int() !=KErrNone)
        {
        GLX_LOG_INFO("HandleRunL - Convert failed");
        ShiftToCloningMode();
        }
    else
        {        
        iZoomRectSz = iDecodedBitmap->SizeInPixels();
        if (iSurfBufferAO->iStatus != KRequestPending
                && !iSurfBufferAO->IsActive())
            {
            Refresh();
            iSurfBufferAO->iStatus = KRequestPending;
            iSurfBufferAO->SetActive();
            iSurfUpdateSession.NotifyWhenAvailable(iSurfBufferAO->iStatus);
            TInt err = iSurfUpdateSession.SubmitUpdate(1, iSurfId, 0, NULL);
            }
		iBitmapReady = ETrue;
        }
	//release imagedecoder after the conversion is over		
    if(iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;    
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
    GLX_LOG_INFO1("CreateImageDecoderL CImageDecoder:FileNewL error %d",error);
    User::LeaveIfError(error);
    
    }

// -----------------------------------------------------------------------------
// ActivateZoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ActivateZoom(TBool aAutoZoomOut)
    {
    TRACER("CGlxHdmiSurfaceUpdater::ActivateZoom()");
    iZoom = ETrue;
    iAutoZoomOut = aAutoZoomOut;
    if(iTimer->IsActive())
        {
		GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ActivateZoom() - Cancel Timer");
        iTimer->Cancel();
        }
    
    if(!iTimer->IsActive() && iBitmapReady)
       {
	   GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::ActivateZoom() - Start Timer");
       iTimer->Start(KZoomDelay,KZoomDelay,TCallBack( TimeOut,this ));
       }
    }

// -----------------------------------------------------------------------------
// DeactivateZoom 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::DeactivateZoom()
    {
    TRACER("CGlxHdmiSurfaceUpdater::DeactivateZoom()");
    
    if(iTimer->IsActive())
        {
		GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::DeactivateZoom() - Cancel Timer");
        iTimer->Cancel();
        }
    if(!iTimer->IsActive() && iBitmapReady && iLeftCornerForZoom.iX)
       {
	   GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::DeactivateZoom() - Start Timer");
	   iZoom = EFalse;
       iTimer->Start(KZoomDelay,KZoomDelay,TCallBack( TimeOut,this ));
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
	
	if(!iBitmapReady)
		{
		return;
		}
		
    if(iLeftCornerForZoom.iX == KMaxZoomLimit)
        {
        iZoom = EFalse;
        //If autozoomout is not set then cancel the timer and do
        //the zoom out on DeactivateZoom.
        if(!iAutoZoomOut)
            {
            iTimer->Cancel();			
            }
        }    
    if(aZoom && iZoom)
        {
        iZoomRectSz.iWidth = TInt(iZoomRectSz.iWidth-KSingleStepForZoom);
        iZoomRectSz.iHeight = TInt(iZoomRectSz.iHeight-KSingleStepForZoom);
        iLeftCornerForZoom.iX =iLeftCornerForZoom.iX+KSingleStepForZoom/2;
        iLeftCornerForZoom.iY =iLeftCornerForZoom.iY+KSingleStepForZoom/2;           
        GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::Zoom()--- 2,iZoomRectSz.iWidth = %d, iZoomRectSz.iHeight = %d", iZoomRectSz.iWidth,iZoomRectSz.iHeight);
        iConfig.SetViewport(TRect(iLeftCornerForZoom.iX,iLeftCornerForZoom.iY,
                iZoomRectSz.iWidth,iZoomRectSz.iHeight));
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
        iConfig.SetViewport(TRect(iLeftCornerForZoom.iX,iLeftCornerForZoom.iY,
                iZoomRectSz.iWidth,iZoomRectSz.iHeight));
        }
    iWindow->SetBackgroundSurface(iConfig, ETrue);   
    }

// -----------------------------------------------------------------------------
// ModifySurfacePostion 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ModifySurfacePostion()
	{
	TRACER("CGlxHdmiSurfaceUpdater::ModifySurfacePostion()");
	TSize bitmapSize = iDecodedBitmap->SizeInPixels();
	TPoint startPoint(0,0);
	if (bitmapSize.iWidth <KHdTvWidth)
	    {
	    startPoint.iX = (KHdTvWidth - bitmapSize.iWidth)/2; 
	    }
	if (bitmapSize.iHeight <KHdTvHeight)
	    {
	    startPoint.iY = (KHdTvHeight - bitmapSize.iHeight)/2;
	    }
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - target bitmapsize=%d, %d",bitmapSize.iWidth,bitmapSize.iHeight);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - startPoint =%d, %d",startPoint.iX,startPoint.iY);

    // target
    iConfig.SetExtent(TRect(startPoint.iX,startPoint.iY,(KHdTvWidth-startPoint.iX),
            (KHdTvHeight-startPoint.iY)));
    // source
    iConfig.SetViewport(TRect(TPoint(0,0),TSize(bitmapSize.iWidth,bitmapSize.iHeight)));
#ifdef _DEBUG
    TRect ex, vp;
    iConfig.GetExtent(ex);
    iConfig.GetViewport(vp);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - vp - TL=%d, %d",vp.iTl.iX,vp.iTl.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - vp - BR=%d, %d",vp.iBr.iX,vp.iBr.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - ex - TL=%d, %d",ex.iTl.iX,ex.iTl.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ModifySurfacePostion() - ex - BR=%d, %d",ex.iBr.iX,ex.iBr.iY);
#endif
    iWindow->SetBackgroundSurface(iConfig, ETrue);   
    }

// -----------------------------------------------------------------------------
// ShiftToCloningMode 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ShiftToCloningMode()
	{
	TRACER("CGlxHdmiSurfaceUpdater::ShiftToCloningMode()");
	iWindow->RemoveBackgroundSurface(ETrue);
	}
	
// -----------------------------------------------------------------------------
// ShiftToPostingMode 
// -----------------------------------------------------------------------------
void CGlxHdmiSurfaceUpdater::ShiftToPostingMode()
	{
	TRACER("CGlxHdmiSurfaceUpdater::ShiftToPostingMode()");
#ifdef _DEBUG
	TRect ex, vp;
	iConfig.GetExtent(ex);
    iConfig.GetViewport(vp);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - vp - TL=%d, %d",vp.iTl.iX,vp.iTl.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - vp - BR=%d, %d",vp.iBr.iX,vp.iBr.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - ex - TL=%d, %d",ex.iTl.iX,ex.iTl.iY);
    GLX_LOG_INFO2("CGlxHdmiSurfaceUpdater::ShiftToPostingMode() - ex - BR=%d, %d",ex.iBr.iX,ex.iBr.iY);
#endif
    iWindow->SetBackgroundSurface(iConfig, ETrue);
	}

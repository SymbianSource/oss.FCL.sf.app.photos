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
* Description:    Texture Manager component/Decodes the Image in step for large sized images
 *
*/




// INCLUDE FILES
#include "glxbitmapdecoderwrapper.h"
#include <glxtracer.h>      //  For Log
#include <bitmaptransforms.h>
#include <imageconversion.h>
#include <ExifRead.h>
#include <glxlog.h>
#include <hal.h>
#include <hal_data.h>
#include <mglxcache.h>
#include <oommonitorsession.h>
#include <e32math.h>
#include <apgcli.h>

namespace
    {
    const TInt KGlxDecodingThreshold = 3000000;
    const TInt KGlxDecodingThresholdDimension = 2000;

    // Photos low_ram_threshold [8.637 MB] as in oomconfig.xml
    const TInt KGlxCriticalRAMForPhotos = 9056550;
    const TInt KGlxDecodeBitmapFactor = 3;
    
    _LIT(KMimeJpeg,"image/jpeg");
    _LIT(KMimeJpg,"image/jpg");
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxBitmapDecoderWrapper* CGlxBitmapDecoderWrapper::NewL(
        MGlxBitmapDecoderObserver* aObserver)
    {
    TRACER("CGlxBitmapDecoderWrapper::NewL()");
    CGlxBitmapDecoderWrapper* self = new (ELeave) CGlxBitmapDecoderWrapper();
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CGlxBitmapDecoderWrapper::CGlxBitmapDecoderWrapper() :
    CActive(EPriorityLow)
    {
    TRACER("CGlxBitmapDecoderWrapper::CGlxBitmapDecoderWrapper()");
    }		

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxBitmapDecoderWrapper::~CGlxBitmapDecoderWrapper()
    {
    TRACER("CGlxBitmapDecoderWrapper::~CGlxBitmapDecoderWrapper()");
    iFs.Close();
    Cancel();
    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    if (iImagePath)
        {
        delete iImagePath;
        iImagePath = NULL;
        }
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CGlxBitmapDecoderWrapper::ConstructL(MGlxBitmapDecoderObserver* aObserver)
    {
    TRACER("CGlxBitmapDecoderWrapper::ConstructL ");
    iObserver = aObserver;
    User::LeaveIfError(iFs.Connect());
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// DoDecodeImageL
// ---------------------------------------------------------------------------		
void CGlxBitmapDecoderWrapper::DoDecodeImageL(const TDesC& aSourceFileName,
        TInt aIndex)
    {
    TRACER("CGlxBitmapDecoderWrapper::DoDecodeImageL()");
    GLX_LOG_URI("CGlxBitmapDecoderWrapper::DoDecodeImageL(%S)",
            &aSourceFileName);

    iThumbnailIndex = aIndex;

#ifdef _DEBUG
    iStartTime.HomeTime();
#endif                          

    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }

    CImageDecoder::TOptions options =
            (CImageDecoder::TOptions) (CImageDecoder::EOptionNoDither
                    | CImageDecoder::EOptionAlwaysThread);

    // Use extended JPEG decoder
    GLX_DEBUG1("DoDecodeImageL:: EHwImplementation" );
    TRAPD( err, iImageDecoder = CExtJpegDecoder::FileNewL(
                    CExtJpegDecoder::EHwImplementation, iFs,
                    aSourceFileName, options ) );
    GLX_DEBUG2("DoDecodeImageL:: EHwImplementation (%d)", err);
    if (KErrNone != err)
        {
        TRAP(err,iImageDecoder = CExtJpegDecoder::FileNewL(
                        CExtJpegDecoder::ESwImplementation, iFs,
                        aSourceFileName, options ) );
        GLX_DEBUG2("DoDecodeImageL:: ESwImplementation (%d)", err);
        if (KErrNone != err)
            {
            GLX_DEBUG1("DoDecodeImageL::CImageDecoder");
            // Not a JPEG - use standard decoder
            iImageDecoder = CImageDecoder::FileNewL(iFs, aSourceFileName,
                    options);
            }
        }
#ifdef _DEBUG
    iStopTime.HomeTime();
    GLX_DEBUG2("*** Decoder Creation took <%d> us ***",
            (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif                          

    TSize imageSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
    GLX_DEBUG3("GlxDecoderWrapper::DecodeImageL() - OverallSize: w=%d, h=%d",
            imageSize.iWidth, imageSize.iHeight);
    iOriginalSize.iWidth = imageSize.iWidth;
    iOriginalSize.iHeight = imageSize.iHeight;

    if (iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    
    if (iImagePath)
        {
        delete iImagePath;
        iImagePath = NULL;
        }
    iImagePath = aSourceFileName.Alloc();
    
    DecodeImageL();
    }

// ---------------------------------------------------------------------------
// DecodeImageL
// ---------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::DecodeImageL()
    {
    TRACER("CGlxBitmapDecoderWrapper::DecodeImageL()");
    TReal32 mFactor = 1;
    TReal mFactor1 = 1;
    TReal32 mFactor2 = 1;
    //Set Size according to level and state
    TReal32 width = iOriginalSize.iWidth;
    TReal32 height = iOriginalSize.iHeight;

    if (KGlxDecodingThreshold < (width * height))
        {
        TReal tempFactor = TReal32(KGlxDecodingThreshold)
                / (width * height);
        User::LeaveIfError(Math::Sqrt(mFactor1, tempFactor));
        GLX_DEBUG2("DecodeImageL() - mFactor1 = %f", mFactor1);
        }

    if (KGlxDecodingThresholdDimension < width
            || KGlxDecodingThresholdDimension < height)
        {
        mFactor2 = TReal32(KGlxDecodingThresholdDimension) / Max(width,
                height);
        GLX_DEBUG2("DecodeImageL() - mFactor2 = %f", mFactor2);
        }

    mFactor = Min(TReal32(mFactor1), mFactor2);
    GLX_DEBUG2("DecodeImageL() - Final mFactor = %f", mFactor);

    // create the destination bitmap
    if (!iBitmap)
        {
        TInt freeMemory = 0;
        HAL::Get(HALData::EMemoryRAMFree, freeMemory);
        width *= mFactor;
        height *= mFactor;
        TInt minmemorytodecode = KGlxDecodeBitmapFactor * width * height;
        GLX_DEBUG3("DecodeImageL: minmemorytodecode=%d, freememory=%d",
                minmemorytodecode, freeMemory);
        
        iTargetBitmapSize.iWidth = width;
        iTargetBitmapSize.iHeight = height;
        GLX_DEBUG3("DecodeImageL: iTargetBitmapSize w=%d, h=%d",
                iTargetBitmapSize.iWidth, iTargetBitmapSize.iHeight);
        __ASSERT_DEBUG(width > 0 && height > 0, Panic(EGlxPanicIllegalArgument));      

        if (minmemorytodecode < (freeMemory - KGlxCriticalRAMForPhotos))
            {
            GLX_DEBUG1("DecodeImageL:RAM available decoding image");            
            iBitmap = new (ELeave) CFbsBitmap();
            iBitmap->Create(ReCalculateSizeL(),
                    iImageDecoder->FrameInfo().iFrameDisplayMode);
#ifdef _DEBUG
            iStartTime.HomeTime(); // Get home time
#endif                          
            iImageDecoder->Convert(&iStatus, *iBitmap);
            }
        else
            {
            //case when sufficient memory is not available
            //request OOM FW to release the required memory
            GLX_DEBUG1("DecodeImageL:insufficient RAM - request OOM");
            TInt err = OOMRequestFreeMemoryL(minmemorytodecode);
            if (err == KErrNoMemory)
                {
                //if OOM fails, release Photos Cache
                GLX_DEBUG1("DecodeImageL:insufficient RAM - OOM failed"
                        " - request Cache");
                MGlxCache* cacheManager = MGlxCache::InstanceL();
                cacheManager->ReleaseRAML(ETrue);
                cacheManager->Close();
                //Try and release memory again
                err = OOMRequestFreeMemoryL(minmemorytodecode);
                }
            if (err != KErrNoMemory)
                {
                GLX_DEBUG1("DecodeImageL:Sufficient RAM available");                
                iBitmap = new (ELeave) CFbsBitmap();
                iBitmap->Create(ReCalculateSizeL(),
                        iImageDecoder->FrameInfo().iFrameDisplayMode);
#ifdef _DEBUG
                iStartTime.HomeTime(); // Get home time
#endif                          
                iImageDecoder->Convert(&iStatus, *iBitmap);
                }
            else
                {
                GLX_DEBUG1("NOT ENOUGH MEMORY - "
                        "Using the Fullscreen Thumbnail For Zoom");
                //release the file held by decoder immediately.
                iImageDecoder->Cancel();
                delete iImageDecoder;
                iImageDecoder = NULL;
                //Inform the client that there is no decode happened and there we take care 
                //of showing the fullscreen thumbnail.
                iObserver->HandleBitmapDecodedL(iThumbnailIndex, NULL);
                return;
                }
            }

        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::RunL()
    {
    TRACER("CGlxBitmapDecoderWrapper::RunL()");
    if( iStatus == KErrNone )
        {
        iObserver->HandleBitmapDecodedL(iThumbnailIndex,iBitmap);
        iBitmap = NULL;

        //release the file held by decoder immediately.
        GLX_DEBUG1("CGlxBitmapDecoderWrapper::RunL:Decoding Finished");
        iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
#ifdef _DEBUG
        iStopTime.HomeTime();
        GLX_DEBUG2("*** Image Decode took <%d> us ***", 
                       (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());            
#endif                          

        }
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::DoCancel()
    {
    TRACER("CGlxBitmapDecoderWrapper::DoCancel ");
    if(iImageDecoder)
        {
        GLX_DEBUG1("CGlxBitmapDecoderWrapper::DoCancel iImageDecoder delete");
        iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    if(iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    }

// ---------------------------------------------------------------------------
// OOMRequestFreeMemoryL
// ---------------------------------------------------------------------------
//
TInt CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL(TInt aBytesRequested)
    {
    TRACER("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL");
    GLX_LOG_INFO1("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL() "
            "aBytesRequested=%d", aBytesRequested);
    ROomMonitorSession oomMonitor;
    User::LeaveIfError(oomMonitor.Connect());
    // No leaving code after this point, so no need to use cleanup stack
    // for oomMonitor
    TInt errorCode = oomMonitor.RequestFreeMemory(aBytesRequested);
    GLX_LOG_INFO1("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL(1) "
            "errorCode=%d",errorCode);
    if (errorCode != KErrNone)
        {
        // try one more time 
        errorCode = oomMonitor.RequestFreeMemory(aBytesRequested);
        GLX_LOG_INFO1("CGlxBitmapDecoderWrapper::OOMRequestFreeMemoryL(2) "
                "errorCode=%d",errorCode);
        }
    oomMonitor.Close();
    return errorCode;
    }

// -----------------------------------------------------------------------------
// DoesMimeTypeNeedsRecalculateL()
// -----------------------------------------------------------------------------
//
TBool CGlxBitmapDecoderWrapper::DoesMimeTypeNeedsRecalculateL()
    {
    TRACER("CGlxBitmapDecoderWrapper::DoesMimeTypeNeedsRecalculateL");
    RApaLsSession session;
    TDataType mimeType;
    TUid uid;

    User::LeaveIfError(session.Connect());
    CleanupClosePushL(session);
    User::LeaveIfError(session.AppForDocument(iImagePath->Des(), uid,
            mimeType));
    CleanupStack::PopAndDestroy(&session);

    if (mimeType.Des().Compare(KMimeJpeg) == 0 || mimeType.Des().Compare(
            KMimeJpg) == 0)
        {
        GLX_LOG_INFO("CGlxBitmapDecoderWrapper::DoesMimeTypeNeedsRecalculateL - jpeg");
        return EFalse;
        }
    else
        {
        GLX_LOG_INFO("CGlxHdmiSurfaceUpdater::DoesMimeTypeNeedsRecalculateL - non jpeg");
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// ReCalculateSize 
// -----------------------------------------------------------------------------
TSize CGlxBitmapDecoderWrapper::ReCalculateSizeL()
    {
    TRACER("CGlxBitmapDecoderWrapper::ReCalculateSizeL()");
    if (DoesMimeTypeNeedsRecalculateL())
        {
        TSize fullFrameSize = iImageDecoder->FrameInfo().iOverallSizeInPixels;
        // calculate the reduction factor on what size we need
        TInt reductionFactor = iImageDecoder->ReductionFactor(fullFrameSize,
                iTargetBitmapSize);
        // get the reduced size onto destination size
        TSize destSize;
        User::LeaveIfError(iImageDecoder->ReducedSize(fullFrameSize,
                reductionFactor, destSize));
        GLX_LOG_INFO2("CGlxBitmapDecoderWrapper::ReCalculateSizeL() "
                        "destSize=%d, %d",destSize.iWidth,destSize.iHeight);
        return destSize;
        }
    else
        {
        return iTargetBitmapSize;
        }
    }

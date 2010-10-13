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
CGlxBitmapDecoderWrapper::CGlxBitmapDecoderWrapper()
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
    if (iTnManager)
        {
        iTnManager->CancelRequest(iTnReqId);
        delete iTnManager;
        iTnManager = NULL;
        }
    if(iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
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
    }

// ---------------------------------------------------------------------------
// DoDecodeImageL
// ---------------------------------------------------------------------------		
void CGlxBitmapDecoderWrapper::DoDecodeImageL(const TGlxMedia& aMedia,
        TInt aIndex)
    {
    TRACER("CGlxBitmapDecoderWrapper::DoDecodeImageL()");

    iThumbnailIndex = aIndex;
    TSize originalDim;
    aMedia.GetDimensions(originalDim);
    iOriginalSize.iWidth = originalDim.iWidth;
    iOriginalSize.iHeight = originalDim.iHeight;

    if (iImagePath)
        {
        delete iImagePath;
        iImagePath = NULL;
        }

    const TDesC& uri = aMedia.Uri();
    GLX_LOG_URI("CGlxBitmapDecoderWrapper::DoDecodeImageL(%S)", &uri);
    iImagePath = uri.Alloc();
    
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
             
            GetThumbnailL(iImagePath);
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
                GetThumbnailL(iImagePath);
                }
            else
                {
                GLX_DEBUG1("NOT ENOUGH MEMORY - "
                        "Using the Fullscreen Thumbnail For Zoom");
                //Inform the client that there is no decode happened and there we take care 
                //of showing the fullscreen thumbnail.
                iObserver->HandleBitmapDecodedL(iThumbnailIndex, NULL);
                return;
                }
            }
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
// CGlxBitmapDecoderWrapper::GetThumbnailL()
// -----------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::GetThumbnailL( HBufC* aImagePath )
    {
    TRACER("CGlxBitmapDecoderWrapper::GetThumbnailL");
    if (!iTnManager)
        {
        iTnManager = CThumbnailManager::NewL(*this);
        iTnManager->SetDisplayModeL(EColor16M);
        }

    iTnManager->SetFlagsL(CThumbnailManager::EDefaultFlags);
    iTnManager->SetThumbnailSizeL(iTargetBitmapSize);
    iTnManager->SetQualityPreferenceL(CThumbnailManager::EOptimizeForQuality);
    const TDesC& uri = aImagePath->Des();
    GLX_LOG_URI("CGlxBitmapDecoderWrapper::GetThumbnailL: (%S)", aImagePath );
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(uri, 0);
#ifdef _DEBUG
    iStartTime.HomeTime(); // Get home time
#endif                          
    iTnReqId = iTnManager->GetThumbnailL(*source);
    CleanupStack::PopAndDestroy(source);
    }

// -----------------------------------------------------------------------------
// CGlxBitmapDecoderWrapper::ThumbnailPreviewReady
// From MThumbnailManagerObserver
// -----------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::ThumbnailPreviewReady( MThumbnailData& /*aThumbnail*/, 
                            TThumbnailRequestId /*aId*/ )
    {
    TRACER("CGlxBitmapDecoderWrapper::ThumbnailPreviewReady");    
    }

// -----------------------------------------------------------------------------
// CGlxBitmapDecoderWrapper::ThumbnailReady
// From MThumbnailManagerObserver
// -----------------------------------------------------------------------------
//                            
void CGlxBitmapDecoderWrapper::ThumbnailReady( TInt aError, 
                     MThumbnailData& aThumbnail, 
                     TThumbnailRequestId /*aId*/ )
    {
    TRACER("CGlxBitmapDecoderWrapper::ThumbnailReady");
#ifdef _DEBUG
    iStopTime.HomeTime();
    GLX_DEBUG2("*** CGlxBitmapDecoderWrapper::ThumbnailReady() TNM took <%d> us to decode***",
            (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif                          

    if (aError == KErrNone)
        {
        if (iBitmap)
            {
            delete iBitmap;
            iBitmap = NULL;
            }
        iBitmap = aThumbnail.DetachBitmap();
        iObserver->HandleBitmapDecodedL(iThumbnailIndex, iBitmap);
        iBitmap = NULL;
		}
    }

// -----------------------------------------------------------------------------
// CGlxBitmapDecoderWrapper::CancelRequest()
// Cancels all the pending requests and release resources
// -----------------------------------------------------------------------------
//
void CGlxBitmapDecoderWrapper::CancelRequest()
    {
    TRACER("CGlxBitmapDecoderWrapper::CancelRequest");
    if (iTnManager)
        {
        iTnManager->CancelRequest(iTnReqId);
        delete iTnManager;
        iTnManager = NULL;
        }

    if (iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    }

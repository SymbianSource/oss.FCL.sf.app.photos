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
 * Description:    DRM Gif Texture creator implementation
 *
 */

#include <e32math.h>
#include <imageconversion.h> 
#include <glxtracer.h>
#include <glxlog.h>
#include <alf/ialfwidgeteventhandler.h>     //  The interface for event handlers used by widget controls
#include <mul/imulmodel.h>                  // An interface for the data model
#include <alf/alfutil.h>                        // AlfUtil
#include <glxmedia.h>

#include "glxtexturemanager.h"
#include "glxbinding.h"
#include "glxuiutility.h"
#include "glxdrmgiftexturecreator.h"
#include "glxdrmgifactivedecoder.h"
#include <glxdrmutility.h>
#include <glximageviewermanager.h>

// Default frame interval for animation, in microseconds
const TInt KDefaultFrameInterval = 100000;

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
CGlxDrmGifTextureCreator* CGlxDrmGifTextureCreator::NewL(
        const CGlxBinding& aBinding, const TGlxMedia& aMedia,
        TInt aItemIndex, Alf::IMulModel* aModel, MGlxMediaList& aMediaList)
    {
    TRACER("CGlxDrmGifTextureCreator* CGlxDrmGifTextureCreator::NewL()");
    CGlxDrmGifTextureCreator* self = new (ELeave) CGlxDrmGifTextureCreator(
            aBinding, aMedia, aItemIndex, aModel, aMediaList);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor 
// -----------------------------------------------------------------------------
CGlxDrmGifTextureCreator::~CGlxDrmGifTextureCreator()
    {
    TRACER("CGlxDrmGifTextureCreator::~CGlxDrmGifTextureCreator()");
    ReleaseContent();

    // Delete the animation timer
    if (iAnimationTimer)
        {
        iAnimationTimer->Cancel();
        delete iAnimationTimer;
        }

    iUiUtility->Close();
    if (iDrmUtility)
        {
        iDrmUtility->Close();
        }
    delete iGlxDecoderAO;
    iFsSession.Close();
    }

// -----------------------------------------------------------------------------
// ReleaseContent 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::ReleaseContent()
    {
    TRACER("void CGlxDrmGifTextureCreator::ReleaseContent()");
    iBitmapReady = EFalse;
    iAnimCount = 0;
    iAnimateFlag = EFalse;
    iTransparencyPossible = EFalse;
    iFrameShift = EFalse;

    //delete image viewer instance, if present.
    if ( iImageViewerInstance )
        {
        iImageViewerInstance->DeleteInstance();
        }
    
    if (iGlxDecoderAO)
        {
        iGlxDecoderAO->Cancel();
        }

    if (iAnimationTimer)
        {
        iAnimationTimer->Cancel();
        }

    for (TInt i = 0; i < iFrameCount; i++)
        {
        GLX_LOG_INFO1("DrmGif: ReleaseContent() Releasing AnimBitmaps %d", i);
        delete (iDecodedBitmap[i]);
        iDecodedBitmap[i] = NULL;
        delete (iDecodedMask[i]);
        iDecodedMask[i] = NULL;
        }

    if (iUiUtility && iMedia)
        {
        iUiUtility->GlxTextureManager().RemoveTexture(iMedia->Id());
        }

    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CGlxDrmGifTextureCreator::CGlxDrmGifTextureCreator(
        const CGlxBinding& aBinding, const TGlxMedia& aMedia,
        TInt aItemIndex, Alf::IMulModel* aModel, MGlxMediaList& aMediaList) :
    iBinding(&aBinding), iMedia(&aMedia), iModel(aModel), iItemIndex(
            aItemIndex), iMediaList(aMediaList)
    {
    TRACER("CGlxDrmGifTextureCreator::CGlxDrmGifTextureCreator()");
    // Implement nothing here
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::ConstructL()
    {
    TRACER("CGlxDrmGifTextureCreator::ConstructL()");
    iUiUtility = CGlxUiUtility::UtilityL();
    User::LeaveIfError(iFsSession.Connect());
    
    //Create DRM Utility to check DRM rights validity 
    iDrmUtility = CGlxDRMUtility::InstanceL();
    // Create the active object
    iGlxDecoderAO = CGlxDRMgifDecoderAO::NewL(this);
    
    //Set the initial texture.And create and starts the Image Decoder
    SetInitialTextureAndStartDecodingL();
    }

// -----------------------------------------------------------------------------
// UpdateNewImageL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::UpdateNewImageL(const TGlxMedia& aMedia,
        TInt aItemIndex)
    {
    TRACER("CGlxDrmGifTextureCreator::UpdateNewImageL()");
    GLX_LOG_INFO1("DrmGif: UpdateNewImageL() aItemIndex=%d", aItemIndex);
    
    //Start Decoding only if the aItemIndex refers to new Item index
    if (aItemIndex == iItemIndex)
        {
        //All textures were flushed when in background.
        //so, when app. comes to foreground again and DRM Rights have expired for 
        //current item, then create default Texture.
        if(iUiUtility->GetForegroundStatus() && IsDRMRightsExpiredL())
            {
            SetTexture();
            }
        return;
        }

    // First release the contents before proceeding further
    ReleaseContent();

    iItemIndex = aItemIndex;
    iMedia = &aMedia;

    //Set the initial texture.And create and starts the Image Decoder
    SetInitialTextureAndStartDecodingL();
    }

// -----------------------------------------------------------------------------
// SetInitialTextureAndStartDecodingL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::SetInitialTextureAndStartDecodingL()
    {
    TRACER("CGlxDrmGifTextureCreator::SetInitialTextureAndStartDecodingL()");
    
    iBitmapReady = EFalse;
    iAnimCount = 0;
    iAnimateFlag = EFalse;
    iTransparencyPossible = EFalse;
    iFrameShift = EFalse;

    //Creates the image viewer instance, if not created already.
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();    
    __ASSERT_ALWAYS(iImageViewerInstance, Panic(EGlxPanicNullPointer));
    
    //Set the initial texture, it could be default or the FS texture
    SetTexture();
#ifdef _DEBUG
    iStartTime.HomeTime();
#endif
    //Check if DRM Rights are not expired before starting image decoding
    if (!IsDRMRightsExpiredL())
        {
        CreateImageDecoderL(iMedia->Uri());
        CreateBitmapAndStartDecodingL();
        }  
    }

// -----------------------------------------------------------------------------
// AnimateDRMGifItem 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::AnimateDRMGifItem(TBool aAnimate)
    {
    TRACER("CGlxDrmGifTextureCreator::AnimateDRMGifItem()");
    if (!iAnimationTimer)
        {
        return;
        }

    if (aAnimate && iBitmapReady)
        {
        if (!iAnimationTimer->IsActive())
            {
            GLX_LOG_INFO1("DrmGif: AnimateDRMGifItem() - iAnimCount=%d", iAnimCount);
            GLX_LOG_INFO1("DrmGif: AnimateDRMGifItem() - Frame Interval <%d> us",
                    (TInt)iFrameInfo.iDelay.Int64());
            TInt interval =((TInt)iFrameInfo.iDelay.Int64()) ? 
                ((TInt)iFrameInfo.iDelay.Int64()) : KDefaultFrameInterval;
            GLX_LOG_INFO1("DrmGif: AnimateDRMGifItem() interval=<%d> us", interval);
            iAnimationTimer->Start(interval, interval, TCallBack(TimerCallbackL, this));
            }
        iAnimateFlag = ETrue;
        }
    else
        {
        if (iAnimationTimer->IsActive())
            {
            iAnimationTimer->Cancel();
            iAnimateFlag = EFalse;
            }
        }
    }

// -----------------------------------------------------------------------------
// RefreshL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::RefreshL()
    {
    TRACER("CGlxDrmGifTextureCreator::RefreshL()");
    GLX_LOG_INFO2("DrmGif: RefreshL() iAnimCount=%d, iFrameShift=%d",
            iAnimCount, iFrameShift);
    TInt textureId = KErrNotFound;
    if(iMediaList.Count())
        {
        const TGlxMedia media = iMediaList.Item(iItemIndex);

        if (iTransparencyPossible && !iFrameShift)
            {
            textureId
                    = (iUiUtility->GlxTextureManager().CreateDRMAnimatedGifTextureL(
                            media, media.IdSpaceId(), iAnimCount,
                            iDecodedBitmap[iAnimCount],
                            iDecodedMask[iAnimCount])).Id();
            }
        else
            {
            textureId
                    = (iUiUtility->GlxTextureManager().CreateDRMAnimatedGifTextureL(
                            media, media.IdSpaceId(), iAnimCount,
                            iDecodedBitmap[iAnimCount], NULL)).Id();
            }
        }

    SetTexture(textureId);
    // Advance animation
    iAnimCount++;
    // if animation count is becoming maximum, then reset to animate again 
    if (iAnimCount >= iFrameCount)
        {
        GLX_LOG_INFO("DrmGif: RefreshL() Reset iAnimCount");
        iAnimCount = 0;
        }
    }

// -----------------------------------------------------------------------------
// CreateBitmapAndStartDecodingL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::CreateBitmapAndStartDecodingL()
    {
    TRACER("CGlxDrmGifTextureCreator::CreateBitmapAndStartDecodingL()");
    GLX_LOG_INFO1("CreateBitmapAndDecodingL() iAnimCount=%d", iAnimCount);
    // Create the bitmap and mask as of original image size, and let the 
    // coverflow widget do the scaling, if required.
    // This is needed for the transparent gifs frames as the
    // frame co-ordinates would mismatch if downscaling is applied.
    TSize frameSize = iImageDecoder->FrameInfo(iAnimCount).iFrameSizeInPixels;
    GLX_LOG_INFO3("DrmGif: CreateBitmapAndStartDecodingL() - Frame[%d] size=%d,%d",
            iAnimCount, frameSize.iWidth, frameSize.iHeight);

    iDecodedBitmap[iAnimCount] = new (ELeave) CFbsBitmap();
    iDecodedBitmap[iAnimCount]->Create(frameSize,
            iFrameInfo.iFrameDisplayMode);
    User::LeaveIfNull(iDecodedBitmap[iAnimCount]);

    if (iFrameInfo.iFlags & TFrameInfo::ETransparencyPossible)
        {
        iDecodedMask[iAnimCount] = new (ELeave) CFbsBitmap();
        iDecodedMask[iAnimCount]->Create(frameSize, iFrameInfo.iFlags
                & TFrameInfo::EAlphaChannel ? EGray256 : EGray2);
        User::LeaveIfNull(iDecodedMask[iAnimCount]);

        // decoding the image
        iGlxDecoderAO->ConvertImageL(iDecodedBitmap[iAnimCount],
                iDecodedMask[iAnimCount], iAnimCount, iImageDecoder);
        iTransparencyPossible = ETrue;
        }
    else
        {
        // decoding the image
        iGlxDecoderAO->ConvertImageL(iDecodedBitmap[iAnimCount], NULL,
                iAnimCount, iImageDecoder);
        }
    }

// -----------------------------------------------------------------------------
// HandleRunL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::HandleRunL(TRequestStatus& aStatus)
    {
    TRACER("CGlxDrmGifTextureCreator::HandleRunL()");
    TInt err = aStatus.Int();
    GLX_LOG_INFO1("DrmGif: HandleRunL : err=%d", err);
    if (err != KErrNone)
        {
        ReleaseContent();
        return;
        }

    GLX_LOG_INFO2("DrmGif: HandleRunL() - Frame=%d/%d",
            iAnimCount, iFrameCount-1);
    if (iAnimCount > 0 && iAnimCount < iFrameCount)
        {
        TPoint point =
                iImageDecoder->FrameInfo(iAnimCount).iFrameCoordsInPixels.iTl;
        GLX_LOG_INFO2("DrmGif: HandleRunL() point=(%d, %d)",
                point.iX, point.iY );
        TSize frameSize = iImageDecoder->FrameInfo(iAnimCount).iFrameSizeInPixels;
        GLX_LOG_INFO2("DrmGif: HandleRunL() - frameSize(%d, %d)",
                frameSize.iWidth, frameSize.iHeight);
        // Frame shift is checked,
        // 1) If the subsequent frame sizes differ from the first frame (or)
        // 2) If the subsequent frame co-ordinates differ from the first frame
        if (point != iFrameInfo.iFrameCoordsInPixels.iTl
                || iFrameInfo.iFrameSizeInPixels != frameSize)
            {
            iFrameShift = ETrue;
            }

        if (iFrameShift)
            {
            TSize firstFrameSize = iDecodedBitmap[0]->SizeInPixels();
            GLX_LOG_INFO2("DrmGif: HandleRunL() - first bitmap size (%d, %d)",
                    firstFrameSize.iWidth, firstFrameSize.iHeight);

            TDisplayMode dispMode = iDecodedBitmap[0]->DisplayMode();
            TInt scanLineLength = CFbsBitmap::ScanLineLength(
                    firstFrameSize.iWidth, dispMode);

            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
            CleanupStack::PushL(bitmap);
            User::LeaveIfError(bitmap->Create(firstFrameSize, dispMode));
            bitmap->LockHeap();
            iDecodedBitmap[0]->LockHeap();
            if (bitmap && bitmap->DataAddress())
                {
                memcpy((void*) bitmap->DataAddress(),
                        (void*) iDecodedBitmap[0]->DataAddress(),
                        scanLineLength * firstFrameSize.iHeight);
                }
            iDecodedBitmap[0]->UnlockHeap();
            bitmap->UnlockHeap();

            CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
            CleanupStack::PushL(bitmapDevice);

            CFbsBitGc* bitmapGc = CFbsBitGc::NewL();
            CleanupStack::PushL(bitmapGc);
            bitmapGc->Activate(bitmapDevice);

            if (iTransparencyPossible)
                {
                GLX_LOG_INFO("DrmGif: HandleRunL() BitBltMasked");
                bitmapGc->BitBltMasked(point, iDecodedBitmap[iAnimCount],
                        iOrigImageDimensions, iDecodedMask[iAnimCount],
                        EFalse);
                }
            else
                {
                GLX_LOG_INFO("DrmGif: HandleRunL() BitBlt");
                bitmapGc->BitBlt(point, iDecodedBitmap[iAnimCount]);
                }

            delete iDecodedBitmap[iAnimCount];
            iDecodedBitmap[iAnimCount] = bitmap;
            CleanupStack::PopAndDestroy(bitmapGc);
            CleanupStack::PopAndDestroy(bitmapDevice);
            CleanupStack::Pop(bitmap);
            }
        }

    if (iAnimCount < iFrameCount - 1)
        {
        if (!iGlxDecoderAO->IsActive())
            {
            iAnimCount++;
            CreateBitmapAndStartDecodingL();
            }
        }
    else
        {
#ifdef _DEBUG
        iStopTime.HomeTime();
        GLX_LOG_INFO1("DrmGif: HandleRunL() ConvertImageL took <%d> us",
                (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif
        iBitmapReady = ETrue;
        iAnimateFlag = ETrue;
        iAnimCount = 0;
        ProcessImageL();

        //release imagedecoder after the conversion is over     
        if (iImageDecoder)
            {
            delete iImageDecoder;
            iImageDecoder = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// ProcessImageL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::ProcessImageL()
    {
    TRACER("CGlxDrmGifTextureCreator::ProcessImageL()");
    RefreshL();
    iAnimationTimer->Cancel();
    if (iAnimateFlag)
        {
        GLX_LOG_INFO1("DrmGif: ProcessImageL() - Frame Interval <%d> us",
                (TInt)iFrameInfo.iDelay.Int64());
        TInt interval =((TInt)iFrameInfo.iDelay.Int64()) ? 
            ((TInt)iFrameInfo.iDelay.Int64()) : KDefaultFrameInterval;
        GLX_LOG_INFO1("DrmGif: ProcessImageL() interval=<%d> us", interval);
        iAnimationTimer->Start(interval, interval, TCallBack(TimerCallbackL,
                this));
        }
    }

// -----------------------------------------------------------------------------
// CreateImageDecoderL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::CreateImageDecoderL(const TDesC& aImageFile)
    {
    TRACER("CGlxDrmGifTextureCreator::CreateImageDecoderL()");
    GLX_LOG_URI("DrmGif::CreateImageDecoderL(%S)", &aImageFile);

    CImageDecoder::TOptions options =
            (CImageDecoder::TOptions) (CImageDecoder::EOptionNoDither
                    | CImageDecoder::EOptionAlwaysThread);
    // Create a decoder for the image in the named file
    TRAPD(error,iImageDecoder = CImageDecoder::FileNewL(iFsSession,
                    aImageFile, options, KNullUid));
    if (error != KErrNone)
        {
        User::Leave(error);
        }
    iFrameInfo = iImageDecoder->FrameInfo();
    iOrigImageDimensions = iImageDecoder->FrameInfo().iOverallSizeInPixels;
    GLX_LOG_INFO1("DrmGif::CreateImageDecoderL() - Gif Frame Interval <%d> us",
            (TInt)iFrameInfo.iDelay.Int64());
    iFrameCount = iImageDecoder->FrameCount();

    // We are creating array of KGlxMaxFrameCount frames
    // So re-setting the array-count with the no.
    // It will animate till that no. of frames.
    if (iFrameCount > KGlxMaxFrameCount)
        {
        iFrameCount = KGlxMaxFrameCount;
        }
    //dont create the timer if it is a singleframe.no need to animate
    if (iFrameCount > 1)
        {
        iAnimationTimer = CPeriodic::NewL(CActive::EPriorityLow);
        }
    }

// -----------------------------------------------------------------------------
// TimerCallbackL 
// -----------------------------------------------------------------------------
TInt CGlxDrmGifTextureCreator::TimerCallbackL(TAny* aThis)
    {
    TRACER("CGlxDrmGifTextureCreator::TimerCallbackL()");
    static_cast<CGlxDrmGifTextureCreator*> (aThis)->ProcessTimerEventL();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ProcessTimerEventL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::ProcessTimerEventL()
    {
    TRACER("CGlxDrmGifTextureCreator::ProcessTimerEventL()");
    ProcessImageL();
    }

// -----------------------------------------------------------------------------
// SetTexture 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::SetTexture(TInt aTextureId)
    {
    TRACER("CGlxDrmGifTextureCreator::SetTexture()");
    auto_ptr<MulVisualItem> item(new (EMM) MulVisualItem());
    if (iMediaList.Count())
        {
        iBinding->PopulateT(*item, iMediaList.Item(iItemIndex), ETrue, aTextureId);
        iModel->SetData(iItemIndex, item);
        }
    }

// -----------------------------------------------------------------------------
// IsDRMRightsExpiredL
// -----------------------------------------------------------------------------
//
TBool CGlxDrmGifTextureCreator::IsDRMRightsExpiredL()
    {
    TRACER("CGlxDrmGifTextureCreator::IsDRMRightsExpiredL");
    //To check if DRM rights are expired
    TBool expired = EFalse;
    TMPXGeneralCategory cat = iMedia->Category();
    
    if (iImageViewerInstance->IsPrivate())
        {
        expired = !iDrmUtility->DisplayItemRightsCheckL
                            (iImageViewerInstance->ImageFileHandle(),(cat == EMPXImage));
        }
    else
        {
        expired = !iDrmUtility->DisplayItemRightsCheckL(iMedia->Uri(), (cat == EMPXImage));
        }
    return expired;
    }


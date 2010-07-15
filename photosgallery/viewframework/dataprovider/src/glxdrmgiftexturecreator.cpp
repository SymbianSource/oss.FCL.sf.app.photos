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
 * Description:    Gif Texture creator implementation
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

const TInt KTimerInterval = 200000;

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
CGlxDrmGifTextureCreator* CGlxDrmGifTextureCreator::NewL(
        const CGlxBinding& aBinding, const TGlxMedia& aMedia,
        TInt aItemIndex, Alf::IMulModel* aModel)
    {
    TRACER("CGlxDrmGifTextureCreator* CGlxDrmGifTextureCreator::NewL()");
    CGlxDrmGifTextureCreator* self = new (ELeave) CGlxDrmGifTextureCreator(
            aBinding, aMedia, aItemIndex, aModel);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// destructor 
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

    delete iGlxDecoderAO;
	
    iFsSession.Close();
    }

// -----------------------------------------------------------------------------
// ReleaseContent 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::ReleaseContent()
    {
    TRACER("void CGlxDrmGifTextureCreator::ReleaseContent()");
    if (iGlxDecoderAO)
        {
        iGlxDecoderAO->Cancel();
        }

    for (TInt i = 0; i < iFrameCount; i++)
        {
        GLX_LOG_INFO1("CGlxDrmGifTextureCreator::ReleaseContent(). Releasing AnimBitmaps %d", i);
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
        TInt aItemIndex, Alf::IMulModel* aModel) :
    iBinding(&aBinding), iMedia(&aMedia), iModel(aModel), iItemIndex(
            aItemIndex)
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
    iBitmapReady = EFalse;
    iAnimCount = 0;
    iAnimateFlag = EFalse;
    
    //Set the initial texture, it could be default or the FS texture
    SetTexture();
    // Create the active object
    iGlxDecoderAO = CGlxDRMgifDecoderAO::NewL(this);
#ifdef _DEBUG
    iStartTime.HomeTime();
#endif	
    CreateImageDecoderL(iMedia->Uri());
    CreateBitmapAndStartDecodingL();
    }

// -----------------------------------------------------------------------------
// UpdateNewImageL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::UpdateNewImageL(const TGlxMedia& aMedia,
        TInt aItemIndex)
    {
    TRACER("CGlxDrmGifTextureCreator::UpdateNewImageL()");
    GLX_LOG_INFO1("CGlxDrmGifTextureCreator::UpdateNewImageL() aItemIndex=%d", aItemIndex);    
	if(aItemIndex == iItemIndex)
        {
        return;
        }
	iTransparencyPossible = EFalse;
	iItemIndex = aItemIndex;
    iMedia = &aMedia;
    // First release the contents before proceeding further
    ReleaseContent();

    iBitmapReady = EFalse;
    iAnimCount = 0;
    iAnimateFlag = EFalse;
    //Set the initial texture, it could be default or the FS texture
    SetTexture();
#ifdef _DEBUG
    iStartTime.HomeTime();
#endif
    CreateImageDecoderL(iMedia->Uri());
    CreateBitmapAndStartDecodingL();
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
            GLX_LOG_INFO1("CGlxDrmGifTextureCreator::AnimateDRMGifItem() - Gif iAnimCount =%d", iAnimCount);
            GLX_LOG_INFO1("=>CGlxDrmGifTextureCreator::AnimateDRMGifItem() - Gif Frame Interval <%d> us",
                    (TInt)iFrameInfo.iDelay.Int64());
            TInt interval =((TInt)iFrameInfo.iDelay.Int64())?((TInt)iFrameInfo.iDelay.Int64())
                                                                                    :KTimerInterval;
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
    GLX_LOG_INFO1("CGlxDrmGifTextureCreator::RefreshL() iAnimCount = %d",iAnimCount);
    TInt textureId = KErrNotFound;
    if (iTransparencyPossible)
        {
        textureId
                = (iUiUtility->GlxTextureManager().CreateDRMAnimatedGifTextureL(
                        *iMedia, iMedia->IdSpaceId(), iAnimCount,
                        iDecodedBitmap[iAnimCount], iDecodedMask[iAnimCount])).Id();
        }
    else
        {
        textureId
                = (iUiUtility->GlxTextureManager().CreateDRMAnimatedGifTextureL(
                        *iMedia, iMedia->IdSpaceId(), iAnimCount,
                        iDecodedBitmap[iAnimCount], NULL)).Id();
        }

    SetTexture(textureId);
    iAnimCount++;
    // Advance animation if the animation count is becoming maximum, 
	// then set it to zero, such that it can animate again frm begining
    if (iAnimCount >= iFrameCount)
        {
        GLX_LOG_INFO("CGlxDrmGifTextureCreator::RefreshL() Reset iAnimCount");
        iAnimCount = 0;
        }    
    }

// -----------------------------------------------------------------------------
// CreateBitmapAndStartDecodingL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::CreateBitmapAndStartDecodingL()
    {
    TRACER("CGlxDrmGifTextureCreator::CreateBitmapAndStartDecodingL()");
    TSize scrnSize = AlfUtil::ScreenSize();
    TSize targetBitmapSize;

    GLX_LOG_INFO2("CGlxDrmGifTextureCreator::CreateBitmapAndDecodingL() - bitmapsize=%d, %d",
            iOrigImageDimensions.iWidth,iOrigImageDimensions.iHeight);
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
    GLX_LOG_INFO1("CGlxDrmGifTextureCreator::CreateBitmapAndDecodingL() - scaleFactor=%f",scaleFactor);
    targetBitmapSize.iHeight = iOrigImageDimensions.iHeight * scaleFactor;
    targetBitmapSize.iWidth = iOrigImageDimensions.iWidth * scaleFactor;
    GLX_LOG_INFO2("CGlxDrmGifTextureCreator::CreateBitmapAndDecodingL() - targetBitmapSize=%d, %d",
            targetBitmapSize.iWidth,targetBitmapSize.iHeight);
    GLX_LOG_INFO1("CGlxDrmGifTextureCreator::CreateBitmapAndDecodingL() iAnimCount =%d", iAnimCount);

    //create the bitmap for the required size
    iDecodedBitmap[iAnimCount] = new (ELeave) CFbsBitmap();
    iDecodedBitmap[iAnimCount]->Create(ReCalculateSizeL(targetBitmapSize),
            iFrameInfo.iFrameDisplayMode);
    User::LeaveIfNull(iDecodedBitmap[iAnimCount]);

    if (iFrameInfo.iFlags & TFrameInfo::ETransparencyPossible)
        {    
        iDecodedMask[iAnimCount] = new (ELeave) CFbsBitmap();
        iDecodedMask[iAnimCount]->Create(ReCalculateSizeL(
                targetBitmapSize), iFrameInfo.iFlags
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
    iAnimCount++;    
    }

// -----------------------------------------------------------------------------
// HandleRunL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::HandleRunL(TRequestStatus& aStatus)
    {
    TRACER("CGlxDrmGifTextureCreator::HandleRunL()");
    GLX_LOG_INFO2("CGlxDrmGifTextureCreator::HandleRunL() - gif image frame=%d/%d",
                                                                     iAnimCount,iFrameCount);

    if (iAnimCount < iFrameCount  )
        {
        if (!iGlxDecoderAO->IsActive())
            {          
            CreateBitmapAndStartDecodingL();
            }
        }
    else
        {
#ifdef _DEBUG
        iStopTime.HomeTime();
        GLX_LOG_INFO1("CGlxDrmGifTextureCreator::HandleRunL() ConvertImageL took"
                " <%d> us", (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif
        TInt err = aStatus.Int();
        GLX_LOG_INFO1("CGlxDrmGifTextureCreator::HandleRunL : err=%d", err);

        if (err == KErrNone)
            {
            iBitmapReady = ETrue;
            iAnimateFlag = ETrue;
            iAnimCount = 0;
            ProcessImageL();
            }
        
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

    GLX_LOG_INFO1("CGlxDrmGifTextureCreator::ProcessImageL() iAnimCount =%d", iAnimCount);
    GLX_LOG_INFO1("=>CGlxDrmGifTextureCreator::ProcessImageL() - Gif Frame Interval <%d> us",
            (TInt)iFrameInfo.iDelay.Int64());
    iAnimationTimer->Cancel();
    if (iAnimateFlag)
        {
        // Next frame
        TInt interval =((TInt)iFrameInfo.iDelay.Int64())?((TInt)iFrameInfo.iDelay.Int64())
                                                                           :KTimerInterval;
        iAnimationTimer->Start(interval,interval, TCallBack(TimerCallbackL, this));
        }
    }

// -----------------------------------------------------------------------------
// CreateImageDecoderL 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::CreateImageDecoderL(const TDesC& aImageFile)
    {
    TRACER("CGlxDrmGifTextureCreator::CreateImageDecoderL()");
    GLX_LOG_URI("CGlxDrmGifTextureCreator::CreateImageDecoderL(%S)", &aImageFile);

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
    GLX_LOG_INFO1("=>CGlxDrmGifTextureCreator::CreateImageDecoderL() - Gif Frame Interval <%d> us",
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
// ReCalculateSize 
// -----------------------------------------------------------------------------
TSize CGlxDrmGifTextureCreator::ReCalculateSizeL(TSize& aTargetBitmapSize)
    {
    TRACER("CGlxDrmGifTextureCreator::ReCalculateSizeL()");
    // calculate the reduction factor on what size we need
    TInt reductionFactor = iImageDecoder->ReductionFactor(iOrigImageDimensions,
            aTargetBitmapSize);
    // get the reduced size onto destination size
    TSize destSize;
    User::LeaveIfError(iImageDecoder->ReducedSize(iOrigImageDimensions,
            reductionFactor, destSize));
    GLX_LOG_INFO2("CGlxDrmGifTextureCreator::ReCalculateSizeL() destSize=%d, %d",
                                                    destSize.iWidth,destSize.iHeight);
    return destSize;
    }

// -----------------------------------------------------------------------------
// SetTexture 
// -----------------------------------------------------------------------------
void CGlxDrmGifTextureCreator::SetTexture(TInt aTextureId)
    {
    TRACER("CGlxDrmGifTextureCreator::SetTexture()");
    auto_ptr<MulVisualItem> item(new (EMM) MulVisualItem());
    iBinding->PopulateT(*item, *iMedia, ETrue, aTextureId);
    iModel->SetData(iItemIndex, item);
    }

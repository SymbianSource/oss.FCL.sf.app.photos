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
 * Description:    Gif Decoder Active Object
 *
 */

#include <glxtracer.h>
#include <glxlog.h>
#include "glxdrmgifactivedecoder.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
CGlxDRMgifDecoderAO* CGlxDRMgifDecoderAO::NewL(
        CGlxDrmGifTextureCreator* aDrmGifTextureCreator)
    {
    TRACER("CGlxDRMgifDecoderAO::NewL()");
    CGlxDRMgifDecoderAO* self = new (ELeave) CGlxDRMgifDecoderAO(
            aDrmGifTextureCreator);
    return self;
    }

// -----------------------------------------------------------------------------
// CGlxDRMgifDecoderAO()
// -----------------------------------------------------------------------------
CGlxDRMgifDecoderAO::CGlxDRMgifDecoderAO(
        CGlxDrmGifTextureCreator* aDrmGifTextureCreator) :
    CActive(CActive::EPriorityStandard), iDrmGifTextureCreator(
            aDrmGifTextureCreator)
    {
    TRACER("CGlxDRMgifDecoderAO::CGlxDRMgifDecoderAO()");
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// ~CGlxDRMgifDecoderAO
// Cancel the outstanding request
// -----------------------------------------------------------------------------
CGlxDRMgifDecoderAO::~CGlxDRMgifDecoderAO()
    {
    TRACER("CGlxDRMgifDecoderAO::~CGlxDRMgifDecoderAO()");
    Cancel();
    }

// -----------------------------------------------------------------------------
// RunL
// Calls iDrmGifTextureCreator->HandleRunL() which is expected to handle the call
// -----------------------------------------------------------------------------
void CGlxDRMgifDecoderAO::RunL()
    {
    TRACER("CGlxDRMgifDecoderAO::RunL()");
    if (iStatus == KErrUnderflow)
        {
        GLX_LOG_INFO("CGlxDRMgifDecoderAO::RunL() - call ContinueConvert");
        iDecoder->ContinueConvert(&iStatus);
        }
    else
        {
        GLX_LOG_INFO("CGlxDRMgifDecoderAO::RunL() - call HandleRunL");
        iDrmGifTextureCreator->HandleRunL(iStatus);
        }
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
void CGlxDRMgifDecoderAO::DoCancel()
    {
    TRACER("CGlxDRMgifDecoderAO::DoCancel()");
    iDecoder->Cancel();
    }

// -----------------------------------------------------------------------------
// ConvertImageL
// -----------------------------------------------------------------------------
void CGlxDRMgifDecoderAO::ConvertImageL(CFbsBitmap* aBitmap,
        CFbsBitmap* aBitmapMask, TInt aFrameNum, CImageDecoder* aDecoder)
    {
    TRACER("CGlxDRMgifDecoderAO::ConvertImageL()");
    iDecoder = aDecoder;
    GLX_LOG_INFO1("CGlxDRMgifDecoderAO::ConvertImageL() aFrameNum=%d", aFrameNum);
    if (aBitmapMask)
        {
        iDecoder->Convert(&iStatus, *aBitmap, *aBitmapMask, aFrameNum);
        }
    else
        {
        iDecoder->Convert(&iStatus, *aBitmap, aFrameNum);
        }
    SetActive();
    }
//end of file	

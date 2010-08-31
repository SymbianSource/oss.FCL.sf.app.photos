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
* Description:   Classes for thumbnail-related tasks.
*
*/



/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnquickthumbnailtask.h"

#include <ExifRead.h>
#include <glxassert.h>
#include <glxtracer.h>
#include <imageconversion.h>

#include "glxtnfileinfo.h"
#include "glxtnfileutility.h"
#include "glxtnimagedecoderfactory.h"
#include "glxtnthumbnailrequest.h"
#include "glxtnvideoutility.h"
#include "mglxtnthumbnailcreatorclient.h"


// All EXIF data is within the first KGlxMaxExifSize bytes of the file
const TInt KGlxMaxExifSize = 0x10000;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxtnQuickThumbnailTask* CGlxtnQuickThumbnailTask::NewL(
        const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient)
    {
    TRACER("CGlxtnQuickThumbnailTask::NewL()");
    CGlxtnQuickThumbnailTask* task = new (ELeave) CGlxtnQuickThumbnailTask(
                                        aRequestInfo, aFileUtility, aClient);
    CleanupStack::PushL(task);
    task->ConstructL(aRequestInfo.iBitmapHandle);
    CleanupStack::Pop( task );
    return task;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnQuickThumbnailTask::CGlxtnQuickThumbnailTask(
                                    const TGlxThumbnailRequest& aRequestInfo,
                                    CGlxtnFileUtility& aFileUtility,
                                    MGlxtnThumbnailCreatorClient& aClient ) :
    CGlxtnLoadThumbnailTask( KGlxtnTaskIdQuickThumbnail,
                            aRequestInfo, aFileUtility, aClient ),
    iQuality( EGlxThumbnailQualityLow )
    {
    TRACER("CGlxtnQuickThumbnailTask::CGlxtnQuickThumbnailTask()");
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnQuickThumbnailTask::~CGlxtnQuickThumbnailTask() 
    {
    TRACER("CGlxtnQuickThumbnailTask::~CGlxtnQuickThumbnailTask()");
    delete iUtility;
    delete iVideoFrame;
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnQuickThumbnailTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnQuickThumbnailTask::DoStartL()");
    iInfo = new (ELeave) CGlxtnFileInfo;
	Client().FetchFileInfoL(iInfo, ItemId(), &aStatus);
    iState = EStateFetchingUri;

    return ETrue;
    } 

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnQuickThumbnailTask::DoCancel() 
    {
    TRACER("void CGlxtnQuickThumbnailTask::DoCancel()");
    CGlxtnLoadThumbnailTask::DoCancel();

    if ( EStateFetchingUri == iState )
        {
        Client().CancelFetchUri( ItemId() );
        }

    if ( iUtility )
        {
        iUtility->Cancel();
        }

    iFileUtility.ClearBadFileMarker();
    }

// -----------------------------------------------------------------------------
// DoRunL
// -----------------------------------------------------------------------------
//
TBool CGlxtnQuickThumbnailTask::DoRunL(TRequestStatus& aStatus) 
    {
    TRACER("TBool CGlxtnQuickThumbnailTask::DoRunL()");
    TInt error = aStatus.Int();
	TBool active = EFalse;

    if ( KErrNone == error )
        {
        switch ( iState )
            {
            case EStateFetchingUri:
                User::LeaveIfNull(iInfo);
                iBadFileMarkerNotNeededFlag = ETrue;

                TBool protectedFile;
                iInfo->IdentifyFileL(iVideo, protectedFile);
                
                // leave if file is DRM protected and client has no DRM 
                // capablity
                if ( protectedFile && !iDrmAllowed )
                    {
                    User::Leave(KErrAccessDenied);
                    }

                if ( !LoadThumbnailL( aStatus ) )
                    {
                    iBadFileMarkerNotNeededFlag = EFalse;
                    iFileUtility.CheckBadFileListL( iInfo->FilePath() );
                    QuickDecodeL(aStatus);
                    }
                active = ETrue;
                break;

            case EStateLoading:
                iQuality = EGlxThumbnailQualityHigh;
                active = HandleLoadedThumbnailL(aStatus);
                break;

            case EStateDecodingThumbnail:
                // Nothing to do
                break;

            case EStateDecodingImage:
                QuickScaleL();
                break;

            default:
                GLX_ASSERT_ALWAYS( EFalse, Panic( EGlxPanicIllegalState ),
                                "CGlxtnQuickThumbnailTask: Illegal state" );
                break;
            }
        }
    else
        {
        if ( EStateLoading == iState )
            {
            // Loading failed - generate low quality thumbnail instead
            iBadFileMarkerNotNeededFlag = EFalse;
            iFileUtility.CheckBadFileListL( iInfo->FilePath() );
            QuickDecodeL(aStatus);
            active = ETrue;
            }
        }

    if ( !active )
        {
        if ( !iBadFileMarkerNotNeededFlag )
            {
            iFileUtility.ClearBadFileMarker();
            }
        Client().ThumbnailFetchComplete(ItemId(), iQuality, error);
        }
       
    return active; 
    }

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnQuickThumbnailTask::DoRunError(TInt aError)
    {
    TRACER("TBool CGlxtnQuickThumbnailTask::DoRunError(TInt aError)");
    iFileUtility.ClearBadFileMarker();
    Client().ThumbnailFetchComplete(ItemId(), iQuality, aError);

    return EFalse;
    }

// -----------------------------------------------------------------------------
// QuickDecodeL
// -----------------------------------------------------------------------------
//
void CGlxtnQuickThumbnailTask::QuickDecodeL(TRequestStatus& aStatus)
    {
    TRACER("void CGlxtnQuickThumbnailTask::QuickDecodeL(TRequestStatus& aStatus)");
    iQuality = EGlxThumbnailQualityLow;

    if ( iVideo )
        {
        RArray<TSize> targetSizes;
        CleanupClosePushL(targetSizes);
        targetSizes.AppendL( TSize(iRequestedSize) );
        iUtility = new (ELeave) CGlxtnVideoUtility;
        iUtility->GetVideoFrameL(&aStatus, iVideoFrame, iInfo->FilePath(), targetSizes, iThumbnail->DisplayMode() );
        iState = EStateDecodingImage;
        CleanupStack::PopAndDestroy(&targetSizes);
        }
    else
        {
        ReadThumbnailL(aStatus);
        }
    }

// -----------------------------------------------------------------------------
// ReadThumbnailL
// -----------------------------------------------------------------------------
//
void CGlxtnQuickThumbnailTask::ReadThumbnailL(TRequestStatus& aStatus)
    {
    TRACER("void CGlxtnQuickThumbnailTask::ReadThumbnailL(TRequestStatus& aStatus)");
    TBool scaleBitmap = EFalse;

    TRAPD(error, ReadExifThumbnailL());
    if ( KErrNotFound == error || KErrCorrupt == error
        || KErrNotSupported == error )
        {
        // Use CImageDecoder to get the EXIF thumbnail instead
        iDecoder = GlxtnImageDecoderFactory::NewL(
                                iFileUtility.FsSession(), iInfo->FilePath() );

        TRAPD( error2, iDecoder->SetImageTypeL(
                                    CImageDecoder::EImageTypeThumbnail ) );
        if ( KErrNotFound == error2 )
            {
            // No EXIF thumbnail - load whole image and scale quickly
            scaleBitmap = ETrue;
            }
        else
            {
            User::LeaveIfError( error2 );
            }
        }
    else
        {
        User::LeaveIfError(error);
        iDecoder = GlxtnImageDecoderFactory::NewL(
                                    iFileUtility.FsSession(), *iThumbData );
        }

    DecodeThumbnailL(aStatus, scaleBitmap);
    }

// -----------------------------------------------------------------------------
// ReadExifThumbnailL
// -----------------------------------------------------------------------------
//
void CGlxtnQuickThumbnailTask::ReadExifThumbnailL()
    {
    TRACER("void CGlxtnQuickThumbnailTask::ReadExifThumbnailL()");
    __ASSERT_ALWAYS(iInfo, Panic(EGlxPanicNullPointer));

    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Open(iFileUtility.FsSession(),
                                    iInfo->FilePath(), EFileRead));
    TInt size;
    User::LeaveIfError(file.Size(size));
    if ( KGlxMaxExifSize < size )
        {
        size = KGlxMaxExifSize;
        }

    HBufC8* exifData = HBufC8::NewLC(size);
    TPtr8 ptr(exifData->Des());
    User::LeaveIfError(file.Read(ptr));

    CExifRead* exifReader = CExifRead::NewL(*exifData, CExifRead::ENoJpeg);
    CleanupStack::PushL(exifReader);
    delete iThumbData;
    iThumbData = NULL;
    iThumbData = exifReader->GetThumbnailL();

    CleanupStack::PopAndDestroy(exifReader);
    CleanupStack::PopAndDestroy(exifData);
    CleanupStack::PopAndDestroy(&file);
    }

// -----------------------------------------------------------------------------
// QuickScaleL
// -----------------------------------------------------------------------------
//
void CGlxtnQuickThumbnailTask::QuickScaleL()
    {
    TRACER("void CGlxtnQuickThumbnailTask::QuickScaleL()");
    User::LeaveIfNull(iVideoFrame);

    TSize frameSize(iVideoFrame->SizeInPixels());
    TSize thumbSize(iRequestedSize);

    // Reduce target thumbSize to same aspect ratio as source image
    if ( thumbSize.iHeight * frameSize.iWidth < thumbSize.iWidth * frameSize.iHeight )
        {
        // Source has taller aspect than target so reduce target width
        thumbSize.iWidth = (thumbSize.iHeight * frameSize.iWidth) / frameSize.iHeight;
        }
    else
        {
        // Source has wider aspect than target so reduce target height
        thumbSize.iHeight = (thumbSize.iWidth * frameSize.iHeight) / frameSize.iWidth;
        }

    // Resize empty bitmap to required size
    User::LeaveIfError( iThumbnail->Resize( thumbSize ) );

    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( iThumbnail );
    CleanupStack::PushL(device);
    CFbsBitGc* context = NULL;
    User::LeaveIfError(device->CreateContext(context));
    CleanupStack::PushL(context);

    context->DrawBitmap(TRect(thumbSize), iVideoFrame);

    CleanupStack::PopAndDestroy(context); 
    CleanupStack::PopAndDestroy(device);
    }

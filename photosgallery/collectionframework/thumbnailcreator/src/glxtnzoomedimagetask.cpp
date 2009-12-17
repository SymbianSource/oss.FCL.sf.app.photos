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
* Description:    Task to generate a cropped thumbnail for a JPEG image.
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnzoomedimagetask.h"

#include <fbs.h>
#include <glxassert.h>
#include <glxlog.h>
#include <IclExtJpegApi.h>  // For CExtJpegDecoder
#include <glxtracer.h>

#include "glxtnfileinfo.h"
#include "glxtnfileutility.h"
#include "glxtnthumbnailrequest.h"
#include "mglxtnthumbnailcreatorclient.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxtnZoomedImageTask* CGlxtnZoomedImageTask::NewL(
        const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient)
    {
    TRACER("CGlxtnZoomedImageTask* CGlxtnZoomedImageTask::NewL( const TGlxThumbnailRequest& aRequestInfo, CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient)");
    CGlxtnZoomedImageTask* task = new (ELeave) CGlxtnZoomedImageTask(
                                        aRequestInfo, aFileUtility, aClient);
    CleanupStack::PushL(task);
    task->ConstructL(aRequestInfo.iBitmapHandle);
    CleanupStack::Pop( task );
    return task;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnZoomedImageTask::~CGlxtnZoomedImageTask()
    {
    TRACER("CGlxtnZoomedImageTask::~CGlxtnZoomedImageTask()");
    delete iDecoder;
    delete iInfo;
    delete iThumbnail;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnZoomedImageTask::CGlxtnZoomedImageTask(
                                    const TGlxThumbnailRequest& aRequestInfo,
                                    CGlxtnFileUtility& aFileUtility,
                                    MGlxtnThumbnailCreatorClient& aClient) :
    CGlxtnClientTask(KGlxtnTaskIdZoomedThumbnail, aRequestInfo.iId, aClient),
    iFileUtility(aFileUtility), iRequestedSize(aRequestInfo.iSizeClass),
    iCroppingRect(aRequestInfo.iCroppingRect), iFilter(aRequestInfo.iFilter),
    iDrmAllowed(aRequestInfo.iDrmAllowed)
    {
    TRACER("CGlxtnZoomedImageTask::CGlxtnZoomedImageTask( const TGlxThumbnailRequest& aRequestInfo, CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient)");
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxtnZoomedImageTask::ConstructL(TInt aBitmapHandle)
    {
    TRACER("void CGlxtnZoomedImageTask::ConstructL(TInt aBitmapHandle)");
    // Duplicate the client's bitmap
    iThumbnail = new (ELeave) CFbsBitmap();
    User::LeaveIfError(iThumbnail->Duplicate(aBitmapHandle));
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnZoomedImageTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnZoomedImageTask::DoStartL(TRequestStatus& aStatus)");
    iInfo = new (ELeave) CGlxtnFileInfo;
    Client().FetchFileInfoL(iInfo, ItemId(), &aStatus);
    iState = EStateFetchingUri;

    return ETrue;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnZoomedImageTask::DoCancel()
    {
    TRACER("void CGlxtnZoomedImageTask::DoCancel()");
    if ( EStateFetchingUri == iState )
        {
        Client().CancelFetchUri(ItemId());
        }
    if ( iDecoder )
        {
        iDecoder->Cancel();
        }

    iFileUtility.ClearBadFileMarker();
    }

// -----------------------------------------------------------------------------
// DoRunL
// -----------------------------------------------------------------------------
//
TBool CGlxtnZoomedImageTask::DoRunL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnZoomedImageTask::DoRunL(TRequestStatus& aStatus)");
    TInt error = aStatus.Int();
	TBool active = EFalse;

    if ( KErrNone == error )
        {
        switch ( iState )
            {
            case EStateFetchingUri:
                {
                User::LeaveIfNull(iInfo);
                iFileUtility.CheckBadFileListL( iInfo->FilePath() );

                TBool video;
                iInfo->IdentifyFileL(video, iProtected);
                if ( iProtected && !iDrmAllowed )
                    {
                    User::Leave(KErrAccessDenied);
                    }

                DecodePartialImageL(aStatus);
                active = ETrue;
                iState = EStateDecoding;
                }
                break;

            case EStateDecoding:
                // Nothing to do
                break;

            default:
                GLX_ASSERT_ALWAYS( EFalse, Panic( EGlxPanicIllegalState ),
                                "CGlxtnZoomedImageTask: Illegal state" );
                break;
            }
        }

    if ( !active )
        {
        iFileUtility.ClearBadFileMarker();
        Client().ThumbnailFetchComplete(ItemId(),
                                        EGlxThumbnailQualityHigh, error);
        }
       
    return active; 
    }

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnZoomedImageTask::DoRunError(TInt aError)
    {
    TRACER("TBool CGlxtnZoomedImageTask::DoRunError(TInt aError)");
    iFileUtility.ClearBadFileMarker();
    Client().ThumbnailFetchComplete(ItemId(), EGlxThumbnailQualityHigh, aError);

    return EFalse;
    }

// -----------------------------------------------------------------------------
// DecodePartialImageL
// -----------------------------------------------------------------------------
//
void CGlxtnZoomedImageTask::DecodePartialImageL(TRequestStatus& aStatus)
    {
    TRACER("void CGlxtnZoomedImageTask::DecodePartialImageL(TRequestStatus& aStatus)");
    TRAPD(err, CreateDecoderL(CExtJpegDecoder::EHwImplementation));
    if ( KErrNone != err )
        {
        CreateDecoderL(CExtJpegDecoder::ESwImplementation);
        }

    TSize sourceSize(iDecoder->FrameInfo().iOverallSizeInPixels);
    if ( 0 == sourceSize.iWidth || 0 == sourceSize.iHeight )
        {
        User::Leave(KErrCorrupt);
        }

    iCroppingRect.Intersection(TRect(sourceSize));
    if ( iCroppingRect.IsEmpty() )
        {
        User::Leave(KErrArgument);
        }

    // Find max scaling factor which won't make image smaller than target size
    TInt shift = 3;
    while ( shift
            && (sourceSize.iWidth >> shift) < iRequestedSize.iWidth
            && (sourceSize.iHeight >> shift) < iRequestedSize.iHeight )
        {
        shift--;
        }
    TInt fudge = (1 << shift) - 1;  // Used to "round up" the scaled values
    TSize loadSize((iCroppingRect.Width() + fudge) >> shift,
                    (iCroppingRect.Height() + fudge) >> shift);

    // Resize empty bitmap to required size
    User::LeaveIfError(iThumbnail->Resize(loadSize));

    iDecoder->SetCroppingL(iCroppingRect);
    iDecoder->Convert(&aStatus, *iThumbnail);
    }

// -----------------------------------------------------------------------------
// CreateDecoderL
// -----------------------------------------------------------------------------
//
void CGlxtnZoomedImageTask::CreateDecoderL(TInt aDecoderType)
    {
    TRACER("void CGlxtnZoomedImageTask::CreateDecoderL(TInt aDecoderType)");
    delete iDecoder;
    iDecoder = NULL;
    iDecoder = CExtJpegDecoder::FileNewL(
                    static_cast<CExtJpegDecoder::TDecoderType>(aDecoderType),
                    iFileUtility.FsSession(), iInfo->FilePath());

    if ( !(CExtJpegDecoder::ECapCropping & iDecoder->CapabilitiesL()) )
        {
        User::Leave(KErrNotSupported);
        }
    }

//  End of File  

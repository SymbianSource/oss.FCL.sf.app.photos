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
* Description:   Base class for tasks which load thumbnails
*
*/



#include "glxtnloadthumbnailtask.h"

#include <glxassert.h>
#include <glxtracer.h>
#include <glxthumbnail.h>
#include <imageconversion.h>
#include <s32mem.h>

#include "glxtnfileinfo.h"
#include "glxtnfileutility.h"
#include "glxtnimagedecoderfactory.h"
#include "glxtnthumbnailrequest.h"
#include "mglxtnstorage.h"



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CGlxtnLoadThumbnailTask::CGlxtnLoadThumbnailTask( const TGlxtnTaskId& aId,
                                    const TGlxThumbnailRequest& aRequestInfo,
                                    CGlxtnFileUtility& aFileUtility,
                                    MGlxtnThumbnailCreatorClient& aClient) :
    CGlxtnClientTask( aId, aRequestInfo.iId, aClient ),
    iFileUtility( aFileUtility ), iRequestedSize( aRequestInfo.iSizeClass ),
    iDrmAllowed( aRequestInfo.iDrmAllowed )
    {
    TRACER("CGlxtnLoadThumbnailTask::CGlxtnLoadThumbnailTask()");
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxtnLoadThumbnailTask::~CGlxtnLoadThumbnailTask() 
    {
    TRACER("CGlxtnLoadThumbnailTask::~CGlxtnLoadThumbnailTask()");
    delete iDecoder;
    delete iThumbnail;
    delete iThumbData;
    delete iInfo;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxtnLoadThumbnailTask::ConstructL( TInt aBitmapHandle )
    {
    TRACER("void CGlxtnLoadThumbnailTask::ConstructL()");
    // Duplicate the client's bitmap
    iThumbnail = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iThumbnail->Duplicate( aBitmapHandle ) );
    }

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxtnLoadThumbnailTask::DoCancel()
    {
    TRACER("void CGlxtnLoadThumbnailTask::DoCancel()");
    if ( iDecoder )
        {
        iDecoder->Cancel();
        }

    Storage()->StorageCancel();
    }

// ---------------------------------------------------------------------------
// LoadThumbnailL
// ---------------------------------------------------------------------------
//
TBool CGlxtnLoadThumbnailTask::LoadThumbnailL( TRequestStatus& aStatus )
    {
    TRACER("TBool CGlxtnLoadThumbnailTask::LoadThumbnailL()");
    GLX_ASSERT_DEBUG( iInfo, Panic( EGlxPanicNullPointer ), "No file info" );

    if ( Storage() && iFileUtility.IsPersistentSize( iRequestedSize ) )
        {
        Storage()->LoadThumbnailDataL( iThumbData, iFormat, ItemId(),
                                    *iInfo, iRequestedSize, &aStatus );
        iState = EStateLoading;

        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// HandleLoadedThumbnailL
// ---------------------------------------------------------------------------
//
TBool CGlxtnLoadThumbnailTask::HandleLoadedThumbnailL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnLoadThumbnailTask::HandleLoadedThumbnailL()");
	TBool active = EFalse;

    User::LeaveIfNull( iThumbData );

    switch ( iFormat )
        {
        case EGlxIDF_Bitmap:
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL( bitmap );

            RDesReadStream stream( *iThumbData );
            CleanupClosePushL( stream );
            stream >> *bitmap;
            CleanupStack::PopAndDestroy( &stream );

            User::LeaveIfError( iThumbnail->Resize( bitmap->SizeInPixels() ) );

            CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( iThumbnail );
            CleanupStack::PushL( device );
            CFbsBitGc* context = NULL;
            User::LeaveIfError( device->CreateContext( context ) );
            CleanupStack::PushL( context );

            context->BitBlt( TPoint(), bitmap );

            CleanupStack::PopAndDestroy( context ); 
            CleanupStack::PopAndDestroy( device );

            CleanupStack::PopAndDestroy( bitmap );
            }
            break;

    	case EGlxIDF_JPEG:
            iDecoder = GlxtnImageDecoderFactory::NewL(
                                    iFileUtility.FsSession(), *iThumbData );
            DecodeThumbnailL( aStatus, EFalse );
            active = ETrue;
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return active;
    }

// ---------------------------------------------------------------------------
// DecodeThumbnailL
// ---------------------------------------------------------------------------
//
void CGlxtnLoadThumbnailTask::DecodeThumbnailL( TRequestStatus& aStatus,
                                                TBool aScaleBitmap )
    {
    TRACER("CGlxtnLoadThumbnailTask::DecodeThumbnailL()");
    GLX_ASSERT_DEBUG( iDecoder, Panic( EGlxPanicNullPointer ), "No decoder" );

    // Resize empty bitmap to required size
    TSize thumbSize( iDecoder->FrameInfo().iOverallSizeInPixels );
    if ( aScaleBitmap )
        {
        // Find scaling factor which makes the image closest to requested size
        // Use biggest size less than 150% of the requested size
        TInt maxWidth = iRequestedSize.iWidth * 3 / 2;
        TInt maxHeight = iRequestedSize.iHeight * 3 / 2;
        TInt shift = 0;
        while ( shift < 3
                && ((thumbSize.iWidth >> shift) > maxWidth
                    || (thumbSize.iHeight >> shift) > maxHeight) )
            {
            shift++;
            }
        TInt fudge = (1 << shift) - 1;  // Used to "round up" the scaled values
        thumbSize.SetSize((thumbSize.iWidth + fudge) >> shift,
                        (thumbSize.iHeight + fudge) >> shift);
        }
    User::LeaveIfError( iThumbnail->Resize(thumbSize) );

    iDecoder->Convert( &aStatus, *iThumbnail );
    iState = EStateDecodingThumbnail;
    }
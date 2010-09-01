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
* Description:    Base class for tasks which load thumbnails
*
*/




#include "glxtnloadthumbnailtask.h"

#include <glxassert.h>
#include <glxtracer.h>
#include <glxlog.h>
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
                                                TBool /*aScaleBitmap*/ )
    {
    TRACER("CGlxtnLoadThumbnailTask::DecodeThumbnailL()");
    GLX_ASSERT_DEBUG( iDecoder, Panic( EGlxPanicNullPointer ), "No decoder" );

    // Resize empty bitmap to required size
    TSize thumbSize( iDecoder->FrameInfo().iOverallSizeInPixels );
    GLX_DEBUG3("CGlxtnLoadThumbnailTask::DecodeThumbnailL() thumbSize w(%d) h(%d)", 
            thumbSize.iWidth, thumbSize.iHeight);
   
    // Always adjust the target bitmap size 
	// as Exif thumbnail could be bigger in size
    TSize targetSize = iRequestedSize;
    if( (thumbSize.iWidth <= targetSize.iWidth ) && 
	    (thumbSize.iHeight <= targetSize.iHeight ) )
         {
         targetSize.iWidth  = thumbSize.iWidth;
         targetSize.iHeight = thumbSize.iHeight;
         }
    else if ( targetSize.iHeight * thumbSize.iWidth
             < targetSize.iWidth * thumbSize.iHeight )
         {
         // Source has taller aspect than target so reduce target width
         targetSize.iWidth =  ( ( targetSize.iHeight * thumbSize.iWidth )
                                 / (  thumbSize.iHeight ) );
         }
    else
         {
         // Source has wider aspect than target so reduce target height
         targetSize.iHeight = (targetSize.iWidth * thumbSize.iHeight)
                                 / thumbSize.iWidth;
         }
    GLX_DEBUG3("CGlxtnLoadThumbnailTask::DecodeThumbnailL() targetSize w(%d) h(%d)", 
            targetSize.iWidth, targetSize.iHeight);
     
    User::LeaveIfError( iThumbnail->Resize(targetSize) );

    iDecoder->Convert( &aStatus, *iThumbnail );
    iState = EStateDecodingThumbnail;
    }

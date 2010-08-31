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

#include "glxtngeneratethumbnailtask.h"

#include <bitdev.h>
#include <fbs.h>
#include <glxassert.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <glxthumbnail.h>

#include "glxtnfileinfo.h"
#include "glxtnfileutility.h"
#include "glxtnimageutility.h"
#include "glxtnsavethumbnailtask.h"
#include "glxtntaskmanager.h"
#include "glxtnthumbnailrequest.h"
#include "glxtnvideoutility.h"
#include "mglxtnthumbnailcreatorclient.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxtnGenerateThumbnailTask* CGlxtnGenerateThumbnailTask::NewL(
        const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient)
    {
    TRACER("CGlxtnGenerateThumbnailTask::NewL()");
    CGlxtnGenerateThumbnailTask* task
                = new (ELeave) CGlxtnGenerateThumbnailTask(
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
CGlxtnGenerateThumbnailTask::CGlxtnGenerateThumbnailTask(
        const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient) :
    CGlxtnLoadThumbnailTask( KGlxtnTaskIdGenerateThumbnail,
                            aRequestInfo, aFileUtility, aClient )
	{
    TRACER("CGlxtnGenerateThumbnailTask::CGlxtnGenerateThumbnailTask()");
	}

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxtnGenerateThumbnailTask::ConstructL(TInt aBitmapHandle)
	{
    TRACER("CGlxtnGenerateThumbnailTask::ConstructL()");
	CGlxtnLoadThumbnailTask::ConstructL( aBitmapHandle );

    iImageUtility = new (ELeave) CGlxtnImageUtility(iFileUtility.FsSession());
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnGenerateThumbnailTask::~CGlxtnGenerateThumbnailTask() 
	{
    TRACER("CGlxtnGenerateThumbnailTask::~CGlxtnGenerateThumbnailTask()");
	delete iImageUtility;
    delete iVideoUtility;
    delete iImage;
	}

// -----------------------------------------------------------------------------
// SetManager
// -----------------------------------------------------------------------------
//
void CGlxtnGenerateThumbnailTask::SetManager(CGlxtnTaskManager* aManager)
    {
    TRACER("void CGlxtnGenerateThumbnailTask::SetManager()");
    iTaskManager = aManager;
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnGenerateThumbnailTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnGenerateThumbnailTask::DoStartL()");
    iInfo = new (ELeave) CGlxtnFileInfo;
    Client().FetchFileInfoL(iInfo, ItemId(), &aStatus);
    iState = EStateFetchingUri;

    return ETrue; // Request has been issued
    } 

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnGenerateThumbnailTask::DoCancel() 
	{
    TRACER("void CGlxtnGenerateThumbnailTask::DoCancel()");
	CGlxtnLoadThumbnailTask::DoCancel();

	iImageUtility->Cancel();

	if ( iVideoUtility ) 
		{
		iVideoUtility->Cancel();
		}

    iFileUtility.ClearBadFileMarker();
	}

// -----------------------------------------------------------------------------
// DoRunL
// -----------------------------------------------------------------------------
//
TBool CGlxtnGenerateThumbnailTask::DoRunL(TRequestStatus& aStatus) 
	{
    TRACER("TBool CGlxtnGenerateThumbnailTask::DoRunL()");
	TInt error = aStatus.Int();

	TBool active = EFalse;

    if ( KErrNone == error )
        {
        switch ( iState )
            {
            case EStateFetchingUri:
                {
                User::LeaveIfNull(iInfo);
                iBadFileMarkerNotNeededFlag = ETrue;

                iInfo->IdentifyFileL(iVideo, iProtected);
                // leave if file is DRM protected and client has no DRM 
                // capablity
                if ( iProtected && !iDrmAllowed )
                    {
                    User::Leave(KErrAccessDenied);
                    }

                if ( !LoadThumbnailL( aStatus ) )
                    {
                    iBadFileMarkerNotNeededFlag = EFalse;
                    iFileUtility.CheckBadFileListL( iInfo->FilePath() );
                    HighQualityDecodeL( aStatus );
                    }
                active = ETrue;
                }
                break;

            case EStateLoading:
                active = HandleLoadedThumbnailL( aStatus );
                break;

            case EStateDecodingThumbnail:
                // Nothing to do
                break;

            case EStateDecodingImage:
                {
                iImageUtility->FreeDecoder();

                // Resize empty bitmap to required size
                TSize targetSize(iRequestedSize);
                iImageUtility->AdjustSize(targetSize, iImage->SizeInPixels());
                User::LeaveIfError(iThumbnail->Resize(targetSize));

                iImageUtility->FilterImageL(&aStatus, iImage, iImage, iThumbnail);
                iState = EStateFiltering;
                active = ETrue;
                }
                break;

            case EStateFiltering:
                {
                TSize targetSize(iRequestedSize);
                const TSize srcSize = iImage->SizeInPixels();
                iImageUtility->AdjustSize(targetSize, iImage->SizeInPixels());
                iImageUtility->ScaleImageL(aStatus, *iImage, srcSize, *iThumbnail, targetSize);
                iState = EStateScaling;
                active = ETrue;
                }
                break;

            case EStateScaling:
            	{
            	User::LeaveIfError(iThumbnail->SetDisplayMode(KGlxThumbnailDisplayMode));
            	           	
                // Create task to save generated thumbnail
                if ( Storage() && !iInfo->iTemporary
                    && !SizeRequiresDrmRights()
                    && iFileUtility.IsPersistentSize(iRequestedSize) )
                    {
                    CGlxtnSaveThumbnailTask* task
                        = CGlxtnSaveThumbnailTask::NewL(ItemId(), iInfo,
                                        iRequestedSize, iThumbnail, Storage());
                    iThumbnail = NULL;  // Task has ownership
                    iInfo = NULL;       // Task has ownership
                    iTaskManager->AddTaskL(task);
                    }
                break;
            	}

            default:
                GLX_ASSERT_ALWAYS( EFalse, Panic( EGlxPanicIllegalState ),
                            "CGlxtnGenerateThumbnailTask: Illegal state" );
                break;
            }
        }
    else
        {
        if ( EStateLoading == iState )
            {
            // Loading failed - generate new thumbnail instead
            HighQualityDecodeL( aStatus );
            active = ETrue;
            }
        }

    if ( !active )
        {
        if ( !iBadFileMarkerNotNeededFlag )
            {
            iFileUtility.ClearBadFileMarker();
            }
        Client().ThumbnailFetchComplete(ItemId(),
                                        EGlxThumbnailQualityHigh, error);
        }

    return active; 
	}

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnGenerateThumbnailTask::DoRunError(TInt aError)
    {
    TRACER("TBool CGlxtnGenerateThumbnailTask::DoRunError(TInt aError)");
    iFileUtility.ClearBadFileMarker();
    Client().ThumbnailFetchComplete(ItemId(), EGlxThumbnailQualityHigh, aError);

    return EFalse;
    }

// ---------------------------------------------------------------------------
// HighQualityDecodeL
// ---------------------------------------------------------------------------
//
void CGlxtnGenerateThumbnailTask::HighQualityDecodeL(
                                                TRequestStatus& aStatus )
    {
    TRACER("void CGlxtnGenerateThumbnailTask::HighQualityDecodeL()");
    RArray<TSize> targetSizes;
    CleanupClosePushL(targetSizes);
    targetSizes.AppendL( TSize(iRequestedSize) );
    if ( iVideo )
        {
        iVideoUtility = new (ELeave) CGlxtnVideoUtility;
        iVideoUtility->GetVideoFrameL( &aStatus, iImage, iInfo->FilePath(), targetSizes, KGlxThumbnailDisplayMode );
        }
    else
        {
        iImage = iImageUtility->DecodeImageL( aStatus, iInfo->FilePath(),
                                    targetSizes, KGlxThumbnailDisplayMode );
        }
    CleanupStack::PopAndDestroy(&targetSizes);
    iState = EStateDecodingImage;
    }

// -----------------------------------------------------------------------------
// SizeRequiresDrmRights
// -----------------------------------------------------------------------------
//
TBool CGlxtnGenerateThumbnailTask::SizeRequiresDrmRights()
    {
    TRACER("TBool CGlxtnGenerateThumbnailTask::SizeRequiresDrmRights()");
    if ( iProtected )
        {
        if ( iVideo )
            {
            return ETrue;
            }

        // Rights are required if thumbnail doesn't fit in KGlxThumbnailDrmWidth
        // by KGlxThumbnailDrmHeight box either portrait or landscape
        if ( (iRequestedSize.iWidth > KGlxThumbnailDrmWidth
                || iRequestedSize.iHeight > KGlxThumbnailDrmHeight)
            && (iRequestedSize.iWidth > KGlxThumbnailDrmHeight
                || iRequestedSize.iHeight > KGlxThumbnailDrmWidth) )
            {
            return ETrue;
            }

        const TSize& imageSize(iImageUtility->OriginalSize());

        // Rights are required if thumbnail is greater than 25% of image size
        // Due to preserving aspect ratio, this will only happen if both width
        // and height are greater than half the image width and height
        if ( iRequestedSize.iWidth > imageSize.iWidth / 2
            && iRequestedSize.iHeight> imageSize.iHeight / 2 )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

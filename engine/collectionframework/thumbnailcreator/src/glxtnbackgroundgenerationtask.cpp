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
* Description:   Thumbnail background generation task implementation
*
*/



/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

// INCLUDE FILES

#include "glxtnbackgroundgenerationtask.h"

#include <fbs.h>
#include <glxassert.h>
#include <glxtracer.h>
#include <glxthumbnail.h>

#include "glxtnfileinfo.h"
#include "glxscreenresolutions.h"
#include "glxtnfileutility.h"
#include "glxtnimageutility.h"
#include "glxtnvideoutility.h"
#include "mglxtnstorage.h"
#include "mglxtnthumbnailcreatorclient.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlxtnBackgroundGenerationTask::CGlxtnBackgroundGenerationTask
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGlxtnBackgroundGenerationTask::CGlxtnBackgroundGenerationTask(
                    const TGlxMediaId& iItemId, CGlxtnFileUtility& aFileUtility,
                    MGlxtnThumbnailCreatorClient& aClient, 
                    CGlxSettingsModel::TSupportedOrientations aSupportedOrientations) :
    CGlxtnSaveThumbnailTask(KGlxtnTaskIdBackgroundThumbnailGeneration,
                            iItemId, aClient.ThumbnailStorage()),
    iClient(&aClient), iFileUtility(aFileUtility),
    iSupportedOrientations(aSupportedOrientations)
    {
    TRACER("CGlxtnBackgroundGenerationTask::CGlxtnBackgroundGenerationTask()");
    // Background task is always low priority
    SetPriority(EPriorityLow);
    }

// -----------------------------------------------------------------------------
// CGlxtnBackgroundGenerationTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxtnBackgroundGenerationTask::ConstructL()
    {
    TRACER("void CGlxtnBackgroundGenerationTask::ConstructL()");
    iImageUtility = new (ELeave) CGlxtnImageUtility(iFileUtility.FsSession());
    }

// -----------------------------------------------------------------------------
// CGlxtnBackgroundGenerationTask::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlxtnBackgroundGenerationTask* CGlxtnBackgroundGenerationTask::NewL(
                    const TGlxMediaId& iItemId, CGlxtnFileUtility& aFileUtility,
                    MGlxtnThumbnailCreatorClient& aClient, 
                    CGlxSettingsModel::TSupportedOrientations aSupportedOrientations)
    {
    TRACER("CGlxtnBackgroundGenerationTask* CGlxtnBackgroundGenerationTask::NewL()");
    CGlxtnBackgroundGenerationTask* self
            = new (ELeave) CGlxtnBackgroundGenerationTask(
                                                iItemId, aFileUtility, aClient,
                                                aSupportedOrientations);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CGlxtnBackgroundGenerationTask::~CGlxtnBackgroundGenerationTask()
    {
    TRACER("Destructor CGlxtnBackgroundGenerationTask::~CGlxtnBackgroundGenerationTask()");
    delete iImageUtility;
    delete iVideoUtility;
    delete iImage;
    delete iFilteredImage;
    iSizes.Close();
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnBackgroundGenerationTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnBackgroundGenerationTask::DoStartL()");
    // Sizes to generate, largest first
    iSizes.AppendL(TSize(KGlxThumbnailLargeWidth, KGlxThumbnailLargeHeight));
    iSizes.AppendL(TSize(KGlxThumbnailSmallWidth, KGlxThumbnailSmallHeight));
    if( CGlxSettingsModel::ELandscapeAndPortrait == iSupportedOrientations )
        {
        iSizes.AppendL(TSize(KGlxThumbnailPortraitWidth, KGlxThumbnailPortraitHeight));
        }

    iFileInfo = new (ELeave) CGlxtnFileInfo;
    iClient->FetchFileInfoL(iFileInfo, ItemId(), &aStatus);
    iState = EStateFetchingUri;

    return ETrue; // Request has been issued
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnBackgroundGenerationTask::DoCancel()
    {
    TRACER("void CGlxtnBackgroundGenerationTask::DoCancel()");
    CGlxtnSaveThumbnailTask::DoCancel();
    
    Storage()->NotifyBackgroundError(ItemId(), KErrCancel);

    if ( iClient && EStateFetchingUri == iState )
        {
        iClient->CancelFetchUri(ItemId());
        }

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
TBool CGlxtnBackgroundGenerationTask::DoRunL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnBackgroundGenerationTask::DoRunL()");
    User::LeaveIfError(aStatus.Int());

    TBool active = EFalse;

    switch ( iState )
        {
        case EStateFetchingUri:
            {
            iImageDecoded = EFalse;
            iBadFileMarkerNotNeededFlag = ETrue;
            // Client is no longer needed
            iClient->ThumbnailFetchComplete(ItemId(),
                                        EGlxThumbnailQualityHigh, KErrNone);
            iClient = NULL;

            User::LeaveIfNull(iFileInfo);
 
            TBool isVideo, isProtected;
            iFileInfo->IdentifyFileL(isVideo, isProtected);
            // if DRM protected leave
            // assume DRM capablity not present
            if(isProtected)
                {
                User::Leave(KErrAccessDenied);
                }
            
            active = CheckNextL(aStatus);
            }
            break;

        case EStateDecoding:
            iImageUtility->FreeDecoder();
            iImageDecoded = ETrue;
            FilterThumbnailL(aStatus);
            active = ETrue;
            break;

        case EStateFiltering:
            ScaleThumbnailL(aStatus);
            active = ETrue;
            break;

        case EStateSaving:
            active = ProcessNextL(aStatus);
            break;

        case EStateChecking:
            if ( KGlxThumbnailNotAvailable == aStatus.Int() )
                {
                iTodoSizes.AppendL(iSize);
                }
            active = CheckNextL(aStatus);
            break;

        case EStateScaling:
            EncodeThumbnailL(aStatus);
            active = ETrue;
            break;

        case EStateEncoding:
            SaveThumbnailL(aStatus);
            active = ETrue;
            break;

        default:
            GLX_ASSERT_ALWAYS( EFalse, Panic( EGlxPanicIllegalState ),
                            "CGlxtnBackgroundGenerationTask: Illegal state" );
            break;
        }

    if ( !active )
        {
        if( !iBadFileMarkerNotNeededFlag )
            {
            iFileUtility.ClearBadFileMarker();
            }
        // Send notification that we've finished
        Storage()->NotifyBackgroundError( ItemId(), KErrNone );
        }

    return active;  // Task is complete
    }

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnBackgroundGenerationTask::DoRunError(TInt aError)
    {
    TRACER("CGlxtnBackgroundGenerationTask::DoRunError()");
    if( !iBadFileMarkerNotNeededFlag )
        {
        iFileUtility.ClearBadFileMarker();
        }
    if ( iClient )
        {
        iClient->ThumbnailFetchComplete(
                                ItemId(), EGlxThumbnailQualityHigh, aError);
        }
    else
        {
        Storage()->NotifyBackgroundError(ItemId(), aError);
        }

    return EFalse;  // Task is complete
    }

// -----------------------------------------------------------------------------
// CheckNextL
// -----------------------------------------------------------------------------
//
TBool CGlxtnBackgroundGenerationTask::CheckNextL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnBackgroundGenerationTask::CheckNextL()");
    if ( iSizes.Count() == 0 )
        {
        return ProcessNextL(aStatus);
        }

    iSize = iSizes[0];
    iSizes.Remove(0);

    Storage()->IsThumbnailAvailableL(ItemId(), *iFileInfo, iSize, &aStatus);
    iState = EStateChecking;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// ProcessNextL
// -----------------------------------------------------------------------------
//
TBool CGlxtnBackgroundGenerationTask::ProcessNextL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnBackgroundGenerationTask::ProcessNextL()");
    if ( iTodoSizes.Count() == 0 )
        {
        return EFalse;
        }

    iSize = iTodoSizes[0];
    if( iImageDecoded )
        {
        FilterThumbnailL(aStatus);
        }
    else
        {
        DecodeImageL(aStatus);
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// FilterThumbnailL
// -----------------------------------------------------------------------------
//
void CGlxtnBackgroundGenerationTask::FilterThumbnailL(TRequestStatus& aStatus)
    {
    TRACER("void CGlxtnBackgroundGenerationTask::FilterThumbnailL()");
    // Create thumbnail bitmap of required size
    TSize targetSize(iTodoSizes[0]);
    iTodoSizes.Remove(0);
    iImageUtility->AdjustSize(targetSize, iImage->SizeInPixels());

    delete iThumbnail;
    iThumbnail = NULL;
    iThumbnail = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iThumbnail->Create(targetSize, KGlxThumbnailDisplayMode));

    iImageUtility->FilterImageL(&aStatus, iImage, iFilteredImage, iThumbnail);
    iState = EStateFiltering;
    }

// -----------------------------------------------------------------------------
// ScaleThumbnailL
// -----------------------------------------------------------------------------
//
void CGlxtnBackgroundGenerationTask::ScaleThumbnailL(TRequestStatus& aStatus)
    {
    TRACER("void CGlxtnBackgroundGenerationTask::ScaleThumbnailL()");
    TSize targetSize(iSize);
    const TSize srcSize = iImage->SizeInPixels();
    iImageUtility->AdjustSize(targetSize, iImage->SizeInPixels());
    iImageUtility->ScaleImageL(aStatus, *iImage, srcSize, *iThumbnail, targetSize);
    iState = EStateScaling;
    }

// -----------------------------------------------------------------------------
// DecodeImageL
// -----------------------------------------------------------------------------
//
void CGlxtnBackgroundGenerationTask::DecodeImageL(TRequestStatus& aStatus)
    {
    TRACER("void CGlxtnBackgroundGenerationTask::DecodeImageL()");
    iBadFileMarkerNotNeededFlag = EFalse;
    iFileUtility.CheckBadFileListL( iFileInfo->FilePath() );
    TBool isVideo, isProtected;
    iFileInfo->IdentifyFileL(isVideo, isProtected);
    if ( isVideo )
        {
        iVideoUtility = new (ELeave) CGlxtnVideoUtility;
        iVideoUtility->GetVideoFrameL(&aStatus, iImage,
                                        iFileInfo->FilePath(), iTodoSizes, KGlxThumbnailDisplayMode);
        }
    else
        {
        iImage = iImageUtility->DecodeImageL(aStatus,
                    iFileInfo->FilePath(), iTodoSizes, KGlxThumbnailDisplayMode);
        }
    delete iFilteredImage;
    iFilteredImage = NULL;
    iState = EStateDecoding;
    }

//  End of File  

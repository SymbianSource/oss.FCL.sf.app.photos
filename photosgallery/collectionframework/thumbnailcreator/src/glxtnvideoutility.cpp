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
* Description:    Utility for creating bitmaps from video files
*
*/

/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnvideoutility.h"

#include <fbs.h>
#include <glxtracer.h>
#include <glxpanic.h>

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnVideoUtility::CGlxtnVideoUtility() : iDestination(NULL)
    {
    TRACER("CGlxtnVideoUtility::CGlxtnVideoUtility()");
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnVideoUtility::~CGlxtnVideoUtility()
    {
    TRACER("CGlxtnVideoUtility::~CGlxtnVideoUtility()");
    delete iVideoInfo;
    }

// -----------------------------------------------------------------------------
// GetVideoFrameL
// -----------------------------------------------------------------------------
//
void CGlxtnVideoUtility::GetVideoFrameL(TRequestStatus* aRequestStatus,
                            CFbsBitmap*& aDestination, const TDesC& aFileName,
                                RArray<TSize>& aTargetSizes, TDisplayMode aMode)
    {
    TRACER("void CGlxtnVideoUtility::GetVideoFrameL(TRequestStatus* aRequestStatus, CFbsBitmap*& aDestination, const TDesC& aFileName, RArray<TSize>& aTargetSizes, TDisplayMode aMode)");
    if ( iRequestStatus )
        {
        User::Leave(KErrNotReady);
        }
        
    iDisplayMode = aMode;
    iSize = aTargetSizes[0];
            
    iRequestStatus = aRequestStatus;
    *iRequestStatus = KRequestPending;

    iDestination = &aDestination;
    delete *iDestination;
    *iDestination = NULL;

    delete iVideoInfo;
    iVideoInfo = NULL;
#ifdef ENABLE_VED
    iVideoInfo = CVedVideoClipInfo::NewL(aFileName, *this);
#else
    iVideoInfo = CTNEVideoClipInfo::NewL(aFileName, *this);
#endif
    }


// -----------------------------------------------------------------------------
// Cancel
// -----------------------------------------------------------------------------
//
void CGlxtnVideoUtility::Cancel()
    {
    TRACER("void CGlxtnVideoUtility::Cancel()");
    if ( iVideoInfo )
        {
#ifdef ENABLE_VED
        iVideoInfo->CancelFrame();
#else
        iVideoInfo->CancelThumb();
#endif
        delete iVideoInfo;
        iVideoInfo = NULL;

	    if (iRequestStatus)
	    	{
	    	User::RequestComplete(iRequestStatus, KErrCancel);
	    	}
        }
    }

// -----------------------------------------------------------------------------
// NotifyVideoClipInfoReady
// -----------------------------------------------------------------------------
//
#ifdef ENABLE_VED
void CGlxtnVideoUtility::NotifyVideoClipInfoReady( CVedVideoClipInfo& aInfo,
                                                        TInt aError )
#else
void CGlxtnVideoUtility::NotifyVideoClipInfoReady( CTNEVideoClipInfo& aInfo,
                                                        TInt aError )
#endif
    {
    TRACER("void CGlxtnVideoUtility::NotifyVideoClipInfoReady( CVedVideoClipInfo& aInfo, TInt aError )");
    __ASSERT_DEBUG(&aInfo == iVideoInfo, Panic(EGlxPanicIllegalArgument));
    

    
    if ( KErrNone == aError )
        {
#ifdef ENABLE_VED
        TRAP(aError, iVideoInfo->GetFrameL(*this, KFrameIndexBestThumb,
                                            NULL /*aResolution*/, iDisplayMode));
#else
        TRAP(aError, iVideoInfo->GetThumbL(*this, KBestThumbIndex,
                                            NULL/*&iSize*/, iDisplayMode));
#endif
        }

    if ( KErrNone != aError )
        {
        User::RequestComplete(iRequestStatus, aError);
        }
    }

// -----------------------------------------------------------------------------
// NotifyVideoClipFrameCompleted
// -----------------------------------------------------------------------------
//
#ifdef ENABLE_VED
void CGlxtnVideoUtility::NotifyVideoClipFrameCompleted(
                    CVedVideoClipInfo& aInfo, TInt aError, CFbsBitmap* aFrame )
#else
void CGlxtnVideoUtility::NotifyVideoClipThumbCompleted(
                    CTNEVideoClipInfo& aInfo, TInt aError, CFbsBitmap* aFrame )
#endif
    {
    TRACER("void CGlxtnVideoUtility::NotifyVideoClipFrameCompleted( CVedVideoClipInfo& aInfo, TInt aError, CFbsBitmap* aFrame )");
    __ASSERT_DEBUG(&aInfo == iVideoInfo, Panic(EGlxPanicIllegalArgument));
    *iDestination = aFrame;

    User::RequestComplete(iRequestStatus, aError);
    }

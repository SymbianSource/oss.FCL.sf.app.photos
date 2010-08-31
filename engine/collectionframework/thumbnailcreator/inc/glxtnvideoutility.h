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
* Description:   Utility for creating bitmaps from video files
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef GLXTNVIDEOUTILITY_H
#define GLXTNVIDEOUTILITY_H

#include <e32base.h>
#ifdef ENABLE_VED
#include <VedVideoClipInfo.h>
#else
#include <TNEVideoClipInfo.h>
#endif

class CFbsBitmap;

/**
 * Utility for creating bitmaps from video files
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS( CGlxtnVideoUtility ) : public CBase,
#ifdef ENABLE_VED
    public MVedVideoClipInfoObserver, public MVedVideoClipFrameObserver
#else
    public MTNEVideoClipInfoObserver, public MTNEVideoClipThumbObserver
#endif
    {
public:
    /**
     * Default constructor.
     */
    CGlxtnVideoUtility();
    /**
     * Destructor.
     */
    ~CGlxtnVideoUtility();

    /**
     * Start asynchronous frame decoding.
     * @param aRequestStatus Request status for the asynchronous operation.
     * @param aDestination Bitmap in which to store the frame.
     * @param aFileName Full name of video file.
     */
    void GetVideoFrameL( TRequestStatus* aRequestStatus,
                        CFbsBitmap*& aDestination, const TDesC& aFileName,
                                RArray<TSize>& aTargetSizes, TDisplayMode aMode );
    /**
     * Cancel asynchronous frame decoding.
     */
    void Cancel();

#ifdef ENABLE_VED
private:    // From MVedVideoClipInfoObserver
    void NotifyVideoClipInfoReady( CVedVideoClipInfo& aInfo, TInt aError );

private:    // From MVedVideoClipFrameObserver
    void NotifyVideoClipFrameCompleted( CVedVideoClipInfo& aInfo,
                                        TInt aError, CFbsBitmap* aFrame );
#else
private:    // From MTNEVideoClipInfoObserver
    void NotifyVideoClipInfoReady( CTNEVideoClipInfo& aInfo, TInt aError );

private:    // From MTNEVideoClipThumbObserver
    void NotifyVideoClipThumbCompleted( CTNEVideoClipInfo& aInfo, 
                                        TInt aError, CFbsBitmap* aFrame );
#endif

private:
#ifdef ENABLE_VED
    CVedVideoClipInfo* iVideoInfo;
#else
    CTNEVideoClipInfo* iVideoInfo;
#endif
    /** Client's request status */
    TRequestStatus* iRequestStatus;
    /** Pointer to variable to hold returned frame */
    CFbsBitmap** iDestination;
    TSize iSize;
    TDisplayMode iDisplayMode;
    };

#endif  // GLXTNVIDEOUTILITY_H

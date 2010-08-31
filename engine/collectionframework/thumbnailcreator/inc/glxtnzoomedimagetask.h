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
* Description:   Task to generate a cropped thumbnail for a JPEG image.
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */


#ifndef C_GLXTNZOOMEDIMAGETASK_H
#define C_GLXTNZOOMEDIMAGETASK_H

#include "glxtntask.h"

#include <glxthumbnailinfo.h>   // For TGlxThumbnailFilter

class CGlxtnFileInfo;
class CGlxtnFileUtility;
class TGlxThumbnailRequest;
class CFbsBitmap;
class CExtJpegDecoder;

/**
 * Task to generate a cropped thumbnail for a JPEG image.
 *
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnZoomedImageTask) : public CGlxtnClientTask
    {
public:
    /**
    * Two-phased constructor.
    */
	static CGlxtnZoomedImageTask* NewL(
	    const TGlxThumbnailRequest& aRequestInfo,
	    CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient);
	~CGlxtnZoomedImageTask();

protected:  // From CGlxtnTask
    virtual TBool DoStartL(TRequestStatus& aStatus);
	virtual void DoCancel();
	virtual TBool DoRunL(TRequestStatus& aStatus);
    TBool DoRunError(TInt aError);

private:
    CGlxtnZoomedImageTask(const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient);
	void ConstructL(TInt aBitmapHandle);

    void DecodePartialImageL(TRequestStatus& aStatus);
    void CreateDecoderL(TInt aDecoderType);

private:
    enum TTaskState
        {
        EStateFetchingUri, EStateDecoding, EStateProcessing
        };

    /** File utility (not owned) */
    CGlxtnFileUtility& iFileUtility;
    /** File information for item to thumbnail */
    CGlxtnFileInfo* iInfo;
	/** Current state for task state machine */
    TTaskState iState;
    /** Bitmap for the generated thumbnail */
    CFbsBitmap* iThumbnail;
    /** Decoder for the JPEG file */
    CExtJpegDecoder* iDecoder;
    /** Requested thumbnail size */
    TSize iRequestedSize;
    /** Part of image to keep */
    TRect iCroppingRect;
    /** Filtering to apply to generated image */
    TGlxThumbnailFilter iFilter;
    /** DRM allowed flag passed from thumbnail request */
    TBool iDrmAllowed;
    /** Whether current file is DRM protected */
    TBool iProtected;
    };
        
#endif  // C_GLXTNZOOMEDIMAGETASK_H

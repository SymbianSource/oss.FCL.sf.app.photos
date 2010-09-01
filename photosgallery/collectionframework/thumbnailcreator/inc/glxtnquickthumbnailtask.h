/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Quick thumbnail generation task.
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef C_GLXTNQUICKTHUMBNAILTASK_H
#define C_GLXTNQUICKTHUMBNAILTASK_H

#include "glxtnloadthumbnailtask.h"
#include "glxthumbnailinfo.h"

// Forward declarations
class CGlxtnFileUtility;
class CGlxtnVideoUtility;
class CFbsBitmap;

/**
 * Task to quickly produce a thumbnail for an item, usually from the EXIF
 * thumbnail.
 *
 * The task first attempts to extract the EXIF thumbnail data from the file
 * using ExifLib.  If this succeeds, an instance of CImageDecoder is constructed
 * from the data and used to produce a bitmap.
 *
 * If this fails, an instance of CImageDecoder is constructed from the file,
 * and set to use the thumbnail.  If this fails, it is assumed that there is no
 * EXIF thumbnail in the file, and the decoder is set to scale the main image
 * as close as possible to the required thumbnail size.
 *
 * No scaling or image enhancement is applied to the decoded bitmap.
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS( CGlxtnQuickThumbnailTask ) : public CGlxtnLoadThumbnailTask
    {
public:
    /**
    * Two-phased constructor.
    * @param aRequestInfo Parameters for the thumbnail request.
    * @param aFileUtility File utility for use by the task.
    * @param aClient Client initiating the request.
    */
	static CGlxtnQuickThumbnailTask* NewL(
        const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient);
    /**
    * Destructor.
    */
    ~CGlxtnQuickThumbnailTask();

protected:  // From CGlxtnTask

    TBool DoStartL(TRequestStatus& aStatus);
	void DoCancel();
	TBool DoRunL(TRequestStatus& aStatus);
    TBool DoRunError(TInt aError);

private:
    /**
    * C++ default constructor.
    * @param aRequestInfo Parameters for the thumbnail request.
    * @param aFileUtility File utility for use by the task.
    * @param aClient Client initiating the request.
    */
    CGlxtnQuickThumbnailTask(const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient);

    /**
    * Start asynchronous decoding process for a low quality thumbnail.
    * @param aStatus Request status for the asynchronous operation.
    */
    void QuickDecodeL(TRequestStatus& aStatus);
    /**
    * Decode an image file using thumbnail embedded in the file if possible.
    * @param aStatus Request status for the asynchronous operation.
    */
    void ReadThumbnailL(TRequestStatus& aStatus);
    /**
    * Read thumbnail data from the file using ExifLib.
    */
    void ReadExifThumbnailL();
    void QuickScaleL();

private:
    /** Quality of thumbnail (high if loaded, low if generated) */
    TGlxThumbnailQuality iQuality;
    /** Utility to get thumbnail from video (owned) */
    CGlxtnVideoUtility* iUtility;
    /** Bitmap to hold video frame (owned) */
    CFbsBitmap* iVideoFrame;
    /** Whether the media file is a video */
    TBool iVideo;
    /** Flag to show BadFileMarker not created so no need to delete **/
    TBool iBadFileMarkerNotNeededFlag;
    };

#endif // C_GLXTNQUICKTHUMBNAILTASK_H

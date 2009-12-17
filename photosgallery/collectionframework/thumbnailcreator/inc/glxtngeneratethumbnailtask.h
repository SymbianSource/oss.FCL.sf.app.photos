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
* Description:    Classes for thumbnail-related tasks.
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef C_GLXTNGENERATETHUMBNAILTASK_H
#define C_GLXTNGENERATETHUMBNAILTASK_H

#include "glxtnloadthumbnailtask.h"

class CGlxtnFileInfo;
class CGlxtnFileUtility;
class CGlxtnImageUtility;
class CGlxtnVideoUtility;
class CFbsBitmap;
class CImageDecoder;
class RFs;

/**
 * Task to generate a high quality thumbnail for a media item.
 *
 * An instance of CImageDecoder is constructed to produce a bitmap from the
 * image file, scaling to the smallest possible size not smaller than the
 * required size.
 *
 * The task then uses ADIS to scale to the exact size required, and also uses
 * the sharpening and IETD algorithms to enhance the image.
 *
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS( CGlxtnGenerateThumbnailTask )
        : public CGlxtnLoadThumbnailTask
    {
public:
    /**
    * Two-phased constructor.
    * @param aRequestInfo Parameters for the thumbnail request.
    * @param aFileUtility File utility for use by the task.
    * @param aClient Client initiating the request.
    */
	static CGlxtnGenerateThumbnailTask* NewL(
	    const TGlxThumbnailRequest& aRequestInfo,
	    CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient);
    /**
    * Destructor.
    */
	~CGlxtnGenerateThumbnailTask();

protected:  // From CGlxtnTask
    void SetManager(CGlxtnTaskManager* aManager);
    virtual TBool DoStartL(TRequestStatus& aStatus);
	virtual void DoCancel();
	virtual TBool DoRunL(TRequestStatus& aStatus); 
    TBool DoRunError(TInt aError);

private:
    /**
    * C++ default constructor.
    * @param aRequestInfo Parameters for the thumbnail request.
    * @param aFileUtility File utility for use by the task.
    * @param aClient Client initiating the request.
    */
    CGlxtnGenerateThumbnailTask(const TGlxThumbnailRequest& aRequestInfo,
        CGlxtnFileUtility& aFileUtility, MGlxtnThumbnailCreatorClient& aClient);
    /**
    * Symbian 2nd phase constructor.
    * @param aBitmapHandle Handle to bitmap in which to store the thumbnail.
    */
	void ConstructL(TInt aBitmapHandle);

    /**
    * Start asynchronous decoding process for a high quality thumbnail.
    * @param aStatus Request status for the asynchronous operation.
    */
    void HighQualityDecodeL( TRequestStatus& aStatus );
    /**
    * @return ETrue if viewing the requested thumbnail size would require
    *       DRM rights.
    */
    TBool SizeRequiresDrmRights();

private:
    /** Pointer to task manager (not owned) */
    CGlxtnTaskManager* iTaskManager;
    /** Utility to get thumbnail from image (owned) */
    CGlxtnImageUtility* iImageUtility;
    /** Utility to get thumbnail from video (owned) */
    CGlxtnVideoUtility* iVideoUtility;
    /** Image loaded from file (owned) */
    CFbsBitmap* iImage;
    /** Whether current item is a video */
    TBool iVideo;
    /** Whether current file is DRM protected */
    TBool iProtected;
    /** Flag to show BadFileMarker not created so no need to delete **/
    TBool iBadFileMarkerNotNeededFlag;
    };
        
#endif // C_GLXTNGENERATETHUMBNAILTASK_H

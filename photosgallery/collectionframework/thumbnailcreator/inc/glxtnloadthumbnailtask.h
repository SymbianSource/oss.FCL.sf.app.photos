/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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




#ifndef C_GLXTNLOADTHUMBNAILTASK_H
#define C_GLXTNLOADTHUMBNAILTASK_H

#include "glxtntask.h"
#include "glxtnstd.h"

// Forward declarations
class CGlxtnFileInfo;
class CGlxtnFileUtility;
class CFbsBitmap;
class CImageDecoder;
class TGlxThumbnailRequest;

/**
 * Base class for tasks which load thumbnails.
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS( CGlxtnLoadThumbnailTask ) : public CGlxtnClientTask
    {
public:
    /**
    * Destructor.
    */
    ~CGlxtnLoadThumbnailTask();

protected:  // From CGlxtnTask
	void DoCancel();

protected:
    /**
    * C++ default constructor.
    * @param aId Task type ID.
    * @param aRequestInfo Parameters for the thumbnail request.
    * @param aFileUtility File utility for use by the task.
    * @param aClient Client initiating the request.
    */
    CGlxtnLoadThumbnailTask( const TGlxtnTaskId& aId,
                            const TGlxThumbnailRequest& aRequestInfo,
                            CGlxtnFileUtility& aFileUtility,
                            MGlxtnThumbnailCreatorClient& aClient );
    /**
    * Symbian 2nd phase constructor.
    * @param aBitmapHandle Handle to bitmap in which to store the thumbnail.
    */
    void ConstructL( TInt aBitmapHandle );

    /**
    * Start asynchronous loading.
    * @param aStatus Request status for the asynchronous operation.
    * @return ETrue if an asynchronous operation has been started.
    */
    TBool LoadThumbnailL( TRequestStatus& aStatus );
    /**
    * Create bitmap from loaded thumbnail data.
    * @param aStatus Request status for the asynchronous operation.
    * @return ETrue if an asynchronous operation has been started.
    */
    TBool HandleLoadedThumbnailL( TRequestStatus& aStatus );
    /**
    * Start asynchronous decoding.  iDecoder should point to a valid decoder.
    * @param aStatus Request status for the asynchronous operation.
    * @param aScaleBitmap If true, scale the loaded bitmap close to
    *       requested size.
    */
    void DecodeThumbnailL( TRequestStatus& aStatus, TBool aScaleBitmap );

protected:
    enum TGeneratorState
        {
        EStateFetchingUri, EStateLoading, EStateDecodingThumbnail,
        EStateDecodingImage, EStateScaling, EStateFiltering
        };

    /** File utility */
    CGlxtnFileUtility& iFileUtility;
	/** Current state for task state machine */
    TGeneratorState iState;
    /** File information for item to thumbnail (owned) */
    CGlxtnFileInfo* iInfo;
    /** Requested thumbnail size */
    TSize iRequestedSize;
    /** Thumbnail image data (owned) */
    HBufC8* iThumbData;
    /** Thumbnail data format */
    TGlxImageDataFormat iFormat;
    /** Decoder to decode the thumbnail data (owned) */
    CImageDecoder* iDecoder;
    /** Bitmap to decode image into (owned) */
    CFbsBitmap* iThumbnail;
    /** DRM allowed flag passed from thumbnail request */
    TBool iDrmAllowed;
    };

#endif // C_GLXTNLOADTHUMBNAILTASK_H

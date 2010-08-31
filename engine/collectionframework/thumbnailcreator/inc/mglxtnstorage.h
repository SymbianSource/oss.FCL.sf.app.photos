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
* Description:   Thumbnail storage interface
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef M_GLXTNSTORAGE_H
#define M_GLXTNSTORAGE_H

// INCLUDES

#include <e32std.h>
#include <glxmediaid.h>
#include "glxtnstd.h"

class CGlxtnFileInfo;

// CLASS DECLARATION

/**
*  Interface for thumbnail persistent storage.
*
* @ingroup glx_thumbnail_creator
*/
class MGlxtnThumbnailStorage
    {
public:
    /**
    * Start asynchronous loading of thumbnail data.
    * @param aData Buffer pointer for the loaded data. Memory is allocated
                internally and ownership passed to the caller.
    * @param aFormat Format of the thumbnail in aData.
    * @param aId Media item ID.
    * @param aFileInfo File info for media item.
    * @param aSize Requested thumbnail size.
    * @param aStatus Request status for the asynchronous operation.
    */
    virtual void LoadThumbnailDataL(HBufC8*& aData,
                        TGlxImageDataFormat& aFormat, const TGlxMediaId& aId,
                        const CGlxtnFileInfo& aFileInfo, const TSize& aSize,
                        TRequestStatus* aStatus) = 0;
    /**
    * Start asynchronous saving of thumbnail data.
    * @param aData Buffer containing the data to be saved.
    * @param aFormat Format of the thumbnail in aData.
    * @param aId Media item ID.
    * @param aFileInfo File info for media item.
    * @param aSize Requested thumbnail size.
    * @param aStatus Request status for the asynchronous operation.
    */
    virtual void SaveThumbnailDataL(const TDesC8& aData,
                        TGlxImageDataFormat aFormat, const TGlxMediaId& aId,
                        const CGlxtnFileInfo& aFileInfo, const TSize& aSize,
                        TRequestStatus* aStatus) = 0;
    /**
    * Start asynchronous deletion of all saved thumbnails for an item.
    * @param aId Media item ID.
    * @param aFileInfo File info for media item.
    * @param aStatus Request status for the asynchronous operation.
    */
    virtual void DeleteThumbnailsL(const TGlxMediaId& aId,
                const CGlxtnFileInfo& aFileInfo, TRequestStatus* aStatus) = 0;
    /**
    * Start asynchronous deletion of all saved thumbnails for items which no
    * longer exist.
    * @param aStatus Request status for the asynchronous operation.
    */
    virtual void CleanupThumbnailsL(TRequestStatus* aStatus) = 0;
    /**
    * Test whether a given thumbnail is available in storage.
    * @param aId Media item ID.
    * @param aFileInfo File info for media item.
    * @param aSize Requested thumbnail size.
    * @param aStatus Request status for the asynchronous operation.
    */
    virtual void IsThumbnailAvailableL(const TGlxMediaId& aId,
                        const CGlxtnFileInfo& aFileInfo,
                        const TSize& aSize, TRequestStatus* aStatus) = 0;
    /**
    * Cancel an ongoing asynchronous operation.
    */
    virtual void StorageCancel() = 0;
    /**
    * Notify that an error occurred in background generation.
    * @param aId Media item ID.
    * @param aError Error code.
    */
    virtual void NotifyBackgroundError(const TGlxMediaId& aId, TInt aError) = 0;
    };

/**
*  Interface for notification of thumbnails becoming available in storage.
*
* @ingroup glx_thumbnail_creator
*/
class MGlxtnThumbnailStorageObserver
    {
public:
    /**
    * Notify that a given thumbnail is available in storage.
    * @param aId Media item ID.
    * @param aSize Requested thumbnail size.
    */
    virtual void ThumbnailAvailable(const TGlxMediaId& aId, const TSize& aSize) = 0;
    /**
    * Notify that an error occurred in background generation.
    * @param aId Media item ID.
    * @param aError Error code.
    */
    virtual void BackgroundThumbnailError(const TGlxMediaId& aId, TInt aError) = 0;
    };

#endif  // M_GLXTNSTORAGE_H

// End of File

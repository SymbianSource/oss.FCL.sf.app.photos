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
* Description:   Definition of CGlxtnVolumeDatabase
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef M_GLXTNVOLUMEDATABASEOBSERVER_H
#define M_GLXTNVOLUMEDATABASEOBSERVER_H

#include "glxtnstd.h"   // For TGlxImageDataFormat

/**
 * Typesafe thumbnail ID.
 */
class TGlxtnThumbnailIdTypeBase {};     // Don't use this
typedef TGlxId<TGlxtnThumbnailIdTypeBase> TGlxtnThumbnailId;    // Use this

/**
 * Interface to receive callbacks from a CGlxtnVolumeDatabase.
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
class MGlxtnVolumeDatabaseObserver
    {
public:
    /**
    * Called when a database operation cannot be completed.
    * @param aError Error code.
    */
    virtual void HandleDatabaseError(TInt aError) = 0;
    /**
    * Called when a thumbnail ID has been found in the Ids table.
    * @param aThumbId Thumbnail ID for the media item.
    */
    virtual void HandleThumbnailIdFromMediaIdL(
                                    const TGlxtnThumbnailId& aThumbId ) = 0;
    /**
    * Called when a thumbnail ID has been found in the Items table.
    * @param aThumbId Thumbnail ID for the media item.
    */
    virtual void HandleThumbnailIdFromFilenameL(
                                    const TGlxtnThumbnailId& aThumbId ) = 0;
    /**
    * Called when a thumbnail ID has been saved in the Ids table.
    */
    virtual void HandleThumbnailIdStoredL() = 0;
    /**
    * Called when a thumbnail has been found in the Thumbnails table.
    * @param aFormat Thumbnail data format.
    * @param aData Thumbnail binary data (ownership passed).
    */
    virtual void HandleThumbnail(TGlxImageDataFormat aFormat, HBufC8* aData) = 0;
    /**
    * Called when a thumbnail has been saved in the Thumbnails table.
    */
    virtual void HandleThumbnailStored() = 0;
    /**
    * Called when a record has been deleted from a Ids table.
    */
    virtual void HandleMediaIdDeletedL() = 0;
    /**
    * Called when records have been deleted from a Thumbnails table.
    */
    virtual void HandleThumbnailsDeletedL() = 0;
    /**
    * Called when a record has been deleted from a Items table.
    */
    virtual void HandleItemDeletedL() = 0;
    /**
    * Called when availability has been chaecked.
    * @param aResult The result of the check.
    */
    virtual void HandleAvailabilityChecked(TInt aResult) = 0;
    /**
    * Called after the database has been cleaned up.
    */
    virtual void HandleDatabaseCleanedL() = 0;
    };

#endif  // M_GLXTNVOLUMEDATABASEOBSERVER_H

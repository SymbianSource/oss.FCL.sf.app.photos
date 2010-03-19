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
* Description:   Thumbnail creator client interface
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef M_GLXTNTHUMBNAILCREATORCLIENT_H
#define M_GLXTNTHUMBNAILCREATORCLIENT_H

// INCLUDES

#include <e32std.h>
#include <glxthumbnailinfo.h>

// FUNCTION PROTOTYPES

class CGlxtnFileInfo;
class MGlxtnThumbnailStorage;
class TGlxMediaId;

// CLASS DECLARATION

/**
*  Interface for clients of the TNC.
*
* @ingroup glx_thumbnail_creator
*/
class MGlxtnThumbnailCreatorClient
    {
public:
    /**
	* Notifies that a thumbnail for a given item is available, or that
	* thumbnail generation failed.
    * @param aItemId The item for which the thumbnail was required.
    * @param aQuality quality of the thumbnail
    * @param aErrorCode KErrNone if successful, otherwise an error code.
    */
    virtual void ThumbnailFetchComplete(const TGlxMediaId& aItemId,
                        TGlxThumbnailQuality aQuality, TInt aErrorCode) = 0;
    /**
	* Notifies that deletion of thumbnails for a given item is complete, or
	* the operation failed.
    * @param aItemId The item whose thumbnails were deleted.
    * @param aErrorCode KErrNone if successful, otherwise an error code.
    */
    virtual void ThumbnailDeletionComplete(const TGlxMediaId& aItemId,
                                            TInt aErrorCode) = 0;
    /**
	* Notifies that filtering items with available thumbnails from a list is
	* complete, or the operation failed.
    * @param aIdArray Array of IDs of items which don't have a thumbnail
    *           available.
    * @param aErrorCode KErrNone if successful, otherwise an error code.
    */
    virtual void FilterAvailableComplete(const RArray<TGlxMediaId>& aIdArray,
                                            TInt aErrorCode) = 0;
    /**
    * Called by a task to request file information for an item.  The
    * information is returned asynchronously.
    * @param aInfo Buffer to hold the information.
    * @param aItemId The item for which the URI is required.
    * @param aStatus Request status for the asynchronous operation.
    */
    virtual void FetchFileInfoL(CGlxtnFileInfo* aInfo,
                    const TGlxMediaId& aItemId, TRequestStatus* aStatus) = 0;
    /**
    * Cancel the request for an URI.
    * @param aItemId The item for which the URI was required.
    */
    virtual void CancelFetchUri(const TGlxMediaId& aItemId) = 0;
    /**
    * Get the thumbnail storage used by this client.
    * @return Pointer to the storage.
    */
    virtual MGlxtnThumbnailStorage* ThumbnailStorage() = 0;
    };

#endif  // M_GLXTNTHUMBNAILCREATORCLIENT_H

// End of File

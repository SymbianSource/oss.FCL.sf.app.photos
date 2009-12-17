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
* Description:    Thumbnail creator: provides thumbnails and generates on background
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef C_GLXTNTHUMBNAILCREATOR_H
#define C_GLXTNTHUMBNAILCREATOR_H

#include "glxtnstd.h"
#include "glxtntask.h"
#include <glxmediaid.h>
#include <glxpanic.h>
#include <glxthumbnailinfo.h>

class CGlxtnFileUtility;
class CGlxSettingsModel;
class MGlxtnThumbnailCreatorClient;
class MGlxtnThumbnailStorage;
class TGlxThumbnailRequest;

/**
 *  CGlxtnThumbnailCreator
 *
 *  Thumbnail creator is the thumbnail engine, and it loads and generates 
 *  thumbnails on the foreground and generates non-created thumbnails on
 *  the background
 *
 * @ingroup glx_thumbnail_creator
 */
class CGlxtnThumbnailCreator : public CBase
    {
public:
    /**
    * Starts the provisioning of a thumbnail.  This will result in a callback
    * to MGlxtnThumbnailCreatorClient::ThumbnailFetchComplete().
    */
    IMPORT_C static CGlxtnThumbnailCreator* InstanceL();
    /**
    * Decrement the reference count, and delete the instance if the count
    * becomes zero.  Ensures any tasks using the given storage are cancelled.
    * @param aStorage Pointer to storage used by client, or NULL.
    */
    IMPORT_C void Close(MGlxtnThumbnailStorage* aStorage);

    /**
    * Destructor.
    */
	~CGlxtnThumbnailCreator();

    /**
    * Starts the provisioning of a thumbnail.  This will result in a callback
    * to MGlxtnThumbnailCreatorClient::ThumbnailFetchComplete().
    * @param aRequestInfo Parameters for required thumbnail.
    * @param aClient Client initiating the request.
    */
    IMPORT_C void FetchThumbnailL(const TGlxThumbnailRequest& aRequestInfo,
                                    MGlxtnThumbnailCreatorClient& aClient);

    /**
    * Starts deleting all stored thumbnails for a given item.  This will result
    * in a callback to MGlxtnThumbnailCreatorClient::ThumbnailDeletionComplete().
    * @param aItemId ID of the media item.
    * @param aClient Client initiating the request.
    */
    IMPORT_C void DeleteThumbnailsL(const TGlxMediaId& aItemId,
                                    MGlxtnThumbnailCreatorClient& aClient);

    /**
    * Starts the filtering of items for which a thumbnail is available from a
    * list of item IDs.  This will result in a callback to
    * MGlxtnThumbnailCreatorClient::FilterAvailableComplete().
    * @param aItemArray Array of item IDs, from which those with thumbnails
    *                   should be removed.
    * @param aSize Size of thumbnail required.
    * @param aClient Client initiating the request.
    */
    IMPORT_C void FilterAvailableThumbnailsL(
                    const TArray<TGlxMediaId>& aItemArray, const TSize& aSize,
                    MGlxtnThumbnailCreatorClient& aClient);

    /**
    * Starts the cleanup of obsolete thumbnails from storage.  There is no
    * callback indicating completion of the request.
    * @param aStorage Pointer to storage used by client.
    */
    IMPORT_C void CleanupThumbnailsL(MGlxtnThumbnailStorage* aStorage);

    /** 
    * Cancel any ongoing tasks for a given item.  This ensures the TNC is not
    * locking the media file.
    * @param aItemId ID of the media item.
    */
    IMPORT_C void CancelRequest(const TGlxMediaId& aItemId);

private:
	CGlxtnThumbnailCreator();
	void ConstructL();

private:
    /** Task manager */
    CGlxtnTaskManager* iTaskManager;
    /** File utility used by the tasks */
    CGlxtnFileUtility* iFileUtility;
    /** Reference count of the single instance */
    TInt iReferenceCount;
    /** Settings Model used by the background tasks */
    CGlxSettingsModel* iSettingsModel;
    };

#endif // C_GLXTNTHUMBNAILCREATOR_H

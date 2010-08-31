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
* Description:   Task used for thumbnail availability filter.
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef C_GLXTNFILTERAVAILABLETASK_H
#define C_GLXTNFILTERAVAILABLETASK_H

#include "glxtntask.h"

// Forward declarations
class CGlxtnFileInfo;

/**
 * Task to filter a list of media item IDs to include only those which don't
 * have a high quality thumbnail of a given size.
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnFilterAvailableTask) : public CGlxtnClientTask
    {
public:
    /**
    * Two-phased constructor.
    * @param aItemArray Array of item IDs, from which those with thumbnails
    *                   should be removed.
    * @param aSize Size of thumbnail required.
    * @param aClient Client initiating the request.
    */
	static CGlxtnFilterAvailableTask* NewL(
            	    const TArray<TGlxMediaId>& aItemArray, const TSize& aSize,
            	    MGlxtnThumbnailCreatorClient& aClient);
    /**
    * Destructor.
    */
    ~CGlxtnFilterAvailableTask();

protected:  // From CGlxtnTask

    TBool DoStartL(TRequestStatus& aStatus);
	void DoCancel();
	TBool DoRunL(TRequestStatus& aStatus);
    TBool DoRunError(TInt aError);

private:
    /**
    * C++ default constructor.
    * @param aSize Size of thumbnail required.
    * @param aClient Client initiating the request.
    */
    CGlxtnFilterAvailableTask(const TSize& aSize,
                                MGlxtnThumbnailCreatorClient& aClient);
    /**
    * Symbian 2nd phase constructor.
    * @param aItemArray Array of item IDs, from which those with thumbnails
    *                   should be removed.
    */
	void ConstructL(const TArray<TGlxMediaId>& aItemArray);

    /**
    * Start asynchronous request for file information for an item.
    * @param aStatus Request status for the asynchronous operation.
    */
    TBool GetFileInfoL(TRequestStatus& aStatus);
    /**
    * Start asynchronous check for thumbnail in persistent storage.
    * @param aStatus Request status for the asynchronous operation.
    */
    TBool CheckIdL(TRequestStatus& aStatus);

private:
    enum TFilterState
        {
        EStateFetchingUri, EStateChecking
        };

    /** Size of thumbnail for which to check */
    TSize iSize;
    /** Array of media IDs from which those with thumbnails are removed */
    RArray<TGlxMediaId> iIdArray;
    /** Current task state */
    TFilterState iState;
    /** Current index in Id array */
    TInt iIndex;
    /** File info for current item */
    CGlxtnFileInfo* iFileInfo;
    };

#endif  // C_GLXTNFILTERAVAILABLETASK_H

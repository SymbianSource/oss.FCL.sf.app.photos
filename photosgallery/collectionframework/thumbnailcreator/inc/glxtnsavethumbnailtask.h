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
* Description:    Thumbnail save task implementation
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef GLXTNSAVETHUMBNAILTASK_H
#define GLXTNSAVETHUMBNAILTASK_H

// INCLUDES

#include "glxtntask.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CFbsBitmap;
class CImageEncoder;
class CGlxtnFileInfo;

// CLASS DECLARATION

/**
* Task to save a generated thumbnail in persistent storage.
*
* @author Dan Rhodes
* @ingroup glx_thumbnail_creator
*/
NONSHARABLE_CLASS(CGlxtnSaveThumbnailTask) : public CGlxtnTask
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param iItemId Media ID of item whose thumbnail is to be saved.
        * @param aFileInfo Information about the media file.
        * @param aSize Requested thumbnail size.
        * @param aThumbnail Thumbnail bitmap to save.
        * @param aStorage Client's storage interface.
        */
        static CGlxtnSaveThumbnailTask* NewL(
                    const TGlxMediaId& aItemId,
                    CGlxtnFileInfo* aFileInfo, const TSize& aSize,
                    CFbsBitmap* aThumbnail, MGlxtnThumbnailStorage* aStorage);

        /**
        * Destructor.
        */
        virtual ~CGlxtnSaveThumbnailTask();

    protected:  // From CGlxtnTask
        TBool DoStartL(TRequestStatus& aStatus);
    	void DoCancel(); 
    	TBool DoRunL(TRequestStatus& aStatus);
        TBool DoRunError(TInt aError);

    protected:
        /**
        * Constructor for derived classes.
        * @param aId Task type ID.
        * @param iItemId Media ID of item whose thumbnail is to be saved.
        * @param aStorage Client's storage interface.
        */
        CGlxtnSaveThumbnailTask(const TGlxtnTaskId& aId,
                const TGlxMediaId& aItemId, MGlxtnThumbnailStorage* aStorage);

        /**
        * Encode the bitmap to a data buffer.
        * @param aStatus Request status for the asynchronous operation.
        */
        void EncodeThumbnailL(TRequestStatus& aStatus);
        /**
        * Save buffer to storage.
        * @param aStatus Request status for the asynchronous operation.
        */
        void SaveThumbnailL(TRequestStatus& aStatus);

    private:
        /**
        * Constructor.
        * @param iItemId Media ID of item whose thumbnail is to be saved.
        * @param aSize Requested thumbnail size.
        * @param aStorage Client's storage interface.
        */
        CGlxtnSaveThumbnailTask(const TGlxMediaId& aItemId, const TSize& aSize,
                                MGlxtnThumbnailStorage* aStorage);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aFileInfo Information about the media file.
        * @param aThumbnail Thumbnail bitmap to save.
        */
        void ConstructL(CGlxtnFileInfo* aFileInfo, CFbsBitmap* aThumbnail);

    protected:  // Data
        /**
        * States for the task's state machine.
        */
        enum TSaveState
            {
            EStateEncoding, EStateSaving, EStateFetchingUri, 
            EStateDecoding, EStateChecking, EStateScaling, EStateFiltering
            };

        /** Requested thumbnail size */
        TSize iSize;
        /** Information about the item's media file */
        CGlxtnFileInfo* iFileInfo;
        /** Thumbnail bitmap to save (owned) */
        CFbsBitmap* iThumbnail;
        /** Current task state */
        TSaveState iState;

    private:    // Data
        /** Format in which the bitmap was encoded */
        TGlxImageDataFormat iFormat;
        /** Encoder for the bitmap (owned) */
        CImageEncoder* iEncoder;
        /** Buffer for encoded data (owned) */
        HBufC8* iData;
    };

#endif  // GLXTNSAVETHUMBNAILTASK_H

// End of File

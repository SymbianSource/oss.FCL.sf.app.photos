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
* Description:   Thumbnail storage implementation
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef GLXTNDATABASE_H
#define GLXTNDATABASE_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include "mglxtnstorage.h"
#include "mglxtnvolumedatabaseobserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CGlxtnVolumeDatabase;

// CLASS DECLARATION

/**
*  Default implementation of thumbnail storage.
*
* @author Dan Rhodes
* @ingroup glx_thumbnail_creator
*/
class CGlxtnThumbnailDatabase : public CBase, public MGlxtnThumbnailStorage,
                                public MGlxtnVolumeDatabaseObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aDbFilename Filename (without path) for database.
        * @param aStorageObserver Observer of storage operations.
        */
        IMPORT_C static CGlxtnThumbnailDatabase* NewL(const TDesC& aDbFilename,
                MGlxtnThumbnailStorageObserver* aStorageObserver = NULL);

        /**
        * Destructor.
        */
        virtual ~CGlxtnThumbnailDatabase();

    public: // New functions

    public: // Functions from MGlxtnThumbnailStorage
        void LoadThumbnailDataL(HBufC8*& aData, TGlxImageDataFormat& aFormat,
                    const TGlxMediaId& aId, const CGlxtnFileInfo& aFileInfo,
                    const TSize& aSize, TRequestStatus* aStatus);
        void SaveThumbnailDataL(const TDesC8& aData,
                    TGlxImageDataFormat aFormat, const TGlxMediaId& aId,
                    const CGlxtnFileInfo& aFileInfo, const TSize& aSize,
                    TRequestStatus* aStatus);
        void DeleteThumbnailsL(const TGlxMediaId& aId,
                    const CGlxtnFileInfo& aFileInfo, TRequestStatus* aStatus);
        void CleanupThumbnailsL(TRequestStatus* aStatus);
        void IsThumbnailAvailableL(const TGlxMediaId& aId,
                    const CGlxtnFileInfo& aFileInfo,
                    const TSize& aSize, TRequestStatus* aStatus);
        void StorageCancel();
        void NotifyBackgroundError(const TGlxMediaId& aId, TInt aError);

    private:    // From MGlxtnVolumeDatabaseObserver
        void HandleDatabaseError(TInt aError);
        void HandleThumbnailIdFromMediaIdL(
                                        const TGlxtnThumbnailId& aThumbId );
        void HandleThumbnailIdFromFilenameL(
                                        const TGlxtnThumbnailId& aThumbId );
        void HandleThumbnailIdStoredL();
        void HandleThumbnail(TGlxImageDataFormat aFormat, HBufC8* aData);
        void HandleThumbnailStored();
        void HandleMediaIdDeletedL();
        void HandleThumbnailsDeletedL();
        void HandleItemDeletedL();
        void HandleAvailabilityChecked(TInt aResult);
        void HandleDatabaseCleanedL();

    private:

        /**
        * C++ default constructor.
        */
        CGlxtnThumbnailDatabase();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aDbFilename Filename (without path) for database.
        * @param aStorageObserver Observer of storage operations.
        */
        void ConstructL(const TDesC& aDbFilename,
                        MGlxtnThumbnailStorageObserver* aStorageObserver);

        /**
        * Continue current operation after receiving the thumbnail ID.
        */
        void DoHandleThumbnailIdL();

        /**
        * Open a volume database on the specified drive.
        * @param aDrive String beginnning with drive letter and colon.
        */
        CGlxtnVolumeDatabase* OpenDatabaseL(const TDesC& aDrive);

    private:    // Data
        enum TDatabaseOperation
            {
            ELoading, ESaving, EDeleting, ECleaning, ECheckingAvailable
            };

        /** Pointer to a storage observer */
        MGlxtnThumbnailStorageObserver* iStorageObserver;
        /** File server session */
        RFs iFs;
        /** Path to the database files (excluding drive letter) */
        TFileName iDatabasePath;
        /** Drive name of the phone memory drive */
        TDriveName iInternalDrive;
        /** Array of open databases (one per volume) */
        RPointerArray<CGlxtnVolumeDatabase> iDatabaseArray;
        /** Request status for asynchronous operation */
        TRequestStatus* iClientStatus;
        /** Operation currently being carried out by the database */
        TDatabaseOperation iCurrentOperation;
        /** Media ID for request */
        TGlxMediaId iMediaId;
        /** URI for request (not owned) */
        const CGlxtnFileInfo* iFileInfo;
        /** Thumbnail size for request */
        TSize iSize;
        /** Data buffer for load request */
        HBufC8** iLoadData;
        /** Data buffer for save request */
        TPtrC8 iSaveData;
        /** Thumbnail format for load request */
        TGlxImageDataFormat* iLoadFormat;
        /** Thumbnail format for save request */
        TGlxImageDataFormat iSaveFormat;
        /** ID for thumbnail */
        TGlxtnThumbnailId iThumbId;
        /** Index to count through iDatabaseArray */
        TInt  iDatabaseIndex;
    };

#endif  // GLXTNDATABASE_H

// End of File

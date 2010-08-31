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

#ifndef GLXTNVOLUMEDATABASE_H
#define GLXTNVOLUMEDATABASE_H

#include <e32base.h>
#include <d32dbms.h>    // For RDbStoreDatabase
#include <f32file.h>    // For TDriveName

#include <glxmediaid.h>
#include "glxtnstd.h"   // For TGlxImageDataFormat
#include "mglxtnvolumedatabaseobserver.h"   // For TGlxtnThumbnailId

class CFileStore;
class CGlxtnFileInfo;
class MGlxtnVolumeDatabaseObserver;

/**
 * CGlxtnVolumeDatabase implements the database for a single volume.
 *
 * @author Dan Rhodes
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnVolumeDatabase) : public CActive
    {
public:
    /**
    * Static constructor.
    * @param aObserver Observer of database operations.
    * @param aFs File server handle.
    * @param aPath Full path to the database file to use.
    */
    static CGlxtnVolumeDatabase* NewLC(MGlxtnVolumeDatabaseObserver& aObserver,
                                       RFs& aFs, 
                                       const TDesC& aPath);

    /**
    * Destructor
    */
    ~CGlxtnVolumeDatabase();

    /**
    * Get the drive this database is stored on
    */
    const TDesC& Drive() const;

    /**
    * Look up thumbnail ID from Ids table
    * @param aMediaId Media item ID.
    */
    void GetThumbnailIdL( const TGlxMediaId& aMediaId );
    /**
    * Look up thumbnail ID from Items table.  If not found, add new record.
    * @param aInfo File info for media item.
    */
    void GetThumbnailIdL(const CGlxtnFileInfo* aInfo);
    /**
    * Add record to Ids table
    * @param aMediaId Media item ID.
    * @param aThumbId Thumbnail ID.
    */
    void StoreThumbnailIdL( const TGlxMediaId& aMediaId,
                            const TGlxtnThumbnailId& aThumbId );
    /**
    * Look up thumbnail from Thumbnails table
    * @param aThumbId Thumbnail ID.
    * @param aSize Thumbnail size.
    */
    void GetThumbnailL( const TGlxtnThumbnailId& aThumbId, const TSize& aSize );
    /**
    * Check if a particular thumbnail is in Thumbnails table
    * @param aThumbId Thumbnail ID.
    * @param aSize Thumbnail size.
    */
    void CheckAvailableL( const TGlxtnThumbnailId& aThumbId, const TSize& aSize );
    /**
    * Add record to Thumbnails table
    * @param aThumbId Thumbnail ID.
    * @param aSize Thumbnail size.
    * @param aFormat Thumbnail data format.
    * @param aData Thumbnail binary data.
    */
    void StoreThumbnailL( const TGlxtnThumbnailId& aThumbId, const TSize& aSize,
                        TGlxImageDataFormat aFormat, const TDesC8& aData );
                        
    /**
    * Delete a record from the Ids table
    * @param aMediaId MediaId of record to delete.
    */
    void DeleteIdL( const TGlxMediaId& aMediaId );
    
    /**
    * Delete a record from the Thumbnails table
    * @param aThumbId ThumbId of records to delete.
    */
    void DeleteThumbnailsL( const TGlxtnThumbnailId& aThumbId );
    
    /**
    * Delete a record from the Items table
    * @param aThumbId ThumbId of record to delete.
    */
    void DeleteItemL( const TGlxtnThumbnailId& aThumbId );
    
    /**
    * Cleanup Database (remove entries that do not have corresponding file)
    */
    void CleanupDatabaseL();
    
protected:  // From CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:
    /**
    * Default constructor.
    * @param aObserver Observer of database operations.
    * @param aFs File server handle.
    */
    CGlxtnVolumeDatabase(MGlxtnVolumeDatabaseObserver& aObserver, RFs& aFs);
    /**
    * 2nd phase constructor.
    * @param aPath Full path to the database file to use.
    */
    void ConstructL(const TDesC& aPath);

    /**
    * Open an existing database.
    * @param aFs File server handle.
    * @param aPath Full path to the database file to use.
    */
    void OpenDbL(RFs& aFs, const TDesC& aFilename);

    /**
    * Create a new database.
    * @param aFs File server handle.
    * @param aPath Full path to the database file to use.
    */
    void CreateDbL(RFs& aFs, const TDesC& aFilename);

    /**
    * Add record to Items table
    * @return Thumbnail ID of new record.
    */
    TGlxtnThumbnailId DoAddItemL();

    /**
    * Evaluate a view based on a SQL query.
    * @param aQuery The SQL select statement.
    */
    void EvaluateQueryL( const TDbQuery &aQuery );

    /**
    * Execute a SQL data update statement.
    * @param aSql The SQL statement.
    */
    void UpdateDataL( const TDesC& aSql );

    /**
    * Test and cleanup a row in Items table
    */
    void CleanupRowL();
 
    /**
    * Modify SQL string to handle quotes correctly
    * @param aText String to be modified
    * @return New HBufC containing modified string
    */
    HBufC* QuoteSqlStringLC(const TDesC& aText);

private:
    enum TDatabaseState
        {
        EStateIdle, 
        EStateGettingIdFromMediaId, 
        EStateGettingIdFromFilename,
        EStateGettingThumbnail,
        EStateDeletingId,
        EStateDeletingThumbnails,
        EStateDeletingItem,
        EStateCheckingAvailability,
        EStateCleaning,
        EStateCleaningDeletingThumbnails
        };

    /** Observer of this database */
    MGlxtnVolumeDatabaseObserver& iObserver;
    /** File server session */
    RFs& iFs;
    /** Drive name of the volume */
    TDriveName iDrive;
    /** Handle to the database */
    RDbStoreDatabase iDatabase;
    /** Store used by the database */
    CFileStore* iStore;
    /** ID for next stored thumbnail */
    TUint iNextThumbId;
    /** File info for media item current query relates to (not owned) */
    const CGlxtnFileInfo* iInfo;
    /** Current state */
    TDatabaseState iState;
    /** View used for current query */
    RDbView iView;
    /** Updater to update a table */
    RDbUpdate iDbUpdater;
    /** Table used for query/deletion of rows in table */
    RDbTable iTable;
    };

#endif  // GLXTNVOLUMEDATABASE_H

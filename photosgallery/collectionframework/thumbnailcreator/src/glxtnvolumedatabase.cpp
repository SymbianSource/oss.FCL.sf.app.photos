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
* Description:    Implementation of CGlxtnVolumeDatabase
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnvolumedatabase.h"

#include <glxtracer.h>
#include <glxpanic.h>
#include <s32file.h>

#include "glxtnfileinfo.h"
#include "mglxtnvolumedatabaseobserver.h"

#include <glxlog.h>
_LIT(KGlxCreateTableIds, "CREATE TABLE Ids (MediaId UNSIGNED INTEGER NOT NULL, ThumbId UNSIGNED INTEGER NOT NULL)");
_LIT(KGlxCreateTableItems, "CREATE TABLE Items (Uri VARCHAR NOT NULL, ThumbId UNSIGNED INTEGER NOT NULL, FileSize INTEGER, ModTime TIME)");
_LIT(KGlxCreateTableThumbnails, "CREATE TABLE Thumbnails (ThumbId UNSIGNED INTEGER NOT NULL, Width INTEGER NOT NULL, Height INTEGER NOT NULL, Format INTEGER NOT NULL, ImageData LONG VARBINARY NOT NULL)");
_LIT(KGlxCreateIndexIds, "CREATE UNIQUE INDEX IdIndex ON Ids (MediaId)");
_LIT(KGlxCreateIndexItems, "CREATE UNIQUE INDEX ItemIndex ON Items (ThumbId ASC)");
_LIT(KGlxCreateIndexThumbnails, "CREATE UNIQUE INDEX ThumbnailIndex ON Thumbnails (ThumbId, Width, Height)");
_LIT(KGlxCreateIndexThumbIds, "CREATE UNIQUE INDEX ThumbIdIndex ON Ids (ThumbId)");

_LIT(KGlxTableIds, "Ids");
_LIT(KGlxTableItems, "Items");
_LIT(KGlxTableThumbnails, "Thumbnails");
_LIT(KGlxIndexItems, "ItemIndex");
_LIT(KGlxIndexThumbIds, "ThumbIdIndex");

const TInt KGlxColIdMediaId = 1;
const TInt KGlxColIdThumbId = 2;

const TInt KGlxColItemUri = 1;
const TInt KGlxColItemId = 2;
const TInt KGlxColItemFileSize = 3;
const TInt KGlxColItemModTime = 4;

const TInt KGlxColThumbnailId = 1;
const TInt KGlxColThumbnailWidth = 2;
const TInt KGlxColThumbnailHeight = 3;
const TInt KGlxColThumbnailFormat = 4;
const TInt KGlxColThumbnailData = 5;

_LIT(KGlxQueryThumbIdFromIds, "SELECT * FROM Ids WHERE MediaId = ");
_LIT(KGlxQueryThumbIdFromItems, "SELECT * FROM Items WHERE Uri = ");
_LIT(KGlxQueryThumbnail, "SELECT * FROM Thumbnails WHERE ThumbId = %d AND Width = %d AND Height = %d");
_LIT(KGlxQueryAvailable, "SELECT ThumbId, Width, Height FROM Thumbnails WHERE ThumbId = %d AND Width = %d AND Height = %d");
_LIT(KGlxQueryItems, "SELECT * FROM Items WHERE ThumbId = %d");

_LIT(KGlxDeleteId, "DELETE FROM Ids WHERE MediaId = %d");
_LIT(KGlxDeleteThumbnails, "DELETE FROM Thumbnails WHERE ThumbId = %d");
_LIT(KGlxDeleteItem, "DELETE FROM Items WHERE ThumbId = %d");

const TInt KGlxTIntMaxDigits = 11;

const TUint KGlxFirstThumbnailId = 1;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxtnVolumeDatabase* CGlxtnVolumeDatabase::NewLC(
        MGlxtnVolumeDatabaseObserver& aObserver, RFs& aFs, const TDesC& aPath)
    {
    TRACER("CGlxtnVolumeDatabase* CGlxtnVolumeDatabase::NewLC( MGlxtnVolumeDatabaseObserver& aObserver, RFs& aFs, const TDesC& aPath)");
    CGlxtnVolumeDatabase* self = 
                        new (ELeave) CGlxtnVolumeDatabase(aObserver, aFs);
    CleanupStack::PushL(self);
    self->ConstructL(aPath);
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnVolumeDatabase::CGlxtnVolumeDatabase(
                                    MGlxtnVolumeDatabaseObserver& aObserver, 
                                    RFs& aFs)
    : CActive(EPriorityStandard), 
      iObserver(aObserver), iFs(aFs), iState(EStateIdle)
    {
    TRACER("CGlxtnVolumeDatabase::CGlxtnVolumeDatabase( MGlxtnVolumeDatabaseObserver& aObserver, RFs& aFs)");
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::ConstructL(const TDesC& aPath)
    {
    TRACER("void CGlxtnVolumeDatabase::ConstructL(const TDesC& aPath)");
    iDrive = aPath.Left(KMaxDriveName);

    TRAPD(error, OpenDbL(iFs, aPath));
    GLX_LOG_INFO1("CGlxtnVolumeDatabase::ConstructL OpenDbL TRAP error = %d", error);
    if ( KErrNone != error )
        {
        iDatabase.Close();
        delete iStore;
        iStore = NULL;
        CreateDbL(iFs, aPath);
        }

    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnVolumeDatabase::~CGlxtnVolumeDatabase() 
    {
    TRACER("CGlxtnVolumeDatabase::~CGlxtnVolumeDatabase()");
    Cancel();
    iView.Close();
    iTable.Close();
    iDbUpdater.Close();
    iDatabase.Close();
    delete iStore;
    }

// -----------------------------------------------------------------------------
// Drive
// -----------------------------------------------------------------------------
//
const TDesC& CGlxtnVolumeDatabase::Drive() const
    {
    TRACER("TDesC& CGlxtnVolumeDatabase::Drive()");
    return iDrive;
    }

// -----------------------------------------------------------------------------
// GetThumbnailIdL
// Look up thumbnail ID from Ids table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::GetThumbnailIdL( const TGlxMediaId& aMediaId )
    {
    TRACER("void CGlxtnVolumeDatabase::GetThumbnailIdL( const TGlxMediaId& aMediaId )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }

    GLX_LOG_INFO1("CGlxtnVolumeDatabase::GetThumbnailIdL() Media Id= %d",aMediaId.Value());
    HBufC* sql = HBufC::NewLC(
                    KGlxQueryThumbIdFromIds().Length() + KGlxTIntMaxDigits );
    *sql = KGlxQueryThumbIdFromIds;
    sql->Des().AppendNum( aMediaId.Value() );

    EvaluateQueryL( *sql );
    iState = EStateGettingIdFromMediaId;

    CleanupStack::PopAndDestroy(sql);
    }

// -----------------------------------------------------------------------------
// GetThumbnailIdL
// Look up thumbnail ID from Items table.  If not found, add new record.
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::GetThumbnailIdL(const CGlxtnFileInfo* aInfo)
    {
    TRACER("void CGlxtnVolumeDatabase::GetThumbnailIdL(const CGlxtnFileInfo* aInfo)");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }

    iInfo = aInfo;
    HBufC* uri = QuoteSqlStringLC(iInfo->FilePath());
    HBufC* sql = HBufC::NewLC(
                    KGlxQueryThumbIdFromItems().Length() + uri->Length());
    *sql = KGlxQueryThumbIdFromItems;
    sql->Des().Append(*uri);

    EvaluateQueryL( *sql );
    iState = EStateGettingIdFromFilename;

    CleanupStack::PopAndDestroy(sql);
    CleanupStack::PopAndDestroy(uri);
    }

// -----------------------------------------------------------------------------
// GetThumbnailIdL
// Retrieve the thumbnail id to be assigned to the next thumbnail that gets stored in the database. 
// -----------------------------------------------------------------------------
//
TGlxtnThumbnailId CGlxtnVolumeDatabase::GetThumbnailId()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxtnVolumeDatabase::GetThumbnailIdL()");
    TGlxtnThumbnailId thumbId( iNextThumbId );
    iNextThumbId++;
    GLX_LOG_INFO1("CGlxtnVolumeDatabase::InitializeThumbIdL iNextThumbId=%d", iNextThumbId);
    return thumbId;
    }

// -----------------------------------------------------------------------------
// StoreThumbnailIdL
// Add record to Ids table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::StoreThumbnailIdL( const TGlxMediaId& aMediaId,
                                        const TGlxtnThumbnailId& aThumbId )
    {
    TRACER("void CGlxtnVolumeDatabase::StoreThumbnailIdL( const TGlxMediaId& aMediaId, const TGlxtnThumbnailId& aThumbId )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }
    GLX_LOG_INFO1("StoreThumbnailIdL():- Media Id = %d",aMediaId.Value());
    GLX_LOG_INFO1("StoreThumbnailIdL():- aThumbId = %d",aThumbId.Value());
    RDbTable table;
    CleanupClosePushL(table);
    User::LeaveIfError( table.Open(
                        iDatabase, KGlxTableIds, RDbRowSet::EInsertOnly ) );

    table.InsertL();
    table.SetColL( KGlxColIdMediaId, aMediaId.Value() );
    table.SetColL( KGlxColIdThumbId, aThumbId.Value() );
    table.PutL();
    GLX_LOG_INFO2("StoreThumbnailIdL():- Added media id [%d] and thumb id  [%d] to Ids table. ", aMediaId.Value(), aThumbId.Value());

    CleanupStack::PopAndDestroy(&table);

    iObserver.HandleThumbnailIdStoredL();
    }

void CGlxtnVolumeDatabase::StoreItemsL( const TGlxtnThumbnailId& aThumbId, const CGlxtnFileInfo* aInfo )
	{
    __ASSERT_ALWAYS(aInfo, Panic(EGlxPanicNullPointer));
    GLX_LOG_ENTRY_EXIT("void CGlxtnVolumeDatabase::StoreItemsL()");
    GLX_LOG_INFO1("StoreItemsL():- ThumbId Id = %d",aThumbId.Value());
    RDbTable table;
    CleanupClosePushL(table);
    User::LeaveIfError(table.Open(iDatabase, KGlxTableItems, RDbRowSet::EUpdatable));
    User::LeaveIfError(table.SetIndex(KGlxIndexItems));

    TGlxtnThumbnailId thumbId( aThumbId );

    HBufC* sql = HBufC::NewLC( KGlxQueryItems().Length() + KGlxTIntMaxDigits);
    sql->Des().Format( KGlxQueryItems, thumbId.Value());

    RDbView view;
    CleanupClosePushL(view);
    User::LeaveIfError(view.Prepare(
                       iDatabase, *sql, RDbRowSet::EReadOnly ) );
    view.EvaluateAll();

    if (view.IsEmptyL())
        {
        GLX_LOG_INFO("StoreItemsL() - Add a new record");
        table.InsertL();
        table.SetColL( KGlxColItemUri, aInfo->FilePath() );
        table.SetColL( KGlxColItemId, thumbId.Value() );
        table.SetColL( KGlxColItemFileSize, aInfo->iFileSize );
        table.SetColL( KGlxColItemModTime, aInfo->iFileTime );
        table.PutL();
        }
       
    CleanupStack::PopAndDestroy(&view);
    CleanupStack::PopAndDestroy(sql);
    CleanupStack::PopAndDestroy(&table);
    }

// -----------------------------------------------------------------------------
// GetThumbnailL
// Look up thumbnail from Thumbnails table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::GetThumbnailL( const TGlxtnThumbnailId& aThumbId,
                                            const TSize& aSize )
    {
    TRACER("void CGlxtnVolumeDatabase::GetThumbnailL( const TGlxtnThumbnailId& aThumbId, const TSize& aSize )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }

    GLX_LOG_INFO1("GetThumbnailL():- ThumbId Id = %d",aThumbId.Value());
    GLX_LOG_INFO2("GetThumbnailL Width=%d, Height=%d", aSize.iWidth, aSize.iHeight);
    HBufC* sql = HBufC::NewLC( KGlxQueryThumbnail().Length()
                                + 3 * KGlxTIntMaxDigits);
    sql->Des().Format( KGlxQueryThumbnail,
                        aThumbId.Value(), aSize.iWidth, aSize.iHeight );

    EvaluateQueryL( *sql );
    iState = EStateGettingThumbnail;

    CleanupStack::PopAndDestroy(sql);
    }

// -----------------------------------------------------------------------------
// CheckAvailableL
// Check if thumbnail is in Thumbnails table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::CheckAvailableL( const TGlxtnThumbnailId& aThumbId,
                                            const TSize& aSize )
    {
    TRACER("void CGlxtnVolumeDatabase::CheckAvailableL( const TGlxtnThumbnailId& aThumbId, const TSize& aSize )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }
    GLX_LOG_INFO1("CheckAvailableL():- ThumbId = %d", aThumbId.Value());

    HBufC* sql = HBufC::NewLC( KGlxQueryAvailable().Length()
                                + 3 * KGlxTIntMaxDigits);
    sql->Des().Format( KGlxQueryAvailable,
                        aThumbId.Value(), aSize.iWidth, aSize.iHeight );

    EvaluateQueryL( *sql );
    iState = EStateCheckingAvailability;

    CleanupStack::PopAndDestroy(sql);
    }

// -----------------------------------------------------------------------------
// StoreThumbnailL
// Add record to Thumbnails table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::StoreThumbnailL( const TGlxtnThumbnailId& aThumbId,
                                           const TSize& aSize,
                                           TGlxImageDataFormat aFormat,
                                           const TDesC8& aData )
    {
    TRACER("void CGlxtnVolumeDatabase::StoreThumbnailL( const TGlxtnThumbnailId& aThumbId, const TSize& aSize, TGlxImageDataFormat aFormat, const TDesC8& aData )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }
    GLX_LOG_INFO1("StoreThumbnailL():- aThumbId = %d",aThumbId.Value());
    GLX_LOG_INFO2("StoreThumbnailL Width=%d, Height=%d", aSize.iWidth, aSize.iHeight);

    RDbTable table;
    CleanupClosePushL(table);
    User::LeaveIfError( table.Open(
                iDatabase, KGlxTableThumbnails, RDbRowSet::EInsertOnly ) );

    table.InsertL();
    table.SetColL( KGlxColThumbnailId, aThumbId.Value() );
    table.SetColL( KGlxColThumbnailWidth, aSize.iWidth );
    table.SetColL( KGlxColThumbnailHeight, aSize.iHeight );
    table.SetColL( KGlxColThumbnailFormat, aFormat );
    table.SetColL( KGlxColThumbnailData, aData );
    table.PutL();

    CleanupStack::PopAndDestroy(&table);

    iObserver.HandleThumbnailStored();
    }

// -----------------------------------------------------------------------------
// DeleteFromIdsL
// Delete from IDs table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::DeleteIdL( const TGlxMediaId& aMediaId )
    {
    TRACER("void CGlxtnVolumeDatabase::DeleteIdL( const TGlxMediaId& aMediaId )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }

    GLX_LOG_INFO1("DeleteIdL():- Media Id = %d",aMediaId.Value());
    HBufC* sql = HBufC::NewLC( KGlxDeleteId().Length() + KGlxTIntMaxDigits);
    
    sql->Des().Format( KGlxDeleteId, aMediaId.Value() );
    
    UpdateDataL( *sql );
    iState = EStateDeletingId;
    
    CleanupStack::PopAndDestroy(sql);
    }
    
// -----------------------------------------------------------------------------
// DeleteThumbnailsL
// Delete Thumbnails from Thumbnail table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::DeleteThumbnailsL(
                                        const TGlxtnThumbnailId& aThumbId )
    {
    TRACER("void CGlxtnVolumeDatabase::DeleteThumbnailsL( const TGlxtnThumbnailId& aThumbId )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }
    GLX_LOG_INFO1("DeleteThumbnailsL():- aThumbId = %d",aThumbId.Value());

    HBufC* sql = HBufC::NewLC(KGlxDeleteThumbnails().Length() + 
                                 KGlxTIntMaxDigits);
    
    sql->Des().Format( KGlxDeleteThumbnails, aThumbId.Value() );
    
    UpdateDataL( *sql );
    iState = EStateDeletingThumbnails;
    
    CleanupStack::PopAndDestroy(sql);
    }
    
// -----------------------------------------------------------------------------
// DeleteItemL
// Delete Item from Items table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::DeleteItemL( const TGlxtnThumbnailId& aThumbId )
    {
    TRACER("void CGlxtnVolumeDatabase::DeleteItemL( const TGlxtnThumbnailId& aThumbId )");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }

    GLX_LOG_INFO1("DeleteItemL():- aThumbId = %d",aThumbId.Value());
    HBufC* sql = HBufC::NewLC( KGlxDeleteItem().Length() + KGlxTIntMaxDigits);
    
    sql->Des().Format( KGlxDeleteItem, aThumbId.Value() );
    
    UpdateDataL( *sql );
    iState = EStateDeletingItem;
    
    CleanupStack::PopAndDestroy(sql);
    }

// -----------------------------------------------------------------------------
// CleanupDatabaseL
// Clean from database entries that do not have a corresponding media file
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::CleanupDatabaseL()
    {
    TRACER("void CGlxtnVolumeDatabase::CleanupDatabaseL()");
    if ( EStateIdle != iState )
        {
        User::Leave(KErrNotReady);
        }
    User::LeaveIfError( iTable.Open(
                        iDatabase, KGlxTableItems, RDbRowSet::EUpdatable ) );
    iTable.BeginningL();

    SetActive();
    TRequestStatus* ts = &iStatus;
    User::RequestComplete(ts, KErrNone);

    iState = EStateCleaning;
    }

    
// -----------------------------------------------------------------------------
// CleanupRowL
// Test entry in one row in Items table to see if file is available. If it is
// not, delete this row and start process of deleting corresponding entries 
// from Ids table and Thumbnails table
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::CleanupRowL()
    {
    TRACER("void CGlxtnVolumeDatabase::CleanupRowL()");
    // Get filename
    iTable.GetL();
    TPtrC filename = iTable.ColDes(KGlxColItemUri);
   
    // check if file exists
    TTime time(0);
    TInt err = iFs.Modified(filename, time);
    GLX_LOG_INFO1("void CGlxtnVolumeDatabase::CleanupRowL() New Time = %Ld",time.Int64());
    TTime modifiedTime(0);
    modifiedTime = iTable.ColTime(KGlxColItemModTime);
    GLX_LOG_INFO1("void CGlxtnVolumeDatabase::CleanupRowL() modifiedTime (from thumb Db) = %Ld",modifiedTime.Int64());
    if(err == KErrNone && (modifiedTime.Int64() == time.Int64()))
        {
        GLX_LOG_INFO("void CGlxtnVolumeDatabase::CleanupRowL() DBmodTime and FileModTime Same");
        //yes, file exists and not modified, so complete to go to next row
        SetActive();
        TRequestStatus* ts = &iStatus;
        User::RequestComplete(ts, KErrNone);
        iState = EStateCleaning;
        }
    else
        {
        GLX_LOG_INFO("void CGlxtnVolumeDatabase::CleanupRowL() DBmodTime and FileModTime NOT Same");
        TUint thumbId = iTable.ColUint( KGlxColItemId );
        if (err != KErrNone)
	        {
	        GLX_LOG_INFO1("void CGlxtnVolumeDatabase::CleanupRowL() DBmodTime and FileModTime NOT Same But err = %d", err);
	        // Problem accessing file, so delete database entries
	        // First delete row in Items table
	        // Delete the row (entry in Items)
	        iTable.DeleteL();
	        }
        else
	        {
	        // file modified, so update the modified time in items table
	        iTable.UpdateL();
	        iTable.SetColL(KGlxColItemModTime, time);
	        iTable.PutL();
	        }
			
        // Now delete from Thumbnails
        HBufC* sql = HBufC::NewLC(KGlxDeleteThumbnails().Length() + 
	                                     KGlxTIntMaxDigits);
        sql->Des().Format( KGlxDeleteThumbnails, thumbId );
        UpdateDataL( *sql );
        iState = EStateCleaningDeletingThumbnails;
        CleanupStack::PopAndDestroy(sql);
        }
    }
    
// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::DoCancel()
    {
    TRACER("void CGlxtnVolumeDatabase::DoCancel()");
    iView.Cancel();
    iTable.Close();
    iDbUpdater.Close();
    iState = EStateIdle;
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::RunL()
    {
    TRACER("void CGlxtnVolumeDatabase::RunL()");
    User::LeaveIfError(iStatus.Int());

    // Continue any database operations which aren't finished
    switch ( iState )
        {
        case EStateGettingIdFromMediaId:
        case EStateGettingIdFromFilename:
        case EStateGettingThumbnail:
        case EStateCheckingAvailability:
            if ( iView.Unevaluated() )
                {
                iView.Evaluate(iStatus);
                SetActive();
                return;
                }
            break;
        case EStateDeletingId:
        case EStateDeletingThumbnails:
        case EStateDeletingItem:
        case EStateCleaningDeletingThumbnails:
            if(iStatus.Int() != 0)
                {
                iDbUpdater.Next(iStatus);
                SetActive();
                return;
                }
            break;
        case EStateCleaning:
        // do nothing
            break;
        default:
            Panic(EGlxPanicIllegalState);
            break;
        }

    // Handle results of database operation
    switch ( iState )
        {
        case EStateGettingIdFromMediaId:
            {
            TGlxtnThumbnailId thumbId;
            if ( iView.FirstL() )
                {
                iView.GetL();
                thumbId = iView.ColUint(KGlxColIdThumbId);
                GLX_LOG_INFO1("RunL - EStateGettingIdFromMediaId  IF (iView.FirstL()): ThumbId = %d", thumbId.Value());
                }
            else
                {
                GLX_LOG_INFO("RunL - EStateGettingIdFromMediaId (iView.FirstL()): ELSE ->  thumbId not retrieved" );                
                }
            iView.Close();
            iState = EStateIdle;
            iObserver.HandleThumbnailIdFromMediaIdL(thumbId);
            }
            break;

        case EStateGettingIdFromFilename:
            {
            TGlxtnThumbnailId thumbId;
            if ( iView.FirstL() )
                {
                iView.GetL();
                thumbId = iView.ColUint(KGlxColItemId);
                iView.Close();
                GLX_LOG_INFO1("RunL - EStateGettingIdFromFilename IF (iView.FirstL()): ThumbId =  %d", thumbId.Value());
                }
            else
                {
                iView.Close();
                GLX_LOG_INFO("RunL - EStateGettingIdFromMediaId (iView.FirstL()): ELSE ->  no Thumbnail Yet. So DoAddItemL() being called" );
                thumbId = DoAddItemL();
                GLX_LOG_INFO1("RunL - EStateGettingIdFromMediaId (iView.FirstL()): ELSE ->  ThumbId =  %d",  thumbId.Value());
                }
            iState = EStateIdle;
            iInfo = NULL;
            GLX_LOG_INFO1("RunL - EStateGettingIdFromFilename - aThumbId = %d", thumbId.Value());
            iObserver.HandleThumbnailIdFromFilenameL(thumbId);
            }
            break;

        case EStateGettingThumbnail:
            if ( iView.FirstL() )
                {
                iView.GetL();
                TGlxImageDataFormat format = static_cast<TGlxImageDataFormat>(
                                        iView.ColInt(KGlxColThumbnailFormat));
                TInt size = iView.ColSize(KGlxColThumbnailData);

                GLX_LOG_INFO("RunL: Reading Thumbnail from dB");
                RDbColReadStream stream;
                stream.OpenLC(iView, KGlxColThumbnailData);
                HBufC8* data = HBufC8::NewLC(size);
                TPtr8 ptr(data->Des());
                // Need to specify amount to read, as the HBufC8 can be bigger
                // than requested
                stream.ReadL(ptr, size);    

                CleanupStack::Pop(data);
                CleanupStack::PopAndDestroy(&stream);

                iView.Close();
                iState = EStateIdle;
                iObserver.HandleThumbnail(format, data);
                }
            else
                {
                iView.Close();
                iState = EStateIdle;
                GLX_LOG_INFO("RunL: HandleDatabaseError - KErrNotFound");
                iObserver.HandleDatabaseError(KErrNotFound);
                }
            break;
            
        case EStateDeletingId:
            iState = EStateIdle;
            iDbUpdater.Close();
            iObserver.HandleMediaIdDeletedL();
            break;
            
        case EStateDeletingThumbnails:
            iState = EStateIdle;
            iDbUpdater.Close();
            iObserver.HandleThumbnailsDeletedL();
            break;
            
        case EStateDeletingItem:
            iState = EStateIdle;
            iDbUpdater.Close();
            iObserver.HandleItemDeletedL();
            break;

        case EStateCheckingAvailability:
            {
            TInt result = KGlxThumbnailAvailable;
            if ( iView.IsEmptyL() )
                {
                result = KGlxThumbnailNotAvailable;
                }
            iView.Close();
            iState = EStateIdle;
            GLX_LOG_INFO1("RunL - EStateCheckingAvailability - result = %d", result);
            iObserver.HandleAvailabilityChecked(result);
            }
            break;
            
        case EStateCleaningDeletingThumbnails:
            iState = EStateCleaning;
            iDbUpdater.Close();      // deliberate fall through to next row
        case EStateCleaning:
            if(iTable.NextL())
                {
                // next row
                CleanupRowL();
                }
            else
                {
                // no more rows
                iTable.Close();
                iState = EStateIdle;
                iObserver.HandleDatabaseCleanedL();
                }
            break;
        default:
            Panic(EGlxPanicIllegalState);
            break;
        }
    }

// -----------------------------------------------------------------------------
// RunError
// -----------------------------------------------------------------------------
//
TInt CGlxtnVolumeDatabase::RunError(TInt aError)
    {
    TRACER("TInt CGlxtnVolumeDatabase::RunError(TInt aError)");
    GLX_LOG_INFO2("RunL: HandleDatabaseError - iState=%d, error=%d", 
	                                                iState, aError);
    TInt error = aError;
    iTable.Close();
    iView.Close();
    iDbUpdater.Close();
    iInfo = NULL;

 	if (aError == KErrCorrupt)
 		{
 		error = KErrCorruptThumbnailDatabase;
 		}
 	else if (aError == KErrEof)
        {
        error = KErrEofThumbnailDatabase;
        }
	
    iObserver.HandleDatabaseError(error);
    iState = EStateIdle;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// OpenDbL
// Open an existing database.
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::OpenDbL(RFs& aFs, const TDesC& aFilename)
    {
    TRACER("void CGlxtnVolumeDatabase::OpenDbL(RFs& aFs, const TDesC& aFilename)");
    iStore = CFileStore::OpenL(aFs, aFilename, EFileRead | EFileWrite);
    iDatabase.OpenL(iStore, iStore->Root());
    User::LeaveIfError(iDatabase.Recover());
    
    RDbTable tableIds;
    CleanupClosePushL(tableIds);

    // open and check the Ids table
    User::LeaveIfError( tableIds.Open(
                        iDatabase, KGlxTableIds, RDbRowSet::EReadOnly ) );

    User::LeaveIfError(tableIds.SetIndex(KGlxIndexThumbIds));
    
    CleanupStack::PopAndDestroy(&tableIds);

    }
    
// -----------------------------------------------------------------------------
// InitializeThumbIdL
// Initialize thumbnail identifier to the last known value plus one.
// -----------------------------------------------------------------------------
void CGlxtnVolumeDatabase::InitializeThumbIdL()
    {
    GLX_LOG_ENTRY_EXIT("void CGlxtnThumbnailDatabase::InitializeThumbIdL()");
    // Get next available thumbnail ID (synchronous)
    RDbTable tableIds;
    CleanupClosePushL(tableIds);

    // open and check the Ids table
    User::LeaveIfError( tableIds.Open(
                        iDatabase, KGlxTableIds, RDbRowSet::EReadOnly ) );

    User::LeaveIfError(tableIds.SetIndex(KGlxIndexThumbIds));

    // Find highest thumbnail ID in use.  New entries are added to the Items
    // table first, and deleted from the Items table last, so all IDs in use
    // will always be found there.
    // Thumbnail IDs are only unique within a volume.
    if ( tableIds.LastL() )
        {
        tableIds.GetL();
        iNextThumbId = tableIds.ColUint(KGlxColIdThumbId) + 1;
        }
    else
        {
        // Database is empty
        iNextThumbId = KGlxFirstThumbnailId;
        }
    GLX_LOG_INFO1("CGlxtnVolumeDatabase::InitializeThumbIdL iNextThumbId=%d", iNextThumbId);

    CleanupStack::PopAndDestroy(&tableIds);
    }

// -----------------------------------------------------------------------------
// CreateDbL
// Create a new database.
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::CreateDbL(RFs& aFs, const TDesC& aFilename)
    {
    TRACER("void CGlxtnVolumeDatabase::CreateDbL(RFs& aFs, const TDesC& aFilename)");
    // Create database, overwriting any existing file
    TInt err = aFs.MkDirAll(aFilename);
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError(err);
        }
    iStore = CPermanentFileStore::ReplaceL(aFs, aFilename,
                                            EFileRead | EFileWrite);
                                            
    iStore->SetTypeL(KPermanentFileStoreLayoutUid);
    iStore->SetRootL(iDatabase.CreateL(iStore));
    iStore->CommitL();
    
    
    User::LeaveIfError(iDatabase.Execute(KGlxCreateTableIds));
    User::LeaveIfError(iDatabase.Execute(KGlxCreateTableItems));
    User::LeaveIfError(iDatabase.Execute(KGlxCreateTableThumbnails));

    User::LeaveIfError(iDatabase.Execute(KGlxCreateIndexIds));
    User::LeaveIfError(iDatabase.Execute(KGlxCreateIndexThumbIds));
    User::LeaveIfError(iDatabase.Execute(KGlxCreateIndexItems));
    User::LeaveIfError(iDatabase.Execute(KGlxCreateIndexThumbnails));
    }

// -----------------------------------------------------------------------------
// DoAddItemL
// -----------------------------------------------------------------------------
//
TGlxtnThumbnailId CGlxtnVolumeDatabase::DoAddItemL()
    {
    TRACER("TGlxtnThumbnailId CGlxtnVolumeDatabase::DoAddItemL()");
    __ASSERT_ALWAYS(iInfo, Panic(EGlxPanicNullPointer));
    RDbTable table;
    CleanupClosePushL(table);
    User::LeaveIfError(table.Open(iDatabase, KGlxTableItems, RDbRowSet::EInsertOnly));
    TGlxtnThumbnailId thumbId( iNextThumbId );
    GLX_LOG_INFO1("TGlxtnThumbnailId CGlxtnVolumeDatabase::DoAddItemL() thumbId =%d", thumbId.Value());

    table.InsertL();
    table.SetColL( KGlxColItemUri, iInfo->FilePath() );
    table.SetColL( KGlxColItemId, thumbId.Value() );
    table.SetColL( KGlxColItemFileSize, iInfo->iFileSize );
    table.SetColL( KGlxColItemModTime, iInfo->iFileTime );
    table.PutL();

    CleanupStack::PopAndDestroy(&table);
    return thumbId;
    }

// -----------------------------------------------------------------------------
// EvaluateQueryL
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::EvaluateQueryL( const TDbQuery &aQuery )
    {
    TRACER("void CGlxtnVolumeDatabase::EvaluateQueryL( const TDbQuery &aQuery )");
    User::LeaveIfError( iView.Prepare(
                                iDatabase, aQuery, RDbRowSet::EReadOnly ) );
    iView.Evaluate( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// UpdateDataL
// -----------------------------------------------------------------------------
//
void CGlxtnVolumeDatabase::UpdateDataL( const TDesC& aSql )
    {
    TRACER("void CGlxtnVolumeDatabase::UpdateDataL( const TDesC& aSql )");
    TInt result = iDbUpdater.Execute( iDatabase, aSql );

    if ( result < KErrNone )
        {
        iDbUpdater.Close();
        User::Leave( result );
        }

    // According to documentation a result of 0 should indicate complete
    // but this does not seem to be the case
    iDbUpdater.Next( iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// QuoteSqlStringLC
// -----------------------------------------------------------------------------
//
HBufC* CGlxtnVolumeDatabase::QuoteSqlStringLC(const TDesC& aText)
    {
    TRACER("HBufC* CGlxtnVolumeDatabase::QuoteSqlStringLC(const TDesC& aText)");
    const TText quote('\'');
    TInt length = aText.Length() + 2;

    for ( TInt i = 0; i < aText.Length(); i++ )
        {
        if ( quote == aText[i] )
            {
            length++;
            }
        }

    HBufC* text = HBufC::NewLC(length);
    TPtr ptr(text->Des());

    ptr.Append(quote);
    for ( TInt i = 0; i < aText.Length(); i++ )
        {
        TText chr = aText[i];
        ptr.Append(chr);
        if ( quote == chr )
            {
            ptr.Append(quote);
            }
        }
    ptr.Append(quote);

    return text;
    }

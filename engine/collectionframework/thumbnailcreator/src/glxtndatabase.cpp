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
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

// INCLUDE FILES

#include "glxtndatabase.h"

#include <glxassert.h>
#include <glxtracer.h>
#include <pathinfo.h>

#include "glxtnfileinfo.h"
#include "glxtnvolumedatabase.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlxtnThumbnailDatabase::CGlxtnThumbnailDatabase
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGlxtnThumbnailDatabase::CGlxtnThumbnailDatabase()
    {
    TRACER("CGlxtnThumbnailDatabase::CGlxtnThumbnailDatabase()");
    iInternalDrive = PathInfo::PhoneMemoryRootPath().Left( KMaxDriveName );
    }

// -----------------------------------------------------------------------------
// CGlxtnThumbnailDatabase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::ConstructL(const TDesC& aDbFilename,
                             MGlxtnThumbnailStorageObserver* aStorageObserver)
    {
    TRACER("void CGlxtnThumbnailDatabase::ConstructL()");
    User::LeaveIfError(iFs.Connect());

    User::LeaveIfError(iFs.PrivatePath(iDatabasePath));
    iDatabasePath.Append(aDbFilename);
    iStorageObserver = aStorageObserver;
    }

// -----------------------------------------------------------------------------
// CGlxtnThumbnailDatabase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxtnThumbnailDatabase* CGlxtnThumbnailDatabase::NewL(
                             const TDesC& aDbFilename,
                             MGlxtnThumbnailStorageObserver* aStorageObserver)
    {
    TRACER("CGlxtnThumbnailDatabase* CGlxtnThumbnailDatabase::NewL()");
    CGlxtnThumbnailDatabase* self = new (ELeave) CGlxtnThumbnailDatabase;
    
    CleanupStack::PushL(self);
    self->ConstructL(aDbFilename, aStorageObserver);
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnThumbnailDatabase::~CGlxtnThumbnailDatabase()
    {
    TRACER("CGlxtnThumbnailDatabase::~CGlxtnThumbnailDatabase()");
    iDatabaseArray.ResetAndDestroy();
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// LoadThumbnailDataL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::LoadThumbnailDataL(HBufC8*& aData,
                        TGlxImageDataFormat& aFormat, const TGlxMediaId& aId,
                        const CGlxtnFileInfo& aFileInfo, const TSize& aSize,
                        TRequestStatus* aStatus)
    {
    TRACER("void CGlxtnThumbnailDatabase::LoadThumbnailDataL()");
    if ( iClientStatus )
        {
        User::Leave(KErrNotReady);
        }

    iCurrentOperation = ELoading;
    iLoadData = &aData;
    iLoadFormat = &aFormat;
    iMediaId = aId;
    iFileInfo = &aFileInfo;
    iSize = aSize;

    OpenDatabaseL( iInternalDrive )->GetThumbnailIdL( iMediaId );

    iClientStatus = aStatus;
    *iClientStatus = KRequestPending;
    }

// -----------------------------------------------------------------------------
// SaveThumbnailDataL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::SaveThumbnailDataL(const TDesC8& aData,
                        TGlxImageDataFormat aFormat, const TGlxMediaId& aId,
                        const CGlxtnFileInfo& aFileInfo, const TSize& aSize,
                        TRequestStatus* aStatus)
    {
    TRACER("void CGlxtnThumbnailDatabase::SaveThumbnailDataL()");
    if ( iClientStatus )
        {
        User::Leave(KErrNotReady);
        }

    iCurrentOperation = ESaving;
    iSaveData.Set(aData);
    ASSERT(iSaveData.Length() == aData.Length());
    iSaveFormat = aFormat;
    iMediaId = aId;
    iFileInfo = &aFileInfo;
    iSize = aSize;

    OpenDatabaseL( iInternalDrive )->GetThumbnailIdL( iMediaId );

    iClientStatus = aStatus;
    *iClientStatus = KRequestPending;
    }

// -----------------------------------------------------------------------------
// DeleteThumbnailsL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::DeleteThumbnailsL(const TGlxMediaId& aId,
                const CGlxtnFileInfo& aFileInfo, TRequestStatus* aStatus)
    {
    TRACER("void CGlxtnThumbnailDatabase::DeleteThumbnailsL()");
    if ( iClientStatus )
        {
        User::Leave(KErrNotReady);
        }
    iCurrentOperation = EDeleting;
    iMediaId = aId;
    iFileInfo = &aFileInfo;

    OpenDatabaseL( iInternalDrive )->GetThumbnailIdL( iMediaId );

    iClientStatus = aStatus;
    *iClientStatus = KRequestPending;
    }

// -----------------------------------------------------------------------------
// CleanupThumbnailsL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::CleanupThumbnailsL(TRequestStatus* aStatus)
    {
    TRACER("void CGlxtnThumbnailDatabase::CleanupThumbnailsL()");
    if ( iClientStatus )
        {
        User::Leave(KErrNotReady);
        }

    OpenDatabaseL( iInternalDrive );
    GLX_ASSERT_ALWAYS( iDatabaseArray.Count() > 0,
                    Panic( EGlxPanicLogicError ), "No databases to clean" );

    iDatabaseIndex = 0;
    iDatabaseArray[iDatabaseIndex]->CleanupDatabaseL();

    iClientStatus  = aStatus;
    *iClientStatus = KRequestPending;
    }

// -----------------------------------------------------------------------------
// IsThumbnailAvailableL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::IsThumbnailAvailableL(const TGlxMediaId& aId,
                        const CGlxtnFileInfo& aFileInfo,
                        const TSize& aSize, TRequestStatus* aStatus)
    {
    TRACER("void CGlxtnThumbnailDatabase::IsThumbnailAvailableL()");
    if ( iClientStatus )
        {
        User::Leave(KErrNotReady);
        }

    iCurrentOperation = ECheckingAvailable;
    iMediaId = aId;
    iFileInfo = &aFileInfo;
    iSize = aSize;

    OpenDatabaseL( iInternalDrive )->GetThumbnailIdL( iMediaId );

    iClientStatus = aStatus;
    *iClientStatus = KRequestPending;
    }

// -----------------------------------------------------------------------------
// StorageCancel
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::StorageCancel()
    {
    TRACER("void CGlxtnThumbnailDatabase::StorageCancel()");
    TInt count = iDatabaseArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iDatabaseArray[i]->Cancel();
        }

    if ( iClientStatus )
        {
        User::RequestComplete(iClientStatus, KErrCancel);
        }
    }

// -----------------------------------------------------------------------------
// NotifyBackgroundError
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::NotifyBackgroundError(
                                        const TGlxMediaId& aId, TInt aError )
    {
    TRACER("void CGlxtnThumbnailDatabase::NotifyBackgroundError()");
    if ( iStorageObserver )
        {
        iStorageObserver->BackgroundThumbnailError(aId, aError);
        }
    }

// -----------------------------------------------------------------------------
// HandleDatabaseError
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleDatabaseError(TInt aError)
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleDatabaseError()");
    __ASSERT_DEBUG(KErrNone != aError, Panic(EGlxPanicIllegalArgument));
    __ASSERT_DEBUG(iClientStatus, Panic(EGlxPanicNotInitialised));
    if ( iClientStatus )
        {
        User::RequestComplete(iClientStatus, aError);
        }
    }

// -----------------------------------------------------------------------------
// HandleThumbnailIdFromMediaIdL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleThumbnailIdFromMediaIdL(
                                        const TGlxtnThumbnailId& aThumbId )
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleThumbnailIdFromMediaIdL()");
    if ( aThumbId == KGlxIdNone )
        {
        // Not found in Ids table - check the Items table
        OpenDatabaseL(iFileInfo->FilePath())->GetThumbnailIdL(iFileInfo);
        }
    else
        {
        iThumbId = aThumbId;
        if(iCurrentOperation == EDeleting)
            {
            OpenDatabaseL( iInternalDrive )->DeleteIdL( iMediaId );
            }
        else
            {
            DoHandleThumbnailIdL();
            }
        }
    }

// -----------------------------------------------------------------------------
// HandleMediaIdDeletedL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleMediaIdDeletedL()
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleMediaIdDeletedL()");
    OpenDatabaseL(iFileInfo->FilePath())->DeleteThumbnailsL(iThumbId);
    }
    
// -----------------------------------------------------------------------------
// HandleThumbnailsDeletedL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleThumbnailsDeletedL()
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleThumbnailsDeletedL()");
    OpenDatabaseL(iFileInfo->FilePath())->DeleteItemL(iThumbId);
    }
    
// -----------------------------------------------------------------------------
// HandleItemsDeletedL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleItemDeletedL()
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleItemDeletedL()");
    __ASSERT_DEBUG(iClientStatus, Panic(EGlxPanicNotInitialised));
    if ( iClientStatus )
        {
        User::RequestComplete(iClientStatus, KErrNone);
        }
    }
    
// -----------------------------------------------------------------------------
// HandleThumbnailIdFromFilenameL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleThumbnailIdFromFilenameL(
                                        const TGlxtnThumbnailId& aThumbId )
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleThumbnailIdFromFilenameL()");
    iThumbId = aThumbId;
    if(iCurrentOperation == EDeleting)
        {
        GLX_LOG_INFO1("GlxtnThumbnailDatabase::HandleThumbnailIdFromFilenameL Current Operation Deleting. aThumbId = %d", aThumbId.Value());
        OpenDatabaseL(iFileInfo->FilePath())->DeleteThumbnailsL(iThumbId);
        }
    else
        {
        GLX_LOG_INFO1("GlxtnThumbnailDatabase::HandleThumbnailIdFromFilenameL Current Operation NOT Deleting. aThumbId = %d", aThumbId.Value());
        // Store thumbnail ID to speed up future lookups
        OpenDatabaseL( iInternalDrive )->StoreThumbnailIdL(
                                                        iMediaId, iThumbId );
        }
    }

// -----------------------------------------------------------------------------
// HandleThumbnailIdStoredL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleThumbnailIdStoredL()
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleThumbnailIdStoredL()");
    DoHandleThumbnailIdL();
    }

// -----------------------------------------------------------------------------
// HandleThumbnail
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleThumbnail(
                                TGlxImageDataFormat aFormat, HBufC8* aData )
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleThumbnail()");
    __ASSERT_DEBUG(iClientStatus, Panic(EGlxPanicNotInitialised));
    if ( iClientStatus )
        {
        *iLoadData = aData;
        *iLoadFormat = aFormat;
        User::RequestComplete(iClientStatus, KErrNone);
        }
    else
        {
        // Data loaded, but client doesn't want it (shouldn't ever happen)
        delete aData;
        }
    }

// -----------------------------------------------------------------------------
// HandleThumbnailStored
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleThumbnailStored()
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleThumbnailStored()");
    if ( iStorageObserver )
        {
        iStorageObserver->ThumbnailAvailable(iMediaId, iSize);
        }

    __ASSERT_DEBUG(iClientStatus, Panic(EGlxPanicNotInitialised));
    if ( iClientStatus )
        {
        User::RequestComplete(iClientStatus, KErrNone);
        }
    }

// -----------------------------------------------------------------------------
// HandleAvailabilityChecked
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleAvailabilityChecked(TInt aResult)
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleAvailabilityChecked()");
    __ASSERT_DEBUG(iClientStatus, Panic(EGlxPanicNotInitialised));
    if ( iClientStatus )
        {
        User::RequestComplete(iClientStatus, aResult);
        }
    }

// -----------------------------------------------------------------------------
// HandleDatabaseCleanedL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::HandleDatabaseCleanedL()
    {
    TRACER("void CGlxtnThumbnailDatabase::HandleDatabaseCleanedL()");

    iDatabaseIndex++;
    if(iDatabaseIndex < iDatabaseArray.Count())
        {
        // cleanup next database
        iDatabaseArray[iDatabaseIndex]->CleanupDatabaseL();
        }
    else
        {
        // Finished cleanup
        __ASSERT_DEBUG(iClientStatus, Panic(EGlxPanicNotInitialised));
        if ( iClientStatus )
            {
            User::RequestComplete(iClientStatus, KErrNone);
            }
        }
    }

// -----------------------------------------------------------------------------
// DoHandleThumbnailIdL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::DoHandleThumbnailIdL()
    {
    TRACER("void CGlxtnThumbnailDatabase::DoHandleThumbnailIdL()");
    CGlxtnVolumeDatabase* db = OpenDatabaseL(iFileInfo->FilePath());

    switch ( iCurrentOperation )
        {
        case ELoading:
            db->GetThumbnailL(iThumbId, iSize);
            break;
        case ESaving:
            db->StoreThumbnailL(iThumbId, iSize, iSaveFormat, iSaveData);
            break;
        case ECheckingAvailable:
            db->CheckAvailableL(iThumbId, iSize);
            break;
        default:
            Panic(EGlxPanicIllegalState);
            break;
        }
    }

// -----------------------------------------------------------------------------
// OpenDatabaseL
// -----------------------------------------------------------------------------
//
CGlxtnVolumeDatabase* CGlxtnThumbnailDatabase::OpenDatabaseL(const TDesC& aDrive)
    {
    TRACER("CGlxtnVolumeDatabase* CGlxtnThumbnailDatabase::OpenDatabaseL()");
    TPtrC drive(aDrive.Left(KMaxDriveName));

    TInt count = iDatabaseArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( 0 == drive.CompareF(iDatabaseArray[i]->Drive()) )
            {
            return iDatabaseArray[i];
            }
        }

    // Get path of DB on specified drive
    TFileName path(drive);
    path.Append(iDatabasePath);

    CGlxtnVolumeDatabase* database = CGlxtnVolumeDatabase::NewLC(*this, iFs, path);
    iDatabaseArray.AppendL(database);
    CleanupStack::Pop(database);

    return database;
    }


//  End of File  

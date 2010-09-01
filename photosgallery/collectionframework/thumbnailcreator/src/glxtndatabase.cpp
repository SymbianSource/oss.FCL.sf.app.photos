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
* Description:    Thumbnail storage implementation
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

// INCLUDE FILES

#include "glxtndatabase.h"

#include <glxassert.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <driveinfo.h>
#include <pathinfo.h>
#include <e32des16.h>
#include <e32std.h>
#include <bautils.h>
#include <centralrepository.h>

#include "glxtnfileinfo.h"
#include "glxtnvolumedatabase.h"

// ============================ CONSTANTS ===============================
_LIT(KDriveNameFormat, "%c:\\");

const TInt KLoggingDriveLetterLength = 8;
const TInt KGlxmediaSerialIdLength = 64;
const TUint32 KGlxTnMassStorageNotFreshlyFlashed    = 0x00000001 ;
const TUint32 KGlxTnSDCardVolumeId                  = 0x00000001 ;
const TUint32 KGlxTnHardDriveFlashState             = 0x00000002 ;

const TInt KRepositoryId    = 0x2000A09 ; 


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

		// Recover from database file corruption 
		// though the database file opening is successful
		if (aError == KErrCorruptThumbnailDatabase)
	        {
	        RecoverFromDatabaseError();
	        aError = KErrCorrupt;
	        }
		else if (aError == KErrEofThumbnailDatabase)
            {
            RecoverFromDatabaseError();
            aError = KErrEof;
            }
	
    if ( iClientStatus )
        {
        User::RequestComplete(iClientStatus, aError);
        }
    }

// -----------------------------------------------------------------------------
// RecoverFromDatabaseError
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::RecoverFromDatabaseError()
	{
    TRACER("void CGlxtnThumbnailDatabase::RecoverFromDatabaseError()");
    GLX_LOG_INFO("*** Database Corrupted ***");
    
    TFileName path(iFileInfo->FilePath().Left(KMaxDriveName));
    TInt count = iDatabaseArray.Count();
    for ( TInt i = 0; i < count; i++ )
	    {
	    if ( 0 == path.CompareF(iDatabaseArray[i]->Drive()) )
	        {
	        
	        CGlxtnVolumeDatabase* volDb = iDatabaseArray[i];
	        iDatabaseArray.Remove(i);
	        
	        delete volDb;
	        volDb = NULL;
	        
	        break;
            }
        }
		
    path.Append(iDatabasePath);
    DeleteFile(path);
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
        CGlxtnVolumeDatabase* db = OpenDatabaseL(iInternalDrive);
        iThumbId = db->GetThumbnailId();
        // Store thumbnail ID to speed up future lookups
        db->StoreThumbnailIdL(iMediaId, iThumbId);
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
       	GLX_LOG_INFO1("HandleAvailabilityChecked+ aResult= %d [0-Av / 1-NotAv]", aResult);
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

    // the following three lines of code make the assumption that (in the worst case) (atleast) 
    // some databases are not present in the iDatabaseArray. This would then prevent those 
    // from being cleaned, since only the databases contained in the iDatabaseArray will be cleaned.
    // this will in effect cause those databases to keep growing.
    
    // The OpenDatabaseL method also adds the database into iDatabaseArray if it 
    // already was not part of it. Here we are using this behaviour of the method
  
    OpenDatabaseL(DriveInfo::EDefaultPhoneMemory);
    OpenDatabaseL(DriveInfo::EDefaultMassStorage);
    OpenDatabaseL(DriveInfo::EDefaultRemovableMassStorage);
    
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
            db->StoreItemsL(iThumbId, iFileInfo);
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
    TRACER("CGlxtnVolumeDatabase* CGlxtnThumbnailDatabase::OpenDatabaseL(const TDesC& aDrive)");
    TBuf<KMaxFSNameLength> drive;
    drive.Append(aDrive.Left(KMaxDriveName));
    TDriveUnit driveNumber(aDrive);
    if (EDriveZ == driveNumber)
    	{
    	GLX_LOG_INFO("CGlxtnThumbnailDatabase::OpenDatabaseL EDriveZ == DriveNumber");
	    drive.FillZ(0);
	    drive.Append(iInternalDrive);
    	}

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
    
    // If the media(removable drive) has changed or the mass storage has been reflashed, 
    // then the existing thumbnail database (if any) needs to be regenerated as per the new images in the media. 
    // else there might be a mismatch between the existing images and their thumbnails. this is achieved by the \
    // following function
    DeleteDBIfStorageChangedL(path);

    CGlxtnVolumeDatabase* database = CGlxtnVolumeDatabase::NewLC(*this, iFs, path);
    GLX_LOG_INFO("New Volume database creation attempted. ");
    iDatabaseArray.AppendL(database);
    CleanupStack::Pop(database);

    // The thumbnail id is placed only in the IDs table of the thumbnail database in the internal phone memory, 
    // irrespective of which drive the images or their databses are placed in. 
    // And since the thumbnail id has to be unique across drives, the 'next' thumbnail Id to be used 
    // is initialized by incrementing the thumbnail id in the internal drive by one.  
    if (KErrNone == drive.CompareF(iInternalDrive))
        {
        database->InitializeThumbIdL();
        }
    return database;
    }
// -----------------------------------------------------------------------------
// DeleteFile
//
// Common function for deletion of files.
// This function removes any 'read only' protection that may be there on the file  
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::DeleteFile(const TDesC&  aPath)
    {
    GLX_LOG_ENTRY_EXIT("void CGlxtnThumbnailDatabase::DeleteFile()");
    
    if (BaflUtils::FileExists( iFs, aPath ))
        {
        GLX_LOG_INFO("void CGlxtnThumbnailDatabase::DeleteFile(): File Exists, Now check if it is read only");

        TUint fileAttributes = KEntryAttNormal;
        iFs.Att(aPath, fileAttributes ) ;
        
        if ( fileAttributes & KEntryAttReadOnly ) 
            {
            GLX_LOG_INFO("void CGlxtnThumbnailDatabase::DeleteFile(): File is Read-Only . Make it 'not Read-Only' ");
            iFs.SetAtt(aPath, 0 , KEntryAttReadOnly);
            }
        
        GLX_LOG_INFO("void CGlxtnThumbnailDatabase::DeleteFile(): File is (or is made) 'Not Read-Only'. Delete it");
        iFs.Delete(aPath);
            
        GLX_LOG_INFO("void CGlxtnThumbnailDatabase::DeleteFile(): File Deleted");
        }
    }


// -----------------------------------------------------------------------------
// DeleteDBIfStorageChanged
//
// This case is useful if the device hard drive was reflashed after the last access.
// or the Memory card has changed after last access to the Memorycard.
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailDatabase::DeleteDBIfStorageChangedL(TFileName aPath)
    {
    TRACER("CGlxtnThumbnailDatabase::DeleteDBIfStorageChangedL");

    TUint   driveStatus  = 0 ;
    TPtrC   drive(aPath.Left(KMaxDriveName));
    TDriveUnit driveNumber(drive);
    TDriveUnit romDrive(EDriveZ);

    // this will make sure that we are not dealing with the intenral phone memory.
    if ((0 != drive.CompareF(PathInfo::PhoneMemoryRootPath().Left( KMaxDriveName ))) && 
    	(0 != drive.CompareF(romDrive.Name())))
        {
        GLX_LOG_INFO("Not Phone Memory / ROM");

        CRepository * pCenRep = CRepository::NewLC(TUid::Uid( KRepositoryId ));

        // that leaves the memory card and the hard drive
        User::LeaveIfError(DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus ) );

        // take care of the removable drives now
        if (driveStatus & DriveInfo::EDriveRemovable)
            {
            GLX_LOG_INFO("CGlxtnThumbnailDatabase::DeleteDBIfStorageChangedL. DriveInfo::EDriveRemovable");

            // get the serial number of the drive.
            TBuf8<KGlxmediaSerialIdLength>  serialNumDriveBuf ;
            serialNumDriveBuf.FillZ(KGlxmediaSerialIdLength);
            iFs.GetMediaSerialNumber( serialNumDriveBuf, driveNumber ) ;

            // get the serial number from the Cen Rep.
            TBuf8<KGlxmediaSerialIdLength>  serialNumCenRepBuf ;
            serialNumCenRepBuf.FillZ(KGlxmediaSerialIdLength);
            pCenRep->Get(KGlxTnSDCardVolumeId, serialNumCenRepBuf) ;

            if (serialNumCenRepBuf.Compare(serialNumDriveBuf))
                {
                GLX_LOG_INFO("SerialNumCenRepBuf.Compare(serialNumDriveBuf) returned a difference between the drive ids");
                
                User::LeaveIfError(pCenRep->Set(KGlxTnSDCardVolumeId, serialNumDriveBuf)) ;

                // delete the database so that later when we try to open the database a
                // new database is created.
                DeleteFile(aPath);
                GLX_LOG_INFO("CGlxtnThumbnailDatabase::DeleteDBIfStorageChangedL(): Deleted file");
                }
            }
        else
            // a rough assumption here. if this is not an internal drive and if it is not a removable drive
            // then it must most probably be the hard drive.
            {
            GLX_LOG_INFO("CGlxtnThumbnailDatabase::DeleteDBIfStorageChangedL(): Mass Memory");
            TInt freshlyFlashed = 0 ; 
            pCenRep->Get(KGlxTnHardDriveFlashState , freshlyFlashed)  ;

            // The default value set in a freshly flased drive for this key ID is 0.
            // (theorotically anything other than int(1) can be used as a
            // value for the cenrep key ID KGlxTnHardDriveFlashState.)
            if ( KGlxTnMassStorageNotFreshlyFlashed != freshlyFlashed )
                {
                User::LeaveIfError(pCenRep->Set(KGlxTnHardDriveFlashState , TInt(KGlxTnMassStorageNotFreshlyFlashed)) );

                // delete the database so that later when we try to open the database a
                // new database is created.
                DeleteFile(aPath);
                GLX_LOG_INFO("CGlxtnThumbnailDatabase::DeleteDBIfStorageChangedL(): Deleted File");
                }
            }
        CleanupStack::PopAndDestroy(pCenRep);
        }
    }

// OpenDatabaseL
// -----------------------------------------------------------------------------
//
CGlxtnVolumeDatabase* CGlxtnThumbnailDatabase::OpenDatabaseL(const DriveInfo::TDefaultDrives& aDrive)
    {
    TRACER("void CGlxtnThumbnailDatabase::OpenDatabaseL(const DriveInfo::TDefaultDrives& aDrive)");
    GLX_LOG_ENTRY_EXIT("void CGlxtnThumbnailDatabase::OpenDatabaseL(const DriveInfo::TDefaultDrives& aDrive)");
    
    TBuf<KLoggingDriveLetterLength> drivePath;
	if ( KErrNone == DriveName(aDrive , drivePath))
        {
        return OpenDatabaseL(drivePath) ;
        }
    else
        {
        return NULL; 
        }
    }
// -----------------------------------------------------------------------------
// DriveName
// -----------------------------------------------------------------------------
//
TInt CGlxtnThumbnailDatabase::DriveName(const TInt& aDefaultDrive,  TDes& aDriveName)
    {
    TRACER("void CGlxtnThumbnailDatabase::DriveName()");
    GLX_LOG_ENTRY_EXIT("void CGlxtnThumbnailDatabase::DriveName()");
    GLX_LOG_INFO1("DriveName aDefaultDrive = %d", aDefaultDrive);
    TChar driveLetter;
    TInt error = DriveInfo::GetDefaultDrive(aDefaultDrive, driveLetter); 
    
    if ( KErrNotSupported != error )
        {
        TUint   driveStatus  = 0 ;
        TDriveUnit driveNumber(aDefaultDrive);
        error = DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus );
        GLX_LOG_INFO1("GetDriveStatus returns = %d", error);
        if (error == KErrNone && ( driveStatus & DriveInfo::EDrivePresent ))
            {
            aDriveName.Format(KDriveNameFormat, TUint(driveLetter));
    		GLX_LOG_INFO1("aDriveName=%S", &aDriveName);
            }
        else
            {
            error = KErrNotReady;
            }
        }
    GLX_LOG_INFO1("DriveName returns = %d", error);
    return error;
    }
//  End of File  

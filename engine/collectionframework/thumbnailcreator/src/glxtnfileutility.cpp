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
* Description:   Utility for thumbnail tasks handling files
*
*/



/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnfileutility.h"
#include "glxscreenresolutions.h"
#include <glxtracer.h>
#include <glxpanic.h>
#include <glxthumbnailinfo.h>
#include <pathinfo.h>
#include <s32file.h>

_LIT(KGlxBadFileList, "glxbadfilelist.dat");
_LIT(KGlxBadFileMarker, "glxbadfilemarker.dat");

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnFileUtility::CGlxtnFileUtility()
    {
    TRACER("CGlxtnFileUtility::CGlxtnFileUtility()");
    }

// -----------------------------------------------------------------------------
// ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxtnFileUtility::ConstructL()
    {
    TRACER("CGlxtnFileUtility::ConstructL()");
	User::LeaveIfError(iFs.Connect());

    iFs.PrivatePath(iBadFileDir);
    iBadFileDir.Insert(
                0, PathInfo::PhoneMemoryRootPath().Left( KMaxDriveName ) );

    TRAP_IGNORE(ReadBadFileListL());

    iPersistentSizeClasses.AppendL(
                TSize(KGlxThumbnailSmallWidth, KGlxThumbnailSmallHeight));
    iPersistentSizeClasses.AppendL(
                TSize(KGlxThumbnailLargeWidth, KGlxThumbnailLargeHeight));
    iPersistentSizeClasses.AppendL(
                TSize(KGlxThumbnailPortraitWidth, KGlxThumbnailPortraitHeight));
    }

// -----------------------------------------------------------------------------
// NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlxtnFileUtility* CGlxtnFileUtility::NewL()
    {
    TRACER("CGlxtnFileUtility::NewL()");
    CGlxtnFileUtility* self = new (ELeave) CGlxtnFileUtility;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnFileUtility::~CGlxtnFileUtility()
    {
    TRACER("CGlxtnFileUtility::~CGlxtnFileUtility()");
	iFs.Close();
	iBadFileArray.ResetAndDestroy();
	iPersistentSizeClasses.Close();
    }

// -----------------------------------------------------------------------------
// FsSession
// Provide file server session for opening images from files.
// -----------------------------------------------------------------------------
//
RFs& CGlxtnFileUtility::FsSession()
    {
    TRACER("RFs& CGlxtnFileUtility::FsSession()");
    return iFs;
    }

// -----------------------------------------------------------------------------
// CheckBadFileListL
// Test whether a file is on the bad file list.  If not the bad file marker
// is set to the filename, so that if a panic occurs it will be added to the
// bad file list.
// -----------------------------------------------------------------------------
//
void CGlxtnFileUtility::CheckBadFileListL(const TDesC& aFilename)
    {
    TRACER("0CGlxtnFileUtility::CheckBadFileListL(const TDesC& aFilename)");
    for ( TInt i = 0; i < iBadFileArray.Count(); i++ )
        {
        if ( 0 == iBadFileArray[i]->CompareF(aFilename) )
            {
            // File is bad
            User::Leave( KErrCorrupt );
            }
        }

    // Set bad file marker
    TPath path(iBadFileDir);
    path.Append(KGlxBadFileMarker);

    // Ensure directory exists
    TInt err = iFs.MkDirAll(path);
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError(err);
        }

    RFileWriteStream stream;
    CleanupClosePushL(stream);
    User::LeaveIfError(stream.Replace(
                                iFs, path, EFileWrite | EFileShareExclusive));
    stream << aFilename;
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);
    }

// -----------------------------------------------------------------------------
// ClearBadFileMarker
// Clear the bad file marker.  Called when processing a file is complete (no
// panic occurred).
// -----------------------------------------------------------------------------
//
void CGlxtnFileUtility::ClearBadFileMarker()
    {
    TRACER("void CGlxtnFileUtility::ClearBadFileMarker()");
    TPath path(iBadFileDir);
    path.Append(KGlxBadFileMarker);

    ( void )iFs.Delete( path );  // Ignore error
    }

// -----------------------------------------------------------------------------
// IsPersistentSize
// Test whether a generated thumbnail should be stored in persistent storage.
// -----------------------------------------------------------------------------
//
TBool CGlxtnFileUtility::IsPersistentSize(const TSize& aSize)
    {
    TRACER("CGlxtnFileUtility::IsPersistentSize()");
    for ( TInt i = 0; i < iPersistentSizeClasses.Count(); i++ )
        {
        if ( iPersistentSizeClasses[i] == aSize )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// ReadBadFileListL
// -----------------------------------------------------------------------------
//
void CGlxtnFileUtility::ReadBadFileListL()
    {
    TRACER("void CGlxtnFileUtility::ReadBadFileListL()");
    TPath pathList(iBadFileDir);
    pathList.Append(KGlxBadFileList);
    TPath pathMarker(iBadFileDir);
    pathMarker.Append(KGlxBadFileMarker);

    RFileReadStream stream;
    CleanupClosePushL(stream);

    // Check bad file list
    if ( KErrNone == stream.Open(
                            iFs, pathList, EFileRead | EFileShareReadersOnly) )
        {
        TInt err;
        // Loop until end of file
        do
            {
            TRAP(err,
                HBufC* file = HBufC::NewLC(stream, KMaxTInt);
                iBadFileArray.AppendL(file);
                CleanupStack::Pop(file);
                )
            } while ( KErrNone == err );

        stream.Close();
        // NOTE: We always get KErrEof even if the file is corrupted
        }

    // Check bad file marker
    if ( KErrNone == stream.Open(
                        iFs, pathMarker, EFileRead | EFileShareReadersOnly) )
        {
        // File exists, file in marker must be bad
        HBufC* file = HBufC::NewLC(stream, KMaxTInt);
        iBadFileArray.AppendL(file);
        CleanupStack::Pop(file);

        // Save the list for next gallery session
        // Recreate whole file in case existing file is corrupted
        WriteBadFileListL();

        stream.Close();
        ClearBadFileMarker();
        }

    CleanupStack::Pop(&stream); // Stream already closed
    }

// -----------------------------------------------------------------------------
// WriteBadFileListL
// -----------------------------------------------------------------------------
//
void CGlxtnFileUtility::WriteBadFileListL()
    {
    TRACER("void CGlxtnFileUtility::WriteBadFileListL()");
    TPath path(iBadFileDir);
    path.Append(KGlxBadFileList);

    RFileWriteStream stream;
    CleanupClosePushL(stream);
    User::LeaveIfError(stream.Replace(
                        iFs, path, EFileWrite | EFileShareExclusive));

    for ( TInt i = 0; i < iBadFileArray.Count() ; i++ )
        {
        stream << *iBadFileArray[i];
        }
    stream.CommitL();

    CleanupStack::PopAndDestroy(&stream);
    }

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
* Description:   Thumbnail save task implementation
*
*/



/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

// INCLUDE FILES

#include "glxtnsavethumbnailtask.h"

#include <glxassert.h>
#include <glxtracer.h>
#include <glxthumbnail.h>
#include <imageconversion.h>
#include <s32mem.h>

#include "glxtnfileinfo.h"
#include "mglxtnstorage.h"

// Minimum size of thumbnail to be saved as JPEG rather than bitmap
const TInt KGlxMinPixelsForJpeg = 30000;

const TInt KGlxBitmapBufferGranularity = 1024;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlxtnSaveThumbnailTask::CGlxtnSaveThumbnailTask
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGlxtnSaveThumbnailTask::CGlxtnSaveThumbnailTask(const TGlxMediaId& aItemId,
                    const TSize& aSize, MGlxtnThumbnailStorage* aStorage) :
    CGlxtnTask(KGlxtnTaskIdSaveThumbnail, aItemId, aStorage), iSize(aSize)
    {
    TRACER("CGlxtnSaveThumbnailTask::CGlxtnSaveThumbnailTask()");
    GLX_ASSERT_ALWAYS( aStorage, Panic( EGlxPanicIllegalArgument ),
                        "CGlxtnSaveThumbnailTask created without storage" );

    // Save task is always low priority
    SetPriority(EPriorityLow);
    }

// -----------------------------------------------------------------------------
// CGlxtnSaveThumbnailTask::CGlxtnSaveThumbnailTask
// Constructor for derived classes.
// -----------------------------------------------------------------------------
//
CGlxtnSaveThumbnailTask::CGlxtnSaveThumbnailTask(const TGlxtnTaskId& aId,
                const TGlxMediaId& aItemId, MGlxtnThumbnailStorage* aStorage) :
    CGlxtnTask(aId, aItemId, aStorage)
    {
    TRACER("CGlxtnSaveThumbnailTask::CGlxtnSaveThumbnailTask()");
    GLX_ASSERT_ALWAYS( aStorage, Panic( EGlxPanicIllegalArgument ),
                        "CGlxtnSaveThumbnailTask created without storage" );
    }

// -----------------------------------------------------------------------------
// CGlxtnSaveThumbnailTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxtnSaveThumbnailTask::ConstructL(CGlxtnFileInfo* aFileInfo,
                                            CFbsBitmap* aThumbnail)
    {
    TRACER("void CGlxtnSaveThumbnailTask::ConstructL()");
    // Shouldn't create save task if client doesn't use storage
    if ( !(Storage() && aFileInfo && aThumbnail) )
        {
        User::Leave(KErrArgument);
        }

    // Must not leave after taking ownership of these
    iFileInfo = aFileInfo;
    iThumbnail = aThumbnail;
    }

// -----------------------------------------------------------------------------
// CGlxtnSaveThumbnailTask::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlxtnSaveThumbnailTask* CGlxtnSaveThumbnailTask::NewL(
                    const TGlxMediaId& aItemId,
                    CGlxtnFileInfo* aFileInfo, const TSize& aSize,
                    CFbsBitmap* aThumbnail, MGlxtnThumbnailStorage* aStorage)
    {
    TRACER("CGlxtnSaveThumbnailTask* CGlxtnSaveThumbnailTask::NewL()");
    CGlxtnSaveThumbnailTask* self = new (ELeave) CGlxtnSaveThumbnailTask(
                                                    aItemId, aSize, aStorage);
    CleanupStack::PushL(self);
    self->ConstructL(aFileInfo, aThumbnail);
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CGlxtnSaveThumbnailTask::~CGlxtnSaveThumbnailTask()
    {
    TRACER("Destructor CGlxtnSaveThumbnailTask::~CGlxtnSaveThumbnailTask()");
    delete iEncoder;
    delete iThumbnail;
    delete iData;
    delete iFileInfo;
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnSaveThumbnailTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnSaveThumbnailTask::DoStartL()");
    EncodeThumbnailL(aStatus);

    return ETrue;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnSaveThumbnailTask::DoCancel()
    {
    TRACER("CGlxtnSaveThumbnailTask::DoCancel()");
    if ( iEncoder )
        {
        iEncoder->Cancel();
        }

    Storage()->StorageCancel();
    }

// -----------------------------------------------------------------------------
// DoRunL
// -----------------------------------------------------------------------------
//
TBool CGlxtnSaveThumbnailTask::DoRunL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnSaveThumbnailTask::DoRunL()");
    if ( KErrNone == aStatus.Int() && EStateEncoding == iState )
        {
        SaveThumbnailL(aStatus);
        return ETrue;
        }

    return EFalse;  // Task is complete
    }

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnSaveThumbnailTask::DoRunError(TInt /*aError*/)
    {
    TRACER("CGlxtnSaveThumbnailTask::DoRunError()");
    delete iEncoder;
    iEncoder = NULL;

    return EFalse;  // Task is complete
    }

// -----------------------------------------------------------------------------
// EncodeThumbnailL
// -----------------------------------------------------------------------------
//
void CGlxtnSaveThumbnailTask::EncodeThumbnailL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnSaveThumbnailTask::EncodeThumbnailL()");
    TSize size(iThumbnail->SizeInPixels());
    User::LeaveIfError(iThumbnail->SetDisplayMode(KGlxThumbnailDisplayMode));

    delete iData;
    iData = NULL;

    if ( KGlxMinPixelsForJpeg <= size.iWidth * size.iHeight )
        {
        // Encode as JPEG
        iFormat = EGlxIDF_JPEG;
        delete iEncoder;
        iEncoder = NULL;
        iEncoder = CImageEncoder::DataNewL(iData, CImageEncoder::EOptionNone,
                                            KImageTypeJPGUid);
        iState = EStateEncoding;
        iEncoder->Convert(&aStatus, *iThumbnail);
        }
    else
        {
        // Externalise bitmap into buffer
        iFormat = EGlxIDF_Bitmap;
        CBufBase* buffer = CBufSeg::NewL(KGlxBitmapBufferGranularity);
        CleanupStack::PushL(buffer);

        RBufWriteStream stream(*buffer);
        CleanupClosePushL(stream);
        stream << *iThumbnail;
        stream.CommitL();
        CleanupStack::PopAndDestroy(&stream);

        TInt length = buffer->Size();
        iData = HBufC8::NewL(length);
        TPtr8 ptr(iData->Des());
        buffer->Read(0, ptr, length);

        CleanupStack::PopAndDestroy(buffer);

        SaveThumbnailL(aStatus);
        }
    }

// -----------------------------------------------------------------------------
// SaveThumbnailL
// -----------------------------------------------------------------------------
//
void CGlxtnSaveThumbnailTask::SaveThumbnailL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnSaveThumbnailTask::SaveThumbnailL()");
    delete iEncoder;
    iEncoder = NULL;

    iState = EStateSaving;
    Storage()->SaveThumbnailDataL(*iData, iFormat, ItemId(),
                                    *iFileInfo, iSize, &aStatus);
    }

//  End of File  

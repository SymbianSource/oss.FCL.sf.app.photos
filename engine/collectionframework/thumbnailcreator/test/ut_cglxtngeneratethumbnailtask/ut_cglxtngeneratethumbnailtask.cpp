/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Thumbnail generation task unit tests.
*
*/



//  CLASS HEADER
#include "UT_CGlxtnGenerateThumbnailTask.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <fbs.h>
#include <Ecom\ECom.h>

//  INTERNAL INCLUDES
#include "glxtngeneratethumbnailtask.h"
#include "glxtnsavethumbnailtask.h"
#include "dummytaskmanager.h"
#include <glxtnfileinfo.h>
#include <glxthumbnail.h>
#include "glxtnfileutility.h"
#include "glxtnthumbnailrequest.h"

EUNIT_DECLARE_PRINTABLE_AS_TINT( TGlxThumbnailQuality )

const TUint KTestMediaId = 1;
const TInt KTestWidth = 320;
const TInt KTestHeight = 240;
const TDisplayMode KTestThumbDisplayMode = KGlxThumbnailDisplayMode;

_LIT(KTestImageUri, "C:\\TncTest\\test.jpg");
_LIT(KTallImageUri, "C:\\TncTest\\tall.jpg");
_LIT(KWideImageUri, "C:\\TncTest\\wide.jpg");
_LIT(KCorruptImageUri, "C:\\TncTest\\corrupt.jpg");
_LIT(KMissingImageUri, "C:\\TncTest\\does_not_exist.jpg");

// -----------------------------------------------------------------------------
// CGlxtnSaveThumbnailTask::NewL
// Dummy implementation.
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxtnSaveThumbnailTask* CGlxtnSaveThumbnailTask::NewL(
                    const TGlxMediaId& /*aItemId*/,
                    CGlxtnFileInfo* /*aFileInfo*/, const TSize& /*aSize*/,
                    CFbsBitmap* /*aThumbnail*/, MGlxtnThumbnailStorage* /*aStorage*/)
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }

// ---------------------------------------------------------------------------

// CONSTRUCTION
UT_CGlxtnGenerateThumbnailTask* UT_CGlxtnGenerateThumbnailTask::NewL()
    {
    UT_CGlxtnGenerateThumbnailTask* self = UT_CGlxtnGenerateThumbnailTask::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxtnGenerateThumbnailTask* UT_CGlxtnGenerateThumbnailTask::NewLC()
    {
    UT_CGlxtnGenerateThumbnailTask* self = new( ELeave ) UT_CGlxtnGenerateThumbnailTask();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxtnGenerateThumbnailTask::~UT_CGlxtnGenerateThumbnailTask()
    {
    }

// Default constructor
UT_CGlxtnGenerateThumbnailTask::UT_CGlxtnGenerateThumbnailTask()
    {
    }

// Second phase construct
void UT_CGlxtnGenerateThumbnailTask::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    // Need to free ecom resources before running tests
    REComSession::FinalClose();
    }

//  METHODS

void UT_CGlxtnGenerateThumbnailTask::ThumbnailFetchComplete(
    const TGlxMediaId& aItemId, TGlxThumbnailQuality aQuality, TInt aErrorCode)
    {
	EUNIT_ASSERT_EQUALS( KTestMediaId, aItemId.Value() );
	iQuality = aQuality;
    iError = aErrorCode;
    }

void UT_CGlxtnGenerateThumbnailTask::ThumbnailDeletionComplete(
                                const TGlxMediaId& /*aItemId*/, TInt /*aErrorCode*/)
    {
    EUNIT_FAIL_TEST("ThumbnailDeletionComplete() called");
    }

void UT_CGlxtnGenerateThumbnailTask::FilterAvailableComplete(
                const RArray<TGlxMediaId>& /*aIdArray*/, TInt /*aErrorCode*/ )
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnGenerateThumbnailTask::FetchFileInfoL(CGlxtnFileInfo* aInfo,
                        const TGlxMediaId& aItemId, TRequestStatus* aStatus)
    {
    EUNIT_ASSERT( NULL != aInfo );
	EUNIT_ASSERT_EQUALS( KTestMediaId, aItemId.Value() );
    aInfo->SetFilePathL(iUri);

    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

void UT_CGlxtnGenerateThumbnailTask::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    }

MGlxtnThumbnailStorage* UT_CGlxtnGenerateThumbnailTask::ThumbnailStorage()
    {
    return NULL;
    }

void UT_CGlxtnGenerateThumbnailTask::SetupL()
    {
    iFileUtility = CGlxtnFileUtility::NewL();
    User::LeaveIfError(RFbsSession::Connect(iFileUtility->FsSession()));

    TGlxMediaId id(KTestMediaId);

    iSizeClass.SetSize(KTestWidth, KTestHeight);

    iBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Create(TSize(), KTestThumbDisplayMode));

    TGlxThumbnailRequest request(id, iSizeClass,
                                TGlxThumbnailRequest::EPrioritizeQuality,
                                iBitmap->Handle(), ETrue);
    iGenerateTask = CGlxtnGenerateThumbnailTask::NewL(request,
                                                    *iFileUtility, *this);
    }

void UT_CGlxtnGenerateThumbnailTask::Teardown()
    {
    delete iGenerateTask;
    iGenerateTask = NULL;
    delete iBitmap;
    iBitmap = NULL;
    if ( RFbsSession::GetSession() )
        {
        RFbsSession::Disconnect();
        }
    delete iFileUtility;
    REComSession::FinalClose();
    }

void UT_CGlxtnGenerateThumbnailTask::UT_CGlxtnGenerateThumbnailTask_ConstructL()
    {
    // Check the task was correctly constructed
    EUNIT_ASSERT( NULL != iGenerateTask );
    EUNIT_ASSERT_EQUALS( iGenerateTask->Id().Value(), KGlxtnTaskIdGenerateThumbnail );
    EUNIT_ASSERT_EQUALS( iGenerateTask->ItemId().Value(), KTestMediaId );
    }

void UT_CGlxtnGenerateThumbnailTask::UT_CGlxtnGenerateThumbnailTask_GenerateL()
    {
    DoGenerateL(KTestImageUri);
    }

void UT_CGlxtnGenerateThumbnailTask::UT_CGlxtnGenerateThumbnailTask_GenerateTallL()
    {
    DoGenerateL(KTallImageUri);

    // Preserving aspect ratio should give TN narrower but same height as size class
    EUNIT_ASSERT( iBitmap->SizeInPixels().iWidth < iSizeClass.iWidth );
    EUNIT_ASSERT_EQUALS( iBitmap->SizeInPixels().iHeight, iSizeClass.iHeight );
    }

void UT_CGlxtnGenerateThumbnailTask::UT_CGlxtnGenerateThumbnailTask_GenerateWideL()
    {
    DoGenerateL(KWideImageUri);

    // Preserving aspect ratio should give TN less tall but same width as size class
    EUNIT_ASSERT_EQUALS( iBitmap->SizeInPixels().iWidth, iSizeClass.iWidth );
    EUNIT_ASSERT( iBitmap->SizeInPixels().iHeight < iSizeClass.iHeight );
    }

void UT_CGlxtnGenerateThumbnailTask::UT_CGlxtnGenerateThumbnailTask_ErrorCorruptFileL()
    {
    EUNIT_ASSERT_LEAVE(DoGenerateL(KCorruptImageUri));
    }

void UT_CGlxtnGenerateThumbnailTask::UT_CGlxtnGenerateThumbnailTask_ErrorFileNotFoundL()
    {
    EUNIT_ASSERT_SPECIFIC_LEAVE(DoGenerateL(KMissingImageUri), KErrNotFound);
    }

void UT_CGlxtnGenerateThumbnailTask::DoGenerateL(const TDesC& aUri)
    {
    iUri.Set(aUri);

    CDummyTaskManager* tm = new (ELeave) CDummyTaskManager(iGenerateTask);
    CleanupStack::PushL(tm);
    tm->StartTaskL();
    User::LeaveIfError(iError);
    CleanupStack::PopAndDestroy(tm);

    EUNIT_ASSERT_EQUALS( iQuality, EGlxThumbnailQualityHigh );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxtnGenerateThumbnailTask,
    "Unit tests for CGlxtnGenerateThumbnailTask.",
    "UNIT" )

EUNIT_TEST(
    "Test construction of CGlxtnGenerateThumbnailTask",
    "CGlxtnGenerateThumbnailTask",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnGenerateThumbnailTask_ConstructL, Teardown)

EUNIT_TEST(
    "Generate a thumbnail",
    "CGlxtnGenerateThumbnailTask",
    "HandleThumbnailTaskCompleteL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnGenerateThumbnailTask_GenerateL, Teardown)

EUNIT_TEST(
    "Generate thumbnail of a tall image",
    "CGlxtnGenerateThumbnailTask",
    "HandleThumbnailTaskCompleteL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnGenerateThumbnailTask_GenerateTallL, Teardown)

EUNIT_TEST(
    "Generate thumbnail of a wide image",
    "CGlxtnGenerateThumbnailTask",
    "HandleThumbnailTaskCompleteL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnGenerateThumbnailTask_GenerateWideL, Teardown)

EUNIT_TEST(
    "Attempting to thumbnail corrupt image",
    "CGlxtnGenerateThumbnailTask",
    "DoStartL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnGenerateThumbnailTask_ErrorCorruptFileL, Teardown)

EUNIT_TEST(
    "Attempting to thumbnail missing file",
    "CGlxtnGenerateThumbnailTask",
    "DoStartL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnGenerateThumbnailTask_ErrorFileNotFoundL, Teardown)

EUNIT_ALLOC_TEST(
    "Test alloc failures",
    "CGlxtnGenerateThumbnailTask",
    "HandleThumbnailTaskCompleteL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnGenerateThumbnailTask_GenerateL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

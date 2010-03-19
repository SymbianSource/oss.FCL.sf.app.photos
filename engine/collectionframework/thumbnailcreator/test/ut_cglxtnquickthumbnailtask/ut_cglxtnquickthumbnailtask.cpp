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
* Description:   Quick thumbnail generation task unit tests.
*
*/



//  CLASS HEADER
#include "UT_CGlxtnQuickThumbnailTask.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <fbs.h>
#include <Ecom\ECom.h>

//  INTERNAL INCLUDES
#include "glxtnquickthumbnailtask.h"
#include "dummytaskmanager.h"
#include <glxtnfileinfo.h>
#include "glxtnfileutility.h"
#include "glxtnthumbnailrequest.h"
#include <glxthumbnail.h>

const TUint KTestMediaId = 1;
const TInt KTestWidth = 320;
const TInt KTestHeight = 240;
const TDisplayMode KTestThumbDisplayMode = KGlxThumbnailCreationMode;

_LIT(KTestImageUri, "C:\\TncTest\\tall.jpg");
_LIT(KNoExifImageUri, "C:\\TncTest\\no_exif.jpg");
_LIT(KCorruptImageUri, "C:\\TncTest\\corrupt.jpg");
_LIT(KMissingImageUri, "C:\\TncTest\\does_not_exist.jpg");

// CONSTRUCTION
UT_CGlxtnQuickThumbnailTask* UT_CGlxtnQuickThumbnailTask::NewL()
    {
    UT_CGlxtnQuickThumbnailTask* self = UT_CGlxtnQuickThumbnailTask::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxtnQuickThumbnailTask* UT_CGlxtnQuickThumbnailTask::NewLC()
    {
    UT_CGlxtnQuickThumbnailTask* self = new( ELeave ) UT_CGlxtnQuickThumbnailTask();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxtnQuickThumbnailTask::~UT_CGlxtnQuickThumbnailTask()
    {
    }

// Default constructor
UT_CGlxtnQuickThumbnailTask::UT_CGlxtnQuickThumbnailTask()
    {
    }

// Second phase construct
void UT_CGlxtnQuickThumbnailTask::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    // Need to free ecom resources before running tests
    REComSession::FinalClose();
    }

//  METHODS

void UT_CGlxtnQuickThumbnailTask::ThumbnailFetchComplete(
    const TGlxMediaId& aItemId, TGlxThumbnailQuality aQuality, TInt aErrorCode)
    {
	EUNIT_ASSERT_EQUALS( KTestMediaId, aItemId.Value() );
	iQuality = aQuality;
    iError = aErrorCode;
    }

void UT_CGlxtnQuickThumbnailTask::ThumbnailDeletionComplete(
                                const TGlxMediaId& /*aItemId*/, TInt /*aErrorCode*/)
    {
    EUNIT_FAIL_TEST("ThumbnailDeletionComplete() called");
    }

void UT_CGlxtnQuickThumbnailTask::FilterAvailableComplete(
                const RArray<TGlxMediaId>& /*aIdArray*/, TInt /*aErrorCode*/ )
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnQuickThumbnailTask::FetchFileInfoL(CGlxtnFileInfo* aInfo,
                        const TGlxMediaId& aItemId, TRequestStatus* aStatus)
    {
    EUNIT_ASSERT( NULL != aInfo );
	EUNIT_ASSERT_EQUALS( KTestMediaId, aItemId.Value() );
    aInfo->SetFilePathL(iUri);

    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

void UT_CGlxtnQuickThumbnailTask::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    }

MGlxtnThumbnailStorage* UT_CGlxtnQuickThumbnailTask::ThumbnailStorage()
    {
    return NULL;
    }

void UT_CGlxtnQuickThumbnailTask::SetupL()
    {
    iFileUtility = CGlxtnFileUtility::NewL();
    User::LeaveIfError(RFbsSession::Connect(iFileUtility->FsSession()));

    TGlxMediaId id(KTestMediaId);

    iSizeClass.SetSize(KTestWidth, KTestHeight);
    iBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Create(TSize(), KTestThumbDisplayMode));

    TGlxThumbnailRequest request(id, iSizeClass,
                                TGlxThumbnailRequest::EPrioritizeSpeed,
                                iBitmap->Handle(), ETrue);
    iQuickThumbnailTask = CGlxtnQuickThumbnailTask::NewL(request,
                                                        *iFileUtility, *this);
    }

void UT_CGlxtnQuickThumbnailTask::Teardown()
    {
    delete iQuickThumbnailTask;
    iQuickThumbnailTask = NULL;
    delete iBitmap;
    iBitmap = NULL;
    if ( RFbsSession::GetSession() )
        {
        RFbsSession::Disconnect();
        }
    delete iFileUtility;
    REComSession::FinalClose();
    }

void UT_CGlxtnQuickThumbnailTask::UT_CGlxtnQuickThumbnailTask_ConstructL()
    {
    // Check the task was correctly constructed
    EUNIT_ASSERT( NULL != iQuickThumbnailTask );
    EUNIT_ASSERT_EQUALS( iQuickThumbnailTask->Id().Value(), KGlxtnTaskIdQuickThumbnail );
    EUNIT_ASSERT_EQUALS( iQuickThumbnailTask->ItemId().Value(), KTestMediaId );
    }

void UT_CGlxtnQuickThumbnailTask::UT_CGlxtnQuickThumbnailTask_GenerateL()
    {
    DoGenerateL(KTestImageUri);
    }

void UT_CGlxtnQuickThumbnailTask::UT_CGlxtnQuickThumbnailTask_GenerateNoExifL()
    {
    DoGenerateL(KNoExifImageUri);
    }

void UT_CGlxtnQuickThumbnailTask::UT_CGlxtnQuickThumbnailTask_ErrorCorruptFileL()
    {
    EUNIT_ASSERT_LEAVE(DoGenerateL(KCorruptImageUri));
    }

void UT_CGlxtnQuickThumbnailTask::UT_CGlxtnQuickThumbnailTask_ErrorFileNotFoundL()
    {
    EUNIT_ASSERT_SPECIFIC_LEAVE(DoGenerateL(KMissingImageUri), KErrNotFound);
    }

void UT_CGlxtnQuickThumbnailTask::DoGenerateL(const TDesC& aUri)
    {
    iUri.Set(aUri);

    CDummyTaskManager* tm = new (ELeave) CDummyTaskManager(iQuickThumbnailTask);
    CleanupStack::PushL(tm);
    tm->StartTaskL();
    User::LeaveIfError(iError);
    CleanupStack::PopAndDestroy(tm);

    EUNIT_ASSERT_EQUALS( iQuality, EGlxThumbnailQualityLow );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxtnQuickThumbnailTask,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CGlxtnQuickThumbnailTask",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnQuickThumbnailTask_ConstructL, Teardown)

EUNIT_TEST(
    "Generate a thumbnail",
    "CGlxtnQuickThumbnailTask",
    "HandleThumbnailTaskCompleteL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnQuickThumbnailTask_GenerateL, Teardown)

EUNIT_TEST(
    "Generate a thumbnail from image without EXIF data",
    "CGlxtnQuickThumbnailTask",
    "HandleThumbnailTaskCompleteL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnQuickThumbnailTask_GenerateNoExifL, Teardown)

EUNIT_TEST(
    "Attempting to thumbnail corrupt image",
    "CGlxtnQuickThumbnailTask",
    "DoStartL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnQuickThumbnailTask_ErrorCorruptFileL, Teardown)

EUNIT_TEST(
    "Attempting to thumbnail missing file",
    "CGlxtnQuickThumbnailTask",
    "DoStartL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnQuickThumbnailTask_ErrorFileNotFoundL, Teardown)

EUNIT_ALLOC_TEST(
    "Test alloc failures",
    "CGlxtnQuickThumbnailTask",
    "HandleThumbnailTaskCompleteL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnQuickThumbnailTask_GenerateL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

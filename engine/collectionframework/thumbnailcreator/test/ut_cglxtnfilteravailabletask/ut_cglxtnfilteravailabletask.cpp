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
* Description:   Thumbnail availability filter task unit tests.
*
*/



//  CLASS HEADER
#include "ut_cglxtnfilteravailabletask.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <fbs.h>
#include <Ecom\ECom.h>

//  INTERNAL INCLUDES
#include "glxtnfilteravailabletask.h"
#include "dummytaskmanager.h"

const TUint KGlxMediaIdWithThumbnail = 1;
const TUint KGlxMediaIdWithoutThumbnail = 2;
const TInt KTestSizeWidth = 320;
const TInt KTestSizeHeight = 240;

// ---------------------------------------------------------------------------

// CONSTRUCTION
UT_CGlxtnFilterAvailableTask* UT_CGlxtnFilterAvailableTask::NewL()
    {
    UT_CGlxtnFilterAvailableTask* self = UT_CGlxtnFilterAvailableTask::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxtnFilterAvailableTask* UT_CGlxtnFilterAvailableTask::NewLC()
    {
    UT_CGlxtnFilterAvailableTask* self = new( ELeave ) UT_CGlxtnFilterAvailableTask();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxtnFilterAvailableTask::~UT_CGlxtnFilterAvailableTask()
    {
    }

// Default constructor
UT_CGlxtnFilterAvailableTask::UT_CGlxtnFilterAvailableTask()
    {
    }

// Second phase construct
void UT_CGlxtnFilterAvailableTask::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CGlxtnFilterAvailableTask::ThumbnailFetchComplete(
    const TGlxMediaId& /*aItemId*/, TGlxThumbnailQuality /*aQuality*/, TInt /*aErrorCode*/)
    {
    EUNIT_FAIL_TEST("ThumbnailFetchComplete() called");
    }

void UT_CGlxtnFilterAvailableTask::ThumbnailDeletionComplete(
                                const TGlxMediaId& /*aItemId*/, TInt /*aErrorCode*/)
    {
    EUNIT_FAIL_TEST("ThumbnailDeletionComplete() called");
    }

void UT_CGlxtnFilterAvailableTask::FilterAvailableComplete(
                        const RArray<TGlxMediaId>& aIdArray, TInt aErrorCode )
    {
    EUNIT_ASSERT( KErrNotFound == aIdArray.Find(
                                    TGlxMediaId(KGlxMediaIdWithThumbnail)) );
    EUNIT_ASSERT( KErrNotFound != aIdArray.Find(
                                    TGlxMediaId(KGlxMediaIdWithoutThumbnail)) );
    iError = aErrorCode;
    }

void UT_CGlxtnFilterAvailableTask::FetchFileInfoL(CGlxtnFileInfo* /*aInfo*/,
                        const TGlxMediaId& /*aItemId*/, TRequestStatus* aStatus)
    {
    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

void UT_CGlxtnFilterAvailableTask::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    }

MGlxtnThumbnailStorage* UT_CGlxtnFilterAvailableTask::ThumbnailStorage()
    {
    return this;
    }

void UT_CGlxtnFilterAvailableTask::LoadThumbnailDataL(HBufC8*& /*aData*/, TGlxImageDataFormat& /*aFormat*/,
            const TGlxMediaId& /*aId*/, const CGlxtnFileInfo& /*aFileInfo*/,
            const TSize& /*aSize*/, TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnFilterAvailableTask::SaveThumbnailDataL(const TDesC8& /*aData*/,
            TGlxImageDataFormat /*aFormat*/, const TGlxMediaId& /*aId*/,
            const CGlxtnFileInfo& /*aFileInfo*/, const TSize& /*aSize*/,
            TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnFilterAvailableTask::DeleteThumbnailsL(const TGlxMediaId& /*aId*/,
            const CGlxtnFileInfo& /*aFileInfo*/, TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnFilterAvailableTask::CleanupThumbnailsL(TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnFilterAvailableTask::IsThumbnailAvailableL(const TGlxMediaId& aId,
            const CGlxtnFileInfo& /*aFileInfo*/,
            const TSize& aSize, TRequestStatus* aStatus)
    {
    TInt result = (KGlxMediaIdWithThumbnail == aId.Value())
                    ? KGlxThumbnailAvailable : KGlxThumbnailNotAvailable;
    EUNIT_ASSERT_EQUALS( aSize, iSize );
    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, result);
    }

void UT_CGlxtnFilterAvailableTask::StorageCancel()
    {
    }

void UT_CGlxtnFilterAvailableTask::NotifyBackgroundError(
            const TGlxMediaId& /*aId*/, TInt /*aError*/)
    {
    EUNIT_FAIL_TEST("NotifyBackgroundError() called");
    }

void UT_CGlxtnFilterAvailableTask::SetupL()
    {
    iSize.SetSize(KTestSizeWidth, KTestSizeHeight);

    RArray<TGlxMediaId> array;
    CleanupClosePushL(array);
    array.AppendL(TGlxMediaId(KGlxMediaIdWithThumbnail));
    array.AppendL(TGlxMediaId(KGlxMediaIdWithoutThumbnail));
    iFilterAvailableTask = CGlxtnFilterAvailableTask::NewL(array.Array(),
                                                            iSize, *this);
    CleanupStack::PopAndDestroy(&array);
    }

void UT_CGlxtnFilterAvailableTask::Teardown()
    {
    delete iFilterAvailableTask;
    }

void UT_CGlxtnFilterAvailableTask::UT_CGlxtnFilterAvailableTask_ConstructL()
    {
    // Check the task was correctly constructed
    EUNIT_ASSERT( NULL != iFilterAvailableTask );
    EUNIT_ASSERT_EQUALS( iFilterAvailableTask->Id().Value(), KGlxtnTaskIdFilterAvailable );
    EUNIT_ASSERT_EQUALS( iFilterAvailableTask->Storage(), this );
    }

void UT_CGlxtnFilterAvailableTask::UT_CGlxtnFilterAvailableTask_FilterL()
    {
    CDummyTaskManager* tm = new (ELeave) CDummyTaskManager(iFilterAvailableTask);
    CleanupStack::PushL(tm);
    tm->StartTaskL();
    User::LeaveIfError(iError);
    CleanupStack::PopAndDestroy(tm);
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxtnFilterAvailableTask,
    "Unit tests for CGlxtnFilterAvailableTask.",
    "UNIT" )

EUNIT_TEST(
    "Test construction of CGlxtnFilterAvailableTask",
    "CGlxtnFilterAvailableTask",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnFilterAvailableTask_ConstructL, Teardown)

EUNIT_TEST(
    "Test filtering of ID array",
    "CGlxtnFilterAvailableTask",
    "StartL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnFilterAvailableTask_FilterL, Teardown)

EUNIT_ALLOC_TEST(
    "Test alloc failures",
    "CGlxtnFilterAvailableTask",
    "StartL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnFilterAvailableTask_FilterL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

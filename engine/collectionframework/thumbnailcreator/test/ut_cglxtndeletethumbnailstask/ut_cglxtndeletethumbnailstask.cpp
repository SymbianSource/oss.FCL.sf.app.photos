/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Delete thumbnails task unit tests.
*
*/



//  CLASS HEADER
#include "ut_cglxtndeletethumbnailstask.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <fbs.h>
#include <Ecom\ECom.h>

//  INTERNAL INCLUDES
#include "glxtndeletethumbnailstask.h"
#include "dummytaskmanager.h"

const TUint KTestMediaId = 27;

// ---------------------------------------------------------------------------

// CONSTRUCTION
UT_CGlxtnDeleteThumbnailsTask* UT_CGlxtnDeleteThumbnailsTask::NewL()
    {
    UT_CGlxtnDeleteThumbnailsTask* self = UT_CGlxtnDeleteThumbnailsTask::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxtnDeleteThumbnailsTask* UT_CGlxtnDeleteThumbnailsTask::NewLC()
    {
    UT_CGlxtnDeleteThumbnailsTask* self = new( ELeave ) UT_CGlxtnDeleteThumbnailsTask();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxtnDeleteThumbnailsTask::~UT_CGlxtnDeleteThumbnailsTask()
    {
    }

// Default constructor
UT_CGlxtnDeleteThumbnailsTask::UT_CGlxtnDeleteThumbnailsTask()
    {
    }

// Second phase construct
void UT_CGlxtnDeleteThumbnailsTask::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_CGlxtnDeleteThumbnailsTask::ThumbnailFetchComplete(
    const TGlxMediaId& /*aItemId*/, TGlxThumbnailQuality /*aQuality*/, TInt /*aErrorCode*/)
    {
    EUNIT_FAIL_TEST("ThumbnailFetchComplete() called");
    }

void UT_CGlxtnDeleteThumbnailsTask::ThumbnailDeletionComplete(
                                const TGlxMediaId& aItemId, TInt aErrorCode)
    {
    EUNIT_ASSERT_EQUALS( iIdDeleted, aItemId.Value() );
    iError = aErrorCode;
    }

void UT_CGlxtnDeleteThumbnailsTask::FilterAvailableComplete(
                const RArray<TGlxMediaId>& /*aIdArray*/, TInt /*aErrorCode*/ )
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnDeleteThumbnailsTask::FetchFileInfoL(CGlxtnFileInfo* /*aInfo*/,
                        const TGlxMediaId& /*aItemId*/, TRequestStatus* aStatus)
    {
    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

void UT_CGlxtnDeleteThumbnailsTask::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    }

MGlxtnThumbnailStorage* UT_CGlxtnDeleteThumbnailsTask::ThumbnailStorage()
    {
    return this;
    }

void UT_CGlxtnDeleteThumbnailsTask::LoadThumbnailDataL(HBufC8*& /*aData*/, TGlxImageDataFormat& /*aFormat*/,
            const TGlxMediaId& /*aId*/, const CGlxtnFileInfo& /*aFileInfo*/,
            const TSize& /*aSize*/, TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("LoadThumbnailDataL() called");
    }

void UT_CGlxtnDeleteThumbnailsTask::SaveThumbnailDataL(const TDesC8& /*aData*/,
            TGlxImageDataFormat /*aFormat*/, const TGlxMediaId& /*aId*/,
            const CGlxtnFileInfo& /*aFileInfo*/, const TSize& /*aSize*/,
            TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("SaveThumbnailDataL() called");
    }

void UT_CGlxtnDeleteThumbnailsTask::DeleteThumbnailsL(const TGlxMediaId& aId,
            const CGlxtnFileInfo& /*aFileInfo*/, TRequestStatus* aStatus)
    {
    iIdDeleted = aId.Value();

    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

void UT_CGlxtnDeleteThumbnailsTask::CleanupThumbnailsL(TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("CleanupThumbnailsL() called");
    }

void UT_CGlxtnDeleteThumbnailsTask::IsThumbnailAvailableL(const TGlxMediaId& /*aId*/,
            const CGlxtnFileInfo& /*aFileInfo*/,
            const TSize& /*aSize*/, TRequestStatus* /*aStatus*/)
    {
    EUNIT_FAIL_TEST("IsThumbnailAvailableL() called");
    }

void UT_CGlxtnDeleteThumbnailsTask::StorageCancel()
    {
    }

void UT_CGlxtnDeleteThumbnailsTask::NotifyBackgroundError(
            const TGlxMediaId& /*aId*/, TInt /*aError*/)
    {
    EUNIT_FAIL_TEST("NotifyBackgroundError() called");
    }

void UT_CGlxtnDeleteThumbnailsTask::SetupL()
    {
    TGlxMediaId id(KTestMediaId);
    iDeleteThumbnailsTask = CGlxtnDeleteThumbnailsTask::NewL(id, *this);
    }

void UT_CGlxtnDeleteThumbnailsTask::Teardown()
    {
    delete iDeleteThumbnailsTask;
    }

void UT_CGlxtnDeleteThumbnailsTask::UT_CGlxtnDeleteThumbnailsTask_ConstructL()
    {
    // Check the task was correctly constructed
    EUNIT_ASSERT( NULL != iDeleteThumbnailsTask );
    EUNIT_ASSERT_EQUALS( iDeleteThumbnailsTask->Id().Value(), KGlxtnTaskIdDeleteThumbnails );
    EUNIT_ASSERT_EQUALS( iDeleteThumbnailsTask->Storage(), this );
    }

void UT_CGlxtnDeleteThumbnailsTask::UT_CGlxtnDeleteThumbnailsTask_DeleteL()
    {
    iIdDeleted = KGlxInvalidIdValue;

    CDummyTaskManager* tm = new (ELeave) CDummyTaskManager(iDeleteThumbnailsTask);
    CleanupStack::PushL(tm);
    tm->StartTaskL();
    User::LeaveIfError(iError);
    CleanupStack::PopAndDestroy(tm);

    EUNIT_ASSERT_EQUALS( iIdDeleted, KTestMediaId );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxtnDeleteThumbnailsTask,
    "Unit tests for CGlxtnDeleteThumbnailsTask.",
    "UNIT" )

EUNIT_TEST(
    "Test construction of CGlxtnDeleteThumbnailsTask",
    "CGlxtnDeleteThumbnailsTask",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnDeleteThumbnailsTask_ConstructL, Teardown)

EUNIT_TEST(
    "Test deleting thumbnails",
    "CGlxtnDeleteThumbnailsTask",
    "StartL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnDeleteThumbnailsTask_DeleteL, Teardown)

EUNIT_ALLOC_TEST(
    "Test alloc failures",
    "CGlxtnDeleteThumbnailsTask",
    "StartL",
    "ERRORHANDLING",
    SetupL, UT_CGlxtnDeleteThumbnailsTask_DeleteL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

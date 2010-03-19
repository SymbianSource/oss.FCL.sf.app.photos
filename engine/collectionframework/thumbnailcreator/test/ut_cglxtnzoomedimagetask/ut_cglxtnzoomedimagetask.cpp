/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Cropped thumbnail generation task unit tests.
*
*/



//  CLASS HEADER
#include "ut_cglxtnzoomedimagetask.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <fbs.h>
#include <Ecom\ECom.h>

//  INTERNAL INCLUDES
#include "glxtngeneratethumbnailtask.h"
#include "dummytaskmanager.h"
#include <glxtnfileinfo.h>
#include "glxtnfileutility.h"
#include "glxtnthumbnailrequest.h"
#include "glxtnzoomedimagetask.h"
#include <glxthumbnail.h>

EUNIT_DECLARE_PRINTABLE_AS_TINT( TGlxThumbnailQuality )

const TUint KTestMediaId = 1;
const TInt KTestWidth = 320;
const TInt KTestHeight = 240;
const TDisplayMode KTestThumbDisplayMode = KGlxThumbnailDisplayMode;

_LIT(KTestImageUri, "C:\\TncTest\\test.jpg");

// ---------------------------------------------------------------------------

// CONSTRUCTION
UT_CGlxtnZoomedImageTask* UT_CGlxtnZoomedImageTask::NewL()
    {
    UT_CGlxtnZoomedImageTask* self = UT_CGlxtnZoomedImageTask::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxtnZoomedImageTask* UT_CGlxtnZoomedImageTask::NewLC()
    {
    UT_CGlxtnZoomedImageTask* self = new( ELeave ) UT_CGlxtnZoomedImageTask();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxtnZoomedImageTask::~UT_CGlxtnZoomedImageTask()
    {
    }

// Default constructor
UT_CGlxtnZoomedImageTask::UT_CGlxtnZoomedImageTask()
    {
    }

// Second phase construct
void UT_CGlxtnZoomedImageTask::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    // Need to free ecom resources before running tests
    REComSession::FinalClose();
    }

//  METHODS

void UT_CGlxtnZoomedImageTask::ThumbnailFetchComplete(
    const TGlxMediaId& aItemId, TGlxThumbnailQuality aQuality, TInt aErrorCode)
    {
	EUNIT_ASSERT_EQUALS( KTestMediaId, aItemId.Value() );
	iQuality = aQuality;
    iError = aErrorCode;
    }

void UT_CGlxtnZoomedImageTask::ThumbnailDeletionComplete(
                                const TGlxMediaId& /*aItemId*/, TInt /*aErrorCode*/)
    {
    EUNIT_FAIL_TEST("ThumbnailDeletionComplete() called");
    }

void UT_CGlxtnZoomedImageTask::FilterAvailableComplete(
                const RArray<TGlxMediaId>& /*aIdArray*/, TInt /*aErrorCode*/ )
    {
    EUNIT_FAIL_TEST("FilterAvailableComplete() called");
    }

void UT_CGlxtnZoomedImageTask::FetchFileInfoL(CGlxtnFileInfo* aInfo,
                        const TGlxMediaId& aItemId, TRequestStatus* aStatus)
    {
    EUNIT_ASSERT( NULL != aInfo );
	EUNIT_ASSERT_EQUALS( KTestMediaId, aItemId.Value() );
    aInfo->SetFilePathL(iUri);

    *aStatus = KRequestPending;
    User::RequestComplete(aStatus, KErrNone);
    }

void UT_CGlxtnZoomedImageTask::CancelFetchUri(const TGlxMediaId& /*aItemId*/)
    {
    }

MGlxtnThumbnailStorage* UT_CGlxtnZoomedImageTask::ThumbnailStorage()
    {
    return NULL;
    }

void UT_CGlxtnZoomedImageTask::SetupL()
    {
    iFileUtility = CGlxtnFileUtility::NewL();
    User::LeaveIfError(RFbsSession::Connect(iFileUtility->FsSession()));

    TGlxMediaId id(KTestMediaId);
    TSize size(KTestWidth, KTestHeight);
    TRect croppingRect(50, 50, 150, 150);

    iBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Create(TSize(), KTestThumbDisplayMode));

    TGlxThumbnailRequest request(id, size,
                TGlxThumbnailRequest::EPrioritizeQuality, iBitmap->Handle(),
                ETrue, croppingRect, EGlxThumbnailFilterNone);
    iZoomedImageTask = CGlxtnZoomedImageTask::NewL(request,
                                                    *iFileUtility, *this);
    }

void UT_CGlxtnZoomedImageTask::Teardown()
    {
    delete iZoomedImageTask;
    iZoomedImageTask = NULL;
    delete iBitmap;
    iBitmap = NULL;
    if ( RFbsSession::GetSession() )
        {
        RFbsSession::Disconnect();
        }
    delete iFileUtility;
    REComSession::FinalClose();
    }

void UT_CGlxtnZoomedImageTask::UT_CGlxtnZoomedImageTask_ConstructL()
    {
    // Check the task was correctly constructed
    EUNIT_ASSERT( NULL != iZoomedImageTask );
    EUNIT_ASSERT_EQUALS( iZoomedImageTask->Id().Value(), KGlxtnTaskIdZoomedThumbnail );
    EUNIT_ASSERT_EQUALS( iZoomedImageTask->ItemId().Value(), KTestMediaId );
    }

void UT_CGlxtnZoomedImageTask::UT_CGlxtnZoomedImageTask_GenerateL()
    {
    // NOTE: This test currently fails on the emulator, as the extended JPEG
    // codec is not present in the environment
    DoGenerateL(KTestImageUri);
    }

void UT_CGlxtnZoomedImageTask::DoGenerateL(const TDesC& aUri)
    {
    iUri.Set(aUri);

    CDummyTaskManager* tm = new (ELeave) CDummyTaskManager(iZoomedImageTask);
    CleanupStack::PushL(tm);
    tm->StartTaskL();
    User::LeaveIfError(iError);
    CleanupStack::PopAndDestroy(tm);

    EUNIT_ASSERT_EQUALS( iQuality, EGlxThumbnailQualityHigh );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxtnZoomedImageTask,
    "Unit tests for CGlxtnZoomedImageTask.",
    "UNIT" )

EUNIT_TEST(
    "Test construction of CGlxtnZoomedImageTask",
    "CGlxtnZoomedImageTask",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnZoomedImageTask_ConstructL, Teardown)

EUNIT_TEST(
    "Generate a cropped thumbnail",
    "CGlxtnZoomedImageTask",
    "HandleThumbnailTaskCompleteL",
    "FUNCTIONALITY",
    SetupL, UT_CGlxtnZoomedImageTask_GenerateL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Thumbnail saver for CommandHandler UPnP unit test
*
*/




//  CLASS HEADER
#include "t_cglxthumbnailsaver.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
#include <fbs.h>
#include <bitdev.h>
#include <gdi.h>

//  INTERNAL INCLUDES


// CONSTRUCTION
t_cglxthumbnailsaver* t_cglxthumbnailsaver::NewL()
    {
    t_cglxthumbnailsaver* self = t_cglxthumbnailsaver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

t_cglxthumbnailsaver* t_cglxthumbnailsaver::NewLC()
    {
    t_cglxthumbnailsaver* self = new( ELeave ) t_cglxthumbnailsaver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
t_cglxthumbnailsaver::~t_cglxthumbnailsaver()
    {
    }

// Default constructor
t_cglxthumbnailsaver::t_cglxthumbnailsaver()
    {
    }

// Second phase construct
void t_cglxthumbnailsaver::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void t_cglxthumbnailsaver::SetupL(  )
    {
    EUNIT_PRINT(_L("Entering t_cglxthumbnailsaver::SetupL"));
    if(!RFbsSession::GetSession())  
            {
            EUNIT_PRINT(_L("t_cglxthumbnailsaver::GetSession"));
            User::LeaveIfError(RFbsSession::Connect());
            EUNIT_PRINT(_L("t_cglxthumbnailsaver::FbsSession:: Connected"));
            }
    iThumbnail = NULL;
    iThumbnailSaver = NULL;
    iThumbnailSaver = CGlxThumbnailSaver::NewL(this);
    
    EUNIT_PRINT(_L("Exiting t_cglxthumbnailsaver::SetupL"));
    }
    

void t_cglxthumbnailsaver::Teardown(  )
    {
    EUNIT_PRINT(_L("t_cglxthumbnailsaver::Teardown"));
  //  if (iData && iData->iFbsCreated) // just not to close session owned by ccoeenv / application
            RFbsSession::Disconnect();

    if(iThumbnail)
        {
        EUNIT_PRINT(_L("t_cglxthumbnailsaver::iThumbnail"));
        delete iThumbnail;
        iThumbnail = NULL;
        }
   if(iThumbnailSaver)
       {
       EUNIT_PRINT(_L("t_cglxthumbnailsaver::iThumbnailSaver"));
       delete iThumbnailSaver;
       iThumbnailSaver = NULL;
       }
   EUNIT_PRINT(_L("Exiting t_cglxthumbnailsaver::Teardown"));
    }
    

void t_cglxthumbnailsaver::T_TestL(  )
    {
    EUNIT_ASSERT_DESC( iThumbnailSaver, "ThumbnailSaver Creation Failed");
    }
    
void t_cglxthumbnailsaver::T_CreateVideoIconL()
    {
	EUNIT_PRINT(_L("Entering t_cglxthumbnailsaver::T_CreateVideoIconL"));
    iThumbnail = new (ELeave) CFbsBitmap;     
	EUNIT_PRINT(_L("T_CreateVideoIconL::iThumbnail instance created"));
    User::LeaveIfError(iThumbnail->Create(TSize(), EColor64K));
	EUNIT_PRINT(_L("T_CreateVideoIconL::iThumbnail create Succeeded"));
    
    iThumbnailSaver->CreateVideoIconL(iThumbnail);
    EUNIT_PRINT(_L("T_CreateVideoIconL::Aftr CreateVideoIconL"));
    CActiveScheduler::Start();
    EUNIT_PRINT(_L("Exiting t_cglxthumbnailsaver::T_CreateVideoIconL"));
    }
//Cancel the request
void t_cglxthumbnailsaver::T_CreateVideoIconL_Cancel(  )
    {    
    EUNIT_PRINT(_L("Entering t_cglxthumbnailsaver::T_CreateVideoIconL_Cancel"));
    iThumbnail = new (ELeave) CFbsBitmap;     
    EUNIT_PRINT(_L("T_CreateVideoIconL_Cancel::iThumbnail instance created"));
    User::LeaveIfError(iThumbnail->Create(TSize(), EColor64K));
    EUNIT_PRINT(_L("T_CreateVideoIconL_Cancel::iThumbnail create failed"));    
    
    iThumbnailSaver->CreateVideoIconL(iThumbnail);
    
    iThumbnailSaver->Cancel();
    
    // We can at least test that DoCancel cancels the outstanding asynchronous
    // request, and that Cancel behaves in an orderly way.
    
    EUNIT_ASSERT( iThumbnailSaver->iStatus == KErrCancel || iThumbnailSaver->iStatus == KErrNone );
    EUNIT_ASSERT( !iThumbnailSaver->IsActive() );
    EUNIT_PRINT(_L("Exiting t_cglxthumbnailsaver::T_CreateVideoIconL_Cancel"));
    } 

void t_cglxthumbnailsaver::T_CreateDefaultVideoIconL()
    {      
    EUNIT_PRINT(_L("Entering t_cglxthumbnailsaver::T_CreateDefaultVideoIconL"));
    }
    
//From MGlxThumbnailSaveComplete    
void t_cglxthumbnailsaver::HandleFileSaveCompleteL(const TDesC& /*aPath*/)    
    {
    
    CActiveScheduler::Stop();
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_cglxthumbnailsaver,
    "Thumbnail Saver",
    "UNIT" )

EUNIT_TEST(
    "Craetion of Thumbnial",
    "NewL",
    "CGlxThumbnailSaver",
    "UNIT",
    SetupL, T_TestL, Teardown)
    
/*EUNIT_TEST(
    "Video",
    "CreateVideoIconL",
    "CGlxThumbnailSaver",
    "UNIT",
    SetupL, T_CreateVideoIconL, Teardown)
    
EUNIT_TEST(
    "Video",
    "CreateVideoIconL_Cancel",
    "CGlxThumbnailSaver",
    "UNIT",
    SetupL, T_CreateVideoIconL_Cancel, Teardown)*/
    
EUNIT_TEST(
    "Video",
    "CreateDefaultVideoIconL",
    "CGlxThumbnailSaver",
    "UNIT",
    SetupL, T_CreateDefaultVideoIconL, Teardown)    
  

EUNIT_END_TEST_TABLE

//  END OF FILE

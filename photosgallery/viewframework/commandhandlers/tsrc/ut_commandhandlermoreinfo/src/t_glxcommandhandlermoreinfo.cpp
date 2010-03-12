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
* Description:  CGlxCommandHandlerMoreInfo unit test cases
*
*/




//  CLASS HEADER
#include "t_glxcommandhandlermoreinfo.h"
#include "glxcommandhandlermoreinfo.h"
#include "ut_glxdummymedialist.h"

#include <glxcommandhandlers.hrh>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>


CGlxCommandHandlerMoreInfoTester* CGlxCommandHandlerMoreInfoTester::NewL()
    {
    CGlxCommandHandlerMoreInfoTester* self = new(ELeave)CGlxCommandHandlerMoreInfoTester();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CGlxCommandHandlerMoreInfoTester::~CGlxCommandHandlerMoreInfoTester()
    {
    if ( iMoreInfoHandler )
        {
        iMoreInfoHandler->Deactivate();
        }
    
    iCommandHandlerList.ResetAndDestroy();
	iCommandHandlerList.Close();
	
	delete iMediaList;
    }

TBool CGlxCommandHandlerMoreInfoTester::ExecuteL(TInt aCommand)
    {
    return iCommandHandlerList[0]->ExecuteL( aCommand );
    }
	
void CGlxCommandHandlerMoreInfoTester::AddItemL(TInt aId, CGlxMedia* aMedia)
    {
    iMediaList->AppendL(aId, aMedia);
    }
	
	
MGlxMediaList& CGlxCommandHandlerMoreInfoTester::MediaList()
    {
    return *iMediaList;
    }


CGlxCommandHandlerMoreInfoTester::CGlxCommandHandlerMoreInfoTester()
    {
    
    }
    
void CGlxCommandHandlerMoreInfoTester::ConstructL()
    {
    EUNIT_PRINT(_L("CGlxCommandHandlerMoreInfoTester::ConstructL"));
    // create 'dummy' media list
    iMediaList = new (ELeave) _CGlxTestMediaList;
    
    // create our command handler
    iMoreInfoHandler = CGlxCommandHandlerMoreInfo::NewL( this, EFalse );
    
    iCommandHandlerList.AppendL( iMoreInfoHandler );
    
    iMoreInfoHandler->ActivateL(0);
    }


// CONSTRUCTION
t_glxcommandhandlermoreinfo* t_glxcommandhandlermoreinfo::NewL()
    {
    t_glxcommandhandlermoreinfo* self = t_glxcommandhandlermoreinfo::NewLC();
    CleanupStack::Pop();

    return self;
    }

t_glxcommandhandlermoreinfo* t_glxcommandhandlermoreinfo::NewLC()
    {
    t_glxcommandhandlermoreinfo* self = new( ELeave ) t_glxcommandhandlermoreinfo();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
t_glxcommandhandlermoreinfo::~t_glxcommandhandlermoreinfo()
    {
    }

// Default constructor
t_glxcommandhandlermoreinfo::t_glxcommandhandlermoreinfo()
    {
    }

// Second phase construct
void t_glxcommandhandlermoreinfo::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void t_glxcommandhandlermoreinfo::SetupL(  )
    {
    iTester = CGlxCommandHandlerMoreInfoTester::NewL();
    }
    

void t_glxcommandhandlermoreinfo::Teardown(  )
    {
    delete iTester;
	iTester = NULL;
    }
    

void t_glxcommandhandlermoreinfo::T_TestL(  )
    {
    
    }
    
void t_glxcommandhandlermoreinfo::T_TestExecutionL()
    {
    MGlxMediaList& mlist = iTester->MediaList();
    
    // try to exe on empty list
    TBool exe = iTester->ExecuteL(EGlxCmdDrmMoreInfoOnline);
    EUNIT_ASSERT_DESC( exe == EFalse, "No items present, option should be disabled");
    
    // create a couple of media items
    TGlxMediaId testId(12346);
    CGlxMedia* newMedia = new (ELeave) CGlxMedia(testId);
    CleanupStack::PushL(newMedia);
    

    iTester->AddItemL(123456, newMedia);
    
	CleanupStack::Pop(newMedia);
	
	TGlxMediaId testId2(12347);
    CGlxMedia* newMedia2 = new (ELeave) CGlxMedia(testId2);
    CleanupStack::PushL(newMedia2);
    
    newMedia2->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia2->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia2->SetTextValueL(KMPXMediaGeneralUri, _L("DRM has info"));
    
    iTester->AddItemL(123457, newMedia2);
    
    CleanupStack::Pop(newMedia2);
    
    mlist.SetFocusL(NGlxListDefs::EAbsolute, 0);
    
    exe = iTester->ExecuteL(EGlxCmdDrmMoreInfoOnline);
    EUNIT_ASSERT_DESC( exe == EFalse, "Non DRM item, option should be disabled");
    
    mlist.SetFocusL(NGlxListDefs::EAbsolute, 1);
    
    exe = iTester->ExecuteL(EGlxCmdDrmMoreInfoOnline);
    EUNIT_ASSERT_DESC( exe == ETrue, "Valid item");
    
    newMedia2->SetTObjectValueL(KMPXMediaDrmProtected, EFalse);
    exe = iTester->ExecuteL(EGlxCmdDrmMoreInfoOnline);
    EUNIT_ASSERT_DESC( exe == EFalse, "Non DRM item, option should be disabled");
    
    newMedia2->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    newMedia2->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXVideo);
    
    exe = iTester->ExecuteL(EGlxCmdDrmMoreInfoOnline);
    EUNIT_ASSERT_DESC( exe == EFalse, "Video, option should be disabled");
    
    newMedia2->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    newMedia2->SetTextValueL(KMPXMediaGeneralUri, _L("No info!"));
    
    EUNIT_ASSERT_DESC( exe == EFalse, "No info");
    
    exe = iTester->ExecuteL(EGlxCmdUpload);
    EUNIT_ASSERT_DESC( exe == EFalse, "Unsupported command");
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_glxcommandhandlermoreinfo,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Test0",
    "Test command handler construction, activation, deactivation",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_TestL, Teardown)
    
EUNIT_TEST(
    "Test1",
    "Test command execution",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_TestExecutionL, Teardown)    
    

EUNIT_END_TEST_TABLE

//  END OF FILE

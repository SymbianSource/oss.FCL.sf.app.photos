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
* Description:  CGlxCommandHandlerDRM unit test cases
*
*/




//  CLASS HEADER
#include "t_glxcommandhandlerdrm.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include "glxcommandhandlerdrm.h"
#include "ut_glxdummymedialist.h"
#include <glxcommandhandlers.hrh>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>
#include <glxmediageneraldefs.h>
#include <Ecom/ECom.h>

//  INTERNAL INCLUDES

CGlxCommandHandlerDRMTester* CGlxCommandHandlerDRMTester::NewL()
    {
    CGlxCommandHandlerDRMTester* self = new(ELeave)CGlxCommandHandlerDRMTester();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CGlxCommandHandlerDRMTester::~CGlxCommandHandlerDRMTester()
    {
    if ( iDRMHandler )
        {
        iDRMHandler->Deactivate();
        }
    
    iCommandHandlerList.ResetAndDestroy();
	iCommandHandlerList.Close();
	
	delete iMediaList;
    }

TBool CGlxCommandHandlerDRMTester::ExecuteL(TInt aCommand)
    {
    return iCommandHandlerList[0]->ExecuteL( aCommand );
    }
	

MGlxMediaList& CGlxCommandHandlerDRMTester::MediaList()
    {
    return *iMediaList;
    }
    
void CGlxCommandHandlerDRMTester::AddNonProtectedItemL()
    {
	TGlxMediaId testId(KGlxIdNone);
    CGlxMedia* newMedia = new (ELeave) CGlxMedia (testId);
    CleanupStack::PushL(newMedia);
    iMediaList->AppendL(0, newMedia);
	CleanupStack::Pop(newMedia);
    }
	
void CGlxCommandHandlerDRMTester::AddProtectedItemL(TInt aId, CGlxMedia* aMedia)
    {
    iMediaList->AppendL(aId, aMedia);
    }
    
void CGlxCommandHandlerDRMTester::ForceFocusChangeEventL( TInt aFocus )
    {
    iMediaList->SetFocusL( NGlxListDefs::EAbsolute, aFocus );
    
    // issue focus change event to command handler
    iDRMHandler->HandleFocusChangedL( aFocus, 0.0, NULL,
                        NGlxListDefs::EUnknown);
    }
    
void CGlxCommandHandlerDRMTester::ForceHandleAttributeL( TInt aFocus )
    {
    iMediaList->SetFocusL( NGlxListDefs::EAbsolute, aFocus );

    iMediaList->HandleAttributesAvailableL();
    }
	
CGlxCommandHandlerDRMTester::CGlxCommandHandlerDRMTester()
    {
    
    }
    
void CGlxCommandHandlerDRMTester::ConstructL()
    {
    EUNIT_PRINT(_L("CGlxCommandHandlerDRMTester::ConstructL"));
    // create 'dummy' media list
    iMediaList = new (ELeave) _CGlxTestMediaList;
    
    // create our command handler
    iDRMHandler = CGlxCommandHandlerDrm::NewL( this, EFalse );
    
    iCommandHandlerList.AppendL(iDRMHandler);
    
    iDRMHandler->ActivateL(0);
    }


// CONSTRUCTION
T_GlxCommandHandlerDRM* T_GlxCommandHandlerDRM::NewL()
    {
    T_GlxCommandHandlerDRM* self = T_GlxCommandHandlerDRM::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

T_GlxCommandHandlerDRM* T_GlxCommandHandlerDRM::NewLC()
    {
    T_GlxCommandHandlerDRM* self = new( ELeave ) T_GlxCommandHandlerDRM();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
T_GlxCommandHandlerDRM::~T_GlxCommandHandlerDRM()
    {
    }

// Default constructor
T_GlxCommandHandlerDRM::T_GlxCommandHandlerDRM()
    {
    }

// Second phase construct
void T_GlxCommandHandlerDRM::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    
    REComSession::FinalClose();
    }

//  METHODS


void T_GlxCommandHandlerDRM::SetupL(  )
    {
    EUNIT_PRINT(_L("T_GlxCommandHandlerDRM::SetupL"));
    iDRMHandlerTester = CGlxCommandHandlerDRMTester::NewL();
    }
    

void T_GlxCommandHandlerDRM::Teardown(  )
    {
    delete iDRMHandlerTester;
    iDRMHandlerTester = NULL;
    
    REComSession::FinalClose();
    }
    

void T_GlxCommandHandlerDRM::T_CreateL(  )
    {
    // no implementation
    }
    
void T_GlxCommandHandlerDRM::T_TestEmptyMediaListL()
    {
    TBool exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
    EUNIT_ASSERT_DESC( exe == EFalse, "No items in list not executed ");
    }
        
void T_GlxCommandHandlerDRM::T_TestHandleCommandsL()
    {
    MGlxMediaList& mlist = iDRMHandlerTester->MediaList();
    
    // test with item, no DRM attribute
    iDRMHandlerTester->AddNonProtectedItemL();
    
    TBool exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
    EUNIT_ASSERT_DESC( exe == ETrue, "No DRM attribute ");
    
    // test with item,  DRM attribute set to false
    TGlxMediaId testId(0);
    CGlxMedia* newMedia = new (ELeave) CGlxMedia(testId);
    CleanupStack::PushL(newMedia);
    
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, EFalse);
   
    iDRMHandlerTester->AddProtectedItemL(0, newMedia);
    
	CleanupStack::Pop(newMedia);
	
	mlist.SetFocusL(NGlxListDefs::EAbsolute, 1);
	
	exe = iDRMHandlerTester->ExecuteL( EGlxCmdDRMOpen );
	EUNIT_ASSERT_DESC( exe == ETrue, "DRM attribute false ");
    
    // test with item,  DRM attribute set to true no URI
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
	
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "No URI attribute ");
    
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
   
	// test with item,  DRM attribute set to true no category
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "No category attribute");
	
    // test with video item,  DRM attribute set to true 
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
    
    newMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXVideo);
	
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "Video");
	
    // test with iamge item,  DRM attribute set to true no size attr
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
   
   
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "Image");
	
    
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
    
    newMedia->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(100,100));
   
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "Size");
	
	newMedia->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(1000,1000));
   
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "Size");
	
	newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia->SetTextValueL(KMPXMediaGeneralUri, _L("DRM invalid"));
    
    newMedia->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(100,100));
   
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "Size");
	
	// test non supported commands
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdStateBrowse);
	
	EUNIT_ASSERT_DESC( exe == EFalse, "Execute fullscreen - command not consumed");
	
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdStateView);
	
	EUNIT_ASSERT_DESC( exe == EFalse, "Execute fullscreen - command not consumed");
	
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdSlideshowPlay);
	
	EUNIT_ASSERT_DESC( exe == EFalse, "Execute non supported command - command not consumed");
    }

void T_GlxCommandHandlerDRM::T_TestHandleFocusChangedL()
    {
    TGlxMediaId testId(12346);
    CGlxMedia* newMedia = new (ELeave) CGlxMedia(testId);
    CleanupStack::PushL(newMedia);
    
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
    
    newMedia->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(100,100));
   
    iDRMHandlerTester->AddProtectedItemL(123456, newMedia);
    
	CleanupStack::Pop(newMedia);
	
	TGlxMediaId testId2(12347);
    CGlxMedia* newMedia2 = new (ELeave) CGlxMedia(testId2);
    CleanupStack::PushL(newMedia2);
    
    newMedia2->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia2->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia2->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
    
    newMedia2->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(100,100));
   
    iDRMHandlerTester->AddProtectedItemL(123457, newMedia2);
    
	CleanupStack::Pop(newMedia2);
	
	TBool exe = iDRMHandlerTester->ExecuteL(EGlxCmdDRMOpen);
	EUNIT_ASSERT_DESC( exe == ETrue, "Execute DRM open");
	
	iDRMHandlerTester->ForceFocusChangeEventL(1);
	
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdStateView);
	
	EUNIT_ASSERT_DESC( exe == EFalse, "Execute fullscreen - command not consumed");
	iDRMHandlerTester->ForceFocusChangeEventL(1);
	
	exe = iDRMHandlerTester->ExecuteL(EGlxCmdStateBrowse);
	
	EUNIT_ASSERT_DESC( exe == EFalse, "Execute not fullscreen - command not consumed");
	iDRMHandlerTester->ForceFocusChangeEventL(1);
    }
    
void T_GlxCommandHandlerDRM::T_TestHandleAttributeL()  
    {
    TGlxMediaId testId(12346);
    CGlxMedia* newMedia = new (ELeave) CGlxMedia(testId);
    CleanupStack::PushL(newMedia);
    
    newMedia->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
    
    newMedia->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(100,100));
   
    iDRMHandlerTester->AddProtectedItemL(123456, newMedia);
    
	CleanupStack::Pop(newMedia);
	
	TGlxMediaId testId2(12347);
    CGlxMedia* newMedia2 = new (ELeave) CGlxMedia(testId2);
    CleanupStack::PushL(newMedia2);
    
    newMedia2->SetTObjectValueL(KMPXMediaDrmProtected, ETrue);
    
    newMedia2->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXImage);
    
    newMedia2->SetTextValueL(KMPXMediaGeneralUri, _L("DRM valid"));
    
    newMedia2->SetTObjectValueL(KGlxMediaGeneralDimensions, TSize(100,100));
   
    iDRMHandlerTester->AddProtectedItemL(123457, newMedia2);
    
	CleanupStack::Pop(newMedia2);
	
	iDRMHandlerTester->ForceHandleAttributeL( 1 );
	
	TBool exe = iDRMHandlerTester->ExecuteL(EGlxCmdStateView);
	
	EUNIT_ASSERT_DESC( exe == EFalse, "Execute fullscreen");
	
	iDRMHandlerTester->ForceHandleAttributeL( 1 );
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_GlxCommandHandlerDRM,
    "Add test suite description here.",
    "UNIT" )
   
EUNIT_TEST(
    "T_CreateL",
    "Create DRM command handler",
    "T_CreateL",
    "FUNCTIONALITY",
    SetupL, T_CreateL, Teardown)

EUNIT_TEST(
    "T_Test1L",
    "Test with empty media list",
    "T_Test1L",
    "FUNCTIONALITY",
    SetupL, T_TestEmptyMediaListL, Teardown)
    
EUNIT_TEST(
    "T_Test2",
    "Test command functionality",
    "T_Test2L",
    "FUNCTIONALITY",
    SetupL, T_TestHandleCommandsL, Teardown)
    
EUNIT_TEST(
    "T_Test3",
    "Test response to HandleFocusChangedL",
    "T_Test3L",
    "FUNCTIONALITY",
    SetupL, T_TestHandleFocusChangedL, Teardown)
    
EUNIT_TEST(
    "T_Test4",
    "Test response to HandleAttributesAvailableL",
    "T_Test4L",
    "FUNCTIONALITY",
    SetupL, T_TestHandleAttributeL, Teardown)
  
EUNIT_END_TEST_TABLE

//  END OF FILE

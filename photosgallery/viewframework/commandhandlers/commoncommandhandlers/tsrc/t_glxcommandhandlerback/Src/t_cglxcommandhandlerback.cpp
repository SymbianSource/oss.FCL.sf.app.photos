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
* Description:  CommandHandlerBack Unit test cases.
*
*/




//  CLASS HEADER
#include "t_cglxcommandhandlerback.h"

//  EXTERNAL INCLUDES
#include <avkon.hrh>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxmediageneraldefs.h>	// For Attribute constants
#include <glxcommandhandlers.hrh>
#include <mglxmedialist.h>
//  INTERNAL INCLUDES
#include "glxcommandhandlerback.h"	// For CGlxCommandHandlerBack

// CONSTRUCTION
t_cglxcommandhandlerback* t_cglxcommandhandlerback::NewL()
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::NewL()"));
    t_cglxcommandhandlerback* self = t_cglxcommandhandlerback::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

t_cglxcommandhandlerback* t_cglxcommandhandlerback::NewLC()
    {
    EUNIT_PRINT(_L("ut_cglxcommandhandlerback::NewLC()"));
    t_cglxcommandhandlerback* self = new( ELeave ) t_cglxcommandhandlerback();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
t_cglxcommandhandlerback::~t_cglxcommandhandlerback()
    {
    }

// Default constructor
t_cglxcommandhandlerback::t_cglxcommandhandlerback()
    {
    }

// Second phase construct
void t_cglxcommandhandlerback::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    EUNIT_PRINT(_L("ut_cglxcommandhandlerback::ConstructL()"));
    CEUnitTestSuiteClass::ConstructL();
    }

void t_cglxcommandhandlerback::CreateMediaListL()
    {
    // Create a path
    EUNIT_PRINT(_L("ut_cglxcommandhandlerback::CreateMediaListL()"));
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	path->AppendL(0x200071AE); // Tag collection implementation Uid
    
    // Create static media list
  iMediaList = MGlxMediaList::InstanceL(*path);
	if(iMediaList)
    	{
    	// Attribute to be used
    	TMPXAttribute attrTitle(KMPXMediaGeneralTitle);
    	
    	// add a static items
	    TGlxMediaId testId(12345);
	    CGlxMedia* newMedia1 = new (ELeave) CGlxMedia(testId);
	    CleanupStack::PushL(newMedia1);
	    newMedia1->SetTextValueL(attrTitle, _L("Picasso"));		    
	    iMediaList->AddStaticItemL(newMedia1, NGlxListDefs::EInsertFirst);
    	CleanupStack::Pop(newMedia1);
    	
    	
    	TGlxMediaId testId2(12346);
	    CGlxMedia* newMedia2 = new (ELeave) CGlxMedia(testId2);
	    CleanupStack::PushL(newMedia2);
	    newMedia2->SetTextValueL(attrTitle, _L("Portugal"));
	    iMediaList->AddStaticItemL(newMedia2, NGlxListDefs::EInsertLast);
    	CleanupStack::Pop(newMedia2);
    	}
    
    CleanupStack::PopAndDestroy( path );
    }
    
MGlxMediaList& t_cglxcommandhandlerback::MediaList()
    {
    EUNIT_PRINT(_L("ut_cglxcommandhandlerback::MediaList()"));
		return *iMediaList;    
		}

//  METHODS
void t_cglxcommandhandlerback::SetupBackChL(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::SetupBackChL(  )"));
    iCommand = CGlxCommandHandlerBack::NewBackCommandHandlerL();
    }

void t_cglxcommandhandlerback::SetupPreviousViewChWithoutMlL(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::SetupPreviousViewChWithoutMlL(  )"));
    iCommand = CGlxCommandHandlerBack::NewPreviousViewCommandHandlerL();
    }

void t_cglxcommandhandlerback::SetupPreviousViewChWithMlL(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::SetupPreviousViewChWithMlL(  )"));
    CreateMediaListL();
    
    iCommand = CGlxCommandHandlerBack::NewPreviousViewCommandHandlerL(this);
    }

void t_cglxcommandhandlerback::SetupPreviousNaviViewChWithoutMlL(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::SetupPreviousNaviViewChWithoutMlL(  )"));
    }
    
void t_cglxcommandhandlerback::SetupPreviousNaviViewChWithMlL(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::SetupPreviousNaviViewChWithMlL(  )"));
    CreateMediaListL();
    }

void t_cglxcommandhandlerback::SetupContainerPreviousViewChWithoutViewId(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::SetupContainerPreviousViewChWithoutViewId(  )"));
    CreateMediaListL();
    iCommand = CGlxCommandHandlerBack::NewContainerPreviousViewCommandHandlerL(this);
    }

void t_cglxcommandhandlerback::SetupContainerPreviousViewChWithViewId(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::SetupContainerPreviousViewChWithViewId(  )"));
    CreateMediaListL();
    
    iCommand = CGlxCommandHandlerBack::NewContainerPreviousViewCommandHandlerL(this,12345);
    }

void t_cglxcommandhandlerback::Teardown(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::Teardown(  )"));
    if( iCommand )
        {
        delete iCommand;
        iCommand = NULL;            
        }
        
    if( iMediaList )
        {
        iMediaList->Close();
        iMediaList = NULL;
        }        
    }


void t_cglxcommandhandlerback::T_TestConstructionL(  )
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::T_TestConstructionL(  )"));
    EUNIT_ASSERT_DESC( iCommand, "Command handler not created properly");
    }

void t_cglxcommandhandlerback::T_TestExecutionL()
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::T_TestExecutionL()"));
    EUNIT_ASSERT_DESC( ETrue == iCommand->ExecuteL(EAknSoftkeyBack),
                        "Unable to handle the command");
    }
         
void t_cglxcommandhandlerback::T_TestInvalidExecutionL()
    {
    EUNIT_PRINT(_L("t_cglxcommandhandlerback::T_TestInvalidExecutionL()"));
    EUNIT_ASSERT_DESC( EFalse == iCommand->ExecuteL(EGlxCmdRename),
                        "Wrong command being handled");
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_cglxcommandhandlerback,
    "Unit test suite for CGlxCommandHandlerBack",
    "UNIT" )

EUNIT_TEST(
    "Create Back CH",
    "CGlxCommandHandlerBack",
    "NewBackCommandHandlerL",
    "FUNCTIONALITY",
    SetupBackChL, T_TestConstructionL, Teardown)

EUNIT_TEST(
    "Create PreviousView CH without Medialist Owner",
    "CGlxCommandHandlerBack",
    "NewPreviousViewCommandHandlerL",
    "FUNCTIONALITY",
    SetupPreviousViewChWithoutMlL, T_TestConstructionL, Teardown)
    
EUNIT_TEST(
    "Create PreviousView CH with Medialist Owner",
    "CGlxCommandHandlerBack",
    "NewPreviousViewCommandHandlerL",
    "FUNCTIONALITY",
    SetupPreviousViewChWithMlL, T_TestConstructionL, Teardown)


/*EUNIT_TEST(
    "Create PreviousView CH without Medialist Owner",
    "CGlxCommandHandlerBack",
    "NewPreviousNaviViewCommandHandlerL",
    "FUNCTIONALITY",
    SetupPreviousNaviViewChWithoutMlL, T_TestConstructionL, Teardown)

         
EUNIT_TEST(
    "Create PreviousView CH with Medialist Owner",
    "CGlxCommandHandlerBack",
    "NewPreviousNaviViewCommandHandlerL",
    "FUNCTIONALITY",
    SetupPreviousNaviViewChWithMlL, T_TestConstructionL, Teardown)*/

EUNIT_TEST(
    "Create Container PreviousView CH without Previous viewId",
    "CGlxCommandHandlerBack",
    "NewContainerPreviousViewCommandHandlerL",
    "FUNCTIONALITY",
    SetupContainerPreviousViewChWithoutViewId, T_TestConstructionL, Teardown)
    
EUNIT_TEST(
    "Create Container PreviousView CH with Previous viewId",
    "CGlxCommandHandlerBack",
    "NewContainerPreviousViewCommandHandlerL",
    "FUNCTIONALITY",
    SetupContainerPreviousViewChWithViewId, T_TestConstructionL, Teardown)

EUNIT_TEST(
    "Execute Back command",
    "CGlxCommandHandlerBack",
    "ExecuteL",
    "FUNCTIONALITY",
    SetupBackChL, T_TestExecutionL, Teardown)
    
EUNIT_TEST(
    "Execute Previous command",
    "CGlxCommandHandlerBack",
    "ExecuteL",
    "FUNCTIONALITY",
    SetupPreviousViewChWithoutMlL, T_TestExecutionL, Teardown)
    
EUNIT_TEST(
    "Execute Previous command with ML",
    "CGlxCommandHandlerBack",
    "ExecuteL",
    "FUNCTIONALITY",
    SetupPreviousViewChWithMlL, T_TestExecutionL, Teardown)

EUNIT_TEST(
    "Execute Container PreviousView CH without Previous viewId",
    "CGlxCommandHandlerBack",
    "ExecuteL",
    "FUNCTIONALITY",
    SetupContainerPreviousViewChWithoutViewId, T_TestExecutionL, Teardown)
    
EUNIT_TEST(
    "Execute Container PreviousView CH with Previous viewId",
    "CGlxCommandHandlerBack",
    "ExecuteL",
    "FUNCTIONALITY",
    SetupContainerPreviousViewChWithViewId, T_TestExecutionL, Teardown)

EUNIT_TEST(
    "Execute Rename command",
    "CGlxCommandHandlerBack",
    "ExecuteL",
    "FUNCTIONALITY",
    SetupPreviousViewChWithoutMlL, T_TestInvalidExecutionL, Teardown)                

EUNIT_END_TEST_TABLE

//  END OF FILE

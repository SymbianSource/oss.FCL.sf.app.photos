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
* Description:  CommandHandlerSortOrder unit test cases
*
*/




//  CLASS HEADER
#include "ut_cglxcommandhandlersortorder.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
#include <mpxmediageneraldefs.h>	// For Attribute constants
#include <glxcommandhandlers.hrh>
#include <glxmedialist.h>
#include <glxpanic.h>
#include <glxsettingsmodel.h>		// For cenrep setting 

//  INTERNAL INCLUDES
#include "glxcommandhandlersortorder.h"	// For CGlxCommandHandlerSortOrder

// CONSTRUCTION
ut_cglxcommandhandlersortorder* ut_cglxcommandhandlersortorder::NewL()
    {
    ut_cglxcommandhandlersortorder* self = ut_cglxcommandhandlersortorder::NewLC();
    CleanupStack::Pop(self);

    return self;
    }

ut_cglxcommandhandlersortorder* ut_cglxcommandhandlersortorder::NewLC()
    {
    ut_cglxcommandhandlersortorder* self = new( ELeave ) ut_cglxcommandhandlersortorder();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
ut_cglxcommandhandlersortorder::~ut_cglxcommandhandlersortorder()
    {
    }

// Default constructor
ut_cglxcommandhandlersortorder::ut_cglxcommandhandlersortorder()
    {
    iFilterFlag = ETrue;
    }

// Second phase construct
void ut_cglxcommandhandlersortorder::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

MGlxMediaList& ut_cglxcommandhandlersortorder::MediaList()
    {
	return *iMediaList;    
	}


//  METHODS
// With medialist owner (filters applied) & valid setting key
void ut_cglxcommandhandlersortorder::SetupMethod1L()
    {
    // Create a path
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	path->AppendL(0x200071AE); // Tag collection implementation Uid
    
	// Create a filter
	CMPXFilter* filter = TGlxFilterFactory::CreateAlphabeticSortFilterL();
	CleanupStack::PushL(filter);
	
    iMediaList = MGlxMediaList::InstanceL(*path, 
									TGlxHierarchyId(0x200071AE),
									filter);
	
	CleanupStack::PopAndDestroy(filter);		// filter
    CleanupStack::PopAndDestroy(path); // path
    
    iCommand = CGlxCommandHandlerSortOrder::NewL(this, KGlxSortOrderTagManager);
    }

// With medialist owner (without filters) & valid setting key         
void ut_cglxcommandhandlersortorder::SetupMethod2L()
    {
    // Create a path
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	path->AppendL(0x200071AE); // Tag collection implementation Uid
    
	iMediaList = MGlxMediaList::InstanceL(*path, 
									TGlxHierarchyId(0x200071AE));
	
    CleanupStack::PopAndDestroy(path); // path
    
    iCommand = CGlxCommandHandlerSortOrder::NewL(this, KGlxSortOrderTagManager);
    }

// With medialist owner as NULL & valid setting key
void ut_cglxcommandhandlersortorder::SetupMethod3L()
    {
    iCommand = CGlxCommandHandlerSortOrder::NewL(NULL, KGlxSortOrderTagBrowser);
    }

// With medialist owner (filters applied) & invalid setting key         
void ut_cglxcommandhandlersortorder::SetupMethod4L()
    {
    // Create a path
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	path->AppendL(0x200071AE); // Tag collection implementation Uid
    
	// Create a filter
	CMPXFilter* filter = TGlxFilterFactory::CreateAlphabeticSortFilterL();
	CleanupStack::PushL(filter);
	
    iMediaList = MGlxMediaList::InstanceL(*path, 
									TGlxHierarchyId(0x200071AE),
									filter);
	
	CleanupStack::PopAndDestroy(filter);		// filter
    CleanupStack::PopAndDestroy(path); // path
    
    iCommand = CGlxCommandHandlerSortOrder::NewL(this, 0x00000000);
    }

// Clean up code
void ut_cglxcommandhandlersortorder::Teardown()
    {
    iFilterFlag = EFalse;
    if( iCommand )
        {
        delete iCommand;
        }
    if( iMediaList )
        {
        iMediaList->Close();
        iMediaList = NULL;
        }
    }

// Test command handler creation
void ut_cglxcommandhandlersortorder::T_TestCreationL()
    {
    EUNIT_ASSERT_DESC( iCommand, "Command creation failed");
    }

// Test alphabetical sort
void ut_cglxcommandhandlersortorder::T_TestAlphaSortL()
    {
    EUNIT_ASSERT_DESC( iCommand->DoExecuteL(EGlxCmdSortByAlphabetical, MediaList()), 
                        "Alphabetical sorting failed");
    }

// Test alphabetical sort where the Medialist is NULL
void ut_cglxcommandhandlersortorder::T_TestAlphaSortWithNullListL()
	{
	EUNIT_ASSERT_PANIC_DESC( iCommand->DoExecuteL(EGlxCmdSortByAlphabetical, MediaList()), 
							"Images & videos",
							EGlxPanicIllegalArgument,
							"Alphabetical sorting failed");
	}

// Test Frequency sort
void ut_cglxcommandhandlersortorder::T_TestFrequencySortL()
    {
    EUNIT_ASSERT_DESC( !iCommand->DoExecuteL(EGlxFilterSortOrderItemCount, MediaList()), 
                        "Frequency sorting failed");
    }

// Test Frequency sort where the Medialist is NULL
void ut_cglxcommandhandlersortorder::T_TestFrequencySortWithNullListL()
	{
	EUNIT_ASSERT_PANIC_DESC( !iCommand->DoExecuteL(EGlxFilterSortOrderItemCount, MediaList()), 
							"Images & videos",
							EGlxPanicIllegalArgument,
							"Frequency sorting failed");
	}

// Test an invalid command with the command handler
void ut_cglxcommandhandlersortorder::T_TestInvalidSortL()
    {
    EUNIT_ASSERT_DESC( !iCommand->DoExecuteL(1000, MediaList()), 
                        "Invalid command is also working");
    }

// Test disable of alphabetical sort menu option
void ut_cglxcommandhandlersortorder::T_TestDisableAlphaL()
    {
	EUNIT_ASSERT_DESC( iCommand->DoIsDisabled(EGlxCmdSortByAlphabetical, MediaList()), 
                        "Menu disable failed");
    }

// Test disable of alphabetical sort menu option where the Medialist is NULL
void ut_cglxcommandhandlersortorder::T_TestDisableAlphaWithNullListL()
	{
	EUNIT_ASSERT_PANIC_DESC( iCommand->DoIsDisabled(EGlxCmdSortByAlphabetical, MediaList()), 
								"Images & videos",
								EGlxPanicIllegalArgument,
								"Menu disable failed");
	}

// Test disable of frequency sort menu option
void ut_cglxcommandhandlersortorder::T_TestDisableFrequencyL()
    {
	EUNIT_ASSERT_DESC( !iCommand->DoIsDisabled(EGlxFilterSortOrderItemCount, MediaList()), 
                        "Menu disable failed");
     }

// Test disable of frequency sort menu option where the Medialist is NULL
void ut_cglxcommandhandlersortorder::T_TestDisableFrequencyWithNullListL()
	{
	EUNIT_ASSERT_PANIC_DESC( !iCommand->DoIsDisabled(EGlxFilterSortOrderItemCount, MediaList()),
							"Images & videos",
							EGlxPanicIllegalArgument,
							"Menu disable failed");
     }

// Test menu disable with invalid command id
void ut_cglxcommandhandlersortorder::T_TestDisableInvalidCommandL()
    {
    EUNIT_ASSERT_DESC( !iCommand->DoIsDisabled(1000, MediaList()), 
                        "Unrelated Menu disabled - Test case failed");
    }
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    ut_cglxcommandhandlersortorder,
    "Unit test suite for CGlxCommandHandlerSortOrder",
    "UNIT" )

EUNIT_TEST(
    "Create with filters",
    "CGlxCommandHandlerSortOrder",
    "NewL",
    "FUNCTIONALITY",
    SetupMethod1L, T_TestCreationL, Teardown)

EUNIT_TEST(
    "Create without filters",
    "CGlxCommandHandlerSortOrder",
    "NewL",
    "FUNCTIONALITY",
    SetupMethod2L, T_TestCreationL, Teardown)
    
EUNIT_TEST(
    "Create with NULL",
    "CGlxCommandHandlerSortOrder",
    "NewL",
    "FUNCTIONALITY",
    SetupMethod3L, T_TestCreationL, Teardown)
    
EUNIT_TEST(
    "Create with Invalid Key",
    "CGlxCommandHandlerSortOrder",
    "NewL",
    "FUNCTIONALITY",
    SetupMethod4L, T_TestCreationL, Teardown) 
    
EUNIT_TEST(
    "Alpha sort with filters",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod1L, T_TestAlphaSortL, Teardown)

EUNIT_TEST(
    "Aplpha sort without filters",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod2L, T_TestAlphaSortL, Teardown)
    
EUNIT_TEST(
    "Aplpha sort with NULL",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod3L, T_TestAlphaSortWithNullListL, Teardown)
    
EUNIT_TEST(
    "Aplpha sort with Invalid Key",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod4L, T_TestAlphaSortL, Teardown)
    
EUNIT_TEST(
    "Freq sort with filters",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod1L, T_TestFrequencySortL, Teardown)

EUNIT_TEST(
    "Freq sort without filters",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod2L, T_TestFrequencySortL, Teardown)
    
EUNIT_TEST(
    "Freq sort with NULL",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod3L, T_TestFrequencySortWithNullListL, Teardown)
    
EUNIT_TEST(
    "Freq sort with Invalid Key",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod4L, T_TestFrequencySortL, Teardown) 
    
EUNIT_TEST(
    "Sort with invalid command id",
    "CGlxCommandHandlerSortOrder",
    "DoExecuteL",
    "FUNCTIONALITY",
    SetupMethod1L, T_TestInvalidSortL, Teardown)
    
EUNIT_TEST(
    "Disable Alpha sort with filters",
    "CGlxCommandHandlerSortOrder",
    "DoIsDisabled",
    "FUNCTIONALITY",
    SetupMethod1L, T_TestDisableAlphaL, Teardown)

EUNIT_TEST(
    "Disable Aplpha sort without filters",
    "CGlxCommandHandlerSortOrder",
    "DoIsDisabled",
    "FUNCTIONALITY",
    SetupMethod2L, T_TestDisableAlphaL, Teardown)
    
EUNIT_TEST(
    "Disable Aplpha sort with NULL",
    "CGlxCommandHandlerSortOrder",
    "DoIsDisabled",
    "FUNCTIONALITY",
    SetupMethod3L, T_TestDisableAlphaWithNullListL, Teardown)
    
EUNIT_TEST(
    "Disable Freq sort with filters",
    "CGlxCommandHandlerSortOrder",
    "DoIsDisabled",
    "FUNCTIONALITY",
    SetupMethod1L, T_TestDisableFrequencyL, Teardown)

EUNIT_TEST(
    "Disable Freq sort without filters",
    "CGlxCommandHandlerSortOrder",
    "DoIsDisabled",
    "FUNCTIONALITY",
    SetupMethod2L, T_TestDisableFrequencyL, Teardown)
    
EUNIT_TEST(
    "Disable Freq sort with NULL",
    "CGlxCommandHandlerSortOrder",
    "DoIsDisabled",
    "FUNCTIONALITY",
    SetupMethod3L, T_TestDisableFrequencyWithNullListL, Teardown)
    
EUNIT_TEST(
    "Disable menu with invalid command id",
    "CGlxCommandHandlerSortOrder",
    "DoIsDisabled",
    "FUNCTIONALITY",
    SetupMethod1L, T_TestDisableInvalidCommandL, Teardown)                              

EUNIT_END_TEST_TABLE

//  END OF FILE

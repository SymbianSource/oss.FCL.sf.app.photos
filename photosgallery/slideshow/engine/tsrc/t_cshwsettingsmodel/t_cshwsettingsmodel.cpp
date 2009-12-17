/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Test for settings model for the slideshow
*
*/




//  CLASS HEADER
#include "t_cshwsettingsmodel.h"

//	CLASS UNDER TEST
#include "shwsettingsmodel.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>
#include <centralrepository.h>

//  INTERNAL INCLUDES


// CONSTRUCTION
T_CShwSettingsModel* T_CShwSettingsModel::NewL()
    {
    T_CShwSettingsModel* self = T_CShwSettingsModel::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CShwSettingsModel* T_CShwSettingsModel::NewLC()
    {
    T_CShwSettingsModel* self = new (ELeave) T_CShwSettingsModel();
    CleanupStack::PushL(self);

    self->ConstructL();
    return self;
    }


// Destructor (virtual by CBase)
T_CShwSettingsModel::~T_CShwSettingsModel()
    {
    }


// Default constructor
T_CShwSettingsModel::T_CShwSettingsModel()
    {
    }


// Second phase construct
void T_CShwSettingsModel::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void T_CShwSettingsModel::SetupL()
    {
    iShwSettingsModel = CShwSettingsModel::NewL();
    }

void T_CShwSettingsModel::Teardown()
    {
	delete iShwSettingsModel;
	iShwSettingsModel = NULL;
	
	const TInt KEngineId = 0x200071D3;	
	const TUid KEngineUid = TUid::Uid(KEngineId);
    TRAP_IGNORE( 
        {
    	CRepository* repository = CRepository::NewL(KEngineUid);
    	TInt err = repository->Reset();
    	User::LeaveIfError(err);
    	delete repository;
        } );
    }



void T_CShwSettingsModel::TestGetDefaultValues()
	{
    TBuf<KMaxFileName> fileNamePath;
    iShwSettingsModel->MusicNamePathL(fileNamePath);
    _LIT(KDefaultFileName, "None");
    EUNIT_ASSERT(fileNamePath == KDefaultFileName);

    const TUint KDefaultUid = 0x200071D6;
    TUid defaultUId;
    defaultUId.iUid = KDefaultUid;

	TUid uId;
	TUint index = 0;	
    iShwSettingsModel->TransitionTypeL(uId, index);
    EUNIT_ASSERT(defaultUId.iUid == uId.iUid);
    EUNIT_ASSERT_EQUALS_DESC( 1, index, "default index is 0");
	}

void T_CShwSettingsModel::TestSetAndGetMusicPathL()
    {
    // Set music name and path field.
	_LIT(KTestFileName, "c:\\test\\knightrider.mp3");
    iShwSettingsModel->SaveMusicNamePathL( KTestFileName() );

    // Compare returned values to those set.
    TFileName fileNamePath;
    iShwSettingsModel->MusicNamePathL(fileNamePath);
    EUNIT_ASSERT(fileNamePath == KTestFileName);

    // try setting an empty music name
    iShwSettingsModel->SaveMusicNamePathL( KNullDesC() );
    // test that it was not set
    iShwSettingsModel->MusicNamePathL(fileNamePath);
    EUNIT_ASSERT(fileNamePath == KTestFileName);
    }


void T_CShwSettingsModel::TestSetAndGetMusicOnOffL()
    {
    iShwSettingsModel->SaveMusicStateL(ETrue);
    EUNIT_ASSERT(iShwSettingsModel->MusicOnL() == ETrue);
    }
    

void T_CShwSettingsModel::TestSetAndGetTransDelayL()
    {
    const TUint KTimeDelay = 5;
    iShwSettingsModel->SaveTransDelayL(KTimeDelay);
	EUNIT_ASSERT(iShwSettingsModel->TransDelayL() == KTimeDelay);
    }    

    
void T_CShwSettingsModel::TestSetAndGetTransitionTypeL()
    {
    const TUint KUid = 0x01234567;
    TUid uId1;
    uId1.iUid = KUid;
    iShwSettingsModel->SaveTransitionTypeL(uId1, 1);

	TUid uId2;
	TUint index = 0;	
    iShwSettingsModel->TransitionTypeL(uId2, index);
    EUNIT_ASSERT(uId1.iUid == uId2.iUid);
    EUNIT_ASSERT(index == 1);
    }       
    
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    T_CShwSettingsModel,
    "Test suite for CShwSettingsModel",
    "MODULE" )

EUNIT_TEST(
    "Test Get Default Values",
    "TestGetDefaultValues",
    "Central Repositoy getter - default values",
    "FUNCTIONALITY",
    SetupL, TestGetDefaultValues, Teardown)


EUNIT_TEST(
    "Test Set And Get Music Path",
    "TestSetAndGetMusicPath",
    "Music name and path Central Repositoy setter and getter",
    "FUNCTIONALITY",
    SetupL, TestSetAndGetMusicPathL, Teardown)


EUNIT_TEST(
    "Test Set And Get Music On/Off",
    "TestSetAndGetMusicOnOffL",
    "Music On/Off Central Repositoy setter and getter",
    "FUNCTIONALITY",
    SetupL, TestSetAndGetMusicOnOffL, Teardown)


EUNIT_TEST(
    "Test Set And Get Transition Delay",
    "TestSetAndGetTransDelayL",
    "Transition Delay Central Repositoy setter and getter",
    "FUNCTIONALITY",
    SetupL, TestSetAndGetTransDelayL, Teardown)


EUNIT_TEST(
    "Test Set And Get Transition Type",
    "TestSetAndGetTransitionTypeL",
    "Transition Type Central Repositoy setter and getter",
    "FUNCTIONALITY",
    SetupL, TestSetAndGetTransitionTypeL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE

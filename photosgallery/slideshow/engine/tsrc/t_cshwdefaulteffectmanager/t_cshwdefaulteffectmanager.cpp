/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Test for default effect order manager
 *
*/



//  CLASS HEADER
#include "t_cshwdefaulteffectmanager.h"

//  EXTERNAL INCLUDES
#include <Digia/eunit/eunitmacros.h>
#include <digia/eunit/EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "shwdefaulteffectmanager.h"
#include "shweffect.h"
#include "shwslideshowenginepanic.h"

//	STUBS
#include "stub_mshweffect.h"

// -----------------------------------------------------------------------------
// Stub for NShwEngine -->
// -----------------------------------------------------------------------------
namespace NShwEngine
	{
	extern void Panic( TShwEnginePanic aPanic )
		{
		// do a leave in test situation
		User::Leave( aPanic );
		}
	}
// -----------------------------------------------------------------------------
// <-- Stub for NShwEngine
// -----------------------------------------------------------------------------

// CONSTRUCTION
T_CShwDefaultEffectManager* T_CShwDefaultEffectManager::NewL()
    {
    T_CShwDefaultEffectManager* self = T_CShwDefaultEffectManager::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CShwDefaultEffectManager* T_CShwDefaultEffectManager::NewLC()
    {
    T_CShwDefaultEffectManager* self = new( ELeave ) T_CShwDefaultEffectManager();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CShwDefaultEffectManager::~T_CShwDefaultEffectManager()
    {
    }

// Default constructor
T_CShwDefaultEffectManager::T_CShwDefaultEffectManager()
    {
    }

// Second phase construct
void T_CShwDefaultEffectManager::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS
void T_CShwDefaultEffectManager::EmptySetupL()
    {
    }

void T_CShwDefaultEffectManager::SetupL()
    {
	// create a default effect manager
	iDefaultEffectManager = CShwDefaultEffectManager::NewL();
    }
    

void T_CShwDefaultEffectManager::Teardown()
    {
    delete iDefaultEffectManager;
    iDefaultEffectManager = NULL; // set to NULL to prevent double delete
    }
   
void T_CShwDefaultEffectManager::TestConstructionL()
	{
	EUNIT_PRINT( _L("TestConstructionL") );

	// create a default effect manager
	CShwDefaultEffectManager* tmp = CShwDefaultEffectManager::NewL();

	EUNIT_ASSERT_DESC( tmp, "Construction successful");

	delete tmp;
	}

void T_CShwDefaultEffectManager::TestAddEffectL()
    {
    T_MShwTestEffect* effect = new(ELeave) T_MShwTestEffect( 0 );

    CleanupStack::PushL( effect );
    iDefaultEffectManager->AddEffectL( effect );  // takes ownership
    CleanupStack::Pop( effect );

	// TRAP since we expect it to leave
	//TRAPD( error, iDefaultEffectManager->CurrentEffect() );
   // EUNIT_ASSERT_EQUALS_DESC( 
   // 	NShwEngine::EIncorrectEffectIndex, error, "iEffects was not set");
	// get effects info
   	TShwEffectInfo info = effect->EffectInfo();
	// set effect order
	iDefaultEffectManager->SetDefaultEffectL( info );
	// ask for effect again
	MShwEffect* current = iDefaultEffectManager->CurrentEffect();
	// and check that it is the same one as this
    EUNIT_ASSERT_DESC( 
    	current == static_cast<MShwEffect*>( effect ), "current effect was set");   
    }
	
void T_CShwDefaultEffectManager::TestAddNullEffectL()
    {
    iDefaultEffectManager->AddEffectL( NULL );

	// TRAP since we expect it to leave
	//TRAPD( error, iDefaultEffectManager->CurrentEffect() );
    //EUNIT_ASSERT_EQUALS_DESC( 
    //	NShwEngine::EIncorrectEffectIndex, error, "iEffects was not set");

	// create empty effect info
   	TShwEffectInfo info;
	TInt error ;
	// set effect order
	TRAP( error, iDefaultEffectManager->SetDefaultEffectL( info ) );
	// ask for effect again
    EUNIT_ASSERT_EQUALS_DESC( KErrArgument, error, "illegal argument");
    }
    
void T_CShwDefaultEffectManager::TestNextEffectL()
    {
    // Create two effects
    T_MShwTestEffect* effect0 = new(ELeave) T_MShwTestEffect(0);
    CleanupStack::PushL( effect0 );

    // Add the effects
    iDefaultEffectManager->AddEffectL( effect0 );  // takes ownership
    CleanupStack::Pop( effect0 );

	// get first effects info
   	TShwEffectInfo info = effect0->EffectInfo();
	// set default effect, this means that we will take a clone
	// of the first effect and return either the original or the
	// clone
	iDefaultEffectManager->SetDefaultEffectL( info );
    
    // Check the current effect
    MShwEffect* effect1 = iDefaultEffectManager->CurrentEffect();
    // Expect to get the effects in the order they were added...
    EUNIT_ASSERT( effect1 == static_cast<MShwEffect*>( effect0 ) );
    // get the next effect
    MShwEffect* effect2 = iDefaultEffectManager->Effect( 1 );
    
    // Proceed to next
    iDefaultEffectManager->ProceedToEffect( 1 );

	// 
    EUNIT_ASSERT( effect2 == iDefaultEffectManager->CurrentEffect() );
    EUNIT_ASSERT( effect1 == iDefaultEffectManager->Effect( 1 ) );
    EUNIT_ASSERT( effect1 == iDefaultEffectManager->Effect( -1 ) );
    EUNIT_ASSERT( effect2 == iDefaultEffectManager->Effect( 2 ) );
    }

/*
void T_CShwDefaultEffectManager::TestProgrammedEffectL()
    {
    // Create two effects
    T_MShwTestEffect* effect0 = new(ELeave) T_MShwTestEffect(0);
    CleanupStack::PushL(effect0);
    T_MShwTestEffect* effect1 = new(ELeave) T_MShwTestEffect(1);
    CleanupStack::PushL(effect1);
    
    // Add the effects
    iDefaultEffectManager->AddEffectL(effect0);  // takes ownership
    iDefaultEffectManager->AddEffectL(effect1);  // takes ownership
    
    CleanupStack::Pop(2, effect0);
    
    // Specify programmed order
    iDefaultEffectManager->SetEffectOrder(MShwEffectManager::EShwEffectOrderProgrammed);
    
    // Create the programmed order
    RArray<MShwEffect*> programme;
    CleanupClosePushL(programme);
//    CleanupStack::PushL(&programme);
    programme.AppendL(effect1);
    programme.AppendL(effect0);
    programme.AppendL(effect1);
    
    // Set the order
    iDefaultEffectManager->SetProgrammedEffects(programme);
    
    // 
    // Check the first effect
    MShwEffect* inEffect = static_cast<MShwEffect*>(effect1);
    MShwEffect* outEffect = iDefaultEffectManager->CurrentEffect();
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);
   
    inEffect = static_cast<MShwEffect*>(effect0); 
    outEffect = iDefaultEffectManager->NextEffect(); 
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);

    iDefaultEffectManager->ProceedToNextEffect();
    
    inEffect = static_cast<MShwEffect*>(effect1); 
    outEffect = iDefaultEffectManager->NextEffect(); 
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);
    
    CleanupStack::PopAndDestroy(&programme);
    }
 
    
void T_CShwDefaultEffectManager::TestMultiProgrammedEffectL()
    {
    // Create two effects
    T_MShwTestEffect* effect0 = new(ELeave) T_MShwTestEffect(0);
    CleanupStack::PushL(effect0);
    T_MShwTestEffect* effect1 = new(ELeave) T_MShwTestEffect(1);
    CleanupStack::PushL(effect1);
    T_MShwTestEffect* effect2 = new(ELeave) T_MShwTestEffect(2);
    CleanupStack::PushL(effect2);
    
    // Add the effects
    iDefaultEffectManager->AddEffectL(effect0);  // takes ownership
    iDefaultEffectManager->AddEffectL(effect1);  // takes ownership
    iDefaultEffectManager->AddEffectL(effect2);  // takes ownership
    
    CleanupStack::Pop(3, effect0);
    
    // Specify programmed order
    iDefaultEffectManager->SetEffectOrder(MShwEffectManager::EShwEffectOrderProgrammed);
    
    // Create the programmed order
    RArray<MShwEffect*> programme;
    CleanupClosePushL(programme);
//    CleanupStack::PushL(&programme);
    programme.AppendL(effect2);
    programme.AppendL(effect1);
    programme.AppendL(effect0);
    programme.AppendL(effect2);
    programme.AppendL(effect1);
    programme.AppendL(effect0); 
       
    // Set the order
    iDefaultEffectManager->SetProgrammedEffects(programme);
    
    // 
    // Check the effect order is retrieved as 2, 1, 0
    MShwEffect* inEffect = static_cast<MShwEffect*>(effect2);
    MShwEffect* outEffect = iDefaultEffectManager->CurrentEffect();
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);

    inEffect = static_cast<MShwEffect*>(effect1); 
    outEffect = iDefaultEffectManager->NextEffect(); 
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);
    
    iDefaultEffectManager->ProceedToNextEffect();
       
    inEffect = static_cast<MShwEffect*>(effect2); 
    outEffect = iDefaultEffectManager->NextEffect(); 
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);
    
    iDefaultEffectManager->ProceedToNextEffect();
       
    inEffect = static_cast<MShwEffect*>(effect1); 
    outEffect = iDefaultEffectManager->NextEffect(); 
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);
    
    iDefaultEffectManager->ProceedToNextEffect();
       
    inEffect = static_cast<MShwEffect*>(effect0); 
    outEffect = iDefaultEffectManager->NextEffect(); 
    EUNIT_ASSERT_EQUALS(inEffect, outEffect);    
    iDefaultEffectManager->ProceedToNextEffect();
       
    CleanupStack::PopAndDestroy(&programme);
    }
*/    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwDefaultEffectManager,
    "Test suite for CShwDefaultEffectManager.",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Construct - Destruct Test",
    "CShwDefaultEffectManager",
    "CShwDefaultEffectManager",
    "FUNCTIONALITY",
    EmptySetupL, TestConstructionL, Teardown )	// needs to have teardown as alloc test

EUNIT_TEST(
    "Add Effect Test",
    "CShwDefaultEffectManager",
    "CShwDefaultEffectManager",
    "FUNCTIONALITY",
    SetupL, TestAddEffectL, Teardown )
    
EUNIT_TEST(
    "Add NULL Effect Test",
    "CShwDefaultEffectManager",
    "CShwDefaultEffectManager",
    "FUNCTIONALITY",
    SetupL, TestAddNullEffectL, Teardown )
    
EUNIT_TEST(
    "Next Effect Test",
    "CShwDefaultEffectManager",
    "CShwDefaultEffectManager",
    "FUNCTIONALITY",
    SetupL, TestNextEffectL, Teardown )
    
/*    
EUNIT_TEST(
    "Programmed Effect Test",
    "CShwDefaultEffectManager",
    "CShwDefaultEffectManager",
    "FUNCTIONALITY",
    SetupL, TestProgrammedEffectL, Teardown )
    
EUNIT_TEST(
    "Multi Prog Test",
    "CShwDefaultEffectManager",
    "CShwDefaultEffectManager",
    "FUNCTIONALITY",
    SetupL, TestMultiProgrammedEffectL, Teardown )
*/    

EUNIT_END_TEST_TABLE

//  END OF FILE

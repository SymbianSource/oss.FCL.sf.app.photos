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
* Description:   Test for effect for the slideshow
 *
*/



//  CLASS HEADER
#include "t_cshweffects.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>
#include <digia/eunit/EUnitDecorators.h>

#include <uiacceltk/huiEnv.h>
#include <uiacceltk/huiDisplayCoeControl.h>
#include <uiacceltk/huiImageVisual.h>
#include <uiacceltk/huiControl.h>

//  INTERNAL INCLUDES
#include "shweffect.h"
#include "shwresourceutility.h"
#include "shwslideshowenginepanic.h"

// -----------------------------------------------------------------------------
// Stub for NShwEngine::Panic -->
// -----------------------------------------------------------------------------
TBool gNShwEnginePanicCalled = EFalse;
namespace NShwEngine
	{
	extern void Panic( TShwEnginePanic aPanic )
	    {
	    gNShwEnginePanicCalled = ETrue;
	    // in test situation just do a leave
	    User::Leave( aPanic );
	    }
	}
// -----------------------------------------------------------------------------
// <-- Stub for NShwEngine::Panic
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for LocalisedNameL >>>
// -----------------------------------------------------------------------------
TPtrC gNameForEffect( 0, 0 );
_LIT( T_KZoomPanName, "ZoomAndPan" );
_LIT( T_KCrossfadeName, "Crossfade" );
HBufC* ShwResourceUtility::LocalisedNameL( TInt aResourceId )
	{
	if( aResourceId == R_SHW_EFFECT_ZOOM_AND_PAN )
		{
		gNameForEffect.Set( T_KZoomPanName() );
		return T_KZoomPanName().AllocL();
		}
	else if( aResourceId == R_SHW_EFFECT_CROSS_FADE )
		{
		gNameForEffect.Set( T_KCrossfadeName() );
		return T_KCrossfadeName().AllocL();
		}
	gNameForEffect.Set( KNullDesC );
	return NULL;		 
	}
// -----------------------------------------------------------------------------
// <<< Stub for LocalisedNameL
// -----------------------------------------------------------------------------

class CTestControl : public CHuiControl
	{
	public:
		static CTestControl* NewL( CHuiEnv& aEnv )
			{
			return new (ELeave) CTestControl( aEnv );
			}
		CTestControl( CHuiEnv& aEnv )
	        : CHuiControl( aEnv )
	    	{
	    	}	
	};

// CONSTRUCTION
T_CShwEffects* T_CShwEffects::NewLC( T_ShwEffectFactoryL* aEffectFactory )
    {
    T_CShwEffects* self = new( ELeave ) T_CShwEffects;
    CleanupStack::PushL( self );

	self->iEffectFactory = aEffectFactory;
    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CShwEffects::~T_CShwEffects()
    {
    }

// Default constructor
T_CShwEffects::T_CShwEffects()
    {
    }

// Second phase construct
void T_CShwEffects::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void T_CShwEffects::EmptyL()
	{
	}

void T_CShwEffects::SetupL()
	{
	// set name for the effect verification
	gNameForEffect.Set( KNullDesC );

	// create HUI env
	iEnv = CHuiEnv::NewL();
	// create Display
    iCoeDisplay = CHuiDisplayCoeControl::NewL( *iEnv, TRect( 0, 0, 100, 100 ) );

	// create control
	iControl = CTestControl::NewL( *iEnv );
	
	// create the visual, ownership goes to iCoeDisplay
	iVisual = CHuiImageVisual::AddNewL( *iControl );
	
	// call the factory method to construct the effect
	iEffect = (*iEffectFactory)();
	}

void T_CShwEffects::Teardown()
	{
	// delete effect
	delete iEffect;
	iEffect = NULL;

	// delete control, it deletes the visual
	delete iControl;
	iControl = NULL;

	// delete display
	delete iCoeDisplay;
	iCoeDisplay = NULL;
	
	// delete env last
	delete iEnv;
	iEnv = NULL; 
	}

void T_CShwEffects::T_ConstructorL()
	{
	// call the factory method to construct the effect
	iEffect = (*iEffectFactory)();

	EUNIT_ASSERT_DESC( iEffect, "Effect is constructed");
	// EUnit checks that memory is in balance
	// teardown deletes the object
	}
    
void T_CShwEffects::T_PrepareViewLL()
	{
	// initialize with null lists but proper screen
	iEffect->InitializeL(
	    NULL, 
	    NULL, 
	    NULL, 
	    TSize( 320, 240 ) );
	// call prepare view with proper size
	TSize size = iEffect->PrepareViewL( iVisual, TSize( 320, 240 ) );
	// verify that the thumbnail size is not 0,0
	EUNIT_ASSERT_GREATER_DESC( 
	    size.iWidth, 0, "thumbnail size is set");
	EUNIT_ASSERT_GREATER_DESC( 
	    size.iHeight, 0, "thumbnail size is set");

	// call prepare view with unknown size
	size = iEffect->PrepareViewL( iVisual, TSize( KErrNotFound, KErrNotFound ) );
	// verify that the thumbnail size is not 0,0
	EUNIT_ASSERT_GREATER_DESC( 
	    size.iWidth, 0, "thumbnail size is set");
	EUNIT_ASSERT_GREATER_DESC( 
	    size.iHeight, 0, "thumbnail size is set");
	}
    
void T_CShwEffects::T_EnterViewLL()
	{
	MGlxLayout* layout = iEffect->EnterViewL( iVisual, 123, 345 );
	EUNIT_ASSERT_DESC( layout, "Layout is not NULL");
	}
    
void T_CShwEffects::T_ExitViewL()
	{
	iEffect->ExitView( iVisual );
	// nothing to really verify what the effect is supposed to do
	EUNIT_ASSERT_DESC( iEffect, "Effect is constructed");
	}
    
void T_CShwEffects::T_EnterTransitionLL()
	{
	MGlxLayout* layout = iEffect->EnterTransitionL( iVisual, 321 );
	EUNIT_ASSERT_DESC( layout, "Layout is not NULL");
	}

void T_CShwEffects::T_ExitTransitionL()
	{
	iEffect->ExitTransition( iVisual );
	// nothing to really verify what the effect is supposed to do
	EUNIT_ASSERT_DESC( iEffect, "Effect is constructed");
	}

void T_CShwEffects::T_PauseLL()
	{
	iEffect->PauseL();
	// nothing to really verify what the effect is supposed to do
	EUNIT_ASSERT_DESC( iEffect, "Effect is constructed");
	}

void T_CShwEffects::T_ResumeL()
	{
	iEffect->Resume();
	// nothing to really verify what the effect is supposed to do
	EUNIT_ASSERT_DESC( iEffect, "Effect is constructed");
	}
    
void T_CShwEffects::T_EffectInfoL()
	{
	// get effect info
	TShwEffectInfo info = iEffect->EffectInfo();
	// assert that the info contains some values
	EUNIT_ASSERT_EQUALS_DESC( info.iName, gNameForEffect, "Info has correct name");
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwEffects,
    "Test suite for MShwEffect",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Constructor test",
    "MShwEffect",
    "Constructor test",
    "FUNCTIONALITY",
    EmptyL, T_ConstructorL, Teardown )	// need teardown since alloc test

EUNIT_TEST(
    "PrepareViewL",
    "CShwCrossFadeEffect",
    "PrepareViewL",
    "FUNCTIONALITY",
    SetupL, T_PrepareViewLL, Teardown)
    
EUNIT_TEST(
    "EnterViewL",
    "CShwCrossFadeEffect",
    "EnterViewL",
    "FUNCTIONALITY",
    SetupL, T_EnterViewLL, Teardown)
    
EUNIT_TEST(
    "ExitView",
    "CShwCrossFadeEffect",
    "ExitView",
    "FUNCTIONALITY",
    SetupL, T_ExitViewL, Teardown)
    
EUNIT_TEST(
    "EnterTransitionL",
    "CShwCrossFadeEffect",
    "EnterTransitionL",
    "FUNCTIONALITY",
    SetupL, T_EnterTransitionLL, Teardown)
    
EUNIT_TEST(
    "ExitTransition",
    "CShwCrossFadeEffect",
    "ExitTransition",
    "FUNCTIONALITY",
    SetupL, T_ExitTransitionL, Teardown)
    
EUNIT_TEST(
    "PauseL",
    "CShwCrossFadeEffect",
    "PauseL",
    "FUNCTIONALITY",
    SetupL, T_PauseLL, Teardown)
    
EUNIT_TEST(
    "Resume",
    "CShwCrossFadeEffect",
    "Resume",
    "FUNCTIONALITY",
    SetupL, T_ResumeL, Teardown)
    
EUNIT_TEST(
    "EffectInfo",
    "CShwCrossFadeEffect",
    "EffectInfo",
    "FUNCTIONALITY",
    SetupL, T_EffectInfoL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

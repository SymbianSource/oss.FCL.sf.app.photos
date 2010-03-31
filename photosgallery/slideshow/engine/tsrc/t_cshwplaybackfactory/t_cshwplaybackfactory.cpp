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
* Description:    Test suite for CShwPlaybackFactory
 *
*/



//  CLASS HEADER
#include "t_cshwplaybackfactory.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>
#include <digia/eunit/EUnitDecorators.h>
#include <glxthumbnailcontext.h>
#include <glxsetvaluelayout.h>

//  INTERNAL INCLUDES
#include "shwplaybackfactory.h"
#include "shwslideshowenginepanic.h"
#include "shwcrossfadeeffect.h"
#include "shwzoomandpaneffect.h"
#include "shwconstants.h"

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

TGlxSetValueLayout gStubLayout( TGlxLayoutInfo::EPosition );

// -----------------------------------------------------------------------------
// Stub for CShwCrossFadeEffect -->
// -----------------------------------------------------------------------------
inline CShwCrossFadeEffect::CShwCrossFadeEffect()
	{
	}
CShwCrossFadeEffect* CShwCrossFadeEffect::NewLC()
	{
	CShwCrossFadeEffect* self = new (ELeave) CShwCrossFadeEffect;
	CleanupStack::PushL( self );
	return self;
	}
CShwCrossFadeEffect::~CShwCrossFadeEffect()
	{
	}
MShwEffect* CShwCrossFadeEffect::CloneLC()
	{
	return NewLC();
	}
void CShwCrossFadeEffect::InitializeL( 
	CHuiEnv* /*aHuiEnv*/, MGlxVisualList* /*aVisualList*/,
    MGlxMediaList* /*aMediaList*/, TSize /*aScreenSize*/ )
	{
	}
TSize CShwCrossFadeEffect::PrepareViewL( CHuiVisual* /*aVisual*/, TSize /*aSize*/ )
	{
	return TSize( 0, 0 );
	}
MGlxLayout* CShwCrossFadeEffect::EnterViewL( 
	CHuiVisual* /*aVisual*/, TInt /*aDuration*/, TInt /*aDuration2*/ )
	{
	return &gStubLayout;
	}
void CShwCrossFadeEffect::ExitView( CHuiVisual* /*aVisual*/ )
	{
	}
MGlxLayout* CShwCrossFadeEffect::EnterTransitionL( 
	CHuiVisual* /*aVisual*/, TInt /*aDuration*/ )
	{
	return &gStubLayout;
	}
void CShwCrossFadeEffect::ExitTransition( CHuiVisual* /*aVisual*/ )
	{
	}
void CShwCrossFadeEffect::PauseL()
	{
	}
void CShwCrossFadeEffect::Resume()
	{
	}
// the effect info for zoom and pan
TShwEffectInfo gCrossFadeEffectInfo;
TShwEffectInfo CShwCrossFadeEffect::EffectInfo()
	{
	return gCrossFadeEffectInfo;
	}

// -----------------------------------------------------------------------------
// <-- Stub for CShwCrossFadeEffect
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for CShwZoomAndPanEffect -->
// -----------------------------------------------------------------------------
inline CShwZoomAndPanEffect::CShwZoomAndPanEffect()
	{
	}
CShwZoomAndPanEffect* CShwZoomAndPanEffect::NewLC()
	{
	CShwZoomAndPanEffect* self = new (ELeave) CShwZoomAndPanEffect;
	CleanupStack::PushL( self );
	return self;
	}
CShwZoomAndPanEffect::~CShwZoomAndPanEffect()
	{
	}
MShwEffect* CShwZoomAndPanEffect::CloneLC()
	{
	return NewLC();
	}
void CShwZoomAndPanEffect::InitializeL( 
	CHuiEnv* /*aHuiEnv*/, MGlxVisualList* /*aVisualList*/,
    MGlxMediaList* /*aMediaList*/, TSize /*aScreenSize*/ )
	{
	}
TSize CShwZoomAndPanEffect::PrepareViewL( CHuiVisual* /*aVisual*/, TSize /*aSize*/ )
	{
	return TSize( 0, 0 );
	}
MGlxLayout* CShwZoomAndPanEffect::EnterViewL(
	CHuiVisual* /*aVisual*/, TInt /*aDuration*/, TInt /*aDuration2*/ )
	{
	return &gStubLayout;
	}
void CShwZoomAndPanEffect::ExitView( CHuiVisual* /*aVisual*/ )
	{
	}
MGlxLayout* CShwZoomAndPanEffect::EnterTransitionL(
	CHuiVisual* /*aVisual*/, TInt /*aDuration*/ )
	{
	return &gStubLayout;
	}
void CShwZoomAndPanEffect::ExitTransition( CHuiVisual* /*aVisual*/ )
	{
	}
void CShwZoomAndPanEffect::PauseL()
	{
	}
void CShwZoomAndPanEffect::Resume()
	{
	}
// the effect info for zoom and pan
TShwEffectInfo gZoomAndPanEffectInfo;
TShwEffectInfo CShwZoomAndPanEffect::EffectInfo()
	{
	return gZoomAndPanEffectInfo;
	}

// -----------------------------------------------------------------------------
// <-- Stub for CShwZoomAndPanEffect
// -----------------------------------------------------------------------------

// CONSTRUCTION
T_CShwPlaybackFactory* T_CShwPlaybackFactory::NewL()
    {
    T_CShwPlaybackFactory* self = T_CShwPlaybackFactory::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

T_CShwPlaybackFactory* T_CShwPlaybackFactory::NewLC()
    {
    T_CShwPlaybackFactory* self = new( ELeave ) T_CShwPlaybackFactory();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CShwPlaybackFactory::~T_CShwPlaybackFactory()
    {
    }

// Default constructor
T_CShwPlaybackFactory::T_CShwPlaybackFactory()
    {
    }

// Second phase construct
void T_CShwPlaybackFactory::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void T_CShwPlaybackFactory::Empty()
	{
	}

void T_CShwPlaybackFactory::SetupL()
	{
	// create the stubs
	iStubVisuallist = new( ELeave ) TMGlxVisualList_Adapter( this );
	iStubMedialist = new( ELeave ) TMGlxMediaList_Stub( this );

    // fix the effect infos
	gCrossFadeEffectInfo.iId.iPluginUid = NShwSlideshow::KDefaultEffectPluginUid;
	gCrossFadeEffectInfo.iId.iIndex 	= NShwSlideshow::KEffectUidXFadeNormal;
    gZoomAndPanEffectInfo.iId.iPluginUid = NShwSlideshow::KDefaultEffectPluginUid;
    gZoomAndPanEffectInfo.iId.iIndex     = NShwSlideshow::KEffectUidZoomAndPan;

	/// @todo add an own test case where the music observer is also given 
	/// so that the music control is constructed
	iCShwPlaybackFactory = 
		CShwPlaybackFactory::NewL( 
			iHuiEnv, iStubVisuallist, iStubMedialist, *this, TSize( 100, 100 ) );
	}

void T_CShwPlaybackFactory::Teardown()
	{
	delete iCShwPlaybackFactory;
	iCShwPlaybackFactory = NULL; 
	delete iStubVisuallist;
	iStubVisuallist = NULL;
	delete iStubMedialist;
	iStubMedialist = NULL;
	}

TInt gCounter = 0;

void T_CShwPlaybackFactory::T_ContructL()
	{
	// trap the call as it will leave, dont trap oom as this is alloc test
	EUNIT_TRAP_EXCEPT_ALLOC_D( err,
	    {
    	iCShwPlaybackFactory = 
    		CShwPlaybackFactory::NewL( 
    			iHuiEnv, iStubVisuallist, iStubMedialist, *this, TSize( 100, 100 ) );
	    } );
	EUNIT_ASSERT_EQUALS_DESC( KErrArgument, err, "wrong effect tried to set" );
	EUNIT_ASSERT_DESC( !iCShwPlaybackFactory, "playback factory not created" );
	}

void T_CShwPlaybackFactory::T_ContructValidEffectL()
    {
    EUNIT_PRINT( _L("T_ContructValidEffectL") );
    // call setup to test the succesfull case
	SetupL();
	
	EUNIT_ASSERT_DESC( iCShwPlaybackFactory, "Test that object created" );
    }

void T_CShwPlaybackFactory::T_EventObserversL()
	{
	// get observers
	RPointerArray< MShwEventObserver > obs = 
		iCShwPlaybackFactory->EventObservers();
	// check the amount
	EUNIT_ASSERT_EQUALS_DESC(
		obs.Count(),
		3,
		"3 observers returned, music is off by default");
	}

void T_CShwPlaybackFactory::T_EventPublishersL(  )
	{
	// get publishers
	RPointerArray< MShwEventPublisher > obs = 
		iCShwPlaybackFactory->EventPublishers();
	// check the amount
	EUNIT_ASSERT_EQUALS_DESC(
		obs.Count(),
		3,
		"3 publishers returned, music is off by default");
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwPlaybackFactory,
    "CShwPlaybackFactory test suite",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Constructor invalid effects",
    "CShwPlaybackFactory",
    "Constructor test",
    "FUNCTIONALITY",
    Empty, T_ContructL, Teardown )

EUNIT_ALLOC_TEST(
    "Constructor valid effects",
    "CShwPlaybackFactory",
    "Constructor test",
    "FUNCTIONALITY",
    Empty, T_ContructValidEffectL, Teardown )

EUNIT_ALLOC_TEST(
    "EventObservers",
    "CShwPlaybackFactory",
    "EventObservers",
    "FUNCTIONALITY",
    SetupL, T_EventObserversL, Teardown)
    
EUNIT_ALLOC_TEST(
    "EventPublishers",
    "CShwPlaybackFactory",
    "EventPublishers",
    "FUNCTIONALITY",
    SetupL, T_EventPublishersL, Teardown)  

EUNIT_END_TEST_TABLE

//  END OF FILE

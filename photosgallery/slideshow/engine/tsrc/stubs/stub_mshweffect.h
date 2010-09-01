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
* Description:    Test stub for MShwEffect
 *
*/




#include "shweffect.h"

// -----------------------------------------------------------------------------
// Stub for MShwEffect -->
// -----------------------------------------------------------------------------
class T_MShwTestEffect : public MShwEffect
    {
    public:
        T_MShwTestEffect( TInt aIndex );
        ~T_MShwTestEffect();

    public: // from MShwEffect

        MShwEffect* CloneLC();
        void InitializeL( 
            CAlfEnv* aAlfEnv, MGlxVisualList* aVisualList,
            MGlxMediaList* aMediaList, TSize aScreenSize );
        TSize PrepareViewL( CAlfVisual* aVisual, TSize aSize );
        MGlxLayout* EnterViewL( CAlfVisual* aVisual, TInt aDuration, TInt aDuration2 );
        void ExitView( CAlfVisual* aVisual );
        MGlxLayout* EnterTransitionL( CAlfVisual* aVisual, TInt aDuration );
        void ExitTransition( CAlfVisual* aVisual );
        void PauseL();
        void Resume();
        TShwEffectInfo EffectInfo();

    public:
        TInt iIndex;
        enum TState 
            {
            EConstruct, 
            EPrepareView,
            EEnterView,
            EExitView,
            EEnterTransition,
            EExitTransition
            } iState;	// state flag to assert prptocol
        CAlfVisual* iVisual;	// to track the visual
    };

T_MShwTestEffect::T_MShwTestEffect( TInt aIndex )
    : iIndex( aIndex ), iState( EConstruct ), iVisual( NULL )
    {
    }
T_MShwTestEffect::~T_MShwTestEffect()
    {
    }
MShwEffect* T_MShwTestEffect::CloneLC()
    {
    T_MShwTestEffect* effect = new( ELeave ) T_MShwTestEffect( iIndex );
    CleanupStack::PushL( effect );
    return effect;
    }

void T_MShwTestEffect::InitializeL( 
    CAlfEnv* /*aAlfEnv*/, MGlxVisualList* /*aVisualList*/,
    MGlxMediaList* /*aMediaList*/, TSize /*aScreenSize*/ )
    {
    }
// this flag defines if the test expects the effects to be called completely
// like Prepare -> Enter view -> Enter trans -> Prepare
// if set to EFalse, then also Prepare -> Enter view -> Exit View -> Prepare
// is allowed
TBool gStrictEffectOrder = ETrue;
TSize T_MShwTestEffect::PrepareViewL( CAlfVisual* aVisual, TSize aSize )
    {
    if( gStrictEffectOrder )
        {
        EUNIT_ASSERT_DESC( 
            EConstruct == iState || EExitTransition == iState,
            "state needs to be construct or exit transition" );
        }
    iState = EPrepareView;	// set state
    EUNIT_PRINT( _L("effect %d; PrepareViewL visual=%d"), iIndex, (TInt)aVisual );
    // remember the visual
    iVisual = aVisual;
    // return the given size
    return aSize;
    }

MGlxLayout* T_MShwTestEffect::EnterViewL( 
    CAlfVisual* aVisual, TInt /*aDuration*/, TInt /*aDuration2*/ )
    {
    if( gStrictEffectOrder )
        {
        EUNIT_ASSERT_DESC( EPrepareView == iState, "state needs to be prepareview" );
        }
    else
        {
        EUNIT_ASSERT_DESC( 
            EPrepareView == iState ||
            EEnterView == iState, "state needs to be prepare or enterview" );
        }
    iState = EEnterView;	// set state
    EUNIT_PRINT( _L("effect %d; EnterViewL visual=%d"), iIndex, (TInt)aVisual );
    EUNIT_ASSERT_DESC( aVisual == iVisual, "Visual should be same as for PrepareViewL" );
    return NULL;
    }

void T_MShwTestEffect::ExitView( CAlfVisual* aVisual )
    {
    EUNIT_ASSERT_DESC( EEnterView == iState, "state needs to be enterview" );
    iState = EExitView;	// set state
    EUNIT_PRINT( _L("effect %d; ExitView visual=%d"), iIndex, (TInt)aVisual );
    EUNIT_ASSERT_DESC( aVisual == iVisual, "Visual should be same as for PrepareViewL" );
    }

MGlxLayout* T_MShwTestEffect::EnterTransitionL( CAlfVisual* aVisual, TInt /*aDuration*/ )
    {
    EUNIT_ASSERT_DESC( EExitView == iState, "state needs to be exitview" );
    iState = EEnterTransition;	// set state
    EUNIT_PRINT( _L("effect %d; EnterTransitionL visual=%d"), iIndex, (TInt)aVisual );
    EUNIT_ASSERT_DESC( aVisual == iVisual, "Visual should be same as for PrepareViewL" );
    return NULL;
    }

void T_MShwTestEffect::ExitTransition( CAlfVisual* aVisual )
    {
    EUNIT_ASSERT_DESC( EEnterTransition == iState, "state needs to be entertransition" );
    iState = EExitTransition;	// set state
    EUNIT_PRINT( _L("effect %d; ExitTransition visual=%d"), iIndex, (TInt)aVisual );
    EUNIT_ASSERT_DESC( aVisual == iVisual, "Visual should be same as for PrepareViewL" );
    }

void T_MShwTestEffect::PauseL()
    {
    }

void T_MShwTestEffect::Resume()
    {
    }

_LIT( KTestEffectName, "EffectName" );
TShwEffectInfo T_MShwTestEffect::EffectInfo()
    {
    TShwEffectInfo info;
    info.iName = KTestEffectName;
    info.iId.iIndex = iIndex;
    return info;
    }

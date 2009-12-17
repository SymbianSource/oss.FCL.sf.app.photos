/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Stub file for managing effect in slideshow
 *
*/




//  CLASS HEADER
#include "tmshweffectmanager_adapter.h"

//  EXTERNAL INCLUDES
//  INTERNAL INCLUDES
#include <EUnitMacros.h>

// declare the enums to be printable as TInts
EUNIT_DECLARE_PRINTABLE_AS_TINT( MShwEffectManager_Observer::TMShwEffectManagerMethodId )

// CONSTRUCTION

TMShwEffectManager_Adapter::TMShwEffectManager_Adapter(
	MShwEffectManager_Observer* aObserver )
	: iViewDuration( 0 ),
	iTransitionDuration( 0 ),
    iPrepareVisual( NULL ),
    iViewVisual( NULL ),
    iTransitionVisual( NULL ),
	iMShwEffectManager_Observer( aObserver ), 
	iMShwEffectManager( NULL )
	{
	}

void TMShwEffectManager_Adapter::SetAdaptee( MShwEffectManager* aAdaptee )
	{
	iMShwEffectManager = aAdaptee;
	}

//  METHODS
void TMShwEffectManager_Adapter::AddEffectL( MShwEffect* aEffect )
	{
	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_void_AdoptEffectsL_TArrayMShwEffect_p );

	// allocate to cause an alloc leave
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;

	// call the actual method
	if ( iMShwEffectManager )
		{
		iMShwEffectManager->AddEffectL( aEffect );
		}
	}
    
MShwEffect* TMShwEffectManager_Adapter::CurrentEffect()
    {
    // inform the observer
    iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
    	MShwEffectManager_Observer::E_MShwEffect_p_CurrentEffect );
    // call the actual method
    if ( iMShwEffectManager )
        {
        return iMShwEffectManager->CurrentEffect();
        }
    return this;
    }
   
MShwEffect* TMShwEffectManager_Adapter::Effect( TInt aDirection )
	{
	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_MShwEffect_p_NextEffect );
	// call the actual method
	if ( iMShwEffectManager )
		{
		return iMShwEffectManager->Effect( aDirection );
		}
	return this;
	}
    
void TMShwEffectManager_Adapter::ProceedToEffect( TInt aDirection )
	{
	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_void_ProceedToNextEffect );
	// call the actual method
	if ( iMShwEffectManager )
		{
		iMShwEffectManager->ProceedToEffect( aDirection );
		}
	}

void TMShwEffectManager_Adapter::SetEffectOrder(
	MShwEffectManager::TShwEffectOrder /*aOrder*/ )
    {
    }
    
MShwEffectManager::TShwEffectOrder TMShwEffectManager_Adapter::EffectOrder()
    {
    return EEffectOrderProgrammed;
    }
    
void TMShwEffectManager_Adapter::SetProgrammedEffects(
	RArray< TShwEffectInfo >& /*aEffects*/ )
    {
    }
    
TInt TMShwEffectManager_Adapter::ProgrammedEffects(
	RArray< MShwEffect* >& /*aEffects*/ )
    {
    return KErrNone;
    }
	
void TMShwEffectManager_Adapter::SetDefaultEffectL( TShwEffectInfo /*aInfo*/ )
	{
	}

void TMShwEffectManager_Adapter::GetActiveEffectsL( RPointerArray< MShwEffect >& /*aEffects*/ )
	{
	}
	
/// MShwEffect part
MShwEffect* TMShwEffectManager_Adapter::CloneLC()
	{
	return NULL;
	}

void TMShwEffectManager_Adapter::InitializeL( 
	CHuiEnv* /*aHuiEnv*/,
    MGlxVisualList* /*aVisualList*/,
    MGlxMediaList* /*aMediaList*/,
    TSize /*aScreenSize*/ )
    {
	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_MGlxLayout_InitializeL );

	// allocate to cause an alloc leave in OOM tests
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;
    }

TSize TMShwEffectManager_Adapter::PrepareViewL( CHuiVisual* aVisual, TSize /*aSize*/ )
	{
	// store the visual
    iPrepareVisual = aVisual;

	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_MGlxLayout_PrepareViewL );

	// allocate to cause an alloc leave in OOM tests
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;

	return TSize( 0, 0 );
	}

MGlxLayout* TMShwEffectManager_Adapter::EnterViewL( 
	CHuiVisual* aVisual, TInt aDuration, TInt /*aDuration2*/ )
	{
	iViewDuration = aDuration;
	// store the visual
    iViewVisual = aVisual;

	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_MGlxLayout_EnterViewL_TInt );

	// allocate to cause an alloc leave
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;

	return NULL;
	}

void TMShwEffectManager_Adapter::ExitView( CHuiVisual* aVisual )
	{
	// store the visual
    iViewVisual = aVisual;
	
	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_void_ExitView );
	}

MGlxLayout* TMShwEffectManager_Adapter::EnterTransitionL(
	CHuiVisual* aVisual, TInt aDuration )
	{
	iTransitionDuration = aDuration;
	// store the visual
    iTransitionVisual = aVisual;

	// inform the observer
	iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
		MShwEffectManager_Observer::E_MGlxLayout_EnterTransitionL_TInt );

	// allocate to cause an alloc leave
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;
	
	return NULL;
	}

void TMShwEffectManager_Adapter::ExitTransition( CHuiVisual* aVisual )
	{
	// store the visual
    iTransitionVisual = aVisual;

    // inform the observer
    iMShwEffectManager_Observer->MShwEffectManager_MethodCalled( 
    	MShwEffectManager_Observer::E_void_ExitTransition );
	}

void TMShwEffectManager_Adapter::PauseL()
	{
	// allocate to cause an alloc leave
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;
	}

void TMShwEffectManager_Adapter::Resume()
	{
	}

TShwEffectInfo TMShwEffectManager_Adapter::EffectInfo()
	{
	TShwEffectInfo info;
	info.iName = KNullDesC;
	return info;
	}

//  END OF FILE

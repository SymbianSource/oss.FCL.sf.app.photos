/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Default effect order manager
 *
*/




//  Include Files
#include "shwdefaulteffectmanager.h"

#include <e32math.h>

#include <glxlog.h>
#include <glxtracer.h>

#include "shweffect.h"
#include "shwconstants.h"
#include "shwslideshowenginepanic.h"

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwDefaultEffectManager::CShwDefaultEffectManager()
	: iEffectOrder( MShwEffectManager::EEffectOrderLinear )	// default order
	{
	TRACER("CShwDefaultEffectManager::CShwDefaultEffectManager()");
	GLX_LOG_INFO( "CShwDefaultEffectManager::CShwDefaultEffectManager()" );
	// no implementation required
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwDefaultEffectManager* CShwDefaultEffectManager::NewL()
	{
	TRACER("CShwDefaultEffectManager::NewL()");
	GLX_LOG_INFO( "CShwDefaultEffectManager::NewL" );
	CShwDefaultEffectManager* self = new( ELeave ) CShwDefaultEffectManager;
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwDefaultEffectManager::~CShwDefaultEffectManager()
	{
	TRACER("CShwDefaultEffectManager::~CShwDefaultEffectManager");
	GLX_LOG_INFO( "CShwDefaultEffectManager::~CShwDefaultEffectManager" );

	// Delete the effect objects
	ResetAndDestroyEffects( iAvailableEffects );
	iAvailableEffects.Close();
	
	// Close the "working" effects array
	// Note that iEffects just references the same elements as iAvailableEffects
	// so don't delete them again.
	iEffects.Close();
	}

// -----------------------------------------------------------------------------
// AddEffectL.
// -----------------------------------------------------------------------------
void CShwDefaultEffectManager::AddEffectL( MShwEffect* aEffect )
	{
	TRACER("CShwDefaultEffectManager::AddEffectL");
    GLX_LOG_INFO( "CShwDefaultEffectManager::AddEffectL" );
    // dont add NULLs to the array
    if( aEffect )
    	{
		iAvailableEffects.AppendL( aEffect );
    	}
	}

// -----------------------------------------------------------------------------
// CurrentEffect.
// -----------------------------------------------------------------------------
MShwEffect* CShwDefaultEffectManager::CurrentEffect()
	{
	TRACER("CShwDefaultEffectManager::CurrentEffect");
	GLX_LOG_INFO( "CShwDefaultEffectManager::CurrentEffect" );
	// check that we have something in iEffects
	__ASSERT_DEBUG( iEffects.Count() > 0 && iEffects.Count() > iCurrentEffect,
		NShwEngine::Panic( NShwEngine::EIncorrectEffectIndex ) );
	
	return iEffects[ iCurrentEffect ];
	}

// -----------------------------------------------------------------------------
// Effect
// -----------------------------------------------------------------------------
MShwEffect* CShwDefaultEffectManager::Effect( TInt aDirection )
	{
	TRACER("CShwDefaultEffectManager::Effect");
	GLX_LOG_INFO1( "CShwDefaultEffectManager::Effect, direction %d",
		aDirection );
	// check that we have something in iEffects
	__ASSERT_DEBUG( iEffects.Count() > 0 && iEffects.Count() > iCurrentEffect,
		NShwEngine::Panic( NShwEngine::EIncorrectEffectIndex ) );

	TInt nextEffectIndex = CalculateNextEffectIndex( aDirection );
	return iEffects[ nextEffectIndex ];
	}

// -----------------------------------------------------------------------------
// ProceedToEffect.
// -----------------------------------------------------------------------------
void CShwDefaultEffectManager::ProceedToEffect( TInt aDirection )
	{
	TRACER("CShwDefaultEffectManager::ProceedToEffect");
	GLX_LOG_INFO1( "CShwDefaultEffectManager::ProceedToEffect,\
		direction %d", aDirection );
	// calculate new effects
	CalculateEffects( aDirection );
	}

// -----------------------------------------------------------------------------
// SetEffectOrder.
// -----------------------------------------------------------------------------
void CShwDefaultEffectManager::SetEffectOrder(MShwEffectManager::TShwEffectOrder
	aOrder)
    {
    TRACER("CShwDefaultEffectManager::SetEffectOrder");
    GLX_LOG_INFO("CShwDefaultEffectManager::SetEffectOrder");
    iEffectOrder = aOrder;
    /// @todo implement the rest for increment 7
    }

// -----------------------------------------------------------------------------
// EffectOrder.
// -----------------------------------------------------------------------------
MShwEffectManager::TShwEffectOrder CShwDefaultEffectManager::EffectOrder()
    {
    TRACER("CShwDefaultEffectManager::EffectOrder");
    GLX_LOG_INFO("CShwDefaultEffectManager::EffectOrder");
    return iEffectOrder;
    }

// -----------------------------------------------------------------------------
// SetProgrammedEffects.
// -----------------------------------------------------------------------------  
void CShwDefaultEffectManager::SetProgrammedEffects( 
	RArray<TShwEffectInfo>& /*aEffects*/ )
    {
    TRACER("CShwDefaultEffectManager::SetProgrammedEffects");
    GLX_LOG_INFO("CShwDefaultEffectManager::SetProgrammedEffects");
    /// @todo implement for increment 7
    }

// -----------------------------------------------------------------------------
// SetDefaultEffect.
// -----------------------------------------------------------------------------
void CShwDefaultEffectManager::SetDefaultEffectL( TShwEffectInfo aInfo )
    {
    TRACER("CShwDefaultEffectManager::SetDefaultEffectL");
	GLX_LOG_INFO( "CShwDefaultEffectManager::SetDefaultEffectL" );

	// Loop through the effects to find the one with matching info
	TInt index = iAvailableEffects.Count();
	MShwEffect* effect = NULL;
	while( index-- > 0 )
		{
		// get effect
		effect = iAvailableEffects[ index ];
		// get its info
		TShwEffectInfo info = effect->EffectInfo();
		// check if we got a match
		if( info == aInfo )
			{
			break;
			}
		}
	// if no effect or no match
	if( !effect || index < 0 )
		{
		// effect not found
    	User::Leave( KErrArgument );
		}

	// Need to clone the effect as in current release
	// we have once single effect at a time and one effect instance can only
	// handle one visual
	/// @todo fix this so that once the effect order is solved, the effects
	/// are cloned as needed
	MShwEffect* clone = effect->CloneLC();
	iAvailableEffects.AppendL( clone );
	
    // This will cause a code scanner warning, but it is not possible to do 
    // CleanupStack::Pop(clone) because the pointer pushed 
    // onto the cleanup stack was either of class CShwZoomAndPanEffect
    // or a CShwCrossFadeEffect and the object 'clone' is of
    // class MShwEffect
    CleanupStack::Pop(); //clone

    iEffects.Append( effect );
    iEffects.Append( clone );

	// calculate the effects    
    CalculateEffects( 1 );
    }

// -----------------------------------------------------------------------------
// GetActiveEffectsL
// -----------------------------------------------------------------------------
void CShwDefaultEffectManager::GetActiveEffectsL( 
	RPointerArray< MShwEffect >& aEffects )
    {
    TRACER("CShwDefaultEffectManager::GetActiveEffectsL");
    GLX_LOG_INFO( "CShwDefaultEffectManager::GetActiveEffectsL" );
    // Retrieve each effect
    for( TInt i = 0; i < iEffects.Count(); ++i )
        {
        // append the effect pointer to the given array
        aEffects.AppendL( iEffects[i] );  
        }
    }

// -----------------------------------------------------------------------------
// CalculateEffects.
// -----------------------------------------------------------------------------
void CShwDefaultEffectManager::CalculateEffects( TInt aDirection )
    {
    TRACER("CShwDefaultEffectManager::CalculateEffects");
	GLX_LOG_INFO1( "CShwDefaultEffectManager::CalculateEffects,\
		direction %d", aDirection );
    // This method updates both the current and next indices for the given
    // direction.  If we're proceeding forwards, temporarily store the
    // "next" index to set it as "current" after the new "next" value has 
    // been determined (as the next value is determined from the current value).
    if ( aDirection > 0 )
    	{
    	// Next effect is "forward"
    	iCurrentEffect = iNextEffect;
    	}
    else
    	{
    	// moving "backwards"
    	iNextEffect = iCurrentEffect;
    	}
    	
    if( iEffectOrder == MShwEffectManager::EEffectOrderLinear )
        {
        // As iAvailableEffects is used to store the effects it can be 
        // walked directly and the order will always be correct
        //
        TInt nextEffect = CalculateNextEffectIndex( aDirection );
        if ( aDirection > 0 )
        	{
        	// moving forwards
			iNextEffect = nextEffect;
        	}
        else
        	{
        	// moving backwards
        	iCurrentEffect = nextEffect;
        	}
		}
    }

// -----------------------------------------------------------------------------
// ResetAndDestroyEffects.
// -----------------------------------------------------------------------------
void CShwDefaultEffectManager::ResetAndDestroyEffects(RArray< MShwEffect* >&
	aArray)
    {
    TRACER("CShwDefaultEffectManager::ResetAndDestroyEffects");
    // get the count
    TInt count = aArray.Count();
	GLX_LOG_INFO1( 
		"CShwDefaultEffectManager::ResetAndDestroyEffects %d", count );

    // Delete the effect objects
	for( TInt i = 0; i < count; i++ )
		{
		// call the destructor
		delete aArray[ i ];
		}
	aArray.Reset();
    }
    
// -----------------------------------------------------------------------------
// CalculateNextEffectIndex.
// -----------------------------------------------------------------------------
TInt CShwDefaultEffectManager::CalculateNextEffectIndex( TInt aDirection )
	{
	TRACER("CShwDefaultEffectManager::CalculateNextEffectIndex");
	GLX_LOG_INFO1( 
	        "CShwDefaultEffectManager::CalculateNextEffectIndex %d", aDirection);
	TInt effectsCount = iEffects.Count();	
	// Guard against direction exceeding the number of effects
	TInt direction = aDirection % effectsCount;	
	TInt nextEffect = iCurrentEffect + direction;
	
	// ensure index is still in bounds
	if ( nextEffect < 0 )
		{
		// -ve index: wrap around to the last element
		nextEffect = effectsCount - 1;
		}
	else if ( nextEffect >= effectsCount )
		{
		// passed the end so return to the start.
		nextEffect = 0;
		}
	return nextEffect;
	}

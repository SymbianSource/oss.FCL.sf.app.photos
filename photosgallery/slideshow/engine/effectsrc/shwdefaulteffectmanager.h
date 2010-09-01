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
* Description:    Default effect manager
 *
*/




#ifndef __CSHWDEFAULTEFFECTMANAGER_H__
#define __CSHWDEFAULTEFFECTMANAGER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "shweffectmanager.h"
#include "shweffectinfo.h"

// FORWARD DECLARATIONS
class MShwEffect;

// CLASS DECLARATION

/**
 *  CShwDefaultEffectManager
 */
NONSHARABLE_CLASS( CShwDefaultEffectManager )  : public CBase, public MShwEffectManager
	{
	public: // Constructors and destructor

		/**
		 * Constructor.
		 * @param aVisualList, the list that this view control manages
		 */
		static CShwDefaultEffectManager* NewL();

		/**
		 * Destructor.
		 */
		~CShwDefaultEffectManager();

	private:

		/**
		 * Constructor
		 */
		CShwDefaultEffectManager();

	public: // from MShwEffectManager

		/// @ref MShwEffectManager::AddEffectL
		void AddEffectL( MShwEffect* aEffect );
		/// @ref MShwEffectManager::CurrentEffect
		MShwEffect* CurrentEffect();
		/// @ref MShwEffectManager::Effect
		MShwEffect* Effect( TInt aDirection);
		/// @ref MShwEffectManager::ProceedToEffect
		void ProceedToEffect( TInt aDirection );
		/// @ref MShwEffectManager::SetEffectOrder
		void SetEffectOrder(TShwEffectOrder aOrder);
		/// @ref MShwEffectManager::EffectOrder
		TShwEffectOrder EffectOrder();
		/** @todo: confirm how to programme effects */
		/// @ref MShwEffectManager::SetProgrammedEffects
        void SetProgrammedEffects( RArray< TShwEffectInfo >& aEffects );
		/// @ref MShwEffectManager::SetDefaultEffect
		void SetDefaultEffectL(TShwEffectInfo aInfo);
		/// @ref MShwEffectManager::GetActiveEffectsL
		void GetActiveEffectsL( RPointerArray< MShwEffect >& aEffects );
		
	private: // Implementation
		
		// Default to "forwards" direction. Use negative value for "backwards"
		void CalculateEffects( TInt aDirection = 1 );
        void ResetAndDestroyEffects(RArray< MShwEffect* >& aArray);
        TInt CalculateNextEffectIndex( TInt aDirection );
		
	private: // Data

		/// Own: The effects in use
		RArray< MShwEffect* > iEffects;
		/// Own: The entire selection of effects
		RArray< MShwEffect* > iAvailableEffects;
		/// Own: current effect number
		TInt iCurrentEffect;
		/// Own: next effect number
		TInt iNextEffect;
		/// Own: the order that the effects are played in
		MShwEffectManager::TShwEffectOrder iEffectOrder;

	};

#endif // __CSHWDEFAULTEFFECTMANAGER_H__

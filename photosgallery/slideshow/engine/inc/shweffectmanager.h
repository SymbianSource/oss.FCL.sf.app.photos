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
* Description:    The effect manager interface
 *
*/




#ifndef __MSHWEFFECTMANAGER_H__
#define __MSHWEFFECTMANAGER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MShwEffect;
class TShwEffectInfo;

// CLASS DECLARATION

/**
 *  MShwEffectManager
 * @internal reviewed 07/06/2007 by Loughlin 
 */
class MShwEffectManager
	{
	public:
	    /**
	     * The order that the effects are shown.
	     */
	    enum TShwEffectOrder
	        {
	        EEffectOrderLinear,
	        EEffectOrderRandom,
	        EEffectOrderProgrammed
	        };

	protected:

		/**
		 * Destructor. Dont allow deleting objects through this interface.
		 */
		virtual ~MShwEffectManager() {};

	public: // the API

		/**
		 * This method is used by the framework to give the effect manager the
		 * ownership of the effect objects.
		 * @param aEffect a effect
		 */
		virtual void AddEffectL( MShwEffect* aEffect ) = 0;

		/**
		 * @return the current effect object.
		 */
		virtual MShwEffect* CurrentEffect() = 0;

		/**
		 * @param aIndex the direction that the list is traversed to retrieve
		 *			 the effect. A positive value gets a "next" effect, 
		 * 			 negative gets "previous".
		 * @return the next effect object. Note! this may be the same as current
		 */
		virtual MShwEffect* Effect(TInt aDirection) = 0;

		/**
		 * Tells the effect manager to proceed to the next or previous effect.
		 * @param aDirection, positive for "next" or negative 
		 *			for "previous" effect.
		 */
		virtual void ProceedToEffect( TInt aDirection ) = 0;
        
        /**
		 * Sets the effect order.
		 * @param aOrder the order in which the effects are retrieved.
		 */
		virtual void SetEffectOrder( TShwEffectOrder aOrder ) = 0;
        
        /**
		 * @return the effect order.
		 */
		virtual TShwEffectOrder EffectOrder() = 0;

        /**
		 * Sets the programmable effect order.
		 * Note that the effect order must be set to
		 * EShwEffectOrderProgrammable for the parameter to be stored.
		 * @param aEffects array of effects
		 */
		virtual void SetProgrammedEffects( RArray< TShwEffectInfo >&
			aEffects ) = 0;
		
		 /**
		 * Sets the default effect to use.
		 * @param aInfo effect info
		 */
		virtual void SetDefaultEffectL(TShwEffectInfo aInfo) = 0;
		
        /**
		 * Gets the active effects.
		 * This array will contain pointers to all the effects that
		 * are used in this show
		 * @param aEffects array of effects
		 */
		virtual void GetActiveEffectsL( RPointerArray< MShwEffect >&
			aEffects ) = 0;
				        
	};

#endif // __MSHWEFFECTMANAGER_H__

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
* Description:    The effect interface
 *
*/




#ifndef __SHWEFFECT_H__
#define __SHWEFFECT_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <alf/alfvisual.h>
#include <alf/alfenv.h>

#include "shweffectinfo.h"

// FORWARD DECLARATIONS
class MGlxVisualList;
class MGlxMediaList;
class CAlfEnv;
class MGlxLayout;
class CAlfVisual;
class TShwEffectInfo;

// CLASS DECLARATION
/**
 * MShwEffect, interface for the effects.
 * Effect gets the visual and media list and the HUI environment in 
 * initialisation as well as the screen size.
 */
class MShwEffect
	{
	public: // Construction & destruction

		/**
		 * Destructor. Allow deletion through this interface
		 */
		virtual ~MShwEffect() {};
		
		/**
		 * Clone the effect, this is needed when same effect
		 * is used multiple times in a row.
		 * @return a clone of the effect.
		 */
		virtual MShwEffect* CloneLC() = 0;

	public: // the API

		/**
		 * Initialize the effect.
		 * @param aHuiEnv, the HUI environment for the slideshow
		 * @param aVisualList, the visual list used for the slideshow
		 * @param aMediaList, the media list used for the slideshow
		 * @param aScreenSize, the screen size for the slideshow
		 */
		virtual void InitializeL( 
    		CAlfEnv* aAlfEnv,
		    MGlxVisualList* aVisualList,
		    MGlxMediaList* aMediaList,
		    TSize aScreenSize ) = 0;

		/**
		 * This method is called before the view mode of the effect for
		 * the given visual is about to start
		 * @param aVisual the HUI visual that is the target for the effect
		 * @param aSize the size of the image to view
		 * @param the size of the thumbnail to load for the effect
		 */
		virtual TSize PrepareViewL(
			CAlfVisual* aVisual, TSize aImageSize ) = 0;
		
		/**
		 * Enter view mode. The effect may fade-in itself,
		 * the duration for this is given as a parameter.
		 * @param aVisual the HUI visual that is the target for the view
		 * @param aDuration the whole view mode time
		 * @param aFadeInDuration the entry "fade-in" time
		 * @return the layout chain that applies for the view mode
		 */
		virtual MGlxLayout* EnterViewL( 
			CAlfVisual* aVisual, TInt aDuration, TInt aFadeInDuration ) = 0;

		/**
		 * Exit view mode. This gets called just before the transition 
		 * phase starts.
		 * @param aVisual the HUI visual that was the target for the view
		 */
		virtual void ExitView( CAlfVisual* aVisual ) = 0;

		/**
		 * Enter transition mode. The duration for the transition is given
		 * as a parameter, during this time the effect can animate its exit.
		 * @param aVisual the HUI visual that is the target for the transition
		 * @param aDuration the length of the transition
		 * @return the layout chain that applies for the transition mode
		 */
		virtual MGlxLayout* EnterTransitionL(
			CAlfVisual* aVisual, TInt aDuration ) = 0;

		/**
		 * Exit from transition mode. This method is called once the transition 
		 * has ended and the corresponding visual is no longer visible on screen.
		 * @param aVisual the HUI visual that was the target for the transition
		 */
		virtual void ExitTransition( CAlfVisual* aVisual ) = 0;

		/**
		 * Go to pause. While on pause the effect is expected to be frozen
		 */
		virtual void PauseL() = 0;

		/**
		 * Resume from pause.
		 */
		virtual void Resume() = 0;

        /**
         * Encapsulates the effect's display name and uid
         * @return the effect's information
         */
        virtual TShwEffectInfo EffectInfo() = 0;
       
	};

#endif // __SHWEFFECT_H__

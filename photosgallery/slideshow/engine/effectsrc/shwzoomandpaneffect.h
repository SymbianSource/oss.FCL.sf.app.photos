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
* Description:    The zoom and pan effect
 *
*/




#ifndef __SHWZOOMANDPANEFFECT_H__
#define __SHWZOOMANDPANEFFECT_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <alf/alfvisual.h>
#include <alf/alfenv.h>

#include "shweffect.h"

// CLASS DECLARATION

/**
 * CShwZoomAndPanEffect, the Zoom And Pan effect.
 * Zooms and pans in the image in full screen in view mode and performs smooth transition.
 * 
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 * @internal reviewed 06/06/2007 by Dan Rhodes
 */
NONSHARABLE_CLASS( CShwZoomAndPanEffect ) : public CBase, public MShwEffect
	{
	public:

		/**
		 * Contructor.
		 */
		static CShwZoomAndPanEffect* NewLC();
		/**
		 * Destructor.
		 */
		~CShwZoomAndPanEffect();

	private:

		/**
		 * C++ contructor.
		 */
		CShwZoomAndPanEffect();

	public: // from MShwEffect

		/// @ref MShwEffect::CloneLC
		MShwEffect* CloneLC();
		/// @ref MShwEffect::InitializeL
		void InitializeL( 
    		CAlfEnv* aAlfEnv, MGlxVisualList* aVisualList,
		    MGlxMediaList* aMediaList, TSize aScreenSize );
		/// @ref MShwEffect::PrepareViewL
		TSize PrepareViewL( CAlfVisual* aVisual, TSize aImageSize );
		/// @ref MShwEffect::EnterViewL
		MGlxLayout* EnterViewL( 
			CAlfVisual* aVisual, TInt aDuration, TInt aFadeInDuration );
		/// @ref MShwEffect::ExitView
		void ExitView( CAlfVisual* aVisual );
		/// @ref MShwEffect::EnterTransitionL
		MGlxLayout* EnterTransitionL( CAlfVisual* aVisual, TInt aDuration );
		/// @ref MShwEffect::ExitTransition
		void ExitTransition( CAlfVisual* aVisual );
		/// @ref MShwEffect::PauseL
		void PauseL();
		/// @ref MShwEffect::Resume
		void Resume();
        /// @ref MShwEffect:EffectInfo
        TShwEffectInfo EffectInfo();

	private: // Data

		/// Own: the implementation class
		class CShwZoomAndPanEffectImpl;
		CShwZoomAndPanEffectImpl* iImpl;

	};

#endif // __SHWZOOMANDPANEFFECT_H__

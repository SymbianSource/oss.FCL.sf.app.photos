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
* Description:   The slideshow engine API
 *
*/




#ifndef __CSHWSLIDESHOWENGINEIMPL_H__
#define __CSHWSLIDESHOWENGINEIMPL_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "shwslideshowengine.h"
#include "shweventobserver.h"
#include "shweventpublisherbase.h"
#include <alf/alfbrush.h>
#include <alf/alfdisplay.h>
// FORWARD DECLARATIONS
class MGlxMediaList;
class CGlxThumbnailContext;
class CGlxVisualListManager;
class MGlxVisualList;
class CShwPlaybackFactory;
class CShwEventRouter;

class CAlfEnv;
// CLASS DECLARATION

/**
 * CCShwSlideshowEngineImpl
 * 
 * @author Kimmo Hoikka
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 */
NONSHARABLE_CLASS( CShwSlideshowEngineImpl )
	: public CShwEventPublisherBase,
	public MShwEventObserver
	{
	public: // Constructors and destructor

		/**
		 * Constructor
		 * @param aObserver, observer of the engine.
		 */
		static CShwSlideshowEngineImpl* NewL( MShwEngineObserver& aObserver );

		/**
		 * Destructor.
		 */
		~CShwSlideshowEngineImpl();

	private:
		
		/**
		 * Constructor for performing 1st stage construction
		 * @param aObserver, observer of the engine.
		 */
		CShwSlideshowEngineImpl( MShwEngineObserver& aObserver );

		/**
		 * 2nd stage construction
		 */
		void ConstructL();
			
	public: // the API

		/// @ref CShwSlideshowEngine::StartL
		void StartL(
			CAlfEnv& aEnv, 
			CAlfDisplay& aDisplay, 
			MGlxMediaList& aItemsToShow,
			MShwMusicObserver& aMusicObserver,
			TSize aScreenSize );
		/// @ref CShwSlideshowEngine::NextItemL
		void NextItemL();
		/// @ref CShwSlideshowEngine::PreviousItemL
		void PreviousItemL();
		/// @ref CShwSlideshowEngine::PauseL
		void PauseL();
		/// @ref CShwSlideshowEngine::ResumeL
		void ResumeL();
		/// @ref CShwSlideshowEngine::ToggleUiControlsVisibiltyL
		void ToggleUiControlsVisibiltyL();
		/// @ref CShwSlideshowEngine::LSKPressedL
		void LSKPressedL();
		/// @ref CShwSlideshowEngine::State
		CShwSlideshowEngine::TShwState State() const;
		/// @ref CShwSlideshowEngine::AvailableEffectsL
		static void AvailableEffectsL( RArray<TShwEffectInfo>& aEffects );
		/// @ref CShwSlideshowEngine::VolumeUpL
		void VolumeUpL();
		/// @ref CShwSlideshowEngine::VolumeDownL
		void VolumeDownL();
	    /// @ref CShwSlideshowEngine::GetMusicVolumeL
	    void GetMusicVolumeL();
	    /// @ref CShwSlideshowEngine::HandleHDMIDecodingEventL
		void HandleHDMIDecodingEventL(THdmiDecodingStatus aStatus);

	public: // From MShwEventObserver

		/// @ref MShwEventObserver::NotifyL
		void NotifyL( MShwEvent* aEvent );

	private:	// Implementation

		/// Ref: the observer
		MShwEngineObserver& iObserver;
		/// Own: the media list to play
		MGlxMediaList* iMediaList;
		/// Own: the visual list manager
		CGlxVisualListManager* iVisualListManager;
		/// Own: the visual list to play
		MGlxVisualList* iVisualList;
		/// Own: the factory that creates the event controllers
		CShwPlaybackFactory* iFactory;
		/// Own: the event controllers
		CShwEventRouter* iEventRouter;
		/// Own: state flag
		CShwSlideshowEngine::TShwState iState;
		/// Ref: The HUI display
		CAlfDisplay* iDisplay;
	};

#endif // __CSHWSLIDESHOWENGINEIMPL_H__

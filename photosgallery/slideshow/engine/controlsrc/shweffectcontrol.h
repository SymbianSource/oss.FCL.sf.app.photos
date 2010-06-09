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
* Description:    The effect control for the slideshow
 *
*/




#ifndef __CSHWEFFECTCONTROL_H__
#define __CSHWEFFECTCONTROL_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <mglxvisuallistobserver.h>
#include <mglxvisuallist.h>
#include <glxlayoutsplitter.h>
#include <glxsetvaluelayout.h>

#include "shwevent.h"
#include "shweventobserver.h"
#include "shweventpublisherbase.h"
#include "shwthumbnailloader.h"

// FORWARD DECLARATIONS
class MShwEffectManager;
class MGlxVisualList;
class MGlxMediaList;
class MShwEffect;
class CAlfVisual;
class CShwTimer;

// CLASS DECLARATION

/**
 *  CShwEffectControl
 * 
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 */
NONSHARABLE_CLASS( CShwEffectControl )
	: public CShwEventPublisherBase, 
	public MShwEventObserver, 
	public MGlxVisualListObserver,
	public MShwThumbnailLoadObserver
	{
	private:

		/// effect control state
		enum TShwEffectState
			{
			EShwEffectStateInView = 1,
			EShwEffectStateInTransition
			};

	public: // Constructors and destructor
        
		/**
		 * Constructor.
		 * @param aEffectManager the owner and manager of the effects
		 * @param aVisualList the visual list where to apply the effects
		 * @param aMediaList the media list which is used for item loading
		 * @param aScreenSize the screen size for the default layout
		 */
		static CShwEffectControl* NewL(
			MShwEffectManager& aEffectManager, 
			MGlxVisualList& aVisualList,
			MGlxMediaList& aMediaList,
			TSize aScreenSize );

		/**
		 * Destructor.
		 */
		~CShwEffectControl();

	private:

		/**
		 * Constructor
		 * See NewL for parameters 
		 */
		CShwEffectControl(
			MShwEffectManager& aEffectManager, 
			MGlxVisualList& aVisualList,
			MGlxMediaList& aMediaList,
			TSize aScreenSize );

		/**
		 * 2nd stage constructor
		 */
		void ConstructL();
		
	public: // Callback API, dont use directly

		/**
		 * This method is called when the transition is complete
		 * @param aEffectControl, pointer to this object
		 */
		TInt SendTransitionReadyL();
		
	public: // From MShwEventObserver

		/// @ref MShwEventObserver::NotifyL
		void NotifyL( MShwEvent* aEvent );

    public: // from MGlxVisualListObserver

		/// @ref MGlxVisualListObserver::HandleFocusChangedL
		void HandleFocusChangedL(
			TInt aFocusIndex, TReal32 aItemsPerSecond, 
			MGlxVisualList* aList, NGlxListDefs::TFocusChangeType aType );
		/// @ref MGlxVisualListObserver::HandleSizeChanged
		void HandleSizeChanged(
			const TSize& aSize, MGlxVisualList* aList );
		/// @ref MGlxVisualListObserver::HandleVisualRemoved
		void HandleVisualRemoved(
			const CAlfVisual* aVisual,  MGlxVisualList* aList );
		/// @ref MGlxVisualListObserver::HandleVisualAddedL
		void HandleVisualAddedL( 
			CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList );

	private: // from MShwThumbnailLoadObserver

		/// @ref MShwThumbnailLoadObserver::HandleThumbnailLoadedL
		void HandleThumbnailLoadedL( TInt aIndex );
		/// @ref MShwThumbnailLoadObserver::HandleThumbnailLoadFailureL
		void HandleThumbnailLoadFailureL( TInt aIndex );
		/// @ref MShwThumbnailLoadObserver::HandleMediaListEmpty
        void HandleMediaListEmpty();

	private: // Implementation

		/**
		 * Prepare view for given effect for focus index
		 * and load the thumbnail in correct size
		 * @param aEffect the effect
		 */
        void PrepareAndLoadImageToViewL();

		/**
		 * StartView event handler
		 * @param the start view event
		 */
		void HandleStartViewEventL( TShwEventStartView& aEvent );
		
		/**
		 * StartTransition event handler
		 * @param the start transition event
		 */
		void HandleStartTransitionEventL( TShwEventStartTransition& aEvent );

		/**
		 * Pause event handler
		 */
		void HandlePauseEventL();

		/**
		 * Resume event handler
		 */
		void HandleResumeEventL();

		/**
		 * Next item event handler
		 */
		void HandleNextImageEventL();		

		/**
		 * Previous item event handler
		 */
		void HandlePreviousImageEventL();
		
		/**
         * Toogle Control Ui event handler
         */
		void HandleToggleControlUiEventL();
		/**
		 * Navigates the list and effects to given direction
		 * @param the direction
		 */
		void NavigateListL( TInt aDirection );

		/**
		 * Returns the next index to the left or right
		 * @param the direction
		 * @return the next index in the visual list
		 */
		TInt NextListIndex( TInt aDirection );

	private:	// Implementation

		/// Ref: the effect manager
		MShwEffectManager& iEffectManager;
		/// Ref: the visual list
		MGlxVisualList& iVisualList;
		/// Ref: the media list
		MGlxMediaList& iMediaList;

		/// Own: the timer
		CShwTimer* iTransitionTimer;
		/// Own: the layout splitter
		TGlxLayoutSplitter iSplitter;
        /// Own: the default size layout
		TGlxSetValueLayout iDefaultIconLayout; 
        /// Own: the default opacity layout
		TGlxSetValueLayout iPausedLayout;
        /// Own: the default layout when a visual is not visible
		TGlxSetValueLayout iNotVisibleLayout;
		/// Own: state flag to know if loop is started
		TBool iEffectLoopRestarted;
		/// Own: the context id
		TGlxViewContextId iContextId;
		/// Own: the thumbnail loader
		CShwThumbnailLoader* iThumbnailLoader;
		/// Ref: the previous effect (fading out)
		MShwEffect* iPreviousEffect;
		/// Ref: the current visual (shown in view)
		CAlfVisual* iCurrentVisual;
		/// Ref: the previous visual (fading out)
		CAlfVisual* iPreviousVisual;
		/// Own: current state
		TShwEffectState iState;
		/// Own: current view duration
		TInt iViewDuration;
		/// Own: flag to tell if user navigated during pause
		TBool iUserNavigated;
		RPointerArray<CAlfVisual> iPausedVisuals;
		
		TInt iFailedThumbnailIndex;
		/// Own: Flag to tell if HDMI decoding is completed
		TBool iHDMIFirstDecoded;
		/// Own: Flag to tell if TN ready received
		TBool iTNReadyReceived;		
	};

#endif // __CSHWEFFECTCONTROL_H__

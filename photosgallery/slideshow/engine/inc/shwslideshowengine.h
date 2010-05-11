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
* Description:    This file defines the API for ShwSlideshowEngine.dll
 *
*/




#ifndef __SHWSLIDESHOWENGINE_H__
#define __SHWSLIDESHOWENGINE_H__

// Include Files
#include <e32base.h>
#include <e32std.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>

// Forward declarations
class CAlfEnv;
class CAlfDisplay;
class MGlxMediaList;
class MShwEngineObserver;
class CShwSlideshowEngineImpl;
class TShwEffectInfo;
class MShwMusicObserver;

// Class Definition
/**
 * CShwSlideshowEngine
 *
 * This is the main API for the slideshow engine
 * 
 * @author Kimmo Hoikka
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 */
NONSHARABLE_CLASS( CShwSlideshowEngine ) : public CBase
    {
    public:

        /**
         * The state of the engine.
         */
        enum TShwState 
            {
            EShwStateInitialized = 1,
            EShwStateRunning,
            EShwStatePaused
            };

    public: // Construction

        /**
         * Constructor
         * @param aObserver, observer of the engine.
         */
        IMPORT_C static CShwSlideshowEngine* NewL( 
            MShwEngineObserver& aObserver );

        /**
         * Destructor
         */
        IMPORT_C ~CShwSlideshowEngine();

    public: // the API

        /**
         * This method starts the slideshow.
         * @param CHuiEnv& the HUI environment to use
         * @param CHuiDisplay& the HUI display to draw the list to
         * @param MGlxMediaList& the items to show.
         * @param MShwMusicObserver& the music control observer.
         * @param aScreenSize the size for the slideshow screen
         */
        IMPORT_C void StartL( CAlfEnv& aEnv, CAlfDisplay& aDisplay,
                            MGlxMediaList& aItemsToShow,
                            MShwMusicObserver& aMusicObserver,
                            TSize aScreenSize );

        /**
         * This method commands engine to proceed to next item.
         */
        IMPORT_C void NextItemL();

        /**
         * This method commands engine to proceed to previous item.
         */
        IMPORT_C void PreviousItemL();

        /**
         * This method pauses the slideshow
         */
        IMPORT_C void PauseL();

        /**
         * This method resumes the paused slideshow
         */
        IMPORT_C void ResumeL();

        /**
         * This method returns the state of the slideshow
         * @return the state of the engine
         */
        IMPORT_C TShwState State() const;

        /**
         * This method retrieves the available effects
         * @param an array where the effect infos are added
         */
        IMPORT_C static void AvailableEffectsL(RArray<TShwEffectInfo>& aEffects);

        /**
         * This method increases the volume
         */
        IMPORT_C void VolumeUpL();

        /**
         * This method decreases the volume
         */
        IMPORT_C void VolumeDownL();
        
		/**
		 * Get the latest music volume value. 
		 */
        IMPORT_C void GetMusicVolumeL();
        
        /**
         * This method change the ui staus in slideshow when MSK Preesed
         */
        IMPORT_C void ToggleUiControlsVisibiltyL();
        
        /**
         * This method react when LSK Preesed
         */
        IMPORT_C void LSKPressedL();

    private: // Implementation

        /// C++ constructor, no derivation or use outside the DLL
        CShwSlideshowEngine();
        /// Own: the real implementation
        CShwSlideshowEngineImpl* iImpl;

    };

#endif  // __SHWSLIDESHOWENGINE_H__

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
* Description:    The playback factory, configures the playback for the slideshow
 *
*/




#ifndef __CSHWPLAYBACKFACTORY_H__
#define __CSHWPLAYBACKFACTORY_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>


// Forward declarations
class CAlfEnv;
class MGlxVisualList;
class MGlxMediaList;
class MShwEventObserver;
class MShwEventPublisher;
class TShwEffectInfo;
class MShwMusicObserver;
// CLASS DECLARATION

/**
 * CShwPlaybackFactory
 * 
 * @lib shwslideshowengine.lib
 * @internal reviewed 08/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS( CShwPlaybackFactory ) : public CBase
    {
    public: // Constructors and destructor

        /**
         * Constructor
         * @param aHuiEnv the HUI environment
         * @param aVisualList the visual list of the slide show
         * @param aMediaList the media list for the show
         * @param aMusicObserver the music control observer. 
         * @param aScreenSize the screen size to use
         */
        static CShwPlaybackFactory* NewL(
            CAlfEnv* aAlfEnv, 
            MGlxVisualList* aVisualList,
            MGlxMediaList* aMediaList,
            MShwMusicObserver& aMusicObserver, 
            TSize aScreenSize );

        /**
         * Destructor.
         */
        ~CShwPlaybackFactory();

    private:

        /**
         * C++ constructor
         */
        CShwPlaybackFactory();

    public: // the API

        /**
         * @return the event observers.
         * Note! ownership of the real objects stays in the factory.
         */
        RPointerArray< MShwEventObserver > EventObservers();

        /**
         * @return the event publishers.
         * Note! ownership of the real objects stays in the factory.
         */
        RPointerArray< MShwEventPublisher > EventPublishers();
        
        /// @ref CShwSlideshowEngine::AvailableEffectsL
        static void AvailableEffectsL( RArray<TShwEffectInfo>& aEffects );
        
        /// @ref CShwSlideshowEngine::GetMusicVolumeL
        void GetMusicVolumeL();
        
    private:

        /// Own: The implementation of the class, hidden from the clients
        class CShwPlaybackFactoryImpl;
        CShwPlaybackFactoryImpl* iImpl;

    };

#endif // __CSHWPLAYBACKFACTORY_H__

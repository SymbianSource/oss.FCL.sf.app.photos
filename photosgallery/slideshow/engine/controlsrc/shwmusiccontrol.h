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
* Description:    The music control for the slideshow
 *
*/



#ifndef __CSHWMUSICCONTROL_H__
#define __CSHWMUSICCONTROL_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <mpxplaybackobserver.h>		// MMPXPlaybackCallback
#include <mpxplaybackframeworkdefs.h> 	// TMPXPlaybackProperty

#include "shwmusicobserver.h"			// MShwMusicObserver
#include "shweventobserver.h"
#include "shweventpublisherbase.h"


// FORWARD DECLARATIONS
class MMPXPlaybackUtility;


// CLASS DECLARATION

/**
 * CShwMusicControl
 * Slideshow music control wraps the MPX music control for slideshow
 * @lib shwslideshowengine.lib
 * @author Loughlin Spollen
 */
NONSHARABLE_CLASS( CShwMusicControl )
		  : public CShwEventPublisherBase, 
		    public MShwEventObserver, 
	        public MMPXPlaybackCallback,
			public MMPXPlaybackObserver
	{
	public: // Constructors and destructor

		/**
		 * Constructor.
		 * @param aFilePath, the list that this view control manages
		 * @param aMusicObsvr, the music control observer which receives
		 * 					   notification of music on/off and 
		 *					   volume level.
		 */
		static CShwMusicControl* NewL(MShwMusicObserver& aMusicObsvr, 
										const TDesC& aFilePath);

		/**
		 * Destructor.
		 */
		~CShwMusicControl();

	private:

		/**
		 * Constructor
		 * @param aFilePath, the list that this view control manages
		 * @param aMusicObsrvr* the music control observer. May be Null. 
		 */
		CShwMusicControl(MShwMusicObserver& aMusicObsvr, 
						const TDesC& aFilePath);

		/**
		 * 2nd stage constructor
		 */
		void ConstructL();

		/**
		 * Initialise the player utility member
		 */
		void InitPlayerL();
		
		/**
		 * Retrieve the current volume settings from the player 
		 *		utility member
		 * Volume settins are retrieved asyncronously
		 */
		void VolumeL();

	public: // From MShwEventObserver

		/** @ref MShwEventObserver::NotifyL */
		void NotifyL(MShwEvent* aEvent);

	public: // From MMPXPlaybackCallback

        /**
         * Handle playback property
		 * @ref MMPXPlaybackCallback::HandlePropertyL 
		 */	
		void HandlePropertyL(
		    TMPXPlaybackProperty aProperty, 
			TInt aValue, TInt aError );
		
        /**
         * Method is called continously until aComplete=ETrue,
         * signifying that it is done and there will be no more 
         * callbacks
         * Only new items are passed each time
		 * @ref MMPXPlaybackCallback::HandleSubPlayerNamesL
         */		
		void HandleSubPlayerNamesL(
		    TUid aPlayer, const MDesCArray* aSubPlayers,
            TBool aComplete, TInt aError );

        /**
         * Handle extended media properties
		 * @ref MMPXPlaybackCallback::HandleMediaL
         */
		void HandleMediaL(
		    const CMPXMedia& aProperties, TInt aError );
		
		/**
		 * Get the latest music volume value. 
		 */
		void GetMusicVolumeL();

	private:	// From MMPXPlaybackObserver 
	
	    /**
         * Handle playback message - required for debugging state of MPX 
         * Player component
		 */	
         void HandlePlaybackMessageL(
            const TMPXPlaybackMessage& aMessage );
         
       /**
        *  Handle playback message
        *  NOTE: only one of HandlePlaybackMessage callback can be implemented
        * 
        *  @param aMsg playback message, ownership not transferred. 
        *         Please check aMsg is not NULL before using it. If aErr is not 
        *         KErrNone, plugin might still call back with more info in the aMsg.
        *  @param aErr system error code.
        */
		 void HandlePlaybackMessage(const CMPXMessage& aMsg);
		 
	private: // Implementation

        // implmentation of the NotifyL
        void HandleEventL( MShwEvent* aEvent );
        
        /// Own: music is beeing switched on
        TBool iSwitchingMusicOn;

		/// Ref: the path to the music file
		const TDesC& iFilePath;

	    /// Own: the MPX player utility
	    MMPXPlaybackUtility* iPlaybackUtility;

	    /// Ref: Music Observer 
	    MShwMusicObserver&   iMusicObsvr;

	    /// Own: the max volume setting
	    TInt iMaxVolume;
	    
	    /// Own: the current volume setting
	    TInt iCurrentVolume;

	    // It is set to ETrue if music can be played
	    TBool iCanPlay;
	    
	    // It is set to ETrue if music player is not yet initialized
	    TBool iPlayCached;

		/// Own: state flag
		enum TState 
			{
			EMusicOn,
			EMusicOff
			} iState;	    

	};

#endif // __CSHWMUSICCONTROL_H__

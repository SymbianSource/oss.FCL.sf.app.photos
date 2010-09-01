/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The pause handling control for the slideshow
 *
*/




#ifndef __CSHWSLIDESHOWPAUSEHANDLER_H__
#define __CSHWSLIDESHOWPAUSEHANDLER_H__

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CShwSlideshowEngine;

// CLASS DECLARATION

/**
 * CShwSlideShowPauseHandler
 * This class handles the different pause related requirements of 
 * slide show: User pause, Phone call pause, Task away pausing.
 * User pause/resume has the highest precedence, phone call is next
 * and then task away (switch to background)
 * So if the user pauses slideshow and then a phone call starts and 
 * ends, slide show stays paused. Similarly if slideshow is running
 * and gets switched to background and a phone call starts, slide show
 * does not continue when switched to foreground unless the phone call
 * ends before that.
 */
NONSHARABLE_CLASS( CShwSlideShowPauseHandler )
		  : public CBase
	{
	public: // Constructors and destructor

        /**
         * Constructor.
         * @param reference to the slide show engine
         */
         static CShwSlideShowPauseHandler* NewL( CShwSlideshowEngine& aEngine );

        /**
         * Destructor.
         */
        ~CShwSlideShowPauseHandler();

    private:

        /**
         * Constructor
         * @param reference to the slide show engine
         */
         CShwSlideShowPauseHandler( CShwSlideshowEngine& aEngine );

    public: // The API

        /**
          * Informs the pause handler that user paused slide show
          * Engine will always be paused if it was running and 
          * vice versa,
          */
        void UserToggledPauseL();

        /**
          * Informs the pause handler that a phone call has started.
          * If engine is running when call starts, if will be paused
          */
        void PhoneCallStartedL();

        /**
          * Informs the pause handler that a phone call has ended
          * If engine was paused by telephone call we resume slide show
          * when call ends
          */
        void PhoneCallEndedL();

        /**
          * Informs the pause handler that slide show was switched to background.
          * If engine is running when switch occurs, if will be paused
          */
        void SwitchToBackgroundL();

        /**
          * Informs the pause handler that slide show was switched to foreground.
          * If engine was paused when switch to background occurred, if will be resumed
          * unless a phone call is ongoing.
          */
        void SwitchToForegroundL();
        
        /**
          * Informs the pause handler that MSK Preesed when slide show is running. 
          */
        void ToggleUiControlsVisibiltyL();
        
        /**
         * Returns the slideshow engine paused state.
         * @return ETrue, if slideshow engine is in paused state
         */
        TBool IsSlideShowEngineStatePaused();

    private: // Implementation

        /// Ref: The slideshow engine
        CShwSlideshowEngine& iEngine;

        /// Own: state flag to know if we are in a call
        TBool iInCall;
        
        /// Own: state flag to know when user paused us
        TBool iUserPaused;
        
        /// Own: state flag to know when we are on foreground
        TBool iOnForeground;

    };

#endif // __CSHWSLIDESHOWPAUSEHANDLER_H__


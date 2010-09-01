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
* Description:    Slideshow backlight timer management..
*
*/




#ifndef C_SHWSLIDESHOWBACKLIGHTTIMER_H
#define C_SHWSLIDESHOWBACKLIGHTTIMER_H

#include <e32base.h>
#include <shwconstants.h>

// FORWARD DECLARATION
class CRepository;

// CLASS DECLARATION

/**
* Media gallery backlight controlling timer (for slide show view)
* @internal reviewed 07/06/2007 by Kimmo Hoikka
*/
NONSHARABLE_CLASS(CShwSlideshowBackLightTimer) : public CBase
    {
    public:  // Constructors and destructor

		/**
        * Two-phased constructor.
        */
        static CShwSlideshowBackLightTimer* NewL();

        /**
        * Destructor.
        */
        virtual ~CShwSlideshowBackLightTimer();

    public: // New functions

		/**
		* Cancels the timer
		*/
		void Cancel();

		/**
		* Starts the timer
		*/
		void StartL();

		/**
		* Is timer active or not?
		* @return ETrue is active, EFalse otherwise
		*/
		TBool IsRunning();

		/**
		* Delay value
		* @return TInt
		*/
		TInt Delay();

		/**
		* Callback function for the timer to call
		* @param aObject any given object
		* @return TInt
		*/
		TInt Tick();

	private:

		/**
        * C++ default constructor.
        */
        CShwSlideshowBackLightTimer();

		/**
        * Symbian 2nd phase constructor
        */
        void ConstructL();

	private: // Data

		TInt  iSettingsDelay;
		TTime iStartTime;

		CPeriodic* iPeriodic; // owned
	};

#endif	// C_SHWSLIDESHOWBACKLIGHTTIMER_H

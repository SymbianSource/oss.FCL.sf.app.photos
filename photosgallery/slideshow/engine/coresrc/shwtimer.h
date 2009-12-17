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
* Description:    Slideshow specific timer
 *
*/




#ifndef __SHWTIMER_H__
#define __SHWTIMER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// Forward declarations

// CLASS DECLARATION

/**
 * CShwTimer. a Pause-able timer
 * 
 * @lib shwslideshowengine.lib
 */
NONSHARABLE_CLASS( CShwTimer ) : public CBase
	{
	public:

		/**
		 * Constructor.
		 * @param TInt aPriority - the priority of the timer.
		 */
		static CShwTimer* NewL( TInt aPriority );

		/**
		 * Destructor.
		 */
		~CShwTimer();

	public: // the API

		/**
		 * This method starts the timer.  It may be called repeatedly
		 * as the timer is always first cancelled.
		 * Note! Time is given in milliseconds
		 * 
		 * @param aDelay - milliseconds for the first callback
		 * @param aInterval - milliseconds for the successive callbacks
		 * @param aCallback - the function to call
		 */
		void Start( TInt aDelay, TInt aInterval, TCallBack aCallback );

		/**
		 * This method cancels the timer.
		 */
		void Cancel();

		/**
		 * Pauses the timer if it is active
		 */
		void Pause();
		
		/**
		 * Resumes the timer. If timer was paused the timer is restarted
		 * for the time left and after that for interval.
		 */
		void Resume();

		/**
		 * Method that returns whether the timer is active.
		 * @return ETrue if the timer is active, EFalse otherwise
		 */
		TBool IsActive();
		
	private: // Constructors

		/**
		 * C++ constructor.
		 */
		CShwTimer();

		/**
		 * 2nd stage constructor
		 */
		void ConstructL( TInt aPriority );

		/**
		 * This method returns the amount of time remaining until
		 * the timer is due to expire.
		 * @return - the time in microseconds left until the timer finishes
		 *			or 0 (whichever is greater).
		 */
		TTimeIntervalMicroSeconds32 TimeLeft();
		
	private:

		/// Own: the timer
		CPeriodic* iTimer;
		
		/// Own: the callback
		TCallBack iCallback;
		/// Own: the time left in pause
		TTimeIntervalMicroSeconds32 iTimeLeft;
		/// Own: the interval for the timer
		TTimeIntervalMicroSeconds32 iInterval;
		/// Own: state flag that tells us if we are paused
		TBool iPaused;

		/// Own: the time at which the timer is set to expire
		TTime iTimeTimerExpires;
	};

#endif // __SHWTIMER_H__

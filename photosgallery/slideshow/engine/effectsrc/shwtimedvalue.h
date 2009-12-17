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
* Description:    Wrapper on top of THuiTimedValue to enable pause / resume
 *
*/




#ifndef __SHWTIMEDVALUE_H__
#define __SHWTIMEDVALUE_H__

//  Include Files
#include <e32std.h>
#include <alf/alftimedvalue.h>
//#include <uiacceltk/huitimedpoint.h>

/**
 * Wrapper class to enable pause and resume for timed values.
 *
 * @author Kimmo Hoikka
 * @lib shwslideshowengine.lib
 */
NONSHARABLE_CLASS( TShwTimedValue )
	{
	public:	// the API

		/**
		 * Constructor, initializes value to zero
		 */
		inline TShwTimedValue();

		/**
		 * Sets the interpolation style
		 * @param aStyle, @ref THuiInterpolationStyle
		 */
		inline void SetStyle( TAlfInterpolationStyle aStyle );

		/**
		 * Sets the value to be aValue now
		 * @param aValue the target value
		 */
		inline void Set( TReal32 aValue );

		/**
		 * Sets the value to be aValue after the specified aTime
		 * @param aValue the target value
		 * @param aTime the target time for the value
		 */
		inline void Set( TReal32 aValue, TInt aTime );

		/**
		 * Return the current interpolated value.
		 * @return the current value
		 */
		inline TReal32 Now();

		/**
		 * Return the target value.
		 * @return the current value
		 */
		inline TReal32 Target();

		/**
		 * @return ETrue if the value has changed since last time
		 */
		inline TBool Changed() const;

		/**
		 * Clears the changed flag
		 */
		inline void ClearChanged();

		/**
		 * Pauses the timed value
		 */
		inline void Pause();

		/**
		 * Resumes the timed value
		 */
		inline void Resume();

	private:	// Data

		TAlfTimedValue iTimedValue;
		TBool iPaused;
		TReal32 iTargetValue;
		TInt iRemainingTime;

	};

/**
 * Wrapper class to enable pause and resume for timed values
 */
NONSHARABLE_CLASS( TShwTimedSize )
	{
	public:	// the API

		/**
		 * Constructor, initializes value to zero
		 */
		inline TShwTimedSize();

		/**
		 * Sets the interpolation style
		 * @param aStyle, @ref THuiInterpolationStyle
		 */
		inline void SetStyle( TAlfInterpolationStyle aStyle );

		/**
		 * Sets the value to be aValue now
		 * @param aValue the target value
		 */
		inline void Set( TAlfRealSize aValue );

		/**
		 * Sets the value to be aValue after the specified aTime
		 * @param aValue the target value
		 * @param aTime the target time for the value
		 */
		inline void Set( TAlfRealSize aValue, TInt aTime );

		/**
		 * Return the current interpolated value.
		 * @return the current value
		 */
		inline TAlfRealSize Now();

		/**
		 * @return ETrue if the value has changed since last time
		 */
		inline TBool Changed() const;

		/**
		 * Clears the changed flag
		 */
		inline void ClearChanged();

		/**
		 * Pauses the timed value
		 */
		inline void Pause();

		/**
		 * Resumes the timed value
		 */
		inline void Resume();

	public:	// Data

		TShwTimedValue iWidth;
		TShwTimedValue iHeight;

	};

#include "shwtimedvalue.inl"

#endif	// __SHWTIMEDVALUE_H__

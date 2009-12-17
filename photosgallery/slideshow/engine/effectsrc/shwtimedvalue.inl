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




// -----------------------------------------------------------------------------
// TShwTimedValue
// -----------------------------------------------------------------------------
inline TShwTimedValue::TShwTimedValue() : 
	iTimedValue( 0 ),
	iPaused( EFalse ),
	iTargetValue( 0 ),
	iRemainingTime( 0 )
	{
	}

// -----------------------------------------------------------------------------
// SetStyle
// -----------------------------------------------------------------------------
inline void TShwTimedValue::SetStyle( TAlfInterpolationStyle aStyle )
	{
	iTimedValue.SetStyle( aStyle );
	}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------
inline void TShwTimedValue::Set( TReal32 /*aValue */)
	{
	// this version set sets the value always, even in pause
//	iTimedValue.Set( aValue );
	}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------
inline void TShwTimedValue::Set( TReal32 /*aValue*/, TInt /*aTime*/ )
	{
	// we dont set the value in pause unless aTime is zero
	// as while paused we dont animate.
	//if( ( !iPaused ) || ( 0 == aTime ) )
	//	{
		//iTimedValue.Set( aValue, aTime );
	//	}
	}

// -----------------------------------------------------------------------------
// Now
// -----------------------------------------------------------------------------
inline TReal32 TShwTimedValue::Now()
	{
	return iTimedValue.ValueNow();
	}

// -----------------------------------------------------------------------------
// Target
// -----------------------------------------------------------------------------
inline TReal32 TShwTimedValue::Target()
	{
	return iTimedValue.Target();
	}

// -----------------------------------------------------------------------------
// Changed
// -----------------------------------------------------------------------------
inline TBool TShwTimedValue::Changed() const
	{
	//return iTimedValue.Changed();
	//!Hack to supress compiler warnings
	//function no longer used
	//needs to be refactored/removed
	return ETrue;
	}

// -----------------------------------------------------------------------------
// ClearChanged
// -----------------------------------------------------------------------------
inline void TShwTimedValue::ClearChanged()
	{
//	iTimedValue.ClearChanged();
	}

// -----------------------------------------------------------------------------
// Pause
// -----------------------------------------------------------------------------
inline void TShwTimedValue::Pause()
	{
	if ( !iPaused )
		{
		iPaused = ETrue;
		// remember the target
		iTargetValue = iTimedValue.Target();
		// remember the remaining time
		//iRemainingTime = iTimedValue.RemainingTime();
		// stop the timed value by setting its value to current
		//iTimedValue.Set( iTimedValue.Now() );
		}
	}

// -----------------------------------------------------------------------------
// Resume
// -----------------------------------------------------------------------------
inline void TShwTimedValue::Resume()
	{
	if ( iPaused )
		{
		iPaused = EFalse;
		// resume the timed value
		// set the target to be the original target
//		iTimedValue.Set( iTargetValue, iRemainingTime );
		}
	}

// -----------------------------------------------------------------------------
// TShwTimedSize
// -----------------------------------------------------------------------------
inline TShwTimedSize::TShwTimedSize()
	{
	// TShwTimedValue objects reset themselves
	}

// -----------------------------------------------------------------------------
// SetStyle
// -----------------------------------------------------------------------------
inline void TShwTimedSize::SetStyle( TAlfInterpolationStyle aStyle )
	{
	iWidth.SetStyle( aStyle );
	iHeight.SetStyle( aStyle );
	}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------
inline void TShwTimedSize::Set( TAlfRealSize aValue )
	{
	iWidth.Set( aValue.iWidth );
	iHeight.Set( aValue.iHeight );
	}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------
inline void TShwTimedSize::Set( TAlfRealSize aValue, TInt aTime )
	{
	iWidth.Set( aValue.iWidth, aTime );
	iHeight.Set( aValue.iHeight, aTime );
	}

// -----------------------------------------------------------------------------
// Now
// -----------------------------------------------------------------------------
inline TAlfRealSize TShwTimedSize::Now()
	{
	return TAlfRealSize( iWidth.Now(), iHeight.Now() );
	}

// -----------------------------------------------------------------------------
// Changed
// -----------------------------------------------------------------------------
inline TBool TShwTimedSize::Changed() const
	{
	return iWidth.Changed() || iHeight.Changed();
	}

// -----------------------------------------------------------------------------
// ClearChanged
// -----------------------------------------------------------------------------
inline void TShwTimedSize::ClearChanged()
	{
	iWidth.ClearChanged();
	iHeight.ClearChanged();
	}

// -----------------------------------------------------------------------------
// Pause
// -----------------------------------------------------------------------------
inline void TShwTimedSize::Pause()
	{
	iWidth.Pause();
	iHeight.Pause();
	}

// -----------------------------------------------------------------------------
// Resume
// -----------------------------------------------------------------------------
inline void TShwTimedSize::Resume()
	{
	iWidth.Resume();
	iHeight.Resume();
	}

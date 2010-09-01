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
* Description:    Default event implementations
 *
*/




// INCLUDES
#include "shwevent.h"

// -----------------------------------------------------------------------------
// Constructor. iValue is set
// -----------------------------------------------------------------------------
TShwParametrizedEvent::TShwParametrizedEvent( TInt aValue ) : iValue( aValue )
	{
	}
	
// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
TShwParametrizedEvent::~TShwParametrizedEvent()
	{
	}

// -----------------------------------------------------------------------------
// Parameter.
// -----------------------------------------------------------------------------
TInt TShwParametrizedEvent::Parameter()
	{
	return iValue;
	}

// Macro to avoid copy & pasting similar code throughout the event classes
#define SHW_DEFINE_EVENT_CLASS( cls ) \
cls::cls() \
	{ } \
cls::~cls() \
	{ } \
MShwEvent* cls::CloneLC()\
	{ \
	cls* copy = new( ELeave ) cls; \
	CleanupStack::PushL( copy ); \
	return copy; \
	}

#define SHW_DEFINE_PARAMETER_EVENT_CLASS( cls ) \
cls::cls( TInt aValue ) : TShwParametrizedEvent( aValue ) \
	{ } \
cls::~cls() \
	{ } \
MShwEvent* cls::CloneLC() \
	{ \
	cls* copy = new( ELeave ) cls( Parameter() ); \
	CleanupStack::PushL( copy ); \
	return copy; \
	}

/**
 * Initialize the show
 */
SHW_DEFINE_EVENT_CLASS( TShwEventInitialize )

/**
 * Start the slideshow
 */
SHW_DEFINE_EVENT_CLASS( TShwEventStart )

/**
 * Pause the slideshow 
 */
SHW_DEFINE_EVENT_CLASS( TShwEventPause )

/**
 * Resume from pause
 */
SHW_DEFINE_EVENT_CLASS( TShwEventResume )

/**
 *  Next image by the user
 */
SHW_DEFINE_EVENT_CLASS( TShwEventNextImage )

/**
 * Previous image by the user
 */
SHW_DEFINE_EVENT_CLASS( TShwEventPreviousImage )

/**
 * Timer beat occurred
 */
SHW_DEFINE_EVENT_CLASS( TShwEventTimerBeat )

/**
 * View mode is starting
 */
SHW_DEFINE_PARAMETER_EVENT_CLASS( TShwEventStartView  )

/**
 * Ready to start viewing current slide
 */
SHW_DEFINE_EVENT_CLASS( TShwEventReadyToView );

/**
 * Ready to advance to next slide
 */
SHW_DEFINE_EVENT_CLASS( TShwEventReadyToAdvance )

/**
 * Transition mode is starting
 */
SHW_DEFINE_PARAMETER_EVENT_CLASS( TShwEventStartTransition )

/**
 * Transition mode is ready
 */
SHW_DEFINE_EVENT_CLASS( TShwEventTransitionReady )

/**
 * Music volume is adjusted down
 */
SHW_DEFINE_EVENT_CLASS( TShwEventVolumeDown );

/**
 * Music volume is adjusted up
 */
SHW_DEFINE_EVENT_CLASS( TShwEventVolumeUp );

/**
 * Slideshow needs to be exited because of a fatal error
 */
SHW_DEFINE_EVENT_CLASS( TShwEventFatalError );

/**
 * Selection key pressed for changed ui state (Hide or visible)
 */
SHW_DEFINE_EVENT_CLASS( TShwEventToggleControlUi);

/**
 * Event for HDMI Disconnected.
 */
SHW_DEFINE_EVENT_CLASS( TShwEventHDMIDisConnected );

/**
 * Event for HDMI Image decoding has started.
 */
SHW_DEFINE_EVENT_CLASS( TShwEventHDMIImageDecodingStarted );

/**
 * Event for HDMI Image decoding has completed.
 */
SHW_DEFINE_EVENT_CLASS( TShwEventHDMIImageDecodingCompleted );

/**
 * Event for HDMI First Image decoding has completed.
 */
SHW_DEFINE_EVENT_CLASS( TShwEventHDMIFirstImageDecodingCompleted );


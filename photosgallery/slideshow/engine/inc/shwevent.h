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
* Description:    The event interface
 *
*/




#ifndef __SHWEVENT_H__
#define __SHWEVENT_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  MShwEvent, interface for event objects
 */
class MShwEvent
	{
	public:

		/**
		 * Destructor, Allow deletion through this interface, 
		 * If the event queue needs a copy of the event object it needs to delete the copy.
		 */
		virtual ~MShwEvent() {};

	public: // the API

		/**
		 * This method returns a permanent copy the event object. 
		 * This is needed so that the event queue can keep a pointer to the 
		 * event while the event sending code goes out of scope.
		 * This method needs to store the original object to cleanupstack
		 * @return a clone of the event object
		 */
		virtual MShwEvent* CloneLC() = 0;

	};

/**
 *  TShwParametrizedEvent, event object with a value
 */
NONSHARABLE_CLASS( TShwParametrizedEvent ) : public MShwEvent
	{
	public:

		/**
		 * Constructor.
		 * @param aValue, the value for the parameter
		 */
		TShwParametrizedEvent( TInt aValue );
		
		/**
		 * Destructor.
		 */
		~TShwParametrizedEvent();

		/**
		 * This method can be used to get a named parameter from the event.
		 * @return the integer value
		 */
		TInt Parameter();

	private:
		/// Own: the value
		TInt iValue;
	};

// Macro to avoid copy & pasting similar code throughout the event classes
#define SHW_DECLARE_EVENT_CLASS( cls ) \
NONSHARABLE_CLASS( cls ) : public MShwEvent { \
	public: \
		/** Constructor */\
		cls(); \
		/** Destructor */\
		~cls(); \
		/** @see MShwEvent::CloneLC() */\
		MShwEvent* CloneLC(); }\

#define SHW_DECLARE_PARAMETER_EVENT_CLASS( cls ) \
NONSHARABLE_CLASS( cls ) : public TShwParametrizedEvent { \
	public: \
		/** Constructor */\
		cls( TInt aValue ); \
		/** Destructor */\
		~cls(); \
		/** @see MShwEvent::CloneLC() */\
		MShwEvent* CloneLC(); } \

/**
 * Initialize slide show
 */
SHW_DECLARE_EVENT_CLASS( TShwEventInitialize );

/**
 * Start the slide show
 */
SHW_DECLARE_EVENT_CLASS( TShwEventStart );

/**
 * Pause the slide show 
 */
SHW_DECLARE_EVENT_CLASS( TShwEventPause );

/**
 * Resume from pause
 */
SHW_DECLARE_EVENT_CLASS( TShwEventResume );

/**
 *  Next image by the user
 */
SHW_DECLARE_EVENT_CLASS( TShwEventNextImage );

/**
 * Previous image by the user
 */
SHW_DECLARE_EVENT_CLASS( TShwEventPreviousImage );

/**
 * Timer beat occurred
 */
SHW_DECLARE_EVENT_CLASS( TShwEventTimerBeat );

/**
 * View mode is starting
 */
SHW_DECLARE_PARAMETER_EVENT_CLASS( TShwEventStartView  );

/**
 * Ready to start viewing current slide
 */
SHW_DECLARE_EVENT_CLASS( TShwEventReadyToView );

/**
 * Ready to advance to next slide
 */
SHW_DECLARE_EVENT_CLASS( TShwEventReadyToAdvance );

/**
 * Transition mode is starting
 */
SHW_DECLARE_PARAMETER_EVENT_CLASS( TShwEventStartTransition );

/**
 * Transition mode is ready
 */
SHW_DECLARE_EVENT_CLASS( TShwEventTransitionReady );

/**
 * Music volume is adjusted down
 */
SHW_DECLARE_EVENT_CLASS( TShwEventVolumeDown );

/**
 * Music volume is adjusted up
 */
SHW_DECLARE_EVENT_CLASS( TShwEventVolumeUp );

/**
 * Exit slide show because of an error
 */
SHW_DECLARE_EVENT_CLASS( TShwEventFatalError );

/**
 * Selection key pressed for changed ui state (Hide or visible)
 */
SHW_DECLARE_EVENT_CLASS( TShwEventToggleControlUi );


#endif // __SHWEVENT_H__

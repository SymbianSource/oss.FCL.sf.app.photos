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
* Description:    The view control for the slideshow
 *
*/




#ifndef __CSHWVIEWCONTROL_H__
#define __CSHWVIEWCONTROL_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "shweventobserver.h"
#include "shweventpublisherbase.h"

// FORWARD DECLARATIONS
class MGlxMediaList;

// CLASS DECLARATION

/**
 * CShwViewControl
 * 
 * @author Kimmo Hoikka
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 */
NONSHARABLE_CLASS( CShwViewControl )
	: public CShwEventPublisherBase, public MShwEventObserver
	{
	public: // Constructors and destructor

		/**
		 * Constructor.
		 * @param aList, the list that is shown in slideshow
		 * @param aTransitionDuration, the duration for transition in milliseconds
		 * @param aViewDuration, the duration for view in milliseconds
		 */
		static CShwViewControl* NewL( 
			MGlxMediaList& aList, TInt aTransitionDuration, TInt aViewDuration );

		/**
		 * Destructor.
		 */
		~CShwViewControl();

	private:

		/**
		 * @param aList, the list that is shown in slideshow
		 * Constructor
		 */
		CShwViewControl( MGlxMediaList& aList );

		/**
		 * 2nd stage constructor
		 */
		void ConstructL();

	public: // From MShwEventObserver

		/// @ref MShwEventObserver::NotifyL
		void NotifyL( MShwEvent* aEvent );

	private: // Implementation

        /// Check if state allows to send start view and send it
	    void SendStartViewL();
        /// Check if state allows to send start transition and send it
	    void CheckAndSendStartTransitionL();

        /// Ref: the medialist
        MGlxMediaList& iList;
		/// Own: Transition duration
		TInt iTransitionDuration;
		/// Own: View duration
		TInt iViewDuration;
		/// Own: Flag to tell whether we received ready to start view
		TBool iReadyToViewReceived;
		/// Own: Flag to tell whether we received transition ready
		TBool iTransitionReadyReceived;
		/// Own: Flag to tell whether we received ready to advance
		TBool iReadyToAdvanceReceived;
		/// Own: Flag to tell whether we received timer beat
		TBool iTimerReceived;
		/// Own: Flag to tell that user navigated during pause
		TBool iUserNavigated;
		/// Own: Flag set to ETrue when paused, EFalse when playing
		TBool iPaused;
		/// Own: Flag to tell if user navigated while paused
		TBool iUserNavigatedWhilePaused;

	};

#endif // __CSHWVIEWCONTROL_H__

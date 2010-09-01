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
* Description:    The timer control for the slideshow
 *
*/




#ifndef __CSHWTIMERCONTROL_H__
#define __CSHWTIMERCONTROL_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "shweventobserver.h"
#include "shweventpublisherbase.h"

// FORWARD DECLARATION
class CShwTimer;

// CLASS DECLARATION

/**
 *  CShwTimerControl
 *
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 */
NONSHARABLE_CLASS( CShwTimerControl ) 
	: public CShwEventPublisherBase, public MShwEventObserver
	{
	public: // Constructors and destructor

		/**
		 * Constructor.
		 */
		static CShwTimerControl* NewL();

		/**
		 * Destructor.
		 */
		~CShwTimerControl();

	private:

		/**
		 * Constructor
		 */
		CShwTimerControl();

		/**
		 * 2nd stage constructor
		 */
		void ConstructL();
		
	public: // API

		/**
		 * This method is called when the time tick occurs
		 */
		TInt SendTimerBeatL();

	public: // From MShwEventObserver
		
		/// @ref MShwEventObserver::NotifyL
		void NotifyL( MShwEvent* aEvent );
		
	private:
		
		/// Own: the timer
		CShwTimer* iTimer;

	};

#endif // __CSHWTIMERCONTROL_H__

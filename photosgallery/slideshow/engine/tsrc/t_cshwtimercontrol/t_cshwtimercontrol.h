/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Test for timer control for the slideshow
 *
*/




#ifndef __T_CSHWTIMERCONTROL_H__
#define __T_CSHWTIMERCONTROL_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>
#include <digia/eunit/EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "shweventqueue.h"
#include "shwtimercontrol.h"

//  FORWARD DECLARATIONS

//  CLASS DEFINITION
/**
 * EUnit test suite for CShwTimerControl
 */
NONSHARABLE_CLASS( T_CShwTimerControl )
	: public CEUnitTestSuiteClass,
	public MShwEventQueue
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwTimerControl* NewL();
        static T_CShwTimerControl* NewLC();
        /**
         * Destructor
         */
        ~T_CShwTimerControl();

    private:    // Constructors and destructors

        T_CShwTimerControl();
        void ConstructL();

	public:	// From MShwEventQueue
	
		void SendEventL( MShwEvent* aEvent );

		// timer callback for test
    	TInt TimerCallBack();
		
    private:    // New methods
    
		void SetupL();
		void Teardown();
		void Empty();

		void TestConstructL();
		void T_TimeTickLL();
		void T_NotifyLL();
		void T_NotifyL2L();
        void T_PauseL();

    private:    // Data
		
		/// Own: the class under test
        CShwTimerControl* iCShwTimerControl;
		/// Own: the event received from cut
        MShwEvent* iEvent;
        /// Own: scheduler wait object
        CActiveSchedulerWait iWait;
        /// Own: a flag to know where the scheduler was stopped
        TBool iStoppedForTimer;


        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CSHWTIMERCONTROL_H__

// End of file

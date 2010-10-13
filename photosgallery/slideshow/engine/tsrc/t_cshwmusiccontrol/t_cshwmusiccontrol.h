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
* Description:  Test for music control for the slideshow
 *
*/




#ifndef __T_CSHWMUSICCONTROL_H__
#define __T_CSHWMUSICCONTROL_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "shweventqueue.h"
#include "shwmusiccontrol.h"
#include "shwmusicobserver.h"

//  FORWARD DECLARATIONS


const TInt KMaxFileLen = 256;

//  CLASS DEFINITION
/**
 * EUnit test suite for CShwMusicControl
 */
NONSHARABLE_CLASS( T_CShwMusicControl )
	: public CEUnitTestSuiteClass,
	  public MShwMusicObserver,
	  public MShwEventQueue
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwMusicControl* NewL();
        static T_CShwMusicControl* NewLC();
        /**
         * Destructor
         */
        ~T_CShwMusicControl();

    private:    // Constructors and destructors

        T_CShwMusicControl();
        void ConstructL();
	
	public: 
		static TInt StopPlaying(TAny* aMusicControl);

	public:	// From MShwEventQueue
	
		void SendEventL( MShwEvent* aEvent );
		

	public:	// From MShwMusicObserver
		
		void MusicOnL();
		void MusicOff();
		void MusicVolume(TInt aCurrentVolume, TInt aMaxVolume);
		void ErrorWithTrackL(TInt aErrorCode);
		
    private:    // New methods

		void SetupL();
		void Teardown();
		void Empty();

		void TestConstructL();
		void T_NotifyLL();
		void T_NotifyL1L();
		void T_NotifyL2L();
		void T_NotifyL3L();
		void T_NotifyL4L();
		void T_NotifyL5L();
		void T_ExtendedPlayL();
		void T_ErrorInFileL();
		void T_LeaveInObserverL();
		
    private:    // Data
		// The music state
		TBool iMusicOn;
		
		// The music volume
		TInt iCurrentVolume;
		
		// The maximum music volume
		TInt iMaxVolume;
		
		// The previous music volume setting 
		// - used to test volume up and down.
		TInt iPrevVolume;
				
		// Own: the class under test
        CShwMusicControl* iCShwMusicControl;

		// Own: the event received from SendEventL
        MShwEvent* iEvent;
        
        // Own: scheduler wait object
        CActiveSchedulerWait iWait;
        
        // Own: scheduler wait object
        CActiveSchedulerWait iPlayWait;
        
        // Own: a timer callback 
        CPeriodic* iTimer;
        
        /// Own: the error code from ErrorWithTrackL
        TInt iErrorCode;
        
        /// Own: error code to cause a leave in observer
        TInt iLeaveInObserver;
        
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CSHWMUSICCONTROL_H__

// End of file

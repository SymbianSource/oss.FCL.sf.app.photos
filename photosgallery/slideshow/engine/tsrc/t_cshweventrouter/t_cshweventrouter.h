/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Test for scheduler for the slideshow
 *
*/




#ifndef __T_CSHWEVENTROUTER_H__
#define __T_CSHWEVENTROUTER_H__

//  EXTERNAL INCLUDES
#include <e32def.h>
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "shweventobserver.h"
#include "shweventpublisher.h"

//  FORWARD DECLARATIONS
class CShwEventRouter;
class T_TestEvent;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( T_CShwEventRouter )
     : public CEUnitTestSuiteClass, public MShwEventObserver, public MShwEventPublisher
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwEventRouter* NewL();
        static T_CShwEventRouter* NewLC();
        /**
         * Destructor
         */
        ~T_CShwEventRouter();

    private:    // Constructors and destructors 

        T_CShwEventRouter();
        void ConstructL();

    public:     // From MShwEventObserver and MShwEventPublisher

		void NotifyL( MShwEvent* aEvent );
		void SetEventQueue( MShwEventQueue* aQueue );

    private:    // New methods

         void SetupL();
         void Teardown();
        
         void TestBasicRoutingL();
         void TestNestedEventsL();
         void TestNestedEventsAndMultipleObserversL();
         void TestLeaveInNotifyL();
         void TestOOML();
        

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE;

		CShwEventRouter* iRouter;
		MShwEventQueue* iQueue;
		MShwEvent* iEvent;
		
		TInt iEventReceiveCount;
		TInt iSendNewEventCount;
		TBool iLeaveInNotify;
		T_TestEvent* iEventToReceive;

    };

#endif      //  __T_CSHWEVENTROUTER_H__

// End of file

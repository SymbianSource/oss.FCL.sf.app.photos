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
* Description:   Test for view control for the slideshow
 *
*/




#ifndef __T_CSHWVIEWCONTROL_H__
#define __T_CSHWVIEWCONTROL_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>
#include <digia/eunit/EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "shwviewcontrol.h"
#include "shweventqueue.h"

#include "tmglxmedialist_stub.h"

//  FORWARD DECLARATIONS

//  CLASS DEFINITION
/**
 * Unit tests for the view control
 */
NONSHARABLE_CLASS( T_CShwViewControl )
	: public CEUnitTestSuiteClass,
	public MGlxMediaList_Stub_Observer,
	public MShwEventQueue
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwViewControl* NewL();
        static T_CShwViewControl* NewLC();
        /**
         * Destructor
         */
        ~T_CShwViewControl();

    private:    // Constructors and destructors

        T_CShwViewControl();
        void ConstructL();

	public:	// from MGlxMediaList_Stub_Observer, MShwEventQueue

		void MGlxMediaList_MethodCalled( TMGlxMediaListMethodId aMethodId );
		void SendEventL( MShwEvent* aEvent );

    private:    // New methods

         void SetupL();
         void Teardown();
         void TestNewlL();
         void TestNotifyL1L();
         void TestNotifyL2L();
         void TestNotifyL3L();
         void TestNotifyL4L();
         void TestNotifyL5L();
         void TestNotifyL6L();
         void TestNotifyL7L();
         void TestNotifyL8L();
         void TestNotifyL9L();
         void TestNotifyL10L();
         void TestNotifyL11L();
         void TestNotifyL12L();
         void TestNotifyL13L();
         void TestNotifyL14L();

    private:    // Data
		
        CShwViewControl* iCShwViewControl;
        
		TMGlxMediaList_Stub* iStubMediaList;
        
        MShwEvent* iEvent;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CSHWVIEWCONTROL_H__

// End of file

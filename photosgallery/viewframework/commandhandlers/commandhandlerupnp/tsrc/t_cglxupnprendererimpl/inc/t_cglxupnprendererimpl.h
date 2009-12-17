/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CommandHandlerUPnP RENDERER unit test case
*
*/




#ifndef __T_CGLXUPNPRENDERERIMPL_H__
#define __T_CGLXUPNPRENDERERIMPL_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>

#include <tmglxmedialist_stub.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxUpnpRendererImpl;

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_cglxupnprendererimpl )
	: public CEUnitTestSuiteClass ,	  
	  public MGlxMediaList_Stub_Observer
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static t_cglxupnprendererimpl* NewL();
        static t_cglxupnprendererimpl* NewLC();
        /**
         * Destructor
         */
        ~t_cglxupnprendererimpl();

    private:    // Constructors and destructors

        t_cglxupnprendererimpl();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_TestL();
         
         void T_TestL1();
         
         void T_IsSupported_Available();
         
         void T_IsSupported_NotAvailable();
        
         void T_HandleAttributesAvailable();
         //From the media list stub
         void MGlxMediaList_MethodCalled(MGlxMediaList_Stub_Observer::TMGlxMediaListMethodId aMethodId);

    private:    // Data
		

        EUNIT_DECLARE_TEST_TABLE; 
        
        CGlxUpnpRendererImpl* iUpnpRendererImpl;
        TMGlxMediaList_Stub* iMediaListStub;

    };

#endif      //  __T_CGLXUPNPRENDERERIMPL_H__

// End of file

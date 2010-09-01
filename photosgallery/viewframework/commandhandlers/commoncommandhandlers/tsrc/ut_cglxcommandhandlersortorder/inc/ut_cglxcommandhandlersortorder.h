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
* Description:  CommandHandlerSortOrder unit test cases
*
*/




#ifndef __UT_CGLXCOMMANDHANDLERSORTORDER_H__
#define __UT_CGLXCOMMANDHANDLERSORTORDER_H__
#include <mglxmedialistprovider.h>

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>

#include <mglxmedialistprovider.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class MGlxMediaList;
class CGlxCommandHandlerSortOrder;


//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( ut_cglxcommandhandlersortorder )
	: public CEUnitTestSuiteClass, public MGlxMediaListProvider
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static ut_cglxcommandhandlersortorder* NewL();
        static ut_cglxcommandhandlersortorder* NewLC();
        /**
         * Destructor
         */
        ~ut_cglxcommandhandlersortorder();

    private:    // Constructors and destructors

        ut_cglxcommandhandlersortorder();
        void ConstructL();

    public: // From MGlxMediaListProvider

        MGlxMediaList& MediaList();

    private:    // New methods

        // With medialist owner (filters applied) & valid setting key
        void SetupMethod1L();
        
        // With medialist owner (without filters) & valid setting key         
        void SetupMethod2L();
        
        // With medialist owner as NULL & valid setting key
        void SetupMethod3L();
        
        // With medialist owner (filters applied) & invalid setting key         
        void SetupMethod4L();                          
        
        // Clean up code
        void Teardown();
        
        // Test command handler creation
        void T_TestCreationL();
        
        // Test alphabetical sort
        void T_TestAlphaSortL();
        
        // Test alphabetical sort where the Medialist is NULL
        void T_TestAlphaSortWithNullListL();
        
        // Test Frequency sort
        void T_TestFrequencySortL();
        
        // Test Frequency sort where the Medialist is NULL
        void T_TestFrequencySortWithNullListL();
        
        // Test an invalid command with the command handler
        void T_TestInvalidSortL();
        
        // Test disable of alphabetical sort menu option
        void T_TestDisableAlphaL();
        
        // Test disable of alphabetical sort menu option where the Medialist is NULL
        void T_TestDisableAlphaWithNullListL();
        
        // Test disable of frequency sort menu option
        void T_TestDisableFrequencyL();
        
        // Test disable of frequency sort menu option where the Medialist is NULL
        void T_TestDisableFrequencyWithNullListL();
        
        // Test menu disable with invalid command id
        void T_TestDisableInvalidCommandL(); 

    private:    // Data
		
        CGlxCommandHandlerSortOrder* iCommand;
        MGlxMediaList* iMediaList;
        TBool iFilterFlag;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CGLXCOMMANDHANDLERSORTORDER_H__

// End of file

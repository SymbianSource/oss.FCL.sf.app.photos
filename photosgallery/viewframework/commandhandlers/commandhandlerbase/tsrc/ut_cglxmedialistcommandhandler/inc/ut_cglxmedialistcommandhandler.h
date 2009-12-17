/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CGlxMediaListCommandHandler unit test cases
*
*/




#ifndef __UT_CGLXMEDIALISTCOMMANDHANDLER_H__
#define __UT_CGLXMEDIALISTCOMMANDHANDLER_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
 
//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxMediaListCommandHandler;
class _CGlxTestMediaList;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CGlxMediaListCommandHandler )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxMediaListCommandHandler* NewL();
        static UT_CGlxMediaListCommandHandler* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxMediaListCommandHandler();

    private:    // Constructors and destructors

        UT_CGlxMediaListCommandHandler();
        void ConstructL();

    private:    // New methods

         void SetupL();

         void Teardown();

         void Test_SelectionLengthL();
         void Test_IsDisabledL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

        _CGlxTestMediaList* iML;
    };

#endif      //  __UT_CGLXMEDIALISTCOMMANDHANDLER_H__

// End of file

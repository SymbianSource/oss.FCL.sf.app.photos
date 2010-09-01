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
* Description:  CommandHandlerDetails unit test cases
*
*/




#ifndef __UT_PROPERTYCOMMANDHANDLERS_H__
#define __UT_PROPERTYCOMMANDHANDLERS_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <mglxmedialistprovider.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class _CGlxTestMediaList;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif


class CGlxCommandHandlerProperties;
//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_propertycommandhandlers )
     : public CEUnitTestSuiteClass , public MGlxMediaListProvider
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_propertycommandhandlers* NewL();
        static UT_propertycommandhandlers* NewLC();
        /**
         * Destructor
         */
        ~UT_propertycommandhandlers();

    private:    // Constructors and destructors

        UT_propertycommandhandlers();
        void ConstructL();

    public:     // From observer interface

        virtual MGlxMediaList& MediaList() ;

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void PropertyCommandHandlerNewL();
        
         void PropertyCommandHandlerExecuteL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        _CGlxTestMediaList* iMediaList;
        CGlxCommandHandlerProperties* iMetadataCommand;
        CGlxCommandHandlerProperties* iTagManagerCommand;

    };

#endif      //  __T_PROPERTYCOMMANDHANDLERS_H__

// End of file

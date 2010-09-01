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
* Description:  CommandHandlerBack Unit test cases
*
*/




#ifndef __T_CGLXCOMMANDHANDLERBACK_H__
#define __T_CGLXCOMMANDHANDLERBACK_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include <mglxmedialistprovider.h>


//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class MGlxMediaList;
class CGlxCommandHandlerBack;

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_cglxcommandhandlerback )
	: public CEUnitTestSuiteClass, public MGlxMediaListProvider
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static t_cglxcommandhandlerback* NewL();
        static t_cglxcommandhandlerback* NewLC();
        /**
         * Destructor
         */
        ~t_cglxcommandhandlerback();

    private:    // Constructors and destructors

        t_cglxcommandhandlerback();
        void ConstructL();

    public: // From MGlxMediaListOwner

        MGlxMediaList& MediaList();

    private:    // New methods

         void CreateMediaListL();

         void SetupBackChL();
         
         void SetupPreviousViewChWithoutMlL(  );
         
         void SetupPreviousViewChWithMlL(  );
         
         void SetupPreviousNaviViewChWithoutMlL( );
         
         void SetupPreviousNaviViewChWithMlL( );
         
         void SetupContainerPreviousViewChWithoutViewId(  );
         
         void SetupContainerPreviousViewChWithViewId(  );
        
         void Teardown();

         void T_TestConstructionL();
         
         void T_TestExecutionL();
                
         void T_TestInvalidExecutionL();
 
    private:    // Data

        CGlxCommandHandlerBack* iCommand;
        MGlxMediaList* iMediaList;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CGLXCOMMANDHANDLERBACK_H__

// End of file

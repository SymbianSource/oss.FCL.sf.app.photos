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
* Description:  AIW Service Handler stub implementation.
*
*/




#ifndef __t_CGlxAiwServiceHandler_H__
#define __t_CGlxAiwServiceHandler_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include <mglxmedialistprovider.h>
#include <EIKMOBS.H>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxAiwServiceHandler;

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_CGlxAiwServiceHandler )
	: public CEUnitTestSuiteClass, public MEikMenuObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static t_CGlxAiwServiceHandler* NewL();
        static t_CGlxAiwServiceHandler* NewLC();
        /**
         * Destructor
         */
        ~t_CGlxAiwServiceHandler();

    private:    // Constructors and destructors

        t_CGlxAiwServiceHandler();
        void ConstructL();

    private:    // From MEikMenuObserver
	    void SetEmphasis(CCoeControl* aMenuControl,TBool aEmphasis);
    	void ProcessCommandL(TInt aCommandId);

    private:    // New methods
    
         void SetupL();
        
         void Teardown();

         void TestConstructionL(  );
         
         void OneImageUniqueParamsL();

         void OneImageDuplicateParamsL();

         void TwoItemsUniqueParamsL();

         void TwoItemsDuplicateParamsL();

    private:    // Data
		
        CGlxAiwServiceHandler* iAiwServiceHandler;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __t_CGlxAiwServiceHandler_H__

// End of file

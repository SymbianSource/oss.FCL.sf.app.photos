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
* Description:  CommandHandlerUpNP unit test cases for upnp renderer
*
*/




#ifndef __T_GLXUPNPRENDERER_H__
#define __T_GLXUPNPRENDERER_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS


//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_glxupnprenderer )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static t_glxupnprenderer* NewL();
        static t_glxupnprenderer* NewLC();
        /**
         * Destructor
         */
        ~t_glxupnprenderer();

    private:    // Constructors and destructors

        t_glxupnprenderer();
        void ConstructL();

    private:    // New methods

         void T_StartShowingL();
         
         void T_StopShowingL();         
         
         
        
         void T_TestL();
         
         void T_ShowVideoL();
         
         void TearDown();
         
         //Status of the rendere
         
         void T_Status_Active();
         
         //UPnpShowCommand is Available and the impl pointer is not created
         void T_Status_AvailableNotActive();
         
         void T_Status_NotAvailable();         
         
         //UPnpShowCommand is Available and the impl pointer is created
         void T_Status_Active_Renderer();
        

    private:    // Data
		

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_GLXUPNPRENDERER_H__

// End of file

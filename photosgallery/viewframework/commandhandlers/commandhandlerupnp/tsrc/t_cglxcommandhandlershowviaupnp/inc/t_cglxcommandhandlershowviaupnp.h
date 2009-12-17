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
* Description:  CommandHandlerShowViaUPnP unit test cases
*
*/




#ifndef __t_cglxcommandhandlershowviaupnp_H__
#define __t_cglxcommandhandlershowviaupnp_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>
#include <digia/eunit/eunitdecorators.h>
#include <digia/eunit/ceunittestsuiteclass.h>


#include "glxcommandhandlershowviaupnp.h"
//#include <mglxmedialistowner.h>

#include <mglxmedialistprovider.h>
#include <mpxcollectionpath.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxUpnpRenderer;
class MGlxMediaList;
class _CGlxTestMediaList;

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_cglxcommandhandlershowviaupnp )
	: public CEUnitTestSuiteClass ,public MGlxMediaListProvider//public MGlxMediaListOwner
    {
    
       
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static t_cglxcommandhandlershowviaupnp* NewL();
        static t_cglxcommandhandlershowviaupnp* NewLC();
        /**
         * Destructor
         */
        ~t_cglxcommandhandlershowviaupnp();

    private:    // Constructors and destructors

        t_cglxcommandhandlershowviaupnp();
        void ConstructL();

    public:

         MGlxMediaList& MediaList();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CGlxCommandHandlerShowViaUpnpL();
        
         void SetupL1(); 

         //Test Add upnp command
      
         void T_AddUPnPCommandL_start();

         void T_AddUPnPCommandL_stop(); 
     
         void T_AddUPnPCommandL_NULL(); 
        
         void T_AddUPnPCommandL_Invalid(); 

         //Test DoIsDisabled - Home Network Not Available

         void T_DoIsDisabled_Show(); 

         void T_DoIsDisabled_Stop(); 
 
         void T_DoIsDisabled_NULL(); 

         void T_DoIsDisabled_Invalid();         
         
         //Test DoIsDisabled - Home Network
         
         void T_DoIsDisabled_Show_Available();
         
         void T_DoIsDisabled_Invalid_Available();
         
         void T_DoIsDisabled_NULL_Available();
         
         void T_DoIsDisabled_Stop_Available();

         //Test DoExecuteL
 
         void T_DoExecuteL_Show();
       
         void T_DoExecuteL_Stop(); 

         void T_DoExecuteL_NULL();
            
         void T_DoExecuteL_Invalid();    
         

    private:    // Data
		

        EUNIT_DECLARE_TEST_TABLE; 

    private:
        CGlxCommandHandlerShowViaUpnp* iCommandShowUpnp;
        

        _CGlxTestMediaList* iList;

    };

#endif      //  __t_cglxcommandhandlershowviaupnp_H__

// End of file

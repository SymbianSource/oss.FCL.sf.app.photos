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
* Description:    Upnp Command Handlers.
*
*/




#ifndef __T_CGLXCOMMANDHANDLERCOPYTOHOMENETWORK
#define __T_CGLXCOMMANDHANDLERCOPYTOHOMENETWORK

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>

#include "glxcommandhandlercopytohomenetwork.h"

#include <mglxmedialistprovider.h>
#include <mpxcollectionpath.h>

//  FORWARD DECLARATIONS
class MGlxMediaList;
class CGlxCommandHandlerCopyToHomeNetwork;

//  CLASS DEFINITION
NONSHARABLE_CLASS( t_cglxcommandhandlercopytohomenetwork )
	: public CEUnitTestSuiteClass ,public MGlxMediaListProvider//public MGlxMediaListOwner
    {   
    public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static t_cglxcommandhandlercopytohomenetwork* NewL();
    static t_cglxcommandhandlercopytohomenetwork* NewLC();
    /**
     * Destructor
     */
    ~t_cglxcommandhandlercopytohomenetwork();

    private:    // Constructors and destructors
        t_cglxcommandhandlercopytohomenetwork();
        
        void ConstructL();

    public:
         MGlxMediaList& MediaList();

    private:    // New methods
        
         void Teardown();
        
         void SetupL(); 

         void T_AddCopyToHomeNetworkCommandL_L();

         void T_DoExecuteL_L();
         
         void T_DoIsDisabled_L();
         
    private:    // Data
        EUNIT_DECLARE_TEST_TABLE; 

    private:
        CGlxCommandHandlerCopyToHomeNetwork* iCommandHandlerCopyToHomeNetwork;
        MGlxMediaList* iList;

    };

#endif      //  __T_CGLXCOMMANDHANDLERCOPYTOHOMENETWORK

// End of file

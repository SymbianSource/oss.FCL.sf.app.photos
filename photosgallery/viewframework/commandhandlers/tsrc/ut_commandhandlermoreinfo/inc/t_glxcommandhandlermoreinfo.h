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
* Description:  CGlxCommandHandlerMoreInfo unit test cases
*
*/




#ifndef __T_GLXCOMMANDHANDLERMOREINFO_H__
#define __T_GLXCOMMANDHANDLERMOREINFO_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>

#include <mglxcommandhandler.h>
#include <glxcommandhandler.h>
#include <mglxmedialistprovider.h>
#include <glxlistdefs.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxCommandHandlerMoreInfo;
class _CGlxTestMediaList;
class CGlxMedia;
NONSHARABLE_CLASS( CGlxCommandHandlerMoreInfoTester): public CBase, 
                                       public MGlxMediaListProvider
    {
public:
	static CGlxCommandHandlerMoreInfoTester* NewL();
	~CGlxCommandHandlerMoreInfoTester();
	
	TBool ExecuteL(TInt aCommand);
		
	void AddItemL(TInt aId, CGlxMedia* aMedia);
	
	
public: // From MGlxMediaListProvider
	virtual MGlxMediaList& MediaList();

private:
	CGlxCommandHandlerMoreInfoTester();
	void ConstructL();
	
private:
	CGlxCommandHandlerMoreInfo* iMoreInfoHandler;
	
	_CGlxTestMediaList* iMediaList;
	
	RPointerArray<CGlxCommandHandler> iCommandHandlerList; 
    };

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_glxcommandhandlermoreinfo )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static t_glxcommandhandlermoreinfo* NewL();
        static t_glxcommandhandlermoreinfo* NewLC();
        /**
         * Destructor
         */
        ~t_glxcommandhandlermoreinfo();

    private:    // Constructors and destructors

        t_glxcommandhandlermoreinfo();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_TestL();
         
         void T_TestExecutionL();
        

    private:    // Data
		CGlxCommandHandlerMoreInfoTester* iTester;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_GLXCOMMANDHANDLERMOREINFO_H__

// End of file

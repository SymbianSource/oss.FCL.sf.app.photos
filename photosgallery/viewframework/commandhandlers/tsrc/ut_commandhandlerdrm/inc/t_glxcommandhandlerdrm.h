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
* Description:  CGlxCommandHandlerDRM unit test cases
*
*/




#ifndef __T_GLXCOMMANDHANDLERDRM_H__
#define __T_GLXCOMMANDHANDLERDRM_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include <glxcommandhandler.h>
#include <mglxmedialistprovider.h>
#include <glxlistdefs.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxCommandHandlerDrm;
class _CGlxTestMediaList;
class CGlxMedia;
class CGlxUiUtility;
NONSHARABLE_CLASS( CGlxCommandHandlerDRMTester): public CBase, 
                                       public MGlxMediaListProvider
    {
public:
	static CGlxCommandHandlerDRMTester* NewL();
	~CGlxCommandHandlerDRMTester();
	
	TBool ExecuteL(TInt aCommand);
	
	void AddNonProtectedItemL();
	
	void AddProtectedItemL(TInt aId, CGlxMedia* aMedia);
	
	void ForceFocusChangeEventL( TInt aFocus );
	
	void ForceHandleAttributeL( TInt aFocus );
	
public: // From MGlxMediaListProvider
	virtual MGlxMediaList& MediaList();

private:
	CGlxCommandHandlerDRMTester();
	void ConstructL();
	
private:
	CGlxCommandHandlerDrm* iDRMHandler;
	
	_CGlxTestMediaList* iMediaList;
	
	RPointerArray<CGlxCommandHandler> iCommandHandlerList; 
	
	CGlxUiUtility* iDummyUiUtility;
    };

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_GlxCommandHandlerDRM )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_GlxCommandHandlerDRM* NewL();
        static T_GlxCommandHandlerDRM* NewLC();
        /**
         * Destructor
         */
        ~T_GlxCommandHandlerDRM();

    private:    // Constructors and destructors

        T_GlxCommandHandlerDRM();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CreateL();
         
         void T_TestEmptyMediaListL();
         
         void T_TestHandleCommandsL();
         
         void T_TestHandleFocusChangedL();
         
         void T_TestHandleAttributeL();
        

    private:    // Data
	    CGlxCommandHandlerDRMTester* iDRMHandlerTester;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_GLXCOMMANDHANDLERDRM_H__

// End of file

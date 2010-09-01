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
* Description:  CommandHandlerMarking unit test cases
*
*/




#ifndef __T_GLXCOMMANDHANDLERMARKING_H__
#define __T_GLXCOMMANDHANDLERMARKING_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "glxcommandhandlermarking.h"
#include <mglxmedialistprovider.h>
#include <mglxsoftkeyhandler.h>
#include <glxcommandhandler.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class _CGlxTestMediaList;

//  CLASS DEFINITION

class CGlxMarkingCommandHandlerTester: public CBase, 
                                       public MGlxMediaListProvider,
                                       public MGlxSoftkeyHandler
	{
public:
	static CGlxMarkingCommandHandlerTester* NewL();
	~CGlxMarkingCommandHandlerTester();
	
	TBool ExecuteL(TInt aCommand);
	
	TBool OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	
	void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex);
	
public: // From MGlxMediaListProvider
	virtual MGlxMediaList& MediaList();
	
public: // From 	MGlxSoftkeyHandler
    void StoreCurrentSoftKeysL() {};
    
    void ChangeSoftkeyL(TGlxSoftkey , TInt ,  
                                        const TDesC& ) {};
    
    void RestoreSoftKeysAndTitleL() {};
    
    void EnableSoftkey( TBool, TGlxSoftkey ) {};

private:
	CGlxMarkingCommandHandlerTester();
	void ConstructL();
	
private:
	CGlxCommandHandlerMarking* iMarker;
	
	_CGlxTestMediaList* iMediaList;
	
	RPointerArray<CGlxCommandHandler> iCommandHandlerList; 
	};

/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( ut__commandhandlermarking )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static ut__commandhandlermarking* NewL();
        static ut__commandhandlermarking* NewLC();
        /**
         * Destructor
         */
        ~ut__commandhandlermarking();

    private:    // Constructors and destructors

        ut__commandhandlermarking();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_TestCreateL();
         
         void T_TestExecuteNoItemsL();
         
         void T_TestExecuteItemsL();
         
         void T_TestOfferKeyL();
        

    private:    // Data
		CGlxMarkingCommandHandlerTester* iTester;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_GLXCOMMANDHANDLERMARKING_H__

// End of file

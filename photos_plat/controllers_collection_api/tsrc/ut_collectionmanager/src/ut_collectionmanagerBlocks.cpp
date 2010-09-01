/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32base.h>
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <glxcollectionmanager.h>

#include "ut_collectionmanager.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cut_collectionmanager::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cut_collectionmanager::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// Cut_collectionmanager::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cut_collectionmanager::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        ENTRY( "NewL", Cut_collectionmanager::NewLTestL ),
        ENTRY( "Destructor", Cut_collectionmanager::DestructionTestL ),
        ENTRY( "AddToCollection", Cut_collectionmanager::AddToCollectionTestL),
        ENTRY( "CollectionInfoL", Cut_collectionmanager::TestCollectionInfoL ),
        ENTRY( "Cancel", Cut_collectionmanager::TestCancelL ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );
    
    
    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// Cut_collectionmanager::NewLTestL
// -----------------------------------------------------------------------------
//
TInt Cut_collectionmanager::NewLTestL( CStifItemParser& /*aItem*/ )
    {       
    // Create CollectionManager Instance
    CGlxCollectionManager* collectionManager = CGlxCollectionManager::NewL();
    
    if( collectionManager )
        {
        // Print test case passed if CGlxCollectionManager instance is created
        iLog->Log(_L("CGlxCollectionManager::NewL() Passed"));      
        delete collectionManager;
        collectionManager = NULL;
        User::InfoPrint(_L("test newL passed"));
        return KErrNone;
        }
    else
        {
        // Print test case passed if CGlxCollectionManager instance is created
        iLog->Log(_L("CGlxCollectionManager::NewL() Failed"));   
        User::InfoPrint(_L("test newL failed"));
        return KErrArgument;
        }   
    }

// -----------------------------------------------------------------------------
// Cut_collectionmanager::DestructionTestL
// -----------------------------------------------------------------------------
//
TInt Cut_collectionmanager::DestructionTestL(CStifItemParser& /*aItem*/)
    {
    _LIT( KDestructionTest, "DestructionTest" );    

    // Print to log file
    iLog->Log( KDestructionTest );

    __UHEAP_MARK;    
    // Create CollectionManager Instance
    CGlxCollectionManager* collectionManager = CGlxCollectionManager::NewL();
    delete collectionManager;
    collectionManager = NULL;
    __UHEAP_MARKEND;
        
    iLog->Log(_L("~CollectionManager() :: Passed"));     
    User::InfoPrint(_L("test ~CollectionManager() Passed"));

    return KErrNone;       
    }

// -----------------------------------------------------------------------------
// Cut_collectionmanager::AddToCollectionTestL
// -----------------------------------------------------------------------------
//
TInt Cut_collectionmanager::AddToCollectionTestL(CStifItemParser& aItem)
    {
	TInt error = KErrNone ;
	TRAP(error,	
    if(iAo)
        {
        //iAo->TestingFunc(aItem) ;//uncomment it later when pass the args from .cfg file and parse it in parse func
        iAo->AddToCollectionTestL();
        iWait->Start(); 
        }
    );
	return error;
	}

// -----------------------------------------------------------------------------
// Cut_collectionmanager::TestCollectionInfoL
// -----------------------------------------------------------------------------
//
TInt Cut_collectionmanager::TestCollectionInfoL()
    { 
    TInt error = KErrNone ;
	TRAP(error,
	if(iAo)
        {
        iAo->TestCollectionInfoL() ;
        iWait->Start(); 
        }
		);
	return error;
    }

// -----------------------------------------------------------------------------
// Cut_collectionmanager::TestCancelL
// -----------------------------------------------------------------------------
//
TInt Cut_collectionmanager::TestCancelL()
    {
	TInt error = KErrNone ;
	TRAP(error,
    if(iAo)
        {
        iAo->TestCancel() ;
        iWait->Start(); 
        }
		);
	return error;
    }


/****
 * callback from ut_collectionmanager.cpp to report the pass/fail
 */
void Cut_collectionmanager::RequestComplete(TInt aError)
    {
    iWait->AsyncStop(); 
    if( aError == KErrNone )
        {
        iLog->Log(_L("Test case  passed"));
        User::InfoPrint(_L("Test case  passed"));
        }
    else if(aError == KErrCancel)
        {
        iLog->Log(_L("cancel Test case  passed"));
        User::InfoPrint(_L("cancel Test case  passed"));
        }
    else
        {
        iLog->Log(_L("Test case Failed"));
        User::InfoPrint(_L("test case  failed"));
        }
    }

//  [End of File] - Do not remove

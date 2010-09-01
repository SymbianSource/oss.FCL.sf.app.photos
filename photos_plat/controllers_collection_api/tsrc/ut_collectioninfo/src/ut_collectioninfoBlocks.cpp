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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <glxcollectionmanager.h>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectioninfo.h>
#include "ut_collectioninfo.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cut_collectioninfo::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cut_collectioninfo::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// Cut_collectioninfo::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cut_collectioninfo::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
                //ADD NEW ENTRY HERE
        ENTRY( "TestNewL", Cut_collectioninfo::Test_NewL ),
        ENTRY( "TestExistL", Cut_collectioninfo::Test_ExistsL ),
        ENTRY( "TestTitleL", Cut_collectioninfo::Test_TitleL ),
        ENTRY( "TestIdL", Cut_collectioninfo::Test_IdL ),
        ENTRY( "TestDestructorL", Cut_collectioninfo::Test_DestructorL ),
        
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cut_collectioninfo::Test_NewL
// Test for CGlxCollectionInfo::NewL API 
// -----------------------------------------------------------------------------

TInt Cut_collectioninfo::Test_NewL( CStifItemParser& /*aItem */)
    {
    // Print to UI
    _LIT( KTest_one, "Test_one" );
    _LIT( KTestText, "In Test_NewL" );
    TestModuleIf().Printf( 0, KTest_one, KTestText );
    // Print to log file
    iLog->Log( KTestText ); 

   	TRAPD(err,iCollectionInfo = CGlxCollectionInfo::NewL(KGlxCollectionPluginAlbumsImplementationUid));
    if(iCollectionInfo)
		{	
		// Test case passed if CGlxCollectionInfo instance is created
	    iLog->Log(_L("CGlxCollectionManager::NewL() Passed"));      
	    delete iCollectionInfo;
	    iCollectionInfo = NULL;
	    User::InfoPrint(_L("test CGlxCollectionInfo::NewL passed"));
	    return KErrNone;	    	
		}
	else
	   {
	   //  Test case Failed if CGlxCollectionManager instance is not created
	   iLog->Log(_L("CGlxCollectionManager::NewL() Failed"));      
	   User::InfoPrint(_L("test newL failed"));
	   return err;
	   }   		  
    }

// -----------------------------------------------------------------------------
// Cut_collectioninfo::Test_ExistsL
// Test for CGlxCollectionInfo::Exists() API 
// Creates the CGlxCollectionInfo instance and calls the Exists() API 
// -----------------------------------------------------------------------------

TInt Cut_collectioninfo::Test_ExistsL( CStifItemParser& /*aItem */)
	{
	 // Print to UI
    _LIT( KTest_two, "Test_two" );
    _LIT( KTestText, "In Test_ExistsL" );
    TestModuleIf().Printf( 0, KTest_two, KTestText );
    // Print to log file
    iLog->Log( KTestText );
    
	TInt apiErr = KErrNone;
	if(!iCollectionInfo)
		{
		TRAPD(err,iCollectionInfo = CGlxCollectionInfo::NewL(KGlxCollectionPluginAlbumsImplementationUid));
		if(iCollectionInfo)
			{
			apiErr = iCollectionInfo->Exists();
			if(!apiErr) //true value // @ToDo: (!apiErr) must chage to (apiErr) and we need to have 2 test cases (1) for case where a collection exists and it is tested for existence. (2) where a collection does not exist and it is tested for non-existence.
				{
				iLog->Log(_L("CGlxCollectionManager::Exists() Passed"));
				User::InfoPrint(_L("Test Exists Passed"));
				return KErrNone;
				}
			else 
				{
				iLog->Log(_L("CGlxCollectionManager::Exists() Failed"));
				User::InfoPrint(_L("Test Exists Failed"));
				return KErrNotFound;
				}					
			}
		else
			{
			return err;
			}		
		}					
	}

// -----------------------------------------------------------------------------
// Cut_collectioninfo::Test_TitleL
// Test for CGlxCollectionInfo::Title() API 
// 
// -----------------------------------------------------------------------------
//
TInt Cut_collectioninfo::Test_TitleL( CStifItemParser& /*aItem */)
	{
	 // Print to UI
    _LIT( KTest_three, "Test_three" );
    _LIT( KTestText, "In Test_TitleL" );
    TestModuleIf().Printf( 0, KTest_three, KTestText );
    // Print to log file
    iLog->Log( KTestText );
    
	TRAPD(err,iCollectionInfo = CGlxCollectionInfo::NewL(KGlxCollectionPluginAlbumsImplementationUid));
	HBufC* textResource = NULL;
	if(iCollectionInfo)
		{
		textResource = iCollectionInfo->Title().AllocLC();
		if(textResource->Length() != 0) //title of collection info is returned
			{
			//passed
			iLog->Log(_L("CGlxCollectionManager::Title()  Passed"));
			User::InfoPrint(_L("Test Title Passed"));
			CleanupStack::PopAndDestroy( textResource );
			return KErrNone;
			}
		else
			{
			if(textResource->Des() == KNullDesC) //title is returned as KNullDesC
				{
				//passed since Title() may return a KNullDesC
				iLog->Log(_L("CGlxCollectionManager::Title()  Passed"));
				User::InfoPrint(_L("Test Title Passed"));
				CleanupStack::PopAndDestroy( textResource );
				return KErrNone;	
				}
			else
				{
				//failed if its a junk value, not sure of how to check for this
				//as the API checks and returns null value if the title does
				//exist and hence there cannot be a failure scenario
				iLog->Log(_L("CGlxCollectionManager::Title() Failed"));
				User::InfoPrint(_L("Test Title Failed"));
				CleanupStack::PopAndDestroy( textResource );
				//Returning KErrNotFound as Title is not available. Still not 100% sure of this.
				return KErrNotFound;	
				}		
			}	
		}
	else
		{
		return err;
		}	
	}
// -----------------------------------------------------------------------------
// Cut_collectioninfo::Test_IdL
// Test for CGlxCollectionInfo::Id() API 
// 
// -----------------------------------------------------------------------------
//	
TInt Cut_collectioninfo::Test_IdL( CStifItemParser& /*aItem */)
	{
	 // Print to UI
    _LIT( KTest_four, "Test_four" );
    _LIT( KTestText, "In Test_IdL" );
    TestModuleIf().Printf( 0, KTest_four, KTestText );
    // Print to log file
    iLog->Log( KTestText );
    TInt err = KErrNone;;
	if(!iCollectionInfo)
		TRAP(err,iCollectionInfo = CGlxCollectionInfo::NewL(KGlxCollectionPluginAlbumsImplementationUid));
	if(iCollectionInfo)
		{
		TUint32 collectionId = 0;
		collectionId = iCollectionInfo->Id();
		if(collectionId) //if non zero
			{
			//passed
			iLog->Log(_L("CGlxCollectionManager::Id()  Passed"));			
			User::InfoPrint(_L("Test Id Passed"));
			return KErrNone;
			}
		else
			{
			//failed if zero
			iLog->Log(_L("CGlxCollectionManager::Id()  Failed"));
			User::InfoPrint(_L("Test Id failed"));
			//Returning KErrNotFound as Id() is not available. Still not 100% sure of this.
			return KErrNotFound;
			}
		}
	else
		{			
		return err;	
		}	
}
// -----------------------------------------------------------------------------
// Cut_collectioninfo::Test_DestructorL
// Test for CGlxCollectionInfo::~CGlxCollectionInfo() API 
// Checks if instance is correctly deleted without any memory leaks
// -----------------------------------------------------------------------------
//
TInt Cut_collectioninfo::Test_DestructorL( CStifItemParser& /*aItem */)
	{
	// Print to UI
    _LIT( KTest_five, "Test_five" );
    _LIT( KTestText, "In Test_DestructorL" );
    TestModuleIf().Printf( 0, KTest_five, KTestText );
    // Print to log file
    iLog->Log( KTestText );	
    
    TInt err = KErrNone;
	if(!iCollectionInfo)
		TRAPD(err,iCollectionInfo = CGlxCollectionInfo::NewL(KGlxCollectionPluginAlbumsImplementationUid));
	if(iCollectionInfo)
		{
		__UHEAP_MARK;
		delete iCollectionInfo;			
		iCollectionInfo = NULL;
		__UHEAP_MARKEND;
		iLog->Log(_L("~CollectionManager() :: Passed"));
		User::InfoPrint(_L("Test Destructor Passed"));
		return KErrNone;
		}
	else
		{
		return err;	
		}			
	}
	
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove

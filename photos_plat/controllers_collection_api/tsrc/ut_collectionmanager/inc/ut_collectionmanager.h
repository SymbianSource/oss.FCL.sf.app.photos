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
* Description: STIF testclass declaration
* 
*
*/


#ifndef UT_COLLECTIONMANAGER_H
#define UT_COLLECTIONMANAGER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include "glxcollectionmanagerao.h"

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( Kut_collectionmanagerLogPath, "\\logs\\testframework\\ut_collectionmanager\\" ); 
// Log file
_LIT( Kut_collectionmanagerLogFile, "ut_collectionmanager.txt" ); 
_LIT( Kut_collectionmanagerLogFileWithTitle, "ut_collectionmanager_[%S].txt" );

// FORWARD DECLARATIONS
class Cut_collectionmanager;


// CLASS DECLARATION
class CGlxCollectionManagerAO ;

/**
*  Cut_collectionmanager test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cut_collectionmanager) : public CScriptBase,public MObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cut_collectionmanager* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cut_collectionmanager();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        Cut_collectionmanager( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */
        
        /**
         * Test case for CollectionManager object creation
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code
         */
        virtual TInt NewLTestL( CStifItemParser& aItem );
        
        /**
         * Test case for CollectionManager object Destruction
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code.
         */
        virtual TInt DestructionTestL(CStifItemParser& aItem);
                
        /**
         * Test case to check whether the image at the specified URI 
         * has been added to the specified collection. 
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code.
         */
        virtual TInt AddToCollectionTestL(CStifItemParser& aItem);
                 
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        void RequestComplete(TInt aError);

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        virtual TInt TestCollectionInfoL();
        virtual TInt TestCancelL();

    private:    // Friend classes
        CGlxCollectionManagerAO* iAo;
        CActiveSchedulerWait* iWait;
    };

#endif      // UT_COLLECTIONMANAGER_H

// End of File

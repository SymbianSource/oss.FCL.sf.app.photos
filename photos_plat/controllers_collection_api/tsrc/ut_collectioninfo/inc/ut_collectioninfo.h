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
*/

#ifndef UT_COLLECTIONINFO_H
#define UT_COLLECTIONINFO_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <e32base.h>
#include <glxcollectioninfo.h>

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( Kut_collectioninfoLogPath, "\\logs\\testframework\\ut_collectioninfo\\" ); 
// Log file
_LIT( Kut_collectioninfoLogFile, "ut_collectioninfo.txt" ); 
_LIT( Kut_collectioninfoLogFileWithTitle, "ut_collectioninfo_[%S].txt" );

// FORWARD DECLARATIONS
class Cut_collectioninfo;

// CLASS DECLARATION

/**
*  Cut_collectioninfo test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cut_collectioninfo) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cut_collectioninfo* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cut_collectioninfo();

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
        Cut_collectioninfo( CTestModuleIf& aTestModuleIf );

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
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
                virtual TInt Test_NewL( CStifItemParser& aItem );
        virtual TInt Test_ExistsL (CStifItemParser& aItem );
        virtual TInt Test_TitleL (CStifItemParser& aItem );
        virtual TInt Test_IdL (CStifItemParser& aItem );        
        virtual TInt Test_DestructorL (CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;
        CGlxCollectionInfo* iCollectionInfo;
    };

#endif      // UT_COLLECTIONINFO_H

// End of File

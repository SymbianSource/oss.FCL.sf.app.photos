/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Test for settings model for the slideshow
 *
*/





#ifndef __T_CSHWSETTINGSMODEL_H__
#define __T_CSHWSETTINGSMODEL_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CShwSettingsModel;

//  CLASS DEFINITION
/**
 *
 * Test class for CShwSettingsModel
 *
 */
NONSHARABLE_CLASS( T_CShwSettingsModel )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwSettingsModel* NewL();
        static T_CShwSettingsModel* NewLC();
        /**
         * Destructor
         */
        ~T_CShwSettingsModel();

    private:    // Constructors and destructors

        T_CShwSettingsModel();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void TestGetDefaultValues();
        
         void TestSetAndGetMusicPathL();
         
         void TestSetAndGetMusicOnOffL();
         
         void TestSetAndGetTransDelayL();
         
         void TestSetAndGetTransitionTypeL();
        

    private:    // Data

		CShwSettingsModel* iShwSettingsModel;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif // __T_CSHWSETTINGSMODEL_H__

// End of file

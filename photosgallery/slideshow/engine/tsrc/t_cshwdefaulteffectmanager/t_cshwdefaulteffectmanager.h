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
* Description:   Test for default effect order manager
 *
*/




#ifndef __T_CSHWDEFAULTEFFECTMANAGER_H__
#define __T_CSHWDEFAULTEFFECTMANAGER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>
#include <digia/eunit/EUnitDecorators.h>


//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CShwDefaultEffectManager;
class CShwCrossFadeEffect;
class CShwZoomAndPanEffect;

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_CShwDefaultEffectManager )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwDefaultEffectManager* NewL();
        static T_CShwDefaultEffectManager* NewLC();
        /**
         * Destructor
         */
        ~T_CShwDefaultEffectManager();

    private:    // Constructors and destructors

        T_CShwDefaultEffectManager();
        void ConstructL();

    private:    // New methods

        void EmptySetupL();
        void SetupL();
        
        void Teardown();
        
        void TestConstructionL();
        void TestAddEffectL();
        void TestAddNullEffectL();
        void TestNextEffectL();
        void TestProgrammedEffectL();
        void TestMultiProgrammedEffectL();

    private:    // Data
		CShwDefaultEffectManager* iDefaultEffectManager;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __T_CSHWDEFAULTEFFECTMANAGER_H__

// End of file

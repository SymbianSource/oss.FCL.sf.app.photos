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
* Description:  Test for zoom and pan effect in slideshow
 *
*/




#ifndef __T_CSHWZOOMANDPANEFFECT_H__
#define __T_CSHWZOOMANDPANEFFECT_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>
#include <digia/eunit/EUnitDecorators.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class CHuiEnv;
class CHuiDisplayCoeControl;
class CHuiControl;
class CHuiImageVisual;
class CShwZoomAndPanEffect;

//  CLASS DEFINITION
/**
 * EUnit test suite for Zoom and pan related classes
 */
NONSHARABLE_CLASS( T_CShwZoomAndPanEffect )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwZoomAndPanEffect* NewL();
        static T_CShwZoomAndPanEffect* NewLC();
        /**
         * Destructor
         */
        ~T_CShwZoomAndPanEffect();

		// helper
		TInt CancelAsyncL();

    private:    // Constructors and destructors

        T_CShwZoomAndPanEffect();
        void ConstructL();

    private:    // New methods

		void Empty();
        void SetupL();
        void Teardown();
        void T_LayoutTestL();
        void T_CurveTestL();
        void T_ZoomAndPanTestL();
        void T_PauseTestL();
        void T_TestGeometryAlgorithmsL();
        void T_TestBoundariesL();

    private:    // Data

		/// Own: HUI environment
		CHuiEnv* iEnv;

		/// Own: HUI display		
		CHuiDisplayCoeControl* iCoeDisplay;

		/// Own: HUI control
		CHuiControl* iControl;

		/// Own: HUI visual
		CHuiImageVisual* iVisual;
		
		/// Own: asynch wait
		CActiveSchedulerWait iAsyncWait;

		/// Own: class under test
		CShwZoomAndPanEffect* iCShwZoomAndPanEffect;
		EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CSHWZOOMANDPANEFFECT_H__

// End of file

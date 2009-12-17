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
* Description:  Test for control for the slideshow
 *
*/




#ifndef __T_CSHWEFFECT_H__
#define __T_CSHWEFFECT_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>
#include <digia/eunit/EUnitDecorators.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class MShwEffect;
class CHuiEnv;
class CHuiDisplayCoeControl;
class CHuiControl;
class CHuiImageVisual;

//	TYPE DEFINITIONS
typedef MShwEffect* (T_ShwEffectFactoryL)();

//  CLASS DEFINITION
/**
 * EUnit test suite for slideshow effects
 */
NONSHARABLE_CLASS( T_CShwEffects )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

		/**
		 * Two phase construction
		 */
		static T_CShwEffects* NewLC( T_ShwEffectFactoryL* aEffectFactory );

		/**
		 * Destructor
		 */
		~T_CShwEffects();

    private:    // Constructors and destructors

		T_CShwEffects();
		void ConstructL();

    private:    // New methods

		void EmptyL();
		void SetupL();
		void Teardown();

		void T_ConstructorL();
		void T_PrepareViewLL();
		void T_EnterViewLL();
		void T_ExitViewL();
		void T_EnterTransitionLL();
		void T_ExitTransitionL();
		void T_PauseLL();
		void T_ResumeL();
		void T_EffectInfoL();

    private:    // Data

		T_ShwEffectFactoryL* iEffectFactory;
		MShwEffect* iEffect;

		CHuiEnv* iEnv;
		CHuiDisplayCoeControl* iCoeDisplay;
		CHuiControl* iControl;
		CHuiImageVisual* iVisual;


		EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CSHWEFFECT_H__

// End of file

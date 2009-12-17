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
* Description:   Test for control for the slideshow
 *
*/




//  CLASS HEADER
#include "t_cshweffects.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuite.h>

// CLASS UNDER TEST
#include "shwcrossfadeeffect.h"
#include "shwzoomandpaneffect.h"

MShwEffect* CreateCrossfadeL()
	{
	// create the normal zoom and pan
	MShwEffect* eff = CShwCrossFadeEffect::NewLC();
	CleanupStack::Pop();
	return eff;
	}

MShwEffect* CreateZoomAndPanL()
	{
	MShwEffect* eff = CShwZoomAndPanEffect::NewLC();
	CleanupStack::Pop();
	return eff;
	}

EXPORT_C MEUnitTest* CreateTestSuiteL()
	{
	// Create a root suite to contain tests for both the effects
	CEUnitTestSuite* rootSuite = 
		CEUnitTestSuite::NewLC( _L("ShwEffect Unit Tests") );

	// Note that NewLC leaves the object in the cleanupstack.
	rootSuite->AddL( T_CShwEffects::NewLC( &CreateCrossfadeL ) );
	CleanupStack::Pop(); // T_CShwEffects instance

	rootSuite->AddL( T_CShwEffects::NewLC( &CreateZoomAndPanL ) );
	CleanupStack::Pop(); // T_CShwEffects instance

	CleanupStack::Pop( rootSuite ); // rootSuite instance

	return rootSuite;
	}

//  END OF FILE

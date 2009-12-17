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
* Description:  Test for timer control for the slideshow
 *
*/




//  CLASS HEADER
#include "t_cshwtimercontrol.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuite.h>

EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    return T_CShwTimerControl::NewL();
    }

//  END OF FILE

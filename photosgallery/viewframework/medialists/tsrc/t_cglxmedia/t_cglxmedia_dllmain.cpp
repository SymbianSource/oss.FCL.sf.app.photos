/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    CGlxMedia unit test entry point
*
*/




//  CLASS HEADER
#include "t_cglxmedia.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>

EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    return T_CGlxMedia::NewL();
    }

#ifndef EKA2
GLDEF_C TInt E32Dll( TDllReason )
	{
	return KErrNone;
	}
#endif

//  END OF FILE
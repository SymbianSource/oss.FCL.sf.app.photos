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
* Description:    Show commmand handler unit test
*
*/




// Stub for the HuiUtility. This is necessary as the real HuiUtility
// causes resource leaks

#include <glxhuiutility.h>
#include "glxhuiutility_stub.h"

/**
 * @internal reviewed 08/02/2008 by Rhodri Byles
 */

TGlxNavigationDirection g_NavigationDirection = EGlxNavigationForwards;

CGlxHuiUtility* CGlxHuiUtility::UtilityL()
    {
    // In this test case and this case only, return NULL is safe
    // because nothing dereferences the "this" pointer
    return NULL;
    }
    
void CGlxHuiUtility::Close()
    {
    
    }

TGlxNavigationDirection CGlxHuiUtility::ViewNavigationDirection()
    {
    return g_NavigationDirection;
    }
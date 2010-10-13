/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Alf visual utilities
*
*/




#ifndef __GLXVISUALUTILITIES_H__
#define __GLXVISUALUTILITIES_H__

// EXTERNAL HEADERS
#include <e32std.h>

// FORWARD DECLARES
class CHuiVisual;
class CHuiControl;

/**
 * Namespace to hold the CHuiVisual related algorithms.
 *
 * @author Kimmo Hoikka
 * @lib glxhuiutils.lib
 */
namespace NGlxVisualUtilities
    {
    /**
     * Method to transfer the ownership of CHuiVisuals to a new CHuiControl.
     * In case of a Leave the ownership is held in the old owner so the ownership transfer
     * succeeds either completely or not at all.
     * @param aVisual reference to the topmost visual in the hierarchy
     * @param aNewParent the new parent to be applied to all visuals in the hierarchy
     */
    IMPORT_C void TransferVisualsL( CHuiVisual& aVisual, CHuiControl& aNewParent );
    }

#endif // __GLXVISUALUTILITIES_H__

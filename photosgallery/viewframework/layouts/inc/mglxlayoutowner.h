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
* Description:    Layout owner interface
*
*/




#ifndef M_GLXLAYOUTOWNER_H
#define M_GLXLAYOUTOWNER_H

class MGlxLayout;

/**
 * Layout owner interface to allow classes to access owned layouts
 */
class MGlxLayoutOwner
    {
public:
    /**
     * Return a specified layout, if found
     * @param aIndex The index for the layout - the meaning of the index is decided
     *               by each implementing class individually
     * @return The specified layout; NULL if not found
     */
    virtual MGlxLayout* GlxLayout(TInt aIndex) = 0;
    };

#endif // M_GLXLAYOUTOWNER_H

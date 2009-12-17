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
* Description:    Observer interface for scrollbar
*
*/




#ifndef M_GLXSCROLLBAROBSERVER_H
#define M_GLXSCROLLBAROBSERVER_H

/**
 * MGlxScrollbarObserver
 *
 * Scrollbar observer interface
 *
 * @lib glxuiutilities.lib
 *
 * @author Michael Yip
 */
class MGlxScrollbarObserver
    {
public:
    /**
     * Callback received to indicate whether scrollbar is scrollable
     */
    virtual void HandleScrollable(TBool aScrollable) = 0;
    };

#endif // M_GLXSCROLLBAROBSERVER_H

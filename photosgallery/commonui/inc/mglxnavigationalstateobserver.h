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
* Description:    Observer of navigational state 
*
*/




#ifndef __M_GLXNAVIGATIONALSTATEOBSERVER_H__
#define __M_GLXNAVIGATIONALSTATEOBSERVER_H__

/**
 * MGlxNavigationalStateObserver
 *
 * Class that will be notified when navigational state changes
 * See @ref CGlxNavigationalState
 *
 * @author Aki Vanhatalo
 */
class MGlxNavigationalStateObserver
    {
public:
    /**
     * Called when navigational state has changed
     */
    virtual void HandleNavigationalStateChangedL() = 0; 
    };
    
#endif // __M_GLXNAVIGATIONALSTATEOBSERVER_H__
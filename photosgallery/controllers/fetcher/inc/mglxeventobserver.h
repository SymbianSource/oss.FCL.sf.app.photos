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
* Description:    Tap event observer class for fetcher app
*
*/



#ifndef MGLXEVENTOBSERVER_H_
#define MGLXEVENTOBSERVER_H_

/**
 *  MGlxEventObserver
 *
 *  List event notification interface
 */
class MGlxEventObserver
    {
public: 
    /**
     * Called from Container to pass Select event (double tap )to Dialog.
     */
    virtual void HandleDoubleTapEventL(TInt aCommandId) = 0;  

    /**
     * Destructor. Dont allow deleting objects through this interface.
     */
    virtual ~MGlxEventObserver() {};
    };

#endif /*MGLXEVENTOBSERVER_H_*/

//EOF

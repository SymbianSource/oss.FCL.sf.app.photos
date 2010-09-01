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
* Description:    Provides a simple observable interface.
*
*/




#ifndef M_GLXSIMPLEOBSERVABLE_H
#define M_GLXSIMPLEOBSERVABLE_H

#include <e32base.h>
#include "glxsimpleobserverevents.h"

class MGlxSimpleObserver;

/**
 *  MGlxSimpleObservable
 *
 *  Provides a simple observable interface.
 *
 *  @lib glxhuiutils.lib
 */
class MGlxSimpleObservable
    {
public:
    /**
    * Registers an observer if not previously registered
    * @param aObserver The observer concerned
    */
    virtual void RegisterObserverL( MGlxSimpleObserver* aObserver ) = 0;

    /**
    * Deregisters an observer if previously registered
    * @param aObserver The observer concerned
    */
    virtual void DeregisterObserver( MGlxSimpleObserver* aObserver ) = 0;
    };
    
#endif // M_GLXSIMPLEOBSERVABLE_H

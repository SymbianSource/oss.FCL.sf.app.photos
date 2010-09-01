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
* Description:    Binding Change Observer
*
*/




#ifndef _MGLXBINDINGOBSERVER_H_
#define _MGLXBINDINGOBSERVER_H_

#include <e32base.h>          // Container Base Class

class CGlxBinding;            // The binding is created by the binding set factory

class MGlxBindingObserver
    {
public:
	/**
	 * Notifies that binding has changed
	 * @param aBinding The binding that is observed
	 */
	
    virtual void HandleBindingChanged( const CGlxBinding& aBinding ) = 0;
    };

#endif // _MGLXBINDINGOBSERVER_H_

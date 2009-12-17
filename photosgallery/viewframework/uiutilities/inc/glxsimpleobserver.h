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
* Description:    Simple observer interface.
*
*/




#ifndef GLXSIMPLEOBSERVER_H
#define GLXSIMPLEOBSERVER_H

#include "glxsimpleobserverevents.h"

/**
 *  MGlxSimpleObserver
 *
 *  Simple observer interface. Only classes implementing this can register with MGlxSimpleObservable classes.
 *
 *  @lib glxuiutils.lib
 */
class MGlxSimpleObserver
	{
public:	
	/**
	* Receive a broadcast event from an MGlxSimpleObservable class.
	* @param aEvent The event received
	*/
	virtual void HandleObservedEvent( TGlxSimpleObserverEvent aEvent ) = 0;
	};
	

#endif // GLXSIMPLEOBSERVER_H
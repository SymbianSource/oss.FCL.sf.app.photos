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
* Description:    Enter Key observer interface
*
*/




#ifndef M_GLXENTERKEYEVENTOBSERVER_H
#define M_GLXENTERKEYEVENTOBSERVER_H

/**
 *  MGlxEnterKeyEventObserver
 *
 *  List event notification interface
 */
class MGlxEnterKeyEventObserver
	{
public:	
	/**
	 * Called when enter key is Pressed.
	 * @param aCommand  Key event Id
	 */
    virtual void HandleEnterKeyEventL(TInt aCommand) = 0;  
	
	};

#endif // M_GLXENTERKEYEVENTOBSERVER_H

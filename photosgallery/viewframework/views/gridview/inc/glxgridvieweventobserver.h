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
*  Description : Grid view events Observers
*
*/


#ifndef GLXGRIDVIEWEVENTOBSERVER_H_
#define GLXGRIDVIEWEVENTOBSERVER_H_


class MGlxGridEventObserver
	{
public :	
	virtual void HandleGridEventsL(TInt aCmd) = 0;
	virtual void HandleLatchToolbar() = 0;
	};

#endif //GLXGRIDVIEWEVENTOBSERVER_H_

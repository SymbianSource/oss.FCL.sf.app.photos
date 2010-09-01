/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The pause handling control for the slideshow
 *
*/




#ifndef SHWTICKOBSERVER_H
#define SHWTICKOBSERVER_H


#include <e32base.h>


class MShwTickObserver
	{
	
	public:
	
	virtual void HandleTickL() = 0;
	virtual void HandleTickCancelled() = 0;
	
	};
	
#endif//SHWTICKOBSERVER_H




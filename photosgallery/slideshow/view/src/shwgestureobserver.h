/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Slideshow view implementation
*
*/






#ifndef SHWGESTUREOBSERVER_H
#define SHWGESTUREOBSERVER_H


class MShwGestureObserver
	{
	
public:
	
	enum TShwGestureEventType
		{
        ENoEvent = 0, 	// This should not be Propogated to observers. 
						// Reserved for those events we will not handle now. 
        ETapEvent,
		ESwipeLeft,
		ESwipeRight,
		EHoldEvent
		};
public:	


	/**
	 * Populate the media list
	 * this needs to be public as it is a callback.
	 * @return TInt, a non-zero value if it is intended to
     *      be called again, otherwise it should return zero.
	 */
	virtual void HandleShwGestureEventL(TShwGestureEventType aType) = 0;
	};
	
#endif //SHWGESTUREOBSERVER_H






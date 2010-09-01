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
* Description:    Screen Furniture cross-fader utility
*
*/




#ifndef GLXSIMPLEOBSERVEREVENTS_H
#define GLXSIMPLEOBSERVEREVENTS_H

class MGlxSimpleObservable;

/**
* Simple-observer event codes
* @bug minor:drhodes:26/09/07:Most of these events are not used and should be removed
*/
enum TGlxSimpleObserverEventCode
	{
	// Universal events	
	EGlxObservableDestroyed,
	
	// Screen Furniture related events
	EGlxSfUiHidden,
	EGlxSfUiShown,
	EGlxSfKeyCaptured,
	EGlxSfCourtesyTimerComplete,
	EGlxSfTitleCourtesyTimerComplete,
	EGlxSfNavipaneCourtesyTimerComplete,
	EGlxSfLeftSoftkeyCourtesyTimerComplete,
	EGlxSfRightSoftkeyCourtesyTimerComplete,
	EGlxSfMiddleSoftkeyCourtesyTimerComplete,
	EGlxSfLayoutUpdated,
	
	// Media List Existence events
	EGlxMvbMediaListCreated,
	EGlxMvbMediaListDestroyed
	
	};

/**
* TGlxSimpleObserverEvent
*
* The event sent by MGlxSimpleObservable objects to MGlxSimpleObserver objects.
* Contains the address of the broadcaster, and an event code.
*/
struct TGlxSimpleObserverEvent
	{
	/**
	* Constructor
	* @param aBroadcaster Address of the broadcaster
	* @param aEventCode Event code to send
	*/
	TGlxSimpleObserverEvent(MGlxSimpleObservable* aBroadcaster, TGlxSimpleObserverEventCode aEventCode)
		{
		iBroadcaster = aBroadcaster;
		iEventCode   = aEventCode;
		}

	/// The broadcaster		
	MGlxSimpleObservable*       iBroadcaster;
	
	/// The event code being broadcast
	TGlxSimpleObserverEventCode iEventCode;
	};
    
#endif // GLXSIMPLEOBSERVEREVENTS_H

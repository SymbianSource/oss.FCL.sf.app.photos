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
* Description:    Key event receiver interface
*
*/




#ifndef M_GLXKEYEVENTRECEIVER_H
#define M_GLXKEYEVENTRECEIVER_H

/**
 *  MGlxKeyEventReceiver
 *
 *  Key event receiver. 
 *
 *  @lib glxviewbase.lib
 */
class MGlxKeyEventReceiver
	{
public:
	/**
	 * Interface to recieve keypress information from an associated CGlxKeyMonitor. 
	 * @param aKeyEvent The key event. 
	 * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown
	 * @return Indicates whether or not the key event was used by this control. EKeyWasNotConsumed or EKeyWasConsumed
	 */
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType) = 0;
	};

#endif // M_GLXKEYEVENTRECEIVER_H
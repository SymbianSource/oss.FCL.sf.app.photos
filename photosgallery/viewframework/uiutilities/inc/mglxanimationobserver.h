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
* Description:    Observer interface for animations
*
*/




/**
 * @internal reviewed 12/07/2007 by Michael Yip
 */

#ifndef M_GLXANIMATIONOBSERVER_H
#define M_GLXANIMATIONOBSERVER_H

class MGlxAnimation;

/**
 * Animation observer interface
 */
class MGlxAnimationObserver
	{
public:
    /**
     * Callback received when animation is complete
     * @param aAnimation The completed animation - may be deleted within callback
     */
	virtual void AnimationComplete(MGlxAnimation* aAnimation) = 0;
	};

#endif //  M_GLXANIMATIONOBSERVER_H

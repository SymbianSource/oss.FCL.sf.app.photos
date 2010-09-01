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
* Description:    General animation interface
*
*/




/**
 * @internal reviewed 12/07/2007 by Michael Yip
 */

#ifndef M_GLXANIMATION_H
#define M_GLXANIMATION_H


class MGlxAnimationObserver;

/// Constant to slow down all animations for testing purposes
const TInt KGlxAnimationSlowDownFactor = 1;

/**
 * General-purpose animation class
 */
class MGlxAnimation
	{
public:
    /**
     * Start the animation
     * @param aTimer Timer to use for the animation. Timed value ranges from 
     *               0 (animation start) to 1 (animation complete)
     * @param aObserver Optional observer to get a callback when the animation 
     *                  completes
     */
	virtual void AnimateL(TInt aTimer, 
	    MGlxAnimationObserver* aObserver = NULL) = 0;
	
	/**
	 * Destructor
	 */
	virtual ~MGlxAnimation() {};
	};


#endif //  M_GLXANIMATION_H

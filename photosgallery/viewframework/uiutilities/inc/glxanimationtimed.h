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
* Description:    Base class for timed animations
*
*/




/**
 * @internal reviewed 12/07/2007 by Michael Yip
 */

#ifndef C_GLXANIMATIONTIMED_H
#define C_GLXANIMATIONTIMED_H

#include <alf/alfeventhandler.h>

// INTERNAL INCLUDES
#include "mglxanimation.h"

// FORWARD DECLARES
class CGlxUiUtility;

/**
 * Base class for timed animations
 */
class CGlxAnimationTimed : public CBase, public MGlxAnimation, public MAlfEventHandler
    {
    public:
        /**
         * Constructor
         */
        CGlxAnimationTimed();
        
        /**
         * Destructor
         */
        ~CGlxAnimationTimed();

    private: // From MGlxAnimation
    	void AnimateL( TInt aTime, MGlxAnimationObserver* aObserver );

    private: // From MAlfEventHandler
        TBool OfferEventL( const TAlfEvent &aEvent );
        
    protected:

        /**
         * Start the animation using the supplied timed value
         * @param aTimer Timer to use for the animation. @see MGlxAnimation::AnimateL
         */
        virtual void StartTimedAnimationL( TInt aTime ) = 0;
        
    private:
        /// Ref: Observer to animation, if any
        MGlxAnimationObserver* iObserver;
        
        /// Own: The Alf utility
        CGlxUiUtility* iUiUtility;
        
        /// Whether an animation is in progress
        TBool iTimedAnimationInProgress;
    };

#endif // C_GLXANIMATIONTIMED_H

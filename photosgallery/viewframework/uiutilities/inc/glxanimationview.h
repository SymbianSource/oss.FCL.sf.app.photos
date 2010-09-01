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
* Description:    View-transition animations
*
*/




/**
 * @internal reviewed 12/07/2007 by Michael Yip
 */

#ifndef GLXANIMATIONVIEW_H
#define GLXANIMATIONVIEW_H

#include "glxanimationtimed.h"
#include "glxanimationfactory.h"

/**
 * Implementation of View-switch animations
 */
class CGlxAnimationView : public CGlxAnimationTimed
    {
public:
    /**
     * Two-phase construction
     * @param aAnimType Type of animation (e.g. entry / exit)
     * @param aDirection Direction of animation (e.g. forwards / backwards)
     * @param aControlGroups List of the control groups to animate
     * @return The requested animation
     */
    static CGlxAnimationView* NewL(
        TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection, 
            const RPointerArray<CAlfControlGroup>& aControlGroups);

    /**
     * Destructor
     */
	~CGlxAnimationView();

    private: // From CGlxAnimationTimed

    	void StartTimedAnimationL( TInt aTime );

private:
    /**
     * Second-phase construction
     * @param aControlGroups List of the control groups to animate
     */
        void ConstructL(const RPointerArray<CAlfControlGroup>& aControlGroups);

    /**
     * Constructor
     * @param aAnimType Type of animation (e.g. entry / exit)
     * @param aDirection Direction of animation (e.g. forwards / backwards)
     */
    CGlxAnimationView(TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection);
        
private:
    /// The type of animation
    TGlxViewswitchAnimation iAnimType;
    
    /// Direction of navigation
    TGlxNavigationDirection iDirection;
    
    /// Control groups to animate (array members not owned)
        RPointerArray<CAlfControlGroup> iControlGroups;
    };
    
#endif //  GLXANIMATIONVIEW_H


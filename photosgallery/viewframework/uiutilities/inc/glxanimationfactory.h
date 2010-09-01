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
* Description:    Factory for gallery animations
*
*/




/**
 * @internal reviewed 11/07/2007 by Michael Yip
 */

#ifndef GLXANIMATIONFACTORY_H
#define GLXANIMATIONFACTORY_H

#include "glxuiutility.h"

class CAlfControlGroup;
class MGlxAnimation;
class CAlfVisual;
class CAlfTexture;

/**
 * View switching animation types
 */
enum TGlxViewswitchAnimation 
    {
    EGlxViewAnimationNone, 
    EGlxViewAnimationEntry, 
    EGlxViewAnimationExit
    }; 

/**
 * Factory class to create animation instances
 */
class GlxAnimationFactory
    {
public:
    /**
     * Create a view-switch animation
     * @param aAnimType Type of animation (e.g. entry / exit)
     * @param aDirection Direction of animation (e.g. forwards / backwards)
     * @param aControlGroup The control group to animate
     * @return The requested animation
     */
    IMPORT_C static MGlxAnimation* CreateViewAnimationL(
        TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection, 
        CAlfControlGroup* aControlGroup);

    /**
     * Create a view-switch animation
     * @param aAnimType Type of animation (e.g. entry / exit)
     * @param aDirection Direction of animation (e.g. forwards / backwards)
     * @param aControlGroups List of control groups to animate
     * @return The requested animation
     */
    IMPORT_C static MGlxAnimation* CreateViewAnimationL(
        TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection, 
        RPointerArray<CAlfControlGroup>& aControlGroups);
        
    /**
     * Create an image-loading animation
     * @param aVisual The visual that the image has been loaded into
     * @param aFlashTexture The texture to use for the "flash" effect
     * @return The requested animation
     */
    IMPORT_C static MGlxAnimation* CreateImageLoadingAnimationL(
        CAlfVisual& aVisual, 
        CAlfTexture& aFlashTexture);
    };
    
#endif //  GLXANIMATIONFACTORY_H

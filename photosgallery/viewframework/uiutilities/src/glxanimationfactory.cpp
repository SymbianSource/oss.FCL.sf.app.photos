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

#include "glxanimationfactory.h"

#include "glxanimationview.h"
#include "glxanimationimageloading.h"


// -----------------------------------------------------------------------------
// CreateViewAnimationL
// -----------------------------------------------------------------------------
//
EXPORT_C MGlxAnimation* GlxAnimationFactory::CreateViewAnimationL(
        TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection, 
        CAlfControlGroup* aControlGroup)
    {
    CGlxAnimationView* anim = NULL;
    
    RPointerArray<CAlfControlGroup> controlGroupArray;
	CleanupClosePushL(controlGroupArray);
    controlGroupArray.AppendL(aControlGroup);
    anim = CGlxAnimationView::NewL(aAnimType, aDirection, controlGroupArray);
    CleanupStack::PopAndDestroy(&controlGroupArray);
    
    return anim;
    }

// -----------------------------------------------------------------------------
// CreateViewAnimationL
// -----------------------------------------------------------------------------
//
EXPORT_C MGlxAnimation* GlxAnimationFactory::CreateViewAnimationL(
        TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection, 
        RPointerArray<CAlfControlGroup>& aControlGroups)
    {
    return CGlxAnimationView::NewL(aAnimType, aDirection, aControlGroups);
    }

// -----------------------------------------------------------------------------
// CreateImageLoadingAnimationL
// -----------------------------------------------------------------------------
//
EXPORT_C MGlxAnimation* GlxAnimationFactory::CreateImageLoadingAnimationL(
        CAlfVisual& aVisual, 
        CAlfTexture& aFlashTexture)
    {
    return CGlxAnimationImageLoading::NewL(aVisual, aFlashTexture);
    }        

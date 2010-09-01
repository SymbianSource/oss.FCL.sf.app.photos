/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The effect interface
 *
*/




#ifndef __SHWCONSTANTS_H__
#define __SHWCONSTANTS_H__

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Namespace for slideshow constants.
 */
namespace NShwSlideshow
    {
    enum TPlayDirection
        {
        // Chronological Order (Older to newer)
        EPlayForwards = 0,
        // Reverse Chronological Order (Newer to older)
        EPlayBackwards = 1 // default
        };

    // Named constants for navigation directions 
    // to avoid "magic number" warnings
    const TInt KNavigateForwards = 1;
    const TInt KNavigateBackwards = -1;

    // Engine DLL's uid
    const TUid KEngineDllUid = {0x200071D3};

    // First visual fade-in time, also applies when user navigates
    const TInt KFirstEffectFadeInDuration = 0;
    // default transition duration, milliseconds
    const TInt KDefaultTransitionDuration = 2000;	

    // @todo get correct UID for effect types
    /**
    The effect Id is a combination of the ecom plugin UID and an array index
    within the plugin.
    */
    const TUid KDefaultEffectPluginUid = { 0x200071D6 };
    
    // Crossfade effects name - used if no localised strings are available
    _LIT( KEffectNameCrossFade, "Cross Fade" );

    // maximum opacity for TShwCrossFadeLayout
    const TReal32 KMaxOpacity = 1.0;
    // minimum opacity for TShwCrossFadeLayout
    const TReal32 KMinOpacity = 0.0;

    // effect Index - the index within the ecom plugin
    const TInt KEffectUidXFadeNormal = 1;
    const TInt KEffectUidZoomAndPan  = 0;

    // Zoom and Pan effect's name - used if no localised strings are available
    _LIT( KEffectNameZoomAndPan, "Zoom and Pan" );
    // maximum zoom ever is 200%, note that this should 
    // not be larger than MaxThumbnailSize
    const TReal32 KMaxZoomAndPanFactor = 2.0;
    // optimal zoom is 100%
    const TReal32 KOptimalZoomAndPanFactor = 1.0;
    // this defines the minimum zoom factor,
    // 1.2 means we zoom images that are originally larger than 120% of the screen
    const TReal32 KMinZoomAndPanFactor = 1.2;
    // this constant specifies the length of the curve for panning
    const TInt KZoomAndPanCurveLength = 100;

    // set the maximum loaded thumbnail size to be 2 times the screen
    // note that this should not be smaller than KMaxZoomAndPanFactor
    // otherwise we zoom beyond 1:1 pixelsize in big pictures
    const TInt KMaxThumbnailSize = 2;

    // maximum effect name length
    const TUint KMaxEffectNameLen = 64;

    /// Thumbnail loading
    // priority 2 is the highest we use, we need size 
    // attribute to be loaded first
    const TInt KSizeContextPriority = 2;
    // load image sizes 4 items in front and back
    const TInt KSizeContextOffset = 4;

    // lowest priority for the high quality context
    // because we have time to wait as we have 
    // default icon to show
    const TInt KHighQualityContextPriority = 0;
    }

#endif // __SHWCONSTANTS_H__

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
* Description:    Image loading animation
*
*/




/**
 * @internal reviewed 12/07/2007 by Michael Yip
 */

#ifndef GLXANIMATIONIMAGELOADING_H
#define GLXANIMATIONIMAGELOADING_H

#include "glxanimationtimed.h"

class CAlfVisual;
class CAlfTexture;
class CAlfImageBrush;

/**
 * Animation for a newly-loaded image
 */
class CGlxAnimationImageLoading : public CGlxAnimationTimed
    {
public:
    /**
     * Two-phase constructor
     * @param aVisual The visual that the image has been loaded into
     * @param aFlashTexture The texture to use for the "flash" effect
     * @return The requested animation
     */
    static CGlxAnimationImageLoading* NewL(
            CAlfVisual& aVisual, 
            CAlfTexture& aFlashTexture);

    /**
     * Destructor
     */
	~CGlxAnimationImageLoading();

        
    private: // From CGlxAnimationTimed

    	void StartTimedAnimationL( TInt aTime );

private:
    /**
     * Second-phase construction
     */
    void ConstructL();

    /**
     * Constructor
     * @param aVisual The visual that the image has been loaded into
     * @param aFlashTexture The texture to use for the "flash" effect
     */
        CGlxAnimationImageLoading(CAlfVisual& aVisual, 
            CAlfTexture& aFlashTexture);
        
private:
        /// Ref: The visual that the image has been loaded into
        CAlfVisual& iVisual;

        /// Ref: The texture to use for the "flash" effect
        CAlfTexture& iFlashTexture;
    
        /// Own: The image brush used for the flash effect
        CAlfImageBrush* iImageBrush; 
    };


#endif // GLXANIMATIONIMAGELOADING_H

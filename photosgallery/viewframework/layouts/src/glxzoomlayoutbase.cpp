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
* Description:    Implementation of TGlxZoomLayoutBase
*
*/




// INCLUDE FILES

#include "glxzoomlayoutbase.h"

#include <alf/alftimedvalue.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxZoomLayoutBase::TGlxZoomLayoutBase()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxZoomLayoutBase::~TGlxZoomLayoutBase()
    {
    }

// -----------------------------------------------------------------------------
// SetImageDimensions
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxZoomLayoutBase::SetImageDimensions(const TSize& aDimensions)
    {
    iImageDimensions = aDimensions;
    iChanged = ETrue;
    }

// -----------------------------------------------------------------------------
// ImageDimensions
// -----------------------------------------------------------------------------
//
EXPORT_C const TSize& TGlxZoomLayoutBase::ImageDimensions() const
    {
    return iImageDimensions;
    }

// -----------------------------------------------------------------------------
// DoSetLayoutValues
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxZoomLayoutBase::DoSetLayoutValues(TGlxLayoutInfo& aInfo)
    {
    TAlfRealRect rect;
    GetCurrentCoords(rect);

    // These checks on iImageDimensions width/height must occur after GetCurrentCoords,
  /*  // since they may be changed as a result of the call to GetCurrentCoords
    if ( iImageDimensions.iWidth > 0 && iImageDimensions.iHeight > 0 )
        {
        // The previous layout in the chain has set the position and size for
        // the visual to fill the screen, so we base the values on that.

        // Now adjust according to zoom coordinates
        TAlfRealPoint centre = rect.Center();        
        TReal32 width = aInfo.iScreenSize.iX * rect.Width();
        TReal32 height = aInfo.iScreenSize.iY * rect.Height();

        // Adjust size according to proportion of screen that image fills
        // Account for TGlxRotationLayout swapping width and height
        TAlfRealSize imageSize(iImageDimensions);
        TAlfRealSize screenSize(aInfo.ScreenInfo()->iWidth,
                                aInfo.ScreenInfo()->iHeight);
        if ( screenSize.iHeight * imageSize.iWidth
                < imageSize.iHeight * screenSize.iWidth )
            {
            // Screen has wider aspect than image
            TReal32 sizeRatioX = (screenSize.iHeight * imageSize.iWidth)
                                / (screenSize.iWidth * imageSize.iHeight);
            if ( aInfo.iRotation )
                {
                // Rotated
                height *= sizeRatioX;
                }
            else
                {
                // Normal
                width *= sizeRatioX;
                }
            }
        else
            {
            // Screen has taller aspect than image
            TReal32 sizeRatioY = (screenSize.iWidth * imageSize.iHeight)
                                / (screenSize.iHeight * imageSize.iWidth);
            if ( aInfo.iRotation )
                {
                // Rotated
                width *= sizeRatioY;
                }
            else
                {
                // Normal
                height *= sizeRatioY;
                }
            }

        aInfo.iPosition.iX += centre.iX * aInfo.iSize.iX;
        aInfo.iPosition.iY += centre.iY * aInfo.iSize.iY;
        aInfo.iSize.iX = width;
        aInfo.iSize.iY = height;
        }
*/
    }


//  End of File

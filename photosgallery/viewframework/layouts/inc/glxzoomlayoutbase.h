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
* Description:    Definition of TGlxZoomLayoutBase
*
*/




#ifndef GLXZOOMLAYOUTBASE_H
#define GLXZOOMLAYOUTBASE_H

// INCLUDES

#include <glxlayout.h>

// FORWARD DECLARATIONS

struct TAlfRealRect;

// CLASS DECLARATION

/**
* Abtract base class for zooming layouts.  Sets layout values based on
* coordinates provided by the derived class.
*
* @internal reviewed 06/06/2007 by Kimmo Hoikka
*/
class TGlxZoomLayoutBase : public TGlxLayout
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C TGlxZoomLayoutBase();

        /**
        * Destructor.
        */
        IMPORT_C ~TGlxZoomLayoutBase();

    public: // New functions

        /**
        * Set the image dimensions.
        * @param aDimensions Dimensions of the image being zoomed (in pixels)
        */
        IMPORT_C void SetImageDimensions(const TSize& aDimensions);
        /**
        * Get the image dimensions.
        * @return const TSize& Dimensions of the image being zoomed (in pixels)
        */
        IMPORT_C const TSize& ImageDimensions() const;

        /**
        * Get current zoom and pan values.  The returned rectangle is relative
        * to the screen, ie. if the width is 2, the visual should be twice the
        * width of the screen, and if the centre of the rectangle is (0, 0) the
        * visual should be centred on the screen. If the rectangle is
        * (-0.5, -0.5) - (0.5, 0.5) then the visual should exactly fill the
        * screen.
        * @param aRect Reference to rect in which to return coords.
        */
        virtual void GetCurrentCoords(TAlfRealRect& aRect) = 0;
        /**
        * Get "predicted" zoom and pan values.  These are the values that would
        * eventually be reached if the user released all zoom/pan keys now.
        * @param aRect Reference to rect in which to return coords.
        */
        virtual void GetPredictedCoords(TAlfRealRect& aRect) = 0;

    protected:  // From TGlxLayout

        /** See @ref TGlxLayout::DoSetLayoutValues */
        IMPORT_C void DoSetLayoutValues(TGlxLayoutInfo& aInfo);
        /** See @ref TGlxLayout::DoChanged */
        IMPORT_C TBool DoChanged() const;
        /** See @ref TGlxLayout::DoClearChanged */
        IMPORT_C void DoClearChanged();

    protected:  // Data
        /** Dimensions of the image being zoomed (in pixels) */
        TSize iImageDimensions;

    private:    // Data
        /** Whether the layout has changed since last refresh */
        TBool iChanged;
    };

#endif  // GLXZOOMLAYOUTBASE_H

// End of File

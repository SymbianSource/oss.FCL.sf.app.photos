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
* Description:    Layout that performs zoom and fade
 *
*/




#ifndef __SHWZOOMANDPANLAYOUT_H__
#define __SHWZOOMANDPANLAYOUT_H__

#include <glxlayout.h>
//#include <alf/alfrealsize.h>
#include <alf/alftimedvalue.h>
#include <alf/alfcurvepath.h>

#include "shwtimedvalue.h"

// FORWARD DECLARATIONS
class CHuiCurvePath;

/**
 * TShwZoomAndPanLayout
 * Handles zooming and panning a visual
 * 
 * @author Kimmo Hoikka
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 * @internal reviewed 06/06/2007 by Dan Rhodes
 */
NONSHARABLE_CLASS( TShwZoomAndPanLayout ) : public TGlxLayout
	{
	public:

		/**
		 * Enumeration for zoom direction.
		 */
		enum TZoomDirection
			{
			EZoomIn = 1,	// Zoom in, from minimum to maximum size
			EZoomOut = 2	// Zoom out, from maximum to minimum size
			};

		/**
		 * Constructor, initializes the layout.
		 */
		TShwZoomAndPanLayout();

		/**
		 * Destructor
		 */
		~TShwZoomAndPanLayout();

		/**
		 * Sets the screen and image size
		 * @param aScreenSize the size of the screen.
		 * @param aImageSize the size of the image.
		 * @param aMaximumSize the maximum ever size for the image
		 * @return the zoom factor, this enables the client to deside 
		 *			if it makes sense to do any zoomimg
		 */
		TReal32 SetSizes( 
		    TSize aScreenSize, TSize aImageSize, TSize aMaximumSize );

		/**
		 * Sets the curve for pan. The curve must be defined so that in
		 * 100% zoom it stays inside the image (no black borders shown)
		 * basically this means a box inside image coordinates:
		 * xi = image.width/2;
		 * yi = image.height/2;
		 *
		 *  (-xi,yi)-------------(xi,yi)
		 *  |  (-xb,yb)-------(xb,yb)  |
		 *  |  |                    |  |
		 *  |  (-xb,-yb)-----(xb,-yb)  |
		 *  (-xi,-yi)-----------(xi,-yi)
		 *
		 * where:
		 *  xb = xi - screen.width/2;
		 *  yb = yi - screen.height/2;
		 *
		 * During zoom the panning curve is scaled according to current
		 * zoom factor so that no black borders show in zoom factors < 100%
		 *
		 * @param aCurve the curve to use for the panning.
		 */
		void SetPanningCurve( CAlfCurvePath* aCurve );

        /**
         * This method returns the maximum zoom size for the image
         * Should not be called before before SetSizes is called
         * @return the maximum size as TSize
         */
        TSize MaximumSize();

		/**
		 * This method resets the size to minimum. Usefull when you dont 
		 * want to do zoom & pan (for example the image is too small)
		 */
		void ResetSizeToMinimum();

		/**
		 * This method starts the zooming. If a pannig curve is used 
		 * it should be set before calling this method. Note, for zoom in
		 * the layout uses the first half of the curve and for zoom out
		 * the latter half; this enables you to use an ellipsis to have
		 * looping zoomin
		 *
		 * @param aZoomDirection the desired zoom direction
		 * @param aDuration the length of the zoom loop
		 */
		void StartZoom( TZoomDirection aZoomDirection, TInt aDuration );

		/** 
		 * This method inverts the zoom the opposite way
		 */
		TInt InvertZoom();

		/**
		 * This method pauses zoom and pan
		 */
		void Pause();

		/**
		 * This method resumes zoom and pan after pause.
		 */
		void Resume();

	protected:	// From TGlxLayout

		/// @ref TGlxLayout::DoSetLayoutValues
		void DoSetLayoutValues( TGlxLayoutInfo& aInfo );
		/// @ref TGlxLayout::DoChanged
		TBool DoChanged() const;
		/// @ref TGlxLayout::DoClearChanged
		void DoClearChanged();

	private:	// Data and private parts

		/// Helper to calculate minimum and maximum size
		void CalculateMinAndMaxSize( TSize aMaximumSize );
		/// Own: Screen size
		TAlfRealSize iScreenSize;
		/// Own: Image size
		TAlfRealSize iImageSize;
		/// Own: the maximum size
		TAlfRealSize iMaximumSize;
		/// Own: the minimum size
		TAlfRealSize iMinimumSize;
		/// Own: the zoom direction
		TZoomDirection iZoomDirection;
		/// Own: the zoom duration
		TInt iZoomDuration;
		/// Own: the zoomed size
		TShwTimedSize iZoomedSize;
		/// Own: Pan curve position
		TShwTimedValue iCurvePosition;
		/// Ref: Pan curve
		CAlfCurvePath* iPanningCurve;

	};

#endif // __SHWZOOMANDPANLAYOUT_H__

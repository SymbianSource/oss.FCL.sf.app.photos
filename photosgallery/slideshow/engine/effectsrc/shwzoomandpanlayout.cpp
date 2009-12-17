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
* Description:    Layout that performs crossfade
 *
*/




//  Include Files
#include "shwzoomandpanlayout.h"
#include <e32math.h>

#include <uiacceltk/HuiCurvePath.h>

#include <glxlog.h>

#include "shwconstants.h"
#include "shwgeometryutilities.h"

using namespace NShwSlideshow;

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
TShwZoomAndPanLayout::TShwZoomAndPanLayout()
	: iScreenSize( 0, 0 ),
	iImageSize(	0, 0 ),
	iPanningCurve( NULL ) // T-class so need to set value
	{
	__DEBUG_ONLY( _iName = _L("TShwZoomAndPanLayout") );
	// default values
	// curve style is linear
	iCurvePosition.SetStyle( EAlfTimedValueStyleLinear );
	// set zoom style to sine so it accelerates and decelerates
	iZoomedSize.SetStyle( EAlfTimedValueStyleSineWave );
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
TShwZoomAndPanLayout::~TShwZoomAndPanLayout()
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// SetSizes
// -----------------------------------------------------------------------------
TReal32 TShwZoomAndPanLayout::SetSizes(
    TSize aScreenSize, TSize aImageSize, TSize aMaximumSize )
	{
	GLX_LOG_INFO( "TShwZoomAndPanLayout::SetSizes" );
	// set the size from TSize (integer) to THuiRealSize (float)
	iScreenSize.iWidth = aScreenSize.iWidth;
	iScreenSize.iHeight = aScreenSize.iHeight;
	iImageSize.iWidth = aImageSize.iWidth;
	iImageSize.iHeight = aImageSize.iHeight;
	// recalculate min and max size for zoom
	CalculateMinAndMaxSize( aMaximumSize );
	// calculate zoom factor for iHeight, iWidth is same as aspect ratio is kept
	// maximum per minimum
	TReal32 zoomFactor = iMaximumSize.iHeight / iMinimumSize.iHeight;
	// return zoom factor
	return zoomFactor;
	}

// -----------------------------------------------------------------------------
// SetPanningCurve
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::SetPanningCurve( CAlfCurvePath* aCurve )
	{
	iPanningCurve = aCurve;
	}

// -----------------------------------------------------------------------------
// MaximumSize
// -----------------------------------------------------------------------------
TSize TShwZoomAndPanLayout::MaximumSize()
    {
    return iMaximumSize;
    }

// -----------------------------------------------------------------------------
// ResetSizeToMinimum
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::ResetSizeToMinimum()
	{
	GLX_LOG_INFO( "TShwZoomAndPanLayout::ResetSizeToMinimum" );
	// set initial size to minimum size
//	iZoomedSize.Set( iMinimumSize );
	}

// -----------------------------------------------------------------------------
// StartZoom
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::StartZoom(
	TZoomDirection aZoomDirection, TInt aDuration )
	{
	GLX_LOG_INFO1(
        "TShwZoomAndPanLayout::StartZoom( %d )", aDuration );

	// remember the zoom direction
	iZoomDirection = aZoomDirection;
	// remember also the zoom duration
	iZoomDuration = aDuration;

	// calculate curve position
	TInt curveLength = 0;
	// set curvelength, if we have curve
	if( iPanningCurve )
		{
		curveLength = iPanningCurve->Length();
		}

	// default case is to run the first half of the curve
	TInt curveStart = 0;
	TInt curveEnd = curveLength  / 2;

	// change the zoom according to direction
	if( EZoomOut == aZoomDirection )
		{
		GLX_LOG_INFO2( 
			"TShwZoomAndPanLayout:: Zooming out start(%.1f,%.1f)", 
			iMaximumSize.iWidth, iMaximumSize.iHeight );
		GLX_LOG_INFO2( 
			"TShwZoomAndPanLayout:: Zooming out target(%.1f,%.1f)", 
			iMinimumSize.iWidth, iMinimumSize.iHeight );
		// zooming out so we start from maximum size
		//iZoomedSize.Set( iMaximumSize );
		// target is the minimum size
	//	iZoomedSize.Set( iMinimumSize, aDuration );
		// run the latter half of the curve
		curveStart = curveLength / 2;
		curveEnd = curveLength;
		}
	else // default case is EZoomIn
		{
		GLX_LOG_INFO2( 
			"TShwZoomAndPanLayout:: Zooming in start(%.1f,%.1f)", 
			iMinimumSize.iWidth, iMinimumSize.iHeight );
		GLX_LOG_INFO2( 
			"TShwZoomAndPanLayout:: Zooming in target(%.1f,%.1f)", 
			iMaximumSize.iWidth, iMaximumSize.iHeight );
		// zooming in so we start from minimum size
//		iZoomedSize.Set( iMinimumSize );
		// target is maximum size
//		iZoomedSize.Set( iMaximumSize, aDuration );
		}

	// set position, if we have curve its either from start to half 
	// or half to end and if we dont then its always 0
	iCurvePosition.Set( curveStart );
	iCurvePosition.Set( curveEnd, aDuration );
	}

// -----------------------------------------------------------------------------
// Pause
// -----------------------------------------------------------------------------
TInt TShwZoomAndPanLayout::InvertZoom()
	{
	GLX_LOG_INFO( "TShwZoomAndPanLayout::InvertZoom" );
	if( EZoomOut == iZoomDirection )
		{
		StartZoom( EZoomIn, iZoomDuration );
		}
	else
		{
		StartZoom( EZoomOut, iZoomDuration );
		}
	
	return 0;
	}

// -----------------------------------------------------------------------------
// Pause
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::Pause()
	{
	GLX_LOG_INFO( "TShwZoomAndPanLayout::Pause" );
	// pause size
	iZoomedSize.Pause();
	// pause curve position
	iCurvePosition.Pause();
	}

// -----------------------------------------------------------------------------
// Resume
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::Resume()
	{
	GLX_LOG_INFO( "TShwZoomAndPanLayout::Resume" );
	// resume size
	iZoomedSize.Resume();
	// resume pan curve
	iCurvePosition.Resume();
	}

// -----------------------------------------------------------------------------
// DoSetLayoutValues
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::DoSetLayoutValues( TGlxLayoutInfo& aInfo )
	{
	// calculate new x size, current value from zoomedsize
	TReal32 x_size = iZoomedSize.iWidth.Now();
	// calculate new y size, current value from zoomedsize
	TReal32 y_size = iZoomedSize.iHeight.Now();

	// ensure x is inside  maximum size and minimum size
    x_size = Min( x_size, iMaximumSize.iWidth );
    x_size = Max( x_size, iMinimumSize.iWidth );
	// set x size, it is now inside the minimum and maximum
	//aInfo.iSize.iX = x_size;

    // ensure y is inside  maximum size and minimum size
    y_size = Min( y_size, iMaximumSize.iHeight );
    y_size = Max( y_size, iMinimumSize.iHeight );
	// set y size, it is now inside the minimum and maximum
//	aInfo.iSize.iY = y_size;

	// set position only if we have a panning curve, this enables the use of this
    // layout as only a zooming layout and then some other layout can define 
    // the position (as we dont overwrite 0,0 there...)
	if( iPanningCurve )
		{
		// need to ensure that position does not make image 
		// go outside the bounding box. We trust that the curve has 
		// been calculated so that in 100% zoom it stays inside the image
		// so the only thing needed is to ensure zoom keeps the image inside
		// the box as well.
		// this is done by scaling the position with the current 
		// zoomfactor per maximum zoomfactor. The scalefactor is between
		// 0 and 1; when size is minimum the scale is 0 and when size
		// is maximum the scale is 1.
		// If we are in minimum size, we cant do any panning and
		// if we are in maximum size we can do the maximum pan.
		// current zoomfactor is current height per the minimum height
		// formula:
		//         current.y - minimum.y
		// scale = ---------------------
		//         maximum.y - minimum.y
		// and need to ensure maximum.y > minimum.y, otherwise scale becomes
		// infinite
		if( iMaximumSize.iHeight > iMinimumSize.iHeight )
			{
/*			TReal32 scale = ( aInfo.iSize.iY - iMinimumSize.iHeight ) /
							( iMaximumSize.iHeight - iMinimumSize.iHeight );
			// multiply the curve with scale
			// set x position
			aInfo.iPosition.iX = 
				scale * iPanningCurve->MapValue( iCurvePosition.Now(), 0 );
			// set y position
			aInfo.iPosition.iY =
				scale * iPanningCurve->MapValue( iCurvePosition.Now(), 1 );*/
			}
		else
	        {
	        // Panning curve defined but we either zoomed to screen size or
	        // the image was smaller than or equal to screen size so
	        // just set position to origo to see image centered on the screen
		//	aInfo.iPosition.iX = 0;
		//	aInfo.iPosition.iY = 0;
	        }
		}
	}

// -----------------------------------------------------------------------------
// DoChanged
// -----------------------------------------------------------------------------
TBool TShwZoomAndPanLayout::DoChanged() const
	{
	TBool ret = EFalse;
	if( iPanningCurve )
		{
//		ret = iPanningCurve->MappingFunctionChanged();
		}
	return iZoomedSize.Changed() || ret;
	}

// -----------------------------------------------------------------------------
// DoClearChanged
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::DoClearChanged() 
	{
	iZoomedSize.ClearChanged();
	if( iPanningCurve )
		{
//		iPanningCurve->MappingFunctionClearChanged();
		}
	}

// -----------------------------------------------------------------------------
// CalculateMinAndMaxSize
// -----------------------------------------------------------------------------
void TShwZoomAndPanLayout::CalculateMinAndMaxSize( TSize aMaximumSize )
	{
    // use the namespace for coord utilities
    using namespace NShwGeometryUtilities;

    // set minimum size to be the image size
    iMinimumSize.iWidth = iImageSize.iWidth;
    iMinimumSize.iHeight = iImageSize.iHeight;

    // scale minimum size inside the screen
    FitToCoverBox( 
        iMinimumSize.iWidth, 
        iMinimumSize.iHeight, 
        iScreenSize.iWidth, 
        iScreenSize.iHeight );

    // check if the image is large enough for zooming
    TReal32 zoomFactor = 
        ( iImageSize.iWidth * iImageSize.iHeight ) / 
            ( iScreenSize.iWidth * iScreenSize.iHeight );
    // if combined zoom factor is big enough then zoom
    // so if image is for example half the width but double the height of screen
    // we dont zoom
    if( zoomFactor > KMinZoomAndPanFactor )
        {
        // the image should be zoomed, figure out if its
        // from screen size -> image size * KOptimalZoomAndPanFactor or
        // from screen size -> image size * KMaxZoomAndPanFactor
    	// set maximum size to image dimensions multiplied
    	// by KShwOptimalZoomAndPanFactor
    	// this defines how much we want the max zoom
    	// to be in optimal case
    	iMaximumSize.iWidth = iImageSize.iWidth * KOptimalZoomAndPanFactor;
    	iMaximumSize.iHeight = iImageSize.iHeight * KOptimalZoomAndPanFactor;
        // check if the image is large enough for zooming, 
        // only need to check either coordinate as aspect ratio is kept
        // Images between screen size and screen size * KMaxZoomAndPanFactor
    	// get zoomed between screen size and screen size * KMaxZoomAndPanFactor
        if( ( iMaximumSize.iWidth < iMinimumSize.iWidth * KMaxZoomAndPanFactor )||
            ( iMaximumSize.iHeight < iMinimumSize.iHeight * KMaxZoomAndPanFactor ) )
            {
            // image is too small from either dimension
            // so zoom from minimum size to Screen x KMaxZoomAndPanFactor
            FitToCoverBox( 
                iMaximumSize.iWidth, 
                iMaximumSize.iHeight,
                iScreenSize.iWidth * KMaxZoomAndPanFactor,
                iScreenSize.iHeight * KMaxZoomAndPanFactor );
            }
        // calculate maximum Width and Height for this image
        // the maximum thumbnailsize defines the maximum amount of pixels 
        // we should ever load. We need to adjust this box to have the
        // same aspect ratio of the image; so in short:
        //      same amount of pixels than maximum but aspect ratio of image
        TReal32 maxWidth = aMaximumSize.iWidth;
        TReal32 maxHeight = aMaximumSize.iHeight;
        // calculate the image size difference and take square root to get
        // multiplier for both width and height, ignore error
        //                    maximumWidth * maximumHeight
        // multiplier = sqrt( ---------------------------- )
        //                      imagewidth * imageheight 
        TReal64 multiplier;
        (void)Math::Sqrt( multiplier, 
            ( maxHeight * maxWidth ) / 
            ( iImageSize.iWidth * iImageSize.iHeight ) );
        // maxwidth = imagewidth * multiplier
        TReal32 maximumWidth = multiplier * iImageSize.iWidth;
        // maxheight = imageheight * multiplier
        TReal32 maximumHeight = multiplier * iImageSize.iHeight;
        // then fit the maximum size inside that box
        FitInsideBox( 
            iMaximumSize.iWidth, iMaximumSize.iHeight,
            maximumWidth, maximumHeight );
        }
    else
        {
        // not enough to zoom so just streth to screen size. 
        // set maximum to be minimum
        iMaximumSize.iWidth = iMinimumSize.iWidth;
        iMaximumSize.iHeight = iMinimumSize.iHeight;
        }

	}


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
* Description:    The zoom and pan effect
 *
*/




//  Include Files
#include "shwzoomandpaneffect.h"

#include <glxsetvaluelayout.h>
#include <alf/alfvisual.h>

#include <alf/alfcurvepath.h>
#include <alf/alftimedvalue.h>

#include "shwresourceutility.h"
#include "shwzoomandpanlayout.h"
#include "shwcurvefactory.h"
#include "shwconstants.h"
#include "shwslideshowenginepanic.h"
#include "shwcrossfadelayout.h"
#include "shwtimer.h"
#include "shwcallback.h"
#include "shwgeometryutilities.h"

#include <glxlog.h>

using namespace NShwSlideshow;

/**
 * CShwZoomAndPanEffectImpl
 * Zoom and pan dependencies
 * This class makes our clients indepandant of the effect implementation
 * This gives greater flexibitily in testing as the test suites can just
 * re-implement all the methods in the real class interface without the 
 * need to stub all our real dependencies. 
 * If for example TShwCrossFadeLayout was a direct member of
 * CShwZoomAndPanEffect class, all the clients would need to have access 
 * to TShwCrossFadeLayout constructor and destructor and the test suite
 * would need to either create a stub implementation for it or include
 * the real class in the test (and all its dependencies)
 *
 * There is however no point of duplicating the whole CShwZoomAndPanEffect
 * interface as it would mean double maintenance
 * so we use the iImpl pointer when referencing the class members
 */
NONSHARABLE_CLASS( CShwZoomAndPanEffect::CShwZoomAndPanEffectImpl )
	: public CBase
	{
	public:
		
		/**
		 * Constructor
		 */
		inline CShwZoomAndPanEffectImpl();

		/**
		 * Destructor
		 */
		inline ~CShwZoomAndPanEffectImpl();

		/**
		 * 2nd phase constructor
		 */
		inline void ConstructL();

	public:	// Data

		/// Own: the size of the screen
		TSize iScreenSize;
		/// Own: the maximum thumbnail size
		TSize iMaxThumbnailSize;
		/// Own: the opacity layout
		TShwCrossFadeLayout iOpacityLayout;
		/// Own: the zoom and pan layout
		TShwZoomAndPanLayout iZoomAndPanLayout;
		/// Own: the pan curve
		CAlfCurvePath* iCurve;
		/// Own: Zoom and pan loop timer
		CShwTimer* iLoopTimer;
		/// Own: flag that defines if the image is large enough for zoom&pan
		TBool iShouldZoom;
        /// Own: Count to change the zoom direction
        TInt iCount;
		/// Own: the effect's info.
		TShwEffectInfo iEffectInfo;

	};

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwZoomAndPanEffect::CShwZoomAndPanEffectImpl::CShwZoomAndPanEffectImpl()
	{
	// CBase clears the values
	// set layout chain
	iOpacityLayout.SetNext( &iZoomAndPanLayout );
	}

// -----------------------------------------------------------------------------
// Destructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwZoomAndPanEffect::CShwZoomAndPanEffectImpl::~CShwZoomAndPanEffectImpl()
	{
	delete iCurve;
	delete iLoopTimer;
	}

// -----------------------------------------------------------------------------
// 2nd phase constructor for the implementation
// -----------------------------------------------------------------------------
inline void CShwZoomAndPanEffect::CShwZoomAndPanEffectImpl::ConstructL()
    {
    // timer for zoom and pan looping
    iLoopTimer = CShwTimer::NewL( CActive::EPriorityStandard );

    // load string for effect name, no need to cleanup stack 
    // as no leaves between here and delete
    HBufC* effectName = 
        ShwResourceUtility::LocalisedNameL( R_SHW_EFFECT_ZOOM_AND_PAN );
    if( !effectName )
        {
        // have to use a non-localised version
        iEffectInfo.iName = KEffectNameZoomAndPan;
        }
    else
        {
        // set the localised version
        iEffectInfo.iName = *effectName;
        }

    // info - uid
    iEffectInfo.iId.iPluginUid = KDefaultEffectPluginUid;
    iEffectInfo.iId.iIndex     = KEffectUidZoomAndPan;
    // delete the local string
    delete effectName;
    }

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwZoomAndPanEffect::CShwZoomAndPanEffect()
	{
	}

// -----------------------------------------------------------------------------
// NewLC. Static construction
// -----------------------------------------------------------------------------
CShwZoomAndPanEffect* CShwZoomAndPanEffect::NewLC()
	{
	CShwZoomAndPanEffect* self = new (ELeave) CShwZoomAndPanEffect;
	CleanupStack::PushL( self );
    
	// create the implementation class
    self->iImpl = new( ELeave ) CShwZoomAndPanEffectImpl;
    // do the second phase, if it leaves impl will be deleted
	self->iImpl->ConstructL();

	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CShwZoomAndPanEffect::~CShwZoomAndPanEffect()
	{
	delete iImpl;
	}

// -----------------------------------------------------------------------------
// CloneLC
// -----------------------------------------------------------------------------
MShwEffect* CShwZoomAndPanEffect::CloneLC()
	{
	GLX_LOG_INFO( "CShwZoomAndPanEffect::CloneLC" );
	// create a copy
	CShwZoomAndPanEffect* copy = CShwZoomAndPanEffect::NewLC();
	// transfer the member variables, only screen 
	// size has relevant value this point
	copy->iImpl->iScreenSize = iImpl->iScreenSize;
	// set count to be one greater so it has a different zoom direction
	copy->iImpl->iCount = iImpl->iCount + 1;
    // copy the max thumbnail size
    copy->iImpl->iMaxThumbnailSize = iImpl->iMaxThumbnailSize;
	// return the clone
	return copy;
	}

// -----------------------------------------------------------------------------
// InitializeL
// -----------------------------------------------------------------------------
void CShwZoomAndPanEffect::InitializeL( 
	CAlfEnv* /*aHuiEnv*/, MGlxVisualList* /*aVisualList*/,
    MGlxMediaList* /*aMediaList*/, TSize aScreenSize )
	{
	GLX_LOG_INFO( "CShwZoomAndPanEffect::SetHuiEnv" );
	// set the screen size
	iImpl->iScreenSize = aScreenSize;

    // zoom and pan wants at least triple the screen size thumbnails
    TInt width = iImpl->iScreenSize.iWidth;
    TInt height = iImpl->iScreenSize.iHeight;

    // set the maximum thumbnail size, we need to optimize memory usage
    // so dont load more pixels than KMaxThumbnailSize x Screen size thumbnail
    iImpl->iMaxThumbnailSize.iWidth = width * KMaxThumbnailSize;
	iImpl->iMaxThumbnailSize.iHeight = height * KMaxThumbnailSize;
	}

// -----------------------------------------------------------------------------
// PrepareViewL
// -----------------------------------------------------------------------------
TSize CShwZoomAndPanEffect::PrepareViewL( 
	CAlfVisual* /*aVisual*/, TSize aImageSize )
	{
	GLX_LOG_INFO( "CShwZoomAndPanEffect::PrepareViewL()" );

    // use the namespace for coord utilities
    using namespace NShwGeometryUtilities;

    TSize originalImageSize = aImageSize;
    // take a local copy of the size
    TSize thumbnailSize = aImageSize;
    // check that size was really found
    if( ( aImageSize.iWidth == KErrNotFound )|| 
        ( aImageSize.iHeight == KErrNotFound ) )
        {
        // size attribute not available so use screen size
        thumbnailSize = iImpl->iScreenSize;
        // need to also adjust aImageSize as its used further to make sure
        // we dont load too big thumbnail; in this case we load screen size
        originalImageSize = iImpl->iScreenSize;
        }

    // calculate zoom first as after that we know the real panning
	// optimist assumes we can zoom every image
	iImpl->iShouldZoom = ETrue;
	// set the sizes for layout chain, return value is the zoom factor
	TReal32 zoomFactor = 
		iImpl->iZoomAndPanLayout.SetSizes( 
			iImpl->iScreenSize, thumbnailSize, iImpl->iMaxThumbnailSize );
	// if the zoomfactor is smaller than minimal desired, dont zoom&pan
	if( zoomFactor < KMinZoomAndPanFactor )
		{
		iImpl->iShouldZoom = EFalse;
		}
    // ask for the maximum size from the layout
    TSize maxSize = iImpl->iZoomAndPanLayout.MaximumSize();
    // make sure we don't load the image larger than maximum size
    thumbnailSize = maxSize;
    // thumbnail is never loaded larger than image size, 
    // either in image size or smaller... zooming may go past image size though
    FitInsideBox( 
        thumbnailSize.iWidth, thumbnailSize.iHeight,
        originalImageSize.iWidth, originalImageSize.iHeight );

	// calculate the area for the curve using the maximum size
	// it must not cause the panning to get outside screen so need to calculate
	// the delta box. We dont allow negative pan, 
	// however we need to support panning only in either direction.
    // Note that curve will have negative values, its just curve -defining 
    // box that we make positive as negative value means too small image to pan
    TInt curveWidth = Max( 
        ( maxSize.iWidth - iImpl->iScreenSize.iWidth ) / 2,
        0 );
    TInt curveHeight = Max( 
        ( maxSize.iHeight - iImpl->iScreenSize.iHeight ) / 2,
        0 );
	// delete old curve and set to NULL to prevent double delete
	delete iImpl->iCurve;
	iImpl->iCurve = NULL;
	// if both are zero, dont bother creating curve
	if( ( curveWidth > 0 ) || ( curveHeight > 0 ) )
		{
    	// create size
    	TSize curvearea( curveWidth, curveHeight );

		// construct new curve inside TRAP, if it fails we just dont pan
	    TRAP_IGNORE(
	        {
	        CAlfCurvePath* curve = 
	            NShwCurveFactory::CreateEllipsisL( 
	                curvearea, KZoomAndPanCurveLength );
	        // set new curve
	        iImpl->iCurve = curve;
	        } );
		}
	// set the curve for the layout, its ok to set NULL
//	iImpl->iZoomAndPanLayout.SetPanningCurve( iImpl->iCurve );

    // return the calculated thumbnail size
    return thumbnailSize;
	}

// -----------------------------------------------------------------------------
// EnterViewL
// -----------------------------------------------------------------------------
MGlxLayout* CShwZoomAndPanEffect::EnterViewL(
	CAlfVisual* /*aVisual*/, TInt aDuration, TInt aFadeInDuration )
    {
	GLX_LOG_INFO1( 
		"CShwZoomAndPanEffect::EnterViewL( %d )", aDuration );

	// calculate zoom&pan length
	// minimum length is the view duration and transition duration * 2
	// this is because there is transition, view and transition while
	// this visual is visible (at least partially)
	TInt zoom_and_pan_dur = aDuration + aFadeInDuration * 2;
    
	// set value, 0% -> 100%
	iImpl->iOpacityLayout.Set( KMinOpacity );
	iImpl->iOpacityLayout.Set( KMaxOpacity, aFadeInDuration );

	// check if the image was large enough for zooming
	if( iImpl->iShouldZoom )
		{
		// start with zoom in
		TShwZoomAndPanLayout::TZoomDirection zoomdir = 
			TShwZoomAndPanLayout::EZoomIn;
		if( iImpl->iCount % 2 )
			{
			zoomdir = TShwZoomAndPanLayout::EZoomOut;
			}

		// start zoom
		// minimum length is the view duration and transition duration * 2
		// this is because there is transition, view and transition while
		// this visual is visible (at least partially)
		iImpl->iZoomAndPanLayout.StartZoom( 
			zoomdir, zoom_and_pan_dur );

		// need to start a timer to invert the zoom in case image load 
		// takes longer than view mode lasts
		iImpl->iLoopTimer->Start( 
			zoom_and_pan_dur, 
			zoom_and_pan_dur,
			TShwCallBack< 
				TShwZoomAndPanLayout, 
				TShwZoomAndPanLayout::InvertZoom >( 
				&iImpl->iZoomAndPanLayout ) );
		}
	else
		{
		// reset the size to minimum
		iImpl->iZoomAndPanLayout.ResetSizeToMinimum();
		// cancel loop timer in case it is running, if not then 
		// this is a no-op
		iImpl->iLoopTimer->Cancel();
		}
	// return layout chain
	return &iImpl->iOpacityLayout;
    }

// -----------------------------------------------------------------------------
// ExitView
// -----------------------------------------------------------------------------
void CShwZoomAndPanEffect::ExitView( CAlfVisual* /*aVisual*/ )
    {
	GLX_LOG_INFO( "CShwZoomAndPanEffect::ExitView" );
	// Cancel the loop timer as we are already going to the 
	// next effect and visual
	iImpl->iLoopTimer->Cancel();
    }

// -----------------------------------------------------------------------------
// EnterTransitionL
// -----------------------------------------------------------------------------
MGlxLayout* CShwZoomAndPanEffect::EnterTransitionL(
	CAlfVisual* /*aVisual*/, TInt aDuration )
    {
    GLX_LOG_INFO1( "CShwZoomAndPanEffect::EnterTransitionL( %d )", aDuration );

	// increase count, so every other time we can zoom in and out but only if we 
	// really got to transition
	iImpl->iCount++;

	// set value, drop from 100% to 0%
    iImpl->iOpacityLayout.Set( KMaxOpacity );
    iImpl->iOpacityLayout.Set( KMinOpacity, aDuration );

	return &iImpl->iOpacityLayout;
    }

// -----------------------------------------------------------------------------
// ExitTransition
// -----------------------------------------------------------------------------
void CShwZoomAndPanEffect::ExitTransition( CAlfVisual* /*aVisual*/ )
    {
	GLX_LOG_INFO( "CShwZoomAndPanEffect::ExitTransition" );
    }

// -----------------------------------------------------------------------------
// PauseL
// -----------------------------------------------------------------------------
void CShwZoomAndPanEffect::PauseL()
    {
	GLX_LOG_INFO( "CShwZoomAndPanEffect::PauseL" );
	// need to pause the layouts, start with opacity
	iImpl->iOpacityLayout.Pause();
	// freeze zoom and pan layout
	iImpl->iZoomAndPanLayout.Pause();
	// pause loop timer, it is safe to pause the timer even if its not active
    iImpl->iLoopTimer->Pause();
	}

// -----------------------------------------------------------------------------
// Resume
// -----------------------------------------------------------------------------
void CShwZoomAndPanEffect::Resume()
    {
	GLX_LOG_INFO( "CShwZoomAndPanEffect::Resume" );
	// resume layouts
	// start with opacity
	iImpl->iOpacityLayout.Resume();
	// resume zoom and pan layout
	iImpl->iZoomAndPanLayout.Resume();
	// resume loop timer
	iImpl->iLoopTimer->Resume();
    }

// -----------------------------------------------------------------------------
// EffectInfo.
// -----------------------------------------------------------------------------
TShwEffectInfo CShwZoomAndPanEffect::EffectInfo()
	{
	return iImpl->iEffectInfo;
	}  

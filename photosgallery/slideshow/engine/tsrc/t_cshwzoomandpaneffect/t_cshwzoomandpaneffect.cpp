/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Test for zoom and pan effect in slideshow
 *
*/



//  CLASS HEADER
#include "t_cshwzoomandpaneffect.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
#include <digia/eunit/teunitfillmem.h>
#include <e32math.h>

#include <uiacceltk/huienv.h>
#include <uiacceltk/huidisplaycoecontrol.h>
#include <uiacceltk/huicurvepath.h>
#include <uiacceltk/huirealpoint.h>
#include <uiacceltk/huicontrol.h>
#include <uiacceltk/huiimagevisual.h>

//  INTERNAL INCLUDES
#include "shwzoomandpaneffect.h"
#include "shwzoomandpanlayout.h"
#include "shwcrossfadelayout.h"
#include "shwcurvefactory.h"
#include "shwconstants.h"
#include "shwautoptr.h"
#include "shwcallback.h"
#include "shwslideshowenginepanic.h"
#include "shwgeometryutilities.h"

using namespace NShwSlideshow;

// -----------------------------------------------------------------------------
// Stub for NShwEngine::Panic -->
// -----------------------------------------------------------------------------
TBool gNShwEnginePanicCalled = EFalse;
namespace NShwEngine
	{
	extern void Panic( TShwEnginePanic aPanic )
	    {
	    gNShwEnginePanicCalled = ETrue;
	    // in test situation just do a leave
	    User::Leave( aPanic );
	    }
	}
// -----------------------------------------------------------------------------
// <-- Stub for NShwEngine::Panic
// -----------------------------------------------------------------------------

//	LOCAL HELPERs
namespace
	{
	/**
	 * @param aSource the TReal32 to round
	 * @return a rounded TInt
	 */
	inline TInt TReal2TInt( TReal32 aSource )
		{
		if( aSource < 0 )
			{
			// just subst 0.5 and cast, -0.4 becomes -0.9 and typecast
			// truncates it to 0, -0.6 becomes -1.1 and its truncated to -1
			return TInt( aSource - TReal32( 0.5 ) );
			}
		else
			{
			// just add 0.5 and cast, 0.4 becomes 0.9 and typecast
			// truncates it to 0, 0.6 becomes 1.1 and its truncated to 1
			return TInt( aSource + TReal32( 0.5 ) );
			}
		}
	}

class CTestControl : public CHuiControl
	{
	public:
		static CTestControl* NewL( CHuiEnv& aEnv )
			{
			return new (ELeave) CTestControl( aEnv );
			}
		CTestControl( CHuiEnv& aEnv )
	        : CHuiControl( aEnv )
	    	{
	    	}	
	};

// CONSTRUCTION
T_CShwZoomAndPanEffect* T_CShwZoomAndPanEffect::NewL()
    {
    T_CShwZoomAndPanEffect* self = T_CShwZoomAndPanEffect::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

T_CShwZoomAndPanEffect* T_CShwZoomAndPanEffect::NewLC()
    {
    T_CShwZoomAndPanEffect* self = new( ELeave ) T_CShwZoomAndPanEffect();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
T_CShwZoomAndPanEffect::~T_CShwZoomAndPanEffect()
    {
    }

// Default constructor
T_CShwZoomAndPanEffect::T_CShwZoomAndPanEffect()
    {
    }

// Second phase construct
void T_CShwZoomAndPanEffect::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS
// global constant for screensize
const TInt gScreenWidth = 100;
const TInt gScreenHeight = 100;
const TRect gScreenRect( 0, 0, gScreenWidth, gScreenHeight );
void T_CShwZoomAndPanEffect::Empty()
	{
	}

void T_CShwZoomAndPanEffect::SetupL()
	{
	// create env
	iEnv = CHuiEnv::NewL();

	// create Display
    iCoeDisplay = CHuiDisplayCoeControl::NewL( *iEnv, gScreenRect );

	// create control
	iControl = CTestControl::NewL( *iEnv );
	
	// create the visual, ownership goes to iControl
	iVisual = CHuiImageVisual::AddNewL( *iControl );

	// create class under test
	CShwZoomAndPanEffect* tmp = CShwZoomAndPanEffect::NewLC();
    // take ownership
    iCShwZoomAndPanEffect = tmp;
	// remove the pointer from cleanup stack
	CleanupStack::Pop( tmp );
	}

void T_CShwZoomAndPanEffect::Teardown()
	{
	// delete class under test
	delete iCShwZoomAndPanEffect; 
	iCShwZoomAndPanEffect = NULL; 

	// delete control, it also deletes the visual
	delete iControl;
	iControl = NULL;

	// delete display
	delete iCoeDisplay;
	iCoeDisplay = NULL;

	// delete environment
	delete iEnv;
	iEnv = NULL;
	}

void T_CShwZoomAndPanEffect::T_TestGeometryAlgorithmsL()
    {
    // use the namespace for coord utilities
    using namespace NShwGeometryUtilities;
    
    // test     FitDimension
        // fit width with same values
        {
        TInt width = 10;
        TInt height = 10;
        FitDimension( width, height, 10 );
        EUNIT_ASSERT_EQUALS_DESC( 10, width, "width is same" );
        EUNIT_ASSERT_EQUALS_DESC( 10, height, "height is same" );
        }
        // fit width with width greater
        {
        TInt width = 20;
        TInt height = 10;
        FitDimension( width, height, 10 );
        EUNIT_ASSERT_EQUALS_DESC( 20, width, "width is same" );
        EUNIT_ASSERT_EQUALS_DESC( 10, height, "height is same" );
        }
        // fit width with new height greater
        {
        TInt width = 20;
        TInt height = 10;
        FitDimension( width, height, 20 );
        EUNIT_ASSERT_EQUALS_DESC( 40, width, "width increased" );
        EUNIT_ASSERT_EQUALS_DESC( 20, height, "height increased" );
        }
        // fit width with new height smaller
        {
        TInt width = 22;
        TInt height = 11;
        FitDimension( width, height, 5 );
        EUNIT_ASSERT_EQUALS_DESC( 10, width, "width decreased" );
        EUNIT_ASSERT_EQUALS_DESC( 5, height, "height decreased" );
        }

        // fit height with same values
        {
        TInt width = 99;
        TInt height = 88;
        FitDimension( height, width, 99 );
        EUNIT_ASSERT_EQUALS_DESC( 99, width, "width is same" );
        EUNIT_ASSERT_EQUALS_DESC( 88, height, "height is same" );
        }
        // fit height with width greater
        {
        TInt width = 22;
        TInt height = 11;
        FitDimension( height, width, 22 );
        EUNIT_ASSERT_EQUALS_DESC( 22, width, "width is same" );
        EUNIT_ASSERT_EQUALS_DESC( 11, height, "height is same" );
        }
        // fit height with new width greater
        {
        TInt width = 22;
        TInt height = 11;
        FitDimension( height, width, 33 );
        EUNIT_ASSERT_EQUALS_DESC( 33, width, "width increased" );
        EUNIT_ASSERT_EQUALS_DESC( 16, height, "height increased" );
        }
        // fit height with new width smaller
        {
        TInt width = 99;
        TInt height = 88;
        FitDimension( height, width, 88 );
        EUNIT_ASSERT_EQUALS_DESC( 88, width, "width increased" );
        EUNIT_ASSERT_EQUALS_DESC( 78, height, "height increased" );
        }

    // test     FitInsideBox
        // fit with same width and height
        {
        TInt width = 99;
        TInt height = 99;
        FitInsideBox( width, height, 99, 99 );
        EUNIT_ASSERT_EQUALS_DESC( 99, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 99, height, "height is same as box" );
        }
        // fit with smaller width and height
        {
        TInt width = 99;
        TInt height = 99;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 99, width, "width did not change" );
        EUNIT_ASSERT_EQUALS_DESC( 99, height, "height did not change" );
        }
        // fit with greater width and height
        {
        TInt width = 111;
        TInt height = 111;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 100, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }
        // fit with greater and width closer to box
        {
        TInt width = 150;
        TInt height = 200;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 75, width, "width is smaller" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }
        // fit with greater and height closer to box
        {
        TInt width = 150;
        TInt height = 100;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 100, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 66, height, "height is smaller" );
        }
        // fit with smaller and width closer to box
        {
        TInt width = 75;
        TInt height = 40;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 75, width, "width did not change" );
        EUNIT_ASSERT_EQUALS_DESC( 40, height, "height did not change" );
        }
        // fit with smaller and height closer to box
        {
        TInt width = 60;
        TInt height = 90;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 60, width, "width did not change" );
        EUNIT_ASSERT_EQUALS_DESC( 90, height, "height did not change" );
        }
        // fit with width greater and height smaller to box
        {
        TInt width = 110;
        TInt height = 90;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 100, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 81, height, "height is smaller" );
        }
        // fit with height greater and width smaller to box
        {
        TInt width = 90;
        TInt height = 120;
        FitInsideBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 90*100/120, width, "width is smaller" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }

    // test     FitToCoverBox
        // fit with same width and height
        {
        TInt width = 99;
        TInt height = 99;
        FitToCoverBox( width, height, 99, 99 );
        EUNIT_ASSERT_EQUALS_DESC( 99, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 99, height, "height is same as box" );
        }
        // fit with smaller width and height
        {
        TInt width = 99;
        TInt height = 99;
        FitToCoverBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 100, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }
        // fit with greater width and height
        {
        TInt width = 111;
        TInt height = 111;
        FitToCoverBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 100, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }
        // fit with greater and width closer to box
        {
        TInt width = 150;
        TInt height = 200;
        FitToCoverBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 100, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 133, height, "height is greater" );
        }
        // fit with greater and height closer to box
        {
        TInt width = 150;
        TInt height = 100;
        FitToCoverBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 150, width, "width is greater than box" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }
        // fit with greater and height closer to box
        {
        TInt width = 300;
        TInt height = 200;
        FitToCoverBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 150, width, "width is greater than box" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }
        // fit with smaller and width closer to box
        {
        TInt width = 75;
        TInt height = 40;
        FitToCoverBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 187, width, "width is greater" );
        EUNIT_ASSERT_EQUALS_DESC( 100, height, "height is same as box" );
        }
        // fit with smaller and height closer to box
        {
        TInt width = 60;
        TInt height = 90;
        FitToCoverBox( width, height, 100, 100 );
        EUNIT_ASSERT_EQUALS_DESC( 100, width, "width is same as box" );
        EUNIT_ASSERT_EQUALS_DESC( 150, height, "height is greater" );
        }
    }

void T_CShwZoomAndPanEffect::T_LayoutTestL()
	{
// test zoom and pan
	// create the layout
	TEUnitFillMem< TShwZoomAndPanLayout > layout;

	// check that layout has correct init values
	// need to upcast to get access to public functions
	MGlxLayout& base = static_cast< MGlxLayout& >( layout );
	TGlxLayoutInfoResetter info;
	info.iPosition.iX = -1;
	info.iPosition.iY = -1;
	info.iSize.iX = -1;
	info.iSize.iY = -1;

	base.SetLayoutValues( info );
	// verify that the position was not set
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iX,"x position not changed" );
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iY,"y position not changed" );
	// check size
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		-1, (TInt)info.iSize.iX,"x size changed" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		-1, (TInt)info.iSize.iY,"y size changed" );

	// set custom screen size and image size; image smaller than screen
	layout.SetSizes( TSize( 99, 88 ), TSize( 44, 55 ), TSize( 500, 500 ) );
	// redo layout
	base.SetLayoutValues( info );
	// verify that the position was not set
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iX,"x position not changed" );
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iY,"y position not changed" );
	// check size
	EUNIT_ASSERT_EQUALS_DESC( 99, TReal2TInt( info.iSize.iX ),"x size changed" );
	EUNIT_ASSERT_EQUALS_DESC( 124, TReal2TInt( info.iSize.iY ),"y size changed" );

	// set custom screen size and image size; image larger than screen
	layout.SetSizes( TSize( 99, 88 ), TSize( 144, 155 ), TSize( 500, 500 ) );
	// redo layout
	base.SetLayoutValues( info );
	// verify that the position was not set
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iX,"x position not changed" );
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iY,"y position not changed" );
	// check size
	EUNIT_ASSERT_EQUALS_DESC( 99, TReal2TInt( info.iSize.iX ),"x size changed" );
	EUNIT_ASSERT_EQUALS_DESC( 107, TReal2TInt( info.iSize.iY ),"y size changed" );

	// set custom screen size and image size; image partially larger than screen
	layout.SetSizes( TSize( 99, 88 ), TSize( 100, 15 ), TSize( 500, 500 ) );
	// redo layout
	base.SetLayoutValues( info );
	// verify that the position was not set
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iX,"x position not changed" );
	EUNIT_ASSERT_EQUALS_DESC( -1, (TInt)info.iPosition.iY,"y position not changed" );
	// check size
	EUNIT_ASSERT_EQUALS_DESC( 587, TReal2TInt( info.iSize.iX ),"x size changed" );
	EUNIT_ASSERT_EQUALS_DESC( 88, TReal2TInt( info.iSize.iY ),"y size changed" );

	// set big enough image so that it zooms
	layout.SetSizes( TSize( 100, 100 ), TSize( 200, 150 ), TSize( 500, 500 ) );
	layout.StartZoom( TShwZoomAndPanLayout::EZoomIn, 0 );
	// redo layout
	base.SetLayoutValues( info );
	// check that layout is changed
	EUNIT_ASSERT_DESC( layout.Changed(), "layout is changed" );
	// clear changeflag
	layout.ClearChanged();
	EUNIT_ASSERT_DESC( !layout.Changed(), "layout is not changed" );

// test crossfade
	// create the crossfade layout
	TEUnitFillMem< TShwCrossFadeLayout > cflayout;
	// get base class pointer
	MGlxLayout& base2 = static_cast< MGlxLayout& >( cflayout );
    // reset info opacity
	info.iOpacity = -1;
    // run the layout
	base2.SetLayoutValues( info );
	// verify that opacity was set to minimum
	EUNIT_ASSERT_EQUALS_DESC(
	    TInt( KMinOpacity ), TReal2TInt( info.iOpacity ),"opacity is minimum" );
	// check that layout is not changed
	EUNIT_ASSERT_DESC( !cflayout.Changed(), "layout is not changed" );
    // set new value
    cflayout.Set( 2.0, 0 );
    // reset info opacity
	info.iOpacity = -1;
    // run the layout
	base2.SetLayoutValues( info );
	// check that layout is changed
	EUNIT_ASSERT_DESC( cflayout.Changed(), "layout is changed" );
	// verify that opacity was set to maximum
	EUNIT_ASSERT_EQUALS_DESC( 
	    TInt( KMaxOpacity ), TReal2TInt( info.iOpacity ),"opacity is maximum" );
	// clear change flag
	cflayout.ClearChanged();
	EUNIT_ASSERT_DESC( !cflayout.Changed(), "layout is not changed" );
	}

void T_CShwZoomAndPanEffect::T_CurveTestL()
	{
	// create env, no need to delete or cleanupstack
	TShwAutoPtr< CHuiEnv > env = CHuiEnv::NewL();

	// create curve path with 200 length
	TShwAutoPtr< CHuiCurvePath > ellipsis = 
		NShwCurveFactory::CreateEllipsisL( TSize( 100, 200 ), 200 );

	// the ellipsis is clockwise around the box
	// check ellipsis values, point 0
	TReal32 x_value = ellipsis->MapValue( 0, 0 );
	TReal32 y_value = ellipsis->MapValue( 0, 1 );
	EUNIT_ASSERT_EQUALS_DESC( 50, TReal2TInt( x_value ), "x coordinate");
	EUNIT_ASSERT_EQUALS_DESC( 0, TReal2TInt( y_value ), "y coordinate");

	// point 50
	x_value = ellipsis->MapValue( 50, 0 );
	y_value = ellipsis->MapValue( 50, 1 );
	EUNIT_ASSERT_EQUALS_DESC( 0, TReal2TInt( x_value ), "x coordinate");
	EUNIT_ASSERT_EQUALS_DESC( 100, TReal2TInt( y_value ), "y coordinate");
	
	// point 100
	x_value = ellipsis->MapValue( 100, 0 );
	y_value = ellipsis->MapValue( 100, 1 );
	EUNIT_ASSERT_EQUALS_DESC( -50, TReal2TInt( x_value ), "x coordinate");
	EUNIT_ASSERT_EQUALS_DESC( 0, TReal2TInt( y_value ), "y coordinate");

	// point 150
	x_value = ellipsis->MapValue( 150, 0 );
	y_value = ellipsis->MapValue( 150, 1 );
	EUNIT_ASSERT_EQUALS_DESC( 0, TReal2TInt( x_value ), "x coordinate");
	EUNIT_ASSERT_EQUALS_DESC( -100, TReal2TInt( y_value ), "y coordinate");

	// point 200
	x_value = ellipsis->MapValue( 200, 0 );
	y_value = ellipsis->MapValue( 200, 1 );
	EUNIT_ASSERT_EQUALS_DESC( 50, TReal2TInt( x_value ), "x coordinate");
	EUNIT_ASSERT_EQUALS_DESC( 0, TReal2TInt( y_value ), "y coordinate");
	}

void T_CShwZoomAndPanEffect::T_ZoomAndPanTestL()
	{
	// create env, no need to delete or cleanupstack
	TShwAutoPtr< CHuiEnv > env = CHuiEnv::NewL();

	// size of screen
	const TInt screenX = 320;
	const TInt screenY = 200;
	TSize screen( screenX, screenY );
	// size of image, smaller than screen x KMaxZoomAndPanFactor
	// but larger than screen
	const TInt originalImageX = ( screenX + screenX * KMaxZoomAndPanFactor ) / 2;
	const TInt originalImageY = ( screenY + screenY * KMaxZoomAndPanFactor ) / 2;
	TSize image( originalImageX, originalImageY );
	// create curve path with 100 length
	TShwAutoPtr< CHuiCurvePath > ellipsis = 
		NShwCurveFactory::CreateEllipsisL( screen, 100 );

	// create the layout
	TEUnitFillMem< TShwZoomAndPanLayout > layout;
	// set screen and image size and maximum size
	layout.SetSizes( screen, image, 
	    TSize( screenX * KMaxZoomAndPanFactor, screenY * KMaxZoomAndPanFactor ) );
	// set the panning curve
	layout.SetPanningCurve( &ellipsis );

	// check that layout has correct init values
	// need to upcast to get access to public functions
	MGlxLayout& base = static_cast< MGlxLayout& >( layout );
	TGlxLayoutInfoResetter info;
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	// run the layout
	base.SetLayoutValues( info );
	// verify that the position was set properly, the initial size is minimum
	// so pan gets scaled to 0
	TInt initialXonCurve = 160;
	TInt initialYonCurve = 0;
	EUNIT_ASSERT_EQUALS_DESC(
		0, TReal2TInt( info.iPosition.iX ),"x position set" );
	EUNIT_ASSERT_EQUALS_DESC(
		0, TReal2TInt( info.iPosition.iY ),"y position set" );
	// verify size
	EUNIT_ASSERT_EQUALS_DESC( 
		screenX, TReal2TInt( info.iSize.iX ),"default x size is screen size" );
	EUNIT_ASSERT_EQUALS_DESC( 
		screenY, TReal2TInt( info.iSize.iY ),"default y size is screen size" );

	// do zoom in in 1 second
	layout.StartZoom( TShwZoomAndPanLayout::EZoomIn, 1 );

	// create timer to give us callback
	TShwAutoPtr< CPeriodic > timer = CPeriodic::NewL( CActive::EPriorityStandard );
	// wait for 1.5 seconds (to be sure the zoom completes)
    timer->Start(1.5 * 1000000, 1.5 * 1000000, TShwCallBack<
            T_CShwZoomAndPanEffect, &T_CShwZoomAndPanEffect::CancelAsyncL> (
            this));
	// start async wait
	iAsyncWait.Start();

	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	// run the layout
	base.SetLayoutValues( info );
	// verify that the position was set; zoom in does one half of the curve
	EUNIT_ASSERT_EQUALS_DESC( 
		-initialXonCurve, TReal2TInt( info.iPosition.iX ),"x position looped on the opposite side" );
	EUNIT_ASSERT_EQUALS_DESC( 
		initialYonCurve, TReal2TInt( info.iPosition.iY ),"y position looped back" );
	// verify size, after zoom in we are in 4x screen size
	EUNIT_ASSERT_GREATER_DESC( 
		TReal2TInt( info.iSize.iX ), originalImageX,"x size is greater than original size" );
	EUNIT_ASSERT_GREATER_DESC( 
		TReal2TInt( info.iSize.iY ), originalImageY,"y size is greater than original size" );

	// perform zoom out in one second
	layout.StartZoom( TShwZoomAndPanLayout::EZoomOut, 1 );
	// cancel old timer
	timer->Cancel();
	// wait for 1.5 seconds (to be sure the zoom completes)
    timer->Start(1.5 * 1000000, 1.5 * 1000000, TShwCallBack<
            T_CShwZoomAndPanEffect, &T_CShwZoomAndPanEffect::CancelAsyncL> (
            this));
	// start async wait
	iAsyncWait.Start();

	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	// run the layout
	base.SetLayoutValues( info );
	// verify that the position was set to zero again as in minimum size the pan is 0
	EUNIT_ASSERT_EQUALS_DESC( 
		0, TReal2TInt( info.iPosition.iX ), "x position looped " );
	EUNIT_ASSERT_EQUALS_DESC( 
		0, TReal2TInt( info.iPosition.iY ), "y position looped back" );
	// verify size, after zoom in we are in 100% size
	EUNIT_ASSERT_EQUALS_DESC( 
		screenX, TReal2TInt( info.iSize.iX ),"x size is back to minimum" );
	EUNIT_ASSERT_EQUALS_DESC( 
		screenY, TReal2TInt( info.iSize.iY ),"y size is back to minimum" );
	}

void T_CShwZoomAndPanEffect::T_PauseTestL()
	{
	// display size is define by gScreenRect
	// give the HUI env to the effect and the size
	TSize screenSize = gScreenRect.Size();
	iCShwZoomAndPanEffect->InitializeL( 
	    iEnv,
	    NULL,
	    NULL,
	    screenSize );

	// prepare view with a max size image
	TSize imageSize( screenSize.iWidth * KMaxThumbnailSize, screenSize.iHeight * KMaxThumbnailSize );
	TSize thumbSize = iCShwZoomAndPanEffect->PrepareViewL( iVisual, imageSize );
	// check thumbnail size
	EUNIT_ASSERT_EQUALS_DESC( 
		imageSize.iHeight, thumbSize.iHeight, "thumbnail is image size" );
	EUNIT_ASSERT_EQUALS_DESC( 
		imageSize.iWidth, thumbSize.iWidth, "thumbnail is image size" );

	// then enter view, fade in should last 250 millliseconds and view 500
	// get the layout chain
	MGlxLayout* layout = 
		iCShwZoomAndPanEffect->EnterViewL( iVisual, 500, 250 );
	// get the initial layout values
	TGlxLayoutInfoResetter info;
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
	
	// next pause the effect
	iCShwZoomAndPanEffect->PauseL();
	// create timer to give us callback
	TShwAutoPtr< CPeriodic > timer = CPeriodic::NewL( CActive::EPriorityStandard );
	// start asynch wait for 1.5 second
    timer->Start(1.5 * 1000000, 1.5 * 1000000, TShwCallBack<
            T_CShwZoomAndPanEffect, &T_CShwZoomAndPanEffect::CancelAsyncL> (
            this));
	// start async wait
	iAsyncWait.Start();
	// cancel the timer
	timer->Cancel();

	// now verify that the layout chain is in same situation
	// get new layout values
	TGlxLayoutInfoResetter info2;
	// reset info2
	info2.iPosition.iX = -1;info2.iPosition.iY = -1;
	info2.iSize.iX = -1; info2.iSize.iY = -1;
	info2.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info2 );
	// check that no changes
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info.iOpacity ), TReal2TInt( info2.iOpacity ), "opacity" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info.iPosition.iX ), TReal2TInt( info2.iPosition.iX ), "position x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info.iPosition.iY ), TReal2TInt( info2.iPosition.iY ), "position y" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info.iSize.iX ), TReal2TInt( info2.iSize.iX ), "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info.iSize.iY ), TReal2TInt( info2.iSize.iY ), "size y" );

	// resume the effect
	iCShwZoomAndPanEffect->Resume();
	
	// start timer for 1.5 seconds
    timer->Start(1.5 * 1000000, 1.5 * 1000000, TShwCallBack<
            T_CShwZoomAndPanEffect, &T_CShwZoomAndPanEffect::CancelAsyncL> (
            this));
	// start async wait
	iAsyncWait.Start();
	// cancel the timer
	timer->Cancel();

	// now verify that the layout chain did change
	// reset info2
	info2.iPosition.iX = -1;info2.iPosition.iY = -1;
	info2.iSize.iX = -1; info2.iSize.iY = -1;
	info2.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info2 );
    // check that values did not change
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iOpacity ), TReal2TInt( info2.iOpacity ), "opacity" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iSize.iX ), TReal2TInt( info2.iSize.iX ), "size x" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iSize.iY ), TReal2TInt( info2.iSize.iY ), "size y" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iPosition.iX ), TReal2TInt( info2.iPosition.iX ), "position x" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iPosition.iY ), TReal2TInt( info2.iPosition.iY ), "position y" );

	// enter view again, fade in should last 250 millliseconds and view 500
	// get the layout chain
	layout = iCShwZoomAndPanEffect->EnterViewL( iVisual, 500, 250 );
	// get the initial layout values
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
	// start timer for .1 seconds, to make sure opacity does not run too fast
    timer->Start(0.1 * 1000000, 0.1 * 1000000, TShwCallBack<
            T_CShwZoomAndPanEffect, &T_CShwZoomAndPanEffect::CancelAsyncL> (
            this));
	// start async wait
	iAsyncWait.Start();
	// cancel the timer
	timer->Cancel();

	// reset info2
	info2.iPosition.iX = -1;info2.iPosition.iY = -1;
	info2.iSize.iX = -1; info2.iSize.iY = -1;
	info2.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info2 );
	// check that size and opacity changed, multiply with 10 to remove rounding errors
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iOpacity * 10 ), TReal2TInt( info2.iOpacity * 10 ), "opacity" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iSize.iX ), TReal2TInt( info2.iSize.iX ), "size x" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info.iSize.iY ), TReal2TInt( info2.iSize.iY ), "size y" );

	// pause the effect
	iCShwZoomAndPanEffect->PauseL();
	// run the layout to get values
	layout->SetLayoutValues( info2 );
	// start timer for 1.0 seconds
    timer->Start(1.0 * 1000000, 1.0 * 1000000, TShwCallBack<
            T_CShwZoomAndPanEffect, &T_CShwZoomAndPanEffect::CancelAsyncL> (
            this));
	// start async wait
	iAsyncWait.Start();
	// cancel the timer
	timer->Cancel();
	TGlxLayoutInfoResetter info3;
	// reset info3
	info3.iPosition.iX = -1;info3.iPosition.iY = -1;
	info3.iSize.iX = -1; info3.iSize.iY = -1;
	info3.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info3 );
	// check that no changes between info2 and info3, multiply opacity to remove rounding error
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info2.iOpacity * 10 ), TReal2TInt( info3.iOpacity * 10 ), "opacity" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info2.iSize.iX ), TReal2TInt( info3.iSize.iX ), "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info2.iSize.iY ), TReal2TInt( info3.iSize.iY ), "size y" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info2.iPosition.iX ), TReal2TInt( info3.iPosition.iX ), "position x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TReal2TInt( info2.iPosition.iY ), TReal2TInt( info3.iPosition.iY ), "position y" );

	// now do the resume
	iCShwZoomAndPanEffect->Resume();
	// start timer for 1.0 seconds
    timer->Start(1.0 * 1000000, 1.0 * 1000000, TShwCallBack<
            T_CShwZoomAndPanEffect, &T_CShwZoomAndPanEffect::CancelAsyncL> (
            this));
	// start async wait
	iAsyncWait.Start();
	// cancel the timer
	timer->Cancel();

	// reset info3
	info3.iPosition.iX = -1;info3.iPosition.iY = -1;
	info3.iSize.iX = -1; info3.iSize.iY = -1;
	info3.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info3 );
	// check that values  did change between info2 and info3, multiply opacity to remove rounding error
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info2.iOpacity * 10 ), TReal2TInt( info3.iOpacity * 10 ), "opacity" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info2.iSize.iX ), TReal2TInt( info3.iSize.iX ), "size x" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info2.iSize.iY ), TReal2TInt( info3.iSize.iY ), "size y" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info2.iPosition.iX ), TReal2TInt( info3.iPosition.iX ), "position x" );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		TReal2TInt( info2.iPosition.iY ), TReal2TInt( info3.iPosition.iY ), "position y" );
	}

void T_CShwZoomAndPanEffect::T_TestBoundariesL()
    {
    // get the screen size
	TSize screenSize = gScreenRect.Size();
    // calculate the maximum width and height
    TInt maximumImageWidth = screenSize.iWidth * KMaxThumbnailSize;
    TInt maximumImageHeight = screenSize.iHeight * KMaxThumbnailSize;
    
	// display size is define by gScreenRect
	// give the HUI env to the effect and the size
	iCShwZoomAndPanEffect->InitializeL( 
	    iEnv,
	    NULL,
	    NULL,
	    screenSize );

	// prepare view with image twice as wide but half the height of maximum
	TSize imageSize( 
	    maximumImageWidth * 2, 
	    maximumImageHeight / 2 );
	TSize thumbSize = iCShwZoomAndPanEffect->PrepareViewL( iVisual, imageSize );
    // check the thumbnail size
    // note that the thumbnail may be wider than higher than the maximage but the area
    // is the same
	EUNIT_ASSERT_EQUALS_DESC( 
		maximumImageWidth * 2, thumbSize.iWidth, "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		maximumImageHeight / 2, thumbSize.iHeight, "size y" );

	// then enter view, fade in should last 0 millliseconds and view 0 so that the new values
	// are immediate
	// get the layout chain
	MGlxLayout* layout = 
		iCShwZoomAndPanEffect->EnterViewL( iVisual, 0, 0 );
	// get the initial layout values
	TGlxLayoutInfoResetter info;
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
    // check that image width is maximum screen width times two and height is 
    // original by two (as original was twice as wide as fitted)
    // note that the thumbnail may be wider than higher than the maximage but the area
    // is the same
	EUNIT_ASSERT_EQUALS_DESC(
		maximumImageWidth * 2, TReal2TInt( info.iSize.iX ), "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		maximumImageHeight / 2, TReal2TInt( info.iSize.iY ), "size y" );

	// prepare view with image max wide but twice the height of maximum
	imageSize.SetSize( maximumImageWidth, maximumImageHeight * 2 );
	iCShwZoomAndPanEffect->PrepareViewL( iVisual, imageSize );
	// then enter view, fade in should last 0 millliseconds and view 0 so that the new values
	// are immediate
	// get the layout chain
	layout = iCShwZoomAndPanEffect->EnterViewL( iVisual, 0, 0 );
	// get the initial layout values
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
    // check that image area is same as maximum image area
    // note that the thumbnail may be wider than higher than the maximage but the area
    // is the same
	EUNIT_ASSERT_EQUALS_DESC( 
		maximumImageWidth * maximumImageHeight, 
		TReal2TInt( info.iSize.iX * info.iSize.iY ), "size x" );

    // test image partially smaller than screen, should not zoom
	// prepare view with image quarter of the screen wide but four times the height of screen
	imageSize.SetSize( gScreenWidth / 4, gScreenHeight * 4 );
	iCShwZoomAndPanEffect->PrepareViewL( iVisual, imageSize );
	// then enter view, fade in should last 0 millliseconds and view 0 so that the new values
	// are immediate
	// get the layout chain
	layout = iCShwZoomAndPanEffect->EnterViewL( iVisual, 0, 0 );
	// get the initial layout values
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
    // check that image size is the maximum screen width and height multiplied by 16
	EUNIT_ASSERT_EQUALS_DESC( 
		gScreenWidth, TReal2TInt( info.iSize.iX ), "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		gScreenHeight * 4 * 4, TReal2TInt( info.iSize.iY ), "size y" );

    // test image partially smaller than screen, should zoom
	// prepare view with image half of the screen wide but three times the height of screen
	imageSize.SetSize( gScreenWidth - 10, gScreenHeight * 2 );
	iCShwZoomAndPanEffect->PrepareViewL( iVisual, imageSize );
	// then enter view, fade in should last 0 millliseconds and view 0 so that the new values
	// are immediate
	// get the layout chain
	layout = iCShwZoomAndPanEffect->EnterViewL( iVisual, 0, 0 );
	// get the initial layout values
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
    // check image area, should be same as max size
	EUNIT_ASSERT_EQUALS_DESC( 
		maximumImageWidth * maximumImageHeight, 
		TReal2TInt( info.iSize.iX * info.iSize.iY ), "size x" );

    // test image that is screen size, should not zoom
	// prepare view with image 
	imageSize.SetSize( gScreenWidth, gScreenHeight );
	iCShwZoomAndPanEffect->PrepareViewL( iVisual, imageSize );
	// then enter view, fade in should last 0 millliseconds and view 0 so that the new values
	// are immediate
	// get the layout chain
	layout = iCShwZoomAndPanEffect->EnterViewL( iVisual, 0, 0 );
	// get the initial layout values
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
    // check that image size is screen size
	EUNIT_ASSERT_EQUALS_DESC( 
		gScreenWidth, TReal2TInt( info.iSize.iX ), "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		gScreenHeight, TReal2TInt( info.iSize.iY ), "size y" );

    // test image partially larger than screen, should zoom
	// prepare view
	imageSize.SetSize( gScreenWidth * 1.5, gScreenHeight * 1.5 );
	iCShwZoomAndPanEffect->PrepareViewL( iVisual, imageSize );
	// then enter view, fade in should last 0 millliseconds and view 0 so that the new values
	// are immediate
	// get the layout chain
	layout = iCShwZoomAndPanEffect->EnterViewL( iVisual, 0, 0 );
	// get the initial layout values
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
    // check that image size is the screen multiplied by max zoom
	EUNIT_ASSERT_EQUALS_DESC( 
		TInt(gScreenWidth * KMaxZoomAndPanFactor), TReal2TInt( info.iSize.iX ), "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		TInt(gScreenHeight * KMaxZoomAndPanFactor), TReal2TInt( info.iSize.iY ), "size y" );
		
	// enter transition to increase the counter, duration zero
	// ignore layout chain
	iCShwZoomAndPanEffect->EnterTransitionL( iVisual, 0 );
    // enter the same effect again, this time we do zoom out (max to min size)
	// get the layout chain
	layout = iCShwZoomAndPanEffect->EnterViewL( iVisual, 0, 0 );
	// reset info
	info.iPosition.iX = -1;info.iPosition.iY = -1;
	info.iSize.iX = -1; info.iSize.iY = -1;
	info.iOpacity = -1;
	// run the layout to get values
	layout->SetLayoutValues( info );
    // check that image size is the screen multiplied by max zoom
	EUNIT_ASSERT_EQUALS_DESC( 
		gScreenWidth, TReal2TInt( info.iSize.iX ), "size x" );
	EUNIT_ASSERT_EQUALS_DESC( 
		gScreenHeight, TReal2TInt( info.iSize.iY ), "size y" );
    }

TInt T_CShwZoomAndPanEffect::CancelAsyncL()
	{
	// stop async wait
	iAsyncWait.AsyncStop();
	// return KErrNone to stop the timer
	return KErrNone;
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwZoomAndPanEffect,
    "Test suite for CShwZoomAndPanEffect and Layout",
    "UNIT" )

EUNIT_TEST(
    "Geometry utilities test",
    "NShwGeometryUtilities",
    "FitDimension,FitInsideBox,FitToCoverBox",
    "FUNCTIONALITY",
    Empty, T_TestGeometryAlgorithmsL, Empty )

EUNIT_TEST(
    "Layout test",
    "TShwZoomAndPanLayout",
    "TShwZoomAndPanLayout",
    "FUNCTIONALITY",
    Empty, T_LayoutTestL, Empty )

EUNIT_TEST(
    "Curve test",
    "ShwCurveFactory",
    "CreateEllipsisL",
    "FUNCTIONALITY",
    Empty, T_CurveTestL, Empty )

EUNIT_TEST(
    "Zoom and pan test",
    "TShwZoomAndPanLayout",
    "DoSetLayoutValues",
    "FUNCTIONALITY",
    Empty, T_ZoomAndPanTestL, Empty )

EUNIT_TEST(
    "Pause Zoom and pan",
    "CShwZoomAndPanEffect",
    "PauseL, Resume",
    "FUNCTIONALITY",
    SetupL, T_PauseTestL, Teardown )

EUNIT_TEST(
    "Test boundaries",
    "CShwZoomAndPanEffect",
    "PrepareViewL",
    "FUNCTIONALITY",
    SetupL, T_TestBoundariesL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE

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
* Description:    Zoom and pan effect curve factory
 *
*/




//  Include Files
#include "shwcurvefactory.h"

#include <alf/alfcurvepath.h>

// -----------------------------------------------------------------------------
// NShwCurveFactory.
// -----------------------------------------------------------------------------
namespace NShwCurveFactory
	{
	// constant for arc angle
	//const TInt KStartAngle = 0;	// start at 0 degrees
	//const TInt KStopAngle = 360;	// end in in 360 degrees
	//const TInt KOrigo = 0;	// centre is in origo
	// -------------------------------------------------------------------------
	// CreateEllipsisL.
	// -------------------------------------------------------------------------
	CAlfCurvePath* CreateEllipsisL( TSize /*aSize*/, TInt /*aLength*/ )
		{
	//!Nasty Hack to supress compiler warnings
	//function no longer used
	//needs to be refactored/removed
		CAlfCurvePath* curve =NULL;
	/*	CAlfCurvePath* curve = CAlfCurvePath::NewL();
		CleanupStack::PushL( curve );
		// HUI creates a double sized ellipsis so need to halve the sides
		TSize realSize( aSize.iWidth / 2, aSize.iHeight / 2 );
		// create the arch for given length
		curve->AppendArcL( 
			TPoint( KOrigo, KOrigo ),	// centre point
			realSize,	// sides of the rect defining the ellipse
			KStartAngle,	// start angle
			KStopAngle, // end angle 
			aLength );	// the whole circle
		// enable loop
        CleanupStack::Pop( curve );
		curve->EnableLoop( ETrue );*/
		return curve;
		}
	}

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
* Description:    Set coefficient layout
*
*/




#include "glxsetvaluelayout.h"
#include <alf/alfvisual.h>
#include <glxpanic.h> // Panic codes

namespace
    {
    const TInt KModifyOpacity  = 0x01;
    const TInt KModifySize     = 0x02;
    const TInt KModifyPosition = 0x04;
    }

// -----------------------------------------------------------------------------
// TGlxSetValueLayout
// -----------------------------------------------------------------------------
EXPORT_C TGlxSetValueLayout::TGlxSetValueLayout()
	: iOpacityValue( 0.0 ),	// set initial values
	iSizeValue( 0.0, 0.0 ),
	iPositionValue( 0.0, 0.0 ),
	iValuesToChange( 0 )
	{
	__DEBUG_ONLY( _iName = _L( "TGlxSetValueLayout" ) );
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxSetValueLayout::~TGlxSetValueLayout()
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// SetValue
// -----------------------------------------------------------------------------
EXPORT_C void TGlxSetValueLayout::SetOpacity( 
	TReal32 aTarget, 
	TInt aMilliseconds )
	{
	// flag that we change opacity
	iValuesToChange |= KModifyOpacity;
	// set the value for alf timed value
	iOpacityValue.SetTarget( aTarget, aMilliseconds );
	}

// -----------------------------------------------------------------------------
// SetValue
// -----------------------------------------------------------------------------
EXPORT_C void TGlxSetValueLayout::SetSize( 
	TReal32 aTargetWidth, TReal32 aTargetHeight, TInt aMilliseconds )
	{
	// flag that we change size
	iValuesToChange |= KModifySize;
	// set the value for alf timed point
	iSizeValue.SetTarget(
	    TAlfRealPoint( aTargetWidth, aTargetHeight ), aMilliseconds );
	}

// -----------------------------------------------------------------------------
// TimedPoint
// -----------------------------------------------------------------------------			
EXPORT_C void TGlxSetValueLayout::SetPosition( 
	TReal32 aTargetX, TReal32 aTargetY, TInt aMilliseconds )
	{
	// flag that we change position
	iValuesToChange |= KModifyPosition;
	// set the value for alf timed point
	iPositionValue.SetTarget( TAlfRealPoint( aTargetX, aTargetY ), aMilliseconds );
	}
	
// -----------------------------------------------------------------------------
// DoMapValue
// -----------------------------------------------------------------------------
void TGlxSetValueLayout::DoSetLayoutValues( TGlxLayoutInfo& aInfo )
	{
	CAlfVisual& visual = aInfo.Visual();
	// modify all the specified values
	if( iValuesToChange & KModifyOpacity )
		{
		// change the opacity
		visual.SetOpacity( iOpacityValue );
		}
	if( iValuesToChange & KModifySize )
		{
		// change the size
		visual.SetSize( iSizeValue );
		}
	if( iValuesToChange & KModifyPosition )
		{
		// change the position
		visual.SetPos( iPositionValue );
		}
	}


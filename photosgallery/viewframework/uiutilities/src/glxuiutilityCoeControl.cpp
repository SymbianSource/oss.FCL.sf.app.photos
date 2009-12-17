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
* Description:    Implementation of the CGlxUiUtilityCoeControl class
*
*/




#include "glxuiutilitycoecontrol.h"  // Class definition

#include <uiacceltk/huidisplay.h>
#include <uiacceltk/huievent.h>
#include <uiacceltk/huiskin.h>

//-----------------------------------------------------------------------------
// NewL
//-----------------------------------------------------------------------------
CGlxUiUtilityCoeControl* CGlxUiUtilityCoeControl::NewL(
	CHuiEnv& aEnv, AknLayoutUtils::TAknLayoutMetrics aArea, TInt aFlags )
    {
    CGlxUiUtilityCoeControl* self = 
    	new (ELeave) CGlxUiUtilityCoeControl( aEnv );
    CleanupStack::PushL( self );
    // 2nd phase
    self->ConstructL( aArea, aFlags );
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGlxUiUtilityCoeControl::~CGlxUiUtilityCoeControl()
    {
    }

//-----------------------------------------------------------------------------
// SetArea
//-----------------------------------------------------------------------------
TRect CGlxUiUtilityCoeControl::SetArea( 
	AknLayoutUtils::TAknLayoutMetrics aArea )
	{
	TRect rect;
    // remember the area name for the orientation changes
    iArea = aArea;
    // ask for the rect of the named area now
    AknLayoutUtils::LayoutMetricsRect( iArea, rect ); 
	// set the new rect
    SetRect( rect ); 
    // return the rect
    return rect;
	}

//-----------------------------------------------------------------------------
// C++ Constructor
//-----------------------------------------------------------------------------
CGlxUiUtilityCoeControl::CGlxUiUtilityCoeControl( CHuiEnv& aEnv )
	: CHuiDisplayCoeControl( aEnv )
    {
    }

//-----------------------------------------------------------------------------
// ConstructL
//-----------------------------------------------------------------------------
void CGlxUiUtilityCoeControl::ConstructL(
	AknLayoutUtils::TAknLayoutMetrics aArea, TInt aFlags )
    {
    // set the area, as a side product we get the rect
    TRect rect = SetArea( aArea );
	// and construct the display with rect and flags
    CHuiDisplayCoeControl::ConstructL( rect, aFlags );
    iAppUi = CCoeEnv::Static()->AppUi();
    }

//-----------------------------------------------------------------------------
// HandleResourceChange
//-----------------------------------------------------------------------------
void CGlxUiUtilityCoeControl::HandleResourceChange( TInt aType )
    {
    // Call base class method
    CCoeControl::HandleResourceChange(aType);
    
	// Was it orientation change
   	if( KEikDynamicLayoutVariantSwitch == aType )
        {
        // re-set the area, it sets the rect
        SetArea( iArea );
		// Notify skin to change size
		TRAP_IGNORE( Env().Skin().NotifyDisplaySizeChangedL() ); 
        }
    }

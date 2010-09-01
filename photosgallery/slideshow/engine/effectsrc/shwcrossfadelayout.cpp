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
#include "shwcrossfadelayout.h"

#include <glxlog.h>
#include <glxtracer.h>

#include "shwconstants.h"

using namespace NShwSlideshow;

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
TShwCrossFadeLayout::TShwCrossFadeLayout()
	{
	TRACER("TShwCrossFadeLayout::TShwCrossFadeLayout");
	GLX_LOG_INFO("TShwCrossFadeLayout::TShwCrossFadeLayout");
	__DEBUG_ONLY( _iName = _L("TShwCrossfadeLayout") );
	// default values
	// style is linear
	iOpacity.SetStyle( EAlfTimedValueStyleLinear );
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
TShwCrossFadeLayout::~TShwCrossFadeLayout()
	{
	TRACER("TShwCrossFadeLayout::~TShwCrossFadeLayout");
	GLX_LOG_INFO("TShwCrossFadeLayout::~TShwCrossFadeLayout");
	// Do nothing
	}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------
void TShwCrossFadeLayout::Set( TReal32 aOpacity, TInt aDuration )
	{
	TRACER("TShwCrossFadeLayout::Set");
	GLX_LOG_INFO("TShwCrossFadeLayout::Set");
	iOpacity.Set( aOpacity, aDuration );
	}

// -----------------------------------------------------------------------------
// Pause
// -----------------------------------------------------------------------------
void TShwCrossFadeLayout::Pause()
	{
	TRACER("TShwCrossFadeLayout::Pause");
	GLX_LOG_INFO("TShwCrossFadeLayout::Pause");
	iOpacity.Pause();
	}

// -----------------------------------------------------------------------------
// Resume
// -----------------------------------------------------------------------------
void TShwCrossFadeLayout::Resume()
	{
	TRACER("TShwCrossFadeLayout::Resume");
	GLX_LOG_INFO("TShwCrossFadeLayout::Resume");
	iOpacity.Resume();
	}

// -----------------------------------------------------------------------------
// DoSetLayoutValues
// -----------------------------------------------------------------------------
void TShwCrossFadeLayout::DoSetLayoutValues( TGlxLayoutInfo& /*aInfo*/ )
	{
	TRACER("TShwCrossFadeLayout::DoSetLayoutValues");
	GLX_LOG_INFO("TShwCrossFadeLayout::DoSetLayoutValues");
	// get new opacity
	TReal32 opacity = iOpacity.Now();

	// ensure value is inside maximum and minimum opacity
    opacity = Min( opacity, KMaxOpacity );
    opacity = Max( opacity, KMinOpacity );
	}

// -----------------------------------------------------------------------------
// DoChanged
// -----------------------------------------------------------------------------
TBool TShwCrossFadeLayout::DoChanged() const
	{
	TRACER("TShwCrossFadeLayout::DoChanged");
	GLX_LOG_INFO("TShwCrossFadeLayout::DoChanged");
	return iOpacity.Changed();
	}
	
// -----------------------------------------------------------------------------
// DoClearChanged
// -----------------------------------------------------------------------------
void TShwCrossFadeLayout::DoClearChanged() 
	{
	TRACER("TShwCrossFadeLayout::DoClearChanged");
	GLX_LOG_INFO("TShwCrossFadeLayout::DoClearChanged");
	iOpacity.ClearChanged();
	}

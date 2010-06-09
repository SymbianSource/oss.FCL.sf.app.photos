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
* Description:   The slideshow engine API
 *
*/


#include <glxtracer.h>
#include <glxlog.h>

// CLASS HEADER
#include "shwslideshowengine.h"

// INTERNAL INCLUDES
#include "shwslideshowengineimpl.h"

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwSlideshowEngine::CShwSlideshowEngine()
    {
    }

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
EXPORT_C CShwSlideshowEngine* CShwSlideshowEngine::NewL( 
	MShwEngineObserver& aObserver )
    {
    TRACER("CShwSlideshowEngine::NewL");
    GLX_LOG_INFO("CShwSlideshowEngine::NewL");
    CShwSlideshowEngine* self = new (ELeave) CShwSlideshowEngine;
    CleanupStack::PushL( self );
    // construct the real implementation
    self->iImpl = CShwSlideshowEngineImpl::NewL( aObserver);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
EXPORT_C CShwSlideshowEngine::~CShwSlideshowEngine()
    {
    TRACER("CShwSlideshowEngine::~CShwSlideshowEngine");
    GLX_LOG_INFO("CShwSlideshowEngine::~CShwSlideshowEngine");
    delete iImpl;
    }


// -----------------------------------------------------------------------------
// StartL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::StartL( 
	CAlfEnv& aEnv, CAlfDisplay& aDisplay, MGlxMediaList& aItemsToShow,
	MShwMusicObserver& aMusicObserver, TSize aScreenSize )
	{
	TRACER("CShwSlideshowEngine::StartL");
	GLX_LOG_INFO("CShwSlideshowEngine::StartL");
	this->iImpl->StartL( 
	    aEnv, aDisplay, aItemsToShow, aMusicObserver, aScreenSize );
	}


// -----------------------------------------------------------------------------
// NextItemL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::NextItemL()
	{
	TRACER("CShwSlideshowEngine::NextItemL");
	GLX_LOG_INFO("CShwSlideshowEngine::NextItemL");
	this->iImpl->NextItemL();
	}

// -----------------------------------------------------------------------------
// PreviousItemL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::PreviousItemL()
	{
	TRACER("CShwSlideshowEngine::PreviousItemL");
	GLX_LOG_INFO("CShwSlideshowEngine::PreviousItemL");
	this->iImpl->PreviousItemL();
	}

// -----------------------------------------------------------------------------
// PauseL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::PauseL()
	{
	TRACER("CShwSlideshowEngine::PauseL");
	GLX_LOG_INFO("CShwSlideshowEngine::PauseL");
	this->iImpl->PauseL();
	}

// -----------------------------------------------------------------------------
// ResumeL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::ResumeL()
	{
	TRACER("CShwSlideshowEngine::ResumeL");
	GLX_LOG_INFO("CShwSlideshowEngine::ResumeL");
	this->iImpl->ResumeL();
	}

// -----------------------------------------------------------------------------
// State.
// -----------------------------------------------------------------------------
EXPORT_C CShwSlideshowEngine::TShwState CShwSlideshowEngine::State() const
	{
	TRACER("CShwSlideshowEngine::TShwState CShwSlideshowEngine::State()");
	GLX_LOG_INFO("CShwSlideshowEngine::TShwState CShwSlideshowEngine::State()");
	return this->iImpl->State();
	}

// -----------------------------------------------------------------------------
// AvailableEffectsL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::AvailableEffectsL(
    RArray<TShwEffectInfo>& aEffects )
	{
	TRACER("CShwSlideshowEngine::AvailableEffectsL");
	GLX_LOG_INFO("CShwSlideshowEngine::AvailableEffectsL");
	CShwSlideshowEngineImpl::AvailableEffectsL( aEffects );
	}	

// -----------------------------------------------------------------------------
// ResumeL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::VolumeUpL()
	{
	TRACER("CShwSlideshowEngine::VolumeUpL");
	GLX_LOG_INFO("CShwSlideshowEngine::VolumeUpL");
	this->iImpl->VolumeUpL();
	}

// -----------------------------------------------------------------------------
// VolumeDownL.
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::VolumeDownL()
	{
	TRACER("CShwSlideshowEngine::VolumeDownL");
	GLX_LOG_INFO("CShwSlideshowEngine::VolumeDownL");
	this->iImpl->VolumeDownL();
	}

// -----------------------------------------------------------------------------
// GetMusicVolumeL
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::GetMusicVolumeL()
    {
    TRACER("CShwSlideshowEngine::GetMusicVolumeL");
    this->iImpl->GetMusicVolumeL();
    }

// -----------------------------------------------------------------------------
// ToggleControlsVisibiltyL. LSKPressed
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::ToggleUiControlsVisibiltyL()
    {
    TRACER("CShwSlideshowEngine::ToggleUiControlsVisibiltyL");
    GLX_LOG_INFO("CShwSlideshowEngine::ToggleUiControlsVisibiltyL");
    this->iImpl->ToggleUiControlsVisibiltyL();
    }
// -----------------------------------------------------------------------------
// LSKPressedL
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::LSKPressedL()
    {
    TRACER("CShwSlideshowEngine::LSKPressedL");
    GLX_LOG_INFO("CShwSlideshowEngine::LSKPressedL");
    this->iImpl->LSKPressedL();
    }

// -----------------------------------------------------------------------------
// HandleHDMIDecodingEvent
// -----------------------------------------------------------------------------
EXPORT_C void CShwSlideshowEngine::HandleHDMIDecodingEventL(
        THdmiDecodingStatus aStatus)
    {
    TRACER("CShwSlideshowEngine::HandleHDMIDecodingEventL");
    this->iImpl->HandleHDMIDecodingEventL(aStatus);
    }


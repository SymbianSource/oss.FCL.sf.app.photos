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
* Description:   Header file for crossfade effect
 *
*/




//  Include Files
#include "shwcrossfadeeffect.h"

#include <glxsetvaluelayout.h>
#include <glxlog.h>
#include <glxtracer.h>

#include "shwcrossfadelayout.h"
#include "shwresourceutility.h"
#include "shwconstants.h"

using namespace NShwSlideshow;
	
/**
 * CShwCrossFadeEffectImpl
 * Crossfade dependencies
 * This class makes our clients indepandant of the effect implementation
 * This gives greater flexibitily in testing as the test suites can just
 * re-implement all the methods in the real class interface without the 
 * need to stub all our dependencies. 
 * If for example TShwCrossFadeLayout was a direct member of
 * CShwCrossFadeEffect class, all the clients would need to have access 
 * to TShwCrossFadeLayout constructor and destructor and the test suite
 * would need to either create a stub implementation for it or include
 * the real class in the test (and all its dependencies).
 *
 * There is however no point of duplicating the whole CShwCrossFadeEffect
 * interface as it would mean double maintenance
 * so we use the iImpl pointer when referencing the class members
 */
NONSHARABLE_CLASS( CShwCrossFadeEffect::CShwCrossFadeEffectImpl )
	: public CBase
	{
	public:
		
		/**
		 * Constructor
		 */
		CShwCrossFadeEffectImpl();

		/**
		 * Destructor
		 */
		~CShwCrossFadeEffectImpl();

	public:

        /// Own: the size of the screen
        TSize iScreenSize;

		/// Own: the layout objects
		TGlxSetValueLayout iSizeLayout;
		TShwCrossFadeLayout iOpacityLayout;

		/// Own: the effect info.
		TShwEffectInfo iEffectInfo;
		
		/// Own: the effect's name
		HBufC*	iEffectName;
		
	};

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwCrossFadeEffect::CShwCrossFadeEffectImpl::CShwCrossFadeEffectImpl()
//	: iSizeLayout( TGlxLayoutInfo::ESize )
	{
	TRACER("CShwCrossFadeEffect::CShwCrossFadeEffectImpl::CShwCrossFadeEffectImpl()");
	GLX_LOG_INFO("CShwCrossFadeEffect::CShwCrossFadeEffectImpl::CShwCrossFadeEffectImpl()");
	// set layout chains, opacity -> size
	iOpacityLayout.SetNext( &iSizeLayout );
	iSizeLayout.SetNext( NULL );

	// define initial values
	iOpacityLayout.Set( KMinOpacity );	// initial value 0%
	}

// -----------------------------------------------------------------------------
// Destructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwCrossFadeEffect::CShwCrossFadeEffectImpl::~CShwCrossFadeEffectImpl()
	{
	TRACER("CShwCrossFadeEffect::CShwCrossFadeEffectImpl::~CShwCrossFadeEffectImpl()");
	GLX_LOG_INFO("CShwCrossFadeEffect::CShwCrossFadeEffectImpl::~CShwCrossFadeEffectImpl()");
	delete iEffectName;
	}

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwCrossFadeEffect::CShwCrossFadeEffect()
	{
	}

// -----------------------------------------------------------------------------
// NewLC. Static construction
// -----------------------------------------------------------------------------
CShwCrossFadeEffect* CShwCrossFadeEffect::NewLC()
	{
	TRACER("CShwCrossFadeEffect::NewLC");
	GLX_LOG_INFO("CShwCrossFadeEffect::NewLC");
	CShwCrossFadeEffect* self = new (ELeave) CShwCrossFadeEffect;
	CleanupStack::PushL( self );

	// create implementation
	self->iImpl = new( ELeave ) CShwCrossFadeEffectImpl;

    // Note that only one name is defined for the cross fade so can only
    // distinguish between normal and zoom effects via the uid
	self->iImpl->iEffectName = 
		ShwResourceUtility::LocalisedNameL( R_SHW_EFFECT_CROSS_FADE );
	if( !self->iImpl->iEffectName )
		{
		// Have to use a non-localised version
		self->iImpl->iEffectInfo.iName = KEffectNameCrossFade;
		}
	else
		{
		// set the localised version
		self->iImpl->iEffectInfo.iName = *self->iImpl->iEffectName;
		}

    // info - uid
	self->iImpl->iEffectInfo.iId.iPluginUid = KDefaultEffectPluginUid;
	self->iImpl->iEffectInfo.iId.iIndex 	= KEffectUidXFadeNormal;

	return self;
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwCrossFadeEffect::~CShwCrossFadeEffect()
	{
	TRACER("CShwCrossFadeEffect::~CShwCrossFadeEffect");
	GLX_LOG_INFO("CShwCrossFadeEffect::~CShwCrossFadeEffect");
	delete iImpl;
	}

// -----------------------------------------------------------------------------
// CloneLC
// -----------------------------------------------------------------------------
MShwEffect* CShwCrossFadeEffect::CloneLC()
	{
	TRACER("CShwCrossFadeEffect::CloneLC");
	GLX_LOG_INFO( "CShwCrossFadeEffect::CloneLC" );
	// create a copy of the effect
	CShwCrossFadeEffect* copy = CShwCrossFadeEffect::NewLC();
	// transfer the member variables, only size has relevant value
	/*copy->iImpl->iSizeLayout.SetValue( 
		iImpl->iSizeLayout.TimedPoint().iX.Now(), 
		iImpl->iSizeLayout.TimedPoint().iY.Now(),
		0 );*/
	// copy the screen size
	copy->iImpl->iScreenSize = iImpl->iScreenSize;
	return copy;
	}

// -----------------------------------------------------------------------------
// InitializeL
// -----------------------------------------------------------------------------
void CShwCrossFadeEffect::InitializeL( 
	CAlfEnv* /*aHuiEnv*/, MGlxVisualList* /*aVisualList*/,
    MGlxMediaList* /*aMediaList*/, TSize aScreenSize )
	{
	TRACER("CShwCrossFadeEffect::InitializeL");
	GLX_LOG_INFO( "CShwCrossFadeEffect::InitializeL" );
	// set the screen size
	iImpl->iScreenSize = aScreenSize;
	}

// -----------------------------------------------------------------------------
// PrepareViewL
// -----------------------------------------------------------------------------
TSize CShwCrossFadeEffect::PrepareViewL( 
	CAlfVisual* /*aVisual*/, TSize /*aImageSize*/ )
	{
	TRACER("CShwCrossFadeEffect::PrepareViewL");
	GLX_LOG_INFO( "CShwCrossFadeEffect::PrepareViewL" );
	// visual list takes care of fitting the image and maintains aspect ratio
	// return the screen size as the thumbnail size
	return iImpl->iScreenSize;
	}

// -----------------------------------------------------------------------------
// EnterViewL.
// -----------------------------------------------------------------------------
MGlxLayout* CShwCrossFadeEffect::EnterViewL(
	CAlfVisual* aVisual/**/, TInt /*aDuration*/, TInt aFadeInDuration )
	{
	TRACER("CShwCrossFadeEffect::EnterViewL");
	GLX_LOG_INFO1(
		"CShwCrossFadeEffect::EnterViewL( %d )", aFadeInDuration );
	TAlfTimedValue initialValue(0.0,0);
	aVisual->SetOpacity(initialValue);
	TAlfTimedValue tranition(1.0,aFadeInDuration);
	tranition.SetStyle(EAlfTimedValueStyleLinear);
	aVisual->SetOpacity(tranition);
	return &iImpl->iOpacityLayout;
	}

// -----------------------------------------------------------------------------
// ExitView.
// -----------------------------------------------------------------------------
void CShwCrossFadeEffect::ExitView( CAlfVisual* /*aVisual*/ )
	{
	TRACER("CShwCrossFadeEffect::ExitView");
	GLX_LOG_INFO( "CShwCrossFadeEffect::ExitView" );
	}

// -----------------------------------------------------------------------------
// EnterTransitionL.
// -----------------------------------------------------------------------------
MGlxLayout* CShwCrossFadeEffect::EnterTransitionL(
	CAlfVisual* /**/aVisual, TInt aDuration )
	{
	TRACER("CShwCrossFadeEffect::EnterTransitionL");
	GLX_LOG_INFO1( "CShwCrossFadeEffect::EnterTransitionL( %d )", aDuration );

	TAlfTimedValue tranition(0.0,aDuration);
	tranition.SetStyle(EAlfTimedValueStyleLinear);
	aVisual->SetOpacity(tranition);
	return &iImpl->iOpacityLayout;
	}

// -----------------------------------------------------------------------------
// ExitTransition.
// -----------------------------------------------------------------------------
void CShwCrossFadeEffect::ExitTransition( CAlfVisual* /*aVisual*/ )
	{
	TRACER("CShwCrossFadeEffect::ExitTransition");
	GLX_LOG_INFO( "CShwCrossFadeEffect::ExitTransition" );
	}

// -----------------------------------------------------------------------------
// PauseL.
// -----------------------------------------------------------------------------
void CShwCrossFadeEffect::PauseL()
	{
	TRACER("CShwCrossFadeEffect::PauseL");
	GLX_LOG_INFO( "CShwCrossFadeEffect::PauseL()");

	// pause the opacity, size does not change so no need to pause
	iImpl->iOpacityLayout.Pause();
	}

// -----------------------------------------------------------------------------
// Resume.
// -----------------------------------------------------------------------------
void CShwCrossFadeEffect::Resume()
	{
	TRACER("CShwCrossFadeEffect::Resume");
	GLX_LOG_INFO( "CShwCrossFadeEffect::Resume()");

	// resume the opacity layout, size does not change so no need to resume
	iImpl->iOpacityLayout.Resume();
	}

// -----------------------------------------------------------------------------
// EffectInfo.
// -----------------------------------------------------------------------------
TShwEffectInfo CShwCrossFadeEffect::EffectInfo()
	{
	TRACER("CShwCrossFadeEffect::EffectInfo");
	GLX_LOG_INFO( "CShwCrossFadeEffect::EffectInfo()");
	return iImpl->iEffectInfo;
	}  

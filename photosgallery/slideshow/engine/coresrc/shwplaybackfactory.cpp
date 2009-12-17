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
* Description:    The playback factory, configures the playback for the slideshow
 *
*/



//  CLASS HEADER
#include "shwplaybackfactory.h"

//  EXTERNAL INCLUDES
#include <mglxvisuallist.h>
#include <ecom/implementationinformation.h>
#include <ecom/ecom.h>

//  INTERNAL INCLUDES
#include <glxlog.h>
#include <glxtracer.h>

#include "shwtimercontrol.h"
#include "shwviewcontrol.h"
#include "shweffectcontrol.h"
#include "shwdefaulteffectmanager.h"
#include "shwcrossfadeeffect.h"
#include "shwzoomandpaneffect.h"
#include "shwmusiccontrol.h"
#include "shwmusicobserver.h"
#include "shwsettingsmodel.h"
#include "shweffectinfo.h"
#include "shwcleanupwrapper.h"

using namespace NShwSlideshow;

// -----------------------------------------------------------------------------
// Local definitions
// -----------------------------------------------------------------------------
//
namespace
	{
	// granularity for observer array, typical count
	const TInt KObserverCount = 8;
	// granularity for publisher array, typical count
	const TInt KPublisherCount = 8;
	}

// -----------------------------------------------------------------------------
// The implementation class
// -----------------------------------------------------------------------------
//
NONSHARABLE_CLASS( CShwPlaybackFactory::CShwPlaybackFactoryImpl )
	: public CBase
	{
	public:

		/**
		 * Constructors.
		 */
		CShwPlaybackFactoryImpl(MShwMusicObserver& aMusicObserver);

		/**
		 * Destructors.
		 */
		~CShwPlaybackFactoryImpl();

		/**
		 * 2nd stage constructor.
		 * @param aHuiEnv the HUI environment
		 * @param aVisualList the visual list of the show
		 * @param aMediaList  the media list of the show
		 * @param aMusicObserver the music control observer
		 * @param aScreenSize the screen size to use
		 */
		void ConstructL(
			CAlfEnv* aAlfEnv, 
			MGlxVisualList* aVisualList,
			MGlxMediaList* aMediaList, 
			TSize aScreenSize );

	public:

		/// @ref CShwPlaybackFactory::EventObservers
		RPointerArray< MShwEventObserver > EventObservers();
		/// @ref CShwPlaybackFactory::EventPublishers
		RPointerArray< MShwEventPublisher > EventPublishers();
		/// @ref CShwSlideshowEngine::AvailableEffectsL
		// @returns ownership of array of effects
		static void CreateEffectsL( RPointerArray<MShwEffect>& aEffects );

	private:

		/// Own: Effect manager
		CShwDefaultEffectManager* iEffectManager;
		/// Own: Timer control
		CShwTimerControl* iTimerControl;
		/// Own: Music control
		CShwMusicControl* iMusicControl;
		/// Own: Effect control
		CShwEffectControl* iEffectControl;
		/// Own: View control
		CShwViewControl* iViewControl;
		/// Own: the observers
		RPointerArray< MShwEventObserver > iObservers;
		/// Own: the publishers
		RPointerArray< MShwEventPublisher > iPublishers;

		/// Ref: the visual list
		MGlxVisualList* iVisualList;
		/// Ref: the music control observer
		MShwMusicObserver& iMusicObserver;
		
	};

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwPlaybackFactory::CShwPlaybackFactory() 
	{
	// No implementation needed
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwPlaybackFactory* CShwPlaybackFactory::NewL(
	CAlfEnv* aAlfEnv,
	MGlxVisualList* aVisualList,
	MGlxMediaList* aMediaList,
	MShwMusicObserver& aMusicObserver, 
	TSize aScreenSize )
	{
	TRACER("CShwPlaybackFactory::NewL");
	GLX_LOG_INFO( "CShwPlaybackFactory::NewL" );

	CShwPlaybackFactory* self = new( ELeave ) CShwPlaybackFactory( );
	CleanupStack::PushL( self );

	// create the implementation, forward the parameters
	self->iImpl = new( ELeave ) CShwPlaybackFactoryImpl (aMusicObserver);
	self->iImpl->ConstructL( aAlfEnv, aVisualList, aMediaList, aScreenSize );
	
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwPlaybackFactory::~CShwPlaybackFactory()
	{
	TRACER("CShwPlaybackFactory::~CShwPlaybackFactory");
	GLX_LOG_INFO("CShwPlaybackFactory::~CShwPlaybackFactory");
	delete iImpl;
	}

// -----------------------------------------------------------------------------
// EventObservers.
// -----------------------------------------------------------------------------
RPointerArray< MShwEventObserver > CShwPlaybackFactory::EventObservers()
	{
	TRACER("CShwPlaybackFactory::EventObservers");
	GLX_LOG_INFO("CShwPlaybackFactory::EventObservers");
	return iImpl->EventObservers();
	}

// -----------------------------------------------------------------------------
// EventPublishers.
// -----------------------------------------------------------------------------
RPointerArray< MShwEventPublisher > CShwPlaybackFactory::EventPublishers()
	{
	TRACER("CShwPlaybackFactory::EventPublishers");
	GLX_LOG_INFO("CShwPlaybackFactory::EventPublishers");
	return iImpl->EventPublishers();
	}

//------------------------------------------------------------------------------
// AvailableEffectsL
//------------------------------------------------------------------------------
void CShwPlaybackFactory::AvailableEffectsL( RArray<TShwEffectInfo>& 
																aEffectInfos )
    {
    TRACER("CShwPlaybackFactory::AvailableEffectsL");
    GLX_LOG_INFO("CShwPlaybackFactory::AvailableEffectsL");
    aEffectInfos.Reset();
    RPointerArray<MShwEffect> effects;
    // put the array in cleanupstack in case effect construction leaves
	ShwCleanupResetAndDestroyPushL( effects );
    CShwPlaybackFactoryImpl::CreateEffectsL( effects );
    TInt i = effects.Count();
    while (i-- > 0)
        {
        // get the effect info and append it to the given array
        aEffectInfos.AppendL( effects[i]->EffectInfo() );
        }
	// ownership transferred, remove array from stack
	CleanupStack::PopAndDestroy( &effects );
    }

//------------------------------------------------------------------------------
// CShwPlaybackFactoryImpl
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwPlaybackFactory::CShwPlaybackFactoryImpl::CShwPlaybackFactoryImpl(
												MShwMusicObserver& aMusicObserver)
						   : iObservers( KObserverCount ), 
						   	 iPublishers( KPublisherCount ),
						   	 iMusicObserver (aMusicObserver)
	{
	TRACER("CShwPlaybackFactory::CShwPlaybackFactoryImpl::CShwPlaybackFactoryImpl");
	GLX_LOG_INFO("CShwPlaybackFactory::CShwPlaybackFactoryImpl::CShwPlaybackFactoryImpl");
	}

// -----------------------------------------------------------------------------
// Destructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwPlaybackFactory::CShwPlaybackFactoryImpl::~CShwPlaybackFactoryImpl()
	{
	TRACER("CShwPlaybackFactory::CShwPlaybackFactoryImpl::~CShwPlaybackFactoryImpl");
	GLX_LOG_INFO("CShwPlaybackFactory::CShwPlaybackFactoryImpl::~CShwPlaybackFactoryImpl");

	delete iEffectManager;
	delete iTimerControl;
	delete iMusicControl;
	delete iEffectControl;
	delete iViewControl;

	iObservers.Close();
	iPublishers.Close();
	}

//------------------------------------------------------------------------------
// CShwPlaybackFactoryImpl::ConstructL
//------------------------------------------------------------------------------
void CShwPlaybackFactory::CShwPlaybackFactoryImpl::ConstructL(
	CAlfEnv* aAlfEnv, 
	MGlxVisualList* aVisualList, 
	MGlxMediaList* aMediaList, 
	TSize aScreenSize )
	{
	TRACER("CShwPlaybackFactory::CShwPlaybackFactoryImpl::ConstructL");
	GLX_LOG_INFO( "CShwPlaybackFactoryImpl::ConstructL" );

	// create the effect manager
	iEffectManager = CShwDefaultEffectManager::NewL();

	// create array for effects
    RPointerArray< MShwEffect > effects;
    // put the array in cleanupstack in case CreateEffectsL leaves
	ShwCleanupResetAndDestroyPushL( effects );
	// create the effects
    CShwPlaybackFactoryImpl::CreateEffectsL( effects );
    // need to go from last to first as we remove items
    TInt i = effects.Count();
    while (i-- > 0)
        {
		effects[ i ]->InitializeL( 
		    aAlfEnv, aVisualList, aMediaList,
		    aScreenSize );
		
		// give the effect to the manager, it takes ownership
		iEffectManager->AddEffectL( effects[ i ] );
		// remove the effect from original array to prevent
		// double delete, effect manager takes care of the effect from now on
		effects.Remove( i );
        }
	// ownership transferred, remove array from stack
	CleanupStack::Pop( &effects );
	// need to also close the array in this case, this frees the array memory
	effects.Close();

	// create the model to retrieve the persisted slideshow settings. 
	CShwSettingsModel* shwSettingsMdl = CShwSettingsModel::NewL();
	CleanupStack::PushL( shwSettingsMdl );

	// retrieve and set the persisted effect settings
	TShwEffectInfo effect;
	shwSettingsMdl->TransitionTypeL( 
	    effect.iId.iPluginUid, effect.iId.iIndex );
	iEffectManager->SetDefaultEffectL( effect );

	// create the timer
	iTimerControl = CShwTimerControl::NewL();
	iObservers.AppendL( iTimerControl );
	iPublishers.AppendL( iTimerControl );

	// create effect control
	iEffectControl =
		CShwEffectControl::NewL(
		    *iEffectManager, *aVisualList, *aMediaList, aScreenSize );
	iObservers.AppendL( iEffectControl );
	iPublishers.AppendL( iEffectControl );

	// create view manager, view duration is stored in seconds so need to
	// multiply with 1000
	TInt viewDuration = shwSettingsMdl->TransDelayL() * 1000;
	iViewControl = 
		CShwViewControl::NewL( 
		    *aMediaList, KDefaultTransitionDuration, viewDuration );
	iObservers.AppendL( iViewControl );
	iPublishers.AppendL( iViewControl );

    // create music control last as it is the least important
	if (shwSettingsMdl->MusicOnL() )
		{
		// retrieve the name of the music file
		TFileName fileName;
		shwSettingsMdl->MusicNamePathL( fileName );
		// instantiate and initialise the music player
		iMusicControl = CShwMusicControl::NewL(iMusicObserver, fileName );
		iObservers.AppendL( iMusicControl );
		iPublishers.AppendL( iMusicControl );
		}
	
	CleanupStack::PopAndDestroy( shwSettingsMdl );
	GLX_LOG_INFO( "CShwPlaybackFactoryImpl::ConstructL ended" );
	}

//------------------------------------------------------------------------------
// CShwPlaybackFactoryImpl::EventObservers
//------------------------------------------------------------------------------
RPointerArray<MShwEventObserver> 
		CShwPlaybackFactory::CShwPlaybackFactoryImpl::EventObservers()
	{
	return iObservers;
	}

//------------------------------------------------------------------------------
// CShwPlaybackFactoryImpl::EventPublishers
//------------------------------------------------------------------------------
RPointerArray<MShwEventPublisher> 
		CShwPlaybackFactory::CShwPlaybackFactoryImpl::EventPublishers()
	{
	TRACER("CShwPlaybackFactory::CShwPlaybackFactoryImpl::EventPublishers");
	GLX_LOG_INFO( "CShwPlaybackFactoryImpl::EventPublishers" );
	return iPublishers;
	}

//------------------------------------------------------------------------------
// CShwPlaybackFactoryImpl::CreateEffectsL
//------------------------------------------------------------------------------
void CShwPlaybackFactory::CShwPlaybackFactoryImpl::CreateEffectsL(
											RPointerArray<MShwEffect>& aEffects )
    {
    TRACER("CShwPlaybackFactory::CShwPlaybackFactoryImpl::CreateEffectsL");
	GLX_LOG_INFO( "CShwPlaybackFactoryImpl::CreateEffectsL" );
	aEffects.AppendL( CShwZoomAndPanEffect::NewLC() );
	CleanupStack::Pop();
	aEffects.AppendL( CShwCrossFadeEffect::NewLC() );
	CleanupStack::Pop();
    }

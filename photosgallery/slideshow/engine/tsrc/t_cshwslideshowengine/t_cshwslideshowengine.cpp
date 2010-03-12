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
* Description:   Test for slideshow engine
 *
*/



//  CLASS HEADER
#include "t_cshwslideshowengine.h"

//	CLASS UNDER TEST
#include "shwslideshowengineimpl.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/EUnitMacros.h>
#include <digia/eunit/EUnitDecorators.h>
#include <uiacceltk/huiEnv.h>
#include <uiacceltk/huiDisplay.h>

//  INTERNAL INCLUDES
#include "shwslideshowenginepanic.h"
#include "shwhuiutility.h"

#include "shwtimercontrol.h"
#include "shwmusiccontrol.h"
#include "shwviewcontrol.h"
#include "shweffectcontrol.h"
#include "shweffect.h"
#include "shwevent.h"

//	STUBS
#include "tmglxvisuallist_adapter.h"
#include "stub_glxfetchcontexts.h"
#include "stub_mshweffect.h"

class T_Scheduler : public CActiveScheduler
	{
	public:
		// replace the default scheduler behaviour (which is to panic on error)
		void Error( TInt aError ) const
			{
			// we got an error, leave to previous TRAP (that is in alloc decorator)
			User::Leave( aError );
			}
	};

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

// -----------------------------------------------------------------------------
// Stub for CGlxVisualListManager -->
// -----------------------------------------------------------------------------
MGlxVisualList_Observer* gVisualListObserver;
TBool gVisualListWasAllocated = EFalse;
TBool gVisualListWasReleased = EFalse;
TBool gVisualListManagerWasDeleted = EFalse;
TInt gVisualListInitialFocus = 2;

class CGlxVisualListManager : public CBase
	{
	public:
		CGlxVisualListManager();
		static CGlxVisualListManager* ManagerL();
		~CGlxVisualListManager();
		MGlxVisualList* AllocListL(
			MGlxMediaList* aItemList, 
    		CHuiEnv& aEnv, CHuiDisplay& aDisplay,
    		CHuiImageVisual::TScaleMode aThumbnailScaleMode = CHuiImageVisual::EScaleFitInside );
		void ReleaseList( MGlxVisualList* aList );
		void Close();
	private:
		TMGlxVisualList_Adapter iList;
	};
CGlxVisualListManager::CGlxVisualListManager()
	: iList( gVisualListObserver )
	{
	}
CGlxVisualListManager::~CGlxVisualListManager()
	{
	gVisualListManagerWasDeleted = ETrue;
	}
CGlxVisualListManager* CGlxVisualListManager::ManagerL()
	{
	return new( ELeave ) CGlxVisualListManager;
	}
MGlxVisualList* CGlxVisualListManager::AllocListL(
	MGlxMediaList* , 
	CHuiEnv& , 
	CHuiDisplay& ,
	CHuiImageVisual::TScaleMode  )
	{
	gVisualListWasAllocated = ETrue;
	// set the size and initial focus of the visual list
	iList.iFocus = gVisualListInitialFocus;
	EUNIT_PRINT( _L("Visual list initial focus %d"), iList.iFocus );
	// this is just long enough not to complicate the test 
	iList.iSize = 100;
	return &iList;
	}
void CGlxVisualListManager::ReleaseList( MGlxVisualList* /*aList*/ )
	{
	gVisualListWasReleased = ETrue;
	}
void CGlxVisualListManager::Close()
	{
	delete this;
	}

// -----------------------------------------------------------------------------
// <-- Stub for CGlxVisualListManager
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for CShwPlaybackFactory -->
// -----------------------------------------------------------------------------
#include "shwplaybackfactory.h"
TBool gPlaybackFactoryWasDeleted = EFalse;

// static arrays for the observers and publishers
RPointerArray< MShwEventObserver > gEventObservers;
RPointerArray< MShwEventPublisher > gEventPublishers;

// array of available effects info
RArray<TShwEffectInfo> gEffects;

MShwEffectManager* gEffectManager = NULL;
CShwTimerControl* gTimerControl = NULL;
CShwMusicControl* gMusicControl = NULL;
CShwEffectControl* gEffectControl = NULL;
CShwViewControl* gViewControl = NULL;
TBool gIncludeMusicControl = EFalse;	// by default no music
TBool gSetRealSong = ETrue;    // by default set real song

namespace
	{
	// NOTE these values work with the current test configuration
	// if you change these some async tests may fail
	// default transition duration, milliseconds
	const TInt KDefaultTransitionDuration = 100;
	// default view duration, milliseconds
	const TInt KDefaultViewDuration = 150;
	}

CShwPlaybackFactory* CShwPlaybackFactory::NewL(
	CHuiEnv* /*aHuiEnv*/, 
	MGlxVisualList* aVisualList, 
	MGlxMediaList* aMediaList,
	MShwMusicObserver& aMusicObserver,
	TSize aScreenSize )
	{
	CShwPlaybackFactory* self = new( ELeave ) CShwPlaybackFactory;
	CleanupStack::PushL( self );
	
	// create the event classes
	// create the timer
	gTimerControl = CShwTimerControl::NewL();
	gEventObservers.AppendL( gTimerControl );
	gEventPublishers.AppendL( gTimerControl );

	// include music control only if wanted
	if( gIncludeMusicControl )
		{
		// create the music control
		_LIT(KTempFileName, "c:\\knightrider.mp3");
		if( gSetRealSong )
		    {
    		gMusicControl = CShwMusicControl::NewL( aMusicObserver, KTempFileName );
		    }
		else
		    {
    		gMusicControl = CShwMusicControl::NewL( aMusicObserver, KNullDesC );
		    }
		gEventObservers.AppendL( gMusicControl );
		gEventPublishers.AppendL( gMusicControl );
		}

	// create effect manager	
	gEffectControl = 
		CShwEffectControl::NewL( 
		    *gEffectManager, *aVisualList, *aMediaList, aScreenSize );
	gEventObservers.AppendL( gEffectControl );
	gEventPublishers.AppendL( gEffectControl );

	// create view manager
	gViewControl = 
		CShwViewControl::NewL(
			*aMediaList, KDefaultTransitionDuration, KDefaultViewDuration );
	gEventObservers.AppendL( gViewControl );
	gEventPublishers.AppendL( gViewControl );

	CleanupStack::Pop( self );
	return self;
	}

CShwPlaybackFactory::~CShwPlaybackFactory()
	{
	gPlaybackFactoryWasDeleted = ETrue;
	// delete the event objects
	delete gTimerControl;
	delete gMusicControl;
	delete gEffectControl;
	delete gViewControl;

	gEventObservers.Close();
	gEventPublishers.Close();
	
	gEffects.Close();
	}

CShwPlaybackFactory::CShwPlaybackFactory()
	{
	gTimerControl = NULL;
	gMusicControl  = NULL;
	gEffectControl = NULL;
	gViewControl = NULL;
	}

RPointerArray< MShwEventObserver > CShwPlaybackFactory::EventObservers()
	{
	return gEventObservers;
	}

RPointerArray< MShwEventPublisher > CShwPlaybackFactory::EventPublishers()
	{
	return gEventPublishers;
	}

void CShwPlaybackFactory::AvailableEffectsL(RArray<TShwEffectInfo>& aEffects)
	{
	aEffects = gEffects;
	}

// -----------------------------------------------------------------------------
// <-- Stub for CShwPlaybackFactory
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for ShwHuiUtility -->
// -----------------------------------------------------------------------------
void ShwHuiUtility::ShowVisualListL(
	CHuiDisplay* /*aDisplay*/, MGlxVisualList* /*aVisualList*/ )
	{
	// do nothing
	}
// -----------------------------------------------------------------------------
// <-- Stub for ShwHuiUtility
// -----------------------------------------------------------------------------

// CONSTRUCTION
T_CShwSlideshowEngine* T_CShwSlideshowEngine::NewL()
	{
	T_CShwSlideshowEngine* self = T_CShwSlideshowEngine::NewLC();
	CleanupStack::Pop();

	return self;
	}

T_CShwSlideshowEngine* T_CShwSlideshowEngine::NewLC()
	{
	T_CShwSlideshowEngine* self = new( ELeave ) T_CShwSlideshowEngine;
	CleanupStack::PushL( self );

	self->ConstructL();

	return self;
	}

// Destructor (virtual by CBase)
T_CShwSlideshowEngine::~T_CShwSlideshowEngine()
	{
	// remove our scheduler
	// replace old with our new
    if ( iScheduler )
        {
        iScheduler->Replace(iOldScheduler);
        }
	delete iScheduler;
	
	delete iTimer;
	}

// Default constructor
T_CShwSlideshowEngine::T_CShwSlideshowEngine()
	{
	}

// Second phase construct
void T_CShwSlideshowEngine::ConstructL()
	{
	// The ConstructL from the base class CEUnitTestSuiteClass must be called.
	// It generates the test case table.
	CEUnitTestSuiteClass::ConstructL();
	
	// remove the old scheduler
	iOldScheduler = CActiveScheduler::Current();
	// create our own scheduler
	iScheduler = new( ELeave ) T_Scheduler;
	// replace old with our new
	iOldScheduler->Replace( iScheduler );
	}

//  METHODS
TBool gObserverEngineStarted = EFalse;
void T_CShwSlideshowEngine::EngineStartedL()
	{
	gObserverEngineStarted = ETrue;
	}
TBool gObserverEnginePaused = EFalse;
void T_CShwSlideshowEngine::EnginePausedL()
	{
	gObserverEnginePaused = ETrue;
	}
TBool gObserverEngineResumed = EFalse;
void T_CShwSlideshowEngine::EngineResumedL()
	{
	gObserverEngineResumed = ETrue;
	}
TBool gObserverEngineError = EFalse;
void T_CShwSlideshowEngine::ErrorDuringSlideshowL()
    {
    gObserverEngineError = ETrue;
    }

void T_CShwSlideshowEngine::MGlxMediaList_MethodCalled( TMGlxMediaListMethodId aMethodId )
	{
	// append the methodid in the array
	TInt error = iMediaListCalls.Append( aMethodId );
	// check that append succeeded
	if( error != KErrNone )
		{
		// critical error, not enough space to append messages
		User::Panic( _L("T_CShwSlideshowEngine::MGlxMediaList_MethodCalled"), __LINE__ );
		}
	}

void T_CShwSlideshowEngine::MGlxVisualList_MethodCalled( TMGlxVisualListMethodId aMethodId )
	{
	// append the methodid in the array
	TInt error = iVisualListCalls.Append( aMethodId );
	// check that append succeeded
	if( error != KErrNone )
		{
		// critical error, not enough space to append messages
		User::Panic( _L("T_CShwSlideshowEngine::MGlxVisualList_MethodCalled"), __LINE__ );
		}
	}

// From MShwEffectManager >>
void T_CShwSlideshowEngine::AddEffectL( MShwEffect* /*aEffect*/ )
	{
	// this is not called in this test
	}

MShwEffect* T_CShwSlideshowEngine::CurrentEffect()
	{
	switch( iCurrentEffect )
		{
		case 0 : return iEffect1;
		case 1 : return iEffect2;
		case 2 : return iEffect3;
		// default falls through
		}
	return NULL;
	}

MShwEffect* T_CShwSlideshowEngine::Effect( TInt aDirection )
	{
	TInt index = iCurrentEffect + aDirection;
	switch( index )
		{
		case -1 : return iEffect3;
		case 0 : return iEffect1;
		case 1 : return iEffect2;
		case 2 : return iEffect3;
		case 3 : return iEffect1;
		// default falls through
		}
	return NULL;
	}
void T_CShwSlideshowEngine::ProceedToEffect( TInt aDirection )
	{
	// inc and modulo with 3 -> values are: 0, 1, 2
	iCurrentEffect += aDirection;
	iCurrentEffect %= 3;
	// if negative, loop back to 2
	if( iCurrentEffect < 0 )
		{
		iCurrentEffect = 2;
		}
	}

void T_CShwSlideshowEngine::SetEffectOrder( MShwEffectManager::TShwEffectOrder /*aOrder*/ )
    {
    }
     	
MShwEffectManager::TShwEffectOrder T_CShwSlideshowEngine::EffectOrder()
    {
    return MShwEffectManager::EEffectOrderProgrammed;
    }
    
void T_CShwSlideshowEngine::SetProgrammedEffects( RArray< TShwEffectInfo >& /*aEffects*/ )
    {
    }

void T_CShwSlideshowEngine::SetDefaultEffectL(TShwEffectInfo /*aInfo*/)
    {
    }
    
void T_CShwSlideshowEngine::GetActiveEffectsL( RPointerArray< MShwEffect >& /*aEffects*/ )
	{
	}

// << from MShwEffectManager
// flag for resetting the thumbnail context request count
TBool gResetThumbnailContextRequestCount = EFalse;
void T_CShwSlideshowEngine::NotifyL( MShwEvent* aEvent )
	{
	// got event so add a copy of it to the list 
	TInt error = iEvents.Append( aEvent->CloneLC() );
	CleanupStack::Pop();
	// check that append succeeded
	if( error != KErrNone )
		{
		// critical error, not enough space to append events
		User::Panic( _L("T_CShwSlideshowEngine::NotifyL"), __LINE__ );
		}

	// if we received init completed we might want to reset the 
	// thumbnail stub to tell that it has panding requests
	if( gResetThumbnailContextRequestCount &&
		dynamic_cast<TShwEventReadyToView*>( aEvent ) )
		{
		gThumbnailContextRequestCount = 1;
		}

	// if we have waited enough events and wait is active
	iEventsReceived++;
	if( iEventsReceived >= iEventsToWait && iSchedulerWait.IsStarted() )
		{
		iSchedulerWait.AsyncStop();
		}
	}
	
TInt T_CShwSlideshowEngine::LoadThumbnailL( TAny* aThis )
	{
	T_CShwSlideshowEngine* self = (T_CShwSlideshowEngine*)aThis;
	// notify engine that thumbnail is ready for index, increase the index
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	self->iStubMedialist->NotifyAttributesAvailableL( self->iThumbnailLoadIndex++ );
	// set the thumbnail request to be not complete
	gThumbnailContextRequestCount = 1;
	return 1;
	}
	
void T_CShwSlideshowEngine::MusicOnL()
	{
	iMusicOn = ETrue;
	}
	
	
void T_CShwSlideshowEngine::MusicOff()
	{
	iMusicOn = EFalse;
	}


void T_CShwSlideshowEngine::MusicVolume(TInt aCurrentVolume, TInt aMaxVolume)
	{
	iCurrentVolume = aCurrentVolume;
	iMaxVolume     = aMaxVolume;

	if(iVolumeWait.IsStarted())
		{
		iVolumeWait.AsyncStop();
		}
	}	


void T_CShwSlideshowEngine::ErrorWithTrackL(TInt /*aErrorCode*/)
	{
	}	
	

// Test methods
void T_CShwSlideshowEngine::EmptyL()
	{
	}

void T_CShwSlideshowEngine::SetupL()
	{
	// reset events received
	iEventsReceived = 0;
	// create thumbnail loader
	iThumbnailLoader = CPeriodic::NewL( 0 ); // default priority
	// create stub list
	iStubMedialist = new( ELeave ) TMGlxMediaList_Stub( this );
	// reset the wait counter
	iEventsToWait = 0;
	iEffect1 = new( ELeave ) T_MShwTestEffect( 1 );
	iEffect2 = new( ELeave ) T_MShwTestEffect( 2 );
	iEffect3 = new( ELeave ) T_MShwTestEffect( 3 );
	// set the test suite as visual list observer
	gVisualListObserver = this;
	// set ourselves as effect manager
	gEffectManager = this;

	// reset state flags
	gObserverEngineStarted = EFalse;
	gObserverEnginePaused = EFalse;
	gObserverEngineResumed = EFalse;
    gObserverEngineError = EFalse;
	
	gNShwEnginePanicCalled = EFalse;
	gVisualListWasAllocated = EFalse;
	gVisualListWasReleased = EFalse;
	gVisualListManagerWasDeleted = EFalse;
	gPlaybackFactoryWasDeleted = EFalse;
	iCurrentEffect = 0;

	gIncludeMusicControl = EFalse;	// by default no music
	gSetRealSong = ETrue;    // by default set real song
	
	gStrictEffectOrder = ETrue; // by default strict effect order

	// reserve space for the events
	iVisualListCalls.ReserveL( 100 );
	iMediaListCalls.ReserveL( 100 );
	iEvents.ReserveL( 100 );

	// add the test suite to event observers
	gEventObservers.AppendL( this );

    // tell the thumbnail stub that it has pending requests
	// which then tells the thumbnail loader that the 
    // thumbnail was not yet loaded
    gThumbnailContextRequestCount = 1;
    // by default dont reset the request count in init
	gResetThumbnailContextRequestCount = EFalse;

	iCShwSlideshowEngine = CShwSlideshowEngine::NewL( *this );
	}


void T_CShwSlideshowEngine::Teardown()
	{
	// delete the class under test
	delete iCShwSlideshowEngine;
	iCShwSlideshowEngine = NULL;

	// delete the thumbnail load simulator
	delete iThumbnailLoader;
	iThumbnailLoader = NULL;

	// release the events
	iVisualListCalls.Close();
	iMediaListCalls.Close();
	for( TInt i=0; i<iEvents.Count(); i++ )
		{
		MShwEvent* event = iEvents[ i ];
		delete event;
		}
	iEvents.Close();
	
	// reclaim the space the array of observers took
	gEventObservers.Close();

	delete iEffect1;
	iEffect1 = NULL;
	delete iEffect2;
	iEffect2 = NULL;
	delete iEffect3;
	iEffect3 = NULL;

	// release stub list
	delete iStubMedialist;
	iStubMedialist = NULL;
	}

void T_CShwSlideshowEngine::T_ConstructL()
	{
	SetupL();
	EUNIT_ASSERT_DESC( iCShwSlideshowEngine, "engine constructed" );
	EUNIT_ASSERT_EQUALS_DESC( 0, iVisualListCalls.Count(), "visual list was not called" );
	EUNIT_ASSERT_EQUALS_DESC( 0, iMediaListCalls.Count(), "media list was not called" );
	Teardown();
	EUNIT_ASSERT_DESC( !gVisualListWasReleased, "visual list was not released" );
	EUNIT_ASSERT_DESC( gVisualListManagerWasDeleted, "visual list manager was deleted" );
	EUNIT_ASSERT_DESC( !gObserverEngineStarted, "observer engine started was not called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );
	// EUnit checks that memory is in balance, so this is all thats needed.
	}

void T_CShwSlideshowEngine::T_StartLWithListNotReadyL()
	{
	// specify the focus for the lists
	gVisualListInitialFocus = 2;
	iStubMedialist->iFocus = gVisualListInitialFocus;

	// call start
	iCShwSlideshowEngine->StartL( 
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// verify that engine did not start
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "1 events sent" );
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventInitialize*>( iEvents[ 0 ] ),
		"event is initialize" );
	// nothing was done
	EUNIT_ASSERT_DESC( !gObserverEngineStarted, "observer engine started was not called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );

	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for current index
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	// start the wait loop to get the construction completed
	iSchedulerWait.Start();
	
	// check that the engine started
	EUNIT_ASSERT_GREATER_DESC( iEvents.Count(), 2, "init complete event sent" );
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventReadyToView*>( iEvents[ 1 ] ),
		"event is ready to view" );

	// test that memory is released properly if we exit after start
	Teardown();
	EUNIT_ASSERT_DESC( gVisualListWasReleased, "visual list was released" );
	EUNIT_ASSERT_DESC( gVisualListManagerWasDeleted, "visual list manager was deleted" );
	EUNIT_ASSERT_DESC( gPlaybackFactoryWasDeleted, "playback factory was deleted" );

	EUNIT_ASSERT_DESC( gObserverEngineStarted, "observer engine started was called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );
	}

void T_CShwSlideshowEngine::T_StartLWithListReadyL()
	{
	// specify the focus for the lists
	gVisualListInitialFocus = 2;
	iStubMedialist->iFocus = gVisualListInitialFocus;

	// call start
	iCShwSlideshowEngine->StartL( 
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify that thumbnail is ready for first index
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	// start the wait loop to get the construction completed and the notify
	iSchedulerWait.Start();

	// verify that engine did start
	EUNIT_ASSERT_GREATER_DESC( iEvents.Count(), 2, "init complete events sent" );
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventInitialize*>( iEvents[ 0 ] ),
		"event is initialize" );
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventReadyToView*>( iEvents[ 1 ] ),
		"first event is ready to view" );

	// test that memory is released properly if we exit after start
	Teardown();
	EUNIT_ASSERT_DESC( gVisualListWasReleased, "visual list was released" );
	EUNIT_ASSERT_DESC( gVisualListManagerWasDeleted, "visual list manager was deleted" );
	EUNIT_ASSERT_DESC( gPlaybackFactoryWasDeleted, "playback factory was deleted" );

	EUNIT_ASSERT_DESC( gObserverEngineStarted, "observer engine started was called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );
	}

void T_CShwSlideshowEngine::T_StartLTwiceL()
	{
	// specify the focus for the lists
	gVisualListInitialFocus = 2;
	iStubMedialist->iFocus = gVisualListInitialFocus;
	// call start
	iCShwSlideshowEngine->StartL(
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for first index
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	// start the wait loop to get the construction completed and the notify
	iSchedulerWait.Start();

	// verify that engine did start
	EUNIT_ASSERT_GREATER_DESC( iEvents.Count(), 2, "init complete event sent" );
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventInitialize*>( iEvents[ 0 ] ),
		"event is initialize" );
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventReadyToView*>( iEvents[ 1 ] ),
		"first event is ready to view" );
	EUNIT_ASSERT_DESC( gObserverEngineStarted, "observer engine started was called" );
	// and then start again
	gObserverEngineStarted = EFalse;
	// in real environment this is supposed to panic as its a programming error
	// in the test we just leave
	TRAPD(err, iCShwSlideshowEngine->StartL(
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) ) );
	EUNIT_ASSERT_DESC( err == NShwEngine::EEngineStartLCalledInWrongState,
					   "Engine start in wrong state panic");

	EUNIT_ASSERT_DESC( !gObserverEngineStarted, "observer engine started was not called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );
	}

void T_CShwSlideshowEngine::T_StartLAsynchL()
	{
	// specify the focus for the lists
	gVisualListInitialFocus = 1;
	iStubMedialist->iFocus = gVisualListInitialFocus;
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// start the thumbnail loader, it periodically informs that thumbnails are ready
	iThumbnailLoadIndex = gVisualListInitialFocus;
	iThumbnailLoader->Start(
			KDefaultViewDuration * 1250, 
			KDefaultViewDuration * 1250, 
			TCallBack( LoadThumbnailL, this ) );

	// call start
	iCShwSlideshowEngine->StartL(
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// wait for a few event Notifys
	iEventsToWait = 16;
	// and start to wait
	iSchedulerWait.Start();
	// cancel the thumbnail loader
	iThumbnailLoader->Cancel();

	// validate the events
	TInt id=0;
	EUNIT_ASSERT_EQUALS_DESC( iEvents.Count(), iEventsToWait, "16 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventInitialize*>( iEvents[ id++ ] ), "event is initialize" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStart*>( iEvents[ id++ ] ), "event is start" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is startview" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ id++ ] ), "event is ready to advance" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTimerBeat*>( iEvents[ id++ ] ), "event is timer" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartTransition*>( iEvents[ id++ ] ), "event is start transition" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTransitionReady*>( iEvents[ id++ ] ), "event is transition ready" );

	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is start view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTimerBeat*>( iEvents[ id++ ] ), "event is timer" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ id++ ] ), "event is ready to advance" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartTransition*>( iEvents[ id++ ] ), "event is start transition" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTransitionReady*>( iEvents[ id++ ] ), "event is transition ready" );

	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is start view" );

	// test that memory is released properly if we exit after start
	Teardown();
	EUNIT_ASSERT_DESC( gVisualListWasReleased, "visual list was released" );
	EUNIT_ASSERT_DESC( gVisualListManagerWasDeleted, "visual list manager was deleted" );
	EUNIT_ASSERT_DESC( gPlaybackFactoryWasDeleted, "playback factory was deleted" );

	EUNIT_ASSERT_DESC( gObserverEngineStarted, "observer engine started was called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );
	}

void T_CShwSlideshowEngine::T_StartLAsynchSlowImagesL()
	{
	// specify the focus for the lists
	gVisualListInitialFocus = 1;
	iStubMedialist->iFocus = gVisualListInitialFocus;

	// start the thumbnail loader, it periodically informs that thumbnails are ready
	iThumbnailLoadIndex = gVisualListInitialFocus;
	iThumbnailLoader->Start(
			KDefaultViewDuration * 3 * 1000, 
			KDefaultViewDuration * 3 * 1000, 
			TCallBack( LoadThumbnailL, this ) );

	// call start
	iCShwSlideshowEngine->StartL(
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	gThumbnailContextRequestCount = KErrNone;
	// set the thumbnail request to be not complete after init
	gResetThumbnailContextRequestCount = ETrue;

	// wait for a few event notifys
	iEventsToWait = 14;
	// and start to wait
	iSchedulerWait.Start();
	// cancel the thumbnail load generator
	iThumbnailLoader->Cancel();

	// validate the events
	TInt id=0;
	EUNIT_ASSERT_EQUALS_DESC( iEvents.Count(), iEventsToWait, "14 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventInitialize*>( iEvents[ id++ ] ), "event is initialize" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStart*>( iEvents[ id++ ] ), "first event is start" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is startview" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTimerBeat*>( iEvents[ id++ ] ), "event is timer" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ id++ ] ), "event is ready to advance" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartTransition*>( iEvents[ id++ ] ), "event is start transition" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );

	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTransitionReady*>( iEvents[ id++ ] ), "event is transition ready" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is start view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventTimerBeat*>( iEvents[ id++ ] ), "event is timer" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ id++ ] ), "event is ready to advance" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartTransition*>( iEvents[ id++ ] ), "event is start transition" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );

	// test that memory is released properly if we exit after start
	Teardown();
	EUNIT_ASSERT_DESC( gVisualListWasReleased, "visual list was released" );
	EUNIT_ASSERT_DESC( gVisualListManagerWasDeleted, "visual list manager was deleted" );
	EUNIT_ASSERT_DESC( gPlaybackFactoryWasDeleted, "playback factory was deleted" );

	EUNIT_ASSERT_DESC( gObserverEngineStarted, "observer engine started was called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );
	}

void T_CShwSlideshowEngine::T_PauseLL()
	{
	// pause without init or start
	iCShwSlideshowEngine->PauseL();

	EUNIT_ASSERT_DESC( !gObserverEngineStarted, "observer engine started was not called" );
	EUNIT_ASSERT_DESC( !gObserverEnginePaused, "observer engine paused was not called" );
	}

void T_CShwSlideshowEngine::T_PauseL2L()
	{
	// specify the focus for the lists
	gVisualListInitialFocus = 1;
	iStubMedialist->iFocus = gVisualListInitialFocus;
	// call start
	iCShwSlideshowEngine->StartL(
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for focus index 
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	// start to wait to get engine fully constructed
	iSchedulerWait.Start();
	EUNIT_ASSERT_DESC( gObserverEngineStarted, "observer engine started was called" );
	// pause engine
	iCShwSlideshowEngine->PauseL();
	EUNIT_ASSERT_DESC( gObserverEnginePaused, "observer engine paused was called" );
	// validate the events
	TInt id=0;
	EUNIT_ASSERT_EQUALS_DESC( 6, iEvents.Count(), "6 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventInitialize*>( iEvents[ id++ ] ), "event is initialize" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStart*>( iEvents[ id++ ] ), "first event is start" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is startview" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ id++ ] ), "event is ready to advance" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventPause*>( iEvents[ id++ ] ), "event is pause" );

	// then resume straight away
	iCShwSlideshowEngine->ResumeL();
	// check that engine was resumed
	EUNIT_ASSERT_DESC( gObserverEngineResumed, "observer engine resumed was called" );
	EUNIT_ASSERT_EQUALS_DESC( 7, iEvents.Count(), "7 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventResume*>( iEvents[ id++ ] ), "event is resume " );
	// pause again
	iCShwSlideshowEngine->PauseL();
	EUNIT_ASSERT_EQUALS_DESC( 8, iEvents.Count(), "8 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventPause*>( iEvents[ id++ ] ), "event is resume " );

	// need to loosen the effect call order since user navigate jumps like:
	// enter view -> exit view -> enter view
	gStrictEffectOrder = EFalse;

	// set the thumbnail request not to be complete so that engine starts to wait for thumbnails
	gThumbnailContextRequestCount = 1;
	// then navigate to previous item
	iCShwSlideshowEngine->PreviousItemL();
	EUNIT_ASSERT_EQUALS_DESC( 9, iEvents.Count(), "9 events received" );
	// check new events
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventPreviousImage*>( iEvents[ id++ ] ), "event is previous" );

	// set the thumbnail request to be complete now
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for index minus 1
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus - 1 );

	// start a wait as the callback for thumbnail gets called asyncronously
	iSchedulerWait.Start();

	// check that we got event
	EUNIT_ASSERT_EQUALS_DESC( 12, iEvents.Count(), "12 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is startview" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ id++ ] ), "event is ready to advance" );

	// navigate to next item, this thumbnail is already loaded
	iCShwSlideshowEngine->NextItemL();
	// check that we got events
	EUNIT_ASSERT_EQUALS_DESC( 13, iEvents.Count(), "13 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventNextImage*>( iEvents[ id++ ] ), "event is next image" );
	// start a wait as the callback for thumbnail gets called asyncronously
	iSchedulerWait.Start();
	// now we should have the new events
	EUNIT_ASSERT_EQUALS_DESC( 15, iEvents.Count(), "15 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ id++ ] ), "event is ready to view" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is start view" );

	// then resume
	iCShwSlideshowEngine->ResumeL();
	// check that engine was resumed
	EUNIT_ASSERT_DESC( gObserverEngineResumed, "observer engine resumed was called" );
	// check that we got events
	EUNIT_ASSERT_EQUALS_DESC( 17, iEvents.Count(), "17 events received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventResume*>( iEvents[ id++ ] ), "event is resume " );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventStartView*>( iEvents[ id++ ] ), "event is start view");
	}


void T_CShwSlideshowEngine::T_StartWithMusicL()
	{
/// @todo make this an alloc test once MPX is fixed
	gIncludeMusicControl = ETrue;	// have some music
	// specify the focus for the lists
	gVisualListInitialFocus = 2;
	iStubMedialist->iFocus = gVisualListInitialFocus;

	// call start
	iCShwSlideshowEngine->StartL(
		*iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );	
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for first index
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	iVolumeWait.Start();
	}

void T_CShwSlideshowEngine::T_MusicVolumeUpL()
	{
	gIncludeMusicControl = ETrue;	// have some music
	// specify the focus for the lists
	gVisualListInitialFocus = 2;
	iStubMedialist->iFocus = gVisualListInitialFocus;

	// call start
	iCShwSlideshowEngine->StartL( 
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for first index
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	// start scheduler wait
	iVolumeWait.Start();
	
	TInt volume = iCurrentVolume;
	// and then pause
	iCShwSlideshowEngine->VolumeUpL();
	// and start to wait
	iVolumeWait.Start();

	if (volume == KErrNotFound) // First time volume values were retrieved.
		{
		EUNIT_ASSERT_DESC( iCurrentVolume != KErrNotFound, "current volume is < 0" );
		// Retrieve the volume for the second time.		
		volume = iCurrentVolume;
		iCShwSlideshowEngine->VolumeUpL();
		iVolumeWait.Start();
		}
	
	if (volume == iMaxVolume)
		{
		EUNIT_ASSERT_DESC( iCurrentVolume == iMaxVolume, "current volume is max volume" );
		}
	else
		{
		EUNIT_ASSERT_GREATER_DESC( iCurrentVolume, volume, "volume was incremented" );
		}
	}

void T_CShwSlideshowEngine::T_MusicVolumeDownL()
	{
	gIncludeMusicControl = ETrue;	// have some music
	
	// specify the focus for the lists
	gVisualListInitialFocus = 2;
	iStubMedialist->iFocus = gVisualListInitialFocus;

	// call start
	iCShwSlideshowEngine->StartL(
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for first index
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	iVolumeWait.Start();

	TInt volume = iCurrentVolume;
	// and then pause
	iCShwSlideshowEngine->VolumeDownL();
	// and start to wait
	iVolumeWait.Start();
	
	if (volume == KErrNotFound) // First time volume values were retrieved.
		{
		EUNIT_ASSERT_DESC( iCurrentVolume != KErrNotFound, "current volume is < 0" );
		// Retrieve the volume for the second time.		
		volume = iCurrentVolume;
		iCShwSlideshowEngine->VolumeDownL();
		iVolumeWait.Start();
		}

	if (volume == 0)
		{
		EUNIT_ASSERT_EQUALS_DESC( 0, iCurrentVolume, "current volume is 0" );
		}
	else
		{
		EUNIT_ASSERT_GREATER_DESC( volume, iCurrentVolume, "volume was decremented" );
		}
	}

void T_CShwSlideshowEngine::T_MusicNoVolumeEventsL()
	{
	gIncludeMusicControl = ETrue;	// have music control
	gSetRealSong = EFalse;  // but not the proper song
	
	// specify the focus for the lists
	gVisualListInitialFocus = 2;
	iStubMedialist->iFocus = gVisualListInitialFocus;

	// call start - no music control
	iCShwSlideshowEngine->StartL(
	    *iHuiEnv, *iDisplay, *iStubMedialist, *this, TSize( 320, 240 ) );
	// set the thumbnail request to be complete
	gThumbnailContextRequestCount = KErrNone;
	// notify engine that thumbnail is ready for first index
	iStubMedialist->NotifyAttributesAvailableL( gVisualListInitialFocus );
	
	iCurrentVolume = KErrNotFound;
	iMaxVolume     = KErrNotFound;

	if (!iTimer)
		{
		iTimer = CPeriodic::NewL(EPriorityNormal);
		}
	// wait 10 seconds for a volume notification
	const TInt KWaitPeriod = 10000000;    // Microseconds
	TCallBack callBack(StopWaiting, this);
	iTimer->Start(KWaitPeriod, KWaitPeriod, callBack);		

	// turn the volume down
	iCShwSlideshowEngine->VolumeDownL();
	iVolumeWait.Start();

	EUNIT_ASSERT_DESC( 
		 (iCurrentVolume == KErrNotFound || iMaxVolume == KErrNotFound),
		"music control exists" );
	
	delete iTimer;
    iTimer = NULL;

	// test that memory is released properly if we exit after start
	Teardown();
	}

TInt T_CShwSlideshowEngine::StopWaiting(TAny* aTestEngine)
    {
    T_CShwSlideshowEngine* self = reinterpret_cast<T_CShwSlideshowEngine*>(aTestEngine);

	if(self->iVolumeWait.IsStarted())
		{
		self->iVolumeWait.AsyncStop();
		}

    return KErrNone;
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwSlideshowEngine,
    "CShwSlideshowEngine test suite",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Construction test",
    "CShwSlideshowEngine",
    "CShwSlideshowEngine",
    "FUNCTIONALITY",
    EmptyL, T_ConstructL, Teardown )	// need to do teardown since alloc test

EUNIT_ALLOC_TEST(
    "StartL, no Image",
    "CShwSlideshowEngine",
    "StartL",
    "FUNCTIONALITY",
    SetupL, T_StartLWithListNotReadyL, Teardown )

EUNIT_ALLOC_TEST(
    "StartL, Image loaded",
    "CShwSlideshowEngine",
    "StartL",
    "FUNCTIONALITY",
    SetupL, T_StartLWithListReadyL, Teardown )
    
EUNIT_ALLOC_TEST(
    "StartL twice",
    "CShwSlideshowEngine",
    "StartL",
    "FUNCTIONALITY",
    SetupL, T_StartLTwiceL, Teardown )

EUNIT_ALLOC_TEST(
    "StartL asynchronous",
    "CShwSlideshowEngine",
    "StartL",
    "FUNCTIONALITY",
    SetupL, T_StartLAsynchL, Teardown )

EUNIT_TEST( // cant be alloc test as it takes too long
    "StartL slow images",
    "CShwSlideshowEngine",
    "StartL",
    "FUNCTIONALITY",
    SetupL, T_StartLAsynchSlowImagesL, Teardown )

EUNIT_ALLOC_TEST(
    "PauseL without start",
    "CShwSlideshowEngine",
    "PauseL",
    "FUNCTIONALITY",
    SetupL, T_PauseLL, Teardown )

EUNIT_TEST( // ALLOC
    "PauseL with start",
    "CShwSlideshowEngine",
    "PauseL",
    "FUNCTIONALITY",
    SetupL, T_PauseL2L, Teardown )

/// @todo make this an alloc test once MPX is fixed
EUNIT_TEST(
    "Start with music",
    "CShwSlideshowEngine",
    "StartL",
    "FUNCTIONALITY",
    SetupL, T_StartWithMusicL, Teardown )
    
EUNIT_NOT_DECORATED_TEST(
    "Turn music volume up",
    "CShwSlideshowEngine",
    "VolumeUpL",
    "FUNCTIONALITY",
    SetupL, T_MusicVolumeUpL, Teardown )    
    
EUNIT_NOT_DECORATED_TEST(
    "Turn music volume down",
    "CShwSlideshowEngine",
    "VolumeDownL",
    "FUNCTIONALITY",
    SetupL, T_MusicVolumeDownL, Teardown )    
        
EUNIT_NOT_DECORATED_TEST(
    "No volume events when not registered for volume events",
    "CShwSlideshowEngine",
    "VolumeDownL",
    "FUNCTIONALITY",
    SetupL, T_MusicNoVolumeEventsL, Teardown )    
    
EUNIT_END_TEST_TABLE

//  END OF FILE

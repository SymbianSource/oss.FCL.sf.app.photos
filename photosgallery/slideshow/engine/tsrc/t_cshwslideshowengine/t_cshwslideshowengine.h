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




#ifndef __T_CSHWSLIDESHOWENGINE_H__
#define __T_CSHWSLIDESHOWENGINE_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>
#include <digia/eunit/EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "shwengineobserver.h"
#include "shweffectmanager.h"
#include "shweventobserver.h"
#include "shwmusicobserver.h"

#include "tmglxmedialist_stub.h"
#include "tmglxvisuallist_adapter.h"

//  FORWARD DECLARATIONS
class CShwSlideshowEngine;
class CHuiEnv;
class CHuiDisplay;
class MShwEvent;
class T_MShwTestEffect;
class T_Scheduler;

//  CLASS DEFINITION
/**
 *
 */
NONSHARABLE_CLASS( T_CShwSlideshowEngine )
	: public CEUnitTestSuiteClass, 
	public MShwEngineObserver,
	public MGlxMediaList_Stub_Observer,
	public MGlxVisualList_Observer, 
	public MShwEffectManager,
	public MShwEventObserver,
	public MShwMusicObserver	
	{
	public:     // Constructors and destructors

		/**
		 * Two phase construction
		 */
		static T_CShwSlideshowEngine* NewL();
		static T_CShwSlideshowEngine* NewLC();
		/**
		 * Destructor
		 */
		~T_CShwSlideshowEngine();

	private:    // Constructors and destructors

		T_CShwSlideshowEngine();
		void ConstructL();

	public:		// from MShwEngineObserver

		void EngineStartedL();
		void EnginePausedL();
		void EngineResumedL();
        void ErrorDuringSlideshowL();
		
	public:     // from MGlxMediaList_Stub_Observer and MGlxVisualList_Observer
	
		void MGlxMediaList_MethodCalled( TMGlxMediaListMethodId aMethodId );
		void MGlxVisualList_MethodCalled( TMGlxVisualListMethodId aMethodId );

	public:		// from MShwEffectManager

		void AddEffectL( MShwEffect* aEffects );
		MShwEffect* CurrentEffect();
		MShwEffect* Effect( TInt aDirection );
		void ProceedToEffect( TInt aDirection );
        void SetEffectOrder( MShwEffectManager::TShwEffectOrder aOrder );
		MShwEffectManager::TShwEffectOrder EffectOrder();
		void SetProgrammedEffects( RArray< TShwEffectInfo >& aEffects );
		void SetDefaultEffectL(TShwEffectInfo aInfo);
		void GetActiveEffectsL( RPointerArray< MShwEffect >& aEffects );
				
	public:		// from MShwEventObserver
	
		void NotifyL( MShwEvent* aEvent );

	public: 	// from MShwMusicObserver
		
		void MusicOnL();
		void MusicOff();
		void MusicVolume( TInt aCurrentVolume, TInt aMaxVolume );
		void ErrorWithTrackL( TInt aErrorCode );

	public:
		
		static TInt LoadThumbnailL( TAny* aThis );
		static TInt StopWaiting(TAny* aTestEngine);
		
	private:    // New methods

		void EmptyL();
		void SetupL();
		void Teardown();

		void T_ConstructL();
		void T_StartLWithListNotReadyL();
		void T_StartLWithListReadyL();
		void T_StartLTwiceL();
		void T_StartLAsynchL();
		void T_StartLAsynchSlowImagesL();
		void T_PauseLL();
		void T_PauseL2L();
		void T_StartWithMusicL();
		void T_MusicVolumeUpL();
		void T_MusicVolumeDownL();
		void T_MusicNoVolumeEventsL();

	private:    // Data

		CActiveScheduler* iOldScheduler;
		T_Scheduler* iScheduler;

		// own: class under test
		CShwSlideshowEngine* iCShwSlideshowEngine;

		// own: the dependant objects
		CHuiEnv* iHuiEnv;
		CHuiDisplay* iDisplay;
		TMGlxMediaList_Stub* iStubMedialist;
		
		// own: timer to simulate the loading of thumbnails
		CPeriodic* iThumbnailLoader;
		TInt iThumbnailLoadIndex;

		// own: the effects
		TInt iCurrentEffect;
		T_MShwTestEffect* iEffect1;
		T_MShwTestEffect* iEffect2;
		T_MShwTestEffect* iEffect3;

		// own: verification information
        RArray<TInt> iVisualListCalls;
        RArray<TInt> iMediaListCalls;
        RArray<MShwEvent*> iEvents;
        // own: the amount of events received and to wait
		TInt iEventsReceived;
        TInt iEventsToWait;
        // own: the asynch wait object
        CActiveSchedulerWait iSchedulerWait;
		
		// own:  The music state
		TBool iMusicOn;
		// own: The current and max music volume
		TInt iCurrentVolume;
		TInt iMaxVolume;
		// own: the asynch wait object 
        CActiveSchedulerWait iVolumeWait;
		// own: timeout for volume callback
		CPeriodic* iTimer;

		EUNIT_DECLARE_TEST_TABLE; 

	};

#endif      //  __T_CSHWSLIDESHOWENGINE_H__

// End of file

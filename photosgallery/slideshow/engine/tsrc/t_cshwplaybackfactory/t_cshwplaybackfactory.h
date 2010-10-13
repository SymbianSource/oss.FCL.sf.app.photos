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
* Description:    Test suite for CShwPlaybackFactory
 *
*/




#ifndef __T_CSHWPLAYBACKFACTORY_H__
#define __T_CSHWPLAYBACKFACTORY_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>
#include <digia/eunit/EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "shwplaybackfactory.h"
#include "shwmusicobserver.h"

#include "tmglxmedialist_stub.h"
#include "tmglxvisuallist_adapter.h"

//  FORWARD DECLARATIONS
class CHuiEnv;

//  CLASS DEFINITION
/**
 * EUnit test suite for CShwPlaybackFactory
 */
NONSHARABLE_CLASS( T_CShwPlaybackFactory )
	: public CEUnitTestSuiteClass,
	public MGlxMediaList_Stub_Observer,
	public MGlxVisualList_Observer,
	public MShwMusicObserver
	{
	public:     // Constructors and destructors

		/**
		 * Two phase construction
		 */
		static T_CShwPlaybackFactory* NewL();
		static T_CShwPlaybackFactory* NewLC();
		/**
		 * Destructor
		 */
		~T_CShwPlaybackFactory();

	private:    // Constructors and destructors

		T_CShwPlaybackFactory();
		void ConstructL();

	public:     // from MGlxMediaList_Stub_Observer and MGlxVisualList_Observer
	
		void MGlxMediaList_MethodCalled( TMGlxMediaListMethodId /*aMethodId*/ )
			{ // not interested on the events
			}
		void MGlxVisualList_MethodCalled( TMGlxVisualListMethodId /*aMethodId*/ )
			{ // not interested on the events
			}

	public:	// From MShwMusicObserver

		void MusicOnL()
			{ // not interested on the events
			}
		void MusicOff()
			{ // not interested on the events
			}
		void MusicVolume( TInt /*aCurrentVolume*/, TInt /*aMaxVolume*/ )
			{ // not interested on the events
			}
		void ErrorWithTrackL( TInt /*aErrorCode*/ )
			{ // not interested on the events
			}

	private:    // New methods

		void Empty();
		void SetupL();
		void Teardown();
		void T_ContructL();
		void T_ContructValidEffectL();
		void T_EventObserversL();
		void T_EventPublishersL();

	private:    // Data
		
		CHuiEnv* iHuiEnv;
		TMGlxMediaList_Stub* iStubMedialist;
		TMGlxVisualList_Adapter* iStubVisuallist;

		// class under test
		CShwPlaybackFactory* iCShwPlaybackFactory;
		EUNIT_DECLARE_TEST_TABLE; 

	};

#endif      //  __T_CSHWPLAYBACKFACTORY_H__

// End of file

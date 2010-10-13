/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Test for effect control for the slideshow
 *
*/




#ifndef __T_CSHWEFFECTCONTROL_H__
#define __T_CSHWEFFECTCONTROL_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "tmglxvisuallist_adapter.h"
#include "tmglxmedialist_stub.h"
#include "tmshweffectmanager_adapter.h"
#include "shweventqueue.h"

//  FORWARD DECLARATIONS
class CShwEffectControl;

//  CLASS DEFINITION
/**
 * EUnit test suite for CShwEffectControl
 *
 */
NONSHARABLE_CLASS( T_CShwEffectControl )
	: public CEUnitTestSuiteClass, 
	public MGlxMediaList_Stub_Observer,
	public MGlxVisualList_Observer, 
	public MShwEffectManager_Observer, 
	public MShwEventQueue
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CShwEffectControl* NewL();
        static T_CShwEffectControl* NewLC();
        /**
         * Destructor
         */
        ~T_CShwEffectControl();

	private:    // Constructors and destructors

		T_CShwEffectControl();
		void ConstructL();

	public:	// from MGlxMediaList_Stub_Observer, MGlxVisualList_Observer, MShwEffectManager_Observer, MShwEventQueue

		void MGlxMediaList_MethodCalled( TMGlxMediaListMethodId aMethodId );
		void MGlxVisualList_MethodCalled( TMGlxVisualListMethodId aMethodId );
		void MShwEffectManager_MethodCalled( TMShwEffectManagerMethodId aMethodId );
		void SendEventL( MShwEvent* aEvent );

	private:    // New methods

		void EmptySetupL();
		void SetupL();
		void Teardown();

		void TestConstructionL();
		void TransitionCompletedL();
		void TransitionCompleted2L();
		void NotifyLL();
		void SlowImageLoadL();
		void SlowImageLoad2L();
		void HandleFocusChangedL();
		void TestThumbnailLoadingL();
		void TestErrorsInThumbnailLoadingL();

    private:    // Data

        CShwEffectControl* iCShwEffectControl;
        
        TMGlxVisualList_Adapter* iStubVisualList;
		TMGlxMediaList_Stub* iStubMediaList;

        TMShwEffectManager_Adapter iStubEffectManager;
        
        CActiveSchedulerWait iWait;
        
        RArray<TInt> iVisualListCalls;
        RArray<TInt> iMediaListCalls;
        RArray<TInt> iEffectCalls;
        RArray<MShwEvent*> iEvents;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CSHWEFFECTCONTROL_H__

// End of file

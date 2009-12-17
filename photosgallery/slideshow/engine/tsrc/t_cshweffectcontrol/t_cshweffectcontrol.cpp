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




//  CLASS HEADER
#include "t_cshweffectcontrol.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "shwslideshowenginepanic.h"
#include "shweffectcontrol.h"
#include "shwevent.h"
#include "shwthumbnailcontext.h"
#include "shwautoptr.h"

#include "stub_tglxlayoutsplitter.h"
#include "stub_glxfetchcontexts.h"

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

// CONSTRUCTION
T_CShwEffectControl* T_CShwEffectControl::NewL()
    {
    T_CShwEffectControl* self = T_CShwEffectControl::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_CShwEffectControl* T_CShwEffectControl::NewLC()
    {
    T_CShwEffectControl* self = new( ELeave ) T_CShwEffectControl();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_CShwEffectControl::~T_CShwEffectControl()
    {
    }

// Default constructor
T_CShwEffectControl::T_CShwEffectControl() :
	iStubEffectManager( this )
    {
    }

// Second phase construct
void T_CShwEffectControl::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS
void T_CShwEffectControl::MGlxMediaList_MethodCalled( TMGlxMediaListMethodId aMethodId )
	{
	// append the methodid in the array
	TInt error = iMediaListCalls.Append( aMethodId );
	// check that append succeeded
	if( error != KErrNone )
		{
		// critical error, not enough space to append messages
		User::Panic( _L("T_CShwEffectControl::MGlxMediaList_MethodCalled"), __LINE__ );
		}
	}

void T_CShwEffectControl::MGlxVisualList_MethodCalled( TMGlxVisualListMethodId aMethodId )
	{
	// append the methodid in the array
	TInt error = iVisualListCalls.Append( aMethodId );
	// check that append succeeded
	if( error != KErrNone )
		{
		// critical error, not enough space to append messages
		User::Panic( _L("TestError:AppendFail"), -1 );
		}
	}

void T_CShwEffectControl::MShwEffectManager_MethodCalled( TMShwEffectManagerMethodId aMethodId )
	{
	// append the methodid in the array
	TInt error = iEffectCalls.Append( aMethodId );
	// check that append succeeded
	if( error != KErrNone )
		{
		// critical error, not enough space to append messages
		User::Panic( _L("TestError:AppendFail"), -2 );
		}
	}

TInt gSendEventLeaveCode = KErrNone;
void T_CShwEffectControl::SendEventL( MShwEvent* aEvent )
	{
	// need to clone the event since the caller goes out of scope
	TInt error = iEvents.Append( aEvent->CloneLC() );
	CleanupStack::Pop();
	// check that append succeeded
	if( error != KErrNone )
		{
		// critical error, not enough space to append events
		User::Panic( _L("T_CShwEffectControl::NotifyL"), __LINE__ );
		}
    User::LeaveIfError( gSendEventLeaveCode );
	
	// stop the scheduler loop if its started
	if( iWait.IsStarted() )
		{
		iWait.AsyncStop();
		}
	}

void T_CShwEffectControl::EmptySetupL()
	{
	gNShwEnginePanicCalled = EFalse; // by default no panic was called
	gSendEventLeaveCode = KErrNone; // by default no leaves in SendEventL
	// make room for 20 entries on each array
	iVisualListCalls.ReserveL( 20 );
	iMediaListCalls.ReserveL( 20 );
	iEffectCalls.ReserveL( 20 );
	iEvents.ReserveL( 20 );

	iStubVisualList = new( ELeave ) TMGlxVisualList_Adapter( this );
	iStubMediaList = new( ELeave ) TMGlxMediaList_Stub( this );

	// set the size and focus of the list
	iStubVisualList->iSize = 3;
	iStubVisualList->iFocus = 1;
	iStubMediaList->iCount = 3;
	iStubMediaList->iFocus = 1;

	iStubEffectManager.iViewDuration = 0;
	iStubEffectManager.iTransitionDuration = 0;
	
	// reset the visuals of effect stub
    iStubEffectManager.iPrepareVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
    iStubEffectManager.iViewVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
    iStubEffectManager.iTransitionVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
    
    // tell the thumbnail stub to add attributes
    // this tells the thumbnailcontext stub to add entries to the
    // array which then tell the thumbnail loader that the 
    // thumbnail was not yet loaded
    gThumbnailContextRequestCount = 1;
    // by default size is available
    gTGlxMediaGetDimensions = ETrue;
    // by default we dont want alloc behavior
    gFetchContextAllocTest = EFalse;
	}

void T_CShwEffectControl::SetupL()
	{
	// reuse the empty version to create the stubs
	EmptySetupL();
	// create the effect control
	iCShwEffectControl = 
		CShwEffectControl::NewL( 
			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
	// set the event queue
	iCShwEffectControl->SetEventQueue( this );

    // perform the second phase of construction
	TShwEventInitialize initEvent;
    iCShwEffectControl->NotifyL( &initEvent );
	}

void T_CShwEffectControl::Teardown()
	{
	// delete effect control
	delete iCShwEffectControl;
	iCShwEffectControl = NULL; 
	// close the arrays
	iEffectCalls.Close();
	iVisualListCalls.Close();
	iMediaListCalls.Close();
	// release events
	for( TInt i=0; i<iEvents.Count(); i++ )
		{
		MShwEvent* event = iEvents[ i ];
		delete event;
		}
	iEvents.Close();
	// release the visual list stub
	delete iStubVisualList;
	iStubVisualList = NULL;
	// delete the media list stub
	delete iStubMediaList;
	iStubMediaList = NULL;
	}

void T_CShwEffectControl::TestConstructionL()
	{
    // we want alloc behavior
    gFetchContextAllocTest = ETrue;
	// reset stub
	iStubVisualList->iSize = 11;
	iStubVisualList->iFocus = 10;
	// reset the state as setup uses splitter
	gSplitterAddLayoutLCalled = EFalse;
	gSplitterRemoveLayoutLCalled = EFalse;
	// create an effect control
	CShwEffectControl* tmp = 
		CShwEffectControl::NewL( 
			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
	// need to put to cleanupstack in case NotifyL leaves
	CleanupStack::PushL( tmp );

	EUNIT_ASSERT_DESC( tmp, "Construction successfull");

	EUNIT_ASSERT_EQUALS_DESC( 0, iEffectCalls.Count(), "Effect manager was not called" );

    // perform the second phase of construction
	TShwEventInitialize initEvent;
    tmp->NotifyL( &initEvent );

	// check that the calls made by CShwEffectControl were correct ones
	EUNIT_ASSERT_EQUALS_DESC( 2, iEffectCalls.Count(), "Effect manager was called 2 times" );
	EUNIT_ASSERT_EQUALS_DESC( E_MShwEffect_p_CurrentEffect, iEffectCalls[ 0 ], "current effect was called" );
	EUNIT_ASSERT_EQUALS_DESC( E_MGlxLayout_PrepareViewL, iEffectCalls[ 1 ], "current effect was called" );

    // test notify again with size not available
    gTGlxMediaGetDimensions = EFalse;
    // call notify again
    tmp->NotifyL( &initEvent );
	// check that the calls made by CShwEffectControl were correct ones
	EUNIT_ASSERT_EQUALS_DESC( 4, iEffectCalls.Count(), "Effect manager was called 4 times" );
	EUNIT_ASSERT_EQUALS_DESC( E_MShwEffect_p_CurrentEffect, iEffectCalls[ 2 ], "current effect was called" );
	EUNIT_ASSERT_EQUALS_DESC( E_MGlxLayout_PrepareViewL, iEffectCalls[ 3 ], "current effect was called" );

	// check splitter usage
	EUNIT_ASSERT_DESC( !gSplitterAddLayoutLCalled, "layout was not set" );
	EUNIT_ASSERT_DESC( !gSplitterRemoveLayoutLCalled, "layout was not removed" );

	CleanupStack::PopAndDestroy( tmp );
	}

void T_CShwEffectControl::TransitionCompletedL()
	{
	// reset the state as setup uses splitter
	gSplitterAddLayoutLCalled = EFalse;
	gSplitterRemoveLayoutLCalled = EFalse;

	// call transition completed
	iCShwEffectControl->SendTransitionReadyL();

	// check that we got the correct event
	EUNIT_ASSERT_DESC( iEvents.Count() > 0, "we got an event");
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventTransitionReady*>( iEvents[ 0 ] ), 
		"event was correct class" );

	// check splitter usage
	EUNIT_ASSERT_DESC( !gSplitterAddLayoutLCalled, "layout was not set" );
	EUNIT_ASSERT_DESC( !gSplitterRemoveLayoutLCalled, "layout was not removed" );
	}
    
void T_CShwEffectControl::TransitionCompleted2L()
	{
	// tell the thumbnail context that the thumbnail is already loaded
	// so that the thumbnail loader will immediately make the callback
	gThumbnailContextRequestCount = 0;
	// tell the medialist stub to notify that we have the first thumbnail loaded
	iStubMediaList->NotifyAttributesAvailableL( iStubMediaList->iFocus );

	// check that we dont have event
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "event received");
	// check that we got the correct event
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventReadyToView*>( iEvents[ 0 ] ), 
		"event was correct class" );

	// send the event
	TShwEventStartTransition trans( 123 );
	iCShwEffectControl->NotifyL( &trans );
	
	// start the wait loop
	iWait.Start();
	// check that we got event
	EUNIT_ASSERT_EQUALS_DESC( 3, iEvents.Count(), "we got an event");
	// check that we got the correct events
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventReadyToView*>( iEvents[ 1 ] ), 
		"event was correct class" );
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventTransitionReady*>( iEvents[ 2 ] ), 
		"event was correct class" );
	}

void T_CShwEffectControl::NotifyLL()
	{
	TInt initialEffectCalls = iEffectCalls.Count();
	TInt initialVisualCalls = iVisualListCalls.Count();

	// reset the state as setup uses splitter
	gSplitterAddLayoutLCalled = EFalse;
	gSplitterRemoveLayoutLCalled = EFalse;
	// reset the visuals
	iStubEffectManager.iPrepareVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	iStubEffectManager.iViewVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	iStubEffectManager.iTransitionVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );

	// test not supported event path
	iCShwEffectControl->NotifyL( NULL );

	// check that there was no new calls made by CShwEffectControl to visual list or effect manager
	EUNIT_ASSERT_EQUALS_DESC( initialEffectCalls, iEffectCalls.Count(), "Effect manager was not called" );
	EUNIT_ASSERT_EQUALS_DESC( initialVisualCalls, iVisualListCalls.Count(), "Visual list was not called" );

	// test if( viewEvent )

	// tell the stub context that a request is complete
	gThumbnailContextRequestCount = KErrNone;
	// tell the media list stub to notify that thumbnail in focus is loaded
	iStubMediaList->NotifyAttributesAvailableL( iStubMediaList->iFocus );

	// check that we got the event
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
	// this was the first time the event was received so we get init complete event
	EUNIT_ASSERT_DESC( 
		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
		"TShwEventReadyToView received" );

	// tell the stub context that its request is complete, at this point there is no
	// context so we only test that thumbnail context works without one
	gThumbnailContextRequestCount = KErrNone;
	// tell the media list stub to notify that next thumbnail is loaded
	// remember the modulo
	iStubMediaList->NotifyAttributesAvailableL( 
		( iStubMediaList->iFocus + 1 )%iStubMediaList->iCount );

	// if( view_event ) : true
	// if( iLoopStarted ) : false
	TShwEventStartView start( 999 );
	iCShwEffectControl->NotifyL( &start );

	// thumbnail notify is always asynchronous so first check we did not yet get the event
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got only previous event");
	// start async wait
	iWait.Start();

	// check that we got view ready event
	EUNIT_ASSERT_EQUALS_DESC( 2, iEvents.Count(), "we got an event");
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ 1 ] ), "TShwEventReadyToAdvance received" );

	// check the visual given to effect
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iPrepareVisual, 2, "prepare was given visual 2 (focus is on 1)" );
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iViewVisual, 1, "view was given visual 1 (focus is on 1)" );
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iTransitionVisual, KErrNotFound, "transition was not given visual" );
	// check splitter usage
	EUNIT_ASSERT_DESC( gSplitterAddLayoutLCalled, "layout was set (view layout)" );
	EUNIT_ASSERT_DESC( gSplitterRemoveLayoutLCalled, "layout was removed (old view layout)" );

	// test path:
	EUNIT_PRINT( _L("if( view_event ) : false") );
	EUNIT_PRINT( _L("else if( trans_event ) : true") );
	// if( view_event ) : false
	// else if( trans_event ) : true
	TInt effectCalls = iEffectCalls.Count();
	TInt visualCalls = iVisualListCalls.Count();
	// reset the visuals
	iStubEffectManager.iPrepareVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	iStubEffectManager.iViewVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	iStubEffectManager.iTransitionVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	// send the event
	TShwEventStartTransition trans( 666 );
	iCShwEffectControl->NotifyL( &trans );
	// check that view duration was stored from view event
	EUNIT_ASSERT_EQUALS_DESC( 999, iStubEffectManager.iViewDuration, "view duration is set correctly" );
	// check that transition duration was given as parameter
	EUNIT_ASSERT_EQUALS_DESC( 666, iStubEffectManager.iTransitionDuration, "transition duration is set correctly" );
	// check the visual given to effect
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iPrepareVisual, KErrNotFound, "prepare was not given visual" );
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iViewVisual, 2, "view was given visual 2 (focus is on 2)" );
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iTransitionVisual, 1, "transition was given visual 1 (focus is on 1)" );
	// check splitter usage
	EUNIT_ASSERT_DESC( gSplitterAddLayoutLCalled, "layout was set" );
	EUNIT_ASSERT_DESC( gSplitterRemoveLayoutLCalled, "layout was removed" );

	// test path:
	EUNIT_PRINT( _L("if( view_event ) : true") );
	EUNIT_PRINT( _L("if( iLoopStarted ) : true") );
	EUNIT_PRINT( _L("else if( trans_event ) : false") );
	// if( view_event ) : true
	// if( iLoopStarted ) : true
	// reset the state of splitter stub
	gSplitterAddLayoutLCalled = EFalse;
	gSplitterRemoveLayoutLCalled = EFalse;
	effectCalls = iEffectCalls.Count();
	visualCalls = iVisualListCalls.Count();
	// reset the visuals
	iStubEffectManager.iPrepareVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	iStubEffectManager.iViewVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	iStubEffectManager.iTransitionVisual = reinterpret_cast<CHuiVisual*>( KErrNotFound );
	// move focus to 2
	iStubVisualList->iFocus = 2;
	iStubMediaList->iFocus = 2;

	// tell the stub context that its request is complete
	gThumbnailContextRequestCount = KErrNone;
	// tell the media list stub to notify that thumbnail is loaded
	// remember the modulo
	iStubMediaList->NotifyAttributesAvailableL( 
		( iStubMediaList->iFocus + 1 )%iStubMediaList->iCount );

	// send the event
	TShwEventStartView start2( 123 );
	iCShwEffectControl->NotifyL( &start2 );

	// check we got the ready to view event 
	EUNIT_ASSERT_EQUALS_DESC( 3, iEvents.Count(), "we got ready to view");
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToView*>( iEvents[ 2 ] ), "TShwEventReadyToView received" );
	// start async wait
	iWait.Start();

	// check that view duration was not set
	EUNIT_ASSERT_EQUALS_DESC( 999, iStubEffectManager.iViewDuration, "view duration is same as last, see above" );
	// check the visual given to effect
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iPrepareVisual, 0, "prepare view was given visual 0 (focus is on 2, size is 3)" );
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iViewVisual, KErrNotFound, "view was not given visual" );
	EUNIT_ASSERT_EQUALS_DESC( (TInt)iStubEffectManager.iTransitionVisual, 1, "exit transition was given visual 1 (focus is on 2)" );
	// check that we got view ready event
	EUNIT_ASSERT_EQUALS_DESC( 4, iEvents.Count(), "event received" );
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ 3 ] ), "TShwEventReadyToAdvance received" );

	// check splitter usage
	EUNIT_ASSERT_DESC( !gSplitterAddLayoutLCalled, "layout was not set" );
	EUNIT_ASSERT_DESC( gSplitterRemoveLayoutLCalled, "layout was removed" );
	}

void T_CShwEffectControl::SlowImageLoadL()
	{
	// test path:
	EUNIT_PRINT( _L("if( view_event ) : true") );
	EUNIT_PRINT( _L("if( iLoopStarted ) : false") );
	EUNIT_PRINT( _L("else if( trans_event ) : false") );

	// tell the stub context that its request is complete
	gThumbnailContextRequestCount = KErrNone;
	// tell the media list stub to notify that thumbnail in focus is loaded
	iStubMediaList->NotifyAttributesAvailableL( iStubMediaList->iFocus );

	// check that we got the event
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
	// this was the first time the event was received so we get init complete event
	EUNIT_ASSERT_DESC( 
		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
		"TShwEventReadyToView received" );

	// tell the stub context that its request is not complete
	gThumbnailContextRequestCount = 1;

	// if( view_event ) : true
	// if( iLoopStarted ) : false
	TShwEventStartView start( 999 );
	iCShwEffectControl->NotifyL( &start );
	// variable iLoopStarted is initialized as false so we dont go through the loop
	// check that we got view ready event
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "new event not received" );

	// tell the stub context that its request is complete
	gThumbnailContextRequestCount = KErrNone;
	// tell the media list stub to notify that thumbnail is loaded
	// remember the modulo
	iStubMediaList->NotifyAttributesAvailableL( 
		( iStubMediaList->iFocus + 1 )%iStubMediaList->iCount );

	EUNIT_ASSERT_EQUALS_DESC( 2, iEvents.Count(), "we got an event");
	EUNIT_ASSERT_DESC( dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ 1 ] ), "TShwEventReadyToAdvance received" );
	}

void T_CShwEffectControl::SlowImageLoad2L()
	{
	// set the size and focus of the list
	iStubVisualList->iSize = 10;
	iStubVisualList->iFocus = 1;
	iStubMediaList->iCount = 10;
	iStubMediaList->iFocus = 1;
	// test path:
	EUNIT_PRINT( _L("if( view_event ) : true") );
	EUNIT_PRINT( _L("if( iLoopStarted ) : false") );
	EUNIT_PRINT( _L("else if( trans_event ) : false") );

	// tell the stub context that its request is complete
	gThumbnailContextRequestCount = KErrNone;
	// tell the media list stub to notify that thumbnail in focus is loaded
	iStubMediaList->NotifyAttributesAvailableL( iStubMediaList->iFocus );

	// check that we got the event
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
	// this was the first time the event was received so we get init complete event
	EUNIT_ASSERT_DESC( 
		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
		"TShwEventReadyToView received" );

	// tell the stub context that its request is not complete
	gThumbnailContextRequestCount = 1;

	// if( view_event ) : true
	// if( iLoopStarted ) : false
	TShwEventStartView start( 999 );
	iCShwEffectControl->NotifyL( &start );
	// variable iLoopStarted is initialized as false so we dont go through the loop
	// check that we got view ready event
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "new event not received" );

	// tell the stub context that its request is complete
	gThumbnailContextRequestCount = KErrNone;
	// tell the media list stub to notify that thumbnail is loaded
	// remember the modulo
	iStubMediaList->NotifyAttributesAvailableL( 
		( iStubMediaList->iFocus + 3 )%iStubMediaList->iCount );
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "new event not received" );

	// tell the media list stub to notify that thumbnail is loaded
	// remember the modulo
	iStubMediaList->NotifyAttributesAvailableL( 
		( iStubMediaList->iFocus + 2 )%iStubMediaList->iCount );
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "new event not received" );

	// tell the stub context that its request is not complete
	gThumbnailContextRequestCount = 1;
	// tell the media list stub to notify that thumbnail is loaded
	// remember the modulo
	iStubMediaList->NotifyAttributesAvailableL( 
		( iStubMediaList->iFocus + 1 )%iStubMediaList->iCount );
	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "new event not received" );

	// tell the stub context that its request is complete
	gThumbnailContextRequestCount = KErrNone;
	iStubMediaList->NotifyAttributesAvailableL( 
		( iStubMediaList->iFocus + 1 )%iStubMediaList->iCount );
	EUNIT_ASSERT_EQUALS_DESC( 2, iEvents.Count(), "event received" );
	// this was second time the event was received so we get view complete event
	EUNIT_ASSERT_DESC( 
		dynamic_cast<TShwEventReadyToAdvance*>( iEvents[ 1 ] ), 
		"TShwEventViewReady received" );
	}

void T_CShwEffectControl::HandleFocusChangedL()
	{
	TInt initialVisualListCalls = iVisualListCalls.Count();
	iCShwEffectControl->HandleFocusChangedL( 0, 0, NULL, NGlxListDefs::EUnknown );
	EUNIT_ASSERT_NOT_EQUALS_DESC( 
		initialVisualListCalls, iVisualListCalls.Count(), "Visual list was called" );
	}

void T_CShwEffectControl::TestThumbnailLoadingL()
	{
	// create shw thumbnail context for index 1 and size 100,100
	const TInt thumbIndex = 1;
	CShwThumbnailContext* context = CShwThumbnailContext::NewLC( thumbIndex, TSize( 100, 100 ) );

    // test that the index is returned
    // set iterator to first
    context->SetToFirst( iStubMediaList );
    // then iterate once
    TInt index = (*context)++;
    EUNIT_ASSERT_EQUALS_DESC( thumbIndex, index, "check first iterated index" );

    index = (*context)++;
    EUNIT_ASSERT_EQUALS_DESC( KErrNotFound, index, "next index is KErrNotFound" );

    // check range
    EUNIT_ASSERT_DESC( !context->InRange( thumbIndex-1 ), "thumbIndex-1 is not in range" );
    EUNIT_ASSERT_DESC( !context->InRange( thumbIndex+1 ), "thumbIndex+1 is not in range" );
    EUNIT_ASSERT_DESC( context->InRange( thumbIndex ), "thumbIndex is in range" );

    CleanupStack::PopAndDestroy( context );
	}

void T_CShwEffectControl::TestErrorsInThumbnailLoadingL()
	{
	// set the size and focus of the list
	iStubVisualList->iSize = 10;
	iStubVisualList->iFocus = 1;
	iStubMediaList->iCount = 10;
	iStubMediaList->iFocus = 1;
// first test that first thumbnail does not load
	// tell the stub context that its request has error
	gThumbnailContextRequestCount = KErrCorrupt;
        {
    	// create an effect control, no need to delete or cleanupstack
    	TShwAutoPtr< CShwEffectControl > tmp = 
    		CShwEffectControl::NewL( 
    			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
    	// need to set the event queue		
    	tmp->SetEventQueue( this );
        // perform the second phase of construction
    	TShwEventInitialize initEvent;
        tmp->NotifyL( &initEvent );
    	
        // check that we did not get an event
    	EUNIT_ASSERT_EQUALS_DESC( 0, iEvents.Count(), "we did not yet get an event");

    	// start async wait loop
    	iWait.Start();
        
    	// check that we got the event
    	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
    	// this was the first time the event was received so we get init complete event
    	EUNIT_ASSERT_DESC( 
    		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
    		"TShwEventReadyToView received" );
        // delete and remove the event
        delete iEvents[ 0 ];
        iEvents.Remove( 0 );
    	// tmp gets deleted here, EUnit checks for memory
        }

// test that second thumbnail does not load
	// tell the stub context that its request is not yet completed
	gThumbnailContextRequestCount = 1;
        {
    	// create an effect control, no need to delete or cleanupstack
    	TShwAutoPtr< CShwEffectControl > tmp = 
    		CShwEffectControl::NewL( 
    			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
    	// need to set the event queue
    	tmp->SetEventQueue( this );
        // perform the second phase of construction
    	TShwEventInitialize initEvent;
        tmp->NotifyL( &initEvent );
        // check that we did not get an event
    	EUNIT_ASSERT_EQUALS_DESC( 0, iEvents.Count(), "we did not yet get new event");
    	
    	// tell the stub context that the request has error
    	gThumbnailContextRequestCount = KErrCorrupt;
    	// tell the media list stub to notify that first thumbnail is loaded
    	iStubMediaList->NotifyAttributesAvailableL( iStubMediaList->iFocus  );

    	// check that we got the event
    	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
    	// this was the first time the event was received so we get init complete event
    	EUNIT_ASSERT_DESC( 
    		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
    		"TShwEventReadyToView received" );
        // delete and remove the event
        delete iEvents[ 0 ];
        iEvents.Remove( 0 );
    	// tmp gets deleted here, EUnit checks for memory
        }

// test HandleError call before first thumbnail
	// tell the stub context that its request is not yet completed
	gThumbnailContextRequestCount = 1;
        {
    	// create an effect control, no need to delete or cleanupstack
    	TShwAutoPtr< CShwEffectControl > tmp = 
    		CShwEffectControl::NewL( 
    			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
    	// need to set the event queue		
    	tmp->SetEventQueue( this );
        // perform the second phase of construction
    	TShwEventInitialize initEvent;
        tmp->NotifyL( &initEvent );
        // check that we did not get an event
    	EUNIT_ASSERT_EQUALS_DESC( 0, iEvents.Count(), "we did not yet get new event");

    	// tell the stub context that the request has error
    	gThumbnailContextRequestCount = KErrCorrupt;
    	// tell the media list stub to notify that there was an error
    	iStubMediaList->NotifyError( KErrNoMemory  );

    	// start async wait loop
    	iWait.Start();

    	// check that we got the event
    	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
    	// this was the first time the event was received so we get init complete event
    	EUNIT_ASSERT_DESC( 
    		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
    		"TShwEventReadyToView received" );
        // delete and remove the event
        delete iEvents[ 0 ];
        iEvents.Remove( 0 );
    	// tmp gets deleted here, EUnit checks for memory
        }

// test HandleError call after first thumbnail
	// tell the stub context that its request is completed
	gThumbnailContextRequestCount = KErrNone;
        {
    	// create an effect control, no need to delete or cleanupstack
    	TShwAutoPtr< CShwEffectControl > tmp = 
    		CShwEffectControl::NewL( 
    			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
    	// need to set the event queue		
    	tmp->SetEventQueue( this );
        // perform the second phase of construction
    	TShwEventInitialize initEvent;
        tmp->NotifyL( &initEvent );
        // check that we did not get an event
    	EUNIT_ASSERT_EQUALS_DESC( 0, iEvents.Count(), "we did not yet get new event");

    	// start async wait loop
    	iWait.Start();

    	// check that we got the event
    	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
    	// this was the first time the event was received so we get init complete event
    	EUNIT_ASSERT_DESC( 
    		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
    		"TShwEventReadyToView received" );
        // delete and remove the event
        delete iEvents[ 0 ];
        iEvents.Remove( 0 );

    	// tell the stub context that its request is not yet completed
    	gThumbnailContextRequestCount = 1;
        // send start view
        TShwEventStartView startView( 123 );
        tmp->NotifyL( &startView );

    	// tell the stub context that the request has error
    	gThumbnailContextRequestCount = KErrCorrupt;
    	// tell the media list stub to notify that there was an error
    	iStubMediaList->NotifyError( KErrNoMemory  );

    	// start async wait loop
    	iWait.Start();

    	// check that we got the event
    	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
    	// this was the first time the event was received so we get init complete event
    	EUNIT_ASSERT_DESC( 
    		dynamic_cast< TShwEventReadyToAdvance* >( iEvents[ 0 ] ), 
    		"TShwEventReadyToView received" );
        // delete and remove the event
        delete iEvents[ 0 ];
        iEvents.Remove( 0 );
    	// tmp gets deleted here, EUnit checks for memory
        }

// test HandleItemRemovedL call
	// tell the stub context that its request is not completed
	gThumbnailContextRequestCount = KErrNone;
        {
    	// create an effect control, no need to delete or cleanupstack
    	TShwAutoPtr< CShwEffectControl > tmp = 
    		CShwEffectControl::NewL( 
    			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
    	// need to set the event queue		
    	tmp->SetEventQueue( this );
        // perform the second phase of construction
    	TShwEventInitialize initEvent;
        tmp->NotifyL( &initEvent );
        // check that we did not get an event
    	EUNIT_ASSERT_EQUALS_DESC( 0, iEvents.Count(), "we did not yet get new event");

    	// start async wait loop
    	iWait.Start();

    	// check that we got the event
    	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
    	// this was the first time the event was received so we get init complete event
    	EUNIT_ASSERT_DESC( 
    		dynamic_cast< TShwEventReadyToView* >( iEvents[ 0 ] ), 
    		"TShwEventReadyToView received" );
        // delete and remove the event
        delete iEvents[ 0 ];
        iEvents.Remove( 0 );
        
        // tell medialist stub to call HandleItemRemovedL
        iStubMediaList->NotifyItemRemoved( 0, 1 );
        // this time the list was not empty so nothing done
    	// check that we got no event
    	EUNIT_ASSERT_EQUALS_DESC( 0, iEvents.Count(), "we got no event");
        // remove items from stub medialist
        iStubMediaList->iCount = 0;
        // tell medialist stub to call HandleItemRemovedL
        iStubMediaList->NotifyItemRemoved( 0, 1 );

    	// check that we got the event
    	EUNIT_ASSERT_EQUALS_DESC( 1, iEvents.Count(), "we got an event");
    	// this was the first time the event was received so we get init complete event
    	EUNIT_ASSERT_DESC( 
    		dynamic_cast< TShwEventFatalError* >( iEvents[ 0 ] ), 
    		"TShwEventReadyToView received" );
        // delete and remove the event
        delete iEvents[ 0 ];
        iEvents.Remove( 0 );

        // tell medialist stub to call HandleItemRemovedL again
        iStubMediaList->NotifyItemRemoved( 0, 1 );
        // this does nothing as the thumbnail loader was deleted and all contexts removed
    	// check that we got no event
    	EUNIT_ASSERT_EQUALS_DESC( 0, iEvents.Count(), "we got an event");
    	// tmp gets deleted here, EUnit checks for memory
        }

    // test also leave on SendEventL
	// tell the stub context that its request is not completed
	gThumbnailContextRequestCount = KErrNone;
        {
    	// create an effect control, no need to delete or cleanupstack
    	TShwAutoPtr< CShwEffectControl > tmp = 
    		CShwEffectControl::NewL( 
    			iStubEffectManager, *iStubVisualList, *iStubMediaList, TSize( 320, 240 ) );
    	// need to set the event queue		
    	tmp->SetEventQueue( this );
        // perform the second phase of construction
    	TShwEventInitialize initEvent;
        tmp->NotifyL( &initEvent );
    	// start async wait loop
    	iWait.Start();
        // remove items from stub medialist
        iStubMediaList->iCount = 0;
        // tell SendEventL to leave
        gSendEventLeaveCode = KErrNoMemory;
        // tell medialist stub to call HandleItemRemovedL
        TRAPD( error, iStubMediaList->NotifyItemRemoved( 0, 1 ) );
        // check that engine did panic
        EUNIT_ASSERT_DESC( gNShwEnginePanicCalled, "Engine panic was called" );
    	EUNIT_ASSERT_EQUALS_DESC( 
    	    NShwEngine::EEngineFatalError, error, "panic code was fatal error");
        }
    
	}


//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CShwEffectControl,
    "Test suite for CShwEffectControl",
    "UNIT" )

EUNIT_ALLOC_TEST(
    "Construct-destruct test",
    "CShwEffectControl",
    "CShwEffectControl",
    "FUNCTIONALITY",
    EmptySetupL, TestConstructionL, Teardown )	// needs to have teardown as alloc test
    
EUNIT_ALLOC_TEST(
    "test TransitionCompleted",
    "CShwEffectControl",
    "TransitionCompleted",
    "FUNCTIONALITY",
    SetupL, TransitionCompletedL, Teardown )

EUNIT_NOT_DECORATED_TEST(	// cant be decorated as active objects
    "test TransitionCompleted",
    "CShwEffectControl",
    "TransitionCompleted",
    "FUNCTIONALITY",
    SetupL, TransitionCompleted2L, Teardown )

EUNIT_NOT_DECORATED_TEST(	// cant be decorated as active objects
    "test NotifyL",
    "CShwEffectControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, NotifyLL, Teardown )

EUNIT_NOT_DECORATED_TEST(	// cant be decorated as active objects
    "slow image load",
    "CShwEffectControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, SlowImageLoadL, Teardown )

EUNIT_NOT_DECORATED_TEST(	// cant be decorated as active objects
    "slow image load",
    "CShwEffectControl",
    "NotifyL",
    "FUNCTIONALITY",
    SetupL, SlowImageLoad2L, Teardown )

EUNIT_TEST(
    "HandleFocusChangedL",
    "CShwEffectControl",
    "HandleFocusChangedL",
    "FUNCTIONALITY",
    SetupL, HandleFocusChangedL, Teardown )

EUNIT_TEST(
    "Thumbnail loading",
    "CShwThumbnailContext",
    "multiple",
    "FUNCTIONALITY",
    SetupL, TestThumbnailLoadingL, Teardown )

EUNIT_TEST(
    "Thumbnail errors",
    "CShwThumbnailLoader",
    "multiple",
    "FUNCTIONALITY",
    EmptySetupL, TestErrorsInThumbnailLoadingL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE

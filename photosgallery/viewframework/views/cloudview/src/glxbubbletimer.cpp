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
* Description:    Implementation of Cloud view
 *
*/




// INCLUDE FILES
#include "glxbubbletimer.h" //class declaration

#include <glxtracer.h>					// For Logs


// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//

CGlxBubbleTimer::~CGlxBubbleTimer()
	{
	TRACER("GLX_CLOUD::CGlxBubbleTimer::~CGlxBubbleTimer");
	iTimer.Close ();
	if ( iNotifier)
		{
		iNotifier =NULL;
		}
	}

// ConstructL
// ---------------------------------------------------------------------------
// 
void CGlxBubbleTimer::ConstructL()
	{
	TRACER("GLX_CLOUD::CGlxBubbleTimer::ConstructL()");
	User::LeaveIfError (iTimer.CreateLocal ());
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
CGlxBubbleTimer *CGlxBubbleTimer::NewL(MGlxTimerNotifier *aNotifier)
	{
	TRACER("GLX_CLOUD::CGlxBubbleTimer::NewL");
	CGlxBubbleTimer *self = new(ELeave)CGlxBubbleTimer(aNotifier);
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (self);
	return self;
	}

// ---------------------------------------------------------------------------
// Default C++ Constructor
// ---------------------------------------------------------------------------
// 

CGlxBubbleTimer::CGlxBubbleTimer(MGlxTimerNotifier *aNotifier) :
	CActive(0), iNotifier(aNotifier)
	{
	TRACER("GLX_CLOUD::CGlxBubbleTimer::CGlxBubbleTimer");
	CActiveScheduler::Add (this);

	}

// ---------------------------------------------------------------------------
// SetDelay()
// ---------------------------------------------------------------------------
//
void CGlxBubbleTimer::SetDelay(TTimeIntervalMicroSeconds32 aDelay)
    {
    TRACER("GLX_CLOUD::CGlxBubbleTimer::SetDelay");
    //iTimer.Cancel() ;
    Cancel ();

    _LIT(KDelayedHelloPanic, "CGlxBubbleTimer");
    __ASSERT_ALWAYS(!IsActive(), User::Panic(KDelayedHelloPanic, 1));
    // iTimer.Close() ;
    if( !IsActive() )
        {
        iTimer.After (iStatus, aDelay);
        SetActive ();
        }
    }

// ---------------------------------------------------------------------------
// RunL()
// ---------------------------------------------------------------------------
//
void CGlxBubbleTimer::RunL()
	{
	TRACER("GLX_CLOUD::CGlxBubbleTimer::RunL");
	iNotifier->TimerCompleteL(); //notify that timer is over
	}

// ---------------------------------------------------------------------------
// DoCancel()
// ---------------------------------------------------------------------------
//

void CGlxBubbleTimer::DoCancel()
	{
	TRACER("GLX_CLOUD::CGlxBubbleTimer::DoCancel");
	iTimer.Cancel ();
	}

//  End of File

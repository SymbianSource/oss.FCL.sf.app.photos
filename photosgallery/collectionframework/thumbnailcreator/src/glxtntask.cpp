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
* Description:    Base classes for multi-purpuse tasks
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */


#include "glxtntask.h"

#include <glxtracer.h>

#include "mglxtnthumbnailcreatorclient.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnTask::CGlxtnTask(const TGlxtnTaskId& aId, const TGlxMediaId& aItemId,
                        MGlxtnThumbnailStorage* aStorage)
		: iState(EStarting), iId(aId), iItemId(aItemId), iStorage(aStorage)
    {
    TRACER("CGlxtnTask::CGlxtnTask()");
    iPriority = CActive::EPriorityStandard;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnTask::~CGlxtnTask()
    {
    TRACER("CGlxtnTask::~CGlxtnTask()");
    }

// -----------------------------------------------------------------------------
// SetManager
// -----------------------------------------------------------------------------
//
void CGlxtnTask::SetManager(CGlxtnTaskManager* /*aManager*/)
    {
    TRACER("CGlxtnTask::SetManager()");
    // No implementation - can be overridden by subclasses
    }

// -----------------------------------------------------------------------------
// StartL
// -----------------------------------------------------------------------------
//
void CGlxtnTask::StartL(TRequestStatus& aStatus)
	{
    TRACER("CGlxtnTask::StartL()");
	__ASSERT_DEBUG(iState == EIdle || iState == EStarting, Panic(EGlxPanicIllegalState));
	
	iState = EStarting;
	
	TBool active = DoStartL(aStatus);
	
	if (active) 
		{
		iState = ERunning;
		}
	else 
		{
		iState = EComplete; // Well, that was quick
		}
	}

// -----------------------------------------------------------------------------
// Cancel
// -----------------------------------------------------------------------------
//
void CGlxtnTask::Cancel()
    {
    TRACER("CGlxtnTask::Cancel()");
    if ( ERunning == iState )
        {
        DoCancel();
        }

    iState = ECanceled;
    }
	
// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CGlxtnTask::RunL(TRequestStatus& aStatus) 
	{
    TRACER("void CGlxtnTask::RunL()");
	__ASSERT_DEBUG(iState == ERunning, Panic(EGlxPanicIllegalState));
	
	TBool active = DoRunL(aStatus);
	
	if (!active) 
		{
		iState = EComplete; // The task is done
		}
	}

// -----------------------------------------------------------------------------
// RunError
// -----------------------------------------------------------------------------
//
void CGlxtnTask::RunError(TInt aError)
    {
    TRACER("void CGlxtnTask::RunError()");
    if ( !DoRunError(aError) ) 
        {
        iState = EComplete; // The task is done
        }
    }

// -----------------------------------------------------------------------------
// SelfComplete
// -----------------------------------------------------------------------------
//
void CGlxtnTask::SelfComplete(TRequestStatus& aStatus) 
	{
    TRACER("CGlxtnTask::SelfComplete()");
    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete(status, KErrNone);
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnClientTask::CGlxtnClientTask(const TGlxtnTaskId& aId,
        const TGlxMediaId& aItemId, MGlxtnThumbnailCreatorClient& aClient) :
    CGlxtnTask(aId, aItemId, aClient.ThumbnailStorage()), iClient(aClient)
    {
    TRACER("CGlxtnClientTask::CGlxtnClientTask()");
    }

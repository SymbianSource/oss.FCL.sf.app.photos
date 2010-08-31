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
* Description:   Task manager for multiple tasks. Allows asynchronosity and 
*                tracking/callback for long running tasks.
*
*/



/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtntaskmanager.h"

#include <glxtracer.h>

#include "glxtnbackgroundgenerationtask.h"
#include "glxtntask.h"

const TInt KGlxTMMaxTasksToRunInOneRound = 4;
// -----------------------------------------------------------------------------
// Static Constructor
// -----------------------------------------------------------------------------
//
CGlxtnTaskManager* CGlxtnTaskManager::NewL()
    {
    TRACER("CGlxtnTaskManager::NewL()");
	CGlxtnTaskManager* self = new (ELeave) CGlxtnTaskManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnTaskManager::CGlxtnTaskManager() 
		: CActive(EPriorityStandard)
    {
    TRACER("CGlxtnTaskManager::CGlxtnTaskManager()");
    }
     
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxtnTaskManager::ConstructL()
    {
    TRACER("void CGlxtnTaskManager::ConstructL()");
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnTaskManager::~CGlxtnTaskManager()
    {
    TRACER("CGlxtnTaskManager::~CGlxtnTaskManager()");
    Cancel(); 
    iTasks.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// Task by id
// -----------------------------------------------------------------------------
//
CGlxtnTask* CGlxtnTaskManager::Task(const TGlxtnTaskId& aId) const
	{
    TRACER("CGlxtnTaskManager::Task(const TGlxtnTaskId& aId)");
	TInt count = iTasks.Count();
	for (TInt i = 0; i < count; i++) 
		{
		CGlxtnTask* task = iTasks[i];
		if (task->Id() == aId) 
			{
			return task;
			}
		}
		
	// Not found
	return NULL;
	}

// -----------------------------------------------------------------------------
// AddTaskL
// -----------------------------------------------------------------------------
//
void CGlxtnTaskManager::AddTaskL(CGlxtnTask* aTask, TExecutionOrder aExecutionOrder)
    {
    TRACER("CGlxtnTaskManager::AddTaskL()");
    __ASSERT_DEBUG(aTask, Panic(EGlxPanicNullPointer));
	__ASSERT_DEBUG(iTasks.Find(aTask) == KErrNotFound, Panic(EGlxPanicIllegalArgument)); // Task already added
    CleanupStack::PushL(aTask);

 	// Order manually to allow full control over placement of tasks when 
 	// priorities are the same
 	TInt priority = aTask->Priority();
 	TInt count = iTasks.Count();

    TInt pos = 0;
    while ( pos < count )
        {
 		TInt compPriority = iTasks[pos]->Priority();
 		if ( compPriority < priority
 		    || (compPriority == priority && EFirstOut == aExecutionOrder) )
 		    {
 		    break;
 		    }

        pos++;
        }

    iTasks.InsertL(aTask, pos);
    CleanupStack::Pop(aTask);

    aTask->SetManager(this);

	// Start processing the task if this active object is not already doing something else
	if ( !iCurrentTask ) 
		{
        AsyncRun();
		}
    else if( dynamic_cast<CGlxtnBackgroundGenerationTask*>(iCurrentTask) && (!dynamic_cast<CGlxtnBackgroundGenerationTask*>(aTask)) )
        {
        iCurrentTask->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CancelTasks
// Cancel all tasks relating to a given media item.
// -----------------------------------------------------------------------------
//
void CGlxtnTaskManager::CancelTasks(const TGlxMediaId& aItemId)
    {
    TRACER("CGlxtnTaskManager::CancelTasks()");
    TInt count = iTasks.Count();

    if( aItemId == TGlxMediaId(0) )
        {
        if( dynamic_cast<CGlxtnBackgroundGenerationTask*>(iCurrentTask) )
        {
        iCurrentTask->Cancel();
        }
        }

    for ( TInt i = 0; i < count; i++ )
        {
        CGlxtnTask* task = iTasks[i];

        if ( task->ItemId() == aItemId )
            {
            task->Cancel();		
            }
        }
    }

// -----------------------------------------------------------------------------
// CancelTasks
// Cancel all tasks using a given storage.
// -----------------------------------------------------------------------------
//
void CGlxtnTaskManager::CancelTasks(MGlxtnThumbnailStorage* aStorage)
    {
    TRACER("void CGlxtnTaskManager::CancelTasks()");
    TInt count = iTasks.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        CGlxtnTask* task = iTasks[i];

        if ( task->Storage() == aStorage )
            {
            task->Cancel();		
            }
        }
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//   
void CGlxtnTaskManager::RunL()
	{
    TRACER("void CGlxtnTaskManager::RunL()");
    TInt taskIndex = 0;
    TInt tasksRun = 0;
    while ( taskIndex < iTasks.Count() )
        {
    	if ( iCurrentTask )
    	    {
            if ( CGlxtnTask::EStarting == iCurrentTask->State() ) 
                {
                // Start the task
                iCurrentTask->StartL(iStatus);
                }
            else if ( CGlxtnTask::ERunning == iCurrentTask->State() )
                {
                // Let the task run
                iCurrentTask->RunL(iStatus);
                }

            if ( CGlxtnTask::ERunning == iCurrentTask->State() )
                {
                SetActive();
                return;
                }

            // Task completed or cancelled, delete the task immediately
            __ASSERT_DEBUG(iStatus != KRequestPending, Panic(EGlxPanicIllegalState));
            __ASSERT_DEBUG(CGlxtnTask::EComplete == iCurrentTask->State()
                        || CGlxtnTask::ECanceled == iCurrentTask->State(), Panic(EGlxPanicIllegalState));

            // Search for the task again, since the processing of the task might have added new tasks to the queue
            TInt index = iTasks.Find(iCurrentTask);
            __ASSERT_DEBUG(index != KErrNotFound, Panic(EGlxPanicLogicError));
            iTasks.Remove(index);
            delete iCurrentTask;
            iCurrentTask = NULL;

            // Start from top, since the task might have added another task
            taskIndex = 0;
    	    }
    	else
    	    {
            CGlxtnTask* task = iTasks[taskIndex];
            CGlxtnTask::TState state = task->State();

            switch ( state )
                {
                case CGlxtnTask::EIdle:
                    // Task not started. Ignore and try next task
                    taskIndex++; // Skip
                    break;
                case CGlxtnTask::EStarting:
                    {
                    // Start the task
                    iCurrentTask = task;

                    // Update active object priority to match task priority
                    TInt priority = task->Priority();
                    if ( priority != Priority() )
                        {
                        SetPriority(priority);
                        }

                    // Check if too many tasks have been run during this call to RunL. 
                    // This check exist to make sure a blocking loop does not occur, in which
                    // a task creates another task which completes immediately, which creates
                    // another task, which also completes immediately. (Task don't have to 
                    // be asynchronous.)
                    tasksRun++;
                    if ( tasksRun >= KGlxTMMaxTasksToRunInOneRound )
                        {
                        AsyncRun();
                        return;
                        }
                    }
                    break;
                case CGlxtnTask::ECanceled:
        			// Task is canceled, remove from queue
        			delete task;
        			iTasks.Remove(taskIndex);
                    break;
                default:
                    Panic(EGlxPanicIllegalState);   // Unknown or wrong state
                    break;
                }
    	    }
        }
	}
	
// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnTaskManager::DoCancel()
    {
    TRACER("CGlxtnTaskManager::DoCancel()");
    TInt count = iTasks.Count();
	for (TInt i = 0; i < count; i++)
		{
		CGlxtnTask* task = iTasks[i];
		CGlxtnTask::TState state = task->State();
		if (state == CGlxtnTask::ERunning) 
			{
			task->Cancel();
			}
		}
    }

// -----------------------------------------------------------------------------
// RunError
// -----------------------------------------------------------------------------
//
TInt CGlxtnTaskManager::RunError(TInt aError)
    {
    TRACER("CGlxtnTaskManager::RunError()");
    if ( iCurrentTask )
        {
        iCurrentTask->RunError(aError);

        if ( CGlxtnTask::ERunning == iCurrentTask->State() )
            {
            SetActive();
            }
        else
            {
            AsyncRun();
            }

        return KErrNone;
        }

    return aError;
    }

// -----------------------------------------------------------------------------
// AsyncRun
// -----------------------------------------------------------------------------
//
void CGlxtnTaskManager::AsyncRun()
    {
    TRACER("CGlxtnTaskManager::AsyncRun()");
    // Issue a self-completing request.
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

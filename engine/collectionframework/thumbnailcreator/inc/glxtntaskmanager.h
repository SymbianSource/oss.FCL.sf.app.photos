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
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef C_GLXTNTASKMANAGER_H
#define C_GLXTNTASKMANAGER_H

#include "glxtnstd.h"
#include <glxmediaid.h>
#include <e32base.h>

// Forward declarations
class CGlxtnTask;
class MGlxtnThumbnailStorage;

/**
 * Active object which maintains a priority queue of long running tasks, and
 * provides callback upon task completion or error.
 *
 * The highest priority task in the queue which has @ref CGlxtnTask::TState "state"
 * EStarting is started and the task manager's RunL then delegates to the task's
 * RunL until the task is complete, or is cancelled.  The task is then deleted
 * and another task is started, if any are ready to start.
 *
 * The task manager's DoCancel and RunError methods also delegate to the current
 * task, if any.
 *
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnTaskManager) : public CActive
    {
public:
	enum TExecutionOrder
		{
		ELastOut,	// Old tasks with the same priority get executed before the new task
		EFirstOut	// New task gets executed before other tasks with the same priority
		};
public:
    /**
     * Static constructor.
     */
    static CGlxtnTaskManager* NewL();
    
    /**
     * Destructor.
     */
    ~CGlxtnTaskManager();
    
public:
    /**
     * @returns CVieTask task at an index
     */

    inline CGlxtnTask* Task(TInt aIndex) const { return iTasks[aIndex]; };

     /**
      * Returns the highest priority task with the id.
      * If there are multiple tasks with same id and priority, 
      * returns the one of those that will be executed first 
      */
   	CGlxtnTask* Task(const TGlxtnTaskId& aId) const;

    /**
     * Adds a task to the task list.  If no task is currently running, the task
     * is started.
     *
     * @param aTask Task to add.  Takes ownership, task is deleted if a leave occurs.
     *
     * @param aExecutionOrder -if ELastOut, the tast gets added to the bottom 
     *					  	  of the queue of the tasks with the same priority 
     * 					      as the new tasks. This means that the task is 
     *						  executed after the previously added tasks (with 
     *						  same priority) have been run.
     *						  -if EFirstOut, the task gets added to the top,
     *						  and will be executed before the other tasks
     *						  with the same priority (unless more tasks added 
     *						  later, of course)
     */
    void AddTaskL(CGlxtnTask* aTask, TExecutionOrder aExecutionOrder = ELastOut);

    /**
     * Cancel all tasks relating to a given media item.
     * @param aItemId ID of media item.
     */
    void CancelTasks(const TGlxMediaId& aItemId);

    /**
     * Cancel all tasks using a given storage.
     * @param aClient Reference to thumbnail storage.
     */
    void CancelTasks(MGlxtnThumbnailStorage* aStorage);

    /**
     * Gets the total task count
     *
     * @return TInt task count
     */
    inline TInt TaskCount() const { return iTasks.Count(); };

protected:
    /**
     * Constructors
     */
    CGlxtnTaskManager();
    void ConstructL(); 

private: // From CActive
	void RunL();		
	void DoCancel();
    TInt RunError(TInt aError);

private:
    /**
     * Issue a self-completing request causing RunL to be called.
     */
    void AsyncRun();

private:
    /**
     * Task list in priority order
     */
    RPointerArray<CGlxtnTask> iTasks;
    
    /**
     * Task that is currently being run.
     */
    CGlxtnTask* iCurrentTask;
    };

#endif // C_GLXTNTASKMANAGER_H

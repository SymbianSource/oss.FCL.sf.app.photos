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
* Description:   Base classes for multi-purpose tasks.
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef C_GLXTNTASK_H
#define C_GLXTNTASK_H

#include "glxtnstd.h"
#include <glxmediaid.h>
#include <e32base.h>

// Forward declarations
class CGlxtnTaskManager;
class MGlxtnThumbnailCreatorClient;
class MGlxtnThumbnailStorage;

/**
 *  Base class for thumbnail tasks.
 *
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnTask) : public CBase
    {
    friend class CGlxtnTaskManager;
public:
    /**
    * Different state a task can go through
    */
    enum TState 
        {
        EIdle,      /**< Task is not ready to start */
        EStarting,  /**< Task is ready to start */
        ERunning,   /**< Task is currently running */
        ECanceled,  /**< Task has been cancelled, and can be deleted */
        EComplete   /**< Task has finished, and can be deleted */
        };

public:
    CGlxtnTask(const TGlxtnTaskId& aId, const TGlxMediaId& aItemId,
                MGlxtnThumbnailStorage* aStorage);
    ~CGlxtnTask();
	
    /**
     * Gets the current task specific state
     * @returns TState The task current state.
     */
    inline TState State() const;

    /**
     * Gets the type of task this is.
     * @returns TGlxtnTaskId Task type ID.
     */
    inline TGlxtnTaskId Id() const;

    /**
     * Gets the media ID of the item to which this task relates.
     * @returns TGlxMediaId Media ID.
     */
    inline TGlxMediaId ItemId() const;

    /**
     * Sets the priority of the task. By default it is set to 
     * EPriorityStandard.
     */
    inline void SetPriority(TInt aPriority);
    /**
     * Gets the task priority.
     * @returns TInt Task priority.
     */
	inline TInt Priority() const;

    /**
     * Get thumbnail storage for the task to use, if any.
     * @returns Pointer to thumbnail storage, or NULL if none.
     */
    inline MGlxtnThumbnailStorage* Storage() const;

    /**
     * Provides a pointer to the task manager.
     * @param aManager Pointer to the task manager.
     */
    virtual void SetManager(CGlxtnTaskManager* aManager);

private: // For CGlxTaskManager only
    /**
     * Starts the task 
     */
    void StartL(TRequestStatus& aStatus);

	/**
	 * Cancels the task
	 */
	void Cancel(); 
	
	/**
	 * Runs a bit of the task
	 */
	void RunL(TRequestStatus& aStatus);

	/**
	 * Handle a leave in StartL or RunL.
	 */
    void RunError(TInt aError);

protected:
    /**
     * Starts the task.
     * @param aStatus Request status for asynchronous operations
     * @returns -ETrue if the task has issued an asyncronous request.
     *			CActive::SetActive() will be called in this case and the 
     *			task state will be ERunning.
     *			-EFalse if the task has not issued a request. 
     *			SetActive will not be called and the task state will be 
     *			EComplete.
     */
    virtual TBool DoStartL(TRequestStatus& aStatus) = 0;

	/**
	 * Cancels the task.  The task's state will be set to ECanceled.
	 */
	virtual void DoCancel() = 0; 

	/**
	 * Runs a bit of the task
     * @param aStatus Request status for asynchronous operations
     * @returns -ETrue if the task has issued an asyncronous request.
     *			CActive::SetActive() will be called in this case and the 
     *			task state will be ERunning.
     *			-EFalse if the task has not issued a request. 
     *			SetActive will not be called and the task state will be 
     *			EComplete.
	 */
	virtual TBool DoRunL(TRequestStatus& aStatus) = 0;

	/**
	 * Handle a leave in StartL or RunL.
     * @param aError The error that occurred.
     * @returns -ETrue if the task has issued an asyncronous request.
     *			CActive::SetActive() will be called in this case and the 
     *			task state will be ERunning.
     *			-EFalse if the task has not issued a request. 
     *			SetActive will not be called and the task state will be 
     *			EComplete.
	 */
    virtual TBool DoRunError(TInt aError) = 0;

protected:
	/**
	 * Issues a request and sets the aStatus as KRequestPending
	 * Use this to make a synchronous operation asynchronous.
	 */
	void SelfComplete(TRequestStatus& aStatus); 
   
private:
    /** Task state */
    TState iState;

    /** Task id */
    TGlxtnTaskId iId;

    /** Media item id */
    TGlxMediaId iItemId;

    /**
     * Priority of this task. 
     * The task schedule will execute tasks based on this priority
     * When this task is run, the execution priority in CActiveScheduler
     * will be this priority. (So it is comparable to priorities of active 
     * objects in the system.)
     */
    TInt iPriority; 

    /** Thumbnail storage for the task to use (can be NULL) */
    MGlxtnThumbnailStorage* iStorage;
    };

/**
 *  Base class for thumbnail tasks which return results to a client.
 *
 * @ingroup glx_thumbnail_creator
 */
NONSHARABLE_CLASS(CGlxtnClientTask) : public CGlxtnTask
    {
protected:
    CGlxtnClientTask(const TGlxtnTaskId& aId, const TGlxMediaId& aItemId,
                MGlxtnThumbnailCreatorClient& aClient);

    /**
     * Get the client for this task.
     */
    inline MGlxtnThumbnailCreatorClient& Client();

private:
    /** Client for this task */
    MGlxtnThumbnailCreatorClient& iClient;
    };

#include "glxtntask.inl"

#endif // C_GLXTNTASK_H

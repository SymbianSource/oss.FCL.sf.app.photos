/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Thumbnail task unit tests.
*
*/



#ifndef __DUMMYTASKMANAGER_H__
#define __DUMMYTASKMANAGER_H__

// HACK: Make CDummyTaskManager a friend of CGlxtnTask
#define CDummyTaskManager CGlxtnTaskManager

//  CLASS DEFINITION

/**
* Dummy task manager to run a single task within a unit test.
*/
NONSHARABLE_CLASS( CDummyTaskManager ) : public CActive
    {
public:
    enum TExecutionOrder
        {
        ELastOut,	// Old tasks with the same priority get executed before the new task
        EFirstOut	// New task gets executed before other tasks with the same priority
        };

    CDummyTaskManager(CGlxtnTask* aTask);
    void StartTaskL();
    void AddTaskL(CGlxtnTask* aTask, TExecutionOrder aExecutionOrder = ELastOut);

private:    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    CGlxtnTask* iTask; // Not owned
    };

// ---------------------------------------------------------------------------

CDummyTaskManager::CDummyTaskManager(CGlxtnTask* aTask)
    : CActive(EPriorityStandard), iTask(aTask)
    {
    CActiveScheduler::Add(this);
    }

void CDummyTaskManager::StartTaskL()
    {
    iTask->StartL(iStatus);
    if ( iTask->State() != CGlxtnTask::EComplete ) 
        {
        SetActive();
        CActiveScheduler::Start();
        }
    }

void CDummyTaskManager::AddTaskL(CGlxtnTask* /*aTask*/, TExecutionOrder /*aExecutionOrder*/)
    {
    User::Leave(KErrNotSupported);
    }

void CDummyTaskManager::RunL()
    {
    iTask->RunL(iStatus);
    if ( iTask->State() == CGlxtnTask::EComplete ) 
        {
        CActiveScheduler::Stop();
        }
    else
        {
        SetActive();
        }
    }

void CDummyTaskManager::DoCancel()
    {
    iTask->Cancel();
    }

TInt CDummyTaskManager::RunError(TInt aError)
    {
    EUNIT_PRINT(_L("CDummyTaskManager RunError(%d)"), aError);
    iTask->RunError(aError);
    if ( CGlxtnTask::ERunning == iTask->State() )
        {
        SetActive();
        }
    else
        {
        CActiveScheduler::Stop();
        }
    return KErrNone;
    }

#endif  // __DUMMYTASKMANAGER_H__

// End of file

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
 * Description:    Request Goom to free memory in asynch way
 *
 */

#include <glxtracer.h>
#include <glxlog.h>

#include "glxrequestfreegoom.h"

const TInt KMinMemoryRequest = 16000000; // 2000 x 2000 x 4 (32 bit display mode)

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxRelaseGPUMemory* CGlxRelaseGPUMemory::NewL(
        MGoomNotifierObserver& aNotify)
    {
    TRACER("CGlxRelaseGPUMemory::NewL()");
    CGlxRelaseGPUMemory* self = CGlxRelaseGPUMemory::NewLC(aNotify);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::NewLC
// ----------------------------------------------------------------------------
//
CGlxRelaseGPUMemory* CGlxRelaseGPUMemory::NewLC(
        MGoomNotifierObserver& aNotify)
    {
    TRACER("CGlxRelaseGPUMemory::NewLC()");
    CGlxRelaseGPUMemory* self = new (ELeave) CGlxRelaseGPUMemory(aNotify);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::CGlxRelaseGPUMemory
// ----------------------------------------------------------------------------
//
CGlxRelaseGPUMemory::CGlxRelaseGPUMemory(MGoomNotifierObserver& aNotify) :
    CActive(CActive::EPriorityStandard), iNotify(aNotify), iIsFirstRequest(
            ETrue),iNotifyCaller(ETrue)
    {
    TRACER("CGlxRelaseGPUMemory::CGlxRelaseGPUMemory()");
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::~CGlxRelaseGPUMemory()
// ----------------------------------------------------------------------------
//
CGlxRelaseGPUMemory::~CGlxRelaseGPUMemory()
    {
    TRACER("CGlxRelaseGPUMemory::~CGlxRelaseGPUMemory()");
    Cancel();
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::IssueRequest()
// ----------------------------------------------------------------------------
//
void CGlxRelaseGPUMemory::IssueRequest()
    {
    TRACER("CGlxRelaseGPUMemory::IssueRequest()");
    if (!IsActive())
        {
#ifdef _DEBUG
        iStartTime.HomeTime();
#endif                          
        iGoom.RequestFreeMemory(KMinMemoryRequest, iStatus);
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::ConstructL()
// ----------------------------------------------------------------------------
//
void CGlxRelaseGPUMemory::ConstructL()
    {
    TRACER("CGlxRelaseGPUMemory::ConstructL()");
    TInt err = iGoom.Connect();
    GLX_LOG_INFO1("CGlxRelaseGPUMemory::ConstructL %d", err);
    User::LeaveIfError(err);
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::RequestMemory()
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxRelaseGPUMemory::RequestMemory(TBool aRequest)
    {
    TRACER("CGlxRelaseGPUMemory::RequestMemory()");
    iNotifyCaller = aRequest;
    IssueRequest();
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::DoCancel()
// ----------------------------------------------------------------------------
//
void CGlxRelaseGPUMemory::DoCancel()
    {
    TRACER("CGlxRelaseGPUMemory::DoCancel()");
    iGoom.CancelRequestFreeMemory();
    iGoom.Close();
    }

// ----------------------------------------------------------------------------
// CGlxRelaseGPUMemory::RunL
// ----------------------------------------------------------------------------
//
void CGlxRelaseGPUMemory::RunL()
    {
    TRACER("CGlxRelaseGPUMemory::RunL()");
    GLX_LOG_INFO1("CGlxRelaseGPUMemory::RunL %d", iStatus.Int());
#ifdef _DEBUG
    iStopTime.HomeTime();
    GLX_DEBUG2("CGlxRelaseGPUMemory: Mem free GOOM took <%d> us",
            (TInt)iStopTime.MicroSecondsFrom(iStartTime).Int64());
#endif                          

    if (iIsFirstRequest && (iStatus.Int() != KErrNone))
        {
        iIsFirstRequest = EFalse;
        IssueRequest();
        return;
        }

    // Notify observer on the RequestFreeMemory() status
    if(iNotifyCaller || (iStatus.Int() != KErrNone))
        {
    	iNotify.HandleGoomMemoryReleased(iStatus.Int());
        }
    iNotifyCaller = EFalse;
    }

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
* Description:    Task used for thumbnail availability filter.
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnfilteravailabletask.h"

#include <glxassert.h>
#include <glxtracer.h>

#include "glxtnfileinfo.h"
#include "mglxtnstorage.h"
#include "mglxtnthumbnailcreatorclient.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxtnFilterAvailableTask* CGlxtnFilterAvailableTask::NewL(
                    const TArray<TGlxMediaId>& aItemArray, const TSize& aSize,
                    MGlxtnThumbnailCreatorClient& aClient)
    {
    TRACER("CGlxtnFilterAvailableTask* CGlxtnFilterAvailableTask::NewL()");
    CGlxtnFilterAvailableTask* task = new (ELeave) CGlxtnFilterAvailableTask(
                                                                aSize, aClient);
    CleanupStack::PushL(task);
    task->ConstructL(aItemArray);
    CleanupStack::Pop(task);
    return task;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnFilterAvailableTask::CGlxtnFilterAvailableTask(const TSize& aSize,
                                        MGlxtnThumbnailCreatorClient& aClient) :
    CGlxtnClientTask(KGlxtnTaskIdFilterAvailable, KGlxIdNone, aClient),
    iSize(aSize)
    {
    TRACER("CGlxtnFilterAvailableTask::CGlxtnFilterAvailableTask()");
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxtnFilterAvailableTask::ConstructL(
                                        const TArray<TGlxMediaId>& aItemArray)
    {
    TRACER("void CGlxtnFilterAvailableTask::ConstructL()");
    TInt count = aItemArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iIdArray.AppendL(aItemArray[i]);
        }
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnFilterAvailableTask::~CGlxtnFilterAvailableTask() 
    {
    TRACER("CGlxtnFilterAvailableTask::~CGlxtnFilterAvailableTask()");
    delete iFileInfo;
    iIdArray.Close();
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnFilterAvailableTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnFilterAvailableTask::DoStartL()");
    iIndex = 0;

    GLX_ASSERT_DEBUG( !iFileInfo, Panic( EGlxPanicAlreadyInitialised ),
                        "iFileInfo not NULL" );
    iFileInfo = new (ELeave) CGlxtnFileInfo;

    return GetFileInfoL(aStatus);
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnFilterAvailableTask::DoCancel() 
    {
    TRACER("void CGlxtnFilterAvailableTask::DoCancel()");
    Storage()->StorageCancel();
    }

// -----------------------------------------------------------------------------
// DoRunL
// -----------------------------------------------------------------------------
//
TBool CGlxtnFilterAvailableTask::DoRunL(TRequestStatus& aStatus) 
    {
    TRACER("TBool CGlxtnFilterAvailableTask::DoRunL()");
    TInt result = aStatus.Int();
    User::LeaveIfError(result);

    TBool active = EFalse;

    switch ( iState )
        {
        case EStateFetchingUri:
            Storage()->IsThumbnailAvailableL(iIdArray[iIndex], *iFileInfo,
                                                iSize, &aStatus);
            iState = EStateChecking;
            active = ETrue;
            break;

        case EStateChecking:
            if ( KGlxThumbnailNotAvailable == result )
                {
                iIndex++;
                }
            else
                {
                iIdArray.Remove(iIndex);
                }

            active = GetFileInfoL(aStatus);
            break;

        default:
            GLX_ASSERT_ALWAYS( EFalse, Panic( EGlxPanicIllegalState ),
                            "CGlxtnFilterAvailableTask: Illegal state" );
            break;
        }

    return active; 
    }

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnFilterAvailableTask::DoRunError(TInt aError)
    {
    TRACER("CGlxtnFilterAvailableTask::DoRunError()");
    Client().FilterAvailableComplete(iIdArray, aError);

    return EFalse;
    }

// -----------------------------------------------------------------------------
// GetFileInfoL
// -----------------------------------------------------------------------------
//
TBool CGlxtnFilterAvailableTask::GetFileInfoL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnFilterAvailableTask::GetFileInfoL()");
    if ( iIndex < iIdArray.Count() )
        {
        Client().FetchFileInfoL(iFileInfo, iIdArray[iIndex], &aStatus);
        iState = EStateFetchingUri;

        return ETrue;
        }

    // Finished checking the array
    Client().FilterAvailableComplete(iIdArray, KErrNone);

    return EFalse;
    }

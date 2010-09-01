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
* Description:    Thumbnail storage cleanup task implementation
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

// INCLUDE FILES

#include "glxtncleanuptask.h"

#include <glxassert.h>
#include <glxtracer.h>

#include "mglxtnstorage.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlxtnCleanupTask::CGlxtnCleanupTask
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGlxtnCleanupTask::CGlxtnCleanupTask(MGlxtnThumbnailStorage* aStorage) :
    CGlxtnTask(KGlxtnTaskIdCleanupThumbnails, KGlxIdNone, aStorage)
    {
    TRACER("CGlxtnCleanupTask::CGlxtnCleanupTask(MGlxtnThumbnailStorage* aStorage)");
    GLX_ASSERT_ALWAYS( aStorage, Panic( EGlxPanicIllegalArgument ),
                        "CGlxtnCleanupTask created without storage" );

    // Background task is always low priority
    SetPriority(EPriorityLow);
    }

// -----------------------------------------------------------------------------
// CGlxtnCleanupTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxtnCleanupTask::ConstructL()
    {
    TRACER("void CGlxtnCleanupTask::ConstructL()");
    }

// -----------------------------------------------------------------------------
// CGlxtnCleanupTask::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlxtnCleanupTask* CGlxtnCleanupTask::NewL(MGlxtnThumbnailStorage* aStorage)
    {
    TRACER("CGlxtnCleanupTask* CGlxtnCleanupTask::NewL(MGlxtnThumbnailStorage* aStorage)");
    CGlxtnCleanupTask* self = new (ELeave) CGlxtnCleanupTask(aStorage);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// Destructor
CGlxtnCleanupTask::~CGlxtnCleanupTask()
    {
    TRACER("Destructor CGlxtnCleanupTask::~CGlxtnCleanupTask()");
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnCleanupTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("TBool CGlxtnCleanupTask::DoStartL(TRequestStatus& aStatus)");
    Storage()->CleanupThumbnailsL(&aStatus);

    return ETrue;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnCleanupTask::DoCancel()
    {
    TRACER("void CGlxtnCleanupTask::DoCancel()");
    Storage()->StorageCancel();
    }

// -----------------------------------------------------------------------------
// DoRunL
// -----------------------------------------------------------------------------
//
TBool CGlxtnCleanupTask::DoRunL(TRequestStatus& /*aStatus*/)
    {
    TRACER("TBool CGlxtnCleanupTask::DoRunL(TRequestStatus& /*aStatus*/)");
    return EFalse;  // Task is complete
    }

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnCleanupTask::DoRunError(TInt /*aError*/)
    {
    TRACER("TBool CGlxtnCleanupTask::DoRunError(TInt /*aError*/)");
    return EFalse;  // Task is complete
    }

//  End of File  

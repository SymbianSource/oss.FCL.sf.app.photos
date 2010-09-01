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
* Description:    Implementation of CGlxtnDeleteThumbnailsTask
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

// INCLUDE FILES

#include "glxtndeletethumbnailstask.h"

#include <glxassert.h>
#include "glxtnfileinfo.h"
#include "mglxtnstorage.h"
#include "mglxtnthumbnailcreatorclient.h"
#include <glxtracer.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGlxtnDeleteThumbnailsTask::CGlxtnDeleteThumbnailsTask(
            const TGlxMediaId& aItemId, MGlxtnThumbnailCreatorClient& aClient) :
    CGlxtnClientTask(KGlxtnTaskIdDeleteThumbnails, aItemId, aClient)
    {
    TRACER("CGlxtnDeleteThumbnailsTask::CGlxtnDeleteThumbnailsTask()");
    GLX_ASSERT_ALWAYS( Storage(), Panic( EGlxPanicIllegalArgument ),
                    "CGlxtnDeleteThumbnailsTask created without storage" );
    }

// -----------------------------------------------------------------------------
// ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxtnDeleteThumbnailsTask::ConstructL()
    {
    TRACER("void CGlxtnDeleteThumbnailsTask::ConstructL()");
    }

// -----------------------------------------------------------------------------
// NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlxtnDeleteThumbnailsTask* CGlxtnDeleteThumbnailsTask::NewL(
            const TGlxMediaId& aItemId, MGlxtnThumbnailCreatorClient& aClient)
    {
    TRACER("CGlxtnDeleteThumbnailsTask::NewL()");
    CGlxtnDeleteThumbnailsTask* self = new (ELeave) CGlxtnDeleteThumbnailsTask(
                                                            aItemId, aClient);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CGlxtnDeleteThumbnailsTask::~CGlxtnDeleteThumbnailsTask()
    {
    TRACER("Destructor CGlxtnDeleteThumbnailsTask::~CGlxtnDeleteThumbnailsTask()");
    delete iInfo;
    }

// -----------------------------------------------------------------------------
// DoStartL
// -----------------------------------------------------------------------------
//
TBool CGlxtnDeleteThumbnailsTask::DoStartL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnDeleteThumbnailsTask::DoStartL()");
    iInfo = new (ELeave) CGlxtnFileInfo;
    Client().FetchFileInfoL(iInfo, ItemId(), &aStatus);

    return ETrue;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CGlxtnDeleteThumbnailsTask::DoCancel()
    {
    TRACER("CGlxtnDeleteThumbnailsTask::DoCancel()");
    Storage()->StorageCancel();
    }

// -----------------------------------------------------------------------------
// DoRunL
// -----------------------------------------------------------------------------
//
TBool CGlxtnDeleteThumbnailsTask::DoRunL(TRequestStatus& aStatus)
    {
    TRACER("CGlxtnDeleteThumbnailsTask::DoRunL()");
    User::LeaveIfError(aStatus.Int());

    TBool active = EFalse;

    if ( iDeleting )
        {
        Client().ThumbnailDeletionComplete(ItemId(), KErrNone);
        }
    else
        {
        User::LeaveIfNull(iInfo);
        Storage()->DeleteThumbnailsL(ItemId(), *iInfo, &aStatus);
        iDeleting = ETrue;
        active = ETrue;
        }

    return active;
    }

// -----------------------------------------------------------------------------
// DoRunError
// -----------------------------------------------------------------------------
//
TBool CGlxtnDeleteThumbnailsTask::DoRunError(TInt aError)
    {
    TRACER("CGlxtnDeleteThumbnailsTask::DoRunError()");
    Client().ThumbnailDeletionComplete(ItemId(), aError);

    return EFalse;
    }

//  End of File

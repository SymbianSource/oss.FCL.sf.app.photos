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
* Description:    Upload cenrep watcher
*
*/

//  CLASS HEADER
#include <centralrepository.h>              // for checking the ShareOnline version
#include "glxcommandhandlerupload.h"
#include "glxuploadcenrepwatcher.h"

#include <glxtracer.h>                      // for TRACER logs


// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
CGlxUploadCenRepWatcher* CGlxUploadCenRepWatcher::NewL
                                        (MGlxUploadIconObserver& aUploadIconObserver,
                                         const TUid& aRepositoryUid,
                                         TUint32 aId )
    {
    TRACER("CGlxUploadCenRepWatcher::NewL");    
                
    CGlxUploadCenRepWatcher* self = 
        new(ELeave) CGlxUploadCenRepWatcher(aUploadIconObserver, aRepositoryUid, aId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// constructor.
// -----------------------------------------------------------------------------
inline CGlxUploadCenRepWatcher::CGlxUploadCenRepWatcher(
        MGlxUploadIconObserver& aUploadIconObserver,
        const TUid& aRepositoryUid, TUint32 aId )
    : CActive(EPriorityStandard),
    iUploadIconObserver(aUploadIconObserver),  
    iRepositoryUid(aRepositoryUid), 
    iId(aId)
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor 
// ----------------------------------------------------------------------------
void CGlxUploadCenRepWatcher::ConstructL()
    {
    TRACER("CGlxUploadCenRepWatcher::ConstructL");
    CActiveScheduler::Add( this );
    iRepository = CRepository::NewL(iRepositoryUid);
    
    //Request notification when the user changes cenrep values
    TInt err = iRepository->NotifyRequest( iId, iStatus );
    if (!(KErrNone ==err ||KErrAlreadyExists  ==err))
        {        
        User::Leave(err);
        }
    
    SetActive();
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CGlxUploadCenRepWatcher::~CGlxUploadCenRepWatcher()
    {
    TRACER("CGlxUploadCenRepWatcher::~CGlxUploadCenRepWatcher");
    
    Cancel();
    delete iRepository;
    }

    
// ----------------------------------------------------------------------------
// CGlxUploadCenRepWatcher::KeyValueL
// ----------------------------------------------------------------------------
void CGlxUploadCenRepWatcher::KeyValueL(TDes& aKeyValue) const
    {
    TRACER("CGlxUploadCenRepWatcher::KeyValueL");
    if(iRepository)
        {
        User::LeaveIfError( iRepository->Get(iId, aKeyValue) );
        }
    }


// ----------------------------------------------------------------------------
// CGlxUploadCenRepWatcher::RunL
// ----------------------------------------------------------------------------
void CGlxUploadCenRepWatcher::RunL()
    {
    TRACER("CGlxUploadCenRepWatcher::RunL");    
    // Check for errors
    User::LeaveIfError( iStatus.Int() );    
    User::LeaveIfError( iRepository->NotifyRequest( iId, iStatus ) );
    if (!IsActive())
        {
        SetActive();
        iUploadIconObserver.HandleUploadIconChangedL( );
        }
    }

// ----------------------------------------------------------------------------
// CGlxUploadCenRepWatcher::DoCancel
// ----------------------------------------------------------------------------
void CGlxUploadCenRepWatcher::DoCancel()
    {
    TRACER("CGlxUploadCenRepWatcher::DoCancel");    
    iRepository->NotifyCancel( iId );
    }

//-----------------------------------------------------------------------------
// CGlxUploadCenRepWatcher::RunError
// From class CActive.
//-----------------------------------------------------------------------------
//
TInt CGlxUploadCenRepWatcher::RunError( TInt /*aError*/ )
    {
    TRACER("CGlxUploadCenRepWatcher::RunError()");
    return KErrNone;
    }

// End of file


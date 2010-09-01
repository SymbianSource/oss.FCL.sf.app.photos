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
* Description:    Updation via IAD
*
*/



// INCLUDE FILES
#include "glxiadupdate.h"
#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <featmgr.h>
#include <glxtracer.h>
#include <glxlog.h>



// CONSTANTS
const TUid KIADParamUid = { 0x200009EE }; // Uid of glxgallary.sis
_LIT( KIADParamExec, "glx.exe" );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CGlxIadUpdate::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CGlxIadUpdate* CGlxIadUpdate::NewL()
    {
    TRACER( "CGlxIadUpdate::NewL()" );
    CGlxIadUpdate* self = new( ELeave ) CGlxIadUpdate();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CGlxIadUpdate::CGlxIadUpdate
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//
CGlxIadUpdate::CGlxIadUpdate()
  : iUpdate( NULL ), iParameters( NULL )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CGlxIadUpdate::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxIadUpdate::ConstructL()
    {
    TRACER( "CGlxIadUpdate::ConstructL()" );

    if( FeatureManager::FeatureSupported( KFeatureIdIAUpdate ) )
        {
        iUpdate = CIAUpdate::NewL( *this );
        iParameters = CIAUpdateParameters::NewL();
        }
    else
        {
        GLX_LOG_INFO( "CGlxIadUpdate::ConstructL(), Feature not supported" );
        }
    }

// --------------------------------------------------------------------------
// CGlxIadUpdate::~CGlxIadUpdate
// Destructor
// --------------------------------------------------------------------------
//
CGlxIadUpdate::~CGlxIadUpdate()
    {
    TRACER( "CGlxIadUpdate::~CGlxIadUpdate()" );
    ReleaseIADUpdate();
    }

// --------------------------------------------------------------------------
// CGlxIadUpdate::StartL
// Starts update process.
// --------------------------------------------------------------------------
//
EXPORT_C void CGlxIadUpdate::StartL()
    {
    TRACER( "CGlxIadUpdate::StartL()" );

    if( iUpdate && iParameters )
        {
        iParameters->SetUid( KIADParamUid );
        // We want VC to be started after update is finished
        iParameters->SetCommandLineExecutableL( KIADParamExec );
        iParameters->SetShowProgress( EFalse );
    
        // Check the updates
        iUpdate->CheckUpdates( *iParameters );
        }
    }

// -----------------------------------------------------------------------------
// CGlxIadUpdate::CheckUpdatesComplete
// -----------------------------------------------------------------------------
//  
void CGlxIadUpdate::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    TRACER( "CGlxIadUpdate::CheckUpdatesComplete()" );
    GLX_LOG_INFO2("CGlxIadUpdate::CheckUpdatesComplete(), aErrorCode: %d, aAvailableUpdates: %d",aErrorCode, aAvailableUpdates );
    if ( aErrorCode == KErrNone )
        {
        if ( aAvailableUpdates > 0 )
            {
            // There were some updates available.
            iUpdate->UpdateQuery();         
            }
        else
            {
            // No updates available.
            ReleaseIADUpdate();
            }
        }
    }

// -----------------------------------------------------------------------------
// CGlxIadUpdate::UpdateComplete
// -----------------------------------------------------------------------------
//  
void CGlxIadUpdate::UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResult )
    {
    TRACER( "CGlxIadUpdate::UpdateComplete()" );
    GLX_LOG_INFO2("CGlxIadUpdate::UpdateComplete(), aErrorCode: %d, SuccessCount: %d",aErrorCode, aResult->SuccessCount());
    if ( aErrorCode == KErrNone )
        {
        // The update process that the user started from IAUpdate UI is now completed.
        // If the client application itself was updated in the update process, this callback
        // is never called, since the client is not running anymore. 
        GLX_LOG_INFO1("UpdateComplete(), SuccessCount: %d", aResult->SuccessCount());
        }
    
    delete aResult; // Ownership was transferred, so this must be deleted by the client

    // We do not need the client-server session anymore
    ReleaseIADUpdate();
    }

// -----------------------------------------------------------------------------
// CGlxIadUpdate::UpdateQueryComplete
// -----------------------------------------------------------------------------
//  
void CGlxIadUpdate::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {    
    TRACER( "CGlxIadUpdate::UpdateQueryComplete()" );
    GLX_LOG_INFO2("CGlxIadUpdate::UpdateComplete(), aErrorCode: %d, aUpdateNow: %d",aErrorCode, aUpdateNow);
    if ( aErrorCode == KErrNone )
        {
        if ( aUpdateNow )
            {
            // User choosed to update now, so let's launch the IAUpdate UI.
            iUpdate->ShowUpdates( *iParameters );
            }
        else
            {
            // The answer was 'Later'. 
            ReleaseIADUpdate();
            }
        }
    }

// -----------------------------------------------------------------------------
// CGlxIadUpdate::ReleaseIADUpdate
// -----------------------------------------------------------------------------
//  
void CGlxIadUpdate::ReleaseIADUpdate()
    {
    TRACER( "CGlxIadUpdate::ReleaseIADUpdate()" );
    
    delete iUpdate; 
    iUpdate = NULL;
    
    delete iParameters; 
    iParameters = NULL;                
    }

//  End of File


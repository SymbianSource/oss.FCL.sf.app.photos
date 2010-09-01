/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Telephone call handler
*
*/



#include "shwslideshowtelephonehandler.h"
#include "shwslideshowengine.h"
#include "shwslideshowpausehandler.h"

#include <glxlog.h>
#include <glxtracer.h>
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CShwTelephoneHandler* CShwTelephoneHandler::NewL(
    CShwSlideshowEngine& aEngine, 
    CShwSlideShowPauseHandler& aPauseHandler )
    {
    TRACER("CShwTelephoneHandler::NewL");
    GLX_LOG_INFO( "CShwTelephoneHandler::NewL" );
    CShwTelephoneHandler* self =
        new( ELeave) CShwTelephoneHandler( aEngine, aPauseHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CShwTelephoneHandler::~CShwTelephoneHandler()
    {
    TRACER("CShwTelephoneHandler::~CShwTelephoneHandler");
    GLX_LOG_INFO( "CShwTelephoneHandler::~CShwTelephoneHandler" );
    Cancel();
    delete iTelephony;
    }

// -----------------------------------------------------------------------------
// StartL
// -----------------------------------------------------------------------------
//
void CShwTelephoneHandler::StartL()
    {
    TRACER("CShwTelephoneHandler::StartL");
    GLX_LOG_INFO( "CShwTelephoneHandler::StartL" );
    if ( !IsActive() )
        {
        // request notification of change in status of the voice line
        iTelephony->NotifyChange( iStatus, CTelephony::EVoiceLineStatusChange,
            iLineStatusPckg );
        // get on with it
        SetActive();
        }
    }	

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CShwTelephoneHandler::CShwTelephoneHandler(
            CShwSlideshowEngine& aEngine, 
            CShwSlideShowPauseHandler& aPauseHandler )
        : CActive( CActive::EPriorityStandard ), 
        iEngine( aEngine ),
        iPauseHandler( aPauseHandler ),
        iLineStatusPckg( iLineStatus )
    {
    TRACER("CShwTelephoneHandler::CShwTelephoneHandler");
    GLX_LOG_INFO( "CShwTelephoneHandler::CShwTelephoneHandler" );
    CActiveScheduler::Add( this );
    // set initial line status
    iLineStatus.iStatus = CTelephony::EStatusUnknown;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CShwTelephoneHandler::ConstructL()
    {
    TRACER("CShwTelephoneHandler::ConstructL");
    GLX_LOG_INFO( "CShwTelephoneHandler::ConstructL" );
    iTelephony = CTelephony::NewL();
    }
    
// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CShwTelephoneHandler::RunL()
    {
    TRACER("CShwTelephoneHandler::RunL");
    GLX_LOG_INFO1( "CShwTelephoneHandler::RunL, status = %d", iStatus.Int() );

    if( iStatus == KErrNone )
        {
        // No error
        GLX_LOG_INFO1( "Line status = %d", iLineStatus.iStatus );
        // Check the line's status
        if ( iLineStatus.iStatus == CTelephony::EStatusAnswering ||
    		 iLineStatus.iStatus == CTelephony::EStatusConnecting ||
    		 iLineStatus.iStatus == CTelephony::EStatusConnected )
            {
            iPauseHandler.PhoneCallStartedL();
            }
        else if ( iLineStatus.iStatus == CTelephony::EStatusIdle )
            {
            iPauseHandler.PhoneCallEndedL();
            }

        // reissue request for notification
        iTelephony->NotifyChange( iStatus, CTelephony::EVoiceLineStatusChange,
            iLineStatusPckg );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CShwTelephoneHandler::DoCancel()
    {
    TRACER("CShwTelephoneHandler::DoCancel");
    GLX_LOG_INFO( "CShwTelephoneHandler::DoCancel" );
    // Cancel the voice line status requests
    if ( iTelephony )
        {
        // The return code is ignored as there's no way of reporting it.
        iTelephony->CancelAsync( CTelephony::EVoiceLineStatusChangeCancel );
        }
    }
// End of file

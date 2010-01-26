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
* Description:    Monitors the Tv Out Connection
*
*/



/**
 * @internal reviewed 24/08/2007 by D Holland
 */

//  CLASS HEADER
#include "glxtvconnectionmonitor.h"

//  EXTERNAL INCLUDES

//  INTERNAL INCLUDES

#include <glxlog.h>
#include <glxpanic.h>
#include "glxtv.h"


//-----------------------------------------------------------------------------
// Return new object
//-----------------------------------------------------------------------------
//
CGlxTvConnectionMonitor* CGlxTvConnectionMonitor::NewL(
                               MGlxTvConnectionObserver& aConnectionObserver ) 
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::NewL");
    CGlxTvConnectionMonitor* self = new (ELeave) 
                CGlxTvConnectionMonitor( aConnectionObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
//
CGlxTvConnectionMonitor::~CGlxTvConnectionMonitor()
    {
    GLX_LOG_INFO("~CGlxTvConnectionMonitor");
    Cancel();
    iTvAccServer.Disconnect();
    }


//-----------------------------------------------------------------------------
// Default C++ constructor
//-----------------------------------------------------------------------------
//
CGlxTvConnectionMonitor::CGlxTvConnectionMonitor
                           ( MGlxTvConnectionObserver& aConnectionObserver ) 
                            :CActive(EPriorityStandard), 
                             iConnectionObserver ( aConnectionObserver )
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor");
    CActiveScheduler::Add( this );
    }


//-----------------------------------------------------------------------------
// Symbian second phase constructor
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::ConstructL()
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::ConstructL");
    User::LeaveIfError( iTvAccServer.Connect() );
    User::LeaveIfError( iTvAccMode.CreateSubSession( iTvAccServer ) );
    User::LeaveIfError( iTvAccMode.GetAccessoryMode( iCurrentAccMode ) );
    iConnectionState = ( iCurrentAccMode.iAccessoryMode == EAccModeTVOut 
                            || iCurrentAccMode.iAccessoryMode == EAccModeHDMI);
    IssueRequest();  
    }

//-----------------------------------------------------------------------------
// From class CActive.
// Receive notification of change in the connection state
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::RunL()
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::RunL");
    // Check for errors
    User::LeaveIfError( iStatus.Int() );
    // Notify observers
    IssueNotificationL();
    // Request the next event
    IssueRequest();  
    }


//-----------------------------------------------------------------------------
// From class CActive.
// DoCancel
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::DoCancel()
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::DoCancel");
    iTvAccMode.CancelNotifyAccessoryModeChanged();
    }



//-----------------------------------------------------------------------------
// From class CActive.
// RunError
//-----------------------------------------------------------------------------
//
TInt CGlxTvConnectionMonitor::RunError(TInt aError)
    {
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::RunError - %d", aError);
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// Is the TV out cable connected
// IsConnected
//-----------------------------------------------------------------------------
//
TBool CGlxTvConnectionMonitor::IsConnected() const
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::IsConnected");
    return iConnectionState;
    }


//-----------------------------------------------------------------------------
// Request accessory server events
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::IssueRequest()
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::IssueRequest");
    if (!IsActive()) // required for testing
        {
        iTvAccMode.NotifyAccessoryModeChanged( iStatus, iCurrentAccMode ); 
        SetActive(); 
        }
    }


//-----------------------------------------------------------------------------
// Sends notification to observers if TV Out is connected
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::IssueNotificationL()
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::IssueNotificationL");
    iConnectionState = ( iCurrentAccMode.iAccessoryMode == EAccModeTVOut || 
            iCurrentAccMode.iAccessoryMode == EAccModeHDMI );
    iConnectionObserver.HandleTvConnectionStatusChangedL();
    }





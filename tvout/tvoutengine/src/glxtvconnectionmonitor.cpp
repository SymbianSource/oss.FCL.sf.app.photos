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
#include <AccPolGenericIdDefinitions.h>
#include <AccPolSubblockNameArray.h>
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
    iTvAccCon.CloseSubSession();
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
    User::LeaveIfError(iTvAccServer.Connect());
	User::LeaveIfError(iTvAccCon.CreateSubSession(iTvAccServer));
	User::LeaveIfError(iTvAccCon.GetAccessoryConnectionStatus(iCurrentAccArray));
	for (int i = 0; i < iCurrentAccArray.Count(); i++)
		{
		iTvOutConnectionState
				= (iCurrentAccArray.GetGenericIDL(i).PhysicalConnectionCaps()
						== KPCWired);
		iHDMIConnectionState
				= (iCurrentAccArray.GetGenericIDL(i).PhysicalConnectionCaps()
						== KPCHDMI);
		}
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
    iTvAccCon.CancelGetAccessoryConnectionStatus();
    iTvAccCon.CancelNotifyAccessoryConnectionStatusChanged();
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
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::IsConnected iTvOutConnectionState=%d",iTvOutConnectionState);
    return iTvOutConnectionState;
    }

//-----------------------------------------------------------------------------
// Is the TV out cable connected
// IsConnected
//-----------------------------------------------------------------------------
//
TBool CGlxTvConnectionMonitor::IsHDMIConnected() const
    {
    GLX_LOG_INFO("CGlxTvConnectionMonitor::IsHDMIConnected");
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::IsHDMIConnected iHDMIConnectionState=%d",iHDMIConnectionState);
    return iHDMIConnectionState;
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
        iTvAccCon.NotifyAccessoryConnectionStatusChanged(iStatus,iCurrentAccArray);
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
	TBool previousTvState = iTvOutConnectionState;
	TBool previousHDMIState = iHDMIConnectionState;
	GLX_LOG_INFO2("previousTvState = %d , previousHDMIState = %d",
			previousTvState,previousHDMIState);

	//gets the TV status in to the iCurrentAccArray and haves the Latest Accesory in 0-index
	User::LeaveIfError(iTvAccCon.GetAccessoryConnectionStatus(iCurrentAccArray));

	CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
	CleanupStack::PushL(nameArray);

	//
	//  Get the number of currently connected accessories
	//  Loop through each connected accessory looking for
	//  the desired capabilities
	//
	iTvOutConnectionState = EFalse;
	iHDMIConnectionState = EFalse;
	const TInt count = iCurrentAccArray.Count();
	for (TInt index = 0; index < count; index++)
		{
		TAccPolGenericID genId = iCurrentAccArray.GetGenericIDL(index);
		//  Get all supported capabilities for this connected accessory.
		iTvAccCon.GetSubblockNameArrayL(genId, *nameArray);
		//  Check if this connected accessory supports TV-Out capabality.
		if (nameArray->HasName(KAccVideoOut))
			{
			GLX_LOG_INFO("TV-Out Capabilities Exist");
			TAccPolNameRecord nameRecord;
			nameRecord.SetNameL(KAccVideoOut);
			TAccValueTypeTInt value;
			iTvAccCon.GetValueL(genId, nameRecord, value);
			if (value.iValue == EAccVideoOutEHDMI)
				{
				GLX_LOG_INFO("HDMI is connected Capabilities Exist");
				iHDMIConnectionState = ETrue;
				break;
				}
			else if (value.iValue == EAccVideoOutCVideo)
				{
				GLX_LOG_INFO("TV is connected Capabilities Exist");
				iTvOutConnectionState = ETrue;
				break;
				}
			}

		}
	CleanupStack::PopAndDestroy(nameArray);

	if (previousTvState != iTvOutConnectionState || previousHDMIState
			!= iHDMIConnectionState)
        {
        GLX_LOG_INFO("CGlxTvConnectionMonitor::IssueNotificationL -"
                " TvConnectionStatusChanged");
        GLX_LOG_INFO2("CurrentTvState = %d , CurrentHDMIState = %d",
                iTvOutConnectionState,iHDMIConnectionState);
        iConnectionObserver.HandleTvConnectionStatusChangedL();
        }
    }





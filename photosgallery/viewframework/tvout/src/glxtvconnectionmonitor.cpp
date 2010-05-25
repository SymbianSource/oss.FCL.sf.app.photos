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
#include <glxtracer.h>
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
    TRACER("CGlxTvConnectionMonitor::NewL");
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
    TRACER("~CGlxTvConnectionMonitor");
    Cancel();
    iTvAccCon.CloseSubSession();
    iTvAccServer.Disconnect();
    }


//-----------------------------------------------------------------------------
// Default C++ constructor
//-----------------------------------------------------------------------------
//
CGlxTvConnectionMonitor::CGlxTvConnectionMonitor(
		MGlxTvConnectionObserver& aConnectionObserver) :
	CActive(EPriorityStandard), iConnectionObserver(aConnectionObserver),
			iTvOutConnectionState(EFalse), iHDMIConnectionState(EFalse),
			iHeadSetConnectionState(EFalse)

	{
	TRACER("CGlxTvConnectionMonitor");
	CActiveScheduler::Add(this);
	}


//-----------------------------------------------------------------------------
// Symbian second phase constructor
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::ConstructL()
    {
    TRACER("CGlxTvConnectionMonitor::ConstructL");
    User::LeaveIfError(iTvAccServer.Connect());
	User::LeaveIfError(iTvAccCon.CreateSubSession(iTvAccServer));

	UpdateConnectionStatusL();
	IssueRequest();  
    }

//-----------------------------------------------------------------------------
// From class CActive.
// Receive notification of change in the connection state
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::RunL()
    {
    TRACER("CGlxTvConnectionMonitor::RunL");
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
    TRACER("CGlxTvConnectionMonitor::DoCancel");
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
    TRACER("CGlxTvConnectionMonitor::IsConnected");
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
    TRACER("CGlxTvConnectionMonitor::IsHDMIConnected");
    GLX_LOG_INFO1("CGlxTvConnectionMonitor::IsHDMIConnected iHDMIConnectionState=%d",iHDMIConnectionState);
    return iHDMIConnectionState;
    }

//-----------------------------------------------------------------------------
// Request accessory server events
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::IssueRequest()
    {
    TRACER("CGlxTvConnectionMonitor::IssueRequest");
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
	TRACER("CGlxTvConnectionMonitor::IssueNotificationL");
	TBool previousTvState = iTvOutConnectionState;
	TBool previousHDMIState = iHDMIConnectionState;
	TBool previousHeadSetState = iHeadSetConnectionState;
	TTvChangeType changeType = EDisconnected;
	
	// Update the connection status
	UpdateConnectionStatusL();

	// trigger tvstatus change only when there is actually a change the in the connection and not 
	// for spurious events
	if (previousTvState != iTvOutConnectionState)
		{
		changeType = iTvOutConnectionState == EFalse ? EDisconnected : ETvConnectionChanged;
		GLX_LOG_INFO1("CGlxTvConnectionMonitor::IssueNotificationL - TVOut Connection Changed %d", changeType);
		iConnectionObserver.HandleTvConnectionStatusChangedL(changeType);	
		}
	else if (previousHDMIState != iHDMIConnectionState)
		{
		changeType = iHDMIConnectionState == EFalse ? EDisconnected : ETvConnectionChanged;
		GLX_LOG_INFO1("CGlxTvConnectionMonitor::IssueNotificationL - HDMI Connection Changed %d", changeType);
		iConnectionObserver.HandleTvConnectionStatusChangedL(changeType);	
		}
	else if (previousHeadSetState != iHeadSetConnectionState)
		{
		changeType = iHeadSetConnectionState == EFalse ? EDisconnected : ETvConnectionChanged;
		GLX_LOG_INFO1("CGlxTvConnectionMonitor::IssueNotificationL - Headset Connection Changed %d", changeType);
		iConnectionObserver.HandleTvConnectionStatusChangedL(changeType);	
		}
	}

//-----------------------------------------------------------------------------
// UpdateConnectionStatusL()
//-----------------------------------------------------------------------------
//
void CGlxTvConnectionMonitor::UpdateConnectionStatusL()
    {
	TRACER("CGlxTvConnectionMonitor::UpdateConnectionStatusL()");

	GLX_LOG_INFO2("CGlxTvConnectionMonitor::UpdateConnectionStatusL previousTvState = %d , previousHDMIState = %d",
			iTvOutConnectionState,iHDMIConnectionState);

	// reset the states
	iHDMIConnectionState = EFalse;
	iTvOutConnectionState = EFalse;
	iHeadSetConnectionState = EFalse;

	//gets the TV status in to the iCurrentAccArray and haves the Latest Accesory in 0-index
	User::LeaveIfError(iTvAccCon.GetAccessoryConnectionStatus(iCurrentAccArray));

	CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
	CleanupStack::PushL(nameArray);

	//  Get the number of currently connected accessories
	//  Loop through each connected accessory looking for
	//  the desired capabilities
	//
	const TInt count = iCurrentAccArray.Count();
	GLX_LOG_INFO1("CGlxTvConnectionMonitor::UpdateConnectionStatusL current count value is %d",count);
	for (TInt index = 0; index < count; index++)
		{
		const TAccPolGenericID genId = iCurrentAccArray.GetGenericIDL(index);
		//  Check if this connected accessory supports TV-Out capabality.
		GLX_LOG_INFO1("CGlxTvConnectionMonitor::UpdateConnectionStatusL current index value is %d",index);
		GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL Video Accessory Supported");
		TAccPolNameRecord nameRecord;
		nameRecord.SetNameL(KAccVideoOut);

		if (genId.DeviceTypeCaps(KDTAVDevice) && genId.PhysicalConnectionCaps(
				KPCHDMI))
			{
			TAccValueTypeTInt value;
			TInt err = iTvAccCon.GetValueL(genId, nameRecord, value);
			if (err == KErrNone)
				{
				if (value.iValue == EAccVideoOutEHDMI)
					{
					GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL HDMI is connected");
					iHDMIConnectionState = ETrue;
					}
				}
			}
		else if (genId.DeviceTypeCaps(KDTAVDevice)
				&& genId.PhysicalConnectionCaps(KPCWired))
			{
			TAccValueTypeTInt value;
			TInt err = iTvAccCon.GetValueL(genId, nameRecord, value);
			if (err == KErrNone)
				{
				if (value.iValue == EAccVideoOutCVideo)
					{
					GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL Tv is connected");
					iTvOutConnectionState = ETrue;
					}
				}
			}
		else if (genId.DeviceTypeCaps(KDTHeadset)
				&& genId.PhysicalConnectionCaps(KPCWired))
			{
			GLX_LOG_INFO("CGlxTvConnectionMonitor::UpdateConnectionStatusL HeadSet connect");
			iHeadSetConnectionState = ETrue;
			}
		}
	CleanupStack::PopAndDestroy(nameArray);

	GLX_LOG_INFO2("CGlxTvConnectionMonitor::UpdateConnectionStatusL CurrentTvState = %d , CurrentHDMIState = %d",
			iTvOutConnectionState,iHDMIConnectionState);
	}

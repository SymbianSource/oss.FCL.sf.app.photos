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
* Description:    The pause handling control for the slideshow
 *
*/



#include <glxlog.h>
#include <glxtracer.h>
#include "shwviewtimer.h"
#include "shwtickobserver.h"

//for converting values in second to microsecond
const TInt KMicroMultiplier = 1000000;
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
CShwTicker* CShwTicker::NewL(MShwTickObserver& aObserver)
	{
	TRACER("CShwTicker::NewL");
	GLX_LOG_INFO("CShwTicker::NewL");
	CShwTicker* self = new(ELeave) CShwTicker(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//	
void CShwTicker::StartTicking(TInt aAfter)
	{
	TRACER("CShwTicker::StartTicking");
	GLX_LOG_INFO("CShwTicker::StartTicking");
	Cancel();
	iTimer.After( iStatus,(aAfter * KMicroMultiplier) );
	SetActive();
	}
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
void CShwTicker::CancelTicking()
	{
	TRACER("CShwTicker::CancelTicking");
	GLX_LOG_INFO("CShwTicker::CancelTicking");
	Cancel();
	iObserver.HandleTickCancelled();
	}
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//	
void CShwTicker::RunL()
	{
	TRACER("CShwTicker::RunL");
	GLX_LOG_INFO("CShwTicker::RunL");
	iObserver.HandleTickL();
	}
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
void CShwTicker::DoCancel()
	{
	TRACER("CShwTicker::DoCancel");
	GLX_LOG_INFO("CShwTicker::DoCancel");
	//Cancel the RTimer
	iTimer.Cancel();
	}
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
void CShwTicker::ConstructL()
	{
	TRACER("CShwTicker::ConstructL");
	GLX_LOG_INFO("CShwTicker::ConstructL");
	iTimer.CreateLocal();
	}
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
CShwTicker::CShwTicker(MShwTickObserver& aObserver) : CActive(EPriorityStandard) , 
						iObserver(aObserver)
	
	{
	TRACER("CShwTicker::CShwTicker");
	GLX_LOG_INFO("CShwTicker::CShwTicker");
	CActiveScheduler::Add(this);
	}
	
// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
CShwTicker::~CShwTicker()
	{
	TRACER("CShwTicker::~CShwTicker");
	GLX_LOG_INFO("CShwTicker::~CShwTicker");
	Cancel();
	iTimer.Close();
	}

/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "glxtnmonitor.h"
#include <glxlog.h>

//#include <ScreenSaverInternalPSKeys.h>
#include <e32debug.h>

const TUid KTAGDPSNotification = { 0x2001FD51 };
//const TInt KForceBackgroundGeneration = 0x00000010;
const TInt KItemsleft = 0x00000008;

CGlxTNMonitor::CGlxTNMonitor( MGlxTNObserver *observer )
: CActive( CActive::EPriorityHigh ),
  iObserver( observer )
{    
}

// ---------------------------------------------------------------------------
// ~CGlxPauseMonitor.
// Destructor.
// ---------------------------------------------------------------------------
//
CGlxTNMonitor::~CGlxTNMonitor()
{
	Cancel();
	iScreenSaverProperty.Close();
}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxTNMonitor* CGlxTNMonitor::NewL( MGlxTNObserver *observer  )
{
    //GLX_LOG_ENTRY_EXIT("CGlxTNMonitor::NewL");    
    CGlxTNMonitor* self = new (ELeave) CGlxTNMonitor( observer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// ---------------------------------------------------------------------------
// ConstructL.
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CGlxTNMonitor::ConstructL()
{
    iScreenSaverProperty.Attach( KTAGDPSNotification, KItemsleft );
	CActiveScheduler::Add( this );
    RunL();
}

void CGlxTNMonitor::RunL()
{	
	GLX_LOG_ENTRY_EXIT("CGlxTNMonitor::RunL");    
    // resubscribe before processing new value to prevent missing updates
	iScreenSaverProperty.Subscribe( iStatus );
    SetActive();
	//User::WaitForRequest(iStatus);
    TInt intValue;
	TInt err = iScreenSaverProperty.Get( intValue ) ;
	if ( err != KErrNotFound ){
	    iObserver->updateTNCount( intValue );
	}
}
    
// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CGlxTNMonitor::DoCancel()
{
	iScreenSaverProperty.Cancel();
}

// ---------------------------------------------------------------------------
// RunError
// ---------------------------------------------------------------------------
//
TInt CGlxTNMonitor::RunError( TInt )
{
    GLX_LOG_ENTRY_EXIT("CGlxTNMonitor::RunError");       
    return KErrNone;
}



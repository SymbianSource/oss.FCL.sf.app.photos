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
* Description:    Key monitoring control
*
*/





#include "glxkeymonitor.h"
#include <alf/alfevent.h>
#include "glxviewbase.h"
#include "glxkeyeventreceiver.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxKeyMonitor::CGlxKeyMonitor()
	: CAlfControl()
	{
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxKeyMonitor::~CGlxKeyMonitor()
	{
	}

// -----------------------------------------------------------------------------
// OfferEventL
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxKeyMonitor::OfferEventL(const TAlfEvent &aEvent)
	{
	TBool consumed = EFalse;

	if ( aEvent.IsKeyEvent() && iEventReceiver )
		{
		TKeyResponse resp = iEventReceiver->OfferKeyEventL(aEvent.KeyEvent(), aEvent.Code());
		consumed = (resp == EKeyWasConsumed);
		}
		
	return consumed;
	}
	
// -----------------------------------------------------------------------------
// SetView
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxKeyMonitor::SetReceiver(MGlxKeyEventReceiver* aEventReceiver)
	{
	iEventReceiver = aEventReceiver;
	}

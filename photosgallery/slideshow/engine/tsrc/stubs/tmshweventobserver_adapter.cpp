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
* Description:   Stub file for observer of event
 *
*/




//  CLASS HEADER
#include "tmshweventobserver_adapter.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
// declate the enum to be printed as TInt
EUNIT_DECLARE_PRINTABLE_AS_TINT( MMShwEventObserver_Observer::TMShwEventObserverMethodId )

//  INTERNAL INCLUDES

// CONSTRUCTION

TMShwEventObserver_Adapter::TMShwEventObserver_Adapter( MMShwEventObserver_Observer* aObserver, MShwEventObserver* aAdaptee )
	: iMMShwEventObserver_Observer( aObserver ),
		iMShwEventObserver( aAdaptee )
	{
	}

void TMShwEventObserver_Adapter::NotifyL( MShwEvent* aEvent )
	{
	// inform the test case
	iMMShwEventObserver_Observer->MShwEventObserver_MethodCalled( MMShwEventObserver_Observer::E_void_NotifyL, aEvent );
	// call the actual method
	if ( iMShwEventObserver )
		{
		iMShwEventObserver->NotifyL( aEvent );
		}
	}

//  END OF FILE

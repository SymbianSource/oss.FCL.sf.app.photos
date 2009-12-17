/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The slideshow settings dialog ecom plugin
 *
*/




//  EXTERNAL INCLUDES
#include <ecom/implementationproxy.h>

//  INTERNAL INCLUDES
#include "shwslideshowsettingsplugin_UID.hrh"
#include "shwslideshowsettingsplugin.h"



#if (!defined IMPLEMENTATION_PROXY_ENTRY)
typedef TAny* TProxyNewLPtr;
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr) {{aUid}, (TProxyNewLPtr)(aFuncPtr)}
#endif



// ========================== OTHER EXPORTED FUNCTIONS =========================

//-----------------------------------------------------------------------------
// ImplementationTable
// Define the interface UIDs
//-----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KShwSettingsDlgImplementationUid,
                                CShwSlideshowSettingsPlugin::NewL )
    };

//-----------------------------------------------------------------------------
// ImplementationGroupProxy
// The one and only exported function that is the ECom entry point
//-----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (TInt& aTableCount)
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

//  End of File



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
* Description:    Renderer Status
*
*/





#ifndef __GLXUPNPTDEFS_H__
#define __GLXUPNPTDEFS_H__

#include <e32std.h>

/**
* Status of the Renderer
*/

namespace NGlxUpnpRenderer
    {
    //UPnP availabilty Status
    enum TStatus
        {
        ENotAvailable,		//The access point is not defined
        EActive,			//The UPnP show is active
        EAvailableNotActive //The access point is defined and show is not active
        };
	//UPnP Show Error status        
	enum TErrorStatus
 		{
 		EUpnpKErrNone,			
 		EUpnpKErrDisconnected	
 		};        
    }
        
#endif
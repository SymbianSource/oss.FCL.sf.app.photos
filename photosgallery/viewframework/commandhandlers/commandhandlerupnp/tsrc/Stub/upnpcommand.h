/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Upnp Command State information
*
*/



#ifndef __C_GLXUPNPCOMMAND_H__
#define __C_GLXUPNPCOMMAND_H__


#include <ecom/ecom.h>


namespace UpnpCommand
    {
        enum TUpnpCommandState
        {
        EUndefinedState = 100,
        EUpnpCommandIdle,           // Upnp Fw resources not allocated
        EUpnpCommandAllocated,      // Upnp Fw resources allocated
        EUpnpCommandExecuting       // Upnp command execution ongoing
        };     
    }

    
class CUpnpCommand : public CBase
    {
    public:           
           static CUpnpCommand* NewL();
           
           virtual UpnpCommand::TUpnpCommandState State() = 0;             
              
    };
    

inline  CUpnpCommand* CUpnpCommand::NewL()
    {
	 CUpnpCommand* command = NULL ; 
	 return command;	 
    }    

#endif


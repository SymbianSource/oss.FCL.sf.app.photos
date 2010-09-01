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
* Description:  Upnp Show Via upnp stub
*
*/


 
 #ifndef __C_GLXUPNPSHOWCOMMAND_H__
 #include <upnpshowcommand.h>
 #endif
 
 #include <upnpcommand.h>
 
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
 
 
TBool CUpnpShowCommand::iIsAvailableValue;
 
UpnpCommand::TUpnpCommandState CUpnpShowCommand::iUpnpCommandState;
 


    
 CUpnpShowCommand* CUpnpShowCommand::NewL(MUpnpCommandObserver* /*aObserver*/ )
    {

    CUpnpShowCommand* command = new(ELeave)
    				CUpnpShowCommand();    				
    // Return the instance
    return command;
    }    
    
    
 UpnpCommand::TUpnpCommandState CUpnpShowCommand:: State() 
    {
    EUNIT_PRINT(_L("Before return state"));
    return iUpnpCommandState;
    }
    
    
TBool CUpnpShowCommand::IsAvailableL()
    {
    return iIsAvailableValue;	
    }    
    
 void CUpnpShowCommand::SetUpIsAvailable(TBool aValue)
    {    
    CUpnpShowCommand::iIsAvailableValue = aValue;    
    }    
    
 void CUpnpShowCommand::ShowImageL(const TDesC& /*aFilename*/)
    {
    
    }
    
    
 void CUpnpShowCommand::ShowVideoL(const TDesC& /*aFilename*/)   
    {
    
    } 

	
 void CUpnpShowCommand::StopShowingL()	
    {
    
    }
		
 void CUpnpShowCommand::StartShowingL()
    {
    
    }
    
    
    
    
void CUpnpShowCommand::SetStatus(UpnpCommand::TUpnpCommandState aCommandState)   
    {   
    EUNIT_PRINT(_L("Start Setstatus"));
    iUpnpCommandState = aCommandState;         
    EUNIT_PRINT(_L("End Setstatus"));
    }
    
 

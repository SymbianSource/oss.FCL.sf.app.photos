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
* Description:  Upnp ShowVia command Stub
*
*/



#ifndef __C_GLXUPNPSHOWCOMMAND_H__
#define __C_GLXUPNPSHOWCOMMAND_H__

#ifndef __C_GLXUPNPCOMMAND_H__
#include <upnpcommand.h>
#endif

#include <upnpcommandobserver.h>
class MUpnpCommandObserver;

/*Class declaration
* The UPNP Framwork Stub
*/

class CUpnpShowCommand: public CUpnpCommand
{
public:
    
    static TBool IsAvailableL();

    static CUpnpShowCommand* NewL( MUpnpCommandObserver* aObserver );    
   
    UpnpCommand::TUpnpCommandState State();

    void StartShowingL();

    void StopShowingL();

    void ShowImageL( const TDesC& aFilename );

    void ShowVideoL( const TDesC& aFilename );

    static void SetUpIsAvailable(TBool aValue);   

public:

    static  TBool iIsAvailableValue;   
    
    void SetStatus(UpnpCommand::TUpnpCommandState aCommandState);
    
    static UpnpCommand::TUpnpCommandState iUpnpCommandState;

};  


 



    


#endif

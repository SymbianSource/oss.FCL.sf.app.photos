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
* Description:    Constants for Back Stepping Service Provider
 *
*/




#ifndef BS_SERVICE_CONSTANTS_H
#define BS_SERVICE_CONSTANTS_H

// BS Service implementation UID
const TInt KBSServiceImplUid( 0x2000F840);

// from LIW
_LIT8( KGenericParamServiceCmdIDStr, "cmd" );

// BSS Service and Interface ID
_LIT8( KBSServiceID, "Service.BackStepping" );
_LIT8( KBSInterface, "IBackStepping" );

// BSS commands
_LIT8( KBSCmdInitialize, "Initialize" );
_LIT8( KBSCmdForwardActivationEvent, "ForwardActivationEvent" );
_LIT8( KBSCmdHandleBackCommand, "HandleBackCommand" );

// BSS in param names
_LIT8( KBSInParamAppUid, "AppUid" );
_LIT8( KBSInParamState, "State" );
_LIT8( KBSInParamEnter, "Enter" );
_LIT8( KBSInParamCheckOnly, "CheckOnly" );

// BSS out param names
_LIT8( KBSOutParamStatusInfo, "StatusInfo" );

#endif // BS_SERVICE_CONSTANTS_H

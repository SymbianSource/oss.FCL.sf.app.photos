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
* Description:    Command Handler abstract class
*
*/




#ifndef M_MGLXUICOMMANDHANDLER_H
#define M_MGLXUICOMMANDHANDLER_H

class CAlfControl;

/**
 *  MGlxUiCommandHandler
 *
 *  Command Handler interface class. 
 *  
 */
class MGlxUiCommandHandler
    {
public:
    /**
    * HandleCommandL
    */
    virtual void HandleCommandL(TInt aCommandId, CAlfControl* aControl) = 0;
    };

#endif // M_MGLXUICOMMANDHANDLER_H

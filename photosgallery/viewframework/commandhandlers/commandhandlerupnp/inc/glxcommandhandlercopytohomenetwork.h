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
* Description:    Upnp CommandHandler
*
*/




#ifndef C_GLXCOMMANDHANDLERCOPYTOHOMENETWORK_H__
#define C_GLXCOMMANDHANDLERCOPYTOHOMENETWORK_H__


//Includes
#include <e32base.h>
#include <glxmedialistcommandhandler.h>

class t_cglxcommandhandlercopytohomenetwork;

// CLASS DECLARATION
  /**
   *  CommandHandler which handles copying media to the home network
   *
   * @lib glxupnpcommandhandler.lib
   * @internal reviewed 1/02/2007 by Kimmo Hoikka
   */   
NONSHARABLE_CLASS( CGlxCommandHandlerCopyToHomeNetwork ) : public CGlxMediaListCommandHandler
    {    
public:
    //Constructors and Destructor    
    /**
    * NewL - Two-phase construction
    * @param aMediaListProvider object that provides the media list.
    */
    
    IMPORT_C static CGlxCommandHandlerCopyToHomeNetwork* 
                        NewL(MGlxMediaListProvider& aMediaListProvider);
    /**
    * Destructor
    */
    IMPORT_C ~CGlxCommandHandlerCopyToHomeNetwork();   
    
private:
    // From CGlxMediaListCommandHandler
    
    /**
     * See @ref CGlxMediaListCommandHandler::DoExecuteL
     */
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);

    /**
     * See @ref CGlxMediaListCommandHandler::DoIsDisabled
     */
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;	
    
private:
    /**
    * First Phase Constructor
    * @param aMediaListProvider object that provides the media list.
    */      
    CGlxCommandHandlerCopyToHomeNetwork(MGlxMediaListProvider& aMediaListProvider);        
    
    /**
     * Second-phase constructor
     */
    void ConstructL();

    /**
     * Add the copy to home network command to the command array.
     */
    void AddCopyToHomeNetworkCommandL();
        
    friend class t_cglxcommandhandlercopytohomenetwork;
    };

#endif // C_GLXCOMMANDHANDLERCOPYTOHOMENETWORK_H__

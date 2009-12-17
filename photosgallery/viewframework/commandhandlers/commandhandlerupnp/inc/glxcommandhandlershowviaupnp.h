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




#ifndef C_GLXCOMMANDHANDLERSHOWVIAUPNP_H__
#define C_GLXCOMMANDHANDLERSHOWVIAUPNP_H__


//Includes
#include <e32base.h>
#include <glxmedialistcommandhandler.h>

class CGlxUpnpRenderer;

// CLASS DECLARATION
  /**
   *  CommandHandler which handles the commands for Show/Stop of the Image/Video
   *
   * @lib glxupnpcommandhandler.lib
   * @internal reviewed 1/02/2007 by Kimmo Hoikka
   */   
NONSHARABLE_CLASS( CGlxCommandHandlerShowViaUpnp ) : public CGlxMediaListCommandHandler
    {    
public:

    //Constructors and Destructor    
    /**
    * NewL - Two-phase construction
    * @param aMediaListProvider object that provides the media list.
    * @param aUseHomeNetworkCascadeMenu enable the home network sub menu
    */
    IMPORT_C static CGlxCommandHandlerShowViaUpnp* 
      NewL(MGlxMediaListProvider* aMediaListProvider, TBool aUseHomeNetworkCascadeMenu);	
    /**
    * Destructor
    */
    IMPORT_C ~CGlxCommandHandlerShowViaUpnp();   
    
public: //From CGlxMediaListCommandHandler
    
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
    * @param aUseHomeNetworkCascadeMenu enable the home network sub menu
    */
    CGlxCommandHandlerShowViaUpnp(MGlxMediaListProvider* aMediaListProvider, 
    		                               TBool aUseHomeNetworkCascadeMenu);
    
    /**
    * Second-phase constructor
    */
    void ConstructL();

    /**
    * Add Command Handlers
    * @param aCommandId    
    */
     void AddUpnpCommandL();
    
private:
    /**
     * If gallery is in the viewing state, as opposed to the
     * browsing state iFullScreen will be set to ETrue
     */
    TBool iFullScreen;

    /**
     * Set on construction, If ETrue then
     * the home network cascade menu (sub menu)
     * will be enabled.
     */
    TBool iUseHomeNetworkCascadeMenu;
    };

#endif
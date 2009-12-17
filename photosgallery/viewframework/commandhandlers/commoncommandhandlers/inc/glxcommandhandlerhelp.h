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
* Description:    Help command handler
*
*/




/**
 * @internal reviewed 03/07/2007 by David Holland
 */

#ifndef C_GLXCOMMANDHANDLERMOREHELP_H
#define C_GLXCOMMANDHANDLERMOREHELP_H

// INCLUDES
#include <e32base.h>
#include <glxcommandhandler.h>

// temp help ids files
#include <photos.hlp.hrh>

#include <hlplch.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

// class

/**
  * TGlxHelpContext
  * Help contexts for views
  *  @author M Byrne
  */
NONSHARABLE_CLASS (TGlxHelpContext)
    {
public:
    IMPORT_C TGlxHelpContext();
    
public:
    TUid iMajor;
    /** Help context for browsing */
    TCoeContextName iBrowseContext;
    
    /** Help context for viewing if applicable */
    TCoeContextName iViewContext;
    };

//class

/**
 *  CGlxCommandHandlerHelp class 
 *  Help command handler
 *  @glxdrmcommandhandler.lib
 *  @author M Byrne
 */
NONSHARABLE_CLASS (CGlxCommandHandlerHelp)
    : public CGlxCommandHandler                                                      
    {
public : //constructor and desctructor
    /**
     * Two-phase constructor
     * @param aHelpContext help context for view
     * @param aMediaListOwner object that provides the media list
     */
    IMPORT_C static CGlxCommandHandlerHelp* NewL(TGlxHelpContext aHelpContext);
        
    /** Destructor */
    IMPORT_C ~CGlxCommandHandlerHelp();
    
    /** Activate command handler
      * @param aViewId id of view activating cmd handler
      */
    void DoActivateL(TInt aViewId);
    
    /**
      * Deactivate command handler
      */
    void Deactivate();
    
protected:
    /** see @ref MGlxCommandHandler::ExecuteL */
    TBool ExecuteL(TInt aCommandId);
    
    /** see @ref MGlxCommandHandler::DynInitMenuPaneL */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    
    /** see @ref MGlxCommandHandler::OfferKeyEventL */
    TKeyResponse OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, 
                                                TEventCode /*aType*/);
    
    /** see @ref MGlxCommandHandler::PreDynInitMenuPaneL */
    virtual void PreDynInitMenuPaneL( TInt aResourceId );
    
    /**
     * See @ref MGlxCommandHandler::GetRequiredAttributesL
     * No implementation required
     */
    void GetRequiredAttributesL(RArray< TMPXAttribute >& /*aAttributes*/,
                                TBool /*aFilterUsingSelection*/,
                                TBool /*aFilterUsingCommandId*/, 
                                TInt /*aCommandId*/) const {}

private:
    /** Constructor */
    CGlxCommandHandlerHelp(TGlxHelpContext aHelpContext);
    
    /**
      * Launch help application
      */
    void LaunchHelpL();

private:
    /** Help contexts for the plugin */
    TGlxHelpContext iHelpContext;
    
    /** flag to indicate whether full-screen view active
     based on dummy view-state commands */
    TBool iInFullScreen;
    };
    
#endif // C_GLXCOMMANDHANDLERMOREHELP_H




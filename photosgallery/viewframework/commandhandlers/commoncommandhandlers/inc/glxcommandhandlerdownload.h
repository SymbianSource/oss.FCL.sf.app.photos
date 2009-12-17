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
* Description:    Download commmand handler
*
*/




#ifndef __GLXCOMMANDHANDLERDOWNLOAD_H__
#define __GLXCOMMANDHANDLERDOWNLOAD_H__



//  INTERNAL INCLUDES
#include "glxcommandhandler.h"


// CLASS DECLARATION

/**
 * @class CGlxCommandHandlerDownload
 *
 * Command handler that opens a browser to allow downloading of images 
 * or videos
 * @author David Holland
 * @internal reviewed 26/06/2007 by Rowland Cook  
 *
 */
NONSHARABLE_CLASS(CGlxCommandHandlerDownload) :
                                   public CGlxCommandHandler
    {
public:  // Constructors and destructor
    /**
    * Two-phased constructor.
    * @return pointer to CGlxCommandHandlerDownload object
    */
    IMPORT_C static CGlxCommandHandlerDownload* NewL();

        
    /**
    * Destructor.
    */
    IMPORT_C ~CGlxCommandHandlerDownload();


private:
    /**
    * Symbian 2nd phase constructor
    */
    void ConstructL();

    /**
    * C++ default constructor.
    */
    CGlxCommandHandlerDownload();


private: // From MGlxCommandHandler
    /**
     * Execute the command, if applicable
     * @param aCommand The command to handle
     * @return ETrue iff command was handled
     * see @ref MGlxCommandHandler::ExecuteL 
     */
   TBool ExecuteL(TInt aCommand);
    
    /**
     * Modify a menu before it is displayed.
     * Null implementation
     * @param aResourceId The resource ID of the menu
     * @param aMenuPane The in-memory representation of the menu pane
     * see @ref MGlxCommandHandler::DynInitMenuPaneL 
     */
    void DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/) {};
    
    /**
     * Called when the owning view is activated
     * Null implementation
     * @param aViewId The ID of the view
     * see @ref CGlxCommandHandler::DoActivateL 
     */
    void DoActivateL(TInt /*aViewId*/) {};

    /**
     * Called when the owning view is deactivated
     * Null implementation
     * see @ref MGlxCommandHandler::Deactivate 
     */
    void Deactivate() {};
    
    /**
     * Offers key events to the command handler
     * Null implementation
     * @param aKeyEvent The key event. 
     * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown
     * @return Indicates whether or not the key event was used by this control. EKeyWasNotConsumed or EKeyWasConsumed
     * see @ref MGlxCommandHandler::OfferKeyEventL 
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,
                            TEventCode /*aType*/) {return EKeyWasNotConsumed;};

    /**
     * Perform any actions needed before the options menu is displayed.
     * The view calling this must first arrange a wait dialog to be displayed, as this operation may take
     * a long time to complete
     * Null implementation
     * see @ref MGlxCommandHandler::PreDynInitMenuPaneL 
     */
    void PreDynInitMenuPaneL( TInt /*aResourceId*/ ) {};
  
    /**
     * See @ref MGlxCommandHandler::GetRequiredAttributesL
     * No implementation required
     */
    void GetRequiredAttributesL(RArray< TMPXAttribute >& /*aAttributes*/,
                                TBool /*aFilterUsingSelection*/,
                                TBool /*aFilterUsingCommandId*/, 
                                TInt /*aCommandId*/) const {}
    
private: // data

    // Not owned 
    };

#endif //__GLXCOMMANDHANDLERDOWNLOAD_H__
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
* Description:    Save command handler
*
*/

#ifndef __C_GLXCOMMANDHANDLERSAVE_H__
#define __C_GLXCOMMANDHANDLERSAVE_H__

#include <e32base.h>
#include <glxcommandhandler.h>
#include <glximageviewermanager.h>

/**
 * @class CGlxCommandHandlerSave
 *
 * Command handler that saves the current item to the file system.
 *
 */

NONSHARABLE_CLASS( CGlxCommandHandlerSave)
    : public CGlxCommandHandler
    {
public:
    /**
     * Two-phase constructor
     * @return pointer to CGlxCommandHandlerSave object
     */
    IMPORT_C static CGlxCommandHandlerSave* NewL();

    /** Destructor */
    IMPORT_C ~CGlxCommandHandlerSave();
   
public: // From MGlxCommandHandler
	/**
	 * Execute the command, if applicable
	 * @param aCommand The command to handle
	 * @return ETrue if command was handled
	 */
	TBool ExecuteL( TInt aCommandId);    

	/**
	 * Modify a menu before it is displayed.
	 * @param aResourceId The resource ID of the menu
	 * @param aMenuPane The in-memory representation of the menu pane
	 */
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	
	/**
	 * Called when the owning view is activated
	 * @param aViewId The ID of the view
	 */
	void DoActivateL(TInt aViewId);

	/**
	 * Called when the owning view is deactivated
	 */
	void Deactivate();
	
	/**
	 * Offers key events to the command handler
	 * @param aKeyEvent The key event. 
	 * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown
	 * @return Indicates whether or not the key event was used by this control. EKeyWasNotConsumed or EKeyWasConsumed
	 */
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

	/**
	 * Perform any actions needed before the options menu is displayed.
	 * The view calling this must first arrange a wait dialog to be displayed, as this operation may take
	 * a long time to complete
	 */
	void PreDynInitMenuPaneL( TInt aResourceId );
	
    /**
     * See @ref MGlxCommandHandler::GetRequiredAttributesL
     * No implementation required
     */	
    void GetRequiredAttributesL(RArray< TMPXAttribute >& /*aAttributes*/,
                                TBool /*aFilterUsingSelection*/,
                                TBool /*aFilterUsingCommandId*/, 
                                TInt /*aCommandId*/) const {}

private:
    /** Second phase constructor */
    void ConstructL();

    /** Constructor */
    CGlxCommandHandlerSave();    
   
private:  
    // Not own
    CGlxImageViewerManager* iImageViewerInstance;
    }; 

#endif // __C_GLXCOMMANDHANDLERSAVE_H__

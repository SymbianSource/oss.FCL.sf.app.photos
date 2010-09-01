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
* Description:    Command Handler interface
*
*/




#ifndef M_GLXCOMMANDHANDLER_H
#define M_GLXCOMMANDHANDLER_H

#include <e32base.h>
#include <coedef.h>
#include <w32std.h>
#include <mpxattribute.h>

class CEikMenuPane;


/**
 * MGlxCommandHandler
 * 
 * Command handler interface
 */
class MGlxCommandHandler
	{
public:
	/**
	 * Execute the command, if applicable
	 * @param aCommand The command to handle
	 * @return ETrue iff command was handled
	 */
	virtual TBool ExecuteL(TInt aCommand) = 0;
	
	/**
	 * Modify a menu before it is displayed.
	 * @param aResourceId The resource ID of the menu
	 * @param aMenuPane The in-memory representation of the menu pane
	 */
	virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane) = 0;

	/**
	 * Called when the owning view is activated
	 * @param aViewId The ID of the view
	 */
	virtual void ActivateL(TInt aViewId) = 0;

	/**
	 * Called when the owning view is deactivated
	 */
	virtual void Deactivate() = 0;
	
	/**
	 * Offers key events to the command handler
	 * @param aKeyEvent The key event. 
	 * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown
	 * @return Indicates whether or not the key event was used by this control. EKeyWasNotConsumed or EKeyWasConsumed
	 */
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType) = 0;

	/**
	 * Perform any actions needed before the options menu is displayed.
	 * The view calling this must first arrange a wait dialog to be displayed, as this operation may take
	 * a long time to complete
     * @param aResourceId Menu resource id
	 */
	virtual void PreDynInitMenuPaneL( TInt aResourceId ) = 0;
	
	/**
	 * Deriving classes should append required attributes to aAttributes
	 * @param aAttributes array to append attributes to.
	 * @param aFilterUsingSelection If ETrue, the deriving class should only append
	 *        attributes relevant to the current selection, If EFalse all attributes
	 *        should be appended.
	 * @param aFilterUsingCommandId If ETrue, only attributes relevant
	 *        to the command id specified by aCommandId will be appended
	 * @param aCommandId if aFilterUsingCommandId is ETrue, only
	 *        attributes relevant to aCommandId will be appened.
	 */
    virtual void GetRequiredAttributesL(RArray< TMPXAttribute >& aAttributes, 
    		                            TBool aFilterUsingSelection,
    		                            TBool aFilterUsingCommandId,
    		                            TInt aCommandId = 0) const = 0;
	
    /**
	 * Destructor
	 */
	virtual ~MGlxCommandHandler() {};
	};
	

#endif // M_GLXCOMMANDHANDLER_H

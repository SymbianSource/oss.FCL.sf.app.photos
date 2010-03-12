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
* Description:    Back command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERBACK_H
#define C_GLXCOMMANDHANDLERBACK_H

// INCLUDES
#include <e32base.h>
#include <glxcommandhandler.h>


// FORWARD DECLARATION
class MGlxMediaListProvider;
class MMPXCollectionUtility;
class MMPXViewUtility;
class CGlxNavigationalState;
class CAknAppUi;

/**
 * @class CGlxCommandHandlerBack
 *
 * Command handler that either takes you back to the previously
 * active view or one step back in the UI hierarchy
 *
 */
NONSHARABLE_CLASS (CGlxCommandHandlerBack)
    : public CGlxCommandHandler
	{
private:

	enum TGlxBackCommandId	
    {
    EGlxCmdBack,
    EGlxCmdPreviousView,
    EGlxCmdContainerPreviousView,
    };

public:

	/**
	 * Create Command handler for going back one step within the
	 * UI Hierarchy
	 * 
	 * @param aMediaListProvider media list owner
	 */
	IMPORT_C static CGlxCommandHandlerBack* NewBackCommandHandlerL();
	
	/**
	 * Create Command handler for going back one view when the
	 * current view is outside UI Hierarchy
	 * 
	 * @param aMediaListProvider media list owner
	 */							
	IMPORT_C static CGlxCommandHandlerBack* NewPreviousViewCommandHandlerL( 	
								MGlxMediaListProvider* aMediaListProvider = NULL);
			    
	/**
	 * Create Command handler for checkig whether to go back one view or two view
	 * when the current view is outside UI Hierarchy 
	 * 
	 * @param aMediaListProvider media list owner
	 * @param aPrevViewId previous view id
	 */							
	IMPORT_C static CGlxCommandHandlerBack* NewContainerPreviousViewCommandHandlerL( 	
								MGlxMediaListProvider* aMediaListProvider = NULL, 
								TInt aPrevViewId = 0 );

	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerBack();
    
private:

    /**
     * Two-phase constructor
	 *
	 * @param aCommandId Command id for command handler
	 * @param aMediaListProvider media list owner
	 * @param aPrevViewId previous view id
	 */
	static CGlxCommandHandlerBack* NewL(
							TInt aCommandId, 
							MGlxMediaListProvider* aMediaListProvider,
							TInt aPrevViewId );
	    
    /** Second phase constructor */
	void ConstructL(TBool aIsContainerList);

    /** Constructor */
	CGlxCommandHandlerBack(MGlxMediaListProvider* aMediaListProvider,
							TInt aPrevViewId );
     
    // This API to be removed after testing with migration is over 
	/**
	 * check if deletion of container need to be checked
	 */ 
	// TBool CheckContainerDeletion() const;
	
public: // From MGlxCommandHandler

	/**
	 * Execute the command, if applicable
	 * @param aCommand The command to handle
	 * @return ETrue iff command was handled
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


private: // data

    /** Provider of media list */
    MGlxMediaListProvider* iMediaListProvider;
    
    /** Back functionality that command handler need to execute */
    TInt iCurrentCommandId;
        
    //Previous view id to be checked 
    TInt iPrevViewId;
    
    // Navigational state is stored here
    CGlxNavigationalState* iNavigationalState;
   
    /**
     * Check for the view activation and avoid previous view activation
     * till the view activation is called on back
     */
    TBool iIsViewActivated;
    
    
    /// The AppUI. Not owned
    CAknAppUi* iAppUi;
	};

#endif // C_GLXCOMMANDHANDLERBACK_H

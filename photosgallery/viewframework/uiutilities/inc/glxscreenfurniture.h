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
* Description:    Screenfurniture interface. Used to show/hide UI or 
* 				 enable/disable induvidual screenfurniture items. Contains 
* 				 softkeys, toolbar and the title statuspane as part of it.
*
*/




#ifndef C_GLXSCREENFURNITURE_H
#define C_GLXSCREENFURNITURE_H

#include "glxuiutility.h"

#include <akntoolbar.h>
#include <akntoolbarobserver.h>
#include <aknbutton.h>

/**
 *  CGlxScreenFurniture
 *
 *  ScreenFurniture is responsible for handling the floating toolbar for 
 *  non-touch
 *
 *  @lib glxuiutils.lib
 */
NONSHARABLE_CLASS( CGlxScreenFurniture ): public CBase, 
                                          public MAknToolbarObserver
    { 

public:
	/**
	* Two-phased constructor
    * @param aParentUtility Instance of CGlxUiUtility used to create this
	* @return Constructed object
	*/
	IMPORT_C static CGlxScreenFurniture* NewL( CGlxUiUtility& aParentUtility );
	
	/**
	 * Two-phased constructor
	 * @param aParentUtility Instance of CGlxUiUtility used to create this
	 * @return Constructed object
	 */
	IMPORT_C static CGlxScreenFurniture* NewLC( CGlxUiUtility& aParentUtility );
        
	/**
	 * Set the ID of the currently active view. Call this in DoActivateL().
	 * @param aViewId The view that is trying to control the screen furniture
	 */	
	IMPORT_C void SetActiveView( TInt aViewId);

	/**
	 * Lets the screen furniture know that a view has been deactivated.
	 * If no other views are Active, the screen furniture will turn itself off.
	 * Call this in DoDeactivate().
	 * @param aViewId The view that is trying to control the screen furniture
	 */	
	IMPORT_C void ViewDeactivated( TInt aViewId);
	
	/**
	 * To set the toolbar visibility
	 * @param aVisible Whether toolbar should be visible or not.
	 * @param aFocus Whether focus is required by default
	 */
	IMPORT_C void SetToolbarVisibility(TBool aVisible);
		
	/**
	 * To set the item specific visibility
	 * @param aCommand The command that should be shown or hidden
	 * @param aVisible ETrue if item needs to be shown else EFalse
	 */
	IMPORT_C void SetToolbarItemVisibility( TInt aCommand, TBool aVisible );
	
	/**
	 * To set the command dimmed
	 * @param aCommand The command that should be dimmed
	 * @param aDimmed ETrue if command needs to be dimmed.
	 */
	IMPORT_C void SetToolbarItemDimmed( TInt aCommand, TBool aDimmed );
	
	/**
	 * To set the focus of the toolbar
	 * @param aCommand The command that should be focused.
	 */
	IMPORT_C void SetFocusL( TInt aCommand );
	
	/**
	 * Set the toolbar position w.r.t the screen area	 * 
	 */
	IMPORT_C void SetToolbarPosition();
	
	/**
	 * Sets the tooltip position of each toolbar item.
	 * @param aCommand Command Id 
	 * @param aPos Position of the tooltip to be set.
	                EPositionTop = 1,  Tool tip alignment vertically to top 
                    EPositionBottom, Tool tip alignment vertically to bottom 
                    EPositionLeft, Tool tip alignment horizontally to left 
                    EPositionRight, Tool tip alignment horizontally to right
	 */
	IMPORT_C void SetTooltipL( TInt aCommand,
	        CAknButton::TTooltipPosition aToolTipPos, const TDesC& aToolTipText = KNullDesC);
			
    /**
     * This will be called for changing softkeys(MSK and RSK) and providing commands to them
     */
    IMPORT_C void ModifySoftkeyIdL(CEikButtonGroupContainer::TCommandPosition aPosition, TInt aCommandId, TInt aResourceId, const TDesC& aText= KNullDesC );
    
	/**
	 * Destructor
	 */      
	~CGlxScreenFurniture();

private:
	/**
	 * Default constructor
	 */
    CGlxScreenFurniture( CGlxUiUtility& aParentUtility );
	
    /**
     * ConstructL
     */
	void ConstructL();
	
private: //From MAknToolbarObserver
	 void OfferToolbarEventL( TInt aCommand );
	 void SetToolbarObserver(MAknToolbarObserver* aObserver);
	  	
private:
    /// The ID of the view currently using the screen furniture
    TInt iActiveView;
    
    // Resource offset
    TInt iResourceOffset;
       
    /// Parent UiUtility reference
    CGlxUiUtility& iParentUtility;
    
    /// Avkon floating toolbar with focus (Owned)
    CAknToolbar* iToolbar;   
    };

#endif /*C_GLXSCREENFURNITUREIMPL_H*/

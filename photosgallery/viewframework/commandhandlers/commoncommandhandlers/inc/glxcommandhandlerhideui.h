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
* Description:    Hide Ui command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERHIDEUI_H
#define C_GLXCOMMANDHANDLERHIDEUI_H

// INCLUDES

#include <glxcommandhandler.h>

// FORWARD DECLARATIONS
class CGlxUiUtility;

// CLASS DECLARATION

/**
 * Command to hide and show the screen furniture.
 *
 * @ingroup glx_group_command_handlers
 */
NONSHARABLE_CLASS (CGlxCommandHandlerHideUi) : 
            public CGlxCommandHandler
    {
public:
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CGlxCommandHandlerHideUi* NewL();

    /**
     * Destructor.
     */
    ~CGlxCommandHandlerHideUi();

public: // From CGlxCommandHandler
	virtual TBool ExecuteL(TInt aCommand);
	virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane,
                                    TBool aIsBrowseMode);
	virtual void DoActivateL(TInt aViewId);
	virtual void Deactivate();
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
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
    /**
     * C++ default constructor.
     */
    CGlxCommandHandlerHideUi();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data
    /// ID of owning view
    TInt iViewId;

    /// Hui utility
    CGlxUiUtility* iUiUtility;

    /// Whether the command is available (only in full-screen)
    TBool iCommandAvailable;
    };

#endif  // C_GLXCOMMANDHANDLERHIDEUI_H

// End of File

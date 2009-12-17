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
* Description:    Rotate command implementation
*
*/




// INCLUDE FILES

#include "glxcommandhandlerhideui.h"

#include <glxuiutility.h>
#include <glxlog.h>
#include <eikmenup.h>

#include "glxcommandhandlers.hrh"


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerHideUi* CGlxCommandHandlerHideUi::NewL()
    {
    CGlxCommandHandlerHideUi* self
            = new (ELeave) CGlxCommandHandlerHideUi();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerHideUi::CGlxCommandHandlerHideUi():CGlxCommandHandler()
    {
    }
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerHideUi::ConstructL()
    {
    iUiUtility = CGlxUiUtility::UtilityL();
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerHideUi::~CGlxCommandHandlerHideUi()
    {
    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
    }
    
// ---------------------------------------------------------------------------
// ExecuteL
// ---------------------------------------------------------------------------
//
TBool CGlxCommandHandlerHideUi::ExecuteL(TInt aCommand)
    {
    GLX_LOG_INFO("CGlxCommandHandlerHideUi::ExecuteL");
    TBool handled = EFalse;

    switch ( aCommand )
        {
        case EGlxCmdStateView:
            // Moved to full screen
            iCommandAvailable = ETrue;
            break;

        case EGlxCmdStateBrowse:
            // Moved back to Grid view
            iCommandAvailable = EFalse;
            break;

        case EGlxCmdHideUI:
            {
                handled = ETrue;
            }
            break;

        default:
            break;
        }

    return handled;
    }
    
// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerHideUi::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* aMenuPane)
    {
    if ( !iCommandAvailable && aMenuPane )
        {
        TInt pos;
        if ( aMenuPane->MenuItemExists(EGlxCmdHideUI, pos) )
            {
            aMenuPane->SetItemDimmed(EGlxCmdHideUI, ETrue);
            }
        }
    }
    
// ---------------------------------------------------------------------------
// DoActivateL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerHideUi::DoActivateL(TInt aViewId)
    {
    iViewId = aViewId;
    }
    
// ---------------------------------------------------------------------------
// Deactivate
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerHideUi::Deactivate()
    {
    // No implementation
    }
    
// ---------------------------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CGlxCommandHandlerHideUi::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }
    
// ---------------------------------------------------------------------------
// PreDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerHideUi::PreDynInitMenuPaneL( TInt /*aResourceId*/ )
    {
    // No implementation
    }
    
    
    

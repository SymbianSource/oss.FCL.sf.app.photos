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
* Description:    Launch help application with appropriate help context
*
*/





/**
 * @internal reviewed 03/07/2007 by David Holland
 */



#include "glxcommandhandlerhelp.h"
#include <glxcommandhandlers.hrh>

#include <glxgallery.hrh>

#include <avkon.hrh>
#include <coemain.h>

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TGlxHelpContext::TGlxHelpContext()
    {
    iMajor = TUid::Uid( KGlxGalleryApplicationUid );
    iBrowseContext = KNullDesC;
    iViewContext = KNullDesC;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerHelp* 
                CGlxCommandHandlerHelp::NewL(TGlxHelpContext aHelpContext)
    {
    CGlxCommandHandlerHelp* self = new ( ELeave )
        CGlxCommandHandlerHelp(aHelpContext);

    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerHelp::CGlxCommandHandlerHelp(TGlxHelpContext aHelpContext)
:CGlxCommandHandler(), iHelpContext(aHelpContext)
    {
    // Do nothing
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerHelp::~CGlxCommandHandlerHelp()
    {       
    // No implementation 
    }

// ---------------------------------------------------------------------------
// DoActivateL
// ---------------------------------------------------------------------------
//    
void CGlxCommandHandlerHelp::DoActivateL(TInt /*aViewId*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// Deactivate
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerHelp::Deactivate()
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// DoExecute - the relevant action for the command id
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerHelp::ExecuteL(TInt aCommandId)
    {
    TBool handledCommand = ETrue;
    
    switch (aCommandId)
        {
        case EAknCmdHelp:
            {
            LaunchHelpL();
            break;
            }
        case EGlxCmdStateView:
            iInFullScreen = ETrue;
            handledCommand = EFalse;
            break;

        case EGlxCmdStateBrowse:
            iInFullScreen = EFalse;
            handledCommand = EFalse;
            break;     
        default:
            {
            handledCommand = EFalse;
            break;
            }
        }
        
    return handledCommand;
    }
    
// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerHelp::DynInitMenuPaneL(TInt /*aResourceId*/, 
                                                CEikMenuPane* /*aMenuPane*/)
    {
    // No implementation
    }    
    
// ---------------------------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CGlxCommandHandlerHelp::
        OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }    
 
// ---------------------------------------------------------------------------
// PreDynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerHelp::PreDynInitMenuPaneL( TInt /*aResourceId*/ )
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// LaunchHelpL
// -----------------------------------------------------------------------------
//    
void CGlxCommandHandlerHelp::LaunchHelpL()
    {
    TCoeHelpContext helpContext;
    
    helpContext.iMajor = iHelpContext.iMajor;
    
    if(iInFullScreen)
        {
        helpContext.iContext = iHelpContext.iViewContext;
        }
    else
        {
        helpContext.iContext = iHelpContext.iBrowseContext;
        }
    
    
    // Create the context array and append the single context item
    CArrayFix<TCoeHelpContext>* contextArray = 
        new ( ELeave ) CArrayFixFlat<TCoeHelpContext> ( 1 );

    CleanupStack::PushL( contextArray );
    contextArray->AppendL( helpContext );
    CleanupStack::Pop( contextArray );

    //Launch help - takes ownership of context array
    RWsSession& wsSession = CCoeEnv::Static()->WsSession();
    HlpLauncher::LaunchHelpApplicationL( wsSession,
                                         contextArray );
    }

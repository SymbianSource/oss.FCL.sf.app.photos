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


#include "glxcommandhandlersave.h"

#include <glxcommandhandlers.hrh>
#include <glxscreenfurniture.h>
#include <glxtracer.h>
#include <DocumentHandler.h>
#include <glxpanic.h>

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSave* CGlxCommandHandlerSave::NewL()
    {
    TRACER("CGlxCommandHandlerSave::NewL");
    CGlxCommandHandlerSave* self = new (ELeave) CGlxCommandHandlerSave();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerSave::CGlxCommandHandlerSave()
    {
    // Don't do anything.
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSave::ConstructL()
    {
    TRACER("CGlxCommandHandlerSave::ConstructL");    
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSave::~CGlxCommandHandlerSave()
    {
    TRACER("CGlxCommandHandlerSave::~CGlxCommandHandlerSave");
    if ( NULL != iImageViewerInstance)
        {
        iImageViewerInstance->DeleteInstance();
        }
    }

// ---------------------------------------------------------------------------
// Create save command
// ---------------------------------------------------------------------------
//
TBool CGlxCommandHandlerSave::ExecuteL(TInt aCommandId)
    {
    TRACER("CGlxCommandHandlerSave::ExecuteL");
    if(aCommandId == EGlxCmdSave)
    	{
        RFile64& imageHandle = iImageViewerInstance->ImageFileHandle();
    	if ( &imageHandle )
    	    {
            if ( imageHandle.SubSessionHandle() != KNullHandle )
                {
                TDataType nullType;
                CDocumentHandler* handler = CDocumentHandler::NewLC(NULL); 
                __ASSERT_ALWAYS(handler, Panic(EGlxPanicNullPointer));
                TRAP_IGNORE(handler->CopyL(imageHandle, KNullDesC, nullType, NULL));
                CleanupStack::PopAndDestroy(handler);
                }            
    		}
    	return ETrue;
    	}
    return EFalse;
    } 
// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSave::DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* aMenuPane)
    {
    TRACER("CGlxCommandHandlerSave::DynInitMenuPaneL");
    if ( aMenuPane )
        {
        TInt pos;
        if ( aMenuPane->MenuItemExists(EGlxCmdSave, pos) )
            {            
            if(!iImageViewerInstance->IsPrivate())
                {
                aMenuPane->DeleteMenuItem(EGlxCmdSave);
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// DoActivateL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSave::DoActivateL(TInt /*aViewId*/)
	{
	// Do Nothing
	}

// -----------------------------------------------------------------------------
// Deactivate
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerSave::Deactivate()
	{
	// Do Nothing
	}

// ----------------------------------------------------------------------------
// OfferKeyEventL
// ----------------------------------------------------------------------------
TKeyResponse CGlxCommandHandlerSave::OfferKeyEventL(const TKeyEvent& aKeyEvent, 
															TEventCode aType)
	{
    TKeyResponse response = EKeyWasNotConsumed;
    // Is the key event from the Cancel (Backspace) key
	if (aKeyEvent.iCode == EKeyBackspace && aType == EEventKey)
		{
		// try to execute the save command
		if(ExecuteL( EGlxCmdSave ))
		    {
    		response = EKeyWasConsumed;
		    }
		}
	return response;	
	}

// -----------------------------------------------------------------------------
// PreDynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerSave::PreDynInitMenuPaneL( TInt /*aResourceId*/ )
	{
	// Do Nothing		
	}
		
//End of file

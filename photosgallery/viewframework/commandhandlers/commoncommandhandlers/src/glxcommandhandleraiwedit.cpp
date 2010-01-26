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
* Description:    Delete command handler
*
*/




#include "glxcommandhandleraiwedit.h"

#include <glxattributecontext.h>

#include <glxcommandhandlers.hrh>
#include <glxmedia.h>
#include <glxuiutilities.rsg>

#include "glxaiwservicehandler.h"
#include <StringLoader.h>
#include <glxmedialist.h> 
#include <glxtracer.h>
#include <glxlog.h>
const TInt KGlxAiwEditCommandSpace = 0x00000200;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxCommandHandlerAiwEdit* CGlxCommandHandlerAiwEdit::NewL(
        MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource,
        TBool aCommandSingleClick)
    {
    CGlxCommandHandlerAiwEdit* self = new ( ELeave ) 
        CGlxCommandHandlerAiwEdit(aMediaListProvider, aMenuResource);
    CleanupStack::PushL( self );
    self->ConstructL(aCommandSingleClick);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwEdit::ConstructL(TBool aCommandSingleClick)
    {
    TRACER("CGlxCommandHandlerAiwEdit::ConstructL");
    CGlxCommandHandlerAiwBase::ConstructL();
    iCommandSingleClick = aCommandSingleClick;
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwEdit::DoGetRequiredAttributesL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwEdit::DoGetRequiredAttributesL
									(RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const
	{
	if (!aFilterUsingSelection || SelectionLength() <= MaxSelectedItems())
		{
	    // Add MIME type and fileaname attributes
	    aAttributes.AppendL(KMPXMediaGeneralMimeType);
		aAttributes.AppendL(KMPXMediaGeneralUri);		
		}
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
CGlxCommandHandlerAiwEdit::CGlxCommandHandlerAiwEdit(
        MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource)
:   CGlxCommandHandlerAiwBase(aMediaListProvider, aMenuResource)
    {
    }
    
// -----------------------------------------------------------------------------
// CommandId
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwEdit::CommandId() const
    {
    if (iCommandSingleClick)
        {
        return EGlxCmdAiwSingleClickEdit;
        }
    return EGlxCmdAiwEdit;
    }
    
// -----------------------------------------------------------------------------
// AiwCommandId
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwEdit::AiwCommandId() const
    {
    return KAiwCmdEdit;
    }
    
// -----------------------------------------------------------------------------
// AiwInterestResource
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwEdit::AiwInterestResource() const
    {
    if (iCommandSingleClick)
        {
        return R_GLX_AIW_SINGLE_CLICK_EDIT_INTEREST;
        }    
    return R_GLX_AIW_EDIT_INTEREST;
    }
        
// -----------------------------------------------------------------------------
// AppendAiwParameterL
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerAiwEdit::AppendAiwParameterL(const TGlxMedia& aItem, 
                                     CGlxAiwServiceHandler& aAiwServiceHandler)
    {
    return AppendDefaultAiwParameterL(  aItem, 
                                        aAiwServiceHandler, 
                                        ETrue,                  // Add the Uri
                                        ETrue);                // Add the Mime Type
		                                       
    }

// -----------------------------------------------------------------------------
// CommandSpace
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwEdit::CommandSpace() const
    {
    return KGlxAiwEditCommandSpace;
    }
	
// -----------------------------------------------------------------------------
// AiwDoDynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwEdit::AiwDoDynInitMenuPaneL(TInt /*aResourceId*/, 
        CEikMenuPane* aMenuPane)
    {
    TRACER("CGlxCommandHandlerAiwEdit::AiwDoDynInitMenuPaneL()");
    HBufC* currentTitle = StringLoader::LoadLC( R_QTN_LGAL_OPTIONS_EDIT );
    TInt cmdId = AiwMenuCmdIdL( *currentTitle,aMenuPane ) ;
    CleanupStack::PopAndDestroy(currentTitle);

    if (KErrNotFound != cmdId)
        {
        if (iCommandSingleClick && (MediaList().SelectionCount() > 1))
            {
            aMenuPane->SetItemDimmed(cmdId,ETrue);    
            }
        else if ( MediaList().SelectionCount() != 1) 
            {
            // Enable ONLY when a single item marked
            aMenuPane->SetItemDimmed(cmdId,ETrue);    
            }    
        }
    }

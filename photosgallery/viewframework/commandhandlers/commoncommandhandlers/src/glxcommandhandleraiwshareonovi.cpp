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
* Description:    AIW Share On Ovi command handler ( Works only with ShareOnline application version 4.3 and above )
*
*/


// INTERNAL INCLUDES
#include "glxcommandhandleraiwshareonovi.h"
#include "glxaiwservicehandler.h"

// EXTERNAL INCLUDES
#include <glxattributecontext.h>
#include <glxmedia.h>
#include <glxcommandhandlers.hrh>
#include <glxuiutilities.rsg>

// CONSTANTS AND FORWARD DECLARATIONS
const TInt KGlxAiwShareOnOviCommandSpace = 0x00000500;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//  
EXPORT_C CGlxCommandHandlerAiwShareOnOvi* CGlxCommandHandlerAiwShareOnOvi::NewL(
		MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource,
		const TDesC& aFileName)
	{
	CGlxCommandHandlerAiwShareOnOvi* self =
			new (ELeave) CGlxCommandHandlerAiwShareOnOvi(aMediaListProvider,
					aMenuResource);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwShareOnOvi::DoGetRequiredAttributesL
// -----------------------------------------------------------------------------
//  
void CGlxCommandHandlerAiwShareOnOvi::DoGetRequiredAttributesL
         (RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const
    {
    if (!aFilterUsingSelection || SelectionLength() <= MaxSelectedItems())
        {
        // Add filename attribute to fetch context
        aAttributes.AppendL(KMPXMediaGeneralUri);
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//  
CGlxCommandHandlerAiwShareOnOvi::CGlxCommandHandlerAiwShareOnOvi(
        MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource)
:   CGlxCommandHandlerAiwBase(aMediaListProvider, aMenuResource)
    {
    }
    
// -----------------------------------------------------------------------------
// CommandId
// -----------------------------------------------------------------------------
//  
TInt CGlxCommandHandlerAiwShareOnOvi::CommandId() const
    {
    return EGlxCmdAiwShareOnOvi;
    }
    
// -----------------------------------------------------------------------------
// AiwCommandId
// -----------------------------------------------------------------------------
//  
TInt CGlxCommandHandlerAiwShareOnOvi::AiwCommandId() const
    {
    return KAiwCmdUpload;
    }
    
// -----------------------------------------------------------------------------
// AiwInterestResource
// -----------------------------------------------------------------------------
//  
TInt CGlxCommandHandlerAiwShareOnOvi::AiwInterestResource() const
    {
    return R_AIW_SHARE_INTEREST;
    }
        
// -----------------------------------------------------------------------------
// AppendAiwParameterL
// -----------------------------------------------------------------------------
//  
TBool CGlxCommandHandlerAiwShareOnOvi::AppendAiwParameterL(const TGlxMedia& aItem, 
                                     CGlxAiwServiceHandler& aAiwServiceHandler)
    {
    return AppendDefaultAiwParameterL(  aItem, 
                                        aAiwServiceHandler, 
                                        ETrue,                  // Add the Uri
                                        EFalse);                // Don't add the Mime Type
    }

// -----------------------------------------------------------------------------
// CommandSpace
// -----------------------------------------------------------------------------
//  
TInt CGlxCommandHandlerAiwShareOnOvi::CommandSpace() const
    {
    return KGlxAiwShareOnOviCommandSpace;
    }

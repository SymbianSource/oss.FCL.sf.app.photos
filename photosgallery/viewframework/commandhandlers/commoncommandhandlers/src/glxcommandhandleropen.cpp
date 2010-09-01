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




#include "glxcommandhandleropen.h"
#include <avkon.hrh>
#include <mglxmedialist.h>

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerOpen* CGlxCommandHandlerOpen::NewL(MGlxMediaListProvider* aMediaListProvider)
    {
    CGlxCommandHandlerOpen* self = new ( ELeave ) CGlxCommandHandlerOpen(aMediaListProvider);
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
CGlxCommandHandlerOpen::CGlxCommandHandlerOpen(MGlxMediaListProvider* aMediaListProvider)
	: CGlxMediaListCommandHandler(aMediaListProvider)
    {
    // Do nothing
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerOpen::ConstructL()
    {
    
    iUiUtility = CGlxUiUtility::UtilityL();
    
   	// Add supported command
   	TCommandInfo info(EAknCmdOpen);
    info.iCategoryFilter = EMPXVideo;
    info.iCategoryRule = TCommandInfo::EForbidAll;
    info.iDisallowSystemItems = EFalse;    
   	AddCommandL( info );
   	}

    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerOpen::~CGlxCommandHandlerOpen()
    { 
    if(iUiUtility)
        {
        iUiUtility->Close();
        }
    }
        
// -----------------------------------------------------------------------------
// DoExecute - the relevant action for the command id
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerOpen::DoExecuteL(TInt /*aCommandId*/,MGlxMediaList& /*aList*/)
    {
     return EFalse;      	 
    }

// -----------------------------------------------------------------------------
// DoActivateL
// -----------------------------------------------------------------------------
//		
void CGlxCommandHandlerOpen::DoActivateL( TInt /*aViewId*/ )
	{
	// No Imp.
	}

// -----------------------------------------------------------------------------
// DoIsDisabled
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerOpen::DoIsDisabled(TInt /*aCommandId*/, 
                                        MGlxMediaList& aList) const 
    {    
    // Disable if device supports touch or 
    // If there are no items in the medialist or
    // If there are items that are marked.
    
  	return ( iUiUtility->IsPenSupported() || 
  	        aList.Count() == 0 || aList.SelectionCount()>0 );
   	}

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
* Description:    Implementation of Metadata dialog
*
*/




#include "glxmetadatacommandhandler.h"

#include <mglxmedialist.h>
#include <glxtracer.h>
#include <glxcommandhandleraiwshowmap.h>
#include <glxcommandhandlers.hrh>
#include <glxmetadatadialog.rsg>


//-----------------------------------------------------------------------------
// C++ default constructor.
//-----------------------------------------------------------------------------
CGlxMetadataCommandHandler::CGlxMetadataCommandHandler(
                MGlxMediaListProvider* aMediaListProvider):
					CGlxMediaListCommandHandler( aMediaListProvider )
	{
	TRACER("CGlxMetadataCommandHandler::CGlxMetadataCommandHandler");
	
	iMediaListProvider = aMediaListProvider;
	}

//-----------------------------------------------------------------------------
// Two-phased constructor.
//-----------------------------------------------------------------------------
CGlxMetadataCommandHandler* CGlxMetadataCommandHandler::NewL(
                                        MGlxMediaListProvider* aMediaListProvider) 
	{
	TRACER("CGlxMetadataCommandHandler::NewL");
	
	CGlxMetadataCommandHandler* self = new( ELeave ) 
	                            CGlxMetadataCommandHandler(aMediaListProvider);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self); 
	return self;
	}

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
void CGlxMetadataCommandHandler::ConstructL()
	{
	TRACER("CGlxMetadataCommandHandler::ConstructL");
	
	iShowOnMapCmdHandler = CGlxCommandHandlerAiwShowMap::NewL(iMediaListProvider, R_METADATA_MENU);
	}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGlxMetadataCommandHandler::~CGlxMetadataCommandHandler()
	{
	TRACER("CGlxMetadataCommandHandler::~CGlxMetadataCommandHandler");
	
    delete iShowOnMapCmdHandler;	
    }

//-----------------------------------------------------------------------------
// DoExecuteL
//-----------------------------------------------------------------------------
TBool CGlxMetadataCommandHandler::DoExecuteL(TInt aCommandId, MGlxMediaList&/* aList*/)
   {
	TRACER("CGlxMetadataCommandHandler::DoExecuteL");
   
   TBool ret = iShowOnMapCmdHandler->ExecuteL( aCommandId );
   return ret;
   }

//-----------------------------------------------------------------------------
// OfferKeyEventL
//-----------------------------------------------------------------------------
TKeyResponse CGlxMetadataCommandHandler::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,
                                                                TEventCode /*aType*/)
	{
	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxMetadataCommandHandler::PreDynInitMenuPaneL( TInt aResourceId )
	{
	TRACER("CGlxMetadataCommandHandler::PreDynInitMenuPaneL");
	iShowOnMapCmdHandler->PreDynInitMenuPaneL(aResourceId);
	}
   
// -----------------------------------------------------------------------------
// DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxMetadataCommandHandler::DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane )
    {
	TRACER("CGlxMetadataCommandHandler::DynInitMenuPaneL");
    
    iShowOnMapCmdHandler->DynInitMenuPaneL(aMenuId,aMenuPane);
    }

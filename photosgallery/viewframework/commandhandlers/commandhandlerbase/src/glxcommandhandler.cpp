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
* Description:    Command Handler base
*
*/





#include "glxcommandhandler.h"

EXPORT_C CGlxCommandHandler::CGlxCommandHandler( TBool aHasToolbarItem )
{
	iHasToolbarItem = aHasToolbarItem;
}

EXPORT_C void CGlxCommandHandler::ActivateL( TInt aViewId )
{
	DoActivateL(aViewId);
	
	if( iHasToolbarItem )
		{	
		PopulateToolbarL();
		}	
}

// -----------------------------------------------------------------------------
// DoActivateL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxCommandHandler::DoActivateL(TInt /*aViewId*/)
{
		/* Requiring derived classes should Implement */
}
	
// -----------------------------------------------------------------------------
// PopulateToolbar
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxCommandHandler::PopulateToolbarL()
{
		// Implement code to set toolbar items in the required derived classes.
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxCommandHandler::~CGlxCommandHandler()
	{
	
	}

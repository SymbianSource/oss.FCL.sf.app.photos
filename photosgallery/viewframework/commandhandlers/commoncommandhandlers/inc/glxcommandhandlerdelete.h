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




#ifndef __C_GLXCOMMANDHANDLERDELETE_H__
#define __C_GLXCOMMANDHANDLERDELETE_H__

#include <e32base.h>
#include <glxmpxcommandcommandhandler.h>

class MGlxMediaListProvider;
class CGlxUiUtility;

/**
 * @class CGlxCommandHandlerDelete
 *
 * Command handler that deletes selected items from a media list.
 * @author Aki Vanhatalo, Alex Birkett
 *
 */

NONSHARABLE_CLASS( CGlxCommandHandlerDelete)
    : public CGlxMpxCommandCommandHandler
	{
public:
    /**
	 * Two-phase constructor
	 * @param aMediaListProvider object that provides the media list
	 * @param aIsContainerList If ETrue, shows "delete container"
	 *                         confirmation notes and does not allow
	 *                         deleting system items
	 *                         If EFalse, shows "delete item" 
	 *                         confirmation notes and does not check 
	 *                         for system items
	 * @param aFileName resource file
	 */
	IMPORT_C static CGlxCommandHandlerDelete* NewL(
			MGlxMediaListProvider* aMediaListProvider, TBool aIsContainerList,
			TBool aHasToolbarItem, const TDesC& aFileName);

	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerDelete();
    
public: // From CGlxMpxCommandCommandHandler
    /** See @ref CGlxMpxCommandCommandHandler::CreateCommandL */
	virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
	    TBool& aConsume) const;    

    /** See CGlxMpxCommandCommandHandler::ProgressTextL */
    virtual HBufC* ProgressTextL(TInt aCommandId) const;

    /** See CGlxMpxCommandCommandHandler::ConfirmationTextL */
    virtual HBufC* ConfirmationTextL(TInt aCommandId, TBool aMultiSelection) const;

public: // From CGlxCommandHandler
	/** See @ref CGlxCommandHandler::DoActivateL */
	void DoActivateL(TInt aViewId);

     /** See @ref CGlxCommandHandler::OfferKeyEventL  */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

protected:
     /** See @ref CGlxCommandHandler::PopulateToolbar*/
	void PopulateToolbarL();

private:
    /** Second phase constructor */
	void ConstructL(TBool aIsContainerList, const TDesC& aFileName);

    /** Constructor */
	CGlxCommandHandlerDelete(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem);
	
	/*single clk chngs-to update delete option*/
	TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
private:
    /// Resource file offset
	TInt iResourceOffset;
	// Does the list contain containers or items
	
	TInt iIsContainerList;      
	
	CGlxUiUtility* iUiUtility;
	};

#endif // __C_GLXCOMMANDHANDLERDELETE_H__

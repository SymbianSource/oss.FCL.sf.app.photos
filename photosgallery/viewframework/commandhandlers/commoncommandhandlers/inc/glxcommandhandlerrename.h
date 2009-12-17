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
* Description:    Rename command handler
*
*/




#ifndef __C_GLXCOMMANDHANDLERRENAME_H__
#define __C_GLXCOMMANDHANDLERRENAME_H__

#include <e32base.h>
#include <glxmpxcommandcommandhandler.h>

class MGlxMediaListProvider;

/**
 * @class CGlxCommandHandlerRename
 *
 * Command handler that renames a container item except if multiple items are 
 * marked, or the selected item is a static or system item
 * or if the view is empty.
 * @author Rhodri Byles
 * @internal reviewed 21/06/2007 by Loughlin  
 *
 */
NONSHARABLE_CLASS(CGlxCommandHandlerRename) :
    public CGlxMpxCommandCommandHandler
	{
public:
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aHasToolbarItem Whether Command Should be toolbar item (ETrue - if toolbar item)
     */
	IMPORT_C static CGlxCommandHandlerRename* NewL(
	    MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem );
	    
	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerRename();
	
protected:
	/*
	 * 
	 */
	void PopulateToolbarL();
    
private: // From CGlxMpxCommandCommandHandler

    /** @ref CGlxMpxCommandCommandHandler::CreateCommandL */
	CMPXCommand* CreateCommandL( TInt aCommandId, MGlxMediaList& aList,
	    TBool& aConsume ) const;

    /** @ref CGlxMpxCommandCommandHandler::HandleErrorL */
    void HandleErrorL( TInt aErrorCode );

private:
    /**
     * Callback functions to handle an error
     */
    static TBool HandleErrorL(TAny* aPtr);
    inline void HandleErrorL();

private:
    /** Second phase constructor */
	void ConstructL();

    /**
     *  Constructor
     * @param aMediaListProvider object that provides the media list
     */
	CGlxCommandHandlerRename( MGlxMediaListProvider* aMediaListProvider,
			TBool aHasToolbarItem );
 
	/**
	 * Helper function to retrieve the title of the currently selected item.
	 * If none is selected, the title of the item with focus is returned.
     * @param aTitle the title of the selected item
     * @param aList the media list containing the item
	 */	
	void GetTitleL( TDes& aTitle, MGlxMediaList& aList ) const;
	
private:
    /// Owned, the resource file offset
	TInt iResourceOffset;
	// Owned, new name
	HBufC* iRenameText;
    /// Owned, error value from HandleErrorL
    TInt iError;
    /// Owned, callback object for HandleErrorL
    CAsyncCallBack* iErrorCallback;
    };

#endif // __C_GLXCOMMANDHANDLERRENAME_H__

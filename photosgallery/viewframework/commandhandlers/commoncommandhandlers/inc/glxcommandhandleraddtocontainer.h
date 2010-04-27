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
* Description:    Add to container commmand handler
*
*/




#ifndef __C_GLXCOMMANDHANDLERADDTOCONTAINER_H__
#define __C_GLXCOMMANDHANDLERADDTOCONTAINER_H__

#include <e32base.h>
#include <glxmpxcommandcommandhandler.h>

class MGlxMediaListProvider;

class CEikAppUi;

/**
 * Command handler for adding to a container.
 * @author Alex Birkett, Aki Vanhatalo 
 * @internal reviewed 05/06/2007 by Dave Schofield
 */
NONSHARABLE_CLASS( CGlxCommandHandlerAddToContainer )
    : public CGlxMpxCommandCommandHandler
	{
public:
	/** 
	 * Create add to album command handler 
	 * @param aMediaListProvider object that provides the media list.
     * @param aFileName resource file
	 */
    IMPORT_C static CGlxCommandHandlerAddToContainer
			* NewAddToAlbumCommandHandlerL(
					MGlxMediaListProvider* aMediaListProvider,
					TBool aHasToolbarItem, const TDesC& aFileName);

	/** 
	 * Create add to album command handler for singleclick
	 * @param aMediaListProvider object that provides the media list.
     * @param aFileName resource file
	 */
	IMPORT_C static CGlxCommandHandlerAddToContainer
			* NewAddToAlbumSingleClickCommandHandlerL(
					MGlxMediaListProvider* aMediaListProvider,
					TBool aHasToolbarItem, const TDesC& aFileName);

     /** 
	 * Create add (to) tags command handler 
	 * @param aMediaListProvider object that provides the media list.
     * @param aFileName resource file
	 */
	IMPORT_C static CGlxCommandHandlerAddToContainer
			* NewAddToTagCommandHandlerL(
					MGlxMediaListProvider* aMediaListProvider,
					TBool aHasToolbarItem, const TDesC& aFileName);

     /** 
	 * Create add (to) tags command handler for singleclick
	 * @param aMediaListProvider object that provides the media list.
     * @param aFileName resource file
	 */
	IMPORT_C static CGlxCommandHandlerAddToContainer
			* NewAddToTagSingleClickCommandHandlerL(
					MGlxMediaListProvider* aMediaListProvider,
					TBool aHasToolbarItem, const TDesC& aFileName);

    /** 
	 * Create add (to) Favourites command handler 
	 * @param aMediaListProvider object that provides the media list.
    * @param aFileName resource file
	 */
	IMPORT_C static CGlxCommandHandlerAddToContainer
			* NewAddToFavCommandHandlerL(
					MGlxMediaListProvider* aMediaListProvider,
					TBool aHasToolbarItem, const TDesC& aFileName);

   /** 
	 * Two-phase constructor: 
	 * Prefer NewAddToAlbumCommandHandlerL or NewAddToTagsCommandHandlerL
	 * @param aMediaListProvider object that provides the media list.
	 * @param aCommand The command to handle.
     * @param aFileName resource file
	 */
	IMPORT_C static CGlxCommandHandlerAddToContainer* NewL(
			MGlxMediaListProvider* aMediaListProvider, TInt aCommandId,
			TBool aHasToolbarItem, const TDesC& aFileName);
	    
    /** Destructor */
	IMPORT_C ~CGlxCommandHandlerAddToContainer();
	
	// from MProgressDialogCallback
	void DialogDismissedL(TInt aButtonId);

protected: // From CGlxMediaListCommandHandler

    /** See @ref CGlxMpxCommandCommandHandler::CreateCommandL */
	virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
	    TBool& aConsume) const;    

    /** See @ref CGlxMpxCommandCommandHandler::CompletionTextL */
    virtual HBufC* CompletionTextL() const;

	/** See @ref CGlxMpxCommandCommandHandler::DoHandleCommandCompleteL */
	virtual void DoHandleCommandCompleteL(TAny* aSessionId,
	        CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList);
	
	virtual TBool OkToExit() const;

    /** See @ref CGlxCommandHandler::DoActivateL */
    void DoActivateL(TInt aViewId);

    /** See @ref CGlxCommandHandler::PopulateToolbar*/
	void PopulateToolbarL();  
	
	/** See @ref CGlxCommandHandler::DoIsDisabled*/ 
	TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
private:
	/** 
	 * Second phase constructor
	 * @param aCommand The command to handle.
     * @param aFileName resource file
	 */
	void ConstructL(TInt aCommandId, const TDesC& aFileName);
	
    /** 
     * First phase constructor 
     * @aMediaListProvider object that provides the media list.
     */
	CGlxCommandHandlerAddToContainer(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem );
	
private:
    // Resource offset
	TInt iResourceOffset;
	
	// Container Command Id
	TInt iCommandId;

	// Target containers selection count
	static TInt iSelectionCount;

	// Popup Completion Status Flag
	mutable TBool iIsProcessOngoing;
	
	};

#endif // __C_GLXCOMMANDHANDLERADDTOCONTAINER_H__

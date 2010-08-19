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
* Description:    MPX Command generating Command Handler
*
*/




#ifndef __C_GLXMPXCOMMANDCOMMANDHANDLER_H__
#define __C_GLXMPXCOMMANDCOMMANDHANDLER_H__

#include <AknProgressDialog.h>
#include <e32base.h>
#include <mpxcommand.h>
#include <mpxmessage.h>
#include "glxmedialistcommandhandler.h"

class MGlxMediaList;
class MGlxMediaListProvider;
class CAknAppUi;
class CGlxImageViewerManager;

/**
 * @class CGlxMpxCommandCommandHandler
 *  
 * Command handler that creates and issues an MPXCommand
 *
 * @ingroup glx_group_command_handlers
 *
 * @lib glxviewbase.lib
 */
class CGlxMpxCommandCommandHandler : public CGlxMediaListCommandHandler,
        public MGlxMediaListObserver, public MProgressDialogCallback
	{
public:	
	/**
	 * Constructor
	 * @param aMediaListProvider The owner of the media list to use
	 */
	IMPORT_C CGlxMpxCommandCommandHandler(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem = EFalse );

	/**
	 * Destructor
	 */
	IMPORT_C virtual ~CGlxMpxCommandCommandHandler();
	
protected: // API for deriving classes
    /**
     * Attempts to "exit" the command handler
     * (Exit means closing the progress note and allowing user to do something.)
     * The command handler will exit if 
     *  - progress is complete <b>and</b> deriving class returns ETrue from OkToExit
     *  - aErrorCode is not KErrNone
     *
     * Usually deriving classes never need to call this. This is called
     * automatically after receiving a collection message. Only call if the 
     * completion can happen (= the result of OkToExit() call changes) based 
     * on another event than collection message.
     *
     * @param aErrorCode Error code. Shows an error note if other than KErrNone
     */
    IMPORT_C void TryExitL(TInt aErrorCode);
    
    /**
     * See @ref CGlxCommandHandler::DoActivateL 
     * Default implementation does nothing
     */
    IMPORT_C virtual void DoActivateL( TInt aViewId );
    
    /**
	* Called when the owning view is deactivated
	*/
	IMPORT_C virtual void Deactivate();
	
	/**
	 * Handles error
	 * Default implementation shows an error note
	 */
	IMPORT_C virtual void HandleErrorL(TInt aErrorCode);
	
    /**
     * Creates a command object, and decides whether to consume the command.
     * @param aList Media list to which the command will be issued.
     * @param aConsume Set this to false, if the command
     *                 id should not be consumed. Default value is true.
     * @return command object If NULL returned, no MPX command will be issued,
     *                        but the command id may still be consumed, depending
     *                        on the value of aDoNotConsume
     *                        Caller gets ownership of the command object.
     */
	virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
	    TBool& aConsume) const = 0;
	
	/**
	 * Handles a message other than progress message.
	 * Deriving classes can override this to receive and handle messages
	 * from media list (i.e., from MPX server)
	 * The default implementation does not do anything.
	 * @return Error code. If other than KErrNone, the command handler
	 *                     closes the progress note and shows an error note.
	 */
	IMPORT_C virtual TInt DoHandleMessageL(const CMPXMessage& aMessage,
	    MGlxMediaList& aList);

	/**
	 * Called by HandleCommandCompleteL() to allow derived classes
	 * to perform processing specific to individual command handlers.
	 * The defalut implementation does not do anything.
	 * 
	 */
	IMPORT_C virtual void DoHandleCommandCompleteL(TAny* aSessionId, 
            CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList);

	/**
	 * Called by HandleCommandCompleteL() to allow derived classes
	 * to perform processing specific to individual command handlers.
	 * The defalut implementation does not do anything.
	 * 
	 */
	IMPORT_C virtual void DoHandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
	
    /**
     * Called to check if the deriving class is ready and progress note
     * can be closed. The deriving class may be expecting a message (etc) after
     * the last progress notification is received, and by overriding
     * this class, they can wait for that info.
     * Default implementation returns ETrue.
     * @return ETrue if command handler is ready exit 
     */	    
	IMPORT_C virtual TBool OkToExit() const;

    /**
     * Creates and returns confirmation note text 
     * Deriving class can override this to get a confirmation note shown
     * before execution
     * @param aCommandId Id of the command that is about to be executed
     * @param aMultiSelection ETrue when more than one item is selected
     * @return New descriptor with confirmation note
     *         If single selection, the descriptor must contain %U, which 
     *         will be populated with the title of the selected/focused item
     *         If multiselection, the descriptor must contain %N, which 
     *         will be populated with the number of selected items
     *         Or return NULL, in which case no note will be shown and the command
     *         is simply executed.
     *         Caller gets ownership of the descriptor.
     *         Default implementation returns NULL.
     */
    IMPORT_C virtual HBufC* ConfirmationTextL(TInt aCommandId, TBool aMultiSelection) const;

    /**
     * Creates and returns progress note text 
     * Deriving class can override this to customise the progress note
     * @return New descriptor with progress text
     *         Or return NULL, in which case default progress text will be
     *         used 
     *         Default implementation returns default progress note
     *         Caller gets ownership of the descriptor.
     */
    IMPORT_C virtual HBufC* ProgressTextL(TInt aCommandId) const;

    /**
     * Creates and returns completion note text 
     * Deriving class can override this to show a completion note
     * @return New descriptor with completion text
     *         Or return NULL, in which case the note will not be shown
     *         Default implementation returns NULL
     *         Caller gets ownership of the descriptor.
     */
    IMPORT_C virtual HBufC* CompletionTextL() const;

private:
    /** See @ref CGlxMediaListCommandHandler::DoExecuteL */
	IMPORT_C virtual TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);

	/**
	 * Launch a confirmation note (single or multiple depending on media list selection)
	 * Declared as virtual to allow deriving class to show non-media list dependent
	 * confirmation note, etc.
	 * @return ETrue if the user confirms, EFalse if user cancels
	 */	
	IMPORT_C virtual TBool ConfirmationNoteL(TInt aCommandId, MGlxMediaList& aMediaList) const;

	/**
	 * Launch a confirmation note (single)
	 * @return ETrue if the user confirms 
	 */	
	TBool ConfirmationNoteSingleL(TInt aCommandId, MGlxMediaList& aMediaList) const;

	/**
	 * Launch a confirmation note (multiple)
	 * @return ETrue if the user confirms 
	 */	
	TBool ConfirmationNoteMultipleL(TInt aCommandId, MGlxMediaList& aMediaList) const;
    
	/**
	 * Launch a progress note
	 */	
	void ProgressNoteL(TInt aCommandId);
    
    /**
     * Update progress based on message
     * @param aMessage message with progress info 
     * @param aError extracts an error code from message and returns in this argument
     * @param ETrue if progress complete. EFalse if note
     */
    TBool UpdateProgressL(const CMPXMessage& aMessage, TInt& aError);

	/**
	 * Dismiss the active progress note
	 */	
	void DismissProgressNoteL();
    
    /**
     * Show a completion note
     */
    void CompletionNoteL() const;

    /**
     * Remove medialist observer
     */
    void RemoveMediaListObserver();
	
    /**
     * Creates the image viewer instance, if not created already.
     */    
    void CreateImageViewerInstanceL();
    
    /**
     * Deletes the image viewer instance, if created already.
     */    
    void DeleteImageViewerInstance();

protected: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleItemAddedL
    IMPORT_C void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMediaL
    IMPORT_C void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemRemovedL
    IMPORT_C void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemModifiedL
    IMPORT_C void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleAttributesAvailableL
    IMPORT_C void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
            
    /// See @ref MGlxMediaListObserver::HandleFocusChangedL
    IMPORT_C void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemSelectedL
    IMPORT_C void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMessageL
    IMPORT_C void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleCommandCompleteL
    IMPORT_C void HandleCommandCompleteL(TAny* aSessionId, CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList);
    
public: // from MProgressDialogCallback
	IMPORT_C void DialogDismissedL(TInt aButtonId);
	
private:
    /// Progress dialog
	CAknProgressDialog* iProgressDialog;
	
	// Progress info for the progress dialog
	CEikProgressInfo* iProgressInfo;

    // CEikProgressInfo does not store whether progress is complete, so store here
	TBool iProgressComplete;

	// Resource offset
	mutable TInt iResourceOffset;
	
	/// Whether the GIF animation is currently halted
	TBool iAnimationStopped;
	
    /// The AppUI. Not owned
    CAknAppUi* iAppUi;
    
    // For image viewer, not own
    CGlxImageViewerManager* iImageViewerInstance;
	};

#endif // __C_GLXMPXCOMMANDCOMMANDHANDLER_H__

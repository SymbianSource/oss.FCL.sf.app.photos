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




#ifndef __C_GLXCOMMANDHANDLERNEWMEDIA_H__
#define __C_GLXCOMMANDHANDLERNEWMEDIA_H__

#include <e32base.h>
#include <glxmediaid.h>
#include <glxmpxcommandcommandhandler.h>

class CEikAppUi;

class MGlxMediaListProvider;

/**
 * Command handler for adding new media (new containers)
 * @author Alex Birkett
 *
 * @internal reviewed 06/06/2007 by Dave Schofield
 */
NONSHARABLE_CLASS(CGlxCommandHandlerNewMedia) : public CGlxMpxCommandCommandHandler
	{
public:
    /** 
     * Two-phase constructor: 
     * @param aMediaListProvider object that provides the media list.
     */
	IMPORT_C static CGlxCommandHandlerNewMedia* NewL(MGlxMediaListProvider* aMediaListProvider);
    
	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerNewMedia();

	/**
	 * Execute the dialog.
	 * @param aNewMediaId
	 *        The ID of the media that is created
	 * @return system wide error code.
	 *        (KErrCancel if the dialog is cancelled).
	 */
	IMPORT_C TInt ExecuteLD(TGlxMediaId& aNewMediaId);
	
public: // from CGlxMpxCommandCommandHandler

	/** See @ref CGlxMpxCommandCommandHandler::DoHandleCommandCompleteL */
	void DoHandleCommandCompleteL(TAny* aSessionId, CMPXCommand* aCommandResult, 
            TInt aError, MGlxMediaList* aList);
		
	/** See @ref CGlxMpxCommandCommandHandler::OkToExit */
	TBool OkToExit() const;
    
	/** See @ref CGlxMpxCommandCommandHandler::CreateCommandL */
	virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
	    TBool& aConsume) const;    
	
	/** See @ref CGlxMpxCommandCommandHandler::HandleErrorL */
	void HandleErrorL(TInt aError);

public: // from CGlxMediaListCommandHandler
	/** See @ref CGlxMediaListCommandHandler::BypassFiltersForExecute */
	IMPORT_C TBool BypassFiltersForExecute() const;
	
private:
    /** Second phase constructor */
	void ConstructL();
	
    /** 
     * First phase constructor 
     * @param aMediaListOwner object that provides the media list. 
     */
	
	CGlxCommandHandlerNewMedia(MGlxMediaListProvider* aMediaListProvider);
    
	/**
     * Fetches the 'media popup title' and 'default new media item title'
     * from the collection.
     * @param aCollectionId Collection plugin id from which to fetch titles.
     * @param aDefaultNewMediaItemTitle On return contains the default new media item title.
     */
	void TitlesL(const TGlxMediaId aCollectionId, TDes& aDefaultNewMediaItemTitle) const;
	
    /**
     * Generates a unique new media item title based on the titles of items that already exist.
     * @param aDefaultNewMediaItemTitle the default new media item title (e.g.) "New Tag"
     * @param aList a media list containing the existing media e.g. "New Tag", "New Tag (1)"
     * @return The new media item title e.g. "New Tag (2)"
     */
	HBufC* GenerateNewMediaItemTitleL(const TDesC& aDefaultNewMediaItemTitle, MGlxMediaList& aList) const;

    /**
     * Sets the media list focus
     * and tries to exit the dialog.
     * @param aIndex. Index of the item in the media list to focus.
     */
    void SetFocusL(TInt aIndex);
    
protected: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleItemAddedL
    IMPORT_C void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);

private:
    /**
     * CGlxAsyncFocuser
     * Used by CGlxCommandHandlerNewMedia to perform focusing operations
     * asynchronously.
     */ 
    NONSHARABLE_CLASS(CGlxAsyncFocuser)
        : public CActive
        {
        public:            
            /**
             * Constructor
             * @param aGlxCommandHandlerNewMedia object on which to call SetFocusL
             */
            CGlxAsyncFocuser(CGlxCommandHandlerNewMedia* aGlxCommandHandlerNewMedia);      
            
            /**
             * Destructor
             */
            ~CGlxAsyncFocuser();

        public:
            /**
             * Calls CGlxCommandHandlerNewMedia::SetFocusL asynchronously
             * i.e from CGlxAsyncFocuser::RunL()
             * @param aIndex index to pass to 
             */
            void SetFocus(TInt aIndex);
        
        public: // from CActive
            /** See @ref CActive::RunL */
            void RunL();
            
            /** See @ref CActive::DoCancel */
            void DoCancel();
        
        private:
            /**
             * Object on which to call SetFocusL (not owned)
             */
            CGlxCommandHandlerNewMedia* iGlxCommandHandlerNewMedia;
            
            /**
             * Index of the item to be focused.
             */
            TInt iFocusIndex;  
        };
   
private:    
    /**
     * Resource offset
     */
    TInt iResourceOffset;
    
    /**
     *  Id of new media
     */
    TGlxMediaId iNewMediaId;
    
    /**
     *  Active scheduler wait object. (Owned)
     */
    CActiveSchedulerWait* iSchedulerWait;
    
    /**
     * Error set by DoHandleCommandCompleteL()
     */
    mutable TInt iNewMediaCreationError;
    
    /**
     * Ok to exit. ETrue if OK to exit else EFalse.
     */
    mutable TBool iOkToExit;
    
    /**
     * Object used to call SetFocusL asynchronously (owned)
     */
    CGlxAsyncFocuser* iAsyncFocuser;
    
    /**
     * The title of the new media item.
     */
    mutable HBufC* iNewMediaItemTitle;

    /**
	 * ETrue if a file with the same name already exists, else EFalse
     */
    mutable TBool iFileNameAlreadyExists ;
	};

#endif // __C_GLXCOMMANDHANDLERNEWMEDIA_H__

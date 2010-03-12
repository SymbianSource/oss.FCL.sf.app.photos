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
* Description:    Send commmand handler
*
*/




#ifndef __GLXCOMMANDHANDLERUPLSEND_H__
#define __GLXCOMMANDHANDLERUPLSEND_H__


//  EXTERNAL INCLUDES

#include <TSendingCapabilities.h>

//  INTERNAL INCLUDES
#include "glxmedialistcommandhandler.h"

//  FORWARD DECLARATIONS
class MGlxMediaListProvider;
class CGlxUiUtility;
class CGlxDefaultAttributeContext;
class CMessageData;
class CGlxMedia;
class CSendUi;


// CLASS DECLARATION
/**
 * Command handler that enables the send plugin 
 * class CGlxCommandHandlerSend
 * @author Loughlin Spollen 
 * @internal reviewed 07/06/2007 by Alex Birkett
 */
 
NONSHARABLE_CLASS (CGlxCommandHandlerSend) : public CGlxMediaListCommandHandler, 
							   public MGlxMediaListObserver
	{
public:  // Constructors and destructor
	/**
	* Two-phased constructor.
    * @param aMediaListProvider pointer to media list owner
    * @param aHasToolbarItem Whether Command Should be toolbar item (ETrue - if toolbar item)
    * @return pointer to CGlxCommandHandlerSend object
    */
	IMPORT_C static CGlxCommandHandlerSend* NewL( 
				MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem );
    
    /**
    * Destructor.
    */
    IMPORT_C ~CGlxCommandHandlerSend();

protected:
	/** @ref CGlxCommandHandler::PopulateToolbar*/
	void PopulateToolbarL();  
	
	/** @ref CGlxCommandHandler::DoIsDisabled*/ 
	TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;

private:
	/**
    * Symbian 2nd phase constructor
	*/
    void ConstructL();

    /**
    * C++ default constructor.
    * @param aMediaListProvider pointer to media list owner
    * @ref CGlxMediaListCommandHandler
    */
    CGlxCommandHandlerSend(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem );


private: // From CGlxMediaListCommandHandler
	/**
    * Execute the command, if applicable. 
	* @param aCommand The command to handle
	* @param aList List of media items
    * @return ETrue if command was handled, EFalse otherwise
    * @ref CGlxMediaListCommandHandler::DoExecuteL
	*/
	TBool DoExecuteL (TInt aCommandId, MGlxMediaList& aList);	
	
	/**
	* Called when the owning view is activated
	* @param aViewId The ID of the view
	* @ref CGlxMediaListCommandHandler::ActivateL
	*/
	void DoActivateL (TInt aViewId);
	
	/**
	* Called when the owning view is deactivated
	*/
	void Deactivate();	

    /**
	* @ref CGlxMediaListCommandHandler::OfferKeyEventL
	*/
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    
private:  // From MGlxMediaListObserver
    
    /**
    * not implemented
	* @ref MGlxMediaListObserver::HandleItemAddedL
	*/
    void HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/,
    							MGlxMediaList* /*aList*/){};
    /**
    * not implemented
	* @ref MGlxMediaListObserver::HandleMediaL
	*/
    void HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/){};
    
    /**
    * not implemented
	* @ref MGlxMediaListObserver::HandleItemRemovedL
	*/
    void HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
    									MGlxMediaList* /*aList*/){};

    /**
    * not implemented
	* @ref MGlxMediaListObserver::HandleItemModifiedL
	*/
    void HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
    									MGlxMediaList* /*aList*/){};
    									
    /**
    * not implemented
	* @ref MGlxMediaListObserver::HandleAttributesAvailableL
	*/
    void HandleAttributesAvailableL(TInt /*aItemIndex*/, 
    		const RArray<TMPXAttribute>& /*aAttributes*/, 
    		MGlxMediaList* /*aList*/){};

    /**
    * is implemented - used to trigger an update of the visible state send 
    *					active palette item 
	* @ref MGlxMediaListObserver::HandleFocusChangedL
	*/
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, 
    				TInt aOldIndex, MGlxMediaList* aList);

    /**
    * not implemented
	* @ref MGlxMediaListObserver::HandleItemSelectedL
	*/
    void HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, 
    							MGlxMediaList* /*aList*/){};

    /**
    * not implemented
	* @ref MGlxMediaListObserver::HandleMessageL
	*/
    void HandleMessageL(const CMPXMessage& /*aMessage*/, 
    							MGlxMediaList* /*aList*/){};

private: // methods local to class
	/**
	* Retrieve the currently selected items file name and path
	* @return The currently selected items filename and path
	*/
	CMessageData* SelectedFilesLC();
	
	/**
	* Determine if the media item is of a valid type (ie video or image)
	* @return ETrue if a valid type otherwise EFalse
	*/
	TBool IsValidTypeL(const CGlxMedia& aMedia);

	
	/**
	* Read the selected items uri and use it to send the items
	*/
	void SendSelectedItemsL();
	
	/**
	* Load the resource file
	*/
	void LoadRscFileL();	

	/**
	* Return the static capabilities for sending a media item
	*/
	TSendingCapabilities Capabilities();
	
private: // data

    /// Not owned - Provider of media list
    MGlxMediaListProvider* iMediaListProvider;
    
    /// ID of owning view 
    TInt iViewId;
    
    /// Not owned - HUI utility 
    CGlxUiUtility* iUiUtility;
    
    /// Not owned - Attribute context
    CGlxDefaultAttributeContext* iAttributeContext;
    
    /// Owned - the Send pop-up list, message creation and sending services
    CSendUi* iSendUi;
    
    /// Array of Send Plugins uids that should not be used
    TBool iFilterOutPlugin;
    
    /// The resource file offset
	TInt iResourceOffset;
	};

#endif //__GLXCOMMANDHANDLERUPLSEND_H__

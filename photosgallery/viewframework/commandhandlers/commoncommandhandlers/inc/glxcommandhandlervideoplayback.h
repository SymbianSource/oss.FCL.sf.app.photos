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
* Description:    Video Playback command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERVIDEOPLAYBACK_H
#define C_GLXCOMMANDHANDLERVIDEOPLAYBACK_H

#include <e32base.h>
#include <shwconstants.h>	// for playback direction
#include "glxmedialistcommandhandler.h"

#include <mpxplaybackutility.h> 
#include <mpxplaybackobserver.h> 
#include <mpxviewutility.h> 

class MGlxMediaListProvider;
class CGlxUiUtility;
class CGlxDefaultAttributeContext;

/**
 * @class CGlxCommandHandlerVideoPlayback
 *
 * Command handler that either launches videoplayback with the selected items
 * from a media list or launches the videoplayback settings.
 * Author: Rowland Cook
 * @internal reviewed 11/06/2007 by Alex Birkett
 */
NONSHARABLE_CLASS(CGlxCommandHandlerVideoPlayback) :
            public CGlxMediaListCommandHandler, 
            public MGlxMediaListObserver,
            public MMPXPlaybackObserver
	{
public:
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * 
     * @param aIsContainerList If ETrue, shows "videoplayback container"
     *                         confirmation notes and does not allow
     *                         deleting system items
     
     *                         If EFalse, shows "videoplayback item" 
     *                         confirmation notes and does not check 
     *                         for system items

     */
	IMPORT_C static CGlxCommandHandlerVideoPlayback* NewL(
	    MGlxMediaListProvider* aMediaListProvider);
	    
	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerVideoPlayback();
    
protected:
	/** see CGlxMediaListCommandHandler::DoExcecuteL */
    virtual TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);
    
    /** see CGlxMediaListCommandHandler::DoIsDisabled */
    virtual void DoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    
private:    // From MGlxMediaListObserver
	// See @ref MGlxMediaListObserver::HandleItemAddedL
    void HandleItemAddedL(TInt /* aStartIndex */, TInt /* aEndIndex */,MGlxMediaList* /* aList */) { };
    
	// See @ref MGlxMediaListObserver::HandleMediaL 
    void HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/) { };
    
	// See @ref MGlxMediaListObserver::HandleItemRemovedL
    void HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)  { };
    
	// See @ref MGlxMediaListObserver::HandleItemModifiedL
    void HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/) { };
    
	// See @ref MGlxMediaListObserver::HandleAttributesAvailableL
    void HandleAttributesAvailableL(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
    	
	// See @ref MGlxMediaListObserver::HandleFocusChangedL
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    
	// See @ref MGlxMediaListObserver::HandleItemSelectedL
    void HandleItemSelectedL(TInt /* aIndex */, TBool /* aSelected */, MGlxMediaList* /* aList */) { };
    
	// See @ref MGlxMediaListObserver::HandleMessageL
    void HandleMessageL(const CMPXMessage& /* aMessage */, MGlxMediaList* /* aList */) { };
    
private:// From MMPXPlaybackObserver 
             
     void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError ); 

private:
    /** Second phase constructor */
	void ConstructL();

    /**
     * Constructor
     * @param aMediaListProvider The owner of the media list to use
     */
	CGlxCommandHandlerVideoPlayback(MGlxMediaListProvider* aMediaListProvider);
	
	/** Helper function to activate the view with the relevant playback
	 * direction
	 */
	void ActivateViewL();

	/** Deactivate this command handler */
    void Deactivate();

	/** Activate this command handler */
    void DoActivateL(TInt aViewId);

	/** Sets the ToolBar to the correct state */
    void SetToolBar();
    
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage ); 
 
    void HandlePlaybackPlayerChangedL(); 
	
	/**
	 *  Retrieves playback utility and creates it if needed
	 */
	void PlaybackUtilityL();
  
private:
    // Pointer to the CGlxHuiUtility (reference counted).
	CGlxUiUtility* iUiUtility;

    // Pointer to an attribute context. We own this.
    CGlxDefaultAttributeContext* iAttributeContext;
    
    // Remembers the ViewId passed in ActivateL.
    TInt iViewId;
    
    // This is current state of the Icon displayed in the toolbar
    // Initially set to EUndefined, this forces the first update
    // After that we only update the toolbar if a change is necessary
    enum TIconDisplayState { EUndefined, EVisible, EHidden };
    TIconDisplayState iIconDisplayState;
    
    MMPXPlaybackUtility*   iPlaybackUtility; 
    MMPXViewUtility*       iViewUtility; 

 	};

#endif // C_GLXCOMMANDHANDLERVIDEOPLAYBACK_H

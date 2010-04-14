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
* Description:    Slideshow command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERSLIDESHOW_H
#define C_GLXCOMMANDHANDLERSLIDESHOW_H

#include <e32base.h>
#include <shwconstants.h>	// for playback direction
#include <glxmedialistcommandhandler.h> 
#include <mglxmedialistobserver.h> // for media list observer

// Forward Declarations
class CGlxDefaultAttributeContext;
class MGlxMediaListProvider;
class TGlxMedia;
class CGlxUiUtility;

/**
 * @class CGlxCommandHandlerSlideshow
 *
 * Command handler that either launches slideshow with the selected items
 * from a media list or launches the slideshow settings.
 * @internal reviewed 07/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS (CGlxCommandHandlerSlideshow) :
            public CGlxMediaListCommandHandler,
            public MGlxMediaListObserver
	{
public:
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aStepBack EFalse if the command handler should not change
     *	 the path's level. E.g. this may depend on whether the view's media 
     *   list contains media items or if it is a "list of lists"
     * @param aHasToolbarItem Whether Command Should be toolbar item (ETrue - if toolbar item)
     */
	IMPORT_C static CGlxCommandHandlerSlideshow* NewL(
	    MGlxMediaListProvider* aMediaListProvider, TBool aStepBack, TBool aHasToolbarItem );
	    
	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerSlideshow();
	
protected:

	/** @ref CGlxMediaListCommandHandler::ActivateL */
    void DoActivateL(TInt aViewId);
	/** @ref CGlxMediaListCommandHandler::DoExecuteL */
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);
    /// @ref CGlxMediaListCommandHandler::DoIsDisabled
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
    /// @ref CGlxMediaListCommandHandler::BypassFiltersForExecute
    TBool BypassFiltersForExecute() const;
    /// @ref CGlxCommandHandler::PopulateToolbar
    void PopulateToolbarL();
private:    // From MGlxMediaListObserver
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
                MGlxMediaList* aList);
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,
                MGlxMediaList* aList);
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes,
                MGlxMediaList* aList);
    void HandleAttributesAvailableL(TInt aItemIndex,
                const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);	
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex,
                TInt aOldIndex, MGlxMediaList* aList);
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    void HandlePopulatedL( MGlxMediaList* aList );
    
private:
    /** Second phase constructor */
	void ConstructL();

    /** Constructor */
	CGlxCommandHandlerSlideshow(MGlxMediaListProvider* aMediaListProvider,
		TBool aStepBack, TBool aHasToolbarItem );
	
	/** 
	 * Helper function to activate the view with the relevant playback
	 * direction
	 */
	void ActivateViewL( NShwSlideshow::TPlayDirection aPlaybackDirection );

	/**
	 * Called when the owning view is deactivated
	 */
	void Deactivate();	

    /**
     * To check whether the focused container contains any slideshow playable item.
     * @param aCommandId The Command id
     * @param aList MediaList reference
     * @return TBool ETrue if container is not slideshow playable
     */
	TBool IsSlideshowNotPlayableOnFocusedContainer(TInt aCommandId, MGlxMediaList& aList);
	
	/**
	 * To update the floating toolbar in case of non-touch
	 */
	void UpdateToolbar();
	
private:
    // ID of owning view 
    TInt iViewId;
    // Not owned - HUI utility 
    CGlxUiUtility* iUiUtility;
    /// Resource file offset
	TInt iResourceOffset;
	// Flag to control where in the list we should be
	TBool iStepBack;
	// Buffer used by the stream
	CBufFlat* iBufFlat;
	// Storage buffer to pass the path to the view
	HBufC* iBuffer;
    /** Whether the command is shown in the active palette */
    TBool iShowInToolbar;
	};

#endif // C_GLXCOMMANDHANDLERSLIDESHOW_H

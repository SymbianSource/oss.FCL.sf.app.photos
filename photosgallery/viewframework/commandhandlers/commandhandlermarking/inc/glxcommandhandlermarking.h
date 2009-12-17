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
* Description:    Marking command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERMARKING_H
#define C_GLXCOMMANDHANDLERMARKING_H

#include <e32base.h>
#include <glxmedialistcommandhandler.h>
#include <mglxsoftkeyhandler.h>

class MGlxMediaListProvider;

class MGlxVisualList;
class CGlxMarkedIconManager;
class MGlxSoftkeyHandler;
class CGlxUiUtility;
class CGlxVisualListManager;
class CGlxScreenFurniture;


/**
 * CGlxCommandHandlerMarking
 *
 * Marking  command handler implementation
 *
 * @lib glxcommandhandlermarking
 * @author M Byrne
 * @internal reviewed 11/06/2007 by Alex Birkett
 * @internal reviewed 19/09/2007 by Aki Vanhatalo
 */
NONSHARABLE_CLASS (CGlxCommandHandlerMarking) : public CGlxMediaListCommandHandler,
        public MGlxMediaListObserver
    {
public:
    /**
     * Static construction function
     * @param aMediaListProvider pointer to media list owner
     * @param aKeyHandler pointer to MGlxSoftkeyHandler interface
     * @param aUpdateMiddleSoftkey Should this command handler update
     *          the middle softkey when not in multiple marking mode.
     * @return pointer to CGlxCommandHandlerMarking object
     */
    IMPORT_C static CGlxCommandHandlerMarking* NewL(
                                MGlxMediaListProvider* aMediaListProvider, 
                                TBool aHasToolbarItem/*,
                                TBool aUpdateMiddleSoftkey = EFalse */);
                                
    /** Destructor */
    IMPORT_C ~CGlxCommandHandlerMarking();

public: // From CGlxCommandHandler
    void DoActivateL(TInt aViewId);
    void Deactivate();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, 
                                            MGlxMediaList* aList);
    
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
public: // From MGlxMediaListObserver
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, 
                                        MGlxMediaList* aList);
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, 
                                        MGlxMediaList* aList);
    void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, 
                        TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    
protected: // From CGlxCommandHandler
    void PopulateToolbarL();
    
protected: // From CGlxMediaListCommandHandler
    TBool DoExecuteL(TInt aCommand, MGlxMediaList& aList);
    void DoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
    
private:
    /** Current marking mode marking or unmarking */
    enum TGlxMarkingMode
        {
        EMarking,
        EUnmarking
        };

    /** status of left softkey (in mupltiple marking mode) */
    enum TGlxLeftSoftKeyStatus
        {
        EGlxLSKUndefined,
        EGlxLSKBlank, // static item
        EGlxLSKMark,
        EGlxLSKUnmark
        };
        
    /**
      * 2nd phase construction
      */
    void ConstructL();

    /**
     * Constructor
     * @param aMediaListProvider pointer to media list owner
     * @param aKeyHandler pointer to MGlxSoftkeyHandler interface
     * @param aUpdateMiddleSoftkey Should this command handler update
     *          the middle softkey when not in multiple marking mode.
     */
    CGlxCommandHandlerMarking( MGlxMediaListProvider* aMediaListProvider,
    		TBool aHasToolbarItem/* , TBool aUpdateMiddleSoftkey */);

    /**
     * Select/unselect all items 
     * @param aSelect select if ETrue unselect otherwise
     */
    void SelectAllL( TBool aSelect );

    /**
     * Enter multiple marking mode
     */
    void EnterMultipleMarkingModeL();

    /**
     * Exit multiple marking mode
     */
    void ExitMultipleMarkingModeL();

    /**
     * Update mark status of selected item
     * and update marked item count
     * @param aList media list
     * @param aIndex index of item in media list
     */
    void SetSelectedL(MGlxMediaList* aList, TInt aIndex);

private:
    /** Resource offset */
    TInt iResourceOffset;

    /**  Flag if basic marking in operation */
    TBool iBasicMarking;

    /** multiple marking mode enabled */
    TBool iMultipleMarking;

    /** Current marking mode (marking or unmarking) */
    TGlxMarkingMode iMarkingMode;

    /** visual list passed through to marking icon manager */
    MGlxVisualList* iVisualList;

    /** ID of view activating command handler */	
    TInt    iViewId;

    // add current lsk status
    TGlxLeftSoftKeyStatus iLeftSoftKeyStatus;

    /** Whether to update the MSK when not in multiple marking mode */
    TBool iUpdateMiddleSoftkey;

    /** Whether context menu is currently enabled */
    TBool iOkOptionsMenuEnabled;

    /** Current middle softkey text resource */
    TInt iMskTextResourceId;

    /**  Instance of Ui utility class  */
    CGlxUiUtility* iUiUtility;

    /**  Instance of Screenfurniture  */
    CGlxScreenFurniture* iScreenFurniture ;
    
    /** Flag to indicate whether full-screen is active or not */
    TBool iInFullScreen;
    
    HBufC* iRskTextCancel;
    HBufC* iRskTextBack;
    HBufC* iMskTextMark;
    HBufC* iMskTextUnMark;
    HBufC* iMskTextOpen;
    
    };

#endif // C_GLXCOMMANDHANDLERMARKING_H

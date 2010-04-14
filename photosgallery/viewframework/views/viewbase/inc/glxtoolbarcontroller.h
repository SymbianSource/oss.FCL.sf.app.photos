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
* Description:    Media item list observer interface 
*
*/




#ifndef GLXTOOLBARCONTROLLER_H
#define GLXTOOLBARCONTROLLER_H

#include <mglxmedialistobserver.h>
#include <akntoolbar.h>             // For Toolbar


NONSHARABLE_CLASS( CGlxToolbarController ): public CBase, public MGlxMediaListObserver
    {
public:
    /*
     * function NewL
     */
    static CGlxToolbarController* NewL ();
    
    /*
     * Adds object of CGlxToolbar to observe medialist
     * @param aList Reference of MGlxMediaList 
     */
    void AddToObserverL(MGlxMediaList& aList, CAknToolbar* aToolbar);
    
    /*
     * Removes object of CGlxToolbar from medialist observer
     * @param aList Reference of MGlxMediaList
     */
    void RemoveFromObserver (MGlxMediaList& aList);
    
    /**
     * Sets toolbar status.This is called after adding CGlxToolbarController
     * to medialist observer, if there is no HandleAttributeAvailable callback.
     * (This is done to activate toolbar when attributes are already available in cache)
     * @param aList Pointer to MGlxMediaList
     */
    void SetStatusOnViewActivationL( MGlxMediaList* aList );
     
    /*
     * Sets the toolbar status on every attribute available callback. 
     * If attributes not available, but there are items, then this call is forced
     * by SetStatusOnViewActivationL from MediaListViewBase
     * @param aList Pointer og MGlxMediaList
     */
    void SetStatusL( MGlxMediaList* aList );
  
    /*
     * Destructor
     */
    ~CGlxToolbarController();
   
public: //From MGlxMediaListObserver  
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList);
    void HandleAttributesAvailableL(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes,
            MGlxMediaList* aList);
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, 
            TInt aOldIndex, MGlxMediaList* aList);
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    void HandlePopulatedL( MGlxMediaList* aList );
    
private:
    /*
     * Default Constructor
     */
    CGlxToolbarController();
    
    /* Enables or disables latching on the toolbar button.
     * Used in Marking mode.
     * @param aCommandId The Command on toolbar that needs to be latched/unlatched.
     * aLatched ETrue if the command button needs to be latched. 
     */
    void EnableLatch( TInt aCommandId, TInt aLatched );

    /*
     * Set toolbar items dimmed.
     * @param aDimmed Flag for the toolbar items to be dimmed.
     */
    void SetToolbarItemsDimmed(TBool aDimmed);
     
private:    
    CAknToolbar* iToolbar;
    TBool iAttributeAvailable;
    };

#endif /*GLXTOOLBARCONTROLLER_H*/



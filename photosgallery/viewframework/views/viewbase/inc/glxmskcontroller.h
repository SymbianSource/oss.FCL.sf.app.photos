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



#ifndef GLXMSKCONTROLLER_H_
#define GLXMSKCONTROLLER_H_


#include <mglxmedialistobserver.h>
#include <eikbtgpc.h>  
#include  "glxviewbase.h"

class CGlxUiUtility;

NONSHARABLE_CLASS( CGlxMSKController ): public CBase, public MGlxMediaListObserver
    {
public:
    /*
     * function NewL
     */
    static CGlxMSKController* NewL ();

    /*
     * Adds object of CGlxMSKController to observe medialist
     * @param aList Reference of MGlxMediaList 
     */
    void AddToObserverL(MGlxMediaList& aList, CEikButtonGroupContainer* aCba);

    /*
     * Removes object of CGlxMSKController from medialist observer
     * @param aList Reference of MGlxMediaList
     */
    void RemoveFromObserver (MGlxMediaList& aList);

    /**
     * Sets MSK status.This is called after adding CGlxMSKController
     * to medialist observer, if there is no HandleAttributeAvailable callback.
     * @param aList Pointer to MGlxMediaList
     */
    void SetStatusOnViewActivationL( MGlxMediaList* aList );
    
    /*
     * Sets the MSK status for the Main List view. 
     */
    void SetMainStatusL();
    
    /*
     * Destructor
     */
    ~CGlxMSKController();

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

private:
    /*
     * Default Constructor
     */
    CGlxMSKController();

    /*
     * Sets the MSK status on every attribute available callback. 
     * If attributes not available, but there are items, then this call is forced
     * by SetStatusOnViewActivationL from MediaListViewBase
     * @param aList Pointer og MGlxMediaList
     */
    void SetStatusL( MGlxMediaList* aList );

private:    
    CEikButtonGroupContainer* iCba;
    TBool iAttributeAvailable;
    CGlxUiUtility* iUiUtility;
    };


#endif /*GLXMSKCONTROLLER_H_*/

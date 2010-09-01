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
* Description:    Scrollbar
*
*/




#ifndef C_GLXSCROLLBAR_H
#define C_GLXSCROLLBAR_H

#include <e32base.h>

class MGlxScrollbarObserver;
class CAlfControl;
class CAlfLayout;
class CAlfAnchorLayout;
class CGlxTextureManager;

/**
 * CGlxScrollbar
 *
 * Class to handle controlling/drawing of scrollbar
 *
 * Example: Scrollbar required for list with 10 items, with 3 visible items
 * The first visible row is at position 0.
 * The position is always from 0 to (full length - 1).
 *
 * Client calls:
 *
 * SetFullLength(10)
 * SetVisibleLength(3)
 * SetPosition(0)
 *
 * @lib glxuiutilities.lib
 *
 * @author Michael Yip
 */
class CGlxScrollbar : public CBase
    {
public:
    /**
     * Two phase constructor
     *
     * @param aControl The Alf control
     * @param aParentLayout The Alf layout that owns this scrollbar
     */
    IMPORT_C static CGlxScrollbar* NewL(CAlfControl& aControl, CAlfLayout& aParentLayout);

    /**
     * Two phase constructor
     *
     * @param aControl The Alf control
     * @param aParentLayout The Alf layout that owns this scrollbar
     */
    IMPORT_C static CGlxScrollbar* NewLC(CAlfControl& aControl, CAlfLayout& aParentLayout);

    /**
     * Add an observer to observe scrollbar
     * Observers are notified if the scrollbar is scrollable
     *
     * @param aObserver The observer
     */
    IMPORT_C void AddObserverL(MGlxScrollbarObserver* aObserver);

    /**
     * Remove an observer
     *
     * @param aObserver The observer
     */
    IMPORT_C void RemoveObserver(MGlxScrollbarObserver* aObserver);

    /**
     * Sets the visible length of the scrollbar
     *
     * @param aVisibleLength The visible length
     */
    IMPORT_C void SetVisibleLength(TUint aVisibleLength);

    /**
     * Sets the full length of the scrollbar
     *
     * @param aFullLength The full length
     * @param aTransitionTime The transition time in milliseconds if any animation is required
     */
    IMPORT_C void SetFullLength(TUint aFullLength, TUint aTransitionTime = 0);

    /**
     * Sets the position of the first visible row in the full length
     *
     * @param aPosition The position
     * @param aTransitionTime The transition time in milliseconds if any animation is required
     */
    IMPORT_C void SetPosition(TUint aPosition, TUint aTransitionTime = 0);

    /**
     * Destructor
     */
    ~CGlxScrollbar();

private:
    /**
     * Constructor
     */
    CGlxScrollbar();

    /**
     * Two phase constructor
     *
     * @param aControl The Alf control
     * @param aParentLayout The Alf layout that owns this scrollbar
     */
    void ConstructL(CAlfControl& aControl, CAlfLayout& aParentLayout);

    /**
     * Set the textures for the visuals
     */
    void SetTexturesL();

    /**
     * Set the textures for the visuals in the background layout
     *
     * @param aTextureManager The texture manager for creating textures
     */
    void SetBackgroundTexturesL(CGlxTextureManager& aTextureManager);

    /**
     * Set the textures for the visuals in the thumb layout
     *
     * @param aTextureManager The texture manager for creating textures
     */
    void SetThumbTexturesL(CGlxTextureManager& aTextureManager);

    /**
     * Set the background or thumb layout anchors
     *
     * @param aLayout The anchor layout to set the anchors for
     */
    void SetAnchors(CAlfAnchorLayout& aLayout);

    /**
     * Update the scrollbar
     *
     * @param aTransitionTime The transition time in milliseconds if any animation is required
     */
    void Update(TUint aTransitionTime = 0);

    /**
     * Notify observers if the scrollbar is scrollable
     */
    void NotifyObservers();

private:
    /// The Alf layout for the scrollbar
    /// Ownership passed to parent layout
    CAlfLayout* iLayout;

    /// The visible length of the scrollbar
    TUint iVisibleLength;

    /// The full length of the scrollbar
    TUint iFullLength;

    /// The position of the first visible row in the full length
    TUint iPosition;

    /// Observers of the scrollbar
    RPointerArray<MGlxScrollbarObserver> iObservers;
    };

#endif // C_GLXSCROLLBAR_H

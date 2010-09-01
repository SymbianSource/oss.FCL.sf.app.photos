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
* Description:    Icon manager for DRM rights icons
*
*/




/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */

#ifndef C_GLXDRMICONMANAGER_H
#define C_GLXDRMICONMANAGER_H

#include <e32base.h>

#include "glxiconmanager.h"

/**
 *  CGlxDRMIconManager
 * 
 *  Icon manager for visuals
 *  Handles adding default or broken icon if required.
 *  If thumbnail not present look for icon attribute
 *  @author M Byrne
 *  @lib glxvisuallistmanager 
 */
struct TIconInfo;
class CAlfVisual;
class CGlxDRMUtility;
class TGlxMediaId;
class CAlfTexture;

NONSHARABLE_CLASS( CGlxDrmIconManager )  : public CGlxIconManager
    {
public:
    /**
      * Static constructor
      * @param aMediaList reference to media list
      * @param aVisualList reference to associated visual list
      * @return pointer to CGlxVisualIconManager instance
      */
    IMPORT_C static CGlxDrmIconManager* NewL(MGlxMediaList& aMediaList,
                                    MGlxVisualList& aVisualList);
    
    /**
      * Destructor
      */
    IMPORT_C ~CGlxDrmIconManager();
    
    /**
      * Remove overlay icon (when enter full-screen)
      * @param aIndex list index of item
      * @param aInvalidIcon ETrue if DRM invalid icon present
      */
    void RemoveOverlayIcon(TInt aIndex, TBool aInvalidIcon);
    
    /**
      * Remove overlay icons (when exit full-screen)
      */
    void AddOverlayIconsL();
    
private: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleItemAddedL
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMediaL
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemRemovedL
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemModifiedL
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleAttributesAvailableL
    void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
            
    /// See @ref MGlxMediaListObserver::HandleFocusChangedL
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemSelectedL
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMessageL
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    
private: // from MGlxVisualListObserver
    /** @see MGlxVisualListObserver::HandleFocusChangedL */
    void HandleFocusChangedL( TInt aFocusIndex, TReal32 aItemsPerSecond, MGlxVisualList* aList, NGlxListDefs::TFocusChangeType aType );
    /** @see MGlxVisualListObserver::HandleSizeChanged */
    void HandleSizeChanged( const TSize& aSize, MGlxVisualList* aList );
    /** @see MGlxVisualListObserver::HandleVisualRemoved */
    void HandleVisualRemoved( const CAlfVisual* aVisual,  MGlxVisualList* aList );
    /** @see MGlxVisualListObserver::HandleVisualAddedL */
    void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList );

private:
    /**
      * Identifiers for DRM icons
      */
      enum TGlxVisualIcon
        {
        EDRMRightsValidIcon,
        EDRMRightsInvalidIcon
        };
    
private:
    /**
      * Constructor
      * @param aMediaList pointer to media list
      * @param aVisualList reference to associated visual list
      */
    CGlxDrmIconManager(MGlxMediaList& aMediaList, MGlxVisualList& aVisualList);
    
    /**
      * 2nd phase construction
      */
    void ConstructL();
    
    /**
      * Check if thumbnail icon has already been generated
      * @param aIconInfo TIconInfo struct for icon
      * @return position of icon in iThumbnailIcons, 
      * KErrNotFound if icon not present
      */
    TInt ThumbnailIconArrayPos(TIconInfo& aIconInfo); 
    

    /**
      * Check if thumbnail attribute is present for specifed vis.
      * If not add icon if present or add default icon
      * @param pointer to visual may be NULL
      * @param aIndex index of item is media list
      * @param aForceIcon force icon to be draw
      */ 
    void CheckDRMRightsL(CAlfVisual* aVisual, 
                    TInt aIndex, TBool aForceIcon = ETrue);
    
    /**
     * Add item to array of items with valid DRM icons
     * @param aId id of item
     */
    void AddValidIconL(TGlxMediaId& aId);
    
    /**
     * Add item to array of items with invalid DRM icons
     * @param aId id of item
     */
    void AddInvalidIconL(TGlxMediaId& aId);
    
    /**
    * Check if item should have DRM icon of specified type
    * @param aId id of item
    * @param icon type (valid or invalid)
    * @return ETrue if should have icon
    */
    TBool ShouldHaveIcon(TGlxMediaId& aId, TGlxVisualIcon& aType);
      
private:
    /** Maintain count of number of icons handled by manager */
    TInt iIconCount;
    
    // array of thumbnail icons added via icon attribute
    RArray<TIconInfo> iThumbnailIcons;
    
    /** Pointer to DRM utulity class owned */
    CGlxDRMUtility* iDRMUtility;
    
    /** Array of items with valid DRM status */
    RArray<TGlxMediaId> iValidDRMItems;
    
    /** Array of items with invalid DRM status */
    RArray<TGlxMediaId> iInvalidDRMItems;
    
    CAlfTexture* iDRMSendForbidTexture;
    
    CAlfTexture* iDRMRightsExpiredTexture;
    };
    
#endif // C_GLXDRMICONMANAGER_H

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
* Description:    Manages icons for basic marking
*
*/




#ifndef C_GLXMARKEDICONMANAGER_H
#define C_GLXMARKEDICONMANAGER_H

#include <e32base.h>

#include <glxiconmanager.h>

/**
 *  CGlxMarkedIconManager
 * 
 *  Marking Icon manager
 *
 *	@lib glxcommandhandlermarking 
 *  @author M Byrne
 *  @internal reviewed 12/06/2007 by Alex Birkett
 */
class CGlxMarkedIconManager : public CGlxIconManager
	{
public:
    /**
     * static construction
     * @param aMediaList reference to media list
     * @param aVisualList reference to associated visual list
     * @return pointer to CGlxMarkedIconManager instance
     */
	static CGlxMarkedIconManager* NewL(MGlxMediaList& aMediaList,
	                                       MGlxVisualList& aVisualList);
	
	/**  destructor */	
	~CGlxMarkedIconManager();
	
	/**
	  * cause the marked and unmarked items to be shown with the required overlays.
	  * @param aMultipleModeEnabled set whether multiple marking mode is
	  *         enabled or disabled.
	  */
	void SetMultipleMarkingModeIndicatorsL(TBool aMultipleModeEnabled);
	
private: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleItemAddedL
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, 
                                        MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMediaL
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemRemovedL
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, 
                                        MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemModifiedL
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, 
                                        MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleAttributesAvailableL
    void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
            
    /// See @ref MGlxMediaListObserver::HandleFocusChangedL
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, 
                                                        MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemSelectedL
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMessageL
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
	
private: // from MGlxVisualListObserver
	/** @see MGlxVisualListObserver::HandleFocusChangedL */
	void HandleFocusChangedL( TInt aFocusIndex, TReal32 aItemsPerSecond, 
	                                            MGlxVisualList* aList, 
	                                            NGlxListDefs::TFocusChangeType aType );
	/** @see MGlxVisualListObserver::HandleSizeChanged */
	void HandleSizeChanged( const TSize& aSize, MGlxVisualList* aList );
	/** @see MGlxVisualListObserver::HandleVisualRemoved */
	void HandleVisualRemoved( const CAlfVisual* aVisual,  MGlxVisualList* aList );
	/** @see MGlxVisualListObserver::HandleVisualAddedL */
	void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList );
	
private:
    /**
      * Identifiers for marking icons
      */
      //enum TGlxMarkIcon
      //  {
        //EMarkIcon,
        //EUnmarkOverlay // semi-transparent overlay for multi.mark mode
      //  };
    
    	
private:
    /**
     * Constructor
     * @param aMediaList reference to media list
     * @param aVisualList reference to associated visual list
     */
	CGlxMarkedIconManager(MGlxMediaList& aMediaList,
	                                       MGlxVisualList& aVisualList);
	
	/**
	  * 2nd phase constructor  
	  */
	void ConstructL();
	
	
private:
    // texture for marking tick, not owned
    CAlfTexture* iMarkTexture;

    // texture for semi-transparent overlay in multiple marking mode, owned
    CAlfTexture* iBlackTexture;
    
    // flag whether multiple marking is active
    TBool iMultipleMarkingEnabled;
	};
	
#endif // C_GLXMARKEDICONMANAGER_H

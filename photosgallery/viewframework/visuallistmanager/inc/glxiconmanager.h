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
* Description:    Icon manager base class
*
*/




#ifndef C_GLXICONMANAGER_H
#define C_GLXICONMANAGER_H

#include <e32base.h>

#include <mglxmedialistobserver.h>
#include "mglxvisuallistobserver.h"

/**
 * @internal reviewed 31/07/2007 by Rowland Cook
 */

/**
 * @internal re-reviewed 09/08/2007 by Rowland Cook
 */

/**
 *  TGlxIconManagerParams
 * 
 *  Container class for icon/brush parameters passed to 
 *  icon manager. (Currently only holds co-ordinates for
 *  brush object relative to its parent visual.)
 *
 *  @author M Byrne
 *	@lib glxvisuallistmanager 
 */ 
NONSHARABLE_CLASS( TGlxIconManagerParams )
	{
public:
    /**
      * TGlxIconManagerParams constructor
      * @param aTlX top left x pos. for icon (relative to parent visual)
      * @param aTlY top left y pos. for icon (relative to parent visual)
      * @param aBrX lower right x pos. for icon (relative to parent visual)
      * @param aBrY lower right y pos. for icon (relative to parent visual)
      * @param aOpacity opacity
      */
	IMPORT_C TGlxIconManagerParams(TReal32 aTlX, TReal32 aTlY, 
	                                TReal32 aBrX, TReal32 aBrY,
	                                TReal32 aOpacity = 1.0);
public:
	TReal32 iTlX;
	TReal32 iTlY;
	TReal32 iBrX;
	TReal32 iBrY;
	TReal32 iOpacity;
	};


/**
 *  CGlxIconManager
 * 
 *  Icon manager base class
 * 
 *  @author M Byrne
 *	@lib glxvisuallistmanager 
 */  
class CGlxUiUtility;
class CAlfBrush;
class CAlfImageBrush;
class CGlxVisualListManager;
class CAlfTexture;

class CGlxIconManager : public CBase, public MGlxMediaListObserver, 
                                            public MGlxVisualListObserver
	{
public:
	                                                
    /** 
      * Destructor
      */	                                                
	IMPORT_C virtual ~CGlxIconManager();
	
	/**
	 * Base construction
	 */
	IMPORT_C void BaseConstructL();
	
	/**
	 * Create image brush from supplied bitmap
	 * @param aIconResourceId id for resource (from .mbg)
	 * @param aFilename full filename for .mbm
	 */
	IMPORT_C virtual void CreateVisualFromIconL(TInt aIconResourceId, 
	                                                    TDesC& aFilename);
	
	/**
	 * Create image brush from supplied bitmap
	 * @param aIconResourceId id for resource (from .mbg)
	 * @param aFilename full filename for .mbm
	 * @param aParams position and opacity params for icon
	 */
	IMPORT_C virtual void CreateVisualFromIconL(TInt aIconResourceId, 
	                    TDesC& aFilename, TGlxIconManagerParams& aParams);

	/**
	 * Create image brush from supplied texture
	 * @param aTexture Texture to apply to the new image brush
	 */
    IMPORT_C virtual void CreateVisualFromTextureL( CAlfTexture& aTexture );
    
    /**
	 * Create image brush from supplied texture
	 * @param aTexture Texture to apply to the new image brush
	 * @param aParams position and opacity params for icon
	 */
    IMPORT_C virtual void CreateVisualFromTextureL(
        CAlfTexture& aTexture, TGlxIconManagerParams& aParams );
	
	/*
	 * Add icon to visual at specified position
	 * @param aListIndex index in visual list
	 * @param aBrushIndex index of item to add
	 */	
	IMPORT_C virtual void AddIconToItemL(TInt aListIndex, TInt aBrushIndex);
	
	/*
	 * Add icon to visual at specified position
	 * @param aVisual pointer to visual
	 * @param aBrushIndex index of item to add
	 */	
	IMPORT_C virtual void AddIconToItemL(CAlfVisual* aVisual, TInt aBrushIndex);
	
	/*
	 * Remove icon from visual at specified position
	 * @param aListIndex index in visual list
	 * @param aBrushIndex index of item to remove
	 */
	IMPORT_C virtual void RemoveFromItem(TInt aListIndex, TInt aBrushIndex);
	
	/*
	 * Remove icon from visual at specified position
	 * @param aVisual pointer to visual
	 * @param aBrushIndex index of item to remove
	 */
	IMPORT_C virtual void RemoveFromItem(CAlfVisual* aVisual, 
	                                                TInt aBrushIndex);
	
	/**
	  * Check for position of specifed brush in visual's brush array
	  * @param aListIndex position of visual in visual list
	  * @param aBrushIndex index of brush in icon mgr's array of brushes
	  * @return position of brush in visual's brush array or KErrNotFound
	  *         if brush not present in visual
	  */
	 IMPORT_C virtual TInt BrushPositionInVisual(TInt aListIndex, 
	                                                TInt aBrushIndex); 
	 
	 /**
	  * Check for position of specifed brush in visual's brush array
	  * @param aListIndex position of visual in visual list
	  * @param aBrushIndex index of brush in icon mgr's array of brushes
	  * @return position of brush in visual's brush array or KErrNotFound
	  *         if brush not present in visual
	  */
	 IMPORT_C virtual TInt BrushPositionInVisual(CAlfVisual* aVisual, 
	                                                   TInt aBrushIndex);
	                                                   
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
	
protected:
    /**
      * Constructor
      * @param aMediaList reference to media list
      * @param aVisualList reference to associated visual list
      * @return pointer to CGlxIconManager
      */
	IMPORT_C CGlxIconManager( MGlxMediaList& aMediaList, MGlxVisualList& aVisualList );

	
protected:
    /** Reference to media list */
	MGlxMediaList&						iMediaList;	
	
	/** Reference to visual list */
	MGlxVisualList& 					iVisualList;
		
	/** UI utility, reference */	
	CGlxUiUtility* 					iUiUtility;
	/** Array of image brushes created by this icon mgr */
	RPointerArray< CAlfBrush >		iImageBrushArray;	
	};
	
#endif // C_GLXICONMANAGER_H

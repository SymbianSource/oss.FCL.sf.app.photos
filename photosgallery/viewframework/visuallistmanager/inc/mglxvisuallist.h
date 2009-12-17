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
* Description:    Interface to visual list
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#ifndef M_GLXVISUALLIST_H
#define M_GLXVISUALLIST_H

#include <glxid.h>
#include <glxlistdefs.h>
#include "glxiconmgrdefs.h"

class CGlxVisualObject;
class MGlxLayout;
class MGlxVisualListObserver;
class CAlfControlGroup;
class CAlfTexture;
class CAlfVisual;

class TGlxVisualListIdBase {}; 
typedef TGlxId<TGlxVisualListIdBase> TGlxVisualListId;

/**
 * View Context id
 */ 
class TGlxIdViewContextBase {}; // Don't use this. This is just to make TGlxId concrete
typedef TGlxId<TGlxIdViewContextBase> TGlxViewContextId; // Use this



/**
 *  MGlxVisualList
 * 
 *  Interface to a visual list.
 *  Used to separate the data model from layout and key handling controls.
 *
 * @author Aki Vanhatalo
 */
class MGlxVisualList
	{
public:
	/**
	 * @returns the id of the list
	 */
	virtual TGlxVisualListId Id() const = 0;

	/**
	 * @returns a visual by list index
	 */	
	virtual CAlfVisual* Visual(TInt aListIndex) = 0;

    /**
     * @returns a visual item by list index
     */	
    virtual CGlxVisualObject* Item(TInt aListIndex) = 0;

	/**
	 * @returns the count of visuals in window
	 */	
	virtual TInt ItemCount(NGlxListDefs::TCountType aType = NGlxListDefs::ECountAll) const = 0; 

	/**
	 * @returns the focus index
	 */	
	virtual TInt FocusIndex() const = 0; 

	/**
	 * @returns the control group for the visual list
	 *			The layout/eventhandler controls should add themselved 
	 *			to this same group. View should show this group when
	 * 			it is activated and hide it when it is deactivated.
	 */
	virtual CAlfControlGroup* ControlGroup() const = 0;

	/**
	 * Add/remove an observer
	 */
    virtual void AddObserverL(MGlxVisualListObserver* aObserver) = 0;
	virtual void RemoveObserver(MGlxVisualListObserver* aObserver) = 0;
	
	/**
	 * Add a layout 
	 * The layout is used when layout out the visuals owned by the
	 * MViuVisualOwner-derived class (or its helper)
	 */
	virtual void AddLayoutL(MGlxLayout* aLayout) = 0;
		
	/**
	 * Remove an existing layout
	 */
	virtual void RemoveLayout(const MGlxLayout* aLayout) = 0;
	
    /**
     * Defines a context in terms of range offsets
     *
     * @param aFrontVisibleRangeOffset The front offset to focus for items that are 
     *								   visible in the UI. Must be less or equal to 0.
     * @param aRearVisibleRangeOffset  The front offset to focus for items that are 
     *								   visible in the UI. Must be greater or equal to 0. 
     * @returns Unique id of the context
     */
	virtual TGlxViewContextId AddContextL(TInt aFrontVisibleRangeOffset, 
		TInt aRearVisibleRangeOffset) = 0;

    /**
     * Removes an existing view context. Forwards the request to List Reader.
     * If you are relying on the visual owner control, use this function to 
     * remove a context instead of calling the list reader directly.
     *
     * @param aContextId Id of the context to be removed
     */
	virtual void RemoveContext(const TGlxViewContextId& aContextId) = 0;
		
	/**
	 * Navigate either forward or backward
	 * @param aIndexCount amount of indexes to navigate
	 *		  			  if positive, navigates forward
	 *					  if negative, navigates backward
	 */
 	virtual void NavigateL(TInt aIndexCount) = 0;
 	
 	/**
 	 * @returns size of the control area
 	 */
 	virtual TSize Size() const = 0;
 	
 	/**
 	 * Brings the visuals to the front of the display
 	 */
 	virtual void BringVisualsToFront() = 0;
 	
    /**
     * Start or stop animation of given item, if animation available
     * @param aAnimate If true, starts the animation if availble; if false, stops it
     * @param aIndex Index of the item to animate
     */
    virtual void EnableAnimationL(TBool aAnimate, TInt aIndex) = 0;
    
    /**
     * Specifies whether the visual list shows default icons or not
     * @param aEnable true to show the default icons, false to not show
     */
    virtual void SetDefaultIconBehaviourL( TBool aEnable ) = 0;
    
    /**
    * Add icon to specified visual
    * @param aListIndex position in list
    * @param aTexture texture of icon to apply
    * @param aIconPos position of icon (centred or relative to a specific corner)
    * @param aForeground foreground or background icon
    * @param aStretch ETrue if icon is scaled to visual, EFalse if texture is fixed size
    * @param aBorderMargin margin from specified corner
    * @param aWidth width for anchor when icon is stretched
    * @param aHeight height for anchor when icon is stretched
    */
    virtual void AddIconL( TInt aListIndex, const CAlfTexture& aTexture, 
            NGlxIconMgrDefs::TGlxIconPosition aIconPos,
            TBool aForeground, TBool aStretch,  TInt aBorderMargin,
            TReal32 aWidth = 1.0, TReal32 aHeight = 1.0 ) = 0;
            
    /**
    * Remove icon from specified visual
    * @param aListIndex position in list
    * @param aTexture texture of icon to remove
    * @return ETrue if texture was present
    */
    virtual TBool RemoveIcon( TInt aListIndex, const CAlfTexture& aTexture ) = 0;
    
    /**
    * Hide or show icon if required
    * @param aListIndex position in list
    * @param aTexture texture of icon
    * @param aVisible Set hidden or show
    */
    virtual void SetIconVisibility( TInt aListIndex, const CAlfTexture& aTexture, 
                                                        TBool aVisible ) = 0;
	};
	
	
#endif // M_GLXVISUALLIST_H

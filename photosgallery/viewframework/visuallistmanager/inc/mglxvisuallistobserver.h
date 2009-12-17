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
* Description:    Visual list observer interface
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#ifndef M_GLXVISUALLISTOBSERVER_H
#define M_GLXVISUALLISTOBSERVER_H

#include <e32std.h>
#include <glxlistdefs.h>

class MGlxVisualList;
class CAlfVisual;

/**
 *  MGlxVisualListObserver 
 * 
 *  Observer to a list of visuals
 *
 * @author Aki Vanhatalo
 */
class MGlxVisualListObserver 
	{
public:

	/**	
	 * Focus has changed.
	 * @param aFocusIndex new focus index
	 * @param aItemsPerSecond speed of focus change
	 * @param aList The visual list to which this notification relates
	 * @param aType Type/direction of focus change
	 */
	virtual void HandleFocusChangedL(TInt aFocusIndex, TReal32 aItemsPerSecond,
		MGlxVisualList* aList, NGlxListDefs::TFocusChangeType aType) = 0;
 
	/**
	 * Size of the rendering area has changed
	 * @param aSize new size of the area
	 * @param aList The visual list to which this notification relates
	 */	
	virtual void HandleSizeChanged(const TSize& aSize,
		MGlxVisualList* aList) = 0;

	/**
	 * Visual is being removed. The visual instance will be 
	 * destroyed after this call returns.
	 * @param aVisual The visual which is being removed
	 * @param aList The visual list to which this notification relates
	 */
	virtual void HandleVisualRemoved(const CAlfVisual* aVisual, 
		MGlxVisualList* aList) = 0;

	/**
	 * Visual has been added
	 * @param aVisual The new visual
	 * @param aIndex The list index of the new visual
	 * @param aList The visual list to which this notification relates
	 */
	virtual void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex, 
		MGlxVisualList* aList) = 0;
	};

#endif // M_GLXVISUALLISTOBSERVER_H

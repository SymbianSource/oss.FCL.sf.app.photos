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
* Description:    Adds a border to icons in visual lists
*
*/




/**
 * @internal reviewed 31/07/2007 by Rowland Cook
 */
 
 
#ifndef C_GLXBORDERICONMANAGER_H
#define C_GLXBORDERICONMANAGER_H

#include <e32base.h>

#include "glxiconmanager.h"

/**
 *  CGlxBorderIconManager
 * 
 *  Icon manager for visuals
 *  Handles adding video icon if required.
 *  If thumbnail not present look for icon attribute
 *  @author D Schofield
 *	@lib glxvisuallistmanager 
 */
struct TIconInfo;
class CAlfVisual;
class TGlxMediaId;
class CGlxMedia;
class CAlfBorderBrush;

NONSHARABLE_CLASS( CGlxBorderIconManager ) : public CGlxIconManager
	{
public:
    /**
      * Static constructor
      * @param aMediaList reference to media list
      * @param aVisualList reference to associated visual list
      * @return pointer to CGlxVideoIconManager instance
      */
	IMPORT_C static CGlxBorderIconManager* NewL(MGlxMediaList& aMediaList,
	                                            MGlxVisualList& aVisualList);
	/**
	  * Destructor
	  */
	~CGlxBorderIconManager();
	
    /**
      *Get the cached color and set it as the thumbnail border color.
      */
	IMPORT_C void SetThumbnailBorderColor();
private: // from MGlxVisualListObserver
	void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList );
	
private:
    /**
      * Constructor
      * @param aMediaList reference to media list
      * @param aVisualList reference to associated visual list
      */
	CGlxBorderIconManager(MGlxMediaList& aMediaList, 
	                            MGlxVisualList& aVisualList);
	                            
    /**
      * 2nd phase constructor
      */	                            
	void ConstructL();

    /**
     * Add a border brush to the visual]
     * @param aVisual Visual to add the border to
     */
    void AddBorderBrushL(CAlfVisual* aVisual);
	
private:
    /// Owned: the border brush
    CAlfBorderBrush* iBorderBrush;	
	};
	
#endif // C_GLXBORDERICONMANAGER_H

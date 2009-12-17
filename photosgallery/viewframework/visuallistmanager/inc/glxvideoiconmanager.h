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
* Description:    Icon manager for visuals
*
*/




/**
 * @internal reviewed 31/07/2007 by Rowland Cook
 */
/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#ifndef C_GLXVIDEOICONMANAGER_H
#define C_GLXVIDEOICONMANAGER_H

#include <e32base.h>

#include "glxiconmanager.h"

class CAlfVisual;
class CAlfTexture;

/**
 *  CGlxVideoIconManager
 * 
 *  Icon manager for visuals
 *  Handles adding video overlay if required.
 *  
 * @author Dave Holland
 */
NONSHARABLE_CLASS ( CGlxVideoIconManager )  : public CGlxIconManager
	{
public:
    /**
      * Static constructor
      * @param aMediaList reference to media list
      * @param aVisualList reference to associated visual list
      * @return pointer to CGlxVideoIconManager instance
      */
	IMPORT_C static CGlxVideoIconManager* NewL(MGlxMediaList& aMediaList,
	                                            MGlxVisualList& aVisualList);

	/**
	  * Destructor
	  */
	~CGlxVideoIconManager();
	
private: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleAttributesAvailableL
    void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
            	
private: // from MGlxVisualListObserver
    void HandleFocusChangedL( TInt aFocusIndex, TReal32 aItemsPerSecond, 
                MGlxVisualList* aList, NGlxListDefs::TFocusChangeType aType );

	/** @see MGlxVisualListObserver::HandleVisualAddedL */
	void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList );

	
private:
    /**
      * Constructor
      * @param aMediaList reference to media list
      * @param aVisualList reference to associated visual list
      */
	CGlxVideoIconManager(MGlxMediaList& aMediaList, 
	                            MGlxVisualList& aVisualList);
	                            
    /**
      * 2nd phase constructor
      */	                            
	void ConstructL();
	
	/**
	  * Check if thumbnail attribute is present for specifed vis.
	  * If not add icon if present or add default icon
	  * @param aIndex index of item in media list
	  */ 
    void AddIconIfVideoL( TInt aIndex );   
    
    
private:
    // Texture for large video overlay not owned
    CAlfTexture*    iLargeVideoIcon;
    
    // Texture for small video overlay not owned
    CAlfTexture*    iSmallVideoIcon;
    
    // focus index
    TInt iFocusIndex;
	};
	
#endif // C_GLXVIDEOICONMANAGER_H

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
* Description:    Manager of visual lists
*
*/





/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#ifndef C_GLXVISUALLISTMANAGER_H
#define C_GLXVISUALLISTMANAGER_H

#include <alf/alfimagevisual.h>

class MGlxVisualList;
class CGlxVisualListControl;
class MGlxMediaList;
class CAlfEnv;
class CAlfDisplay;

/**
 *  MGlxVisualListManager
 * 
 *  Manager of lists of visuals
 *
 * @author Aki Vanhatalo
 */
class CGlxVisualListManager : public CBase
	{
public:
    /*
     * @return pointer to instance of visual list manager
     */
	IMPORT_C static CGlxVisualListManager* ManagerL();
	
	/**
	  * Close visual list manager
	  */
	IMPORT_C void Close();
	
	/**
	  * Destructor
	  */
	~CGlxVisualListManager();
	
	/**
	 * Allocate new visual list
	 * @param aItemList ponter to media list
     * @param aEnv HuiEnv
     * @param aDisplay HUI display
     * @return pointer to visual list
	 */
	IMPORT_C MGlxVisualList* AllocListL(
	    MGlxMediaList& aItemList, 
    	CAlfEnv& aEnv, CAlfDisplay& aDisplay,
    	CAlfImageVisual::TScaleMode aThumbnailScaleMode = CAlfImageVisual::EScaleFitInside );
    	
    /**
     * Return visual list associated with specified Media List
     * Allocate new list if one does not already exist
     * @param aItemList ponter to media list
     * @param aEnv HuiEnv
     * @param aDisplay HUI display
     * @return pointer to visual list
     */
     IMPORT_C MGlxVisualList* ListL(
        MGlxMediaList& aItemList, 
    	CAlfEnv& aEnv, CAlfDisplay& aDisplay, 
    	CAlfImageVisual::TScaleMode aThumbnailScaleMode = CAlfImageVisual::EScaleFitInside );
    	
	/**
	 * Release a reference to a visual list
	 * @param aList list to be released or NULL. 
     *			    If NULL, does nothing.
	 */
	IMPORT_C void ReleaseList(MGlxVisualList* aList);
	
private:
    /**
      * Constructor
      */
	CGlxVisualListManager();
	
private:
    /** Array of visual lists (owned) */
	RPointerArray<CGlxVisualListControl> iVisualLists;
	};
	
#endif // C_GLXVISUALLISTMANAGER_H


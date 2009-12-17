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
 

#ifndef C_GLXVISUALICONMANAGER_H
#define C_GLXVISUALICONMANAGER_H

#include <e32base.h>

#include "glxiconmanager.h"
#include <mglxanimationobserver.h>
#include "mglxvisualobjectstatusobserver.h"
#include <alf/alftexture.h>
#include <glxmedia.h>

// FORWARD DECLARES
struct TIconInfo;
class CAlfVisual;
class TGlxMediaId;
class CAlfTexture;
class CGlxTextureManager;

/**
 *  CGlxVisualIconManager
 * 
 *  Icon manager for visuals
 *  Handles adding default or broken icon if required.
 *  If thumbnail not present look for icon attribute
 *  @author M Byrne
 *	@lib glxvisuallistmanager 
 */
NONSHARABLE_CLASS( CGlxVisualIconManager ) : public CGlxIconManager,
        public MGlxAnimationObserver, public MGlxVisualObjectStatusObserver
	{
	public:
	    /**
	      * Static constructor
	      * @param aMediaList reference to media list
	      * @param aVisualList reference to associated visual list
	      * @return pointer to CGlxVisualIconManager instance
	      */
		IMPORT_C static CGlxVisualIconManager* NewL(MGlxMediaList& aMediaList,
		                                            MGlxVisualList& aVisualList);

		/**
		 * Destructor
		 */	
		IMPORT_C ~CGlxVisualIconManager();
	
		/**
		 * Trigger the image-loading animation
		 * @param aItemIndex Index of item to animate
		 */
	    IMPORT_C void AnimateImageLoadedL(TInt aItemIndex);

	public: // from MGlxAnimationObserver
		virtual void AnimationComplete(MGlxAnimation* aAnimation);

    private:    // From MGlxVisualObjectStatusObserver
        void VisualStatusChangedL( CGlxVisualObject& aObject,
                                TInt aListIndex, TBool aHasThumbnail );
        void ThumbnailLoadedL( CGlxVisualObject& aObject );

	private:
	    /**
	      * Identifiers for default and broken icons
	      */
	      enum TGlxVisualIcon
	        {
	        EDefaultIcon,
	        EBrokenIcon,
	        EEmptyIcon,
	        EBlackBrush
	        };
		
	private:
	    /**
	      * Constructor
	      * @param aMediaList refernce to media list
	      * @param aVisualList reference to associated visual list
	      */
	    CGlxVisualIconManager(
	        CGlxTextureManager& aTextureManager,
	        MGlxMediaList& aMediaList, 
	        MGlxVisualList& aVisualList );
		void ConstructL();
		
		/**
		  * Check if thumbnail attribute is present for specifed vis.
		  * If not add icon if present or add default icon
		  * @param aIndex index of item in media list
		  */ 
		void CheckThumbnailAttributesL( TInt aIndex );

		/**
		 * Trigger the image-loading animation
		 * @param aItemVisual Visual of item to animate
		 */
	    void AnimateImageLoadedL(CAlfVisual* aItemVisual);
	    /**
	     * Helper function used be HandleError
	     * @param aError the error code to handle.
	     */
	    void DoHandleErrorL( TInt aError );

        /**
         * Select the icon to be used for a visual.
         * @param aIndex Index of item in media list
         * @return Pointer to an icon texture.
         */
        const CAlfTexture* SelectIconTextureL( TInt aIndex );

        /**
         * Helper method to compare two icons
         */
        static TBool MatchIcon( const TIconInfo& aIcon1,
                                const TIconInfo& aIcon2 );
        
        /**
         * Helper function to determine if an item is a DRM protected video.
         * @param aItem, the item to test
         * @return ETrue if the item is a DRM protected video, EFalse otherwise
         */
     	TBool IsDrmVideoItem( const TGlxMedia& aItem );

	private:
		/** @see MGlxVisualListObserver::HandleVisualAddedL */
		void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList );
	    
	    /** @see MGlxMediaListObserver::HandleError */
	    void HandleError( TInt aError );

	private:
        /// @ref texture manager
	    CGlxTextureManager& iTextureManager;
	    
	    // array of thumbnail icons added via icon attribute
	    RArray<TIconInfo> iThumbnailIcons;

		/// White texture for image-loading "flash" effect
	    CAlfTexture* iWhiteTexture;
	    
	    /// List of current animations, maintained in address order
	    RPointerArray<MGlxAnimation> iAnimations;

        // texture for default icon overlay not owned
	    CAlfTexture* iDefaultIconTexture;
	    
	    // texture for broken icon overlay not owned
	    CAlfTexture* iBrokenIconTexture;
	    
	    // texture for empty icon overlay not owned
	    CAlfTexture* iEmptyIconTexture;
	    
	    // array of icon textures (textures not owned)
	    RPointerArray< CAlfTexture > iIconTextureArray;
	};
	
#endif // C_GLXVISUALICONMANAGER_H

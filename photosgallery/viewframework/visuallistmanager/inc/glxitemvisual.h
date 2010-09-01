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
* Description:    A visual representing an image/video/etc thumbnail
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#ifndef C_GLXITEMVISUAL_H
#define C_GLXITEMVISUAL_H

#include <e32base.h>
#include <alf/alfimagevisual.h> // for CAlfImageVisual::TScaleMode
#include <alf/alfanchorlayout.h> // for anchor types
#include <alf/alfimage.h> // TAlfImage
//	FORWARD DECLARATIONS
class CAlfAnchorLayout;
class CAlfDeckLayout;
class CAlfLayout;
class CAlfControl;
class CAlfBrush;

class TMPXAttribute;


class CAlfDisplay;
class CAlfTexture;
class CAlfImageBrush;
class CGlxUiUtility;
class TGlxMedia;
class TGlxAnchors;
//	INCLUDES
#include <e32base.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfimage.h> // TAlfImage
#include <alf/alfimagevisual.h>
#include <glxlayouthelper.h>
#include <glxmediaid.h>
#include <mglxtextureobserver.h>
class MGlxMediaList;
#include "glxiconmgrdefs.h"

class MGlxVisualObjectStatusObserver;

/**
 * MGlxVisualObjectParameterFactory
 * Interface for the CGlxVisualObject
 * to retrieve parameters for visual creation
 *
 * @author Kimmo Hoikka
 */
class MGlxVisualObjectParameterFactory
	{
	protected:

		/**
		 * Destructor, dont allow deletion through this interface
		 */
		virtual ~MGlxVisualObjectParameterFactory() { };
		
	public:
		
		/**
		 * @return the HUI layout for the visual object
		 */
		virtual CAlfLayout* Layout() = 0;

		/**
		 * @return the HUI control that owns the HUI object
		 */
		virtual CAlfControl& VisualOwner() = 0;
		
		/**
		 * Ownership of the brush stays in the factory
		 * 
		 * @return the Border brush for the Visual.
		 */
		virtual CAlfBrush* BorderBrush() = 0;
		
		/**
		 * @return the scale mode to use for the thumbnails
		 */
		virtual CAlfImageVisual::TScaleMode ThumbnailScaleMode() = 0;
	}; 

/**
 *  CGlxVisualObject
 *
 *  A visual representing an image/video/etc thumbnail
 *
 * @author Kimmo Hoikka
 */
class CGlxVisualObject : public CBase, public MGlxTextureObserver
    {
	public:
        /**
         * Thumbnail/icon status of a visual.
         */
        enum TVisualStatus
            {
            EHasDefaultIcon, EHasErrorIcon, EHasOtherIcon, EHasThumbnail
            };

	public:
		/**
		 * Constructor.
		 * @param aParent the parent HUI control
		 * @param aLayout the layout chain for the visual
		 */
		static CGlxVisualObject* NewLC( 
			MGlxVisualObjectParameterFactory& aFactory,
			MGlxMediaList& aMediaList );

		/**
		 * Destructor
		 */
		~CGlxVisualObject();
		

        /**
         * Set the scale mode for the visual.
         * @param aMode The scale mode to be used.
         */
        IMPORT_C void SetScaleMode(CAlfImageVisual::TScaleMode aMode);

		/**
		 * @return the visual of this object
		 */
		CAlfVisual* Visual();
        
        /**
         * Set the visual either visible or invisible
         * @param aVisible If true, set visible, otherwise invisible
         */
        void SetVisible( TBool aVisible );
        
		/**
		 * Sets the index
		 * @param aIndex, the index of this visual
		 */
		void SetIndex( TInt aIndex );

		/**
		 * Specifies the image for the visual
		 * @param aImage the image
		 * @param aImageId the image id
		 */
		void SetImage( TAlfImage aImage );
		 
		/**
		 * @return the THuiImage that corresponds to the image
		 */
		TAlfImage Image();

		/**
		 * Notify that the display refresh has just started
		 * @param aDisplay the HUI display
		 */
		void RefreshLayout( TSize aScreenSize );

        /**
         * Handle new attributes recieved for the item
         * @param aIdSpaceId ID space ID of the media item
         * @param aItem Media item to check attributes
         * @param aAttributes Array of new attributes
         */
        void HandleAttributesAvailableL( const TGlxIdSpaceId& aIdSpaceId,
                                    const TGlxMedia& aItem,
                                    const RArray<TMPXAttribute>& aAttributes);

        /**
         * Start an animation
         * @param aAnimatedTexture The animated texture to use
         */
        void StartAnimation( CAlfTexture& aAnimatedTexture );

        /**
         * Stop current animation
         */
        void StopAnimation();
        
        /**
         * Set a flag indicating whether an item should be animated
         * when all necessary attributes have been fetched
         * @param aFlag Value of the flag
         */
	    void SetAnimateWhenAttributesAvailable(TBool aFlag);

        /**
         * Try to start an item animation, if available
         * @param aItem Media item to animate
         */
        void TryAnimateL(const TGlxMedia& aItem);
        
        /**
        * Add icon to visual
        * @param aTexture texture of icon to apply
        * @param aIconPos position of icon (centred or relative to a specific corner)
        * @param aForeground foreground or background icon
        * @param aStretch ETrue if texture scales to visual
        * @param aBorderMargin margin from specified corner
        * @param aWidth width as proportion of visual width
        * @param aHeight height as proportion of height width
        */
        void AddIconL( const CAlfTexture& aTexture, NGlxIconMgrDefs::TGlxIconPosition aIconPos,
                TBool aForeground, TBool aStretch, TInt aBorderMargin,
                                        TReal32 aWidth, TReal32 aHeight );
                
        /**
        * Remove icon from visual
        * @param aTexture texture of icon to remove
        */
        TBool RemoveIcon(  const CAlfTexture& aTexture );
        
        /**
        * Hide or show icon if required
        * @param aTexture texture of icon
        * @param aVisible Set hidden or show
        */
        void SetIconVisibility( const CAlfTexture& aTexture, TBool aVisible );

        /**
        * Get thumbnail/icon status.
        * @return Current status.
        */
        TVisualStatus Status() const;
        /**
        * Set thumbnail/icon status.
        * @param aStatus New status.
        */
        void SetStatus( TVisualStatus aStatus );

        /**
        * Set observer to be notified when the thumbnail texture gains or
        * loses content.
        * @param aObserver Observer for the thumbnail status.
        * @return Whether the thumbnail texture currently has content.
        */
        TBool SetObserver( MGlxVisualObjectStatusObserver& aObserver );
        
        
		void Reset();
		void ResetLayout( CAlfVisual& aVisual );
		
    private: // From MGlxTextureObserver
        void TextureContentChangedL( TBool aHasContent, CAlfTexture* aNewTexture );

	private:
		// default constructor
		CGlxVisualObject( MGlxMediaList& aMediaList);

		// 2nd phase constructor
		void ConstructL( MGlxVisualObjectParameterFactory& aFactory);

        /**
         * Remove zoom tiles for attributes no longer available
         * @param aItem Media item to check attributes
         */
        void RemoveOldZoomTiles(const TGlxMedia& aItem);
        /**
         * Add zoom tiles for new cropped thumbnail attributes
         * @param aIdSpaceId ID space ID of the media item
         * @param aItem Media item to check attributes
         * @param aAttributes Array of new attributes
         */
        void AddNewZoomTilesL(TGlxIdSpaceId aIdSpaceId, const TGlxMedia& aItem,
                            const RArray<TMPXAttribute>& aAttributes);

        /**
         * Remove a zoom tile visual from the array and delete it.
         * @param aIndex Which tile to remove.
         */
        void RemoveZoomTile(TInt aIndex);

        /**
         * Create a zoom tile visual as a child of the anchor layout.
         * @param aItem Media item to check attributes
         * @param aAttribute Cropped thumbnail attribute
         * @param aIdSpaceId ID space ID of the media item
         * @param aCroppingRect Cropping rect for the tile.
         */
        TInt AddZoomTileL(
                    const TGlxMedia& aItem, const TMPXAttribute& aAttribute,
                    TGlxIdSpaceId aIdSpaceId, const TRect& aCroppingRect );

        /**
         * Reapply the anchors to tiles which have changed position (anchor
         * layout doesn't reorder the anchors when the child order changes).
         * @param aFirstPosition Index of first tile which has changed.
         */
        void ReanchorZoomTiles( TInt aFirstPosition );

        /**
          * Structure to hold icon anchor details
          */
        class TGlxAnchorDetails
            {
            public:
                TAlfAnchorOrigin iAnchorHorizOrigin;
                TAlfAnchorOrigin iAnchorVertiOrigin;
                TAlfAnchorMetric iAnchorMetric;
                TAlfTimedPoint   iPosition;
            };

        /**
          * Structure to store both anchors for an icon
          */
        class TGlxIconAnchors
            {
            public:
                /**
                 * Helper method to compare two icons
                 */
                static TBool Match( const TGlxIconAnchors& aIcon1,
                                    const TGlxIconAnchors& aIcon2 );

                /// The texture for this icon (not owned)
                const CAlfTexture* iTexture;
                TGlxAnchorDetails iTopLeftAnchor;
                TGlxAnchorDetails iBotRightAnchor;
            };
            
                                  
        /**
          * Calculate anchors based on icon position and scaling
          * @param aAnchors Structure to hold anchor information
          * @param aTexture Texture used to create icon (need texture size)
          * @param aIconPos position of icon (relative to specified corner 
          *                 or central)
          * @param aBorderMargin distance from corner of icon
          * @param aStretch is texture icon to scaled with layout or fixed size
          */
        void GenerateAnchors( TGlxIconAnchors& aAnchors,
                                const CAlfTexture& aTexture, 
                                NGlxIconMgrDefs::TGlxIconPosition aIconPos, 
                                    TInt aBorderMargin, TBool aStretch, 
                                    TReal32 aWidth, TReal32 aHeight );
                                    
        /**
          * Set the anchors
          * @param aAnchors Structure to hold anchor information
          * @param aLayout Layout (foreground or background)
          * @param aPos position of icon in layout
          * @param aUseOffsets whether to apply horizontal and vertical offsets
          *                                                          to anchors
          */                            
        void SetAnchors( TGlxIconAnchors& aAnchors, CAlfAnchorLayout& aLayout,
                                            TInt aPos, TBool aUseOffsets = EFalse );                           
                                    
                                    
        /**
          * Find position in layout of icon with specified texture
          * @param aTexture Texture to search for
          * @param aForeground ETrue if icon is in foreground, EFalse if background
          * @return positon of icon in layout, KErrNotFound if icon not present
          */
        TInt FindExistingTexture( const CAlfTexture& aTexture, TBool aForeground );

        /**
          * Remove icon and reset anchors for following icons.
          * @param aLayout Layout (foreground or background)
          * @param aAnchorsArray Stored anchors for specified layout
          * @param aPos position in layout of icon that was removed
          */
        void DoRemoveIcon( CAlfAnchorLayout& aLayout, 
                            RArray<TGlxIconAnchors>& aAnchorsArray, 
                            TInt aPos );
                    
        /**
        * If visual has resized calcuate offsets for overlay icons (so we can
        * ensure icons are positioned over thumbnail if thumbnail dos not fill visual)
        * @param aTexture texture for image
        * @param aVisSize size of visual
        */            
        void UpdateIconAnchors( const CAlfTexture& aTexture, TSize aVisSize );                    
          
              

	private:	// Data
	    /**
	     * Structure to hold information about a zoom tile visual.
	     */
        class TZoomTile
            {
            public:
                /** Visual to display the tile (not owned) */
                CAlfImageVisual* iVisual;
                /** Attribute ID of the cropped thumbnail */
                TUint iAttributeId;
                /** Zoom level of the cropped thumbnail */
                TInt iZoomLevel;
                /** Anchor point for top left of tile */
                TAlfRealPoint iTopLeft;
                /** Anchor point for bottom right of tile */
                TAlfRealPoint iBottomRight;
            };
            
		/// Own: the index
		TInt iIndex;
		MGlxMediaList& iMediaList;
		
		/// Top-level visual 
		CAlfAnchorLayout* iVisual;
		
		/// The visual that handles the drawing of image(not owned)
		CAlfAnchorLayout* iMainVisual;
		
		/// The visual that handles the drawing of background underlay icons
	    /// (not owned)
		CAlfImageVisual* iBackgroundOverlayVisual;
		
		/// Child visual for normal image items (not owned)
		CAlfImageVisual* iImageVisual;
		
		/// Own: the HUI image
		TAlfImage iImage;
		TBool iImageValid;
		/// Own: the HUI animated texture
		CAlfTexture* iAnimatedTexture;
		/// Whether this item should be animated when the correct attributes
		/// are available
		TBool iAnimateWhenAttributesAvailable;
        /// Own: The HUI utility	
		CGlxUiUtility* iUiUtility;
		/// Own: Array of tiles for zoomed image
        RArray<TZoomTile> iZoomTiles;
        /// Original image dimensions
        TSize iDimensions;
        
        /// Anchor details for foreground overlay icons
        RArray<TGlxIconAnchors> iForegroundIconAnchors;
        
        // Anchor details for background 'underlay' icons
        RArray<TGlxIconAnchors> iBackgroundIconAnchors;
        
        // vertical offset for overlay icons
        TReal32 iVertIconOffset;
        
        // horizontal offset for overlay icons
        TReal32 iHoriIconOffset;
        
        // Store size of visual
        TSize iVisSize;

        /// Current thumbnail/icon status
        TVisualStatus iStatus;
        
        /// Observer of this visual object.
        MGlxVisualObjectStatusObserver* iObserver;
        
        // The id of the zoom texture from the texture manager
        TInt iZoomTextureId;
    };

#endif // C_GLXITEMVISUAL_H

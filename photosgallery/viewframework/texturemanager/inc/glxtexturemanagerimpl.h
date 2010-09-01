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
* Description:    Texture Manager component
*
*/




#ifndef C_GLXTEXTUREMANAGERIMPL_H
#define C_GLXTEXTUREMANAGERIMPL_H

#include <e32base.h>
#include <badesca.h>    // For CDesCArrayFlat
#include <e32cmn.h>

#include <AknsItemID.h>
#include <alf/alftexture.h>
#include <alf/alfbitmapprovider.h>
#include <alf/alftimedvalue.h>
#include <mpxattribute.h>
#include <mglxresolutionchangeobserver.h>
#include "glxbitmapdecoderwrapper.h"
#include <glxmediaid.h>
#include <glximageviewermanager.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>
#include <mglxcacheobserver.h>
class CAlfTexture;
class CGlxDRMUtility;
class CGlxMedia;
//class CAlfGifAnimationTexture;
class CAlfImageLoaderUtil;
class CAlfTextureManager;
class MGlxCache;
class TGlxMedia;
class CGlxResolutionUtility;
class MGlxTextureObserver;
class CGlxBitmapDecoderWrapper;
class MGlxBitmapDecoderObserver;
class CGlxThumbnailAttribute;

const TInt KGlxMaxFrameCount = 25;

/**
 *  CGlxTextureManagerImpl
 *
 *  Implementation of Texture Manager component. 
 *
 * @author David Holland
 * @internal reviewed 03/07/2007 by Dan Rhodes
 *
 */
class CGlxTextureManagerImpl : public CBase, public MAlfBitmapProvider,
                                         public MGlxResolutionChangeObserver,
										 public MGlxBitmapDecoderObserver,
										 public MGlxCacheObserver
    {
public:
    /**
    * Two-phased constructor
    * @param aAlfTextureManager The Alf Texture Manager
    * @return Constructed object
    */
    static CGlxTextureManagerImpl* NewL(
                                CAlfTextureManager& aAlfTextureManager);

    /**
    * Two-phased constructor
    * @param aAlfTextureManager The Alf Texture Manager
    * @return Constructed object
    */
    static CGlxTextureManagerImpl* NewLC(
                                CAlfTextureManager& aAlfTextureManager);

    /**
    * Destructor
    */      
    ~CGlxTextureManagerImpl();

    /**
     * Determins if the texture need updating
     * @param aMediaId The Id of the media item
     * @param aIdSpaceId The Id of the Id space in which the media Id is defined
     * @param aRequiredSize The preferred size of the texture
     * @return ETrue if the texture needs updating. EFalse if not.
     */
    TBool TextureNeedsUpdating(TGlxMedia aMedia, TGlxIdSpaceId aIdSpaceId,
                                                const TSize& aRequiredSize);
    /**
    * Create a texture from an icon within the application
    * @param aSkinID The skin Id associated with the icon 
    * @param aIconResourceId The Id associated with the icon .mif file
    * @param aFilename The full path name of the .mif file
    * @param aSetColor ETrue if colour of icon is to be modified
    * @param aDefaultColor colour use if skin clour not specified or not found
    * @param aSkinColorId Id of the skin colour table
    * @param aSkinColorIndex Index of the colour in the table
    * @param aRequestedSize requested size. If default used size is based on size of original graphic
    * @return Created texture
    */
    CAlfTexture& CreateIconTextureL(const TAknsItemID& aSkinID,
                                    TInt aIconResourceId, 
                                    const TDesC& aFilename,
                                    TBool aSetColor,
                                    TRgb aDefaultColor,
                                    const TAknsItemID& aSkinColorId,
                                    TInt aSkinColorIndex,
                                    TSize aRequestedSize);

    /**
     * Requests the best match texture. If it already exists this method does nothing.
     * However if it doed not exist it will create it and replace the old texture
     * if necessary.
     * @param aSize The size of the requested texture
     * @param aMedia The TGlxMedia item
     * @param aIdSpaceId The Id of the Id space in which the media Id is defined
     * @param aObserver Observer for the texture.
	 * @param aScaleGridTnm ETrue if the grid tnm to be scaled else EFalse.
     * @return Created texture, or NULL if the texture alreay exists
     */
    CAlfTexture& CreateNewTextureForMediaL(TSize aSize, const TGlxMedia& aMedia,
        const TGlxIdSpaceId& aIdSpaceId, MGlxTextureObserver* aObserver, TBool aScaleGridTnm );
            
                
    /**
    * Create a texture from a zoomed thumbnail attribute of a media item
    * @param aMedia The media item
    * @param aAttribute Which attribute
    * @param aIdSpaceId The Id of the Id space in which the media Id is defined
    * @param aObserver Observer for the texture.
    * @return Created texture
    */
    CAlfTexture& CreateZoomedTextureL(const TGlxMedia& aMedia,
                                            const TMPXAttribute& aAttribute,
                                            TGlxIdSpaceId aIdSpaceId, MGlxTextureObserver* aObserver);

    /**
    * If we have a texture ID, we recreate the Zoomed texture based on that
    * 
    * @param aTextureID the texture ID to create the texture on. 
    */
    CAlfTexture* CreateZoomedTextureL( );

    /**
    * Create an animated texture from a GIF file
    * @param aFilename The filename of the GIF file
    * @param aSize The size of the GIF file
	* @param aMedia The media item
	* @param aIdSpaceId The Id of the Id space in which the media Id is defined	
    * @return Created texture: ownership transfered
    */
    CAlfTexture& CreateAnimatedGifTextureL(const TDesC& aFilename, const TSize& aSize,
                                           const TGlxMedia& aMedia, TGlxIdSpaceId aIdSpaceId);
 	
	/**
    * Creates textures for the given GIF media
	* @param aMedia The media item
	* @param aIdSpaceId The Id of the Id space in which the media Id is defined
	* @param aFrameNumber frame number of gif media
    * @param aBitmap Bitmap for the particular frame
	* @return Created texture: ownership transfered
    */    
    CAlfTexture& CreateDRMAnimatedGifTextureL(const TGlxMedia& aMedia,
            TGlxIdSpaceId aIdSpaceId, TInt aFrameNumber, CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask);
    /**
     * Removes the texture if it was created by CreateThumbnailTextureL or
     * CreateZoomedTextureL
     * @param aTexture The texture to be removed
     */
    void RemoveTexture( const CAlfTexture& aTexture );
    
    /**
     * Removes all Texteures 
     * 
     */
    void FlushTextures() ;
    
    /**
     * Removes the texture if it was created by CreateThumbnailTextureL or
     * CreateZoomedTextureL
     * @param aTexture The texture to be removed
     */
    void RemoveZoomList();
    
    /**
     * Removes the texture if it was created by CreateThumbnailTextureL or
     * CreateZoomedTextureL
     * @param TGlxMediaId The iMediaId to be removed
     * @param aAllTexture Flag to remove all textures
	 * Bug fix for PKAA-7NRBYZ - added bAllTexture param.
     */
    void RemoveTexture( const TGlxMediaId& aMediaId, TBool aAllTexture);
    
    
    /**
     * Creates a flat color texture and returns the reference to it
     */
    CAlfTexture& CreateFlatColourTextureL( TRgb aColour );   

   /**
    * Animate the given media.
    * @param aMediaId The Id of the media to be animated
    * @param aState Animation state [ETrue to Start, EFalse to Stop]
    */
    void AnimateMediaItem(const TGlxMediaId& aMediaId, TBool aState);
        
public:  // From MGlxCacheObserver
    void HandleAttributesAvailableL(const TGlxIdSpaceId& aIdSpaceId, 
                                    const TGlxMediaId& aMediaId, 
                                    const RArray<TMPXAttribute>& aAttributes, 
                                    const CGlxMedia* aMedia);

    void CleanupMedia(const TGlxMediaId& aMediaId);
    
public: // from MAlfBitmapProvider
    void ProvideBitmapL(TInt aId, 
                        CFbsBitmap*& aBitmap, 
                        CFbsBitmap*& aMaskBitmap);
   
    
public: // From MGlxResolutionChangeObserver    
    void HandleResolutionChangedL();
    
    /**
    * Updates the texture for every Icon in the IconList.
    */
    void UpdateTexture();
public: // From MGlxTextureRotatorObserver    
    void HandleBitmapDecodedL(TInt aThumbnailIndex,CFbsBitmap* aBitmap);

    
private:
    /**
    * Constructor
    * @param aAlfTextureManager The Alf Texture Manager
    */
    CGlxTextureManagerImpl(CAlfTextureManager& aAlfTextureManager);
    
    /**
    * Second-phase constuction
    */
    void ConstructL();
    
    
    /**
    * GetThumbnailAttributeAndIndexL
    * @param aSize The size of the requested texture
    * @param aMedia The TGlxMedia item.
    * @param aThumbnailIndex on return will contain the Index of the thumbnail in the iThumbnailList
    *        or KErrNotFound
    * @param aThumbnailAttribute on return will contain the Thumbnail attribute (if found)
    * @return ETrue if Thumbnail is available, EFalse if it needs to be created
    */
    TBool GetThumbnailAttributeAndIndexL( TSize aSize, const TGlxMedia& aMedia,
        const TGlxIdSpaceId& aIdSpaceId, TInt& aThumbnailIndex,
        TMPXAttribute& aThumbnailAttribute );

    /**
     * Create Image Viewer manager Instance
     */
    void CreateImageViewerInstanceL();
    
    /**
     * Delete Image Viewer manager Instance
     */
    void DeleteImageViewerInstance();
    
    /**
     * TGlxIcon
     * Values associated with a bitmap/svg icon
     */
    class TGlxIcon 
    {
    public:
        TInt           iTextureId;
        CAlfTexture*   iTexture;
        TAknsItemID    iAvkonSkinId;
        TInt           iBitmapId;
        TInt           iFilenameIndex;
        TBool          iSetColor;    // indicates if color to be modified
        TRgb           iDefaultColor;
        TAknsItemID    iSkinColorId;
        TInt           iSkinColorIndex;
        TReal32        iHeightInPoss;
        TReal32        iWidthInPoss;
        
        /**
         * Helper functor to be able to find texture from array
         */
        static TBool MatchTexture( 
                const CAlfTexture* aTexture, 
                const TGlxIcon& aRhs )
        	{
        	// return true if the address of the texture match
        	return aTexture == aRhs.iTexture;
        	}
        };

    /**
     * TGlxThumbIcon
     * Values associated with a thumbnail.
     */
    class TGlxThumbnailIcon 
        {
    public:
        TInt              iTextureId;
        CAlfTexture*      iTexture;
        TMPXAttribute     iAttribId;
        TSize             iRequiredSize;
        TGlxMediaId       iMediaId;
        TGlxIdSpaceId     iIdSpaceId;
        MGlxTextureObserver* iObserver;
        CFbsBitmap*         iBitmap;
		TTime				iImageDate; 
        /**
         * Helper function to be able to find texture from array
         */
        static TBool MatchTexture( 
            const CAlfTexture* aTexture, 
            const TGlxThumbnailIcon& aThumbData )
        	{
        	// return true if the address of the texture match
        	return aTexture == aThumbData.iTexture;
        	}

        /**
         * Helper function to be able to find mediaid from array
         */
        static TBool MatchMediaId(const TGlxMediaId* aMediaId,
                const TGlxThumbnailIcon& aThumbData)
            {
        	// return true if the Media Id match
            return *aMediaId == aThumbData.iMediaId;
            }
         };
    
    /**
     * TGlxDRMGifThumbnailIcon
     * Values associated with a DRM Gif thumbnail.
     */
    class TGlxDRMGifThumbnailIcon
        {
    public:
        TInt iTextureId[KGlxMaxFrameCount];
        CAlfTexture* iTexture[KGlxMaxFrameCount];
        TMPXAttribute iAttribId;
        TSize iRequiredSize;
        TGlxMediaId iMediaId;
        TGlxIdSpaceId iIdSpaceId;
        MGlxTextureObserver* iObserver;
        CFbsBitmap* iBitmap[KGlxMaxFrameCount];
        CFbsBitmap* iBitmapMask[KGlxMaxFrameCount];
        TTime iImageDate;
        /**
         * Helper function to be able to find texture from array
         */
        static TBool MatchTexture(const CAlfTexture* aTexture,
                const TGlxDRMGifThumbnailIcon& aThumbData)
            {
            // return true if the address of the texture match
            return aTexture == aThumbData.iTexture[0];
            }

        /**
         * Helper function to be able to find mediaid from array
         */
        static TBool MatchMediaId(const TGlxMediaId* aMediaId,
                const TGlxDRMGifThumbnailIcon& aThumbData)
            {
            // return true if the Media Id match
            return *aMediaId == aThumbData.iMediaId;
            }
        };
    /**
    * Requests the best match texture. If it already exists this method does nothing.
    * However if it doed not exist it will create it and replace the old texture
    * if necessary.
    * @param aSize The size of the requested texture
    * @param aMedia The TGlxMedia item
    * @param aIdSpaceId The Id of the Id space in which the media Id is defined
    * @param aObserver Observer for the texture.
    * @param aAttribute specifies the type of texture to create
    * @param aThumbData the tumbnail data
    * @param aScaleGridTnm ETrue if the grid tnm to be scaled else EFalse.	
    * @return Created texture, or NULL if the texture alreay exists
    */
    CAlfTexture& CreateNewTextureL( TSize aSize, const TGlxMedia& aMedia,
        const TGlxIdSpaceId& aIdSpaceId, MGlxTextureObserver* aObserver,
        TMPXAttribute& aAttribute, TGlxThumbnailIcon& aThumbData,TBool aIsThumbnailTexture, TBool aScaleGridTnm);
        

    /**
     * TGlxProvideBitmapHolder
     * Used to hold the bitmaps and their associated texture id between
     * the call to create the alf texture and the callback to ProvideBitmap.
     */
    class TGlxProvideBitmapHolder
        {
    public:
        TInt iTextureId;
        CFbsBitmap* iBitmap;        // not owned
        CFbsBitmap* iMaskBitmap;    // not owned
        };
    
    /**
    * Select the most appropriate attribute to use to load bitmap
    * @param aThumbnail contains the data concerning the texture
    * @param aMedia the media 
    * @return Index of selected attribute in list or KErrNotFound
    */
    TMPXAttribute SelectAttributeL(TSize& aThumbnail,
                          const TGlxMedia& aMedia);


    /**
    * Duplicates a bitmap
    * @param aOriginalBitmap original bitmap
    * @return duplicate bitmap
    */
    CFbsBitmap* CreateDuplicateBitmapL( CFbsBitmap& aOriginalBitmap );
    
    /**
    * Increments the texture id and returns it
    * @return next texture id
    */
    TInt NextTextureId();

    /**
    * Scales the grid Thumbnail bitmap to the fullscreen keeping the aspect 
    * ratio.
    * @param aSrcSize The size of the original image
    * @param aDestSize The Fullscreen size
    * @param aScaledBitmap Pointer to the scaled bitmap
    * @param thumb thumbnail attributes.
    */    
    void ScaleGridTnmToFsL(TSize aSrcSize, TSize aDestSize, 
            CFbsBitmap *aScaledBitmap, const CGlxThumbnailAttribute* thumb);


    /**
    * GetAnimatedGifThumbnailIndex
    * @param aSize The size of the requested texture
    * @param aMedia The TGlxMedia item.
    * @param aIdSpaceId The Id of the Id space in which the media Id is defined	
    * @param aThumbnailIndex on return will contain the Index of the thumbnail in the iAnimatedTnmList
    *        or KErrNotFound
    * @return ETrue if Thumbnail is available, EFalse if it needs to be created
    */
    TBool GetAnimatedGifThumbnailIndex( TSize aSize,
            const TGlxMedia& aMedia, const TGlxIdSpaceId& aIdSpaceId,
            TInt& aThumbnailIndex);
     /**
    * GetDRMAnimatedGifThumbnailIndex
    * @param aMedia The TGlxMedia item.
    * @param aIdSpaceId The Id of the Id space in which the media Id is defined	
    * @param aThumbnailIndex on return will contain the Index of the thumbnail in the iAnimatedTnmList
    *        or KErrNotFound
    * @return ETrue if Thumbnail is available, EFalse if it needs to be created
    */   
    TBool GetDRMAnimatedGifThumbnailIndex(const TGlxMedia& aMedia, const TGlxIdSpaceId& aIdSpaceId,
            TInt& aThumbnailIndex);
private:
    // Alf Texture manager (not owned) 
    CAlfTextureManager& iAlfTextureManager;

    // Next id to be allocated to icon textures provide within the application. 
    TInt iNextTextureId;

    // List containing data for icon textures provided within the application. 
    RArray<TGlxIcon> iIconList;
        
    // List containing data for textures generated from thumbnail bitmaps. 
    RArray<TGlxThumbnailIcon> iThumbnailList;
    
    // List containing data for textures generated from zoomed thumbnail. 
    RArray<TGlxThumbnailIcon> iZoomedList;

    // List containing data for textures generated from Animated thumbnail. 
    RArray<TGlxThumbnailIcon> iAnimatedTnmList;
    
    // List containing data for textures generated from DRM Gif Animated thumbnail. 
    RArray<TGlxDRMGifThumbnailIcon> iDRMGifAnimatedTnmList;
        
    // List of .mif filenames of files containing icons. 
    CDesCArrayFlat* iMifFilenames;
    
    // DRM utility class 
    CGlxDRMUtility* iDrmUtility;
    
    // MGlxCache instance 
    MGlxCache* iCache;
    
    // CGlxResolutionUtility instance 
    CGlxResolutionUtility* iResUtil;
    
	//The Bitmap Decoder Engine
	CGlxBitmapDecoderWrapper* iZoomDecoder; 
    // Temporary storage class used between call to create a texture and the 
    //callback from alf to ProvideBitmap 
    TGlxProvideBitmapHolder iProvideBitmapHolder;
    // For image viewer, not own
    CGlxImageViewerManager* iImageViewerInstance;
    };

#endif // C_GLXTEXTUREMANAGERIMPL_H

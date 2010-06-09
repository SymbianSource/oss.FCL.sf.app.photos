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




#ifndef C_GLXTEXTUREMANAGER_H
#define C_GLXTEXTUREMANAGER_H

#include <e32base.h>
#include <gdi.h>
#include <AknsConstants.h>
#include <glxmediaid.h>
#include "mglxskinchangeobserver.h"

class TAknsItemID;
class CAlfTexture;
class TMPXAttribute;
class CGlxDRMUtility;
class CGlxMedia;
class CAlfImageLoaderUtil;
class CAlfTextureManager;
class MGlxCache;
class TGlxMedia;
class CGlxResolutionUtility;
class CGlxTextureManagerImpl;
class MGlxTextureObserver;

/**
 *  CGlxTextureManager
 *
 *  Texture Manager component. 
 *
 * @author David Holland
 *
 */
NONSHARABLE_CLASS ( CGlxTextureManager ) : public CBase, 
                                           public MGlxSkinChangeObserver
    {
public:
    /**
    * Two-phased constructor
    * @param aAlfTextureManager The Alf Texture Manager
    * @return Constructed object
    */
    IMPORT_C static CGlxTextureManager* NewL(
                                CAlfTextureManager& aAlfTextureManager);


    /**
    * Destructor
    */      
    ~CGlxTextureManager();
    /**
    * Determins if the texture need updating
    * @param aMediaId The Id of the media item
    * @param aIdSpaceId The Id of the Id space in which the media Id is defined
    * @param aRequiredSize The preferred size of the texture
    * @return ETrue if the texture needs updating. EFalse if not.
    */
    IMPORT_C TBool TextureNeedsUpdating(TGlxMedia aMedia, TGlxIdSpaceId aIdSpaceId,
                                                const TSize& aRequiredSize);

 /**
     * Requests the best match texture. If it already exists this method does nothing.
     * However if it doed not exist it will create it and replace the old texture
     * if necessary.
     * @param aSize The size of the requested texture
     * @param aMedia The TGlxMedia item
     * @param aIdSpaceId The Id of the Id space in which the media Id is defined
     * @param aObserver Observer for the texture.
     * @return Created texture, or NULL if the texture alreay exists
     */
    
    IMPORT_C CAlfTexture& CreateNewTextureForMediaL(
            TSize aSize, 
            const TGlxMedia& aMedia, 
            const TGlxIdSpaceId& aIdSpaceId, 
            MGlxTextureObserver* aObserver );
    /**
    * Create a texture from an Avkon icon
    * @param aId The Id associated with the icon in the Skin
    * @param aBitmapId The Id associated with the bitmap in the Avkon icon bitmap file
    *  The bitmap Id is used if no icon is found for the skin Id
    * @param aRequestedSize requested size. If default used size is based on size of original graphic
    * @return Created texture
    */
    IMPORT_C CAlfTexture& CreateAvkonIconTextureL(const TAknsItemID &aID, 
                                          TInt aBitmapId,
                                          TSize aRequestedSize = TSize(0,0));
                                          
    /**
    * Create a texture from an Avkon icon
    * @param aId The Id associated with the icon in the Skin
    * @param aBitmapId The Id associated with the bitmap in the Avkon icon bitmap file
    *  The bitmap Id is used if no icon is found for the skin Id
    * @param aDefaultColor colour use if skin clour not specified or not found
    * @param aSkinColorId Id of the skin colour table
    * @param aSkinColorIndex Index of the colour in the table
    * @param aRequestedSize requested size. If default used size is based on size of original graphic
    * @return Created texture
    */
    IMPORT_C CAlfTexture& CreateColorAvkonIconTextureL(
                                                        const TAknsItemID& aID, 
                                                        TInt aBitmapId,
                                                        TRgb aDefaultColor,
                                                        const TAknsItemID& aSkinColorId = KAknsIIDNone,
                                                        TInt aSkinColorIndex = 0,
                                                        TSize aRequestedSize = TSize(0,0));
                                          
    
    /**
    * Create a texture from an Avkon icon
    * @param aId The Id associated with the icon in the Skin
    * @param aRequestedSize requested size. If default used size is based on size of original graphic
    * @return Created texture
    */
    IMPORT_C CAlfTexture& CreateAvkonIconTextureL(const TAknsItemID &aID, 
                                          TSize aRequestedSize = TSize(0,0));
    
    /**
    * Create a texture from an icon within the application
    * @param aIconResourceId The Id associated with the icon .mif file
    * @param aFilename The full path name of the .mif file
    * @param aRequestedSize requested size. If default used size is based on size of original graphic
    * @return Created texture
    */
    IMPORT_C CAlfTexture& CreateIconTextureL(TInt aIconResourceId, 
                                           TDesC& aFilename,
                                           TSize aRequestedSize = TSize(0,0));

    /**
    * Create a texture from an icon within the application, modifying the icon colour
    * @param aIconResourceId The Id associated with the icon .mif file
    * @param aFilename The full path name of the .mif file
    * @param aDefaultColor colour use if skin clour not specified or not found
    * @param aSkinColorId Id of the skin colour table
    * @param aSkinColorIndex Index of the colour in the table
    * @param aRequestedSize requested size. If default used size is based on size of original graphic
    * @return Created texture
    */
    IMPORT_C CAlfTexture& CreateColorIconTextureL(
                               TInt aIconResourceId, 
                               TDesC& aFilename,
                               TRgb  aDefaultColor,
                               const TAknsItemID& aSkinColorId = KAknsIIDNone,
                               TInt aSkinColorIndex = 0,
                               TSize aRequestedSize = TSize(0,0));
    
    /**
    * Create a texture from a bitmap from the attribute of a media item
    * @param aMediaId The Id of the media item
    * @param aIdSpaceId The Id of the Id space in which the media Id is defined
    * @param aRequiredSize The preferred size of the texture
    * @param aObserver Observer for the texture.
    * @param aScaleGridTnm ETrue if the grid tnm to be scaled else EFalse by default.
    * @return Created texture
    */
    IMPORT_C CAlfTexture& CreateThumbnailTextureL(
                const TGlxMedia& aMedia, const TGlxIdSpaceId& aIdSpaceId,
                const TSize& aRequiredSize, MGlxTextureObserver* aObserver, 
                TBool aScaleGridTnm=EFalse);

    /**
    * Create a texture from a zoomed thumbnail attribute of a media item
    * @param aMedia The media item
    * @param aAttribute Which attribute
    * @param aIdSpaceId The Id of the Id space in which the media Id is defined
    * @param aObserver Observer for the texture.
    * @return Created texture
    */
    IMPORT_C CAlfTexture& CreateZoomedTextureL(const TGlxMedia& aMedia,
                                            const TMPXAttribute& aAttribute,
                                            TGlxIdSpaceId aIdSpaceId, MGlxTextureObserver* aObserver);

    /**
    * Create a texture from a zoomed thumbnail for a given texture ID
    * @param aTextureId The texture id, for which the texture is removed.
    * @return Created texture
    */
    IMPORT_C CAlfTexture* CreateZoomedTextureL() ;
    
    /**
    * Create an animated texture from a GIF file
    * @param aFilename The filename of the GIF file
    * @param aSize The size of the GIF file
	* @param aMedia The media item
	* @param aIdSpaceId The Id of the Id space in which the media Id is defined
    * @return Created texture: ownership transfered
    */
    IMPORT_C CAlfTexture& CreateAnimatedGifTextureL( const TDesC& aFilename, 
       const TSize& aSize, const TGlxMedia& aMedia, TGlxIdSpaceId aIdSpaceId );
    
    /**
     * Creates a flat color texture and returns the reference to it
     * Note that the texture ownership stays within texture manager 
     * @param aColour the color for the texture
     */
    IMPORT_C CAlfTexture& CreateFlatColourTextureL( TRgb aColour );
    
    /**
    * Removes the texture if it was created by CreateThumbnailTextureL or
    * CreateZoomedTextureL
    * @param aTexture The texture to be removed
    */
    IMPORT_C void RemoveTexture(const CAlfTexture& aTexture);
    /**
     * Removes the texture if it was created by CreateThumbnailTextureL or
     * CreateZoomedTextureL
     * @param aTexture The texture to be removed
     */
    IMPORT_C void RemoveZoomList();
    
 /**
    * Removes the texture if it was created by CreateThumbnailTextureL or
    * CreateZoomedTextureL
    * @param aMediaId The MediaId to be removed
    * @param aAllTexture Flag to remove all textures
	* Bug fix for PKAA-7NRBYZ - added bAllTexture param
    */
    IMPORT_C void RemoveTexture(const TGlxMediaId& aMediaId, TBool aAllTexture=EFalse);
    
    /**
       * Removes ALL Textures 
       */
    IMPORT_C void FlushTextures() ;
    
   /**
    * Animate the given media.
    * @param aMediaId The Id of the media to be animated
    * @param aState Animation state [ETrue to Start, EFalse to Stop]
    */
    IMPORT_C void AnimateMediaItem(const TGlxMediaId& aMediaId, TBool aState);
    
public: // from MGlxSkinChangeObserver

    void HandleSkinChanged();    


private:
       //implementation class
       CGlxTextureManagerImpl* iImpl;
    };
 


#endif // C_GLXTEXTUREMANAGER_H

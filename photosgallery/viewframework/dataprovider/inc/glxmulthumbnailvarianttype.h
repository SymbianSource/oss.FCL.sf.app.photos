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
* Description:    Makes the Decision to Create Thumbnail Texture or Icon Texture
*                 Considering Drm Validity and Thumbnail Availabilty
*
*/




#ifndef _GLXTHUMBNAILVARIANTTYPE_H_
#define _GLXTHUMBNAILVARIANTTYPE_H_

#include <e32base.h>                  // Container Base Class
#include <mul/mulvarianttype.h>       // Observer interface used to be notified when texture content changes
#include "mglxtextureobserver.h"      // Observer interface used to be notified when texture content changes

class TGlxMedia;                      // Reference to a media item in the item pool.
class CGlxUiUtility;                  // Alf utils ; lib glxalfutils.lib
class TSize;                          // Stores a two-dimensional size as a width and a height value
class CGlxDRMUtility;
class CGlxMedia;
/**
 *
 */
NONSHARABLE_CLASS( GlxThumbnailVariantType ) : public CBase, public 
	Alf::MulVariantType, public MGlxTextureObserver
    {
public:
	
	static GlxThumbnailVariantType* NewL( const TGlxMedia& aMedia, const TSize& aSize, 
    		TBool aIsFocused = EFalse );
	
	static GlxThumbnailVariantType* NewLC( const TGlxMedia& aMedia, const TSize& aSize, 
    		TBool aIsFocused = EFalse );
	
	void ConstructL( const TGlxMedia& aMedia, const TSize& aSize, TBool 
			aIsFocused = EFalse );
			
    /** constructor
     * Create via new (EMM) ... */
    GlxThumbnailVariantType( const TGlxMedia& aMedia, const TSize& aSize, 
    		TBool aIsFocused = EFalse );
    /** destructor */
    ~GlxThumbnailVariantType();

    void TextureContentChangedL( TBool aHasContent, CAlfTexture* aNewTexture );
    
    Alf::IMulVariantType::TMulType Type() const ;
    int integer() const ;

private:

    void ConsumeDRMRightsL( const TGlxMedia& aMedia );
    
    TSize ThumbnailSize(const CGlxMedia* aMedia); 
    
    /**
     * Determine if DRM rights need to be consuned based on thumbnail
     * or visual size
     * @param aImageSize size of image
     * @param aBitmapSize bitmap size
     * @return ETrue if rights are to be consumed
     */
     TBool ConsumeRightsBasedOnSize( TSize aImageSize, 
               TSize aBitmapSize);

    /**
     * Determine if the relevant thumbnail is available.
     * @param aMedia The media item.
     * @param aSize Size of the Thumbnail.
     * @return ETrue if the requested size thumbnail present.
     */     
     TBool HasRelevantThumbnail(const TGlxMedia& aMedia, 
                                     const TSize& aSize);
        
private:
    int mTextureId;
    CGlxUiUtility* iUiUtility;
    /** Poniter to instance of DRM utility (owned) */
    CGlxDRMUtility* iDrmUtility; 
    };  

/**
 *
 */
NONSHARABLE_CLASS( GlxIconVariantType ) : public CBase, public Alf::MulVariantType
    {
public:
	
	static GlxIconVariantType* NewL( TInt aIconId, TFileName aFileName, const TSize& aSize );
	
	static GlxIconVariantType* NewLC( TInt aIconId, TFileName aFileName, const TSize& aSize );
	
	void ConstructL( TInt aIconId, TFileName aFileName, const TSize& aSize );
	
    /** constructor
     * Create via new (EMM) ... */
    GlxIconVariantType( TInt aIconId, TFileName aFileName, const TSize& aSize );
    /** destructor */
    ~GlxIconVariantType();
    Alf::IMulVariantType::TMulType Type() const ;
    int integer() const ;

private:
    int mIconId;
    CGlxUiUtility* iUiUtility;
    };  
#endif // _GLXTHUMBNAILVARIANTTYPE_H_

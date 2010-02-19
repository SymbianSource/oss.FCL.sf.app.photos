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
* Description:    Texture Manager
*
*/




// INCLUDE FILES

#include "glxtexturemanagerimpl.h"

#include <AknIconUtils.h>
#include <AknsUtils.h>
#include <aknconsts.h>
#include <alf/alftexturemanager.h>
#include <alf/alfutil.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

#include <glxdrmutility.h>
#include <glxmedia.h>
#include <glxmediageneraldefs.h>
#include <glxthumbnailattributeinfo.h>
#include <glxthumbnailutility.h>
#include <mglxcache.h>
#include <glxresolutionutility.h>
#include <glxlog.h>
#include <glxtracer.h>
#include "mglxtextureobserver.h"

namespace
    {
    const TInt KGlxFirstAlfThumbnailTextureId = 0x10000001;
    const TInt KGlxLastAlfThumbnailTextureId = 0x20000000;
    const TInt KGlxFirstThumbnailTextureId = 0x20000001;
    /** Bitmap size for flat (solid colour) textures */
    const TInt KGlxFlatTextureSize = 64; 
    }


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//  
CGlxTextureManagerImpl::CGlxTextureManagerImpl
       (CAlfTextureManager& aAlfTextureManager) :
    iAlfTextureManager(aAlfTextureManager),
      iNextTextureId( KGlxFirstThumbnailTextureId )
    {
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//  
CGlxTextureManagerImpl::~CGlxTextureManagerImpl()
    {
    TRACER("CGlxTextureManagerImpl::~CGlxTextureManagerImpl");
    
    // delete other icon textures
    // iIconLIst should deleted only once in destructor as
    // they are using in full applicaiton
    TInt count = iIconList.Count();
    for(TInt i = count - 1; i >= 0; i--)
        {
        delete iIconList[i].iTexture;
        }
    iIconList.Close();

    iThumbnailList.Close();

    // delete zoom textures
    count = iZoomedList.Count();
    GLX_LOG_INFO1("CGlxTextureManagerImpl iZoomedList.Count=%d",count);
    for(TInt i = count - 1; i >= 0; i--)
        {
        GLX_LOG_INFO1("CGlxTextureManagerImpldeleted i=%d",i);
        delete iZoomedList[i].iTexture;
        }
    iZoomedList.Close();

    delete iMifFilenames;

    if ( iDrmUtility )
        {
        iDrmUtility->Close();
        }
        
    if ( iCache )
        {
	    iCache->RemoveObserver(this);
        iCache->Close();
        }
        
    if ( iResUtil )
        {
        iResUtil->RemoveObserver(*this);
        iResUtil->Close();
        }
    if(iZoomDecoder)
        {
        delete iZoomDecoder;
        }
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxTextureManagerImpl* CGlxTextureManagerImpl::NewL(
                                        CAlfTextureManager& aAlfTextureManager)
    {
    TRACER("CGlxTextureManagerImpl::NewL");
    CGlxTextureManagerImpl* self = CGlxTextureManagerImpl::NewLC(aAlfTextureManager);
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CGlxTextureManagerImpl* CGlxTextureManagerImpl::NewLC(
                                        CAlfTextureManager& aAlfTextureManager)
    {
    TRACER("CGlxTextureManagerImpl::NewLC");
    CGlxTextureManagerImpl* self = 
                        new (ELeave) CGlxTextureManagerImpl(aAlfTextureManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//  
void CGlxTextureManagerImpl::ConstructL()
    {
    TRACER("CGlxTextureManagerImpl::ConstructL");
    iMifFilenames = new (ELeave) CDesCArrayFlat(2);
    
    // get handle to DRM utility
    iDrmUtility = CGlxDRMUtility::InstanceL();
    // get handle to the media cache
    iCache = MGlxCache::InstanceL();
    iCache->AddObserverL(this);
    
    // Get instance of Resolution utility and register for notification of 
    // resolution changes
    /// @todo implement CGlxResolutionUtility this later        
    iResUtil = CGlxResolutionUtility::InstanceL();
    iResUtil->AddObserverL(*this);
    // Set the TextureId range. Now we can set our own ID outside this Range
    iAlfTextureManager.SetAutomaticTextureIdRange(
                            KGlxFirstAlfThumbnailTextureId, 
                            KGlxLastAlfThumbnailTextureId);
    }
    
    
// -----------------------------------------------------------------------------
// TextureNeedsUpdating
// -----------------------------------------------------------------------------
//  
TBool CGlxTextureManagerImpl::TextureNeedsUpdating( TGlxMedia aMedia,
    TGlxIdSpaceId aIdSpaceId, const TSize& aRequiredSize )
    {
    TRACER("CGlxTextureManagerImpl::TextureNeedsUpdating");
    TInt thumbnailIndex = KErrNotFound;
    TMPXAttribute thumbNailAttribute( 0, 0 );
    TBool updateTexture = ETrue;
    TRAPD( err, updateTexture = GetThumbnailAttributeAndIndexL( aRequiredSize,
        aMedia, aIdSpaceId, thumbnailIndex, thumbNailAttribute ) );
    // don't update the texture if there's an error
    return ( err == KErrNone ? updateTexture : EFalse );
    }
    
// -----------------------------------------------------------------------------
// CreateIconTextureL 
// -----------------------------------------------------------------------------
//
CAlfTexture& CGlxTextureManagerImpl::CreateIconTextureL(
                                               const TAknsItemID& aSkinID,
                                               TInt aIconResourceId, 
                                               const TDesC& aFilename,
                                               TBool aSetColor,
                                               TRgb aDefaultColor,
                                               const TAknsItemID& aSkinColorId,
                                               TInt aSkinColorIndex,
                                               TSize aRequestedSize)
    {
    TRACER("CGlxTextureManagerImpl::CreateIconTextureL");
    // Convert size to POSS
    TReal32 heightInPoss = 
                  iResUtil->PixelsToPoss(TReal32(aRequestedSize.iHeight));
    
    TReal32 widthInPoss = 
                  iResUtil->PixelsToPoss(TReal32(aRequestedSize.iWidth));
    
    
    CAlfTexture* texture = NULL;
    //search array for existing item
    TInt i = iIconList.Count();
    while(i > 0 && !texture)
        {
        i--;
        TGlxIcon icon = iIconList[i];
        if( icon.iAvkonSkinId == aSkinID && 
            icon.iBitmapId == aIconResourceId &&
            icon.iSetColor == aSetColor &&
            icon.iDefaultColor == aDefaultColor &&
            icon.iSkinColorId == aSkinColorId &&
            icon.iSkinColorIndex == aSkinColorIndex &&
            icon.iHeightInPoss == heightInPoss &&
            icon.iWidthInPoss == widthInPoss )
            {
            // also compare file names
            TFileName filename(KNullDesC);
            if(icon.iFilenameIndex >= 0)
                {
                filename = iMifFilenames->MdcaPoint(icon.iFilenameIndex);
                }
            if(0 == filename.CompareF(aFilename) )
                {
                // match found - use existing texture
                texture = icon.iTexture;
                }
            }
        }

    if(!texture)
        {
        // existing texture not found - prepare to create new one
        TInt filenameIndex = -1;
        if(aFilename.Length() > 0)
            {
            // is filename already in filename list
            if(iMifFilenames->MdcaCount())
                {
                iMifFilenames->Find(aFilename, filenameIndex);
                if(filenameIndex == iMifFilenames->MdcaCount())
                    {
                    filenameIndex = -1;
                    }
                }
            if(filenameIndex < 0)
                {
                // name not in list. Add it
                iMifFilenames->AppendL(aFilename);
                filenameIndex = iMifFilenames->MdcaCount() - 1;
                }
            }
        // Create new icon entry
        TGlxIcon icon;
        icon.iTextureId = NextTextureId();
        icon.iTexture = NULL;
        icon.iAvkonSkinId = aSkinID;
        icon.iBitmapId = aIconResourceId;
        icon.iFilenameIndex = filenameIndex;
        icon.iSetColor = aSetColor;
        icon.iDefaultColor = aDefaultColor;
        icon.iSkinColorId  = aSkinColorId;
        icon.iSkinColorIndex = aSkinColorIndex;
        icon.iHeightInPoss = heightInPoss;
        icon.iWidthInPoss = widthInPoss;
        
        // add to list so ProvideBitmapL can use it
        iIconList.AppendL(icon);
        
        // trap create texture
       TRAPD(err, texture = &iAlfTextureManager.CreateTextureL(
                                               icon.iTextureId, 
                                               this, 
                                               EAlfTextureFlagDefault));
        if(err)
            {
            // if error delete entry and leave
            iIconList.Remove(iIconList.Count() - 1);
            User::Leave(err);    
            }
        else
            {
            // if no error add texture to entry
            iIconList[iIconList.Count() - 1].iTexture = texture;
            }
        }

    if (NULL == texture)
        {
        User::Leave(KErrNotFound);
        }
    
    return *texture;
    }



// -----------------------------------------------------------------------------
// CreateNewTextureForMediaL
// -----------------------------------------------------------------------------
//
CAlfTexture& CGlxTextureManagerImpl::CreateNewTextureForMediaL(
                                            TSize aSize, 
                                            const TGlxMedia& aMedia, 
                                            const TGlxIdSpaceId& aIdSpaceId, 
                                            MGlxTextureObserver* aObserver,
                                            TBool aScaleGridTnm)
    {
    TRACER("CGlxTextureManagerImpl::CreateNewTextureForMediaL");
    TInt thumbnailIndex = KErrNotFound;
    TMPXAttribute thumbNailAttribute(0,0);
    TGlxThumbnailIcon thumbData;
    
    // If the current thumbnail matches what is required then return the current texture otherwise
    // create a new one.
    if (!GetThumbnailAttributeAndIndexL(aSize, aMedia, aIdSpaceId, thumbnailIndex, thumbNailAttribute))
        {
        // only texture is missing. 
        if ( (NULL != iThumbnailList[thumbnailIndex].iBitmap) && (NULL == iThumbnailList[thumbnailIndex].iTexture)) 
            {
            CAlfTexture* newTexture = &iAlfTextureManager.CreateTextureL(
                                                                    iThumbnailList[thumbnailIndex].iTextureId, 
                                                                    this, 
                                                                    EAlfTextureFlagDefault);
            iThumbnailList[thumbnailIndex].iTexture = newTexture ;
            }
        return *iThumbnailList[thumbnailIndex].iTexture;
        }
         
    // We have 2 scenarios here.
    // 1) We do not have a thumbnail
    // 2) The thumbnail we have is not good enough   
    iThumbnailList.ReserveL( iThumbnailList.Count() + 1 );

    CAlfTexture& texture = CreateNewTextureL(aSize, aMedia, aIdSpaceId, 
                            aObserver, thumbNailAttribute, thumbData,ETrue,aScaleGridTnm);   


    if ( thumbnailIndex != KErrNotFound )
        {
        // A new texture was created so remove the old one and 
        // update the thumbnail list.
        iAlfTextureManager.UnloadTexture(iThumbnailList[thumbnailIndex].iTextureId);
        iThumbnailList.Remove(thumbnailIndex);
        GLX_LOG_INFO("CGlxTextureManagerImpl CreateNewTextureForMediaL UnloadTexture ");
        }
        
    return texture;
    }

// -----------------------------------------------------------------------------
// CreateFlatColourTextureL
// -----------------------------------------------------------------------------
//  
CAlfTexture& CGlxTextureManagerImpl::CreateFlatColourTextureL( TRgb aColour )
    {
    TRACER("CGlxTextureManagerImpl::CreateFlatColourTextureL");
    // Create a picture with the colour
	CFbsBitmap* picture = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(picture);
	User::LeaveIfError(picture->Create(TSize(KGlxFlatTextureSize,
	                                        KGlxFlatTextureSize), EColor16MU ) );

	CFbsBitmapDevice* drawBufDevice = CFbsBitmapDevice::NewL(picture);
	CleanupStack::PushL(drawBufDevice);

	CFbsBitGc* drawBufContext;
	User::LeaveIfError(drawBufDevice->CreateContext(drawBufContext));
	CleanupStack::PushL(drawBufContext);
	
    // draw the color
	drawBufContext->Activate(drawBufDevice);
    drawBufContext->SetBrushColor(aColour);
    drawBufContext->Clear();
    
    TInt alpha = aColour.Alpha();
	
    CleanupStack::PopAndDestroy(drawBufContext); 
    CleanupStack::PopAndDestroy(drawBufDevice); 
	
	CFbsBitmap* mask = NULL;
	if( alpha != 0 )
	    {
	    mask = new (ELeave) CFbsBitmap();
    	CleanupStack::PushL( mask );
    	User::LeaveIfError( mask->Create(TSize( KGlxFlatTextureSize,
    	                                        KGlxFlatTextureSize ), EGray256 ));
    	                                        
        CFbsBitmapDevice* maskBufDevice = CFbsBitmapDevice::NewL( mask );
    	CleanupStack::PushL( maskBufDevice );

    	CFbsBitGc* maskBufContext = NULL;
    	User::LeaveIfError( maskBufDevice->CreateContext( maskBufContext ) );
    	CleanupStack::PushL( maskBufContext );

        // draw the mask    	
    	maskBufContext->Activate(maskBufDevice);
        maskBufContext->SetBrushColor( TRgb::Color256( alpha ) );
        maskBufContext->Clear();
        
        CleanupStack::PopAndDestroy( maskBufContext ); 
        CleanupStack::PopAndDestroy( maskBufDevice ); 
        }
    
    // Manage the texture ids ourself. Get the next id and store the
    // associated bitmap and mask to ensure that we supply the correct bitmaps
    // when alf calls back to ProvideBitmap 
    TInt nextTextureId = NextTextureId();
    iProvideBitmapHolder.iTextureId = nextTextureId;
    iProvideBitmapHolder.iBitmap = picture;
    iProvideBitmapHolder.iMaskBitmap = mask;
    
    CAlfTexture& newTexture = iAlfTextureManager.CreateTextureL(
                                                           nextTextureId, 
                                                           this, 
                                                           EAlfTextureFlagDefault );
    // don't want alf to delete this texture
    newTexture.SetPriority( EAlfTexturePriorityHighest );
    
    // if we had alpha, release the mask
	if( alpha != 0 )
	    {
	    CleanupStack::Pop( mask );
	    }
    
    CleanupStack::Pop( picture ); 

    return newTexture;
    }
    
    
// -----------------------------------------------------------------------------
// RemoveTexture
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::RemoveTexture( const CAlfTexture& aTexture )
    {
    TRACER("CGlxTextureManagerImpl::RemoveTexture");
    // Find the texture in the iThumbnailList
    TInt index = iThumbnailList.Find( aTexture, &TGlxThumbnailIcon::MatchTexture );
    if ( index != KErrNotFound )
        {
        iThumbnailList.Remove( index );
        iAlfTextureManager.UnloadTexture( aTexture.Id() );
        }
    else
        {
        // Find the texture in the zoomed list
        index = iZoomedList.Find( aTexture, &TGlxThumbnailIcon::MatchTexture );
        GLX_LOG_INFO1("CGlxTextureManagerImpl RemoveTexture index=%d",index);
        if ( index != KErrNotFound )
            {
            GLX_LOG_INFO("CGlxTextureManagerImpl RemoveTexture index != KErrNotFound");
            iAlfTextureManager.UnloadTexture( aTexture.Id() );
            if(iZoomedList[index].iBitmap)
                {
                GLX_LOG_INFO("CGlxTextureManagerImpl RemoveTexture iZoomedList[index].iBitmap delete");
                delete iZoomedList[index].iBitmap;
                iZoomedList[index].iBitmap=NULL;
                iZoomedList.Remove( index );
                }
            }
            // iIconLIst should deleted only once in destructor as
            // they are using in full applicaiton
        }
    }

// -----------------------------------------------------------------------------
// RemoveZoomList
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::RemoveZoomList()
    {
    TRACER("CGlxTextureManagerImpl::RemoveZoomList");
    TInt count = iZoomedList.Count();
    GLX_LOG_INFO1("CGlxTextureManagerImpl RemoveZoomList  Count()=%d",count);
	if(iZoomDecoder)
      {
       iZoomDecoder->Cancel();
      }
    for(TInt i = count - 1; i >= 0; i--)
        {
        iAlfTextureManager.UnloadTexture( iZoomedList[i].iTextureId );
        if( iZoomedList[i].iBitmap)
            {
            GLX_LOG_INFO("CGlxTextureManagerImpl RemoveZoomList delete iZoomedList[i]");
            delete iZoomedList[i].iBitmap;
            iZoomedList[i].iBitmap=NULL;
            }
        iZoomedList.Remove( i );
        }
    iZoomedList.Reset();

    }

// -----------------------------------------------------------------------------
// RemoveTexture Using TGlxMediaId&
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::RemoveTexture(const TGlxMediaId& aMediaId,TBool aAllTexture )
    {
    TRACER("CGlxTextureManagerImpl::RemoveTexture 2");
    //Bug fix for PKAA-7NRBYZ - added bAllTexture param
    // Find the texture in the iThumbnailList
    TInt i = iThumbnailList.Count();
    while(i > 0)
        {
        --i;
        if (iThumbnailList[i].iMediaId == aMediaId)
            {
            TInt aTexture = iThumbnailList[i].iTextureId;
            iThumbnailList.Remove(i);
            iAlfTextureManager.UnloadTexture(aTexture );
       	//Bug fix for PKAA-7NRBYZ - Delete all the texture only if it is asked.
            if(!aAllTexture)
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// FlushTextures Removes All Textures
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::FlushTextures()
    {
    TRACER("CGlxTextureManagerImpl::FlushTextures");

    TInt textureID ;
    TInt i = iThumbnailList.Count();
    
    while(i > 0)
        {
        --i;
        textureID = iThumbnailList[i].iTextureId;
        iAlfTextureManager.UnloadTexture(textureID );
        iThumbnailList[i].iTexture = NULL;
        }
    
    i = iIconList.Count();
    while(i > 0)
        {
        --i;
        textureID = iIconList[i].iTextureId;
        iAlfTextureManager.UnloadTexture(textureID );
        iIconList[i].iTexture = NULL;
        }
    
    i = iZoomedList.Count();
    while(i > 0)
        {
        --i;
        textureID = iZoomedList[i].iTextureId;
        iAlfTextureManager.UnloadTexture(textureID );
        iZoomedList[i].iTexture = NULL;
        }
    }

// -----------------------------------------------------------------------------
// GetThumbnailAttributeAndIndexL
// -----------------------------------------------------------------------------
//
TBool CGlxTextureManagerImpl::GetThumbnailAttributeAndIndexL( TSize aSize,
    const TGlxMedia& aMedia, const TGlxIdSpaceId& aIdSpaceId,
    TInt& aThumbnailIndex, TMPXAttribute& aThumbnailAttribute )
    {
    TRACER("CGlxTextureManagerImpl::GetThumbnailAttributeAndIndexL");
    GLX_LOG_INFO( "CGlxTextureManagerImpl::GetThumbnailAttributeAndIndexL" );
    aThumbnailIndex = KErrNotFound;
    
    aThumbnailAttribute = SelectAttributeL(aSize, aMedia);  // Find Thumbnail attributes

    TInt i = iThumbnailList.Count();
	TTime reqTime;
	aMedia.GetLastModifiedDate(reqTime);
    while(i > 0 && aThumbnailIndex == KErrNotFound)
        {
        --i;
        if ((iThumbnailList[i].iMediaId == aMedia.Id()) && 
            (iThumbnailList[i].iIdSpaceId == aIdSpaceId) &&
			(iThumbnailList[i].iImageDate == reqTime) &&
					(iThumbnailList[i].iRequiredSize == aSize))
            {
            aThumbnailIndex = i;
            if ( iThumbnailList[aThumbnailIndex].iAttribId ==  aThumbnailAttribute) 
                {
                // We have found that the best match already exists
                // No need to do anything
                return EFalse;
                }
            }
        }
    return ETrue;
    }
// -----------------------------------------------------------------------------
// CreateNewTextureL
// -----------------------------------------------------------------------------
CAlfTexture* CGlxTextureManagerImpl::CreateZoomedTextureL()
    {
    TRACER("CGlxTextureManagerImpl::CreateZoomedTextureL");
    TInt count = iZoomedList.Count();
    TInt index = 0 ;
    
    for (index = count-1; index >=0  ; index--)
        {
        if (NULL == iZoomedList[index].iTexture)
            {
            if (NULL != iZoomedList[index].iBitmap)
                {
                // If we got this far we need to create a new texture
                iZoomedList[index].iTextureId = NextTextureId();
                iZoomedList[index].iTexture = &iAlfTextureManager.CreateTextureL(
                                                                       iZoomedList[index].iTextureId, 
                                                                       this, 
                                                                       EAlfTextureFlagDefault);
                return iZoomedList[index].iTexture ;
                }
            }
        }
    return NULL;
    }
// -----------------------------------------------------------------------------
// CreateNewTextureL
// -----------------------------------------------------------------------------
CAlfTexture& CGlxTextureManagerImpl::CreateNewTextureL( TSize aSize,
    const TGlxMedia& aMedia, const TGlxIdSpaceId& aIdSpaceId,
    MGlxTextureObserver* aObserver, TMPXAttribute& aAttribute,
    TGlxThumbnailIcon& aThumbData,TBool aIsThumbnailTexture, TBool aScaleGridTnm )
    {
    TRACER("CGlxTextureManagerImpl::CreateNewTextureL");
    // This method only works if the call to CreateTextureL which in turn calls ProvideBitmapL behaves ina synchronous manor
    
    // First find if there is an existing texture
    // Find the texture in the iThumbnailList
    
    // We have 2 scenarios here.
    // 1) We do not have a thumbnail
    // 2) The thumbnail we have is not good enough
    
    aThumbData.iTextureId = NextTextureId();
    aThumbData.iTexture = NULL;
    aThumbData.iAttribId = aAttribute;
    aThumbData.iMediaId = aMedia.Id();
    aThumbData.iIdSpaceId = aIdSpaceId;
	//Get the ThumbData creation date
	aMedia.GetLastModifiedDate(aThumbData.iImageDate);
    const CGlxThumbnailAttribute* thumb = aMedia.ThumbnailAttribute(aAttribute);
    __ASSERT_DEBUG(thumb, Panic(EGlxPanicLogicError)); // thumb should not be NULL
    aThumbData.iBitmap = thumb->iBitmap;
    CFbsBitmap *scaledBitmap = NULL;
    
    if(aScaleGridTnm)
        {
#ifdef _DEBUG        
        TTime startTime, stopTime;
        startTime.HomeTime();
#endif        
        TSize origSize;
        aMedia.GetDimensions(origSize);
        scaledBitmap = new (ELeave) CFbsBitmap();
        CleanupStack::PushL(scaledBitmap);
        ScaleGridTnmToFsL(origSize,aSize,scaledBitmap,thumb);
        aThumbData.iBitmap = scaledBitmap;

#ifdef _DEBUG
        stopTime.HomeTime();        
        GLX_LOG_INFO1("=>GRID TNM SCALE TO FS took <%d> us", 
                        (TInt)stopTime.MicroSecondsFrom(startTime).Int64());
#endif        
        }
    
    if( aIsThumbnailTexture )
	    {
	     //Add to the thumbnail list
        GLX_LOG_INFO("CGlxTextureManagerImpl::CreateNewTextureL iThumbnailList.Append ");
	    iThumbnailList.Append(aThumbData); 	
	    }
   else
	   {
        GLX_LOG_INFO1("CGlxTextureManagerImpl::CreateNewTextureL,count=%d",iZoomedList.Count());
	   iZoomedList.Append(aThumbData);
	   }
         
    // If we got this far we need to create a new texture
    CAlfTexture* newTexture = &iAlfTextureManager.CreateTextureL(
                                                           aThumbData.iTextureId, 
                                                           this, 
                                                           EAlfTextureFlagDefault);
        
    
    if( aIsThumbnailTexture )
	    {
        GLX_LOG_INFO("CGlxTextureManagerImpl::CreateNewTextureL aIsThumbnailTexture ");
	    TInt index = iThumbnailList.Count()-1;
	    iThumbnailList[index].iTexture = newTexture;
	    iThumbnailList[index].iRequiredSize = aSize;
	    }
	else
	    {
        GLX_LOG_INFO("CGlxTextureManagerImpl::CreateNewTextureL else aIsThumbnailTexture ");
	    TInt index = iZoomedList.Count()-1;
	    iZoomedList[index].iTexture = newTexture;
	    iZoomedList[index].iRequiredSize = aSize;
        iZoomedList[index].iBitmap=NULL;
	    }
    
    if (aObserver)
        {
        GLX_LOG_INFO("CGlxTextureManagerImpl::CreateNewTextureL aObserver ");
        aThumbData.iObserver = aObserver;
        aThumbData.iObserver->TextureContentChangedL( ETrue ,  newTexture);
        }
    
    if(scaledBitmap)
       {
       CleanupStack::PopAndDestroy(scaledBitmap);  
       }
	
    return *newTexture;
    }

// -----------------------------------------------------------------------------
// CreateZoomedTextureL
// -----------------------------------------------------------------------------
//  
CAlfTexture& CGlxTextureManagerImpl::CreateZoomedTextureL(
                    const TGlxMedia& aMedia, const TMPXAttribute& aAttribute,
                    TGlxIdSpaceId aIdSpaceId, MGlxTextureObserver* aObserver)
    {
    TRACER("CGlxTextureManagerImpl::CreateZoomedTextureL");
    if(!iZoomDecoder)
        {
        GLX_LOG_INFO("CGlxTextureManagerImpl:: CreateZoomedTextureL,iZoomDecoder == NULL");
        iZoomDecoder = CGlxBitmapDecoderWrapper::NewL(this);
        }
        
    TGlxThumbnailIcon aThumbData;
            
    aThumbData.iTextureId = NextTextureId();
    aThumbData.iTexture = NULL;
    aThumbData.iAttribId = aAttribute;
    aThumbData.iMediaId = aMedia.Id();
    aThumbData.iIdSpaceId = aIdSpaceId;
    
    //Add to the thumbnail list
    iZoomedList.Append(aThumbData);  
            
    TInt index = iZoomedList.Count()-1;
	GLX_LOG_INFO1("CGlxTextureManagerImpl:: CreateZoomedTextureL,index=%d",index);
		
	iZoomedList[index].iTexture = NULL;
	aMedia.GetDimensions( iZoomedList[index].iRequiredSize );
	   
	iZoomedList[index].iBitmap=NULL;
	iZoomedList[index].iObserver = aObserver ;  

	iZoomDecoder->DoDecodeImageL(aMedia.Uri(), iZoomedList.Count()-1);
	return *(iZoomedList[index].iTexture);
    }

// -----------------------------------------------------------------------------
// CreateAnimatedGifTextureL
// -----------------------------------------------------------------------------
//  
CAlfTexture& CGlxTextureManagerImpl::CreateAnimatedGifTextureL(
    const TDesC& aFilename, const TSize& aSize)
    { 
    TRACER("CGlxTextureManagerImpl::CreateAnimatedGifTextureL");
    return iAlfTextureManager.LoadTextureL(aFilename,aSize, 
        EAlfTextureFlagDefault,NextTextureId() );
    }

    
// -----------------------------------------------------------------------------
// UpdateTexture
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::UpdateTexture()
	{
	TRACER("CGlxTextureManagerImpl::UpdateTexture");
	TInt count = iIconList.Count();
    for(TInt i = count - 1; i >= 0; i--)
        {
        //to-check
       // TRAP_IGNORE(DoHandleResolutionChangedL(i);)
        }
    }
    
// -----------------------------------------------------------------------------
// HandleResolutionChanged
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::HandleResolutionChangedL()
    {
    TRACER("CGlxTextureManagerImpl::HandleResolutionChangedL");
    UpdateTexture();
    }

// -----------------------------------------------------------------------------
// DoHandleResolutionChangedL
// -----------------------------------------------------------------------------
//
//To-do
/*void CGlxTextureManagerImpl::DoHandleResolutionChangedL(TInt aIconIndex)
    {
    TGlxIcon& icon = iIconList[aIconIndex];
    CAlfTexture* texture = icon.iTexture;
    // Clear previous content
    //texture->SetSegmentCountL(0);
    
    // Get new bitmaps
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    ProvideBitmapL(icon.iTextureId, bitmap, mask);
    // Upload to texture
    if(bitmap)
        {
        CleanupStack::PushL(bitmap);
        texture->SetSize(bitmap->SizeInPixels());
        if(mask)
            {
            CleanupStack::PushL(mask);
            texture->UploadL( *bitmap, mask,
                        EAlfTextureUploadFlagDefault );
            CleanupStack::PopAndDestroy(mask);
            }
        else
            {
            texture->UploadL( *bitmap, NULL,
                        EAlfTextureUploadFlagDefault );
            
            }
        CleanupStack::PopAndDestroy(bitmap);    
        }
    }*/


// -----------------------------------------------------------------------------
// ProvideBitmapL
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::ProvideBitmapL(TInt aTextureId, 
                                        CFbsBitmap*& aBitmap, 
                                        CFbsBitmap*& aMaskBitmap)
    {
    TRACER("CGlxTextureManagerImpl::ProvideBitmapL");
    // Test whether we've already created bitmaps for the given texture id
    if ( aTextureId == iProvideBitmapHolder.iTextureId )
        {
        GLX_LOG_INFO("CGlxTextureManagerImpl::ProvideBitmapL aTextureId");
        aBitmap = iProvideBitmapHolder.iBitmap;
        aMaskBitmap = iProvideBitmapHolder.iMaskBitmap;
        // reset the flags to prevent multiple calls into here
        iProvideBitmapHolder.iTextureId = KErrNotFound;
        iProvideBitmapHolder.iBitmap = NULL;
        iProvideBitmapHolder.iMaskBitmap = NULL;
        return;
        }
    
    // loop to search the zoomed thumbnail list
    TInt i = iZoomedList.Count();
    GLX_LOG_INFO1("CGlxTextureManagerImpl::ProvideBitmapL iZoomedList.Count=%d",i);
    while(i > 0)
        {
        --i;
        if (iZoomedList[i].iTextureId == aTextureId)
            {
            // We have found the texture, create a duplicate as alf destroys
            // the bitmap once the texture's been created.
            GLX_LOG_INFO1("CGlxTextureManagerImpl::ProvideBitmapL i=%d",i);
            aBitmap = CreateDuplicateBitmapL( *iZoomedList[i].iBitmap );
            return;
            }
        }
    // add loop to search the iThumbnailList for the aTextureId
    i = iThumbnailList.Count();
    while(i > 0)
        {
        --i;
        if ((iThumbnailList[i].iTextureId == aTextureId))
            {
            // We have found the texture, create a duplicate as alf destroys
            // the bitmap once the texture's been created.
            aBitmap = CreateDuplicateBitmapL( *iThumbnailList[i].iBitmap );
            return;
            }
        }
    // find in iIconList
    i = iIconList.Count();
    TBool found = EFalse;
    TGlxIcon icon;
    while(i > 0 && !found)
        {
        i--;
        if(iIconList[i].iTextureId == aTextureId)
            {
            icon = iIconList[i];
            found = ETrue;
            }
        }
    
    if(!found)
        {
        User::Leave(KErrNotFound);
        }
    
    TInt bitmapId = icon.iBitmapId;
    TInt maskId  = bitmapId;
    if(bitmapId)
        {
        maskId++;
        }
        
    TFileName filename(KNullDesC);
    if(icon.iFilenameIndex >= 0)
        {
        filename = iMifFilenames->MdcaPoint(icon.iFilenameIndex);
        }
        
    if(icon.iSetColor)
        {
        AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(), 
                                     icon.iAvkonSkinId,
                                     icon.iSkinColorId, 
                                     icon.iSkinColorIndex,
                                     aBitmap, 
                                     aMaskBitmap,
                                     filename,
                                     bitmapId, 
                                     maskId,
                                     icon.iDefaultColor );
        }
    else
        {
         AknsUtils::CreateIconL( AknsUtils::SkinInstance(), 
                                 icon.iAvkonSkinId,
                                 aBitmap, 
                                 aMaskBitmap, 
                                 filename,
                                 bitmapId, 
                                 maskId );
        }

    // Leave if bitmap or mask is NULL.
    
    if (NULL == aBitmap)
        {
        User::Leave(KErrNotFound);
        }
    
    // set size
    // In preference use requested size if there is one
    TInt height =TInt(iResUtil->PossToPixels(icon.iHeightInPoss));
    TInt width =TInt(iResUtil->PossToPixels(icon.iWidthInPoss));
    
    TSize size(width, height);

    // Disable compression so that the bitmap may be able to be 
    // duplicated inside HUITK when BITGDI renderer is in use.
    AknIconUtils::DisableCompression( aBitmap );

    if ( size == TSize(0, 0) )
        {
        // If no requested size use original size of the graphic
        TAknContentDimensions origDim;
        AknIconUtils::GetContentDimensions(aBitmap, origDim);
        size = TSize(origDim.iWidth, origDim.iHeight);
        
        // But if the original size is small make smallest dimension 64
        // and keep aspect ratio
        if ( size.iHeight < 64 && size.iWidth < 64 )
            {
            if(origDim.iWidth < origDim.iHeight)
                {
                size = TSize(64, 64.0F * origDim.iHeight / origDim.iWidth);
                }
            else
                {
                size = TSize(64.0F * origDim.iWidth / origDim.iHeight, 64);
                }
            }
        }
    AknIconUtils::SetSize(aBitmap, size, EAspectRatioNotPreserved);
    }

// -----------------------------------------------------------------------------
// SelectAttributeL
// -----------------------------------------------------------------------------
//
TMPXAttribute CGlxTextureManagerImpl::SelectAttributeL( TSize& aSize,
                                              const TGlxMedia& aMedia )
    {
    TRACER("CGlxTextureManagerImpl::SelectAttributeL");
    const CGlxMedia& media = *aMedia.Properties();
    if ( !aMedia.Properties() || media.Count() == 0 )
        {
        GLX_LOG_INFO("SelectAttributeL - NULL CGlxMedia / No Attribs"); 
        User::Leave( KErrArgument );
        }
    // first check if DRM protected
    TBool drmInvalid = EFalse;
    if ( aMedia.GetDrmProtected(drmInvalid) && drmInvalid )
        {
        // require URI attribute
        TMPXGeneralCategory cat = aMedia.Category();
	    // get URI 
        const TDesC& uri = aMedia.Uri();
        if( (uri.Length() == 0) || (EMPXNoCategory == cat) ) 
            {
            User::Leave( KErrArgument );
            }

        // check if rights have expired
        TBool checkViewRights = (cat==EMPXImage);

        drmInvalid = !iDrmUtility->CheckDisplayRightsL(uri, checkViewRights);
        CGlxMedia* properties = const_cast<CGlxMedia*>(aMedia.Properties());
        if( !drmInvalid )
            {
            properties->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid, EGlxDrmRightsValid);
            }
        else
            {
            properties->SetTObjectValueL(KGlxMediaGeneralDRMRightsValid, EGlxDrmRightsInvalid);
            }
        }

    // if invalid need dimensions
    if ( drmInvalid )
        {
        TSize dimensions(0,0);
        if(!aMedia.GetDimensions(dimensions))
            {
            User::Leave( KErrArgument );
            }

        aSize = iDrmUtility->DRMThumbnailSize(dimensions);
        }

    // Get the index for the attribute
    TInt index =  GlxThumbnailUtility::ClosestThumbnail( aSize,
                                                media, !drmInvalid );
    // check it's valid
    if (KErrNotFound == index)
        {
        GLX_LOG_INFO("GlxThumbnailUtility::ClosestThumbnail fail ");	
        User::Leave( KErrArgument );
        }

    return media.Attribute(index);
    }


// -----------------------------------------------------------------------------
// CreateDuplicateBitmapL
// -----------------------------------------------------------------------------
//
CFbsBitmap* CGlxTextureManagerImpl::CreateDuplicateBitmapL(CFbsBitmap& aOriginalBitmap )
    {
    TRACER("CGlxTextureManagerImpl::CreateDuplicateBitmapL");
    CFbsBitmap* duplicateBitmap = new( ELeave) CFbsBitmap();
    TInt err=duplicateBitmap->Duplicate( aOriginalBitmap.Handle());
    if ( err != KErrNone )
        {
        delete duplicateBitmap;
        duplicateBitmap = NULL;
        GLX_LOG_INFO1("CGlxTextureManagerImpl::CreateDuplicateBitmapL fail =%d",err);
        }
    
    return duplicateBitmap;
    }

// -----------------------------------------------------------------------------
// NextTextureId
// -----------------------------------------------------------------------------
//
TInt CGlxTextureManagerImpl::NextTextureId()
    {
    TRACER("CGlxTextureManagerImpl::NextTextureId");
    return ++iNextTextureId;
    }

    
void CGlxTextureManagerImpl::HandleBitmapDecodedL(TInt aThumbnailIndex,CFbsBitmap* aBitmap)
    {
    TRACER("CGlxTextureManagerImpl::HandleBitmapDecodedL");
    CAlfTexture* newTexture = NULL;
    TBool textureCreated = ETrue;
    TInt textureID = -1;
    TInt textureToBeUnloaded = -1;

    //check if the image is decoded
    if ( aBitmap )
        {
        iZoomedList[aThumbnailIndex].iBitmap = aBitmap;

        //if we already have a texture then dont unload the texture before creating 
        //the next one. It might happen that because of low memory we might not be able
        //to create a new texture.
        if(iZoomedList[aThumbnailIndex].iTexture)
            {				    
            textureID = NextTextureId();
            textureToBeUnloaded = iZoomedList[aThumbnailIndex].iTextureId  ;   
            iZoomedList[aThumbnailIndex].iTextureId = textureID  ;
            }
        else
            {
            textureID = iZoomedList[aThumbnailIndex].iTextureId;
            }
        
        TRAPD(err, newTexture = &iAlfTextureManager.CreateTextureL(
                                             textureID, this, EAlfTextureFlagDefault));
        if ( KErrNone != err && KErrNoMemory == err )
            {  
            GLX_LOG_INFO("CGlxTextureManagerImpl::HandleBitmapDecoded ReleaseRam : START RAM  RELEASE");
            iCache->ReleaseRAML(ETrue);
            TRAPD(err, newTexture = &iAlfTextureManager.CreateTextureL(textureID, this, EAlfTextureFlagDefault));
            if ( KErrNone != err && KErrNoMemory == err )
                {
                GLX_LOG_INFO("CGlxTextureManagerImpl::HandleBitmapDecoded CreateTextureL : LOW MEMORY CONDITION");
                textureCreated = EFalse;
                }		
            }
        }    
    else
        {
        textureCreated = EFalse;	     
        }



    if (textureCreated && (NULL != newTexture))
        {
        GLX_LOG_INFO("CGlxTextureManagerImpl::HandleBitmapDecoded textureCreated && newTexture");
        //If the new texture is created then unload the old texture and store the new texture and textureID.
        if(iZoomedList[aThumbnailIndex].iTexture)
            {				    
            GLX_LOG_INFO("CGlxTextureManagerImpl::HandleBitmapDecoded Unloading Old Texture");
            iAlfTextureManager.UnloadTexture(textureToBeUnloaded );
            }
        iZoomedList[aThumbnailIndex].iTextureId = textureID;
        iZoomedList[aThumbnailIndex].iTexture = newTexture;
        }

    if (iZoomedList[aThumbnailIndex].iObserver)
        {
        GLX_LOG_INFO("CGlxTextureManagerImpl::HandleBitmapDecoded TextureContentChangedL");
        iZoomedList[aThumbnailIndex].iObserver->TextureContentChangedL( textureCreated ,iZoomedList[aThumbnailIndex].iTexture);
        }
    GLX_LOG_INFO("CGlxTextureManagerImpl::HandleBitmapDecodedL Exit ");
    }

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::HandleAttributesAvailableL(const TGlxIdSpaceId& /*aIdSpaceId*/, 
                                                const TGlxMediaId& /*aMediaId*/, 
                                                const RArray<TMPXAttribute>& /*aAttributes*/, 
                                                const CGlxMedia* /*aMedia*/)
	{
		
	}

// -----------------------------------------------------------------------------
// CleanupTextureCacheL
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::CleanupMedia(const TGlxMediaId& aMediaId)
	{
	TRACER("CGlxTextureManagerImpl::CleanupMedia");
	RemoveTexture(aMediaId, EFalse);
	}



// -----------------------------------------------------------------------------
// ScaleGridTnmToFsL
// -----------------------------------------------------------------------------
//
void CGlxTextureManagerImpl::ScaleGridTnmToFsL(TSize aSrcSize, TSize aDestSize, 
                   CFbsBitmap *aScaledBitmap, const CGlxThumbnailAttribute* thumb)
    {      
	TRACER("CGlxTextureManagerImpl::ScaleGridTnmToFsL");
    // Scale the Image to required size if the thumbnail is smaller.Instead of Displaying Small thumbnail        
    // Calculate destination rect to maintain aspect ratio
    TReal aspectRatio = (TReal)aDestSize.iWidth/(TReal)aDestSize.iHeight ;
    TReal thumbAspectRatio = (TReal)aSrcSize.iWidth/(TReal)aSrcSize.iHeight ;
    TSize destinationSize ;
    if(thumbAspectRatio >= aspectRatio)
       {
       destinationSize.iWidth = aDestSize.iWidth ;
       destinationSize.iHeight = aDestSize.iWidth*aSrcSize.iHeight/aSrcSize.iWidth ;
       }
    else
       {
       destinationSize.iHeight = aDestSize.iHeight ;
       destinationSize.iWidth  = aDestSize.iHeight*aSrcSize.iWidth/aSrcSize.iHeight ;
       }
    aScaledBitmap->Create(destinationSize,thumb->iBitmap->DisplayMode());
    CFbsBitmapDevice *bitmapDevice = CFbsBitmapDevice::NewL(aScaledBitmap);
    CleanupStack::PushL(bitmapDevice);
    CFbsBitGc * bitmapGc = CFbsBitGc::NewL();
    CleanupStack::PushL(bitmapGc);
    bitmapGc->Activate(bitmapDevice);
    bitmapGc->DrawBitmap(TRect(destinationSize),thumb->iBitmap);
    CleanupStack::PopAndDestroy(bitmapGc);      
    CleanupStack::PopAndDestroy(bitmapDevice);
    GLX_LOG_INFO2("===== Actual Image Size %d x %d",aSrcSize.iWidth,aSrcSize.iHeight);
    GLX_LOG_INFO2("===== Scaled Image Size %d x %d",destinationSize.iWidth,destinationSize.iHeight);
    }

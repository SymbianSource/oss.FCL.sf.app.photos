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




#include "glxmulthumbnailvarianttype.h"
#include <data_caging_path_literals.hrh>
#include <alf/ialfvarianttype.h>
#include "glxuiutility.h"
#include "glxtexturemanager.h"
#include <glxerrormanager.h>
#include <glxmedia.h>
#include <glxthumbnailattributeinfo.h>
#include <glxicons.mbg>
#include <glxlog.h>    // Logging
#include <glxtracer.h>
#include <glxdrmutility.h>
#include <mglxmedialist.h>
#include <caf/caferr.h>
_LIT(KGlxIconsFilename, "glxicons.mif");


// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
GlxThumbnailVariantType* GlxThumbnailVariantType::NewL( const TGlxMedia& aMedia, const TSize& aSize, 
	TBool aIsFocused, TInt aTextureId  )
	{
	TRACER("GlxThumbnailVariantType::NewL");
	GlxThumbnailVariantType* self = GlxThumbnailVariantType::NewLC( aMedia, 
		aSize, aIsFocused, aTextureId );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//
GlxThumbnailVariantType* GlxThumbnailVariantType::NewLC( const TGlxMedia& aMedia, const TSize& aSize, 
	TBool aIsFocused, TInt aTextureId )
	{
	TRACER("GlxThumbnailVariantType::NewLC");
	
	GlxThumbnailVariantType* self = new ( EMM ) GlxThumbnailVariantType(
		aMedia, aSize, aIsFocused, aTextureId );
	CleanupStack::PushL( self );
	self->ConstructL( aMedia, aSize, aIsFocused, aTextureId );
	return self;
	}

// ----------------------------------------------------------------------------
// constructor
// ----------------------------------------------------------------------------
//
GlxThumbnailVariantType::GlxThumbnailVariantType( const TGlxMedia& /*aMedia*/, 
    const TSize& /*aSize*/, TBool /*aIsFocused*/, TInt /*aTextureId*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void GlxThumbnailVariantType::ConstructL( const TGlxMedia& aMedia, const TSize& aSize, 
		TBool aIsFocused, TInt aTextureId )   
	{
	TRACER("GlxThumbnailVariantType::ConstructL");    
    GLX_DEBUG2("GlxThumbnailVariantType::ConstructL Media Id=%d", aMedia.Id().Value());
    TInt err = KErrNone;

    if(aTextureId != KErrNotFound)
        {
        mTextureId = aTextureId;
        GLX_DEBUG1("GlxThumbnailVariantType::ConstructL valid aTextureId");
        return;
        }
    
    iUiUtility = CGlxUiUtility::UtilityL();
    TSize defaultSize = iUiUtility->GetGridIconSize();
    TFileName resFile(KDC_APP_BITMAP_DIR);
	resFile.Append(KGlxIconsFilename);

	const TDesC& uri = aMedia.Uri();
    GLX_LOG_URI( "GlxThumbnailVariantType::ConstructL(uri=%S)", &uri );
    
    if (!iUiUtility->GetForegroundStatus() || uri.Length() == 0)
        {        
        GLX_DEBUG1("GlxThumbnailVariantType::Create default texture & return");
        TRAP(err, mTextureId = iUiUtility->GlxTextureManager().CreateIconTextureL(
                        EMbmGlxiconsQgn_prop_image_notcreated, resFile, defaultSize).Id());
        return;
        }

    TBool drm = EFalse;
    TGlxMediaGeneralRightsValidity isValid = EGlxDrmRightsValidityUnknown;
	
	if(aMedia.GetDrmProtected(drm))
        {
        GLX_DEBUG1("GlxThumbnailVariantType::ConstructL CGlxDRMUtility::InstanceL");        
        iDrmUtility = CGlxDRMUtility::InstanceL();
        aMedia.GetDrmValidity(isValid);
        }

	TBool fsTnmAvailable = HasRelevantThumbnail(aMedia,aSize);    
    
	TInt frameCount = 0;
	aMedia.GetFrameCount(frameCount);
	
	TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                                aMedia.Properties(), KGlxMediaIdThumbnail );
                                	
    TIconInfo icon;
	TBool expired = EFalse;
  
    if (aIsFocused && frameCount > 1 && fsTnmAvailable && !drm)
        {
        GLX_DEBUG1("GlxThumbnailVariantType::CreateAnimatedGifTextureL");

        TRAP( err, mTextureId = iUiUtility->GlxTextureManager().
                CreateAnimatedGifTextureL( uri, aSize, aMedia,
                        aMedia.IdSpaceId() ).Id() );
        }
    //URI length could be zero for Media Id based Thumbnail fetch
    else if ( fsTnmAvailable ) 
	    {
        TMPXGeneralCategory cat = aMedia.Category();
        //Check if media is DRM rights protected
        if (drm)
            {
            TBool privatePath = EFalse;
            CreateImageViewerInstanceL();
            if (iImageViewerInstance->IsPrivate())
                {
                privatePath = ETrue;
                }

            //call 'DisplayItemRightsCheckL' only for focused item
            if (aIsFocused)
                {
                if (privatePath)
                    {
                    expired = !iDrmUtility->DisplayItemRightsCheckL
                                        (iImageViewerInstance->ImageFileHandle(),(cat == EMPXImage));
                    }
                else
                    {
                    expired = !iDrmUtility->DisplayItemRightsCheckL(uri, (cat == EMPXImage));
                    }
                }
            else
                {
                if (privatePath)
                    {
                    expired = !iDrmUtility->ItemRightsValidityCheckL(
                            iImageViewerInstance->ImageFileHandle(), (cat
                                    == EMPXImage));
                    }
                else
                    {
                    //call 'ItemRightsValidityCheckL' for validity check of non-focused item
                    expired = !iDrmUtility->ItemRightsValidityCheckL(uri,
                            (cat == EMPXImage));
                    }
                }
            DeleteImageViewerInstance();
            //Check If DRM rights have expired.
            if (expired)
                {
                GLX_DEBUG1("GlxThumbnailVariantType::CreateIconTextureL:Default (expired)");
                TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateIconTextureL(
                                EMbmGlxiconsQgn_prop_image_notcreated, resFile, defaultSize ).Id() );
                }
            else
                {
                if (isValid == EGlxDrmRightsValid)
                    {
                    // Fix for EABI-7RL9DD
                    // Replaced defaultSize with aSize
                    GLX_DEBUG1("GlxThumbnailVariantType::CreateThumbnailTextureL:EGlxDrmRightsValid");
                    TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateThumbnailTextureL(
                                    aMedia, aMedia.IdSpaceId(), aSize, this ).Id() );
                    }

                }
            }
        else
            {
            GLX_DEBUG1("GlxThumbnailVariantType::CreateThumbnailTextureL");
            TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateThumbnailTextureL(
                            aMedia, aMedia.IdSpaceId(), aSize, this ).Id() );
            }
        }
    else if ( aMedia.GetIconInfo(icon) )
	    {  
	    GLX_DEBUG1("GlxThumbnailVariantType::CreateIconTextureL");
        TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateIconTextureL( 
    	    icon.bitmapId, resFile, defaultSize ).Id() );
        }
    else if (KErrNone != thumbnailError && aMedia.Category() == EMPXVideo)
        {
        //show larger (twice) default icon for videos, which has errors
        TSize newSize = defaultSize;
        newSize += defaultSize;
        GLX_DEBUG1("GlxThumbnailVariantType::CreateIconTextureL::Default (video)");
        TRAP(err, mTextureId
                = iUiUtility->GlxTextureManager().CreateIconTextureL(
                        EMbmGlxiconsQgn_prop_image_notcreated, resFile,
                        newSize).Id());
        }
    else if( drm && isValid == EGlxDrmRightsInvalid )
    	{
		//show default image for DRM expired images
    	GLX_LOG_INFO( "GlxThumbnailVariantType::CreateIconTextureL"
    			"::EGlxDrmRightsInvalid" );
    	TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateIconTextureL( 
    			EMbmGlxiconsQgn_prop_image_notcreated, resFile, defaultSize ).Id() );
		}
	else if ( (KErrNone == thumbnailError) || (KErrArgument == thumbnailError) 
        || (KErrDiskFull == thumbnailError) || (KErrNoMemory == thumbnailError) 
		|| (KErrCANoRights == thumbnailError) )
	    {
        GLX_LOG_INFO1( "GlxThumbnailVariantType::thumbnailError=%d" , 
        		thumbnailError );
		//Try and see if we can scale and show the grid tnm else show the default
	    TBool isGridTnmShown = EFalse;		
	    if(HasRelevantThumbnail(aMedia,defaultSize))
	        {
	        TSize origSize;
	        if(aMedia.GetDimensions(origSize))
                {
				//Currently scaling the grid TNM's only if their original size > grid size.
				//Have to relook at this.
                if(origSize.iWidth > defaultSize.iWidth && origSize.iHeight > defaultSize.iHeight)
                      {
					  GLX_DEBUG1("GlxThumbnailVariantType::CreateThumbnailTextureL::ScaledTnm");
                      TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateThumbnailTextureL( 
                                aMedia, aMedia.IdSpaceId(), aSize, this, ETrue ).Id() );
					  if(err == KErrNone)
					      {
	                      isGridTnmShown = ETrue;
						  }
                      }
                }
	        }
	    if(!isGridTnmShown)
	        {	    
			GLX_DEBUG1("GlxThumbnailVariantType::CreateThumbnailTextureL::Default");
	        TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateIconTextureL( 
	                EMbmGlxiconsQgn_prop_image_notcreated, resFile, defaultSize ).Id() );
	        }
	    }
	else
		{
		GLX_DEBUG1("GlxThumbnailVariantType::CreateIconTextureL::Corrupt");
	    TRAP( err, mTextureId = iUiUtility->GlxTextureManager().CreateIconTextureL( 
	    	EMbmGlxiconsQgn_prop_image_corrupted, resFile, defaultSize ).Id() );
		}
    
     if ( KErrNone != err )
        {
		GLX_DEBUG2("GlxThumbnailVariantType::ConstructL() ERROR err=%d", err);
        throw std::bad_alloc();
        }
    }

// -----------------------------------------------------------------------------
// ConsumeRightsBasedOnSize - check if DRM rights should be consumed
// -----------------------------------------------------------------------------
//    
TBool GlxThumbnailVariantType::ConsumeRightsBasedOnSize(
                                       TSize aImageSize, TSize aBitmapSize)
    {
    TRACER("GlxThumbnailVariantType::ConsumeRightsBasedOnSize");
    
    TBool drmRightsChecked = EFalse;
    // minimum size (111 x 83)
    TInt minSize = KGlxThumbnailDrmWidth * KGlxThumbnailDrmHeight;
    // size of actual image
    TInt imgSize = aImageSize.iWidth * aImageSize.iHeight;
    
    // bitmap size
    TInt bmpSize = aBitmapSize.iWidth * aBitmapSize.iHeight;
    
    // size of HUI display
    TSize dispSize = iUiUtility->DisplaySize();
    
    
    if(dispSize.iWidth>0 && dispSize.iHeight>0)
        {
        TReal widthRatio = (TReal)aImageSize.iWidth/(TReal)dispSize.iWidth;
        TReal heightRatio = (TReal)aImageSize.iHeight/(TReal)dispSize.iHeight;
        
        if(widthRatio > heightRatio)
            {
            dispSize.iHeight = aImageSize.iHeight / widthRatio;
            }
        else
            {
            if(heightRatio>0)
                {
                dispSize.iWidth = aImageSize.iWidth / heightRatio;
                }
            }
        }     
        
    TInt visSize2 = dispSize.iWidth * dispSize.iHeight;
    
    // if thumbnail is smaller than visual use this for comparison
    if(bmpSize < visSize2)
        {
        visSize2 = bmpSize;
        }
    
    // is bmp smaller than 1/4 of image size    
    if(imgSize/4 < minSize)
        {
        minSize = imgSize/4;
        }   
    
    if(visSize2 >= minSize)
        {        
        drmRightsChecked = ETrue;
        }
    
    return drmRightsChecked;
    }

// -----------------------------------------------------------------------------
// ThumbnailSizeAndQuality - search for largest available thumbnail
// -----------------------------------------------------------------------------
// 
TSize GlxThumbnailVariantType::ThumbnailSize(const CGlxMedia* aMedia)
    {
    TRACER("GlxThumbnailVariantType::ThumbnailSize");
    TSize bmpSize(0,0);
    
    TArray<TMPXAttribute> attr = aMedia->Attributes();
    
    TInt selectedHeight = 0;
    TInt selectedWidth = 0;
    
    TInt count = attr.Count();
    for(TInt i=0; i<count; i++)
        {
        
        if ( KGlxMediaIdThumbnail == attr[i].ContentId()
            && GlxIsFullThumbnailAttribute(attr[i].AttributeId()) )
            {
            // Get selected Quality, height and width from attribute
            const CGlxThumbnailAttribute* thumbAtt =  
                    static_cast<const CGlxThumbnailAttribute*>
                         (aMedia->ValueCObject( attr[i]));
                         
            if(thumbAtt)
                {
                
                selectedHeight = thumbAtt->iDimensions.iHeight;
                selectedWidth = thumbAtt->iDimensions.iWidth;
                
                if((selectedHeight * selectedWidth) > 
                    (bmpSize.iHeight * bmpSize.iWidth))
                    {
                    bmpSize.iWidth = selectedWidth;
                    bmpSize.iHeight = selectedHeight;
                    }
                }
            }
        }
    return bmpSize;
    }
// ----------------------------------------------------------------------------
// destructor
// ----------------------------------------------------------------------------
//
GlxThumbnailVariantType::~GlxThumbnailVariantType() 
    {
    TRACER("GlxThumbnailVariantType::~GlxThumbnailVariantType");
    if(  iDrmUtility )
        {
        iDrmUtility->Close();
        }
    
    if ( iUiUtility)
		{
		iUiUtility->Close ();
		}
    }
    
    
void GlxThumbnailVariantType::TextureContentChangedL( TBool /*aHasContent*/ , CAlfTexture* /*aNewTexture*/)
    {
    
    }
// ----------------------------------------------------------------------------
// integer
// ----------------------------------------------------------------------------
//
int GlxThumbnailVariantType::integer() const
    {
    TRACER("GlxThumbnailVariantType::integer");
    return mTextureId;
    }

// ----------------------------------------------------------------------------
// HasRelevantThumbnail
// ----------------------------------------------------------------------------
//
TBool GlxThumbnailVariantType::HasRelevantThumbnail(const TGlxMedia& aMedia, 
                                                    const TSize& aSize)
    {
    TRACER("GlxThumbnailVariantType::HasRelevantThumbnail()");
    
    TMPXAttribute tnAttribQuality(KGlxMediaIdThumbnail, 
                                     GlxFullThumbnailAttributeId( ETrue, 
                                     aSize.iWidth, aSize.iHeight ) );                                           

    TMPXAttribute tnAttribSpeed(KGlxMediaIdThumbnail, 
                                     GlxFullThumbnailAttributeId( EFalse, 
                                     aSize.iWidth, aSize.iHeight ) );
    
    const CGlxThumbnailAttribute* qualityTn = aMedia.ThumbnailAttribute(
            tnAttribQuality );
    const CGlxThumbnailAttribute* speedTn = aMedia.ThumbnailAttribute(
            tnAttribSpeed );
    if ( qualityTn || speedTn )
        {
        GLX_LOG_INFO("GlxThumbnailVariantType::HasRelevantThumbnail() - TN avail");
        return ETrue;
        }
    return EFalse;
    }
	
// -----------------------------------------------------------------------------
// CreateImageViewerInstanceL
// -----------------------------------------------------------------------------
//
void GlxThumbnailVariantType::CreateImageViewerInstanceL()
    {
    TRACER("GlxThumbnailVariantType::CreateImageViewerInstanceL");
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();    
    __ASSERT_ALWAYS(iImageViewerInstance, Panic(EGlxPanicNullPointer));
    }

// -----------------------------------------------------------------------------
// DeleteImageViewerInstance
// -----------------------------------------------------------------------------
//
void GlxThumbnailVariantType::DeleteImageViewerInstance()
    {
    TRACER("GlxThumbnailVariantType::DeleteImageViewerInstance");
    if ( iImageViewerInstance )
        {
        iImageViewerInstance->DeleteInstance();
        }
    }

// ----------------------------------------------------------------------------
// type
// ----------------------------------------------------------------------------
//
Alf::IMulVariantType::TMulType GlxThumbnailVariantType::Type() const
    {
    TRACER("GlxThumbnailVariantType::type");
    return EInt;
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
GlxIconVariantType* GlxIconVariantType::NewL(TInt aIconId, TFileName aFileName,
                                                           const TSize& aSize)
	{
	TRACER("GlxIconVariantType::NewL");
	GlxIconVariantType* self = GlxIconVariantType::NewLC( aIconId, 
			aFileName, aSize );
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//
GlxIconVariantType* GlxIconVariantType::NewLC( TInt aIconId, TFileName aFileName, const TSize& aSize )
	{
	TRACER("GlxIconVariantType::NewLC");
	GlxIconVariantType* self = new ( EMM ) GlxIconVariantType( aIconId, 
			aFileName, aSize );
	CleanupStack::PushL( self );
	self->ConstructL( aIconId, aFileName, aSize );
	return self;
	
	}
	
// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//	
void GlxIconVariantType::ConstructL( TInt aIconId, TFileName aFileName, 
                                     const TSize& aSize )
	{
    TRACER("GlxIconVariantType::ConstructL");
    iUiUtility = CGlxUiUtility::UtilityL();
    
    TRAPD( err, mIconId = iUiUtility->GlxTextureManager().CreateIconTextureL( 
            aIconId, aFileName, aSize).Id() );
        
    if ( KErrNone != err )
        {
        throw std::bad_alloc();
        }
	}

// ----------------------------------------------------------------------------
// constructor
// ----------------------------------------------------------------------------
//
GlxIconVariantType::GlxIconVariantType( TInt /*aIconId*/, TFileName /*aFileName*/,
	const TSize& /*aSize*/ )
    {
    
    }
    
// ----------------------------------------------------------------------------
// destructor
// ----------------------------------------------------------------------------
//
GlxIconVariantType::~GlxIconVariantType() 
    {
   
    TRACER("GlxIconVariantType::~GlxIconVariantType");
	if ( iUiUtility)
		{
		iUiUtility->Close ();
		}
    }
    
// ----------------------------------------------------------------------------
// integer
// ----------------------------------------------------------------------------
//
int GlxIconVariantType::integer() const
    {
    TRACER("GlxIconVariantType::integer");
    return mIconId;
    }

// ----------------------------------------------------------------------------
// type
// ----------------------------------------------------------------------------
//
Alf::IMulVariantType::TMulType GlxIconVariantType::Type() const
    {
    TRACER("GlxIconVariantType::type");
    return EInt;
    }   
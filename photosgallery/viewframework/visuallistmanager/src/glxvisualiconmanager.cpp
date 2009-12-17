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
 * @internal reviewed 31/07/2007 by Rowland Cook
 */

#include "glxvisualiconmanager.h"

#include <data_caging_path_literals.hrh>
#include <alf/alftexture.h>
#include <alf/alfvisual.h>
#include <mpxmediageneraldefs.h>

#include <glxlog.h>
#include <glxtracer.h>

#include <glxanimationfactory.h>
#include <glxassert.h>
#include <glxerrormanager.h>
#include <glxerrors.h>
#include <glxuiutility.h>
#include <glxicons.mbg> // icons 
#include <glxthumbnailattributeinfo.h>
#include <glxuistd.h>
#include <mglxanimation.h>
#include <mglxmedialist.h>
#include <glxtexturemanager.h>
#include "mglxvisuallist.h"

#include "glxitemvisual.h"


/// How long the image-loading animation should last for (in milliseconds)
const TInt KGlxImageLoadingAnimationDuration = 400 * KGlxAnimationSlowDownFactor;


// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxVisualIconManager* CGlxVisualIconManager::NewL(
    MGlxMediaList& aMediaList, MGlxVisualList& aVisualList)
	{
	TRACER("CGlxVisualIconManager::NewL");
	GLX_LOG_INFO("CGlxVisualIconManager::NewL ");
	// get the ui utility
	CGlxUiUtility* utility = CGlxUiUtility::UtilityL();
	CleanupClosePushL( *utility );
	CGlxVisualIconManager* self = 
        new ( ELeave ) CGlxVisualIconManager(
            utility->GlxTextureManager(), aMediaList, aVisualList);
	CleanupStack::PopAndDestroy( utility ); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
EXPORT_C CGlxVisualIconManager::~CGlxVisualIconManager()
	{
	TRACER("CGlxVisualIconManager::~CGlxVisualIconManager");
	GLX_LOG_INFO("CGlxVisualIconManager::~CGlxVisualIconManager");
	// remove us from the medialist and visual list observer
	iMediaList.RemoveMediaListObserver(this);
	iVisualList.RemoveObserver( this );

	TInt animCount = iAnimations.Count();
	
	for ( TInt animIndex = 0; animIndex < animCount; animIndex++ )
	    {
	    delete iAnimations[animIndex];
	    }
	iAnimations.Reset();
	    
	iIconTextureArray.Reset();
	
	if( iWhiteTexture )
	    {
        iTextureManager.RemoveTexture( *iWhiteTexture );
	    }

	iThumbnailIcons.Close();
	}

// ---------------------------------------------------------------------------
// HandleVisualAddedL
// ---------------------------------------------------------------------------
//
void CGlxVisualIconManager::HandleVisualAddedL( CAlfVisual* aVisual, 
    TInt aIndex, MGlxVisualList* aList )
    {
    TRACER("CGlxVisualIconManager::HandleVisualAddedL");
    GLX_LOG_INFO("CGlxVisualIconManager::HandleVisualAddedL");
    if ( aIndex >= 0 && aIndex < iMediaList.Count() )
        {
        CheckThumbnailAttributesL( aIndex );
        }   
    }

// ---------------------------------------------------------------------------
// HandleError
// ---------------------------------------------------------------------------
//
void CGlxVisualIconManager::HandleError( TInt aError )
    {
    TRACER("CGlxVisualIconManager::HandleError");
    GLX_LOG_INFO1("CGlxVisualIconManager::HandleError: %d",aError);
    TRAP_IGNORE( DoHandleErrorL( aError ) );
    }
    
// ---------------------------------------------------------------------------
// DoHandleErrorL
// ---------------------------------------------------------------------------
//
void CGlxVisualIconManager::DoHandleErrorL( TInt /*aError*/ )
    {
    TRACER("CGlxVisualIconManager::DoHandleErrorL");
    GLX_LOG_INFO("CGlxVisualIconManager::DoHandleErrorL");
    // Check for items for which fetching a thumbnail has failed, and replace
    // the default icon with broken icon
	TInt count = iMediaList.Count();
	for ( TInt i = 0; i < count; i++ )
	    {
	    CGlxVisualObject* visItem = iVisualList.Item( i );

        // Only need to check that visual item exitst or not
	    if ( visItem )	   
            {
            const CAlfTexture* iconTexture = SelectIconTextureL( i );

            if ( iDefaultIconTexture != iconTexture )
                {
                visItem->RemoveIcon( *iDefaultIconTexture );

                visItem->SetStatus( CGlxVisualObject::EHasErrorIcon );
                visItem->AddIconL( *iconTexture,
                                NGlxIconMgrDefs::EGlxIconCentred,
                                ETrue, EFalse, 0, 1.0, 1.0 );
                visItem->SetScaleMode(CAlfImageVisual::EScaleNormal);                
                }
	        }
	    }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CGlxVisualIconManager::CGlxVisualIconManager(
        CGlxTextureManager& aTextureManager,
        MGlxMediaList& aMediaList, 
        MGlxVisualList& aVisualList )
	: CGlxIconManager( aMediaList, aVisualList ),
	  iTextureManager( aTextureManager )
	{
	TRACER("CGlxVisualIconManager::CGlxVisualIconManager");
	GLX_LOG_INFO("CGlxVisualIconManager::CGlxVisualIconManager");
	// No implementation
	}

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//	
void CGlxVisualIconManager::ConstructL()
	{
	TRACER("CGlxVisualIconManager::ConstructL");
	GLX_LOG_INFO("CGlxVisualIconManager::ConstructL");
	BaseConstructL();
	
    //iWhiteTexture = 
       // &( iTextureManager.CreateFlatColourTextureL( KRgbWhite ) );

	// create 'default' icon
    TFileName resFile(KDC_APP_BITMAP_DIR);
    resFile.Append(KGlxIconsFilename);
    // get the ui utility
    CGlxUiUtility* utility = CGlxUiUtility::UtilityL();
    CleanupClosePushL( *utility );
    TSize iconSize = utility->GetGridIconSize();
    CleanupStack::PopAndDestroy( utility );   
	iDefaultIconTexture =
	    &(iTextureManager.CreateIconTextureL( 
	        EMbmGlxiconsQgn_prop_image_notcreated, resFile, iconSize ) );

    iBrokenIconTexture =
        &(iTextureManager.CreateIconTextureL(
            EMbmGlxiconsQgn_prop_image_corrupted, resFile, iconSize ) );

    iEmptyIconTexture =
        &(iTextureManager.CreateIconTextureL(
            EMbmGlxiconsQgn_prop_image_notcreated, resFile, iconSize ) );

	// add as observers
	iMediaList.AddMediaListObserverL(this);	
	iVisualList.AddObserverL(this);
	
	
	// check for any visual already present in list
	TInt itemCount = iMediaList.Count();
	for ( TInt i = 0; i < itemCount; i++ )
	    {
        CheckThumbnailAttributesL( i );
	    }
	}
    
// ---------------------------------------------------------------------------
// CheckThumbnailAttributesL
// ---------------------------------------------------------------------------
//    
void CGlxVisualIconManager::CheckThumbnailAttributesL( TInt aIndex )
    {
    TRACER("CGlxVisualIconManager::CheckThumbnailAttributesL");
    GLX_LOG_INFO1("CGlxVisualIconManager::CheckThumbnailAttributesL: %d",aIndex);
    CGlxVisualObject* visItem = iVisualList.Item( aIndex );

    if ( visItem )
        {
        TBool thumbnail = visItem->SetObserver( *this );
		const TGlxMedia& item = iMediaList.Item( aIndex );       
        TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                                item.Properties(), KGlxMediaIdThumbnail );
  
        if ( thumbnail && thumbnailError == KErrNone )
            {            
            visItem->SetStatus( CGlxVisualObject::EHasThumbnail );
            }
        else
            {
            const CAlfTexture* iconTexture = iDefaultIconTexture;
			visItem->SetScaleMode(CAlfImageVisual::EScaleNormal);
            TIconInfo icon;
            if ( item.GetIconInfo( icon ) )
                {
                // The item has its own icon
                // In this case we treat the icon as a thumbnail
                // i.e. we want it to scale appropriate to its
                // position in the view
                visItem->SetStatus( CGlxVisualObject::EHasOtherIcon );

                GLX_ASSERT_DEBUG( iThumbnailIcons.Count() == iIconTextureArray.Count(),
                    Panic( EGlxPanicIllegalState ), "Icon array mismatch" );

                // Check if icon texture already generated
                TInt pos = iThumbnailIcons.Find( icon, MatchIcon );

                if ( pos == KErrNotFound )
                    {
                    // Ask texture manager to generate icon
                    /// @todo Should specify a size here
                    iconTexture = 
                        &iUiUtility->GlxTextureManager().CreateIconTextureL( 
                            icon.bitmapId, icon.bmpfile );

                    iThumbnailIcons.ReserveL( iThumbnailIcons.Count() + 1 );
                    iIconTextureArray.AppendL( iconTexture );
                    // Can't fail because of reservation
                    iThumbnailIcons.AppendL( icon );
                    }
                else
                    {
                    iconTexture = iIconTextureArray[pos];
                    }
                // add to visual
				//@Migration info: Required?
                //visItem->SetImage( THuiImage( *iconTexture ) );
                }
            else
                {
                iconTexture = SelectIconTextureL( aIndex );
                if ( iDefaultIconTexture == iconTexture )
                    {
                    visItem->SetStatus( CGlxVisualObject::EHasDefaultIcon );
                    }
                else
                    {
                    visItem->SetStatus( CGlxVisualObject::EHasErrorIcon );
                    }
                // add to visual
                visItem->AddIconL( *iconTexture, NGlxIconMgrDefs::EGlxIconCentred,
                                    ETrue, EFalse, 0, 1.0, 1.0 );
               // }
            }
        }
    }
    }


// ---------------------------------------------------------------------------
// AnimateImageLoaded
// ---------------------------------------------------------------------------
//    
void CGlxVisualIconManager::AnimateImageLoadedL( CAlfVisual* aItemVisual)
    {
    TRACER("CGlxVisualIconManager::AnimateImageLoadedL");
    GLX_LOG_INFO("CGlxVisualIconManager::AnimateImageLoadedL");
    MGlxAnimation* anim = 
        GlxAnimationFactory::CreateImageLoadingAnimationL( *aItemVisual, *iWhiteTexture );
           
    CleanupStack::PushL(anim);
    iAnimations.InsertInAddressOrderL(anim);
    CleanupStack::Pop(anim);
    
    anim->AnimateL( KGlxImageLoadingAnimationDuration, this );
   }

// ---------------------------------------------------------------------------
// AnimationComplete
// ---------------------------------------------------------------------------
//    
void CGlxVisualIconManager::AnimationComplete(MGlxAnimation* aAnimation)
    {
    TRACER("CGlxVisualIconManager::AnimationComplete");
    GLX_LOG_INFO("CGlxVisualIconManager::AnimationComplete");
    TInt animIndex = iAnimations.FindInAddressOrder(aAnimation);
    
    if ( animIndex != KErrNotFound )
        {
        iAnimations.Remove(animIndex);
        }
    
    delete aAnimation;
    }

// ---------------------------------------------------------------------------
// IsDrmVideoItem to be done
// ---------------------------------------------------------------------------
// 
TBool CGlxVisualIconManager::IsDrmVideoItem( const TGlxMedia& aItem )
    {
    TRACER("CGlxVisualIconManager::IsDrmVideoItem");
    GLX_LOG_INFO("CGlxVisualIconManager::IsDrmVideoItem");
    if ( aItem.IsDrmProtected() && aItem.Category() == EMPXVideo )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// VisualStatusChangedL
// ---------------------------------------------------------------------------
//    
void CGlxVisualIconManager::VisualStatusChangedL( CGlxVisualObject& aObject,
                                        TInt aListIndex, TBool aHasThumbnail )
    {
    TRACER("CGlxVisualIconManager::VisualStatusChangedL");
    GLX_LOG_INFO("CGlxVisualIconManager::VisualStatusChangedL");
    // Note: The visual may not yet be in the visual list

    if ( aHasThumbnail )
        {
        // Remove icons
        if ( CGlxVisualObject::EHasDefaultIcon == aObject.Status() )
            {
            aObject.RemoveIcon( *iDefaultIconTexture );
            }
        else if ( CGlxVisualObject::EHasErrorIcon == aObject.Status() )
            {
            aObject.RemoveIcon( *iBrokenIconTexture );
            aObject.RemoveIcon( *iEmptyIconTexture );
            }

        aObject.SetStatus( CGlxVisualObject::EHasThumbnail );
        }
    else
        {
        if ( CGlxVisualObject::EHasThumbnail == aObject.Status() )
            {
            const CAlfTexture* iconTexture = SelectIconTextureL( aListIndex );
            if ( iDefaultIconTexture == iconTexture )
                {
                aObject.SetStatus( CGlxVisualObject::EHasDefaultIcon );
                }
            else
                {
                aObject.SetStatus( CGlxVisualObject::EHasErrorIcon );
                }

            aObject.AddIconL( *iconTexture, NGlxIconMgrDefs::EGlxIconCentred,
                                ETrue, EFalse, 0, 1.0, 1.0 );
            aObject.SetScaleMode(CAlfImageVisual::EScaleNormal);                    
            }
        }
    }

// ---------------------------------------------------------------------------
// ThumbnailLoadedL
// ---------------------------------------------------------------------------
//    
void CGlxVisualIconManager::ThumbnailLoadedL( CGlxVisualObject& aObject )
    {
    TRACER("CGlxVisualIconManager::ThumbnailLoadedL");
    GLX_LOG_INFO("CGlxVisualIconManager::ThumbnailLoadedL");
    AnimateImageLoadedL( aObject.Visual() );
    }

// ---------------------------------------------------------------------------
// SelectIconTextureL
// ---------------------------------------------------------------------------
//    
const CAlfTexture* CGlxVisualIconManager::SelectIconTextureL( TInt aIndex )
    {
    TRACER(" CGlxVisualIconManager::SelectIconTextureL");
    GLX_LOG_INFO(" CGlxVisualIconManager::SelectIconTextureL");
    const CAlfTexture* iconTexture = iDefaultIconTexture;

    const TGlxMedia& item = iMediaList.Item( aIndex );
    TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                                item.Properties(), KGlxMediaIdThumbnail );

  if ( KErrGlxEmptyContainer == thumbnailError )
        {
        iconTexture = iEmptyIconTexture;
        }
    else if ( KErrNone == thumbnailError ||
             // If the error is not supported display the default icon, not the corrupted icon. e.g. real media
             thumbnailError == KErrNotSupported || 

             IsDrmVideoItem( item ) )
        {
        // Use default icon
        // Test whether the item is a DRM'd video to fix error EDKZ-77UJZA
        // as DRM'd videos should show the default icon not the broken icon
        }
    else
        {
        iconTexture = iBrokenIconTexture;
        }

    return iconTexture;
    }

// ---------------------------------------------------------------------------
// MatchIcon
// ---------------------------------------------------------------------------
//    
TBool CGlxVisualIconManager::MatchIcon( const TIconInfo& aIcon1,
                                const TIconInfo& aIcon2 )
    {
    TRACER("CGlxVisualIconManager::MatchIcon");
    GLX_LOG_INFO("CGlxVisualIconManager::MatchIcon ");
    return ( aIcon1.bmpfile == aIcon2.bmpfile &&
            aIcon1.bitmapId == aIcon2.bitmapId );
    }

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
* Description:    Item visual container
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#include "glxitemvisual.h"

#include <alf/alfanchorlayout.h>
#include <alf/alfcontrol.h>
#include <alf/alfdecklayout.h>
#include <AknUtils.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimagebrush.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <alf/alftexture.h>
#include <mpxmediageneraldefs.h>
#include <imageconversion.h>

#include <glxlog.h>
#include <glxtracer.h>

#include <glxassert.h>
#include <glxuiutility.h>
#include <glxmedia.h>
#include <glxmediageneraldefs.h>
#include <glxtexturemanager.h>
#include <glxthumbnailattributeinfo.h>
#include <mglxmedialist.h>
#include <glxthumbnailutility.h>
#include <alf/alftransformation.h>
#include "mglxvisualobjectstatusobserver.h"
#include "mglxvisualobjectlayoutrefreshobserver.h"

const TInt KGlxOpacityFadeDuration = 400;

const TInt KGlxForegroundAnchorOrdinal = 2;

// -----------------------------------------------------------------------------
// Constructor. Inlined to save a few bits of rom
// -----------------------------------------------------------------------------
inline CGlxVisualObject::CGlxVisualObject( MGlxMediaList& aMediaList)
    : iMediaList( aMediaList )
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
CGlxVisualObject* CGlxVisualObject::NewLC( 
            MGlxVisualObjectParameterFactory& aFactory,MGlxMediaList& aMediaList)
    {
    TRACER("CGlxVisualObject::NewLC");
    CGlxVisualObject* self = new ( ELeave ) CGlxVisualObject( aMediaList);
    CleanupStack::PushL( self );
    self->ConstructL( aFactory );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CGlxVisualObject::~CGlxVisualObject()
    {
    TRACER("CGlxVisualObject::~CGlxVisualObject");
    iForegroundIconAnchors.Close();
    iBackgroundIconAnchors.Close();
    
    while ( iZoomTiles.Count() )
        {
        RemoveZoomTile(0);
        }
    iZoomTiles.Close();

	if ( iUiUtility && iImageValid )
        {
        // Clean up the texture from the texture manager
        iUiUtility->GlxTextureManager().RemoveTexture( iImage.Texture() );
        }

    if ( iVisual )
        {
        iVisual->RemoveAndDestroyAllD();
        }
        delete iAnimatedTexture;
    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CGlxVisualObject::ConstructL( MGlxVisualObjectParameterFactory& aFactory) 
    {
    TRACER("CGlxVisualObject::ConstructL");
    iUiUtility = CGlxUiUtility::UtilityL();
    // create the visual with parents layout
    iVisual = CAlfAnchorLayout::AddNewL(aFactory.VisualOwner(),
                                        aFactory.Layout());
    iVisual->EnableTransformationL();
    
    // create background underlay anchor layout 
    //to act as a black screen and avoid overlapping images                                      
    iBackgroundOverlayVisual = 
        CAlfImageVisual::AddNewL( aFactory.VisualOwner(), iVisual); 
    CAlfTexture& backgroundTexture = 
    	iUiUtility->GlxTextureManager().CreateFlatColourTextureL(KRgbBlack);
    iBackgroundOverlayVisual->SetClipping(ETrue);	
    iBackgroundOverlayVisual->SetImage(TAlfImage(backgroundTexture));	
    // create main visual
    iMainVisual = 
        CAlfAnchorLayout::AddNewL( aFactory.VisualOwner(), iVisual );    
                                        
    // create the child image visual
    iImageVisual = 
        CAlfImageVisual::AddNewL( aFactory.VisualOwner(), iMainVisual );
    //iImageVisual->SetScaleMode( aFactory.ThumbnailScaleMode() );
	iImageVisual->SetScaleMode(CAlfImageVisual::EScaleNormal);
    // Turn on clipping only if cover-mode has been chosen
    iImageVisual->SetClipping(ETrue);

	User::LeaveIfError( iMainVisual->SetRelativeAnchorRect(
  	0, EAlfAnchorOriginLeft, EAlfAnchorOriginTop, TAlfRealPoint(),
	 EAlfAnchorOriginRight, EAlfAnchorOriginBottom, TAlfRealPoint() ) );

	// get screen size
	SetVisible(EFalse);
	TSize screenSize = iUiUtility->DisplaySize();
    RefreshLayout( screenSize );	
    }
        
// -----------------------------------------------------------------------------
// SetScaleMode
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxVisualObject::SetScaleMode(CAlfImageVisual::TScaleMode aMode)
    {
    TRACER("CGlxVisualObject::SetScaleMode");
    iImageVisual->SetScaleMode(aMode);
    }
    
// -----------------------------------------------------------------------------
// Reset state of the object as if it had just been constructed
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::Reset() 
    {
    TRACER("CGlxVisualObject::Reset");
    SetVisible(EFalse);
    StopAnimation();
    iAnimateWhenAttributesAvailable = EFalse;
    iUiUtility->GlxTextureManager().RemoveTexture(iImage.Texture());
    }

// -----------------------------------------------------------------------------
// Remove and delete all child visuals of the layout
// @param aLayout layout to reset
// -----------------------------------------------------------------------------
//	
void CGlxVisualObject::ResetLayout( CAlfVisual& /*aVisual*/ )
    {
    //to be used when layouts are back
    // until no visuals left
    }
    
// -----------------------------------------------------------------------------
// Visual
// -----------------------------------------------------------------------------
CAlfVisual* CGlxVisualObject::Visual()
	{
	TRACER("CGlxVisualObject::Visual");
	return iVisual;
	}
    
// -----------------------------------------------------------------------------
// SetVisible
// -----------------------------------------------------------------------------
void CGlxVisualObject::SetVisible( TBool aVisible )
    {
    TRACER("CGlxVisualObject::SetVisible");
    if(aVisible)
	    {
	    iVisual->SetOpacity(  TAlfTimedValue(1.0 , 0 ));	
	    }
    else
	    {
	    iVisual->SetOpacity( TAlfTimedValue(0.0, 0 ));	
	    }
    
    }

// -----------------------------------------------------------------------------
// SetIndex
// -----------------------------------------------------------------------------
void CGlxVisualObject::SetIndex( TInt aIndex )
	{
	TRACER("CGlxVisualObject::SetIndex");
	GLX_LOG_INFO1("CGlxVisualObject::SetIndex %d",aIndex);
	iIndex = aIndex;
	}

// -----------------------------------------------------------------------------
// SetImage
// -----------------------------------------------------------------------------
void CGlxVisualObject::SetImage( TAlfImage aImage )
	{
	TRACER("CGlxVisualObject::SetImage");
	iImageVisual->SetImage( aImage );
	iImage = aImage;
	iImageValid = ETrue;
	}

// -----------------------------------------------------------------------------
// Image
// -----------------------------------------------------------------------------
TAlfImage CGlxVisualObject::Image()
	{
	TRACER("CGlxVisualObject::Image");
	return iImage;
	}

// -----------------------------------------------------------------------------
// RefreshLayout
// the visual list has requested us to refresh the layout
// -----------------------------------------------------------------------------
void CGlxVisualObject::RefreshLayout( TSize /*aScreenSize*/ )
	{
	TRACER("CGlxVisualObject::RefreshLayout");
    // get texture manager
    CGlxTextureManager& textureMgr = iUiUtility->GlxTextureManager();
    // get new size, note that this is synchronous c/s call
	TSize visSize = Visual()->Size().ValueNow().AsSize();

	// check if visual size has changed since last time
	if( visSize != iVisSize )
	    {
	    // store size
	    iVisSize = visSize;
	    
	    // get the media id and id space id for this index
	    TGlxMedia item = iMediaList.Item( iIndex );
	    TGlxIdSpaceId idspace = iMediaList.IdSpaceId( iIndex );

        // check if there was a better thumbnail available
        if( textureMgr.TextureNeedsUpdating( item, idspace, visSize ) )
            {
            // need to update the texture
            TRAP_IGNORE(CAlfTexture& texture = 
                textureMgr.CreateThumbnailTextureL( 
                    item, idspace, visSize, this );
            // set the new texture in place
        	SetImage( TAlfImage( texture ) ));
            }
        
        // update positions of overlay icons            
        UpdateIconAnchors( Image().Texture(), visSize );             
	    }
	}

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::HandleAttributesAvailableL(
    const TGlxIdSpaceId& aIdSpaceId, const TGlxMedia& aItem,
    const RArray<TMPXAttribute>& aAttributes)
    {
    TRACER("CGlxVisualObject::HandleAttributesAvailableL");
    TBool animate = ( EHasDefaultIcon == Status() );

    // get texture manager
    CGlxTextureManager& textureMgr = iUiUtility->GlxTextureManager();
    // get new size, note that this is synchronous c/s call          
    TSize visSize = iUiUtility->DisplaySize();

    // check if there was a better thumbnail available
    if( textureMgr.TextureNeedsUpdating( aItem, aIdSpaceId, visSize ) )
        {
        // need to update the texture
        CAlfTexture& texture = 
            textureMgr.CreateThumbnailTextureL( 
                aItem, aIdSpaceId, visSize, this );
        // set the new texture in place
    	SetImage( TAlfImage( texture ) );

        // update positions of overlay icons 
        UpdateIconAnchors( Image().Texture(), visSize );             

        if ( animate && iObserver )
            {
            iObserver->ThumbnailLoadedL( *this );
            }
        }

    if ( iAnimateWhenAttributesAvailable )
        {
        TryAnimateL(aItem);
        }

    }

// -----------------------------------------------------------------------------
// StartAnimation
// -----------------------------------------------------------------------------
void CGlxVisualObject::StartAnimation( CAlfTexture& aAnimatedTexture )
    {
    TRACER("CGlxVisualObject::StartAnimation");
    // if we have an old texture
    if ( iAnimatedTexture )
        {
        // release the old texture
        iUiUtility->GlxTextureManager().RemoveTexture( *iAnimatedTexture );
        iAnimatedTexture = NULL;
        }
    // store the texture pointer so that we can stop it without a 
    // reference to the texture
    iAnimatedTexture = &aAnimatedTexture;

    // start the animation
    aAnimatedTexture.StartAnimation();
    // set the image to the visual
    iImageVisual->SetImage( TAlfImage( aAnimatedTexture ) );
    }
    
// -----------------------------------------------------------------------------
// StopAnimation
// -----------------------------------------------------------------------------
void CGlxVisualObject::StopAnimation()
    {
    TRACER("CGlxVisualObject::StopAnimation");
    if ( iAnimatedTexture )
        {
        iImageVisual->SetImage( iImage );

        // stop the animation
        iAnimatedTexture->StopAnimation();
        
        // release the texture
        iUiUtility->GlxTextureManager().RemoveTexture( *iAnimatedTexture );
        iAnimatedTexture = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// SetAnimateWhenAttributesAvailable
// -----------------------------------------------------------------------------
void CGlxVisualObject::SetAnimateWhenAttributesAvailable(TBool aFlag)
    {
    TRACER("CGlxVisualObject::SetAnimateWhenAttributesAvailable");
    iAnimateWhenAttributesAvailable = aFlag;
    }

// -----------------------------------------------------------------------------
// TryAnimateL
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::TryAnimateL(const TGlxMedia& aItem)
    {
    TRACER("CGlxVisualObject::TryAnimateL");
    TInt frameCount = 0;
    TSize mediaItemSize( KErrNotFound, KErrNotFound );
    aItem.GetDimensions( mediaItemSize );
    if ( aItem.GetFrameCount( frameCount ) )
        {
        const TDesC& uri = aItem.Uri();
        if (frameCount > 1 && uri.Length() > 0)
            {
            // create the animated texture from the URI
    		CAlfTexture& animTexture =
    		   iUiUtility->GlxTextureManager().CreateAnimatedGifTextureL( uri, mediaItemSize );
            StartAnimation( animTexture );
    		}
        SetAnimateWhenAttributesAvailable(EFalse);
		}
    else
        {
        SetAnimateWhenAttributesAvailable(ETrue);
        }
    }

// -----------------------------------------------------------------------------
// RemoveOldZoomTiles
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::RemoveOldZoomTiles(const TGlxMedia& aItem)
    {
    TRACER("CGlxVisualObject::RemoveOldZoomTiles");
    const CGlxMedia* properties = aItem.Properties();
    TInt firstPosition = iZoomTiles.Count();

    for ( TInt i = iZoomTiles.Count() - 1; i >= 0; i-- )
        {
        if ( !(properties && properties->IsSupported(TMPXAttribute(
                        KGlxMediaIdThumbnail, iZoomTiles[i].iAttributeId))) )
            {
            RemoveZoomTile(i);
            firstPosition = i;
            }
        }

    ReanchorZoomTiles( firstPosition );
    }

// -----------------------------------------------------------------------------
// AddNewZoomTilesL
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::AddNewZoomTilesL(TGlxIdSpaceId aIdSpaceId,
            const TGlxMedia& aItem, const RArray<TMPXAttribute>& aAttributes)
    {
    TRACER("CGlxVisualObject::AddNewZoomTilesL");
    // Get dimension if we don't already have it
    if ( (iDimensions.iWidth == 0 || iDimensions.iHeight == 0) )
        {
        // Ignore success/failure return, test dimension instead
        aItem.GetDimensions(iDimensions);
        }

    if ( iDimensions.iWidth == 0 || iDimensions.iHeight == 0 )
        {
        return;
        }

    TInt firstPosition = iZoomTiles.Count();

    // Check new attributes for cropped thumbnails
    TInt count = aAttributes.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TMPXAttribute attr(aAttributes[i]);

        if ( KGlxMediaIdThumbnail == attr.ContentId()
            && !GlxIsFullThumbnailAttribute( attr.AttributeId() ) )
            {
            const CGlxThumbnailAttribute* thumbnail = 
                                          aItem.ThumbnailAttribute( attr );
            if ( thumbnail )
                {
                TInt position = AddZoomTileL( aItem, attr, aIdSpaceId,
                                                thumbnail->iCroppingRect );

                if ( position < firstPosition )
                    {
                    firstPosition = position;
                    }
                }
            }
        }

    ReanchorZoomTiles( firstPosition );
    }

// -----------------------------------------------------------------------------
// RemoveZoomTile
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::RemoveZoomTile(TInt aIndex)
    {
    TRACER("CGlxVisualObject::RemoveZoomTile");
    __ASSERT_ALWAYS(0 <= aIndex && aIndex < iZoomTiles.Count(),
                                            Panic(EGlxPanicIllegalArgument));

    CAlfImageVisual* visual = iZoomTiles[aIndex].iVisual;
	const TAlfImage& image = visual->Image();
	if( image.HasTexture() )
	    {
        iUiUtility->GlxTextureManager().RemoveTexture( image.Texture() );
	    }
	iVisual->Owner().Remove(visual);   // Caller gets ownership of the visual
    delete visual;
    iZoomTiles.Remove(aIndex);
    }

// ---------------------------------------------------------------------------
// AddZoomTileL
// ---------------------------------------------------------------------------
//
TInt CGlxVisualObject::AddZoomTileL(
                    const TGlxMedia& aItem, const TMPXAttribute& aAttribute,
                    TGlxIdSpaceId aIdSpaceId, const TRect& aCroppingRect )
    {
    TRACER("CGlxVisualObject::AddZoomTileL");
    // Reserve space for new tile
    iZoomTiles.ReserveL( iZoomTiles.Count() + 1 );

    // Extract zoom level field from attribute ID
    TInt zoomLevel = GlxZoomedThumbnailZoomLevel( aAttribute.AttributeId() );
    TInt position = 0;
    // Lowest zoom level goes to highest position (front)
    while ( position < iZoomTiles.Count()
            && zoomLevel < iZoomTiles[position].iZoomLevel )
        {
        position++;
        }

    CAlfImageVisual* visual = CAlfImageVisual::AddNewL(
                            static_cast<CAlfControl&>( iMainVisual->Owner() ),
                            iMainVisual );
    CleanupStack::PushL(visual);

    // Set temporary anchor to reserve space in anchors array
    TInt ordinal = iMainVisual->ChildOrdinal( iMainVisual->Count() - 1 );
    User::LeaveIfError( iMainVisual->SetRelativeAnchorRect( ordinal,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop, TAlfRealPoint(),
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop, TAlfRealPoint() ) );

    // Move new visual to correct position
    iMainVisual->Reorder(*visual, position + 1);   // Full image thumbnail is position 0

    TReal32 imageWidth = iDimensions.iWidth;
    TReal32 imageHeight = iDimensions.iHeight;

    TZoomTile tile;
    tile.iVisual = visual;
    tile.iAttributeId = aAttribute.AttributeId();
    tile.iZoomLevel = zoomLevel;
    tile.iTopLeft.iX = aCroppingRect.iTl.iX / imageWidth;
    tile.iTopLeft.iY = aCroppingRect.iTl.iY / imageHeight;
    tile.iBottomRight.iX = aCroppingRect.iBr.iX / imageWidth;
    tile.iBottomRight.iY = aCroppingRect.iBr.iY / imageHeight;
    // Shouldn't leave due to reservation above
    iZoomTiles.InsertL(tile, position);
    CleanupStack::Pop(visual);

    return position;
    }

// ---------------------------------------------------------------------------
// ReanchorZoomTiles
// ---------------------------------------------------------------------------
//    
void CGlxVisualObject::ReanchorZoomTiles( TInt aFirstPosition )
    {
    TRACER("CGlxVisualObject::ReanchorZoomTiles");
    TInt count = iZoomTiles.Count();
    for ( TInt i = aFirstPosition; i < count; i++ )
        {
        TZoomTile& tile( iZoomTiles[i] );
        TInt ordinal = iMainVisual->ChildOrdinal( i + 1 );

        TInt error = iMainVisual->SetRelativeAnchorRect( ordinal,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop, tile.iTopLeft,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop, tile.iBottomRight );
        if ( KErrNone != error )
            {
            // Should never fail since space was reserved in the anchor array
            GLX_LOG_WARNING1( "Failed to update anchor, error %d", error );
            __ASSERT_DEBUG( EFalse, Panic( EGlxPanicUnhandledError ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// AddIconL
// -----------------------------------------------------------------------------
//    
void CGlxVisualObject::AddIconL( const CAlfTexture& aTexture, 
                        NGlxIconMgrDefs::TGlxIconPosition /*aIconPos*/, 
                        TBool /*aForeground*/, TBool /*aStretch*/, TInt /*aBorderMargin*/,
                                                TReal32 /*aWidth*/, TReal32 /*aHeight*/ )
    {
    TRACER("CGlxVisualObject::AddIconL");
    //currently foreground and background visuals are not being used
    //the implications needs to be studied
    //background visual is used mainly to provide a background for small visual objects
    //foreground visual is used to supply specialised icons for clips
    
    
    // Set image for corrupted thumbnail
      SetImage(TAlfImage( aTexture ));
   
    }

// -----------------------------------------------------------------------------
// RemoveIcon
// -----------------------------------------------------------------------------
//    
TBool CGlxVisualObject::RemoveIcon(  const CAlfTexture& aTexture )
    {
    TRACER("CGlxVisualObject::RemoveIcon");
    
   TInt visPos = FindExistingTexture( aTexture, ETrue );
    if( visPos != KErrNotFound )
        {       
        DoRemoveIcon( *iMainVisual, 
                            iForegroundIconAnchors, visPos );
        }
    else 
        {
        visPos = FindExistingTexture( aTexture, EFalse );        
        }
    
    return ( visPos != KErrNotFound );
    
    }

// -----------------------------------------------------------------------------
// SetIconVisibility
// -----------------------------------------------------------------------------
//        
void CGlxVisualObject::SetIconVisibility( const CAlfTexture& aTexture, TBool aVisible )
    {
    TRACER("CGlxVisualObject::SetIconVisibility");
    // find texture (assume it's a foreground visual)
    TInt pos = FindExistingTexture( aTexture, ETrue );
    if( pos != KErrNotFound )
        {
        CAlfVisual& vis = iMainVisual->Visual( pos );
        
        if( aVisible )
            {
            vis.SetOpacity( TAlfTimedValue( 1.0, KGlxOpacityFadeDuration ) );
            }
        else
            {
            vis.SetOpacity( TAlfTimedValue( 0.0, KGlxOpacityFadeDuration ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// Status
// -----------------------------------------------------------------------------
//
CGlxVisualObject::TVisualStatus CGlxVisualObject::Status() const
    {
    TRACER("CGlxVisualObject::Status()");
    return iStatus;
    }

// -----------------------------------------------------------------------------
// SetStatus
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::SetStatus( TVisualStatus aStatus )
    {
    TRACER("CGlxVisualObject::SetStatus");
    iStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// SetObserver
// -----------------------------------------------------------------------------
//
TBool CGlxVisualObject::SetObserver( MGlxVisualObjectStatusObserver& aObserver )
    {
    TRACER("CGlxVisualObject::SetObserver");
    iObserver = &aObserver;
    return iImage.HasTexture();
    }

// -----------------------------------------------------------------------------
// TextureContentChangedL
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::TextureContentChangedL( TBool aHasContent, CAlfTexture* /*aNewTexture */)
    {
    TRACER("CGlxVisualObject::TextureContentChangedL");
    if ( iObserver )
        {
        iObserver->VisualStatusChangedL( *this, iIndex, aHasContent );
        }
    }

// -----------------------------------------------------------------------------
// SetAnchors
// -----------------------------------------------------------------------------
//
void CGlxVisualObject::SetAnchors( TGlxIconAnchors& aAnchors, 
                        CAlfAnchorLayout& aLayout, TInt aPos, TBool aUseOffsets )
    {
    TRACER("CGlxVisualObject::SetAnchors");
    TAlfTimedPoint leftPoint = aAnchors.iTopLeftAnchor.iPosition;
    TAlfTimedPoint rightPoint = aAnchors.iBotRightAnchor.iPosition;
     
    if( aUseOffsets && 
        aAnchors.iTopLeftAnchor.iAnchorMetric == EAlfAnchorMetricRelativeToSize )
        {
        // update horizontal positions if there is a horizontal offset  
        if( aAnchors.iTopLeftAnchor.iAnchorHorizOrigin == EAlfAnchorOriginLeft )
            {
            leftPoint.iX.SetValueNow( leftPoint.iX.Target() + iHoriIconOffset );
            
            if( rightPoint.iX.Target() == 1.0)
                {
                rightPoint.iX.SetValueNow( rightPoint.iX.Target() - iHoriIconOffset );
                }
            
            }
        else if ( aAnchors.iTopLeftAnchor.iAnchorHorizOrigin == EAlfAnchorOriginRight )
            {
            if( leftPoint.iX.Target() == -1.0)
                {
                leftPoint.iX.SetValueNow( leftPoint.iX.Target() + iHoriIconOffset );
                }
                 
            rightPoint.iX.SetValueNow( rightPoint.iX.Target() - iHoriIconOffset );
            }
        else
            {
            
            }
        
        // update vertical positions if there is a vertical offset    
        if( aAnchors.iTopLeftAnchor.iAnchorVertiOrigin == EAlfAnchorOriginTop )
            {
            leftPoint.iY.SetValueNow( leftPoint.iY.Target() + iVertIconOffset );
            if( rightPoint.iY.Target() == 1.0)
                {
                rightPoint.iY.SetValueNow( rightPoint.iY.Target() - iVertIconOffset );
                }
            }
        else if ( aAnchors.iTopLeftAnchor.iAnchorHorizOrigin == EAlfAnchorOriginBottom )
            {
            if( leftPoint.iY.Target() == -1.0)
                {
                leftPoint.iY.SetValueNow( leftPoint.iY.Target() + iVertIconOffset );
                }
            rightPoint.iY.SetValueNow( rightPoint.iY.Target() - iVertIconOffset );
            }
        else
            {
            
            }               
        }
   
    
    aLayout.SetAnchor( EAlfAnchorTopLeft, aPos,
                    aAnchors.iTopLeftAnchor.iAnchorHorizOrigin, 
                    aAnchors.iTopLeftAnchor.iAnchorVertiOrigin,
                    aAnchors.iTopLeftAnchor.iAnchorMetric,
                    aAnchors.iTopLeftAnchor.iAnchorMetric,
                    leftPoint );
                    
    aLayout.SetAnchor( EAlfAnchorBottomRight, aPos,
                    aAnchors.iBotRightAnchor.iAnchorHorizOrigin, 
                    aAnchors.iBotRightAnchor.iAnchorVertiOrigin,
                    aAnchors.iBotRightAnchor.iAnchorMetric,
                    aAnchors.iBotRightAnchor.iAnchorMetric,
                    rightPoint );
                    
    aLayout.UpdateChildrenLayout();
    }

// -----------------------------------------------------------------------------
// GenerateAnchors
// -----------------------------------------------------------------------------
//    
void CGlxVisualObject::GenerateAnchors( TGlxIconAnchors& aAnchors,
                                        const CAlfTexture& aTexture, 
                                    NGlxIconMgrDefs::TGlxIconPosition aIconPos, 
                                            TInt aBorderMargin, TBool aStretch, 
                                            TReal32 aWidth, TReal32 aHeight )
    {
    TRACER("CGlxVisualObject::GenerateAnchors");
    // top-left anchor
    TGlxAnchorDetails topLeftAnchor;
    TGlxAnchorDetails bottomRightAnchor;
    
    if( aStretch)
        {
        topLeftAnchor.iAnchorMetric = EAlfAnchorMetricRelativeToSize;
        bottomRightAnchor.iAnchorMetric = EAlfAnchorMetricRelativeToSize;
        }
     else
        {
        topLeftAnchor.iAnchorMetric = EAlfAnchorMetricAbsolute;
        bottomRightAnchor.iAnchorMetric = EAlfAnchorMetricAbsolute;
        }
        
    TInt textureWidth = aTexture.Size().iWidth + aBorderMargin;
    TInt textureHeight = aTexture.Size().iHeight + aBorderMargin;
   
    // 
    switch( aIconPos )
        {
        case NGlxIconMgrDefs::EGlxIconTopLeft:
            {
            topLeftAnchor.iAnchorHorizOrigin = EAlfAnchorOriginLeft;
            topLeftAnchor.iAnchorVertiOrigin = EAlfAnchorOriginTop;
            
            bottomRightAnchor.iAnchorHorizOrigin = EAlfAnchorOriginLeft;
            bottomRightAnchor.iAnchorVertiOrigin = EAlfAnchorOriginTop;
            
            if( aStretch )
                {
                topLeftAnchor.iPosition = TAlfTimedPoint( 0, 0 );
                bottomRightAnchor.iPosition = TAlfTimedPoint( aWidth, aHeight );
                }
            else
                {
                topLeftAnchor.iPosition = 
                        TAlfTimedPoint( aBorderMargin, aBorderMargin );
                bottomRightAnchor.iPosition = 
                      TAlfTimedPoint( textureWidth,textureHeight);
                }
            
            break;
            }
        case NGlxIconMgrDefs::EGlxIconTopRight:
            {
            topLeftAnchor.iAnchorHorizOrigin = EAlfAnchorOriginRight;
            topLeftAnchor.iAnchorVertiOrigin = EAlfAnchorOriginTop;
            
            bottomRightAnchor.iAnchorHorizOrigin = EAlfAnchorOriginRight;
            bottomRightAnchor.iAnchorVertiOrigin = EAlfAnchorOriginTop;
            
            if( aStretch )
                {
                topLeftAnchor.iPosition = TAlfTimedPoint( -aWidth, 0.0 );
                bottomRightAnchor.iPosition = TAlfTimedPoint( 0.0, aHeight );
                }
            else
                {
                topLeftAnchor.iPosition = 
                    TAlfTimedPoint( -textureWidth, aBorderMargin );
                bottomRightAnchor.iPosition = 
                    TAlfTimedPoint( -aBorderMargin, textureHeight);
                }            
            break;
            }
        case NGlxIconMgrDefs::EGlxIconBottomLeft:
            {
            topLeftAnchor.iAnchorHorizOrigin = EAlfAnchorOriginLeft;
            topLeftAnchor.iAnchorVertiOrigin = EAlfAnchorOriginBottom;
            
            bottomRightAnchor.iAnchorHorizOrigin = EAlfAnchorOriginLeft;
            bottomRightAnchor.iAnchorVertiOrigin = EAlfAnchorOriginBottom;
           
            if( aStretch)
                {
                topLeftAnchor.iPosition = TAlfTimedPoint( aWidth, -aHeight );
                bottomRightAnchor.iPosition = TAlfTimedPoint( 0.0, 1.0 );
                }
            else
                {
                topLeftAnchor.iPosition = 
                    TAlfTimedPoint( aBorderMargin, -textureHeight );
                bottomRightAnchor.iPosition = 
                    TAlfTimedPoint( textureWidth, aBorderMargin );
                }  
                
            break;
            }
        case NGlxIconMgrDefs::EGlxIconBottomRight:
            {
            topLeftAnchor.iAnchorHorizOrigin = EAlfAnchorOriginRight;
            topLeftAnchor.iAnchorVertiOrigin = EAlfAnchorOriginBottom;
            
            bottomRightAnchor.iAnchorHorizOrigin = EAlfAnchorOriginRight;
            bottomRightAnchor.iAnchorVertiOrigin = EAlfAnchorOriginBottom;
                          
            if( aStretch)
                {
                topLeftAnchor.iPosition = TAlfTimedPoint( -aWidth, -aHeight );
                bottomRightAnchor.iPosition = TAlfTimedPoint( 0.0, 0.0 );
                }
            else
                {
                topLeftAnchor.iPosition = 
                    TAlfTimedPoint(  -textureWidth, -textureHeight );
                bottomRightAnchor.iPosition =
                    TAlfTimedPoint( -aBorderMargin, -aBorderMargin );
                }  
                
            break;
            }
        case NGlxIconMgrDefs::EGlxIconCentred:
            {
            topLeftAnchor.iAnchorHorizOrigin = EAlfAnchorOriginHCenter;
            topLeftAnchor.iAnchorVertiOrigin = EAlfAnchorOriginVCenter;
            
            bottomRightAnchor.iAnchorHorizOrigin = EAlfAnchorOriginHCenter;
            bottomRightAnchor.iAnchorVertiOrigin = EAlfAnchorOriginVCenter;
            
            if( aStretch)
                {
                topLeftAnchor.iPosition = TAlfTimedPoint( -aWidth/2, -aHeight/2 );
                bottomRightAnchor.iPosition = TAlfTimedPoint( aWidth/2, aHeight/2 );
                }
            else
                {
                topLeftAnchor.iPosition = 
                    TAlfTimedPoint( -textureWidth/2, -textureHeight/2 );
                bottomRightAnchor.iPosition = 
                    TAlfTimedPoint( textureWidth/2, textureHeight/2 );
                } 
                
            break;
            }
        default:
            {
            
            }
        };

    aAnchors.iTexture = &aTexture;
    aAnchors.iTopLeftAnchor = topLeftAnchor;
    aAnchors.iBotRightAnchor = bottomRightAnchor;
    }

// -----------------------------------------------------------------------------
// FindExistingTexture
// -----------------------------------------------------------------------------
// 
TInt CGlxVisualObject::FindExistingTexture( const CAlfTexture& aTexture,
                                            TBool aForeground )
    {
    TRACER("CGlxVisualObject::FindExistingTexture");
    TGlxIconAnchors icon;
    icon.iTexture = &aTexture;

    return ( aForeground )
            ? iForegroundIconAnchors.Find( icon, TGlxIconAnchors::Match )
            : iBackgroundIconAnchors.Find( icon, TGlxIconAnchors::Match );
    }

// -----------------------------------------------------------------------------
// DoRemoveIcon
// -----------------------------------------------------------------------------
//    
void CGlxVisualObject::DoRemoveIcon( CAlfAnchorLayout& aLayout, 
                        RArray<TGlxIconAnchors>& aAnchorsArray, TInt aPos )
    {
    TRACER("CGlxVisualObject::DoRemoveIcon");
    // First remove the icon visual
    if ( aPos < aLayout.Count() )
        {
        CAlfVisual* visual = &aLayout.Visual( aPos );
        visual->Owner().Remove( visual );
        delete visual;
        }

    // after an icon has been removed from layout the anchor for all icons with
    // positions in layout after that of removed icon will need to be updated
    // due to HUI issue.
    TInt count = aAnchorsArray.Count();
    
    if ( aPos < count )
        {
        // removed anchor information for icon that has been removed
        aAnchorsArray.Remove( aPos );
        
        count--;
        
        // reset anchors for remaining icons
        // only for icons with positions that were after 
        // removed icon
        for ( TInt i = aPos; i < count; i++ )
            {
            SetAnchors( aAnchorsArray[i], aLayout, i );
            }
        }
    }

// -----------------------------------------------------------------------------
// UpdateIconAnchors
// -----------------------------------------------------------------------------
//    
void CGlxVisualObject::UpdateIconAnchors( const CAlfTexture& aTexture, TSize aVisSize )
    {
    TRACER("CGlxVisualObject::UpdateIconAnchors");
    TInt iconCount = iForegroundIconAnchors.Count();
    
    // only proceed if there are foreground icons
    if( iconCount )  
        {
        TSize textureSize = aTexture.Size();
        
        iVertIconOffset = 0.0;
        iHoriIconOffset = 0.0;
        
        TReal32 visWidth = ( TReal32 )aVisSize.iWidth;
        TReal32 visHeight = ( TReal32 )aVisSize.iHeight;

        TReal32 textWidth = ( TReal32 )textureSize.iWidth;
        TReal32 textHeight = ( TReal32 )textureSize.iHeight;
        
        if( textWidth == 0 || textHeight == 0 )
            {
            return;
            }
        
        // compare aspect ratios between visual and texture
        // and calculate either horizontal or vertical offset
        if( (visWidth * textHeight ) < ( textWidth * visHeight ) )
            {
            TInt dispHeight = textHeight * visWidth/textWidth; 
            
            iVertIconOffset = ( visHeight - dispHeight ) /  visHeight;
            iVertIconOffset /= 2;
            }
        else
            {
            TInt dispWidth = textWidth * visHeight/textHeight;
            
            iHoriIconOffset = ( visWidth - dispWidth ) / visWidth;
            iHoriIconOffset /= 2;
            }
        }
    
    // move anchor positon of foreground anchor layout
    iVisual->SetRelativeAnchorRect( KGlxForegroundAnchorOrdinal, 
                EAlfAnchorOriginLeft, EAlfAnchorOriginTop, 
                TAlfRealPoint( iHoriIconOffset, iVertIconOffset),
                EAlfAnchorOriginRight, EAlfAnchorOriginBottom, 
                TAlfRealPoint( -iHoriIconOffset, - iVertIconOffset));  
    }

// ---------------------------------------------------------------------------
// TGlxIconAnchors::Match
// ---------------------------------------------------------------------------
//    
TBool CGlxVisualObject::TGlxIconAnchors::Match(
                const TGlxIconAnchors& aIcon1, const TGlxIconAnchors& aIcon2 )
    {
    TRACER("CGlxVisualObject::Match");
    return ( aIcon1.iTexture == aIcon2.iTexture );
    }
    
void CGlxVisualObject::AddObserver( MGlxVisualObjectLayoutRefreshObserver* aObserver ) 
    {
    TRACER("CGlxVisualObject::AddObserver");
    	__ASSERT_DEBUG(iObservers.Find(aObserver) == KErrNotFound, 
	                    Panic(EGlxPanicIllegalArgument)); // Already exists
	iObservers.Append(aObserver);
    }
    
void CGlxVisualObject::RemoveObserver( MGlxVisualObjectLayoutRefreshObserver* aObserver )
    {
    TRACER("CGlxVisualObject::RemoveObserver");
    TInt i = iObservers.Find(aObserver);
    if (i != KErrNotFound)
        {
        iObservers.Remove(i);
        }
    }
    
 
 

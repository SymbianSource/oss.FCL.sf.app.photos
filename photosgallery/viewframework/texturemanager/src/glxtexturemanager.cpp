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

#include <glxmedia.h>
#include <glxlog.h>
#include <glxtracer.h>
#include "glxtexturemanager.h"

#include <aknconsts.h>

#include "glxtexturemanagerimpl.h"


    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//  
CGlxTextureManager::~CGlxTextureManager()
    {
    TRACER("CGlxTextureManager::~CGlxTextureManager");
    GLX_LOG_INFO( "CGlxTextureManager::~CGlxTextureManager" );
    delete iImpl;
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxTextureManager* CGlxTextureManager::NewL(
                                        CAlfTextureManager& aAlfTextureManager)
    {
    TRACER("CGlxTextureManager::NewL");
    GLX_LOG_INFO( "CGlxTextureManager::NewL" );
    CGlxTextureManager* self = 
                        new (ELeave) CGlxTextureManager();
    CleanupStack::PushL(self);
    self->iImpl = CGlxTextureManagerImpl::NewL(aAlfTextureManager);
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
///CreateNewTextureForMediaL
// -----------------------------------------------------------------------------

EXPORT_C CAlfTexture& CGlxTextureManager::CreateNewTextureForMediaL(
        TSize aSize, 
        const TGlxMedia& aMedia, 
        const TGlxIdSpaceId& aIdSpaceId, 
        MGlxTextureObserver* aObserver )
    {
    TRACER("CGlxTextureManager::CreateNewTextureForMediaL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateNewTextureForMediaL" );
    return iImpl->CreateNewTextureForMediaL(aSize,aMedia,aIdSpaceId,aObserver,EFalse);
    
    }
// CreateAvkonIconTextureL - with backup bitmap ID
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateAvkonIconTextureL(
                                                        const TAknsItemID &aID, 
                                                        TInt aBitmapId,
                                                        TSize aRequestedSize)
    {
    TRACER("CGlxTextureManager::CreateAvkonIconTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateAvkonIconTextureL" );
    return iImpl->CreateIconTextureL(aID, 
                                     aBitmapId, 
                                     KAvkonBitmapFile,
                                     EFalse,
                                     KRgbWhite,
                                     KAknsIIDNone,
                                     0, 
                                     aRequestedSize);
    }

// -----------------------------------------------------------------------------
// CreateColorAvkonIconTextureL - with backup bitmap ID and ability to set color
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateColorAvkonIconTextureL(
                                                        const TAknsItemID& aID, 
                                                        TInt aBitmapId,
                                                        TRgb aDefaultColor,
                                                        const TAknsItemID& aSkinColorId,
                                                        TInt aSkinColorIndex,
                                                        TSize aRequestedSize)
    {
    TRACER("CGlxTextureManager::CreateColorAvkonIconTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateColorAvkonIconTextureL" );
    return iImpl->CreateIconTextureL(aID, 
                                     aBitmapId, 
                                     KAvkonBitmapFile,
                                     ETrue,
                                     aDefaultColor,
                                     aSkinColorId,
                                     aSkinColorIndex, 
                                     aRequestedSize);
    }

// -----------------------------------------------------------------------------
// CreateAvkonIconTextureL - no backup bitmap ID
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateAvkonIconTextureL(
                                                        const TAknsItemID &aID, 
                                                        TSize aRequestedSize)
    {
    TRACER("CGlxTextureManager::CreateAvkonIconTextureL 2");
    GLX_LOG_INFO( "CGlxTextureManager::CreateAvkonIconTextureL 2" );
    return iImpl->CreateIconTextureL(aID,
                                     0, 
                                     KNullDesC(),
                                     EFalse,
                                     KRgbWhite,
                                     KAknsIIDNone,
                                     0, 
                                     aRequestedSize);
    }

// -----------------------------------------------------------------------------
// CreateIconTextureL - from ID and mif/mbm file name
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateIconTextureL(
                                                   TInt aIconResourceId, 
                                                   TDesC& aFilename,
                                                   TSize aRequestedSize)
    {
    TRACER("CGlxTextureManager::CreateIconTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateIconTextureL" );
    return iImpl->CreateIconTextureL(KAknsIIDNone, 
                                     aIconResourceId, 
                                     aFilename,
                                     EFalse,
                                     KRgbWhite,
                                     KAknsIIDNone,
                                     0, 
                                     aRequestedSize);
    }
    
// -----------------------------------------------------------------------------
// CreateColorIconTextureL - from ID and mif/mbm file name, modifying colour
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateColorIconTextureL(
                                                   TInt aIconResourceId, 
                                                   TDesC& aFilename,
                                                   TRgb  aDefaultColor,
                                                   const TAknsItemID& aSkinColorId,
                                                   TInt aSkinColorIndex,
                                                   TSize aRequestedSize)

    {
    TRACER("CGlxTextureManager::CreateColorIconTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateColorIconTextureL" );
    return iImpl->CreateIconTextureL(KAknsIIDNone, 
                                     aIconResourceId, 
                                     aFilename,
                                     ETrue,
                                     aDefaultColor,
                                     aSkinColorId,
                                     aSkinColorIndex, 
                                     aRequestedSize);
    }
    
    

// -----------------------------------------------------------------------------
// CreateThumbnailTextureL
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateThumbnailTextureL(
                const TGlxMedia& aMedia, const TGlxIdSpaceId& aIdSpaceId,
                const TSize& aRequiredSize, MGlxTextureObserver* aObserver,
                TBool aScaleGridTnm)
    {
    TRACER("CGlxTextureManager::CreateThumbnailTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateThumbnailTextureL" );
    return iImpl->CreateNewTextureForMediaL( aRequiredSize, aMedia, aIdSpaceId,
                                            aObserver,aScaleGridTnm );
    }

// -----------------------------------------------------------------------------
// CreateZoomedTextureL
// -----------------------------------------------------------------------------
//  
EXPORT_C CAlfTexture& CGlxTextureManager::CreateZoomedTextureL(
                    const TGlxMedia& aMedia, const TMPXAttribute& aAttribute,
                    TGlxIdSpaceId aIdSpaceId, MGlxTextureObserver* aObserver)
    {
    TRACER("CGlxTextureManager::CreateZoomedTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateZoomedTextureL" );
    return iImpl->CreateZoomedTextureL(aMedia, aAttribute, aIdSpaceId,aObserver);
    }

// -----------------------------------------------------------------------------
// CreateZoomedTextureL
// -----------------------------------------------------------------------------
//  
EXPORT_C CAlfTexture* CGlxTextureManager::CreateZoomedTextureL()
    {
    TRACER("CGlxTextureManager::CreateZoomedTextureL 2");
    GLX_LOG_INFO( "CGlxTextureManager::CreateZoomedTextureL 2" );
    return iImpl->CreateZoomedTextureL();
    }

// -----------------------------------------------------------------------------
// CreateAnimatedGifTextureL
// -----------------------------------------------------------------------------
//  
EXPORT_C CAlfTexture& CGlxTextureManager::CreateAnimatedGifTextureL(
        const TDesC& aFilename,const TSize& aSize,
        const TGlxMedia& aMedia, TGlxIdSpaceId aIdSpaceId)
    {
    TRACER("CGlxTextureManager::CreateAnimatedGifTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateAnimatedGifTextureL" );
    return iImpl->CreateAnimatedGifTextureL( aFilename, aSize, aMedia, aIdSpaceId);
    }    		    

// -----------------------------------------------------------------------------
// CreateFlatColourTextureL
// -----------------------------------------------------------------------------
//  
EXPORT_C CAlfTexture& CGlxTextureManager::CreateFlatColourTextureL( TRgb aColour )
    {
    TRACER("CGlxTextureManager::CreateFlatColourTextureL");
    GLX_LOG_INFO( "CGlxTextureManager::CreateFlatColourTextureL" );
    return iImpl->CreateFlatColourTextureL( aColour );
    }
    
// -----------------------------------------------------------------------------
// RemoveTexture
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxTextureManager::RemoveTexture(
                                   const CAlfTexture& aTexture)
    {
    TRACER("CGlxTextureManager::RemoveTexture");
    GLX_LOG_INFO( "CGlxTextureManager::RemoveTexture" );
    iImpl->RemoveTexture(aTexture);
    }

// -----------------------------------------------------------------------------
// RemoveTexture
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxTextureManager::RemoveTexture(const TGlxMediaId& aMediaId,TBool aAllTexture)
    {
    TRACER("CGlxTextureManager::RemoveTexture 2");
    GLX_LOG_INFO( "CGlxTextureManager::RemoveTexture 2" );
	//Bug fix for PKAA-7NRBYZ - added aAllTexture param
    iImpl->RemoveTexture(aMediaId, aAllTexture);
    }

// -----------------------------------------------------------------------------
// FlushTextures
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxTextureManager::FlushTextures() 
    {
    TRACER("CGlxTextureManager::FlushTextures ");
    iImpl->FlushTextures();
    }

// -----------------------------------------------------------------------------
// IsBetterThumbnailAvailable
// -----------------------------------------------------------------------------
//  
EXPORT_C TBool CGlxTextureManager::TextureNeedsUpdating( TGlxMedia aMedia,
    TGlxIdSpaceId aIdSpaceId, const TSize& aRequiredSize )
    {
    TRACER("CGlxTextureManager::TextureNeedsUpdating");
    GLX_LOG_INFO( "CGlxTextureManager::TextureNeedsUpdating" );
    return iImpl->TextureNeedsUpdating( aMedia, aIdSpaceId, aRequiredSize );
    }


//  ----------------------------------------------------------------------------
//	HandleSkinChanged
//  ----------------------------------------------------------------------------

void CGlxTextureManager::HandleSkinChanged()
	{
	TRACER("CGlxTextureManager::HandleSkinChanged");
    GLX_LOG_INFO( "CGlxTextureManager::HandleSkinChanged" );
	iImpl->UpdateTexture();
	}

	
// -----------------------------------------------------------------------------
// RemoveZoomList
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxTextureManager::RemoveZoomList()
    {
    TRACER("CGlxTextureManager::RemoveZoomList");
    GLX_LOG_INFO( "CGlxTextureManager::RemoveZoomList" );
    iImpl->RemoveZoomList();
    }

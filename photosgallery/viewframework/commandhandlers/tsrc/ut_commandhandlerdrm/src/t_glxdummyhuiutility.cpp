/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    HUITK utilities
*
*/





// INCLUDE FILES
#include <Alf/AlfControlGroup.h>
#include <Alf/AlfDisplay.h>
#include <Alf/AlfUtil.h>
#include <Alf/AlfEnv.h>
#include <Alf/AlfTexture.h>
#include <Alf/AlfTextureManager.h>
#include <Alf/AlfTextStyleManager.h>
#include "glxtexturemanager.h"
#include <glxsingletonstore.h>

#include "glxpanic.h"
#include "glxuiutility.h"
#include "glxuiutilitycoecontrol.h"
#include "glxscreenfurniture.h"

#include <aknutils.h>
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
	
// -----------------------------------------------------------------------------
// UtilityL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxUiUtility* CGlxUiUtility::UtilityL()
	{
	return CGlxSingletonStore::InstanceL(&NewL);
	}

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::Close()
	{
    CGlxSingletonStore::Close(this);
    //delete this;
	}

// -----------------------------------------------------------------------------
// 2-phase constructor
// -----------------------------------------------------------------------------
//
CGlxUiUtility* CGlxUiUtility::NewL() 
    {
    EUNIT_PRINT(_L("CGlxUiUtility::NewL"));
	CGlxUiUtility* obj = new (ELeave) CGlxUiUtility();
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
    return obj;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxUiUtility::CGlxUiUtility()
:   iNavigationDirection(EGlxNavigationForwards)
	{
	}

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxUiUtility::ConstructL()
	{
	EUNIT_PRINT(_L("Entering CGlxUiUtility::ConstructL"));
    iEnv = CAlfEnv::NewL();
    EUNIT_PRINT(_L("CGlxUiUtility::iEnv Created"));
    iGlxTextureManager = CGlxTextureManager::NewL(iEnv->TextureManager());
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxUiUtility::~CGlxUiUtility()
	{
	delete iGlxTextureManager;
  
  	delete iEnv;
   	iEnv = NULL;
	}

// -----------------------------------------------------------------------------
// Env
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfEnv* CGlxUiUtility::Env() 
	{
	return iEnv;
	}

// -----------------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfDisplay* CGlxUiUtility::Display() const
	{
	return NULL;
	}

// -----------------------------------------------------------------------------
// ShowHuiDisplay
// -----------------------------------------------------------------------------
EXPORT_C void CGlxUiUtility::ShowAlfDisplayL()
	{
	}

// -----------------------------------------------------------------------------
// HideHuiDisplay
// -----------------------------------------------------------------------------
EXPORT_C void CGlxUiUtility::HideAlfDisplayL()
	{
	}

// -----------------------------------------------------------------------------
// ScreenFurniture
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxScreenFurniture* CGlxUiUtility::ScreenFurniture()
	{
    return NULL;
	}

	
// -----------------------------------------------------------------------------
// GlxTextureManager
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxTextureManager& CGlxUiUtility::GlxTextureManager()
	{
	__ASSERT_ALWAYS(iGlxTextureManager, Panic(EGlxPanicLogicError));
    return *iGlxTextureManager;
	}
	

// -----------------------------------------------------------------------------
// ViewNavigationDirection
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxNavigationDirection CGlxUiUtility::ViewNavigationDirection()
    {
    return iNavigationDirection;
    }
    

// -----------------------------------------------------------------------------
// SetViewNavigationDirection
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::SetViewNavigationDirection(TGlxNavigationDirection /*aDirection*/)
    {
    }



// -----------------------------------------------------------------------------
// DisplaySize
// -----------------------------------------------------------------------------
//
EXPORT_C TSize CGlxUiUtility::DisplaySize() const
    {
    return TSize();
    }


// -----------------------------------------------------------------------------
// TextStyleIdL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::TextStyleIdL(TInt /*aFontId*/, TInt /*aSizeInPixels*/)
    {
    TInt id = 0;
    return id;
    }
    
EXPORT_C void CGlxUiUtility::HandleTvStatusChangedL( TTvChangeType /*aChangeType*/ )    
    {
    
    }
    
EXPORT_C void CGlxUiUtility:: HandleActionL (const TAlfActionCommand &aActionCommand)
    {
    
    }

   
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//  
CGlxTextureManager::~CGlxTextureManager()
    {
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxTextureManager* CGlxTextureManager::NewL(
                                        CAlfTextureManager& aAlfTextureManager)
    {
    CGlxTextureManager* self =  new (ELeave) CGlxTextureManager();
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CreateAvkonIconTextureL
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateAvkonIconTextureL(
                                                        const TAknsItemID &/*aID*/, 
                                                        TInt /*aBitmapId*/,
                                                        TSize /*aRequestedSize*/)
    {

    CAlfTexture* texture = NULL;
    return *texture;
    }

// -----------------------------------------------------------------------------
// CreateIconTextureL
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CGlxTextureManager::CreateIconTextureL(
                                                   TInt /*aIconResourceId*/, 
                                                   TDesC& /*aFilename*/,
                                                   TSize /*aRequestedSize*/)
    {
   
    CAlfTexture* texture = NULL;       
    return *texture;
    }

// -----------------------------------------------------------------------------
// CreateThumbnailTextureL
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfTexture& CreateThumbnailTextureL(
                TGlxMediaId aMediaId, TGlxIdSpaceId aIdSpaceId,
                const TSize& aRequiredSize, MGlxTextureObserver* aObserver,
                TInt& aTextureId )
                
    {
    // Create empty texture (not owned)
    CAlfTexture* texture = NULL;
    return *texture;
    }

// -----------------------------------------------------------------------------
// CreateZoomedTextureL
// -----------------------------------------------------------------------------
//  

EXPORT_C CAlfTexture& CreateZoomedTextureL(const TGlxMedia& aMedia,
                                            const TMPXAttribute& aAttribute,
                                            TGlxIdSpaceId aIdSpaceId,
                                            TInt& aTextureId )
    {
    // Create empty texture
    CAlfTexture* texture = NULL;
    return *texture;
    }


// -----------------------------------------------------------------------------
// RemoveTexture
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxTextureManager::RemoveTexture(
                                   const CAlfTexture& aTexture)
    {
    }

EXPORT_C void CGlxTextureManager:: HandleSkinChanged()
    {
    
    }

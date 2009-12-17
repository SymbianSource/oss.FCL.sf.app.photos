/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Control to create a black background for the slideshow
*
*/



#include "shwslideshowblackoutcontrol.h"

#include <uiacceltk/huienv.h> 
#include <uiacceltk/huidisplay.h>
#include <uiacceltk/huiImageVisual.h>
#include <uiacceltk/huiAnchorLayout.h>

#include "glxhuiutility.h"

namespace
	{
	/**
	 * Time to complete blackout
	 */
	const TInt KShwBlackoutDelay = 0;
	/**
	 * Opacity value - fully opaque
	 */
	const TInt KShwOpaque = 1;
	/**
	 * Opacity value - transparent
	 */
	const TInt KShwTransparent = 0;
	const TInt KShwScreenSizeMultiplier = 3;
	const TInt KShwOrigin = 0;
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CShwBlackoutControl* CShwBlackoutControl::NewL( CHuiEnv& aEnv,
	CHuiDisplay& aDisplay )
    {
    CShwBlackoutControl* self = new( ELeave )
        CShwBlackoutControl( aEnv, aDisplay );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CShwBlackoutControl::CShwBlackoutControl( CHuiEnv& aEnv, CHuiDisplay& aDisplay )
        : CHuiControl( aEnv ), iDisplay( aDisplay )
    {    
    BindDisplay( aDisplay );
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CShwBlackoutControl::ConstructL()
    {
    // Create the black background to fade out the rest of the screen
	iBlackoutTexture = CHuiTexture::NewL();
	CGlxHuiUtility::UploadFlatColourToTextureL( KRgbBlack, iBlackoutTexture );

	iBlackoutLayout = CHuiAnchorLayout::AddNewL( *this );
	iBlackoutLayout->SetClipping( EFalse );

	iBlackoutImageVisual = CHuiImageVisual::AddNewL( *this, iBlackoutLayout );
	iBlackoutImageVisual->SetImage( THuiImage( *iBlackoutTexture ) );
	iBlackoutImageVisual->iOpacity.Set( KShwTransparent );

	iBlackoutImageVisual->SetFlag( EHuiVisualFlagManualLayout );
	TSize displaySize = iDisplay.Size();
	iBlackoutImageVisual->SetSize( THuiRealSize(
	    displaySize.iWidth * KShwScreenSizeMultiplier,
		displaySize.iHeight * KShwScreenSizeMultiplier ));

	iBlackoutImageVisual->SetPos( THuiRealPoint( KShwOrigin, KShwOrigin) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CShwBlackoutControl::~CShwBlackoutControl()
    {   
    delete iBlackoutTexture;
    }
	
// -----------------------------------------------------------------------------
// ActivateL
// -----------------------------------------------------------------------------
//
void CShwBlackoutControl::ActivateL()
    {
    if ( !iActivated )
        {        
        iBlackoutImageVisual->iOpacity.Set( KShwOpaque, KShwBlackoutDelay );
        iActivated = ETrue;
        }
    }
    
// -----------------------------------------------------------------------------
// Deactivate
// -----------------------------------------------------------------------------
//
void CShwBlackoutControl::Deactivate()
    {
    if ( iActivated )
        {
        iBlackoutImageVisual->iOpacity.Set( KShwTransparent,
            KShwBlackoutDelay );
        iActivated = EFalse;
        }
    }

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
* Description:    Image loading animation
*
*/





#include "glxanimationimageloading.h"

#include <alf/alfvisual.h>
#include <alf/alfimage.h>
#include <alf/alftexture.h>
#include <alf/alfimagebrush.h>
#include <alf/alfbrusharray.h>

#include "glxuiutility.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxAnimationImageLoading* CGlxAnimationImageLoading::NewL( 
        CAlfVisual& aVisual, 
        CAlfTexture& aFlashTexture)
    {
    CGlxAnimationImageLoading* self = new (ELeave) CGlxAnimationImageLoading(aVisual, aFlashTexture);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxAnimationImageLoading::CGlxAnimationImageLoading(CAlfVisual& aVisual, 
        CAlfTexture& aFlashTexture)
:   iVisual(aVisual),
    iFlashTexture(aFlashTexture)
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxAnimationImageLoading::ConstructL()
    {
    
    // Create brush
    CGlxUiUtility* utility = CGlxUiUtility::UtilityL();
    CleanupClosePushL(*utility);
    
    // Rowland 30/10/07 Now need to get the environment
    CAlfEnv* env = utility->Env();
    
    TAlfImage img(iFlashTexture);
    
    iImageBrush = CAlfImageBrush::NewL(*env, img);
    
    CleanupStack::PopAndDestroy(utility);
    
    iImageBrush->SetLayer(EAlfBrushLayerForeground);
    iImageBrush->SetBorders(1,1,1,1);
    /// @todo check is this is correct? Rowland 30/10/07 SetClipToVisual is no longer defined
    //iImageBrush->SetClipToVisual(ETrue);
    iImageBrush->SetScaleMode(CAlfImageVisual::EScaleFit);
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxAnimationImageLoading::~CGlxAnimationImageLoading()
    {
    }
    
// -----------------------------------------------------------------------------
// StartTimedAnimationL
// -----------------------------------------------------------------------------
//
void CGlxAnimationImageLoading::StartTimedAnimationL( TInt aTime )
    {
    iVisual.EnableBrushesL();
    iVisual.Brushes()->AppendL(iImageBrush, EAlfHasOwnership);
    TAlfTimedValue opacity;
    opacity.SetValueNow( 1.0 ); // immediate change
    opacity.SetTarget(0.0, aTime ); // target
    iImageBrush->SetOpacity(opacity);
    }
    


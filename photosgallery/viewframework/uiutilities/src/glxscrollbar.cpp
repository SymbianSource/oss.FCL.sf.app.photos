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
* Description:    Scrollbar
*
*/




#include "glxscrollbar.h"

#include <e32cmn.h>

#include <AknsConstants.h>

#include <alf/alfcontrol.h>
#include <alf/alflayout.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexture.h>
#include <alf/alfimage.h>

#include <glxtexturemanager.h>

#include <glxpanic.h>
#include <glxlog.h>

#include "mglxscrollbarobserver.h"
#include "glxuiutility.h"

// S60 standard size for scrollbar
const TInt KWidth = 8;

const TInt KBackgroundLayoutIndex = 0;
const TInt KThumbLayoutIndex = 1;

const TInt KTopVisualIndex = 0;
const TInt KMiddleVisualIndex = 1;
const TInt KBottomVisualIndex = 2;

// -----------------------------------------------------------------------------
// Two phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxScrollbar* CGlxScrollbar::NewLC(CAlfControl& aControl, CAlfLayout& aParentLayout)
    {
    GLX_LOG_INFO( "CGlxScrollbar::NewLC" );

    CGlxScrollbar* self = new (ELeave) CGlxScrollbar();
    CleanupStack::PushL( self );
    self->ConstructL( aControl, aParentLayout );
    return self;
    }

// -----------------------------------------------------------------------------
// Two phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxScrollbar* CGlxScrollbar::NewL(CAlfControl& aControl, CAlfLayout& aParentLayout)
    {
    GLX_LOG_INFO( "CGlxScrollbar::NewL" );

    CGlxScrollbar* self = CGlxScrollbar::NewLC( aControl, aParentLayout );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxScrollbar::CGlxScrollbar()
    {
    GLX_LOG_INFO( "CGlxScrollbar::CGlxScrollbar" );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxScrollbar::~CGlxScrollbar()
    {
    GLX_LOG_INFO( "CGlxScrollbar::~CGlxScrollbar" );

    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// Two phase constructor
// -----------------------------------------------------------------------------
//
void CGlxScrollbar::ConstructL(CAlfControl& aControl, CAlfLayout& aParentLayout)
    {
    GLX_LOG_INFO( "CGlxScrollbar::ConstructL" );

    iLayout = CAlfLayout::AddNewL( aControl, &aParentLayout );

    // Create the background layout and child visuals
    CAlfAnchorLayout* backgroundLayout = CAlfAnchorLayout::AddNewL( aControl, iLayout );

    CAlfImageVisual::AddNewL( aControl, backgroundLayout );
    CAlfImageVisual::AddNewL( aControl, backgroundLayout );
    CAlfImageVisual::AddNewL( aControl, backgroundLayout );

    // Create the thumb layout and child visuals
    CAlfAnchorLayout* thumbLayout = CAlfAnchorLayout::AddNewL( aControl, iLayout );

    CAlfImageVisual::AddNewL( aControl, thumbLayout );
    CAlfImageVisual::AddNewL( aControl, thumbLayout );
    CAlfImageVisual::AddNewL( aControl, thumbLayout );

    // Create and set textures for the visuals
    SetTexturesL();
    }

// -----------------------------------------------------------------------------
// Add an observer to observe scrollbar
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScrollbar::AddObserverL(MGlxScrollbarObserver* aObserver)
    {
    GLX_LOG_INFO( "CGlxScrollbar::AddObserverL" );

    __ASSERT_DEBUG( iObservers.Find( aObserver ) == KErrNotFound, 
                    Panic( EGlxPanicIllegalArgument ) );

    iObservers.AppendL( aObserver );
    }

// -----------------------------------------------------------------------------
// Remove an observer
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScrollbar::RemoveObserver(MGlxScrollbarObserver* aObserver)
    {
    GLX_LOG_INFO( "CGlxScrollbar::RemoveObserver" );

    TInt index = iObservers.Find( aObserver );

    __ASSERT_DEBUG( index != KErrNotFound, Panic( EGlxPanicIllegalArgument ) );

    if (index != KErrNotFound) 
        {
        iObservers.Remove( index );	
        }
    }

// -----------------------------------------------------------------------------
// Sets the visible length of the scrollbar
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScrollbar::SetVisibleLength(TUint aVisibleLength)
    {
    GLX_LOG_INFO( "CGlxScrollbar::SetVisibleLength" );

    __ASSERT_DEBUG( aVisibleLength > 0, Panic( EGlxPanicIllegalArgument ) );

    iVisibleLength = aVisibleLength;

    NotifyObservers();

    Update( 0 );
    }

// -----------------------------------------------------------------------------
// Sets the full length of the scrollbar
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScrollbar::SetFullLength(TUint aFullLength, TUint aTransitionTime)
    {
    GLX_LOG_INFO( "CGlxScrollbar::SetFullLength" );

    __ASSERT_DEBUG( aFullLength > 0, Panic( EGlxPanicIllegalArgument ) );

    iFullLength = aFullLength;

    NotifyObservers();

    Update( aTransitionTime );
    }

// -----------------------------------------------------------------------------
// Sets the position of the first visible row in the full length
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScrollbar::SetPosition(TUint aPosition, TUint aTransitionTime)
    {
    GLX_LOG_INFO( "CGlxScrollbar::SetPosition" );

    __ASSERT_DEBUG( aPosition < iFullLength, Panic( EGlxPanicIllegalArgument ) );

    iPosition = aPosition;
    if ( iPosition > iFullLength - iVisibleLength )
        {
        iPosition = iFullLength - iVisibleLength;
        }

    Update( aTransitionTime );
    }

// -----------------------------------------------------------------------------
// Set the textures for the visuals
// -----------------------------------------------------------------------------
//
void CGlxScrollbar::SetTexturesL()
    {
    GLX_LOG_INFO( "CGlxScrollbar::SetTexturesL" );

    CGlxUiUtility* UiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL( *UiUtility );

    CGlxTextureManager& textureManager = UiUtility->GlxTextureManager();

    SetBackgroundTexturesL( textureManager );
    SetThumbTexturesL( textureManager );

    CleanupStack::PopAndDestroy( UiUtility );
    }

// -----------------------------------------------------------------------------
// Set the textures for the visuals in the background layout
// -----------------------------------------------------------------------------
//
void CGlxScrollbar::SetBackgroundTexturesL(CGlxTextureManager& aTextureManager)
    {
    GLX_LOG_INFO( "CGlxScrollbar::SetBackgroundTexturesL" );

    CAlfAnchorLayout& backgroundLayout = 
        static_cast<CAlfAnchorLayout&>( iLayout->Visual( KBackgroundLayoutIndex ) );

    CAlfTexture& bgTopTexture = 
        aTextureManager.CreateAvkonIconTextureL( KAknsIIDQsnCpScrollBgTop,
                                                 TSize( KWidth, KWidth ) );

    CAlfImageVisual& bgTop = 
        static_cast<CAlfImageVisual&>( backgroundLayout.Visual( KTopVisualIndex ) );
    bgTop.SetImage( TAlfImage( bgTopTexture ) );

    CAlfTexture& bgMiddleTexture = 
        aTextureManager.CreateAvkonIconTextureL( KAknsIIDQsnCpScrollBgMiddle,
                                                 TSize( KWidth, KWidth ) );

    CAlfImageVisual& bgMiddle = 
        static_cast<CAlfImageVisual&>( backgroundLayout.Visual( KMiddleVisualIndex ) );
    bgMiddle.SetImage( TAlfImage( bgMiddleTexture ) );

    CAlfTexture& bgBottomTexture = 
        aTextureManager.CreateAvkonIconTextureL( KAknsIIDQsnCpScrollBgBottom,
                                                 TSize( KWidth, KWidth ) );

    CAlfImageVisual& bgBottom = 
        static_cast<CAlfImageVisual&>( backgroundLayout.Visual( KBottomVisualIndex ) );
    bgBottom.SetImage( TAlfImage( bgBottomTexture ) );
    }

// -----------------------------------------------------------------------------
// Set the textures for the visuals in the thumb layout
// -----------------------------------------------------------------------------
//
void CGlxScrollbar::SetThumbTexturesL(CGlxTextureManager& aTextureManager)
    {
    GLX_LOG_INFO( "CGlxScrollbar::SetThumbTexturesL" );

    CAlfAnchorLayout& thumbLayout = 
        static_cast<CAlfAnchorLayout&>( iLayout->Visual( KThumbLayoutIndex ) );

    CAlfTexture& thumbTopTexture = 
        aTextureManager.CreateAvkonIconTextureL( KAknsIIDQsnCpScrollHandleTop,
                                                 TSize( KWidth, KWidth ) );

    CAlfImageVisual& thumbTop = 
        static_cast<CAlfImageVisual&>( thumbLayout.Visual( KTopVisualIndex ) );
    thumbTop.SetImage( TAlfImage( thumbTopTexture ) );

    CAlfTexture& thumbMiddleTexture = 
        aTextureManager.CreateAvkonIconTextureL( KAknsIIDQsnCpScrollHandleMiddle,
                                                 TSize( KWidth, KWidth ) );

    CAlfImageVisual& thumbMiddle = 
        static_cast<CAlfImageVisual&>( thumbLayout.Visual( KMiddleVisualIndex ) );
    thumbMiddle.SetImage( TAlfImage( thumbMiddleTexture ) );

    CAlfTexture& thumbBottomTexture = 
        aTextureManager.CreateAvkonIconTextureL( KAknsIIDQsnCpScrollHandleBottom,
                                                 TSize( KWidth, KWidth ) );

    CAlfImageVisual& thumbBottom = 
        static_cast<CAlfImageVisual&>( thumbLayout.Visual( KBottomVisualIndex ) );
    thumbBottom.SetImage( TAlfImage( thumbBottomTexture ) );
    }

// -----------------------------------------------------------------------------
// Set the background or thumb layout anchors
// -----------------------------------------------------------------------------
//
void CGlxScrollbar::SetAnchors(CAlfAnchorLayout& aLayout)
    {
    GLX_LOG_INFO( "CGlxScrollbar::SetAnchors" );

    TInt width = aLayout.Size().Target().AsSize().iWidth;

    aLayout.SetAnchor(EAlfAnchorTopLeft, KTopVisualIndex, 
                      EAlfAnchorOriginLeft, EAlfAnchorOriginTop, 
                      EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, 
                      TAlfTimedPoint(0, 0));
    aLayout.SetAnchor(EAlfAnchorBottomRight, KTopVisualIndex, 
                      EAlfAnchorOriginRight, EAlfAnchorOriginTop, 
                      EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, 
                      TAlfTimedPoint(0, width));

    aLayout.SetAnchor(EAlfAnchorTopLeft, KMiddleVisualIndex, 
                      EAlfAnchorOriginLeft, EAlfAnchorOriginTop, 
                      EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, 
                      TAlfTimedPoint(0, width));
    aLayout.SetAnchor(EAlfAnchorBottomRight, KMiddleVisualIndex, 
                      EAlfAnchorOriginRight, EAlfAnchorOriginBottom, 
                      EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, 
                      TAlfTimedPoint(0, -width));

    aLayout.SetAnchor(EAlfAnchorTopLeft, KBottomVisualIndex, 
                      EAlfAnchorOriginLeft, EAlfAnchorOriginBottom, 
                      EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, 
                      TAlfTimedPoint(0, -width));
    aLayout.SetAnchor(EAlfAnchorBottomRight, KBottomVisualIndex, 
                      EAlfAnchorOriginRight, EAlfAnchorOriginBottom, 
                      EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, 
                      TAlfTimedPoint(0, 0));

    aLayout.UpdateChildrenLayout();
    }

// -----------------------------------------------------------------------------
// Update the scrollbar
// -----------------------------------------------------------------------------
//
void CGlxScrollbar::Update(TUint aTransitionTime)
    {
    GLX_LOG_INFO( "CGlxScrollbar::Update" );

    TSize layoutSize = iLayout->Size().Target().AsSize();

    TSize thumbSize(layoutSize);
    TPoint thumbPos( 0, 0 );
    if ( iFullLength > iVisibleLength )
        {
        thumbSize.iHeight = layoutSize.iHeight * iVisibleLength / iFullLength;
        thumbPos.iY = layoutSize.iHeight * iPosition / iFullLength;
        }

    CAlfAnchorLayout& backgroundLayout = 
        static_cast<CAlfAnchorLayout&>( iLayout->Visual( KBackgroundLayoutIndex ) );

    backgroundLayout.SetPos( TPoint( 0, 0 ) );
    backgroundLayout.SetSize( layoutSize );
    SetAnchors( backgroundLayout );

    CAlfAnchorLayout& thumbLayout = 
        static_cast<CAlfAnchorLayout&>( iLayout->Visual( KThumbLayoutIndex ) );

    thumbLayout.SetPos( thumbPos, aTransitionTime );
    thumbLayout.SetSize( thumbSize, aTransitionTime );
    SetAnchors( thumbLayout );
    }

// -----------------------------------------------------------------------------
// Notify observers if the scrollbar is scrollable
// -----------------------------------------------------------------------------
//
void CGlxScrollbar::NotifyObservers()
    {
    GLX_LOG_INFO( "CGlxScrollbar::NotifyObservers" );

    TBool scrollable(EFalse);
    if ( iFullLength > iVisibleLength )
        {
        scrollable = ETrue;
        }

    TInt count = iObservers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iObservers[i]->HandleScrollable(scrollable);
        }
    }

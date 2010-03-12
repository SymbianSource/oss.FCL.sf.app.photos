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
* Description:    Cloud View Context menu control class 
 *
*/

// Alf Headers
#include <alf/alflayout.h>
#include <alf/alfgridlayout.h> // For CAlfGridLayout
#include <alf/alfborderbrush.h> // For CAlfBorderBrush
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfbrusharray.h>
#include <alf/alftextstyle.h>
#include <alf/alfenv.h>
#include <alf/alfeventhandler.h>
#include <alf/alfutil.h>
#include <alf/alftexture.h>
#include <alf/alfevent.h>
#include <alf/ialfwidgetfactory.h>
#include <alf/ialfviewwidget.h>
#include <alf/alfdisplay.h>

#include <aknnotewrappers.h>
#include <fbs.h>
#include <StringLoader.h>
#include "utf.h"                    // UtfConverter

// Photos Headers
#include "glxtagscontextmenucontrol.h"
#include "glxcommandfactory.h"
#include <mglxmedialist.h>                          //for medialist
#include <glxuiutility.h>
#include <glxtexturemanager.h>                      // Texturemanager
#include <glxicons.mbg>                             // icons
#include <glxuistd.h>                               // Attribute fetch priority
#include <glxcommandhandlers.hrh>
#include "glxbubbletimer.h"                         //for timer functionality
#include <glxtagsbrowserview.rsg>                   // For resources
#include <glxtracer.h>                              // For Logs

//Left & Top padding of text within text box (In Pixels)
const TInt KMinimalGap = 5;         
//Height of each menu item
const TInt KReqHeightPerMenuItem = 47;
//Minimum width for control
const TInt KReqWidth = 113;
//Number of menu items present in stylus menu
const TInt KNumofMenuItems = 3;
//Number of columns present in grid control showing menu  
const TInt KNoOfColumns = 1;
//Highest possible value to make control opaque
const TReal KOpacityOpaque = 1.0;
//lowest possible value to make control completely transparent
const TReal KOpacityTransparent = 0.0;
const TPoint KDummyPoint(100,100);
//6 Seconds delay for menu control visibility on screen 
const TInt KTimerDelay = 6000000;
//Control complete height
const TInt KGridHeight = KReqHeightPerMenuItem * KNumofMenuItems;
//Text size for menu items
const TInt KTextSizeInPixels = 20;

//For Tagging the visuals
_LIT8(KTagSlideshow, "SS");
_LIT8(KTagRename, "Ren");
_LIT8(KTagDelete, "Del");

// --------------------------------------------------------------------------- 
// NewL()
// --------------------------------------------------------------------------- 
//
CGlxTagsContextMenuControl* CGlxTagsContextMenuControl::NewL( 
        MGlxItemMenuObserver& aItemMenuObserver)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::NewL");
    CGlxTagsContextMenuControl* self = CGlxTagsContextMenuControl::NewLC( aItemMenuObserver);
    CleanupStack::Pop(self);
    return self;
    }
// --------------------------------------------------------------------------- 
// NewLC()
// --------------------------------------------------------------------------- 
//
CGlxTagsContextMenuControl* CGlxTagsContextMenuControl::NewLC( 
        MGlxItemMenuObserver& aItemMenuObserver )
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::NewLC");
    CGlxTagsContextMenuControl* self = new (ELeave) CGlxTagsContextMenuControl( aItemMenuObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// --------------------------------------------------------------------------- 
// CGlxTagsContextMenuControl()
// --------------------------------------------------------------------------- 
//
CGlxTagsContextMenuControl::CGlxTagsContextMenuControl(MGlxItemMenuObserver& aItemMenuObserver)
    : iItemMenuObserver(aItemMenuObserver)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::CGlxTagsContextMenuControl");
    //Nothing more to do for now
    }
// --------------------------------------------------------------------------- 
// ConstructL()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::ConstructL()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::ConstructL");
    iUiUtility = CGlxUiUtility::UtilityL ();
    CAlfControl::ConstructL(*(iUiUtility->Env()) );
    
    iTimer = CGlxBubbleTimer::NewL(this);
    
    iMainVisual = CAlfAnchorLayout::AddNewL(*this);
    iMainVisual->SetFlag(EAlfVisualFlagManualLayout);
    iMainVisual->SetSize(TSize(KReqWidth, KGridHeight));
    iMainVisual->SetPos(TAlfRealPoint(KDummyPoint));
    
    CFbsBitmap* bitmap = AknsUtils::GetCachedBitmap(
            AknsUtils::SkinInstance(), KAknsIIDQsnFrPopupCenter);
    CleanupStack::PushL(bitmap);
    
    CAlfTexture& backgroundAvkonTexture = 
		iUiUtility->GlxTextureManager().CreateColorAvkonIconTextureL( 
				KAknsIIDQsnFrPopupCenter, bitmap->Handle(), KRgbTransparent) ;
    
    CleanupStack::Pop(bitmap);
    
    // BackGround Border Image Visual
    iBackgroundBorderImageVisual = CAlfImageVisual::AddNewL(*this,iMainVisual);
    iBackgroundBorderImageVisual->SetImage(TAlfImage(backgroundAvkonTexture));

    // Create a new 3x1 grid layout visual.
    iGrid = CAlfGridLayout::AddNewL(*this, KNoOfColumns, KNumofMenuItems , 
			iMainVisual);//columns, rows

    //Finally create the menu list that will appear in screen
    CreateMenuListL(CreateFontL());
	ShowItemMenu(EFalse);
    }
// --------------------------------------------------------------------------- 
// ~CGlxTagsContextMenuControl()
// --------------------------------------------------------------------------- 
//
CGlxTagsContextMenuControl::~CGlxTagsContextMenuControl()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::CGlxTagsContextMenuControl");
    if (iMainVisual)
        {
        iMainVisual->RemoveAndDestroyAllD();
        iMainVisual = NULL;
        }
    if ( iTimer)
        {
        iTimer->Cancel();//cancels any outstanding requests
        delete iTimer;
        iTimer = NULL;
        }
    
    if (iUiUtility)
        {
        iUiUtility->Close();
        iUiUtility = NULL;
        }
    }
// --------------------------------------------------------------------------- 
// CreateFont()
// --------------------------------------------------------------------------- 
//
TInt CGlxTagsContextMenuControl::CreateFontL()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::CreateFont");
    
    // Create a new style based on the required font
    CAlfTextStyleManager& styleMan = iUiUtility->Env()->TextStyleManager();
    
    // remember its id for return later
    TInt id = styleMan.CreatePlatformTextStyleL(EAlfTextStyleNormal);
    
    // Get style. It is not owned
    CAlfTextStyle* style = styleMan.TextStyle(id);
    
    // Set this style to have required size and to be normal weight
    style->SetTextSizeInPixels(KTextSizeInPixels);
    style->SetBold(ETrue);
    
    return id;
    }

// --------------------------------------------------------------------------- 
// CreateMenuListL()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::CreateMenuListL(TInt aFontId)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::CreateMenuList");
    
    TRgb color;
    //Gets the color of the text specific to skin 
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(),
            color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
    
    //Loading the strings from rss
    HBufC* renameTitle = StringLoader::LoadLC( R_GLX_TAGS_RENAME_TITLE );
    HBufC* slideshowTitle = StringLoader::LoadLC( R_GLX_TAGS_SLIDESHOW_TITLE );
    HBufC* deleteTitle = StringLoader::LoadLC( R_GLX_TAGS_DELETE_TITLE );
    
    iSlideshowTextVisual = CAlfTextVisual::AddNewL(*this, iGrid);
    iSlideshowTextVisual->SetTagL(KTagSlideshow);
    iSlideshowTextVisual->SetTextStyle(aFontId);
    iSlideshowTextVisual->SetColor(color);
    iSlideshowTextVisual->SetTextL(*slideshowTitle);
    iSlideshowTextVisual->SetOffset(TAlfTimedPoint(10,0));
    iSlideshowTextVisual->SetAlign(EAlfAlignHLocale, EAlfAlignVCenter);
    
    iDeleteTextVisual = CAlfTextVisual::AddNewL(*this, iGrid);
    iDeleteTextVisual->SetTagL(KTagDelete);
    iDeleteTextVisual->SetTextStyle(aFontId);
    iDeleteTextVisual->SetColor(color);
    iDeleteTextVisual->SetTextL(*deleteTitle);
    iDeleteTextVisual->SetOffset(TAlfTimedPoint(10,0));
    iDeleteTextVisual->SetAlign(EAlfAlignHLocale, EAlfAlignVCenter);
    
    iRenameTextVisual = CAlfTextVisual::AddNewL(*this, iGrid);
    iRenameTextVisual->SetTagL(KTagRename);
    iRenameTextVisual->SetTextStyle(aFontId);
    iRenameTextVisual->SetColor(color);
    iRenameTextVisual->SetTextL(*renameTitle);
    iRenameTextVisual->SetOffset(TAlfTimedPoint(10,0));
    iRenameTextVisual->SetAlign(EAlfAlignHLocale, EAlfAlignVCenter);
    
    CleanupStack::PopAndDestroy(deleteTitle); // for deleteTitle
    CleanupStack::PopAndDestroy(slideshowTitle);// slideshowTitle
    CleanupStack::PopAndDestroy(renameTitle);// renameTitle
    }
// --------------------------------------------------------------------------- 
// SetDisplayL()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::SetDisplay(const TPoint& aPoint)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::SetDisplayL");
    //if already started , cancel it
    if(iTimer && iTimer->IsActive())
        {
        iTimer->Cancel();//cancels any outstanding requests
        }

    TInt upperYPos = aPoint.iY - KMinimalGap;
    TInt XPos = aPoint.iX ;
    
    //Preferred is to display in upper area
    TInt upperDisplayableHeight = upperYPos - iViewableRect.iTl.iY;
    TInt rightDisplayableWidth = iViewableRect.iBr.iY - XPos;

    //always draw above
    if(rightDisplayableWidth < KReqWidth)
        {
        XPos = aPoint.iX - KReqWidth;
        }
    if(upperDisplayableHeight < KGridHeight)
        {
        iMainVisual->SetPos(TAlfRealPoint(XPos , upperYPos ));
        }
    else
        {
        iMainVisual->SetPos(TAlfRealPoint(XPos , upperYPos - KGridHeight));
        }
    
    iTimer->Cancel();          //cancels any outstanding requests
    iTimer->SetDelay(KTimerDelay);
    }

// --------------------------------------------------------------------------- 
// ShowItemMenu()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::ShowItemMenu (TBool aShow)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::ShowItemMenu");
    if(aShow)
        {
        iMainVisual->SetOpacity(KOpacityOpaque);    
        iItemMenuVisibility = ETrue;
        }
    else
        {
        iMainVisual->SetOpacity(KOpacityTransparent);
        iItemMenuVisibility = EFalse;
        }
    }
// --------------------------------------------------------------------------- 
// OfferEventL()
// --------------------------------------------------------------------------- 
//
TBool CGlxTagsContextMenuControl::OfferEventL(const TAlfEvent& aEvent)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::OfferEventL");
    TBool consumed = EFalse;

    if(iItemMenuVisibility && aEvent.IsPointerEvent() && aEvent.PointerDown() )
        {
        CAlfVisual* onVisual = aEvent.Visual();
        TInt cmdId = KErrNotFound;
        if(AlfUtil::TagMatches(onVisual->Tag(), KTagSlideshow))
            {
            cmdId = EGlxCmdSlideshowPlay;
            }
        else if(AlfUtil::TagMatches(onVisual->Tag(), KTagDelete))
            {
            cmdId = EGlxCmdDelete;
            }
        else if(AlfUtil::TagMatches(onVisual->Tag(), KTagRename))
            {
            cmdId = EGlxCmdRename;
            }
        
        if(cmdId >= 0)
            {
            ShowItemMenu(EFalse);
            iItemMenuObserver.HandleGridMenuListL(cmdId);
            consumed = ETrue;
            }
        }
    
    return consumed;
    }

// ---------------------------------------------------------------------------
// TimerComplete()
// ---------------------------------------------------------------------------
//
void CGlxTagsContextMenuControl::TimerComplete()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::TimerComplete");
    ShowItemMenu(EFalse);
    }

// --------------------------------------------------------------------------- 
// SetViewableRect()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::SetViewableRect(TRect aRect)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::SetViewableRect");
    iViewableRect.SetRect(aRect.iTl.iX, aRect.iTl.iY, 
                                aRect.iBr.iX, aRect.iBr.iY);
    }

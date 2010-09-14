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
#include <alf/alfroster.h> // For CalfRoster
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfbrusharray.h>
#include <alf/alftextstyle.h>
#include <alf/alfenv.h>
#include <alf/alfeventhandler.h>
#include <alf/alfutil.h>
#include <alf/alftexture.h>
#include <alf/alfcurvepath.h>
#include <alf/alflinevisual.h>
#include <alf/alfevent.h>
#include <alf/alfdisplay.h>
#include <alf/alfframebrush.h>

#include <StringLoader.h>
#include <touchfeedback.h>

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
//Number of menu items present in stylus menu
const TInt KNumofMenuItems = 3;
//Number of columns present in grid control showing menu  
const TInt KNoOfColumns = 1;
//Highest possible value to make control opaque
const TReal KOpacityOpaque = 1.0;
//lowest possible value to make control completely transparent
const TReal KOpacityTransparent = 0.0;
const TPoint KDummyPoint(500, 500);
//6 Seconds delay for menu control visibility on screen 
const TInt KTimerDelay = 6000000;
//Control complete height
const TInt KGridHeight = ((KReqHeightPerMenuItem * KNumofMenuItems) + (KMinimalGap * (KNumofMenuItems + 1)));
//Text size for menu items
const TInt KTextSizeInPixels = 20;
//X shrink factor for stylus menu border to be drawn/visible
const TInt KShrinkXCoord = 5;
//Y shrink factor for stylus menu border to be drawn/visible
const TInt KShrinkYCoord = 5;
//Padding value for Minimum width for control
const TInt KWidthPadding = 30;
//Padding value for Minimum spacing for line separators
const TInt KLinePadding = 5;
//Padding value for right margin
const TInt KRightMargin = 10;
//Context menu separator line thickness value
const TReal KSeparatorLineThickness = 0.2;
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
    CGlxTagsContextMenuControl* self = CGlxTagsContextMenuControl::NewLC(
            aItemMenuObserver);
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
    CGlxTagsContextMenuControl* self =
            new (ELeave) CGlxTagsContextMenuControl(aItemMenuObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// --------------------------------------------------------------------------- 
// CGlxTagsContextMenuControl()
// --------------------------------------------------------------------------- 
//
CGlxTagsContextMenuControl::CGlxTagsContextMenuControl(
        MGlxItemMenuObserver& aItemMenuObserver) :
    iItemMenuObserver(aItemMenuObserver), iCommandId(KErrNotFound)
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
    CGlxUiUtility* utility = CGlxUiUtility::UtilityL();
    CleanupClosePushL( *utility );
    iAlfEnv = utility->Env();
    CleanupStack::PopAndDestroy( utility );
    CAlfControl::ConstructL(*iAlfEnv);
    
    iTimer = CGlxBubbleTimer::NewL(this);
    
    iMainVisual = CAlfAnchorLayout::AddNewL(*this);
    iMainVisual->SetFlag(EAlfVisualFlagManualLayout);
    iMainVisual->SetPos(TAlfRealPoint(KDummyPoint));
    // Create a new 3x1 grid layout visual.
    iGrid = CAlfGridLayout::AddNewL(*this, KNoOfColumns, KNumofMenuItems,
            iMainVisual);//columns, rows
    iGrid->SetFlag(EAlfVisualFlagManualLayout);
    //Finally create the menu list that will appear in screen
    CreateMenuListL(CreateFontL());
    CalculateMaxWidth();
    iMainVisual->SetSize(TSize(KWidthPadding + iMaxTextWidth, KGridHeight));
    iGrid->SetInnerPadding(TPoint(KLinePadding,KLinePadding));
    const TRect gridSize(TPoint(KLinePadding,KLinePadding),TSize(KTextSizeInPixels + iMaxTextWidth, KGridHeight - 10));
    iGrid->SetRect(TAlfRealRect(gridSize));
    
    iMainVisual->EnableBrushesL(ETrue);

    TRect outerRect(TRect(TPoint(KDummyPoint),
                    TSize(KWidthPadding + iMaxTextWidth, KGridHeight)));
    TRect innerRect(outerRect);
    innerRect.Shrink(KShrinkXCoord, KShrinkYCoord);

    CAlfFrameBrush* frameBrush = CAlfFrameBrush::NewLC(*iAlfEnv,
                                        KAknsIIDQsnFrPopupSub);
    frameBrush->SetFrameRectsL(innerRect, outerRect);

    iMainVisual->Brushes()->AppendL(frameBrush, EAlfHasOwnership);
    CleanupStack::Pop(frameBrush);

    DrawLineSeparatorsL();
	ShowItemMenuL(EFalse);
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
    }
// --------------------------------------------------------------------------- 
// CreateFont()
// --------------------------------------------------------------------------- 
//
TInt CGlxTagsContextMenuControl::CreateFontL()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::CreateFont");
    
    // Create a new style based on the required font
    CAlfTextStyleManager& styleMan = iAlfEnv->TextStyleManager();
    
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
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG20);
    
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
    TInt xPos = aPoint.iX;
    
    //always draw above
    if ((iViewableRect.iBr.iX - (xPos + KRightMargin + KWidthPadding))
            < iMaxTextWidth)
        {
        xPos = aPoint.iX - iMaxTextWidth;
        }

    if (upperYPos + KGridHeight > iViewableRect.iBr.iY)
        {
        upperYPos = iViewableRect.iBr.iY - KGridHeight;
        }

    iMainVisual->SetPos(TAlfRealPoint(xPos, upperYPos));
    
	if ( iTimer)
	    {
		iTimer->Cancel();          //cancels any outstanding requests
	    iTimer->SetDelay(KTimerDelay);
		}
    }

// --------------------------------------------------------------------------- 
// ShowItemMenuL()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::ShowItemMenuL(TBool aShow)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::ShowItemMenuL");
    if(aShow)
        {
        iMainVisual->SetOpacity(KOpacityOpaque);    
        iItemMenuVisibility = ETrue;
        }
    else
        {
        iMainVisual->SetOpacity(KOpacityTransparent);
        iRenameTextVisual->EnableBrushesL(EFalse);
        iDeleteTextVisual->EnableBrushesL(EFalse);
        iSlideshowTextVisual->EnableBrushesL(EFalse);
        iItemMenuVisibility = EFalse;
        iMainVisual->SetPos(TAlfRealPoint(KDummyPoint));
        }
    }

// --------------------------------------------------------------------------- 
// ItemMenuVisibility()
// --------------------------------------------------------------------------- 
//
TBool CGlxTagsContextMenuControl::ItemMenuVisibility()
    {
    return iItemMenuVisibility;
    }

// --------------------------------------------------------------------------- 
// OfferEventL()
// --------------------------------------------------------------------------- 
//
TBool CGlxTagsContextMenuControl::OfferEventL(const TAlfEvent& aEvent)
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::OfferEventL");
    TBool consumed = EFalse;

    if (aEvent.IsPointerEvent() && iItemMenuVisibility )
        {
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if (aEvent.PointerDown())
            {
            iCommandId = KErrNotFound;
            Display()->Roster().SetPointerEventObservers(
                    EAlfPointerEventReportDrag
                            + EAlfPointerEventReportLongTap
                            + EAlfPointerEventReportUnhandled, *this);
            if (iItemMenuVisibility)
                {
                CAlfVisual* onVisual = aEvent.Visual();
                CAlfFrameBrush* brush = CAlfFrameBrush::NewLC(*iAlfEnv,
                        KAknsIIDQsnFrList);
                TRect textVisualRect = TRect(TPoint(
                        iSlideshowTextVisual->Pos().IntValueNow()), TSize(
                        iSlideshowTextVisual->Size().ValueNow().AsSize()));
                TRect innerRect(textVisualRect);
                innerRect.Shrink(KShrinkXCoord, KShrinkYCoord);
                brush->SetFrameRectsL(innerRect, textVisualRect);
                iTimer->Cancel(); //cancels any outstanding requests
                if (AlfUtil::TagMatches(onVisual->Tag(), KTagSlideshow))
                    {
                    iSlideshowTextVisual->EnableBrushesL(ETrue);
                    iSlideshowTextVisual->Brushes()->AppendL(brush,
                            EAlfHasOwnership);

                    iCommandId = EGlxCmdSlideshowPlay;
                    }
                else if (AlfUtil::TagMatches(onVisual->Tag(), KTagDelete))
                    {
                    iDeleteTextVisual->EnableBrushesL(ETrue);
                    iDeleteTextVisual->Brushes()->AppendL(brush,
                            EAlfHasOwnership);

                    iCommandId = EGlxCmdDelete;
                    }
                else if (AlfUtil::TagMatches(onVisual->Tag(), KTagRename))
                    {
                    iRenameTextVisual->EnableBrushesL(ETrue);
                    iRenameTextVisual->Brushes()->AppendL(brush,
                            EAlfHasOwnership);

                    iCommandId = EGlxCmdRename;
                    }
                consumed = ETrue;
                if (feedback)
                    {
                    feedback->InstantFeedback(ETouchFeedbackBasic);
                    }
                CleanupStack::Pop(brush);
                }//End of iItemMenuVisibility check
            }//End of Pointer down event 
        else if (aEvent.PointerUp())
            {
            if (iCommandId >= 0)
                {
                TBool eventInsideControl = HitTest(
                        aEvent.PointerEvent().iParentPosition);
                //If Up event is received only within Menu control, handle the command
                if (eventInsideControl)
                    {
                    ShowItemMenuL(EFalse);

                    iItemMenuObserver.HandleGridMenuListL(iCommandId);
                    iCommandId = KErrNotFound;
                    }
                else
                    {
                    HandleUpEventL();
                    }
                }
            consumed = ETrue;
            }
        else if (aEvent.PointerEvent().EDrag)
            {
            TBool eventInsideControl = HitTest(
                    aEvent.PointerEvent().iParentPosition);
            if (!eventInsideControl)
                {
                HandleUpEventL();
                }
            else if (eventInsideControl && feedback)
                {
                feedback->InstantFeedback(ETouchFeedbackBasic);
                }
            consumed = ETrue;
            }
        consumed = ETrue;
        }
    return consumed;
    }

// ---------------------------------------------------------------------------
// TimerCompleteL()
// ---------------------------------------------------------------------------
//
void CGlxTagsContextMenuControl::TimerCompleteL()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::TimerCompleteL");
    ShowItemMenuL(EFalse);
    iItemMenuObserver.HandleGridMenuListL();
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
// --------------------------------------------------------------------------- 
// CalculateMaxWidth()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::CalculateMaxWidth()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::CalculateMaxWidth");

    iMaxTextWidth
            = (iSlideshowTextVisual->TextExtents().iWidth
                    > iRenameTextVisual->TextExtents().iWidth
                              ? iSlideshowTextVisual->TextExtents().iWidth
                                 : iRenameTextVisual->TextExtents().iWidth);

    iMaxTextWidth
            = (iMaxTextWidth > iDeleteTextVisual->TextExtents().iWidth
                              ? iMaxTextWidth
                                 : iDeleteTextVisual->TextExtents().iWidth);

    }
// --------------------------------------------------------------------------- 
// HandleUpEventL()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::HandleUpEventL()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::HandleUpEventL");
    if(iCommandId >=0 && iItemMenuVisibility)
        {
        iCommandId = KErrNotFound;
        
        iRenameTextVisual->EnableBrushesL(EFalse);
        iDeleteTextVisual->EnableBrushesL(EFalse);
        iSlideshowTextVisual->EnableBrushesL(EFalse);

        iTimer->SetDelay(KTimerDelay);
        }
    }
// --------------------------------------------------------------------------- 
// DrawLineSeparatorsL()
// --------------------------------------------------------------------------- 
//
void CGlxTagsContextMenuControl::DrawLineSeparatorsL()
    {
    TRACER("GLX_CLOUD::CGlxTagsContextMenuControl::DrawLineSeparators");
    TRgb color;
    //Gets the color of the line specific to skin 
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), color,
            KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG11);

    for (TInt i = 1; i < KNumofMenuItems; i++)
        {
        CAlfCurvePath* curvePath = CAlfCurvePath::NewLC(*iAlfEnv);
        curvePath->AppendArcL(TPoint(KLinePadding, ((KReqHeightPerMenuItem
                * i) + (KLinePadding * i + 2))), TSize(), 0, 0, 0);
        curvePath->AppendLineL(TPoint(KLinePadding, ((KReqHeightPerMenuItem
                * i) + (KLinePadding * i + 2))), TPoint(
                iMainVisual->DisplayRect().Width() - KLinePadding,
                ((KReqHeightPerMenuItem * i) + (KLinePadding * i + 2))), 0);

        CAlfLineVisual* line = CAlfLineVisual::AddNewL(*this, iMainVisual);
        line->SetPath(curvePath, EAlfHasOwnership);
        line->SetThickness(KSeparatorLineThickness);
        line->SetColor(color);
        line->SetFlag(EAlfVisualFlagIgnorePointer);

        CleanupStack::Pop(curvePath);
        }
    }

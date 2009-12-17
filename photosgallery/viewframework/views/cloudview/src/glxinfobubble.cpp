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
* Description:    Info Bubble source file
 *
*/



//Includes
#include "glxinfobubble.h" //header file

//AlfT classes
#include <alf/alftextstylemanager.h> // For CAlfTextStyleManager
#include <alf/alftextstyle.h> // For CAlfTextStyle
#include <alf/alfanchorlayout.h> // For CAlfAnchorLayout
#include <alf/alfdecklayout.h> // For CAlfDeckLayout
#include <alf/alfimagevisual.h> // For CAlfImageVisual
#include <alf/alftextvisual.h> // For CAlfTextVisual
#include <alf/alfborderbrush.h> // For CAlfBorderBrush
#include <alf/alfbrusharray.h> // For CAlfBrushArray


#include <AknUtils.h> //main pane rect
#include <glxicons.mbg> //gallery icons file
#include <glxtracer.h>	
#include <glxlog.h>	// For Logs
#include <data_caging_path_literals.hrh> 
#include <e32math.h> //math functions
#include <glxgeneraluiutilities.h> 
#include <glxuiutility.h>  // For CGlxUiUtilities
#include <glxtexturemanager.h> // For CGlxTextureManager


const TInt KGlxBubbleMinimumWidth(22);
const TInt KGlxBubbleWidthOffset(65);
const TInt KGlxBubbleThumbnailOffset(55);
const TInt KGlxBubblePosOffset(17);
const TInt KGlxBubbleHeight(75);
const TInt KGlxBubblePosPreOffset( 5 );
const TInt KGlxBubbleTextHeightPos(25);
const TInt KGlxBubbleMaxExcluder( 78 ); // Max Space to be excluded to form the biggest bubble.
const TInt KGlxBubbleRightStrip(5);


_LIT(KCloudViewMifFile, "glxicons.mif");
 


// ---------------------------------------------------------------------------
// Default C++ Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxInfoBubble::CGlxInfoBubble(): 
CAlfControl()
	{
	TRACER("GLX_CLOUD::CGlxInfoBubble::CGlxInfoBubble");
	}

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxInfoBubble::BaseConstructL(CAlfControl &aOwnerControl, CAlfEnv &aEnv)
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::BaseConstructL");
	CAlfControl::ConstructL(aEnv);
	iUiUtility = CGlxUiUtility::UtilityL();
	iUiUtility->AddSkinChangeObserverL(*this);
	iInfoDeck = CAlfDeckLayout::AddNewL (aOwnerControl); //deck layout is the root layout
	//adding deck layout to anchor layout
	iInfoDeck->SetFlags (EAlfVisualFlagManualSize);
	iInfoDeck->SetFlags (EAlfVisualFlagManualPosition);
	iInfoBubbleContainer = CAlfAnchorLayout::AddNewL (*this, iInfoDeck); //item container anchor layout
	iInfoBubble = CAlfAnchorLayout::AddNewL (*this, iInfoDeck); //anchor layout for bubble

	//InfoBubbleContainer associated items
	iDeckCentreimage = CAlfImageVisual::AddNewL (*this, iInfoBubbleContainer);
	iBubbleTextTitle = CAlfTextVisual::AddNewL (*this, iInfoBubbleContainer);
    iBubbleTextTitle->SetFlag( EAlfVisualFlagManualSize );
	iBubbleTextTitle->SetWrapping( CAlfTextVisual::ELineWrapTruncate );	
	iBubbleTextSubTitle = CAlfTextVisual::AddNewL (*this, iInfoBubbleContainer);

	//InfoBubble components
	iBubbleLeft= CAlfImageVisual::AddNewL (*this); //Visual for left arc of Bubble
	iBubbleRight= CAlfImageVisual::AddNewL (*this); //Visual for rightarc of Bubble
	iBubbleTailStrip= CAlfImageVisual::AddNewL (*this); //Visual for strip that connects to tail of Bubble
	iBubbleTail= CAlfImageVisual::AddNewL (*this); //Visual for tail of Bubble
	iTempTitleText = CAlfTextVisual::AddNewL (*this);//temporary visual for storing tilte

	//load common components for bubble
	iMifFile=KDC_APP_BITMAP_DIR;
	iMifFile.Append(KCloudViewMifFile);
	iImageBubbleLeft = iUiUtility->GlxTextureManager().CreateIconTextureL( 
		EMbmGlxiconsQgn_graf_mediatag_bubble_side_l, iMifFile);
	iImageBubbleRight = iUiUtility->GlxTextureManager().CreateIconTextureL(
		EMbmGlxiconsQgn_graf_mediatag_side_r, iMifFile);
	iImageBubbleExpand = iUiUtility->GlxTextureManager().CreateIconTextureL(
		EMbmGlxiconsQgn_graf_mediatag_bubble_center, iMifFile);

	iThumbnailBorderBrush = CAlfBorderBrush::NewL(aEnv,2, 2, 0, 0);
	
	//set the text properties
	//fetching the s60 font styles
	TInt iTypefaceSecondaryStyleId = CGlxInfoBubble::Env().TextStyleManager().CreatePlatformTextStyleL 
		(EAknLogicalFontSecondaryFont, EAlfTextStyleNormal);
	TInt iTypefacePrimaryStyleId = CGlxInfoBubble::Env().TextStyleManager().CreatePlatformTextStyleL 
		(EAknLogicalFontPrimaryFont, EAlfTextStyleNormal);
	CAlfTextStyle* stylePrimary = CGlxInfoBubble::Env().TextStyleManager().TextStyle
		(iTypefacePrimaryStyleId);
	CAlfTextStyle* styleSecondary = CGlxInfoBubble::Env().TextStyleManager().TextStyle 
		(iTypefaceSecondaryStyleId);
	styleSecondary->SetTextSizeInPixels (14, 0);
	stylePrimary->SetTextSizeInPixels (21, 0);

	iBubbleTextTitle->SetTextStyle (iTypefacePrimaryStyleId);
	
	TAlfTimedValue opacity;
    opacity.SetTarget(1, 500);
    iBubbleTextTitle->SetOpacity(opacity);
    
//	iBubbleTextTitle->iOpacity.Set (1);
	iBubbleTextTitle->SetColor (KRgbBlack);

	iBubbleTextSubTitle->SetTextStyle (iTypefaceSecondaryStyleId);
	
    opacity.SetTarget(1, 500);
    iBubbleTextSubTitle->SetOpacity(opacity);
    
//	iBubbleTextSubTitle->iOpacity.Set (1);
	iBubbleTextSubTitle->SetColor (KRgbBlack);

	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxInfoBubble::~CGlxInfoBubble()
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::~CGlxInfoBubble");
	if ( iThumbnailBorderBrush)
		{
		delete iThumbnailBorderBrush;
		}
	ResetBubbleComponent();
	iBubbleExpandStrip.ResetAndDestroy ();
	iBubbleExpandStrip.Close ();
	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::ResetBubbleComponent -iBubbleExpandStrip.Count()(%d)", iBubbleExpandStrip.Count());
	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::ResetBubbleComponent -iInfoBubble->Count()(%d)", iInfoBubble->Count());
	iUiUtility->RemoveSkinChangeObserver(*this);
	if ( iUiUtility)
		{
		iUiUtility->Close ();
		}
	}

// ---------------------------------------------------------------------------
// DisappearBubble()
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxInfoBubble::DisappearBubble()
	{
	TRACER("GLX_CLOUD::CGlxInfoBubble::DisappearBubble");
    TAlfTimedValue opacity;
    opacity.SetTarget(0, 500);
    iInfoDeck->SetOpacity(opacity);
	}
	
// ---------------------------------------------------------------------------
// CreateThumbnailTextureL()
// ---------------------------------------------------------------------------
//	
EXPORT_C CAlfTexture& CGlxInfoBubble::CreateThumbnailTextureL(const TGlxMedia& aMedia,
        const TGlxIdSpaceId& aIdSpaceId,const TSize& aSize )
    {
    TRACER("GLX_CLOUD::CGlxInfoBubble::CreateThumbnailTextureL");
    return iUiUtility->GlxTextureManager().CreateThumbnailTextureL(aMedia,
        aIdSpaceId, aSize, this );    
    }
    
void CGlxInfoBubble::ResetBubbleComponent()
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::ResetBubbleComponent");
	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::ResetBubbleComponent -iBubbleExpandStrip.Count()(%d)", iBubbleExpandStrip.Count());
	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::ResetBubbleComponent -iInfoBubble->Count()(%d)", iInfoBubble->Count());
	iBubbleExpandStrip.ResetAndDestroy ();
	if ( iInfoBubble->Count ()!=0)
		{
		iInfoBubble->Remove (iBubbleLeft);
		iInfoBubble->Remove (iBubbleTailStrip);
		iInfoBubble->Remove (iBubbleRight);
		iInfoBubble->Remove (iBubbleTail);

		iInfoBubble->UpdateChildrenLayout (); //update layout
		}
	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::ResetBubbleComponent -iBubbleExpandStrip.Count()(%d)", iBubbleExpandStrip.Count());
	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::ResetBubbleComponent -iInfoBubble->Count()(%d)", iInfoBubble->Count());

	}

void CGlxInfoBubble::LoadTailAssemblyL(TInt aTailEnumId, TInt aTailStripEnumId)
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::LoadTailAssemblyL");
	iImageBubbleTail = iUiUtility->GlxTextureManager().CreateIconTextureL (aTailEnumId, iMifFile);
	iImageBubbleStrip =iUiUtility->GlxTextureManager().CreateIconTextureL (aTailStripEnumId, iMifFile);
	iBubbleTailStrip->SetImage (iImageBubbleStrip);
	iBubbleTail->SetImage (iImageBubbleTail);

	}

void CGlxInfoBubble::CreateItemContainerLayoutL()
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::CreateItemContainerLayoutL");

	iDeckCentreimage->EnableBrushesL ();
	//Ownership of the brush is not transferred to the visual.
	iDeckCentreimage->Brushes()->AppendL (iThumbnailBorderBrush, EAlfDoesNotHaveOwnership);
 
    TAlfTimedValue opacity;
    opacity.SetValueNow(1); // immediate change
    iDeckCentreimage->SetOpacity(opacity);

    SetThumbnailBorderColor();
	iThumbnailBorderBrush->SetLayer (EAlfBrushLayerForeground);
    if( GlxGeneralUiUtilities::LayoutIsMirrored () )
        {	   
        TInt bublepos = iBubbleSize.iWidth-60;
        
        iInfoBubbleContainer->SetAnchor (EAlfAnchorTopLeft, 0,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            bublepos, iPositionOffset+ 5));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorBottomRight, 0,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            bublepos+50, iPositionOffset + 55));       

        iInfoBubbleContainer->SetAnchor (EAlfAnchorTopLeft, 1,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            5, iPositionOffset + 5));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorBottomRight, 1,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            5+ iBubbleTextTitle->TextExtents().iWidth,
            iPositionOffset+iBubbleTextTitle->TextExtents().iHeight+5));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorTopLeft, 2,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            5, iPositionOffset+iBubbleTextTitle->TextExtents().iHeight +10));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorBottomRight, 2,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            5+ iBubbleTextSubTitle->TextExtents().iWidth,
            iPositionOffset+iBubbleTextTitle->TextExtents().iHeight +10+
            iBubbleTextSubTitle->TextExtents().iHeight));
        }

    else
        {       
        //setting anchors 
        iInfoBubbleContainer->SetAnchor (EAlfAnchorTopLeft, 0,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            5, iPositionOffset+ 5));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorBottomRight, 0,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            55, iPositionOffset + 55));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorTopLeft, 1,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            60, iPositionOffset + 5));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorBottomRight, 1,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            60+ iBubbleTextTitle->TextExtents().iWidth,
            iPositionOffset+iBubbleTextTitle->TextExtents().iHeight+5));

        iInfoBubbleContainer->SetAnchor (EAlfAnchorTopLeft, 2,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            65, iPositionOffset+iBubbleTextTitle->TextExtents().iHeight +10));
            
        iInfoBubbleContainer->SetAnchor (EAlfAnchorBottomRight, 2,
            EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
            65+ iBubbleTextSubTitle->TextExtents().iWidth,
            iPositionOffset+iBubbleTextTitle->TextExtents().iHeight +10+
            iBubbleTextSubTitle->TextExtents().iHeight));				

        }
	}

void CGlxInfoBubble::SetVariableVisualCountL()
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::SetVariableVisualCountL");
    TInt stripCount( 0 );    
    TInt tempTitleLength = iTempTitleText->TextExtents().iWidth; 
    TInt tempPos = (iPos.iX>KGlxBubbleWidthOffset?KGlxBubbleThumbnailOffset:iPos.iX-25);
	if ( tempTitleLength > (iBubbleTextSubTitle->TextExtents().iWidth))
		{
		stripCount = tempTitleLength;
		}
	else
		{
		stripCount = iBubbleTextSubTitle->TextExtents().iWidth;
		}
		
    if ( stripCount>iScreenWidth/2 - 15 )
	    {
     	stripCount = ( ( iScreenWidth - iPos.iX > stripCount+KGlxBubbleThumbnailOffset ) ? stripCount :
     	                                                     stripCount-iPos.iX +tempPos );
     	                                                     
	    }
	// Bubbles stripCount should be a maximum of Screenwidth - KGlxBubbleMaxExcluder
	stripCount = Min( stripCount, iScreenWidth - KGlxBubbleMaxExcluder );
	
	//set the tag size
	TAlfRealSize tagSize( stripCount, KGlxBubbleTextHeightPos );
	iBubbleTextTitle->SetSize( tagSize, 0 );

	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::DrawBubbleExtemsibleComponent stripCount  %d ",stripCount);
	//set the deck layout size
	iBubbleSize.iHeight = KGlxBubbleHeight; //height is constant
	iBubbleSize.iWidth = KGlxBubbleWidthOffset + stripCount + KGlxBubbleRightStrip; //KGlxBubbleRightStrip (5) is for right strip
	iInfoDeck->SetSize (iBubbleSize);
	TReal variableWidth = (KGlxBubbleWidthOffset + stripCount) - KGlxBubbleMinimumWidth;
	TReal slotsToFill =variableWidth/12.0;
	Math::Round (iRoudedSlotValue, slotsToFill, 0);
	}
// ---------------------------------------------------------------------------
// UpdateTextureL()
// ---------------------------------------------------------------------------
//	
EXPORT_C void CGlxInfoBubble::UpdateTextureL(CAlfTexture& aTexture)
    {  
    iDeckCentreimage->SetImage(TAlfImage(aTexture));
	// Fix for BUG EDKZ-77UKMZ( The Image is cropped to fit in the info bubble thumbnail space )
    iDeckCentreimage->SetScaleMode( CAlfImageVisual::EScaleCover );
    iDeckCentreimage->SetFlag( EAlfVisualFlagClipping );
	return;
    }
// ---------------------------------------------------------------------------
// ResetImage()
// ---------------------------------------------------------------------------
//	
EXPORT_C void CGlxInfoBubble::ResetImage()
    { 
    TAlfImage image;
    iDeckCentreimage->SetImage(image);
    }
    
// ---------------------------------------------------------------------------
// DisplayBubble()
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxInfoBubble::DisplayBubbleL(TPoint aPos, CAlfTexture& aTexture, const TDesC
		&aTitle, const TDesC &aSubTitle)
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::DisplayBubble");
	iPos = aPos; //bubble draw point
	ResetBubbleComponent();

	//assign the params
	//setting the texts value and image brush boundary
	iBubbleTextTitle->SetTextL(aTitle);
	//to-check
	//iBubbleTextTitle->SetChanged();
	iBubbleTextSubTitle->SetTextL(aSubTitle);
	_LIT(KNullDesc,"");
	iTempTitleText->SetTextL(KNullDesc());
	iTempTitleText->SetTextL(aTitle);
	
	//loading the texture for thumbanil image		
    UpdateTextureL(aTexture);
    //to-check
	//iInfoDeck->Changed();
	TRect rect;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
	iScreenWidth = rect.Width();
	iScreenHeight = rect.Height();
	
	//determing the info bubble posotion
	TInt dx (iPos.iY);
	TInt dy (iPos.iX);
    TInt rx = iScreenHeight - dx; //remaining distance form x axis
    TInt ry = iScreenWidth - dy;//remaining distance form y axis

	//sets the variable Visual count
	SetVariableVisualCountL();

	//Deciding bubble positions
	if ((ry >= dy) && (rx >= dx))
	//down- towards right
		{
		iPositionOffset=15;
		LoadTailAssemblyL(EMbmGlxiconsQgn_graf_mediatag_bubble_corner_tl,EMbmGlxiconsQgn_graf_mediatag_bubble_side_b);
		DrawBubbleFirstQuadL(aPos);

		}
	if ((ry < dy) && (rx > dx))
	//down- towards left
		{
		iPositionOffset=15;
		LoadTailAssemblyL(EMbmGlxiconsQgn_graf_mediatag_bubble_corner_tr,EMbmGlxiconsQgn_graf_mediatag_bubble_side_b);
		DrawBubbleSecondQuadL(aPos);
		}
	if ((ry >= dy) && (rx <= dx))
	//up-  towards right
		{
		iPositionOffset=0;
		LoadTailAssemblyL(EMbmGlxiconsQgn_graf_mediatag_bubble_corner_bl,EMbmGlxiconsQgn_graf_mediatag_bubble_side_t);
		DrawBubbleThirdQuadL(aPos);
		}
	if ((ry < dy) && (rx < dx))
	//up- towards left
		{
		iPositionOffset=0;
		LoadTailAssemblyL(EMbmGlxiconsQgn_graf_mediatag_bubble_corner_br,EMbmGlxiconsQgn_graf_mediatag_bubble_side_t);
		DrawBubbleFourthQuadL(aPos);
		}
//to-check
//	iInfoBubble->SetChanged();
	
	//all texture loaded
	//now find the width of bubblle and create layout based on these visuals.
	//optimize logic for tail allignment towards centre of the focussed item  
	TInt bubblewidth = iInfoDeck->Size().iX.Target();
	
	// make sure that bubble does not get outside the screen
	// first check the left hand side
	if( GlxGeneralUiUtilities::LayoutIsMirrored () )
	    {
	   	if( iBubbleRectDrawPoint.iX < 0 )
    	    {
    	    // set bubble to be as left as possible
    	    iBubbleRectDrawPoint.iX = 13;
    	    }
    	    
    	// then check the right hand side
    	if( ( iBubbleRectDrawPoint.iX + bubblewidth ) > iScreenWidth )
    	    {
    	    // set bubble to be as left as possible
    	    iBubbleRectDrawPoint.iX = iScreenWidth - bubblewidth+5;
    	    } 
	    }
	else
	    {	   
    	if( iBubbleRectDrawPoint.iX < 0 )
    	    {
    	    // set bubble to be as left as possible
	        iBubbleRectDrawPoint.iX = 0;
	        }
    // then check the right hand side
    	if( ( iBubbleRectDrawPoint.iX + bubblewidth ) > iScreenWidth )
    	    {
    	    // set bubble to be as left as possible
    	    iBubbleRectDrawPoint.iX = iScreenWidth - bubblewidth;
    	    }
	    }
	 
	TAlfTimedValue opacity;
    opacity.SetTarget(0.9, 500);
    iInfoBubble->SetOpacity(opacity);
        
	//iInfoBubble->iOpacity.Set(0.9, 1000);
	iInfoDeck->SetPos(iBubbleRectDrawPoint);
	CreateItemContainerLayoutL();

	//Setting properies for Layouts
	iInfoDeck->MoveToFront();
	
//	TAlfTimedValue opacity;
    opacity.SetTarget(0, 500);
    iInfoDeck->SetOpacity(opacity);
    
//	iInfoDeck->iOpacity.Set(0);
	
	opacity.SetTarget(1.0, 800);
	iInfoDeck->SetOpacity(opacity);
	//iInfoDeck->iOpacity.Set(1.0,800);
	
	opacity.SetTarget(1.0, 500);
	iInfoBubble->SetOpacity(opacity);
	//iInfoBubble->iOpacity.Set(1.0);
	
	iInfoBubbleContainer->MoveToFront();
	//to-check
   //	iInfoDeck->SetChanged();

	/// @todo would need to reposition the tail as well somehow!!!

	}

void CGlxInfoBubble::DrawBubbleFirstQuadL(TPoint aReferencepos)
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::DrawBubble");

	//determining the position of tail and setting postion for bubble rect
	if ( (aReferencepos.iX - KGlxBubblePosOffset) >= 5)
		{
		iBubbleRectDrawPoint.iX= aReferencepos.iX - KGlxBubblePosOffset;
		iTailNormalPosition=1;
		}
	else
		if ( (aReferencepos.iX - KGlxBubblePosPreOffset) >= 5)
			{
			iBubbleRectDrawPoint.iX= aReferencepos.iX - KGlxBubblePosPreOffset;
			iTailNormalPosition=0;
			}
	iBubbleRectDrawPoint.iY = aReferencepos.iY;

	//drawing bubble
	iBubbleComponentCount=0;
	iBubbleXaxisInc=0;

	DrawBubbleLeftComponentL ();
	if ( iTailNormalPosition ==1)
		{
		//draw tail:
		iInfoBubble->Append (iBubbleTail);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (17, 1));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (29, 16));
		iBubbleComponentCount+=1;
		//tail end
		DrawBubbleExtensibleComponentL (1);
		DrawBubbleTailComponentL ();
		DrawBubbleExtensibleComponentL (iRoudedSlotValue -1);
		}
	else
		{
		DrawBubbleTailComponentL ();
		DrawBubbleExtensibleComponentL (iRoudedSlotValue);
		//draw tail:
		iInfoBubble->Append (iBubbleTail);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (5, 1));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (17, 16));
		iBubbleComponentCount+=1;
		//tail end
		}
	DrawBubbleRightComponentL ();

	}

void CGlxInfoBubble::DrawBubbleSecondQuadL(TPoint aReferencepos)
	{

	//determining the position of tail and setting postion for bubble rect
	if ( (aReferencepos.iX + KGlxBubblePosOffset) <= iScreenWidth)
		{
		iBubbleRectDrawPoint.iX= aReferencepos.iX -(iBubbleSize.iWidth - KGlxBubblePosOffset);
		iTailNormalPosition=1;
		}
	else
		if ( (aReferencepos.iX + KGlxBubblePosPreOffset) <= iScreenWidth)
			{
			iBubbleRectDrawPoint.iX= aReferencepos.iX - (iBubbleSize.iWidth -KGlxBubblePosPreOffset);
			iTailNormalPosition=0;
			}
	iBubbleRectDrawPoint.iY = aReferencepos.iY;

	//drawing bubble
	iBubbleComponentCount=0;
	iBubbleXaxisInc=0;
	DrawBubbleLeftComponentL ();
	TInt xCord=KGlxBubbleMinimumWidth + (iRoudedSlotValue*12);
	xCord -=29;
	if ( iTailNormalPosition ==1)
		{
		//draw tail:
		iInfoBubble->Append (iBubbleTail);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (xCord, 1));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (xCord+12, 16));
		iBubbleComponentCount+=1;
		//tail end
		DrawBubbleExtensibleComponentL (iRoudedSlotValue-1);
		DrawBubbleTailComponentL ();
		DrawBubbleExtensibleComponentL (1);
		}
	else
		{
		DrawBubbleExtensibleComponentL (iRoudedSlotValue);
		DrawBubbleTailComponentL ();
		//draw tail:
		iInfoBubble->Append (iBubbleTail);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (xCord+12, 1));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (xCord+24, 16));
		iBubbleComponentCount+=1;
		//tail end
		}
	DrawBubbleRightComponentL ();

	}

void CGlxInfoBubble::DrawBubbleThirdQuadL(TPoint aReferencepos)
	{

	//determining the position of tail and setting postion for bubble rect
	if ( (aReferencepos.iX - KGlxBubblePosOffset) >= 5)
		{
		iBubbleRectDrawPoint.iX= aReferencepos.iX - KGlxBubblePosOffset;
		iTailNormalPosition=1;
		}
	else
		if ( (aReferencepos.iX - KGlxBubblePosPreOffset) >= 5)
			{
			iBubbleRectDrawPoint.iX= aReferencepos.iX - KGlxBubblePosPreOffset;
			iTailNormalPosition=0;
			}
	iBubbleRectDrawPoint.iY = aReferencepos.iY -KGlxBubbleHeight;
	//drawing bubble
	iBubbleComponentCount=0;
	iBubbleXaxisInc=0;
	DrawBubbleLeftComponentL ();
	if ( iTailNormalPosition ==1)
		{
		//draw tail:
		iInfoBubble->Append (iBubbleTail);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (17, 60));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (29, 75));
		iBubbleComponentCount+=1;
		//tail end
		DrawBubbleExtensibleComponentL (1);
		DrawBubbleTailComponentL ();
		DrawBubbleExtensibleComponentL (iRoudedSlotValue -1);

		}
	else
		{
		DrawBubbleTailComponentL ();
		DrawBubbleExtensibleComponentL (iRoudedSlotValue);
		//draw tail:
		iInfoBubble->Append (iBubbleTail);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (5, 60));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (17, 75));
		iBubbleComponentCount+=1;
		//tail end
		}
	DrawBubbleRightComponentL ();
	}

void CGlxInfoBubble::DrawBubbleFourthQuadL(TPoint aReferencepos)
	{
	//determining the position of tail and setting postion for bubble rect

	if ( (aReferencepos.iX + KGlxBubblePosOffset) <= iScreenWidth)
		{
		iBubbleRectDrawPoint.iX= aReferencepos.iX -(iBubbleSize.iWidth - KGlxBubblePosOffset);
		iTailNormalPosition=1;
		}
	else
		if ( (aReferencepos.iX + KGlxBubblePosPreOffset) <= iScreenWidth)
			{
			iBubbleRectDrawPoint.iX= aReferencepos.iX - (iBubbleSize.iWidth -KGlxBubblePosPreOffset);
			iTailNormalPosition=0;
			}
	iBubbleRectDrawPoint.iY = aReferencepos.iY -KGlxBubbleHeight;

	//drawing bubble
	iBubbleComponentCount=0;
	iBubbleXaxisInc=0;
	DrawBubbleLeftComponentL ();
	TInt xCord=KGlxBubbleMinimumWidth + (iRoudedSlotValue*12);
	xCord -=29;
	if ( iTailNormalPosition ==1)
		{
		//draw tail:
		iInfoBubble->Append (iBubbleTail);

		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
						xCord, 60));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
						xCord+12, 75));
		iBubbleComponentCount+=1;
		//tail end
		DrawBubbleExtensibleComponentL (iRoudedSlotValue-1);
		DrawBubbleTailComponentL ();
		DrawBubbleExtensibleComponentL (1);
		}
	else
		{
		DrawBubbleExtensibleComponentL (iRoudedSlotValue);
		DrawBubbleTailComponentL ();
		//draw tail:
		iInfoBubble->Append (iBubbleTail);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (xCord+12, 60));
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (xCord+24, 75) );
		iBubbleComponentCount+=1;
		//tail end
		}
	DrawBubbleRightComponentL ();

	}

void CGlxInfoBubble::DrawBubbleLeftComponentL()
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::DrawBubbleLeftComponent");
	iInfoBubble->Append (iBubbleLeft);
	iBubbleLeft->SetImage (iImageBubbleLeft);
	iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
			EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
			EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
					iBubbleXaxisInc, iPositionOffset));
	iBubbleXaxisInc += 5;
	iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
			EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
			EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
					iBubbleXaxisInc, iPositionOffset+60));
	iBubbleComponentCount+=1;

	}
void CGlxInfoBubble::DrawBubbleRightComponentL()
	{
	TRACER("GLX_CLOUD::CGlxInfoBubble::DrawBubbleRightComponent");

	iInfoBubble->Append (iBubbleRight);
	iBubbleRight->SetImage (iImageBubbleRight);
	iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
			EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
			EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
					iBubbleXaxisInc, iPositionOffset));
	iBubbleXaxisInc+=5;
	iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
			EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
			EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
					iBubbleXaxisInc, iPositionOffset+60));
	iBubbleComponentCount+=1;

	}
void CGlxInfoBubble::DrawBubbleExtensibleComponentL(TInt aSlots)
	{

	TRACER("GLX_CLOUD::CGlxInfoBubble::DrawBubbleExtemsibleComponent");
	GLX_LOG_INFO1("GLX_CLOUD::CGlxInfoBubble::DrawBubbleExtemsibleComponent slots  %d ",
			aSlots);
	for (TInt j=0; j<aSlots;j++)
		{
		CAlfImageVisual *tempimagevisual=CAlfImageVisual::AddNewL (*this);
		iBubbleExpandStrip.AppendL (tempimagevisual);
		this->Remove (tempimagevisual);
		tempimagevisual->SetImage (iImageBubbleExpand); //image visual that will be  used multiple times
		iInfoBubble->Append (tempimagevisual);
		iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (iBubbleXaxisInc, iPositionOffset));
		iBubbleXaxisInc+=12;
		iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
				EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
				EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
				TAlfTimedPoint (iBubbleXaxisInc, iPositionOffset+60));
		iBubbleComponentCount+=1;
		}
	}
	
void CGlxInfoBubble::DrawBubbleTailComponentL()
	{
	TRACER("GLX_CLOUD::CGlxInfoBubble::DrawBubbleTailComponent");

	iInfoBubble->Append (iBubbleTailStrip);
	iInfoBubble->SetAnchor (EAlfAnchorTopLeft, iBubbleComponentCount,
			EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
			EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
					iBubbleXaxisInc, iPositionOffset));
	iBubbleXaxisInc+=12;
	iInfoBubble->SetAnchor (EAlfAnchorBottomRight, iBubbleComponentCount,
			EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
			EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute, TAlfTimedPoint (
					iBubbleXaxisInc, iPositionOffset+60));
	iBubbleComponentCount+=1;

	}

// ---------------------------------------------------------------------------
// SetThumbnailBorderColor()
// ---------------------------------------------------------------------------
//	
void CGlxInfoBubble::SetThumbnailBorderColor()
    {   
	TRgb brushColor;
	//Get the cached color
	AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), brushColor, KAknsIIDQsnHighlightColors,EAknsCIQsnHighlightColorsCG3);
	iThumbnailBorderBrush->SetColor (brushColor);
    }

// ---------------------------------------------------------------------------
// HandleSkinChanged()
// ---------------------------------------------------------------------------
//
void CGlxInfoBubble::HandleSkinChanged()
    {
    SetThumbnailBorderColor();
    }

// ---------------------------------------------------------------------------
// TextureContentChangedL
// ---------------------------------------------------------------------------
//
void CGlxInfoBubble::TextureContentChangedL( TBool /*aHasContent*/ , CAlfTexture* /*aNewTexture*/)
    {
    }
    

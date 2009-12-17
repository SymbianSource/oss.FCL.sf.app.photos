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
* Description:    Info Bubble Implementation
 *
*/




#ifndef C__GLXINFOBUBBLE_H__
#define C__GLXINFOBUBBLE_H__

//Includes

#include <alf/alfcontrol.h>  //base class for Alf control
#include <alf/alfimage.h>  //for TAlfImage
#include <mglxskinchangeobserver.h> //for interface MGlxSkinChangeObserver
#include <glxmedia.h>

#include <mglxtextureobserver.h>

//Forward Declaration
class CGlxUiUtility;
class CAlfAnchorLayout;
class CAlfBorderBrush;
class CAlfDeckLayout;
class CAlfTexture;
class CAlfImageVisual;
class CAlfTextVisual;

/**
 * CGlxInfoBubble is a common calss based on Alf Control.
 *It draws a bitmap and two lines on text.
 *It has a position logic of its own.
 */
class CGlxInfoBubble : public CAlfControl,public MGlxSkinChangeObserver,
    public MGlxTextureObserver
	{
public:

	/* Constructors. */

	/** 	
	 *  @Default c++ contructor
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aEnv - Env Variable of Alf
	 */
	IMPORT_C CGlxInfoBubble();

	/** 	
	 *  @function ConstructL
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aOwnerControl - Owner control which will be the parent of Infobubble
	 *  @param aEnv - Env Variable of Alf
	 */
	IMPORT_C void BaseConstructL(CAlfControl &aOwnerControl, CAlfEnv &aEnv);
	
	/**
	 * Destructor.
	 */
	IMPORT_C virtual ~CGlxInfoBubble();

protected:

	/** 	
	 *  @function DisplayBubbleL
	 *  @since S60 3.2
	 *  @param aPos - Position of Visual for which InfoBubble will be shown
	 *  @param aTexture - Thumbnail texture associated with an item
	 *  @param aTitle - title of a tag
	 *  @param aSubTitle - subtitle of a tag
	 */	
	IMPORT_C void DisplayBubbleL(TPoint aPos, CAlfTexture& aTexture,
    	const TDesC& aTitle, const TDesC& aSubTitle);	
	
	/** 	
	 *  @function UpdateTextureL
	 *  @since S60 3.2
	 *  @param aTexture - AlfTexture associated with an item thumbnail
	 */
	IMPORT_C void UpdateTextureL(CAlfTexture& aTexture);
	
	
	/** 	
	 *  @function DisapperBubble
	 *  Animates when bubble becomes out of context
	 *  @since S60 3.2
	 */
	IMPORT_C void DisappearBubble();
	
	/** 	
	 *  @function CreateThumbnailTextureL
	 *  creates the thumbnailtexture
	 *  @param aMedia,media
	 *  @param aIdSpaceId,Id-space identifier
	 *  @param aSize,thumbnail size
	 */
	IMPORT_C CAlfTexture& CreateThumbnailTextureL(const TGlxMedia& aMedia,
        const TGlxIdSpaceId& aIdSpaceId,const TSize& aSize);
        
    /** 	
	 *  @function ResetImage()
	 *  Resets the image 	 
	 */       
    IMPORT_C void ResetImage();

private: // From MGlxTextureObserver
    void TextureContentChangedL( TBool aHasContent, CAlfTexture* aNewTexture );

private:

	/**   
	 *  @function DrawBubbleFirstQuad
	 *  @since S60 3.2
	 *  @param aReferencepos - reference postion for tail
	 *  @param aQuadrantId - determines the bubble type
	 */
	void DrawBubbleFirstQuadL(TPoint aReferencepos);
	
	
	/**   
	 *  @function DrawBubbleSecondQuad
	 *  @since S60 3.2
	 *  @param aReferencepos - reference postion for tail
	 *  @param aQuadrantId - determines the bubble type
	 */
	void DrawBubbleSecondQuadL(TPoint aReferencepos);
	
	
	/**   
	 *  @function DrawBubbleThirdQuad
	 *  @since S60 3.2
	 *  @param aReferencepos - reference postion for tail
	 *  @param aQuadrantId - determines the bubble type
	 */
	void DrawBubbleThirdQuadL(TPoint aReferencepos);
	

	/**   
	 *  @function DrawBubbleFourthQuad
	 *  @since S60 3.2
	 *  @param aReferencepos - reference postion for tail
	 *  @param aQuadrantId - determines the bubble type
	 */
	void DrawBubbleFourthQuadL(TPoint aReferencepos);
	
	
	/**   
	 *  @function DrawBubbleLeftComponent
	 *  @since S60 3.2
	 */
	void DrawBubbleLeftComponentL();
	
	
	/**   
	 *  @function DrawBubbleRightComponent
	 *  @since S60 3.2
	 */
	void DrawBubbleRightComponentL();
	
	
	/**   
	 *  @function DrawBubbleExtemsibleComponent
	 *  @since S60 3.2
	 *  @param aQuadrantId - determines the bubble type
	 */
	void DrawBubbleExtensibleComponentL(TInt aSlots);
	
	
	/**   
	 *  @function DrawBubbleTailComponent()
	 *  @since S60 3.2
	 */
	void DrawBubbleTailComponentL();
	
	
	/**   
	 *  @function ResetBubbleComponent()
	 *  @since S60 3.2
	 */
	void ResetBubbleComponent();
	
	
	/**   
	 *  @function LoadTailAssembly()
	 *  @since S60 3.2
	 */
	void LoadTailAssemblyL(TInt aTailEnumId, TInt aTailStripEnumId);
	
	void CreateItemContainerLayoutL();
	
	void SetVariableVisualCountL();
	
	//from MGlxSkinChangeObserver
	void HandleSkinChanged();
	
    /**   
	 *  @function SetThumbnailBorderColor()
	 *  @since S60 3.2
	 *  Sets the color for thumbnail border
	 */
	void SetThumbnailBorderColor();

protected:
	/**UI Utility  */
	CGlxUiUtility* iUiUtility;
	
private:
	//Enum for deciding the location of bubble
	enum TGlxBubbleQuadrantFlags
		{
		EQuadrantFirst = 0,
		EQuadrantSecond ,
		EQuadrantThird,
		EQuadrantFourth
		};
		
	/**Anchor layout for Bubble container */
	CAlfAnchorLayout *iInfoBubbleContainer;
	
	/**Anchor layout for Bubble */
	CAlfAnchorLayout *iInfoBubble;
	
	/**deck layout which is base layout for Bubble */
	CAlfDeckLayout *iInfoDeck;
	
	/**Visual for centre Image of Bubble */
	CAlfImageVisual *iDeckCentreimage;
	
	/**Visual for first line of text in Bubble */
	CAlfTextVisual *iBubbleTextTitle;
	
	/**Visual for second line of text in Bubble */
	CAlfTextVisual *iBubbleTextSubTitle;
	
	/**Image variable of bubble components */
	TAlfImage iImageThumb, iImageBubbleLeft, iImageBubbleRight;
	TAlfImage iImageBubbleStrip, iImageBubbleExpand, iImageBubbleTail;
			
	/**Border brush */
	CAlfBorderBrush *iThumbnailBorderBrush;
	
	/**Visual  array for expandable middle bubble component */
	RPointerArray <CAlfImageVisual> iBubbleExpandStrip; //
	
	/**Visual for left arc of Bubble */
	CAlfImageVisual *iBubbleLeft;
	
	/**Visual for rightarc of Bubble */
	CAlfImageVisual *iBubbleRight;
	
	/**Visual for strip that connects to tail of Bubble */
	CAlfImageVisual *iBubbleTailStrip;
	
	/**Visual for tail of Bubble */
	CAlfImageVisual *iBubbleTail;
	
	/**number of times centre bubble image is needed */
	TReal iRoudedSlotValue;
	
	/**flag to decide iInfoBubbleContainer position */
	TInt iTailPositionUp;
	
	/**flag to indicate whether tail will be drawn on normal position or not */
	TInt iTailNormalPosition;
	
	/**point from which the drawing of bubble will start */
	TPoint iBubbleRectDrawPoint;
	
	/**Storing screen width */
	TInt iScreenWidth;
	
	/** storing screen height*/
	TInt iScreenHeight;
	
	/**offset for bubble drawing */
	TInt iPositionOffset;
	
	/** Stores Size of bubble*/
	TSize iBubbleSize;
	
	/**stores counter of bubble component drawn */
	TInt iBubbleComponentCount;
	
	/**Increment variable for X axis */
	TPoint iPos;
	
	TInt iBubbleXaxisInc;
	
	//mif file
	TFileName iMifFile;
	CAlfTexture *iTexture;

	/**temporary object to store Bubble Text*/
	CAlfTextVisual *iTempTitleText;
	};

#endif // C__GLXINFOBUBBLE_H__


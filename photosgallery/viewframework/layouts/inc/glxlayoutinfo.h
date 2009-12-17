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
* Description:    Layout base classes
*
*/




#ifndef __GLXLAYOUTINFO_H__
#define __GLXLAYOUTINFO_H__

#include <e32std.h>
#include <alf/alftimedvalue.h>
// FORWARD DECLARATIONS
class CAlfVisual;
class TAlfTimedValue ;
/**
 * TGlxScreenInfo
 * This class contains information about the screen, the layout may utilize these when laying out the visuals
 */
 //@TODO : This class has to removed. 
 //The views get the screen info from - iScreenSize, TGlxLayoutInfo
NONSHARABLE_CLASS( TGlxScreenInfo )
	{
	public:

		/**
		 * Contructor, resets the variables
		 */
		IMPORT_C TGlxScreenInfo();

	public:

		/**
		 * Width of the display
		 */
		TInt iWidth;
		
		/**
		 * Height of the display
		 */
		TInt iHeight;

		/**
		 * Orientation of the display
		 */
		enum TOrientation { EPortrait, ELandscape } iOrientation;

	};

/**
 * TGlxLayoutInfo.
 * This class contains the fields that the layouts can modify in order to 
 * get the visuals drawn the way they want.
 */
NONSHARABLE_CLASS( TGlxLayoutInfo )
	{
	
	public:

		/**
		 * Mapped index, use this to get the index relative to focus
		 */
		TReal32 iMappedIndex;
		/**
		 * Display size
		 */
		TSize iScreenSize;
		/**
		 * Visual size
		 */
		TSize iVisualSize;
		/**
		 * Index of the visual
		 */
		inline TInt Index() const;
		/**
		 * The visual that the layout is targeted to
		 */
		inline CAlfVisual& Visual();

	protected:

		/**
		 * Constructor, resets the state
		 * @param the visual
		 */
		TGlxLayoutInfo( CAlfVisual* aVisual );

		/// Own: the index
		TInt iIndex;

		/// Ref: the visual
		CAlfVisual* iVisual;

	};

/**
 * TGlxLayoutInfoResetter
 * Modifiable version of the layout info
 * get the visuals drawn the way they want.
 */
NONSHARABLE_CLASS( TGlxLayoutInfoResetter ) : public TGlxLayoutInfo
	{
	public:
	
		/**
		 * Constructor, resets the state
		 * This one sets the visual as NULL, use SetVisual to
		 * update the visual
		 */
		IMPORT_C TGlxLayoutInfoResetter();

		/**
		 * Constructor, resets the state
		 * @param the target visual
		 */
		IMPORT_C TGlxLayoutInfoResetter( CAlfVisual* aVisual );

		/**
		 * Set the visual
		 * @param the target visual
		 */
		IMPORT_C void SetVisual( CAlfVisual* aVisual );

		/**
		 * Set the screen size
		 * @param the screen size
		 */
		IMPORT_C void SetScreenSize( TSize aSize );
		/* Reset, Sets the index and cleas all values.
		 * iIndex and iMappedIndex are set to aIndex, 
		 * other values are set to 0, except iTextureBottomRight
		 * which is set to (1,1)
		 * @param aIndex, the index for this item
		 */
		IMPORT_C void Reset( TInt aIndex );

	};

#include "glxlayoutinfo.inl"
	
#endif // __GLXLAYOUTINFO_H__


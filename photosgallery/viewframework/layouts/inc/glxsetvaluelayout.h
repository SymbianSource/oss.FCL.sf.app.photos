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
* Description:    Set value layout
*
*/




#ifndef T_GLXSETVALUELAYOUT_H
#define T_GLXSETVALUELAYOUT_H

#include <glxlayout.h>
#include <alf/alftimedvalue.h>

/**
 *  TGlxSetValueLayout
 *  
 *  Sets value of specific layout attribute(s)
 *
 *  @lib glxlayouts.lib
 */
class TGlxSetValueLayout : public TGlxLayout
	{
	public:

		/**
		 * Constructor
		 */
		IMPORT_C TGlxSetValueLayout();

		/**
		 * Destructor
		 */
		IMPORT_C ~TGlxSetValueLayout();

		/**
		 * Set the value. This variant sets the value for opacity and rotation
		 * value is (TReal)
		 * This method can only be used for setting EOpacity or ERotation
		 * @param aTarget the value you wish the visual(s) to have
		 * @param aMilliseconds the duration for the change
		 */
		IMPORT_C void SetOpacity( TReal32 aTarget, TInt aMilliseconds );

		/**
		 * Set the value. This variant sets the value for size and position 
		 * value is (TReal, TReal)
		 * This method can only be used for setting ESize or EPosition
		 * @param aTargetX the value for x
		 * @param aTargetY the value for y
		 * @param aMilliseconds the duration for the change
		 */
		IMPORT_C void SetSize( 
			TReal32 aTargetWidth, TReal32 aTargetHeight, TInt aMilliseconds );

		/**
		 * Set the position
		 * @param aTargetX the value for x
		 * @param aTargetY the value for y
		 * @param aMilliseconds the duration for the change
		 */
		IMPORT_C void SetPosition( 
			TReal32 aTargetX, TReal32 aTargetY, TInt aMilliseconds );

	protected:	// From TGlxLayout

		/// @ref TGlxLayout::DoSetLayoutValues
		void DoSetLayoutValues( TGlxLayoutInfo& aInfo );
	
	private:

	    /// Own: the value for opacity
	    TAlfTimedValue iOpacityValue;
	    /// Own: the value for size
	    TAlfTimedPoint iSizeValue;
	    /// Own: the value for position
	    TAlfTimedPoint iPositionValue;
	    /// Own: flag to tell which layout values need to be changed
	    TInt iValuesToChange;

	};
	
#endif // T_GLXSETVALUELAYOUT_H

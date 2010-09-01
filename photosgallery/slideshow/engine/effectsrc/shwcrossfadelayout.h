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
* Description:   Layout that performs cross fade
 *
*/



#ifndef __SHWCROSSFADELAYOUT_H__
#define __SHWCROSSFADELAYOUT_H__

#include <glxlayout.h>

#include "shwtimedvalue.h"

// FORWARD DECLARATIONS
class CHuiCurvePath;

/**
 * TShwCrossFadeLayout. 
 * Layout that handles crossfade effect.
 * @author Kimmo Hoikka
 * @lib shwslideshowengine.lib
 * @internal reviewed 07/06/2007 by Loughlin
 */
NONSHARABLE_CLASS( TShwCrossFadeLayout ) : public TGlxLayout
	{
	public:

		/**
		 * Constructor, initializes the layout.
		 */
		TShwCrossFadeLayout();

		/**
		 * Destructor
		 */
		~TShwCrossFadeLayout();

		/**
		 * Specifies the opacity value
		 * @param aOpacity the desired opacity target
		 * @param aDuration the target time for opacity
		 */
		void Set( TReal32 aOpacity, TInt aDuration = 0 );

		/**
		 * This method pauses zoom and pan
		 */
		void Pause();

		/**
		 * This method resumes zoom and pan after pause.
		 */
		void Resume();

	protected:	// From TGlxLayout

		/// @ref TGlxLayout::DoSetLayoutValues
		void DoSetLayoutValues( TGlxLayoutInfo& aInfo );
		/// @ref TGlxLayout::DoChanged
		TBool DoChanged() const;
		/// @ref TGlxLayout::DoClearChanged
		void DoClearChanged();

	private:	// Data and private parts

		/// Own: the opacity
		TShwTimedValue iOpacity;

	};

#endif // __SHWCROSSFADELAYOUT_H__

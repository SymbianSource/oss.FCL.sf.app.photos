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
* Description:    Layout to mapping function adapter class
*
*/




#ifndef T_GLXLAYOUTHELPER_H
#define T_GLXLAYOUTHELPER_H

// INCLUDE DEPENDENCIES
#include <e32std.h>
#include <glxlayoutinfo.h>	// TGlxLayoutInfoResetter

// FORWARD DECLARES
class MGlxLayout;

/**
 * TGlxLayoutHelper
 *  
 * Sets the layout values from MGlxLayout and TGlxLayoutInfo
 * to a CHuiVisual.
 *
 * @lib glxlayouts.lib
 */
class TGlxLayoutHelper
	{
	public:

		/**
		 * Constructor
		 * @param aVisual the visual to layout
		 */
		IMPORT_C TGlxLayoutHelper( CAlfVisual& aVisual );

		/**
		 * Destructor.
		 */
		IMPORT_C ~TGlxLayoutHelper();

		/**
		 * This method should be called when the layout needs to be updated
		 * @param aIndex the current index for the visual this helper is mapped
		 * @param aScreenSize the screen size, passed on to the layout
		 */
		IMPORT_C void LayoutVisual( TInt aIndex, TSize aScreenSize );
		/**
         * This method should be called when the Panning (Drag)layout needs to be updated
         * @param aIndex the current index for the visual this helper is mapped
         * @param aScreenSize the screen size, passed on to the layout
         */
        IMPORT_C void LayoutPanVisual( TInt aIndex, TSize aScreenSize );
		
		/**
		 * Sets the layout chain to this adapter.
		 * @param aLayout layout to use
		 */
		IMPORT_C void SetLayout( MGlxLayout* aLayout );
		/**
		 * Gets the layout updated Size.
		 */
		IMPORT_C TSize GetUpdatedVisualSize( );
		/**
         * Gets the layout information.
		 */
        IMPORT_C TGlxLayoutInfo& GetUpdatedLayoutInfo( );

	private:

		/// Own: The layout info
		TGlxLayoutInfoResetter iLayoutInfo;
		/// Ref: The visual
		CAlfVisual& iVisual;
		/// Ref: The layout 
		MGlxLayout* iLayout;

	};
	
#endif // T_GLXLAYOUTHELPER_H


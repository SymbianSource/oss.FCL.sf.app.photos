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
* Description:    Layout blender
*
*/




#ifndef C_GLXBLENDLAYOUTBLEND_H
#define C_GLXBLENDLAYOUTBLEND_H

#include <glxlayout.h>

/**
 *  TGlxBlendLayout 
 *
 *  Blender of multiple MGlxLayouts based on coefficients
 *
 *  @lib glxlayouts.lib
 */
 
//@TODO : Class wont be used - To be removed 
class TGlxBlendLayout : public TGlxLayout
	{
	public:

		/**
		 * Constructor, reset the values
		 */
		IMPORT_C TGlxBlendLayout();

		/**
		 * Destructor
		 */
		IMPORT_C ~TGlxBlendLayout();
		
		/**
		 * Add a layout to be blended
		 * @param aLayout the layout to add, cannot be NULL
		 */
		IMPORT_C void AddLayoutL( MGlxLayout* aLayout );

		/**
		 * Remove an existing layout
		 * @param aLayout the layout to remove
		 */
		IMPORT_C void RemoveLayout( const MGlxLayout* aLayout );

		/**
		 * Remove all existing layouts
		 */
		IMPORT_C void RemoveAllLayouts();

	protected:	// From TGlxLayout

		/// @ref TGlxLayout::DoSetLayoutValues
		void DoSetLayoutValues( TGlxLayoutInfo& aInfo );
		/// @ref TGlxLayout::DoChanged
		TBool DoChanged() const;
		/// @ref TGlxLayout::DoClearChanged
		void DoClearChanged();

	private:

		/**
		 * Helper struct to store the layout and layout info
		 */
		struct TLayout
			{
			/// Ref: the layout
			MGlxLayout* iLayout;
			/// Own: the layoutinfo for the layout
			TGlxLayoutInfoResetter iLayoutInfo;
			/**
			 * Helper to tell whether two TLayout structs match
			 */
			static TBool Match( const TLayout& aLhs, const TLayout& aRhs );
			};
		/// Own: array of cached layout infos
		RArray< TLayout > iLayouts;

		/**
		 * Changed flag to remember whether any layouts have been added or removed
		 * since the las call to DoClearChanged
		 */
		TBool iChanged;
	};

#endif // C_GLXBLENDLAYOUTBLEND_H

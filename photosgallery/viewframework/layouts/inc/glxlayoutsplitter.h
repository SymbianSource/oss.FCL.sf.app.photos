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
* Description:    Splits layout chain by visual
*
*/




#ifndef T_GLXLAYOUTSPLITTER_H
#define T_GLXLAYOUTSPLITTER_H

#include <glxlayout.h>
#include <mglxvisuallistobserver.h>

class CAlfVisual;

/**
 *  TGlxLayoutSplitter
 *  
 *  Splits the layout chain based on a visual. Client can define
 *  any number of visual-layout pairs, and a default layout
 *  for the rest.
 *
 *  @lib glxlayouts.lib
 */
class TGlxLayoutSplitter : public TGlxLayout, public MGlxVisualListObserver
	{
	public:

		/**
		 * Constructor. Resets the values
		 */
		IMPORT_C TGlxLayoutSplitter();

		/**
		 * Destructor
		 */
		IMPORT_C ~TGlxLayoutSplitter();

		/**
		 * Sets the visual list for the splitter.
		 * Must be called before using the splitter
		 * @param aVisualList the visual list that this layout should deal with
		 */
		IMPORT_C void SetVisualListL( MGlxVisualList* aVisualList );
		
		/**
		 * Sets a layout that is bound to a visual
		 * @param aVisual the visual that this layout should deal with
		 */
		 
		 //Changed the API from AddLayout to SetLayout
		IMPORT_C void SetLayoutL(
			MGlxLayout* aLayout, const CAlfVisual* aVisual );

		/**
		 * Removes a layout that is bound to a visual
		 * @param aVisual the visual that this layout should deal with
		 */
		IMPORT_C void RemoveLayout( const CAlfVisual* aVisual );

		/**
		 * Sets the default layout. This layout is used when the 
		 * visual being evaluated is not linked to another layout 
		 * (by calling AddLayoutL). If default layout has not been 
		 * defined, the layout will fall through, i.e., values and 
		 * index will not be modified (for non-lined visuals).
		 * @param aLayout layout pointer or null if client want's to
		 *        undefine default layout.
		 */
		IMPORT_C void SetDefaultLayout( MGlxLayout* aLayout );

		/**
		 * Remove all layouts and remove association with visual list. 
		 */
		IMPORT_C void Reset();

	protected:	// From TGlxLayout

		/// @ref TGlxLayout::DoSetLayoutValues
		void DoSetLayoutValues( TGlxLayoutInfo& aInfo );

	public:	// From MGlxVisualListObserver

		/// @ref MGlxVisualListObserver::DoSetLayoutValues
		void HandleFocusChangedL(
			TInt aFocusIndex, TReal32 aItemsPerSecond, MGlxVisualList* aList, 
			NGlxListDefs::TFocusChangeType aType );
		/// @ref MGlxVisualListObserver::HandleSizeChanged
		void HandleSizeChanged( const TSize& aSize, MGlxVisualList* aList );
		/// @ref MGlxVisualListObserver::HandleVisualRemoved
		void HandleVisualRemoved(
			const CAlfVisual* aVisual, MGlxVisualList* aList );
		/// @ref MGlxVisualListObserver::HandleVisualAddedL
		void HandleVisualAddedL(
			CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList );

	private:

		// Helper fo get the layout that corresponds to given visual
		MGlxLayout* Layout( const CAlfVisual* aVisual ) const;
		// Helper fo get the layoutindex that corresponds to given visual
		TInt LayoutIndex( const CAlfVisual* aVisual ) const;

	private:
	
		/**
		 * Helper struct to store the layout and visual
		 */
		struct TLayout
			{
			/// Ref: the layout
			MGlxLayout* iLayout;
			/// Ref: the visual
			const CAlfVisual* iVisual;
			/**
			 * Helper to tell whether two TLayout structs match
			 */
			static TBool Match( const TLayout& aLhs, const TLayout& aRhs );
			};
		/// Own: the layout-visual structs
		RArray< TLayout > iLayouts;
		/// Ref: the default layout
		MGlxLayout* iDefaultLayout;
		/// Ref: the visual list
		MGlxVisualList* iVisualList;
		
	};

#endif // T_GLXLAYOUTSPLITTER_H

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
* Description:    Layout mapping index to distance from focus
*
*/




#ifndef T_GLXFOLLOWFOCUSLAYOUT_H
#define T_GLXFOLLOWFOCUSLAYOUT_H

// INCLUDES

#include <glxlayout.h>
#include <mglxvisuallistobserver.h>

// CLASS DECLARATION

/**
 *  TGlxFollowFocusLayout 
 *
 *  Layout mapping index to distance from focus
 *
 *  @lib glxlayouts.lib
 */
class TGlxFollowFocusLayout : public TGlxLayout, public MGlxVisualListObserver
    {
	public:
	
		/**
		 * Constructor, resets the state
		 */
	    IMPORT_C TGlxFollowFocusLayout();

		/**
		 * Destructor
		 */
		IMPORT_C virtual ~TGlxFollowFocusLayout();

		/**
		* Set visual list to which the layout is applied.
		*/
	    IMPORT_C void SetVisualListL( MGlxVisualList* aVisualList );

	protected:	// From TGlxLayout

		/// @ref TGlxLayout::DoSetLayoutValues
		void DoSetLayoutValues( TGlxLayoutInfo& aInfo );

	private:

	    /** Visual list to which the layout is applied (not owned) */
	    MGlxVisualList* iVisualList;

	    };

#endif // T_GLXFOLLOWFOCUSLAYOUT_H

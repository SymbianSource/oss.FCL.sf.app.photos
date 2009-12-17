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
* Description:    Set coefficient layout
*
*/




#ifndef T_GLXSETCOEFFICIENTLAYOUT_H
#define T_GLXSETCOEFFICIENTLAYOUT_H

#include <glxlayout.h>
#include <alf/alfTimedValue.h>

/**
 *  TGlxSetCoefficientLayout
 *  
 *  Resets the coefficient to a defined value.
 *  The value can be animated.
 *
 *  @lib glxlayouts.lib
 */
class TGlxSetCoefficientLayout : public TGlxLayout
	{
	public:

		/**
		 * Constructor.
		 */
		IMPORT_C TGlxSetCoefficientLayout();

		/**
		 * Destructor.
		 */
		IMPORT_C ~TGlxSetCoefficientLayout();

		/**
		 * Set layout type to which the coefficient is set
		 * @param aType the layout type, see @ref TGlxLayoutInfo::TLayoutType
		 */
		IMPORT_C void SetType( TGlxLayoutInfo::TLayoutType aType );

		/**
		 * SetCoefficient.
		 * @param aCoefficient the value to set
		 * @param aMilliseconds the target time for the coefficient value 
		 */
		IMPORT_C void SetCoefficient( TReal32 aCoefficient, TInt aMilliseconds );
		
	protected:	// From TGlxLayout

		/// @ref TGlxLayout::DoSetLayoutValues
		void DoSetLayoutValues( TGlxLayoutInfo& aInfo );
		/// @ref TGlxLayout::DoChanged
		TBool DoChanged() const;
		/// @ref TGlxLayout::DoClearChanged
		void DoClearChanged();

	private:

		/// Own: the type of the layout
		TGlxLayoutInfo::TLayoutType iType;
		/// Own: the timed value for the coefficient
	    TAlfTimedValue iCoefficient;

	};
	
#endif // T_GLXSETCOEFFICIENTLAYOUT_H

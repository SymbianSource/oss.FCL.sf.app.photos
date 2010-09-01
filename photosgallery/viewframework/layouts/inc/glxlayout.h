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




#ifndef __GLXLAYOUT_H__
#define __GLXLAYOUT_H__

#include <e32base.h>
#include <glxlayoutinfo.h>

/**
 * MGlxLayout
 *  
 * Layout base class. Layouts are objects that specify the 
 * renderable values, such as position, size or opacity for the visuals
 * They can be chained to form processing chains, and the chains
 * can also split into several chains (different chain for different 
 * visuals) and combine several chains into one. This allows building
 * complex visual effects by using simple layouts as building blocks.
 */
class MGlxLayout 
	{
	protected:
	
		/**
		 * Destructor. Dont allow deletion through this interface.
		 */
		~MGlxLayout() { }

	public:

		/**
		 * This method can specify the layout of given visual by setting 
		 * the corresponding fields in the layout info struct.
		 * @param aInfo the layout information to specify
		 */
		virtual void SetLayoutValues( TGlxLayoutInfo& aInfo ) = 0;

		/**
		 * Sets the next layout in the chain.
		 */
		virtual void SetNext( MGlxLayout* aLayout ) = 0;

		/**
		 * Returns the next layout in the chain.
		 */
		virtual MGlxLayout* Next() const = 0;

	};
	
/**
 * TGlxLayout
 *  
 * Implements the MGlxLayout interface and provides common layout chain functionality.
 *
 * @lib glxlayouts.lib
 */
class TGlxLayout : public MGlxLayout
	{
	public:

		/**
		 * Constructor, resets the object.
		 */
		IMPORT_C TGlxLayout();

		/**
		 * Destructor.
		 */
		IMPORT_C virtual ~TGlxLayout();

		/**
		 * Sets the next layout in the chain.
		 */
		IMPORT_C void SetNext( MGlxLayout* aLayout );

		/**
		 * Returns the next layout in the chain.
		 */
		IMPORT_C MGlxLayout* Next() const;

	public: 	// From MGlxLayout

		/// @ref MGlxLayout::SetLayoutValues
		IMPORT_C void SetLayoutValues( TGlxLayoutInfo& aInfo );

	protected:	// New API, template method pattern
	
		/**
		 * This method can specify the layout of given visual by setting the corresponding fields
		 * in the layout info struct.
		 * @param aInfo the layout information to specify
		 */
		virtual void DoSetLayoutValues( TGlxLayoutInfo& aInfo );

	private:

		/// Ref: The next layout in chain
		MGlxLayout* iNextLayout;
	
	public:
	
		__DEBUG_ONLY( TBuf<50> _iName ); 

	};

/**
 * CGlxLayout
 *  
 * Implements the MGlxLayout interface and provides common layout chain functionality.
 * This class is provided as a base class for CBase deriving layout classes.
 *
 * @lib glxlayouts.lib
 */
class CGlxLayout : public CBase, public MGlxLayout
	{
	public:

		/**
		 * Constructor, resets the object.
		 */
		IMPORT_C CGlxLayout();

		/**
		 * Destructor. Virtual by CBase and MGlxLayout
		 */
		IMPORT_C ~CGlxLayout();

		/**
		 * Sets the next layout in the chain.
		 */
		IMPORT_C void SetNext( MGlxLayout* aLayout );

		/**
		 * Returns the next layout in the chain.
		 */
		IMPORT_C MGlxLayout* Next() const;

	public: 	// From MGlxLayout

		/// @ref MGlxLayout::SetLayoutValues
		IMPORT_C void SetLayoutValues( TGlxLayoutInfo& aInfo );

	protected:	// New API, template method pattern
	
		/**
		 * This method can specify the layout of given visual by setting the corresponding fields
		 * in the layout info struct.
		 * @param aInfo the layout information to specify
		 */
		virtual void DoSetLayoutValues( TGlxLayoutInfo& aInfo );

	private:

		/// Own: the layout implementation
		TGlxLayout iLayoutImpl;
	
	public:
	
		__DEBUG_ONLY( TBuf<50> _iName ); 

	};
	
#endif // __GLXLAYOUT_H__


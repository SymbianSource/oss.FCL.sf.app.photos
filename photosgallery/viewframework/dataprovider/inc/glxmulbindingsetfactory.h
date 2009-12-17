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
* Description:    Binding Set Factory ;Base for all Binding Set
*
*/




#ifndef _GLXBINDINGSETFACTORY_H_
#define _GLXBINDINGSETFACTORY_H_

#include <e32base.h>                // Container Base Class
#include <mpxattribute.h>           // MPX Attribute data type class
#include <mul/imulwidget.h>
#include <mul/mulvisualitem.h>          // Client need to use this class to add data in data model
class CGlxBindingSet;               // basically holds the bindings required to populate the visual item.
class CMPXCollectionPath;           // Encapsulates a 'bookmark' to a specific collection entry
class MGlxBindingObserver;          // Observer for Bindings
class MGlxBoundCommand;             // Handles user commands
class TMPXAttribute;                // TMPXAttribute data type class.
class CGlxMulIconProvider;          // Icon details  

class TGlxMulBindingSetFactory 
    {
public:
	/**
	* CreateBindingSetL creates instance of Binding set 
	* @param aObserver Binding observer
	*/
	CGlxBindingSet* CreateBindingSetL( MGlxBindingObserver& aObserver ) const; 
    
protected: 
	/**
	 * Constructor
	 */
	IMPORT_C TGlxMulBindingSetFactory();
	
	/**
	 * Destructor
	 */
	IMPORT_C virtual ~TGlxMulBindingSetFactory();

	/**
	 * Creates String binding and adds it to binding set	
	 * @param aMulTag Template attribute of widget
	 * @param aString MPX constant string
	 */
	IMPORT_C void AddStringBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
	TInt aStringId ) const;
	
	/**
	 * Creates MPX Attribute binding and adds it to binding set	
	 * @param aMulTag Template attribute of widget
	 * @param aAttribute MPX attribute
	 */
	IMPORT_C void AddMpxAttributeBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
	const TMPXAttribute& aAttribute, TInt aFormatStringId = NULL ) const;
	
	/**
	 * Creates Thumbnail binding and adds it to binding set	
	 * @param aMulTag Template attribute of widget
	 */
	IMPORT_C void AddThumbnailBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
	const TSize& aSize ) const;
	
		/**
	 * Creates FullScreenThumbnail binding and adds it to binding set	
	 * @param aMulTag Template attribute of widget
	 */
	IMPORT_C void AddFullScreenThumbnailBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/ )const;
	
	/**
	 * Creates icon binding and adds it to binding set	
	 * @param aMulTag Template attribute of widget
	 */
	IMPORT_C void AddIconBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
        const CGlxMulIconProvider* aMulIcon, const TSize& aSize )const;
	
	/**
	 * Creates Container list binding and adds it to binding set	
	 * @param aMulTag Template attribute of widget
	 * @param aPath MPX collection path which will be used to fetch the attributes for media item
	 */
	IMPORT_C void AddContainerListBindingL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
	const CMPXCollectionPath& aPath ) const;
	
	/**
	 * Creates command binding and adds it to binding set	
	 * @param aCommand command associated with option menu item/items
	 */
	/** takes ownership of aCommand */
	IMPORT_C void AddCommandBindingL( MGlxBoundCommand* aCommand ) const; 
	
	/** @param aTemplateWhenFocused template to be used when item is focused.
	*         if not defined, uses aTemplate also for the focused item */
	IMPORT_C void AddTemplateBindingL(
	    Alf::mulwidget::TLogicalTemplate aTemplate, 
		Alf::mulwidget::TLogicalTemplate aTemplateWhenFocused = Alf::mulwidget::KTemplateUnknown) const;
	
	/**
	 * Creates preview thumbnail binding and adds it to binding set
	 */
	IMPORT_C void AddPreviewThumbnailBindingL() const;
    
private:
	virtual void AddBindingsL() const = 0;
    
private: 
	mutable CGlxBindingSet* iBindingSet; // not own
    };

#endif // _GLXBINDINGSETFACTORY_H_

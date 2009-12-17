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
* Description:    bindings required to populate the visual item
*
*/




#ifndef _GLXBINDINGSET_H_
#define _GLXBINDINGSET_H_

#include <e32base.h>                  // Container Base Class
#include "glxbinding.h"               // holds the bindings required to populate the visual item
#include "mglxbindingobserver.h"      // Notifies that binding has changed

class TGlxMedia;                      // media item in the item pool

namespace Alf
    {
    class MulVisualItem;              // Client need to use this class to add data in data model
    }

class CGlxAttributeRequirements;      // class will set the required attribute to the medialist

/**
 * CGlxBindingSet
 * The binding set is created by the binding set factory.
 * This basically holds the bindings required to populate the visual item.
 */
NONSHARABLE_CLASS( CGlxBindingSet ) : public CGlxBinding, public MGlxBindingObserver
    {
public:
	/**
	* Two phase construction
	* @param aObserver Observes binding
	*/
    static CGlxBindingSet* NewLC( MGlxBindingObserver& aObserver );
	
	/**
	*Destructor
	*/
    ~CGlxBindingSet();
	
 	/**
	* Adds the bindings to the binding set
	* @param aBinding The binding to be added to the binding set : owned
	*/
    void AddBindingL( CGlxBinding* aBinding );
    
	// From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
		TBool aIsFocused ) const;
    TBool HasRelevantAttributes( const RArray< TMPXAttribute >& aAttributes ) const;
    
    TResponse HandleFocusChanged( TBool aIsGained );
    
    void HandleItemChangedL(const CMPXCollectionPath& aPath );
    /**
	* Adds the fetch context to medialist
	* @param aAttributeRequirement The object that sets the attribute to medialist
	*/
    void AddRequirementsL( CGlxAttributeRequirements& aAttributeRequirement,TSize aSize);
    
private: 
	/**
	* Constructor
	* @param aObserver Observes binding
	*/
    CGlxBindingSet( MGlxBindingObserver& aObserver );
    
	// From MGlxBindingObserver
    void HandleBindingChanged( const CGlxBinding& aBinding );
    
private:
	// Binding Observer
    MGlxBindingObserver& iObserver;
	
	// Array of Binding-Bindingset
    RPointerArray< CGlxBinding > iBindings;
    
    };

#endif // _GLXBINDINGSET_H_

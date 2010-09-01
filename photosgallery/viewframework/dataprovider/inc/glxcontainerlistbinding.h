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
* Description:    CGlxContainerListBinding This class holds the list of 
*                 tags/albums associated with a media item
*
*/




#ifndef _GLXCONTAINERLISTBINDING_H_
#define _GLXCONTAINERLISTBINDING_H_

#include <e32base.h>                  // Container Base Class
#include <mpxattribute.h>             // lib mpxcommon.lib
#include <mglxmedialistobserver.h>    // Observes for changes in media list
#include "glxattributecontext.h"      // Fetch context to retrieve thumbnails
#include "glxbinding.h"               // holds the bindings required to populate the visual item
#include "glxustringconverter.h"      // class that converts the symbian types to UString type

class CMPXCollectionPath;             //  mpxcommon.lib
class CGlxAttributeContext;           // Fetch context to retrieve thumbnails
class MGlxMediaList;                  // lib glxmedialists.lib
class TGlxMedia;                      // media item in the item pool

namespace Alf
    {
    class IAlfVariantType;            // lib alfwidgetmodel.lib
    class MulVisualItem;              // Client need to use this class to add data in data model
    }

namespace osncore
    {
    class UString;                    // encapsulates utf-8 encoded string
    }
       
/**
 * CGlxContainerListBinding This class holds the list of tags/albums associated with a media item
 */
NONSHARABLE_CLASS( CGlxContainerListBinding ) : public CGlxSingleTagBinding, 
        public MGlxMediaListObserver
    {
public:

	/**
	 * NewL Two phase construction
	 */
	static CGlxContainerListBinding* NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
		const CMPXCollectionPath& aPath );
	
	/**
	 * NewLC Two phase construction
	 */
	static CGlxContainerListBinding* NewLC( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
		const CMPXCollectionPath& aPath );
		
	/**
	* Destructor
	*/	
	~CGlxContainerListBinding();

private:

	/**
	 * ConstructL Two phase construction
	 */
	void ConstructL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, const CMPXCollectionPath& aPath );

	// From MGlxMediaListObserver
	void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
	void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
	void HandleAttributesAvailableL( TInt aItemIndex, 
	    const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );    
	void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, 
        TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
	void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList ) ;
	void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList ) ;
	void HandleMediaL( TInt aListIndex, MGlxMediaList* aList ) ;
	void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );
	    
	// From CGlxBinding
	void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
								TBool aIsFocused, TInt aTextureId = KErrNotFound) const;

	/**
     * Create media list
     * @ param aMediaId media id for creating filtered media list
     */ 
	void CreateMediaListL(TGlxMediaId aMedia) const;
	
    /**
     * Close media list
     */ 
	void CloseMediaList();
	
	/**
	 * HandleFocusChanged
	 * @param aIsGained holds the focus change status
	 * @param aPath source path to the collection
	 */
    TResponse HandleFocusChanged( TBool aIsGained );
    
private:

	//Medialist that holds the list of tags/albums
	mutable MGlxMediaList* iContainerList;
	
	//Fetch context to get the attributes
	CGlxDefaultAttributeContext* iFetchContext;
	
	// collection path from which to fetch the attribute
	CMPXCollectionPath* iContainerPath;
	
	TGlxSequentialIterator iIterator;
	
    };
    
#endif //_GLXCONTAINERLISTBINDING_H_

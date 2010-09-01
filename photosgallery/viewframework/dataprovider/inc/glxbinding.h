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




#ifndef _GLXBINDING_H_
#define _GLXBINDING_H_

#include "glxattributerequirement.h"     // class will set the required attribute to the medialist
#include <glxattributecontext.h>         // Fetch context to retrieve thumbnails
#include <glxthumbnailcontext.h>         // Fetch context to retrieve thumbnails
#include <glxmedialistiterator.h>        // Interface for going through items in the list
#include <mpxattribute.h>                // lib mpxcommon.lib
#include <mglxmedialistobserver.h>       // Observes for changes in media list
#include <mglxfetchcontext.h>            // which attributes should be fetched
#include "glxuiutility.h"                // lib glxalfutils.lib
#include <mul/mulvisualitem.h>          // Client need to use this class to add data in data model
#include <mul/imulwidget.h>
// Forward Declaration
class CMPXCollectionPath;                // Encapsulates a 'bookmark' to a specific collection entry
class CGlxMulIconProvider;               // Icon details
class MGlxMediaList;                     // lib glxmedialists.lib
class MGlxBindingObserver;               // Notifies that binding has changed
class MGlxBoundCommand;                  // Handles user commands
class TGlxMedia;                         // media item in the item pool

namespace Alf
    {
    class IAlfVariantType;               // lib alfwidgetmodel.lib
    class MulVisualItem;                 // Client need to use this class to add data in data model
    }

namespace osncore
    {
    class UString;                       // encapsulates utf-8 encoded string
    }

/**
* CGlxBinding
* The binding is created by the binding set factory.
* This class holds the bindings required to populate the visual item.
*/      
NONSHARABLE_CLASS( CGlxBinding ) : public CBase 
    {
public:
	/**
	* Virtual Destructor
	*/	
	virtual ~CGlxBinding();
	
	/**
	 * PopulateT populates the visual item with the data from the media item
	 * @param aItem Visual item to be populated
	 * @param aMedia Media item 
	 * @param aIsFocused the item is focused or not
	 * @param aTextureId textureid
	 */
	/// T for "throws" (instead of L for "leaves")
	virtual void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia, 
		TBool aIsFocused, TInt aTextureId = KErrNotFound ) const = 0;
	
	/**
	 * HasRelevantAttributes Checks whether the attribute is relevent to the context
	 * @param aAttrbutes MPX attributes
	 */
	virtual TBool HasRelevantAttributes( const RArray< TMPXAttribute >& aAttributes ) const;
	
	enum TResponse
	{
	ENoUpdateNeeded ,
	EUpdateRequested
	};
	
	/**
	 * HandleFocusChanged handles the focus change events
	 * @param aIsGained
	 * @param aPath 
	 */
	virtual TResponse HandleFocusChanged( TBool aIsGained );


    virtual void HandleItemChangedL(const CMPXCollectionPath& aPath );
    
	
	
	/**
	 * SetObserver setting the binding observer
	 * @param aObserver Binding observer
	 */
	// only to be called by CGlxBindingSet
	void SetObserver( MGlxBindingObserver& aObserver );
	
    /**
	 * adding fetch context to medialist
	 * @param aAttributeRequirement
	 */
	virtual void AddRequestL( CGlxAttributeRequirements& 
        aAttributeRequirement,TSize aSize);

protected:
	/**
	 * Update informs the observer that binding has changed
	 */
	void Update();

private: 
	/// From Binding observer
	MGlxBindingObserver* iObserver;
    };
 
/**
 * CGlxTemplateBinding
 * The binding is created by the binding set factory.
 * This class holds the template information when there is editable 
 * template and changing the template when the focus moves away
 */ 
NONSHARABLE_CLASS( CGlxTemplateBinding ): public CGlxBinding
    {
    
public:

    /**
	 * Two phase constructor
	 * @param aTemplate 
	 * @param aFocusedTemplate 
	 */
    static CGlxTemplateBinding* NewL( 
        Alf::mulwidget::TLogicalTemplate aTemplate, 
	    Alf::mulwidget::TLogicalTemplate aTemplateWhenFocused);
        
    /**
	 * Two phase constructor
	 * @param aTemplate 
	 * @param aFocusedTemplate 
	 */    
    static CGlxTemplateBinding* NewLC(
        Alf::mulwidget::TLogicalTemplate aTemplate, 
	    Alf::mulwidget::TLogicalTemplate aTemplateWhenFocused);

    /**
	 * Destructor
	 */    
    ~CGlxTemplateBinding();
    
private: 

    // From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia, 
        TBool aIsFocused,TInt aTextureId = KErrNotFound ) const;

    /**
	 * @todo
	 */        
    TResponse HandleFocusChanged( TBool aIsGained );
    
    void HandleItemChangedL(const CMPXCollectionPath& aPath );
    
private:   
    Alf::mulwidget::TLogicalTemplate iTemplate; 
    Alf::mulwidget::TLogicalTemplate iFocusedTemplate; 
    };
       
// ----------------------------------------------------------------------------
    
/** 
 * CGlxSingleTagBinding
 * Base class for bindings that provide the value for a single mul tag 
 */
NONSHARABLE_CLASS( CGlxSingleTagBinding ) : public CGlxBinding 
    {
public:
	/**
      * Constructor
      */
    CGlxSingleTagBinding(); 
	
    /**
      * Destructor
      */
    ~CGlxSingleTagBinding(); 
    
protected:
    /**
      * Constructs single tag binding 
      * @param aMulTag The mul field that MPX attribute is mapped to
      */      
    void BaseConstructL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/ );
    
    /**
      * Returns the MUL field
      */
    //const osncore::UString& MulTag() const;
    Alf::mulvisualitem::TVisualAttribute MulTag() const;

private:
    /// Stores the MUL field
    //osncore::UString* iMulTag;
    Alf::mulvisualitem::TVisualAttribute iMulTag;
    };
    
// ----------------------------------------------------------------------------

/** 
 * CGlxStringBinding
 * This class holds all string bindings required to populate the visual item 
 */   
NONSHARABLE_CLASS( CGlxStringBinding ) : public CGlxSingleTagBinding
    {
public:
    
    /**
      * Two phase Construction 
      * @param aMulTag The mul field that MPX attribute is mapped to
      * @param aStringId  The MPX attribute
      */
    static CGlxStringBinding* NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/,
        TInt aStringId );
    /**
      * Two phase Construction 
      * @param aMulTag The mul field that MPX attribute is mapped to
      * @param aStringId 
      */
    static CGlxStringBinding* NewLC( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/,
        TInt aStringId );
    
    /**
      * Two phase Construction 
      * @param aMulTag The mul field that MPX attribute is mapped to
      * @param aString 
      */
    static CGlxStringBinding* NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/,
        const char* const aString );
        
    /**
      * Two phase Construction 
      * @param aMulTag The mul field that MPX attribute is mapped to
      * @param aString 
      */
    static CGlxStringBinding* NewLC( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/,
            const char* const aString );
	    
    /**
      * Destructor
      */	    
    ~CGlxStringBinding();
    
    /**
      * Destructor
      */	    
    CGlxStringBinding();

protected:    
	
	/**
      * Two phase Construction 
      * Converts the symbian types to Ustring
      */
	void ConstructL( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/, TInt aStringId );
	
	/**
      * Two phase Construction 
      */
	void ConstructL( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/, const char* const aString );

private: 
    /// From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
        TBool aIsFocused,TInt aTextureId = KErrNotFound ) const;
    
private:
    /// Holds MPX data 
    HBufC* iStringBuffer;
    };

// ----------------------------------------------------------------------------
    
NONSHARABLE_CLASS( CGlxMpxAttributeBinding ) : public CGlxSingleTagBinding
    {
public:
    
    /**
      * Two phase Construction 
      * @param aMulTag The mul field that MPX attribute is mapped to
      * @param aAttribute MPX attribute name 
      */
    static CGlxMpxAttributeBinding* NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/,
        const TMPXAttribute& aAttribute, TInt aFormatStringId );
        
    /**
      * Two phase Construction 
      * @param aMulTag The mul field that MPX attribute is mapped to
      * @param aAttribute MPX attribute name 
      */
    static CGlxMpxAttributeBinding* NewLC( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/,
        const TMPXAttribute& aAttribute, TInt aFormatStringId );
        
    // From CGlxBinding
    void AddRequestL( CGlxAttributeRequirements& aAttributeRequirement ,TSize aSize);
    
protected:

	/**
      * Two phase Construction 
      */
	void ConstructL( const Alf::mulvisualitem::TVisualAttribute& aMulTag/*const char* const aMulTag*/ );
    
private:

    /**
      * Constructor
      * @param aAttribute MPX attribute name 
      */
    CGlxMpxAttributeBinding( const TMPXAttribute& aAttribute, TInt aFormatStringId );
    // From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
        TBool aIsFocused, TInt aTextureId = KErrNotFound ) const;
	
    /**
      * HasRelevantAttributes
      * @param aAttribute MPX attribute name 
      */	
    TBool HasRelevantAttributes( const RArray< TMPXAttribute >& aAttributes ) const;
    
    /**
      * HandleFocusChanged
      * @param aIsGained holds the focus change status
      * @param aPath source path to the collection
      */
    TResponse HandleFocusChanged( TBool aIsGained );
    
    void HandleItemChangedL(const CMPXCollectionPath& aPath );

private:
    /// stores the MPX attribute
    const TMPXAttribute iAttribute;
    TInt iFormatStringId;
    
    };

// ----------------------------------------------------------------------------
    
NONSHARABLE_CLASS( CGlxThumbnailBinding ) : public CGlxSingleTagBinding
    {
public:
    
    /**
      * Two phase construction
      * @param aMulTag The mul field that MPX attribute is mapped to 
      */
    static CGlxThumbnailBinding* NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
	const TSize& aSize  );
    
    /**
      * Two phase construction
      * @param aMulTag The mul field that MPX attribute is mapped to 
      */
    static CGlxThumbnailBinding* NewLC(const  Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
	const TSize& aSize  );
    
    /**
      * Destructor
      */
    ~CGlxThumbnailBinding();
    
    /**
      * Constructor
      */
    CGlxThumbnailBinding( const TSize& aSize );
    
    // From CGlxBinding
    void AddRequestL(CGlxAttributeRequirements& aAttributeRequirement,TSize aSize);
    
    TResponse HandleFocusChanged( TBool aIsGained );
    
private: // From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
        TBool aIsFocused, TInt aTextureId = KErrNotFound ) const;
	
    /**
       * HasRelevantAttributes
       * @param aAttribute MPX attribute name 
       */
    TBool HasRelevantAttributes( const RArray< TMPXAttribute >& aAttributes ) const;

private:
    TSize iThumbnailSize;
    };
  
 // ----------------------------------------------------------------------------
    
NONSHARABLE_CLASS( CGlxFullScreenThumbnailBinding ) : public CGlxSingleTagBinding
    {
public:
    
    /**
      * Two phase construction
      * @param aMulTag The mul field that MPX attribute is mapped to 
      */
    static CGlxFullScreenThumbnailBinding* NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/);
    
    /**
      * Two phase construction
      * @param aMulTag The mul field that MPX attribute is mapped to 
      */
    static CGlxFullScreenThumbnailBinding* NewLC(const  Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/ );
    
    /**
      * Destructor
      */
    ~CGlxFullScreenThumbnailBinding();
    
    /**
      * Constructor
      */
    CGlxFullScreenThumbnailBinding( );
    
    // From CGlxBinding
    void AddRequestL(CGlxAttributeRequirements& aAttributeRequirement,TSize aSize);
    
    TResponse HandleFocusChanged( TBool aIsGained );
    
private: // From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
        TBool aIsFocused, TInt aTextureId = KErrNotFound ) const;
	
    /**
       * HasRelevantAttributes
       * @param aAttribute MPX attribute name 
       */
    TBool HasRelevantAttributes( const RArray< TMPXAttribute >& aAttributes ) const;

private:
    // The size of the thumbnails in fullscreen
    TSize iFullScreenThumbnailSize;
    };
 
// ----------------------------------------------------------------------------
    
NONSHARABLE_CLASS( CGlxIconBinding ) : public CGlxSingleTagBinding
    {
public:

    /**
      * Constructor
      * @param aAttribute MPX attribute name 
      */
    CGlxIconBinding( const CGlxMulIconProvider* aIconProvider , const TSize& aSize);
    
    /**
      * Two phase construction
      * @param aMulTag The mul field that icon from resource file is mapped to 
      */
    static CGlxIconBinding* NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
        const CGlxMulIconProvider* aIconProvider, const TSize& aSize );
        
    /**
      * Two phase construction
      * @param aMulTag The mul field that icon from resource file is mapped to 
      */
    static CGlxIconBinding* NewLC( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
        const CGlxMulIconProvider* aIconProvider, const TSize& aSize );
    
    /**
      * Destructor
      */
    ~CGlxIconBinding();
    
private: // From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
        TBool aIsFocused, TInt aTextureId = KErrNotFound ) const;
        
private:
    const CGlxMulIconProvider* iIconProvider;
    
    TSize iIconSize;
    };
    
// ----------------------------------------------------------------------------

NONSHARABLE_CLASS( CGlxCommandBinding ) : public CGlxSingleTagBinding
    {
public:

    /**
      * Two phase construction
      * @param aCommand MGlxBoundCommand is mapped to 
      * aCommand must be topmost in cleanup stack. This function will pop it.
      */
	static CGlxCommandBinding* NewL ( MGlxBoundCommand* aCommand );
	
    /**
      * Two phase construction
      * @param aCommand MGlxBoundCommand is mapped to 
      */
    static CGlxCommandBinding* NewLC( MGlxBoundCommand* aCommand ); // takes ownership
    
    /**
      * Constructor
      */
    CGlxCommandBinding();

     /**
      * Destructor
      */
    ~CGlxCommandBinding();
    
private:    
// From CGlxBinding
    void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia,
        TBool aIsFocused,TInt textureId = KErrNotFound ) const;
    
private:    
    MGlxBoundCommand* iCommand;
    };
#endif
    

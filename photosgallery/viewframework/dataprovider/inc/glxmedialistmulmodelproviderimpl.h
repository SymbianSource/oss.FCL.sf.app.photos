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
* Description:    Implementation class for Media data provider.
*                 providign the data to the clients
*
*/




#ifndef _GLXMEDIALISTMULMODELPROVIDERIMPL_H_
#define _GLXMEDIALISTMULMODELPROVIDERIMPL_H_

#include <e32base.h>                       // Container Base Class
#include "glxmulmodelproviderbase.h"       // DataProvider Base
#include "mglxbindingobserver.h"           // Notifies that binding has changed
 
namespace Alf
	{
	class IMulWidget;                      // An interface for all Multimedia widgets
	}

// Forward declarations
class TGlxMulBindingSetFactory;            // Create binding set 
class CGlxAttributeRequirements;           // This class will set the required attribute to the medialist
class CAlfEnv;                             // This is the UI Accelerator Toolkit environment object
class MGlxMediaList;                       // Interface for reading lists of media items
class CGlxBindingSet;                      // basically holds the bindings required to populate the visual item.
class CGlxResolutionUtility;               // Singleton containing resolution utility methods
class CGlxUiUtility;                       // lib glxalfutils.lib  ; Alf utils

/**
* CGlxMediaListMulModelProviderImpl
* The actual implemntation for providign the data to the clients is done here
*/
NONSHARABLE_CLASS( CGlxMediaListMulModelProviderImpl ) : public CGlxMulModelProviderBase,
														public MGlxBindingObserver 
    {
    	
public:
	/**
	* The medialist data provider uses this API to create the provider impl
	* @param aEnv The Alf Environment
	* @param aWidget The widget being used by client for which the model has to be set
	* @param aMediaList The instance of medialist which provides data for the model
	* @param aFactory The instance of the bindingset factory 
	* @param aDefaultTemplate The template that is being used for the widget.
	*/
    static CGlxMediaListMulModelProviderImpl* NewL( CAlfEnv& aEnv, 
													Alf::IMulWidget& aWidget, 
													MGlxMediaList& aMediaList,
													const TGlxMulBindingSetFactory& aFactory, 
													Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
													TInt aDataWindowSize );
            
    /**
	* Destructor
	*/                       
    ~CGlxMediaListMulModelProviderImpl();
    
private:
	/**
	* Constructor
	* @param aWidget The widget being used by client for which the model has to be set
	* @param aMediaList The instance of medialist which provides data for the model
	*/
    CGlxMediaListMulModelProviderImpl( Alf::IMulWidget& aWidget, MGlxMediaList& aMediaList );
        
	/**
	* ConstructL
	* @param aEnv The Alf Environment
	* @param aFactory The instance of the bindingset factory 
	* @param aDefaultTemplate The template that is being used for the widget.
	*/   
    void ConstructL( CAlfEnv& aEnv,
                     const TGlxMulBindingSetFactory& aFactory,
                     Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
                     TInt aDataWindowSize );
    
	/**
     * Helper function used be HandleError
     * @param aError the error code to handle.
     */
    void DoHandleErrorL( TInt aError );

	// From MGlxBindingObserver
    void HandleBindingChanged( const CGlxBinding& aBinding );
    
    void HandleOrientationChanged();  
	// From CGlxMulModelProviderBase
	void HandleFocusChanged(TInt aNewFocusIndex, TInt aPreviousFocusIndex);    
	// From MGlxMediaListObserver
    void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleAttributesAvailableL( TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );   
	void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, 
        TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
    void HandleError( TInt aError );
    //Bug fix for PKAA-7NRBYZ
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );
     
    // From IMulModelProvider
    void  ProvideData (int aIndex, int aCount, Alf::MulDataPath aPath);
private:

    MGlxMediaList& iMediaList;
    CGlxBindingSet* iBindingSet; // own
    TBool iIsReleventAttribute;
    CGlxAttributeRequirements* iAttributeRequirements; // own
    CGlxResolutionUtility* iResolutionUtility;
    Alf::IMulWidget& iWidget;
    CGlxUiUtility* iUiUtility;

    };
    
#endif // _GLXMEDIALISTMULMODELPROVIDERIMPL_H_



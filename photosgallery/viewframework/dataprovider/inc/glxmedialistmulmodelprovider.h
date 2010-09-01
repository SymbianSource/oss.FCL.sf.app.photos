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
* Description:    Implementation class for Medialist Access for Data
*
*/




#ifndef _GLXMEDIALISTMULMODELPROVIDER_H_
#define _GLXMEDIALISTMULMODELPROVIDER_H_

#include <e32base.h>                        // Container Base Class
#include <mul/imulwidget.h>

// Forward declarations
class CAlfEnv;                              // This is the UI Accelerator Toolkit environment object
class MGlxMediaList;                        // Interface for reading lists of media items
class TGlxMulBindingSetFactory;             // Create binding set
class CGlxMediaListMulModelProviderImpl;    // Actual implemntation for providign the data to the clients is done here

/**
* CGlxMediaListMulModelProvider is the provider class which provides data for 
* list view, grid view. 
* This doesnt have any implementaiton.
* This actual implementaion is provided in CGlxMediaListMulModelProviderImpl
*/
NONSHARABLE_CLASS( CGlxMediaListMulModelProvider ) : public CBase
    {
public:
	/**
	* The client uses this API to create the provider
	* @param aEnv The Alf Environment
	* @param aWidget The widget being used by client for which the model has to be set
	* @param aMediaList The instance of medialist which provides data for the model
	* @param aFactory The instance of the bindingset factory 
	* @param aDefaultTemplate The template that is being used for the widget.
	*/
    IMPORT_C static CGlxMediaListMulModelProvider* NewL( CAlfEnv& aEnv, 
											Alf::IMulWidget& aWidget, 
											MGlxMediaList& aMediaList, 
											const TGlxMulBindingSetFactory& aFactory, 
											Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
											TInt aDataWindowSize );
    /**
	* Destructor
	*/            
    IMPORT_C ~CGlxMediaListMulModelProvider();
    
      /**
      * Requests the data model to update
      * 
      * @param aIndex Position from where items have to be updated.
      * @param aCount Number of items to be updated.
      */ 
    IMPORT_C void UpdateItems( TInt aIndex, TInt aCount );

      /**
      * Enable/disable animation
      * @param aAnimate - ETrue to enable the animation; EFalse Otherwise.
      */ 
    IMPORT_C void AnimateDRMGifItem( TBool aAnimate );

private:
    CGlxMediaListMulModelProvider();
    
    /** ConstructL
	* @param aEnv Env variable of ALF
 	* @param aWidget Alf Widget type
 	* @param aMediaList medialist from which attributes to be fetched
 	* @param aDefaultTemplate Template that will be set to model
 	*/
    void ConstructL(CAlfEnv& aEnv, 
					Alf::IMulWidget& aWidget, 
					MGlxMediaList& aMediaList,
					const TGlxMulBindingSetFactory& aFactory, 
					Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
					TInt aDataWindowSize );

	// The implementaion APIs of CGlxMediaListMulModelProviderImpl 
	// have to be called: owns
    CGlxMediaListMulModelProviderImpl* iImpl;
    };
    
#endif // _GLXMEDIALISTMULMODELPROVIDER_H_


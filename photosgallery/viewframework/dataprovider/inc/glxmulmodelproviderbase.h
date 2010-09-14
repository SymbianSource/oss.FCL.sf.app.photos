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
* Description:    Interface to Alf Objects
*
*/




#ifndef _GLXMULMODELPROVIDERBASE_H_
#define _GLXMULMODELPROVIDERBASE_H_

#include<e32base.h>                         // Container Base Class
#include <alf/ialfwidgeteventhandler.h>     //  The interface for event handlers used by widget controls 
#include <mglxmedialistobserver.h>          // Observes for changes in media list
#include <mul/imulmodel.h>                  // An interface for the data model
#include <mul/imulmodelprovider.h>          // An interface for requesting the data provider for data
#include <mul/imulwidget.h>
//constants 
static const TInt LAST_INDEX = -1;

class CAlfEnv;                              // This is the UI Accelerator Toolkit environment object 
class CGlxBinding;                          // The binding is created by the binding set factory
class MGlxBoundCommand;                     // Handles user commands
class TGlxMedia;                            // Reference to a media item in the item pool
class CGlxNavigationalState;                // Class that stores the navigational state of the application
class CGlxDrmGifTextureCreator;

namespace Alf
    {
    class IMulModel;                        // An interface for the data model
    class IMulWidget;                       // An interface for all Multimedia widgets
    class MulVisualItem;                    // Client need to use this class to add data in data model
    }

    NONSHARABLE_CLASS( CGlxMulModelProviderBase ) : public CBase, public Alf::IAlfWidgetEventHandler,
											 public MGlxMediaListObserver, public Alf::IMulModelProvider 
	{
protected:

	/**
	 * Constructor
	 * @param aWidget The widget to which the model is to be set
	 */
	CGlxMulModelProviderBase( Alf::IMulWidget& aWidget );
	
	/**
	 * BaseConstructL
	 * @param aEnv Alf environment
	 * @param aDefaultTemplate The default template of the widget
	 */
	void BaseConstructL( CAlfEnv& aEnv, Alf::mulwidget::TLogicalTemplate aDefaultTemplate, TInt aDataWindowSize );
	
	/**
	 * Destructor
	 */
	~CGlxMulModelProviderBase();
	/**
     * CreateModelL
     */
	void CreateModelL();
	/**
	 * InsertItemsL
	 * @param aIndex Start index
	 * @param aCount Number of items to insert
	 * @param aMediaListFocusIndex Focus Index to set in Model
	 */
	void InsertItemsL( TInt aIndex, TInt aCount, TInt aMediaListFocusIndex = NULL );
        
    /**
	 * RemoveItem
	 * @param aAtIndex media list index
	 */   
    void RemoveItems( TInt aIndex, TInt aCount );
	 
	/**
	 * UpdateItemL
	 * @param aBinding Binding used to update the item
	 * @param aMedia media item which has the updated data
	 * @param aAtIndex media list index
	 */
	void SetDataT( const CGlxBinding& aBinding, const TGlxMedia& aMedia,
        TInt aAtIndex, MGlxMediaList& aMediaList);

    /// @todo docs	 
	void UpdateItems( TInt aIndex, TInt aCount );
                
 	/**
     * FocusIndex
     */   
    TInt FocusIndex() const;
    
 	/**
     * BoundCommand
     *@param aVisualItemIndex index of visual item
     *@return MGlxBoundCommand for that visual item
     */   
    MGlxBoundCommand* BoundCommand( TInt aVisualItemIndex ) const;
    
    // @todo docs
    void SetFocusIndex( TInt aIndex );
    
    void AddWidgetEventHandler();
    /**
         * HandleOrientationChanged
         * Icon Size change notification to be implemented dependent on Orientation
         */
    virtual void HandleOrientationChanged();

      /**
      * Enable/disable animation
      * @param aAnimate enable/disable animation
      */ 
    void AnimateDRMGifItem( TBool aAnimate );

private: 

    /**
	 * CreateItemT
	 * @param aBinding Binding used to update the item
	 * @param aMedia media item which has the updated data
	 * @param aIsFocused 
	 */
    std::auto_ptr<Alf::MulVisualItem> CGlxMulModelProviderBase::CreateItemT( const CGlxBinding& aBinding, 
        const TGlxMedia& aMedia, TBool aIsFocused );
    /**
     * IsFocused
     * @param aIndex Media list index
     */
    TBool IsFocused( TInt aIndex ) const;
    
	// From MGlxMediaListObserver
    void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleAttributesAvailableL( TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );    
	void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, 
    TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList );
    void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );
    void HandleError( TInt aError );
    void HandleCommandCompleteL( TAny* /*aSessionId*/, CMPXCommand* aCommandResult, TInt aError, 
        MGlxMediaList* aList );
    void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );
    
	// From IAlfWidgetEventHandler
	bool accept( Alf::CAlfWidgetControl& aControl, const TAlfEvent& aEvent ) const;
    Alf::AlfEventStatus offerEvent( Alf::CAlfWidgetControl& aControl, const TAlfEvent& aEvent );
	void setActiveStates( unsigned int /*aStates*/ );
	void setEventHandlerData( const Alf::AlfWidgetEventHandlerInitData& /*aData*/ );
	Alf::AlfWidgetEventHandlerInitData* eventHandlerData();
	Alf::IAlfInterfaceBase* makeInterface(const Alf::IfId& /*aType */);
    AlfEventHandlerType eventHandlerType() ;
    AlfEventHandlerExecutionPhase eventExecutionPhase() ;

	/**
	 * HandleFocusChanged
	 * Focus change notification to implemented class
	 */
	virtual void HandleFocusChanged( TInt aNewFocusIndex, TInt aPreviousFocusIndex );
	
 	/**
     * Item
     *@param aIndex index of visual item 
     *@return MulVisualItem the visual item at that index
     */   
	const Alf::MulVisualItem& Item( TInt aIndex ) const;
	
private:
	//The widget to which the model will be set
	/// Never expose this widget to any other class so that the osn-symbian interface
	/// is minimised to here. (i.e., try/catch vs. trap/leave)
	Alf::IMulWidget& iWidget;
	
	//Owned: Model
	/// Never expose this model to any other class so that the osn-symbian interface
	/// is minimised to here. (i.e., try/catch vs. trap/leave)
	Alf::IMulModel* iModel;
	
	TBool iWithinFocusChangeCall;
	
	//previous focus index
	TInt iPreviousFocusIndex;
	
	CAlfEnv* iEnv; 
	
	Alf::mulwidget::TLogicalTemplate iDefaultTemplate;
	
	TInt iDataWindowSize;
	
	// Create navigational state 
    CGlxNavigationalState* iNavigationalState;
    CGlxDrmGifTextureCreator* iDrmGifTextureCreator;
	};

#endif // _GLXMULMODELPROVIDERBASE_H_

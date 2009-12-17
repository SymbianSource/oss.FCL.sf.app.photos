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
* Description:    Implementation class for details data provider
*
*/




#ifndef _GLXDETAILSMULMODELPROVIDERIMPL_H_
#define _GLXDETAILSMULMODELPROVIDERIMPL_H_

#include "glxmulmodelproviderbase.h"            // Dataprovider base
#include "mglxbindingobserver.h"                // Notifies that binding has changed

// Forward declarations
class TGlxMulBindingSetFactory;                 // Create binding set
class CGlxAttributeRequirements;                // This class will set the required attribute to the medialist
class CAlfEnv;                                  // This is the UI Accelerator Toolkit environment object
class CEikMenuPane;                             // Menu panes are opened by activating the menu title
class CGlxBinding;                              // This class holds the bindings required to populate the visual item
/** Error Id EMPY-7MKDHP **/
class MGlxMetadataDialogObserver ;				// call back to  details dailog

namespace Alf
    {
    class IMulWidget;                           // An interface for all Multimedia widgets
    }

/**
 * CGlxDetailsMulModelProviderImpl 
 * This is the implementation class of details data provider
 */
NONSHARABLE_CLASS( CGlxDetailsMulModelProviderImpl ) : public CGlxMulModelProviderBase,
														public MGlxBindingObserver
    {
public:
	
	/** Constructor
 	 * @param aEnv Env variable of ALF
   	 * @param aWidget Alf Widget type
 	 * @param aMediaList medialist from which attributes to be fetched
 	 * @param aDefaultTemplate Template that will be set to model
 	 */
	static CGlxDetailsMulModelProviderImpl* NewL(
	    CAlfEnv& aEnv, 
    	Alf::IMulWidget& aWidget,
    	MGlxMediaList& aMediaList,
    	Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
    	TInt aDataWindowSize );
    	
    /**
	 * ConstructL
	 * @param aEnv Alf environment
	 * @param aDefaultTemplate The default template of the widget
	 */
    void ConstructL(
        CAlfEnv& aEnv,
        Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
        TInt aDataWindowSize );
        
    /**
	 * Destructor
	 */
    ~CGlxDetailsMulModelProviderImpl();
    
	/**
	 * This Api uses the binding set factory to create the binding set 
	 * @param aFactory factory instance
	 */
   void AddEntryL( const TGlxMulBindingSetFactory& aFactory );

    /// the ModelProvider will be calling this , change the comments
	/**
	 * OfferCommandL
	 * This will be called from details provider whenever there is an event (for example: navi key pressed ) 
	 * @param aCommandId Id of the command that is about to be executed
	 */
    TBool OfferCommandL( TInt aCommandId );
	
	/**
	 * InitMenuL
	 * This will determine what options to be present in menu pane
	 * @param aMenu The menu pane to edit
	 */
    void InitMenuL( CEikMenuPane& aMenu );
    
    /**
     * HideLocationMenuItem
     * @param aMenu The menu pane to edit
     */
    void HideLocationMenuItem( CEikMenuPane& aMenu ); 
   
    /**
     * EnableMskL
     * This will change the middle softkey as per the focussed item
     */
    void EnableMskL();

     /**
     * SetDetailsDailogObserver
     * This will set the details dailog observer
     */
     /** Error Id EMPY-7MKDHP **/
    void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver ) ;

private:

	/**
	 * Constructor
	 * @param aWidget Alf Widget type
	 */
	CGlxDetailsMulModelProviderImpl(
	    Alf::IMulWidget& aWidget, 
	    MGlxMediaList& aMediaList,
		Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
		TInt aDataWindowSize);
    
	// From MGlxBindingObserver
	void HandleBindingChanged( const CGlxBinding& aBinding );
    
    
	// From CGlxMulModelProviderBase
	void HandleFocusChanged( TInt aNewFocus, TInt aPreviousFocus );
	
	// From MGlxMediaListObserver
    void HandleAttributesAvailableL( TInt aIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );
        
	void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
                                  MGlxMediaList* aList);        
	
	void HandleError( TInt aError );
	
	void HandleCommandCompleteL( TAny* aSessionId, CMPXCommand* aCommandResult, 
	    TInt aError, MGlxMediaList* aList );
    // From IMulModelProvider
   void  ProvideData (int aIndex, int aCount, Alf::MulDataPath aPath);

	/**
	 * HasCommandL
	 * This will check weather command is present in menu pane
	 * @param aMenuPane The menu pane 
 	 * @param aCommandId the command in menu pane 
	 */
   TBool HasCommandL( CEikMenuPane& aMenuPane, const TInt aCommandId );
   
    /**
    * Helper function used be HandleError
    * @param aError the error code to handle.
    */
   void DoHandleErrorL( TInt /*aError*/ );
private:

	// medialist not owned
	MGlxMediaList& iMediaList;
	
	//Array of Bindings owned
	RPointerArray< CGlxBinding > iEntries;
    
    //Fetch context
    CGlxAttributeRequirements* iAttributeRequirements;
    };
    
#endif // _GLXDETAILSMULMODELPROVIDERIMPL_H_

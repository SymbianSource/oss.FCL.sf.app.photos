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
* Description:    Metadata view implementation
 *
*/


#ifndef C_GLXMETADATAVIEWIMP_H__
#define C_GLXMETADATAVIEWIMP_H__

// INCLUDES
#include <alf/alfwidgeteventhandler.h>

#include "glxmetadataview.h"

// FORWARD DECLARATIONS
class MGlxMediaList;
class CGlxDetailsMulModelProvider;

// CLASS DECLARATION
/**
 *  CGlxMetadataViewImp
 * 
 *  Metadata view implementation.
 */
NONSHARABLE_CLASS( CGlxMetadataViewImp ) : public CGlxMetadataView, public IAlfWidgetEventHandler
    {
public:
    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    static CGlxMetadataViewImp* NewL( MGlxMediaListFactory* aMediaListFactory );

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    static CGlxMetadataViewImp* NewLC( MGlxMediaListFactory* aMediaListFactory );
   
    /**
     * Destructor.
     */
    virtual ~CGlxMetadataViewImp();
    
private:

    /**
     * C++ default constructor.
     */
    CGlxMetadataViewImp();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( MGlxMediaListFactory* aMediaListFactory );
    
	// From MEikMenuObserver
	void ViewDynInitMenuPaneL( TInt /*aResourceId*/, CEikMenuPane *aMenuPane );
	
public: // From CGlxMediaListViewBase
	
	virtual TBool HandleViewCommandL( TInt aCommand );
	
	virtual TInt MenuResourceId();
	
	MGlxMediaList& MediaList();

private: // from base class CGlxMediaListViewBase
	
    TUid Id() const;
	
    void DoMLViewActivateL( const TVwsViewId& /*aPrevViewId*/,  
   						TUid /*aCustomMessageId*/, 
    						const TDesC8& aCustomMessage );   										    
    	
    void DoMLViewDeactivate();
    
  
public: // ialfwidgeteventhandler

    bool accept( CAlfWidgetControl& /*aControl*/, const TAlfEvent& /*aEvent*/ ) const;
    
    void setActiveStates( unsigned int /*aStates*/ );
    
    AlfEventStatus offerEvent( CAlfWidgetControl& aControl, const TAlfEvent& aEvent );
      
    IAlfInterfaceBase* makeInterface( const IfId& /*aType*/ ); 
    
    void setEventHandlerData( const AlfWidgetEventHandlerInitData& /*aData*/ );
    
    AlfWidgetEventHandlerInitData* eventHandlerData();	
    
    AlfEventHandlerType eventHandlerType() ;

    AlfEventHandlerExecutionPhase eventExecutionPhase() ;
        
private : //new method
	
	/**
	 * Create media list with URI filter
	 * 
	 * @param aURI uri or file name of item
	 */
	void CreateMediaListL( const TDesC8& aURI ); 
	
	/**
	* Sets the default text for the List View when empty
	*/
	void CreateEmptyText();

	/**
 	 * Constructs the Alf Environment and display
	 */
	void ConstructAlfEnvL();
	
	/**
	 * Constructs the list widget
	 */	
	void ConstructInitialViewL();
	
	/**
	 * Destroys the view Widget
	 */
	void DestroyListViewWidget();

	/**
	 * Send factory instance for the items to be shown in the list
	 */	
	void AddListEntryL();

private:    // Data

	MGlxMediaList*	iItemMediaList;//own	
    TInt	iResourceOffset;
    HBufC*	iItemURI;
    
    //Alf environment (not owned)
   	CAlfEnv* iEnv; 
   	
   	//Alf display (not owned)
	CAlfDisplay* iDisp; 
	
	// Owns the list widget (not owned)
	IMulListWidget*     iListWidget;
	IAlfViewWidget* 	iViewWidget;
	
	//Owns DetailsMulModelProvider
	CGlxDetailsMulModelProvider* iDetailsMulModelProvider;
	
	
    //Control gorup id
    TInt iControlGroupId;
    
    TBool iIsVideo;
    
    TBool iIsDrm;

    };

#endif  // C_GLXMETADATAVIEWIMP_H__

// End of File


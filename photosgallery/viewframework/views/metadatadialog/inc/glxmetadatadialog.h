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
* Description:    Metadata dialog implementation
*
*/


#ifndef GLXMETADATADIALOG_H 
#define GLXMETADATADIALOG_H

// SYSTEM INCLUDES
#include <AknDialog.h>

#include <alf/alfscreenbuffer.h>

#include "glxmetadatacontainer.h"
#include "mglxmedialistprovider.h"
#include "mglxmedialistobserver.h"
#include <mglxmetadatadialogobserver.h>
#include "glxmedia.h"
#include <aknphysicsobserveriface.h> //Physics - Kinetic scrolling listener

// FORWARD DECLARATIONS
class CGlxMetadataCommandHandler;class CGlxCommandHandlerAddToContainer;
class CGlxUiUtility;

// CLASS DECLARATION
/**
 *  CGlxMetadataDialog
 * 
 *  Metadata dialog implementation
 */
class CGlxMetadataDialog : public CAknDialog,
                           public MGlxMediaListProvider,
                           public MGlxMetadataDialogObserver,
                           public MToolbarResetObserver,
                           public MAknPhysicsObserver
                          
    {
public: // Constructors and destructor
    
    /**
	* Two-phased constructor.
	*
	* @param aURI uri or file name of item
	*/
    IMPORT_C static CGlxMetadataDialog* NewL( const TDesC& aUri );
         
    /**
    * Destructor.
    */
    virtual ~CGlxMetadataDialog();
    
private :
    CGlxMetadataDialog(const TDesC& aUri);


public: // Functions from base classes

    /**
    * Initializes the dialog and calls CAknDialog's 
    *   ExecuteLD()
    * This method need to be called to execute the dialog.
    * After exiting the dialog gets automatically destroyed
    *
    * @return TInt  ID value of the button that closed the dialog.
    */
    IMPORT_C TInt ExecuteLD();
    
    /**
    * From CEikDialog. Handles button events and checks if the dialog 
    *   can be closed
    *
    * @param aKeycode Id of the 'pressed' button
    *
    * @return   EFalse if Options menu was pressed 
    *           ETrue otherwise
    */
    TBool OkToExitL( TInt aKeycode );  
    /*
     * From MToolbarResetObserver 
     */
    void HandleToolbarResetting(TBool aVisible);
    
protected: // from MAknPhysicsObserver   

    void ViewPositionChanged( const TPoint& /*aNewPosition*/, TBool /*aDrawNow*/, TUint /*aFlags*/ );
      
    void PhysicEmulationEnded();
      
    TPoint ViewPosition() const;
      
protected:  // Functions from base classes

    /**
    * From MEikCommandObserver Prosesses menucommands
    *
    * @param aCommandId     Commant value defined in resources.
    */
    void ProcessCommandL( TInt aCommandId );

    /**
    * From CCoeControl Handles key events
    *
    * @param aKeyEvent      TKeyEvent 
    * @param aType          TEventCode
    *
    * @return TKeyResponse depending on was is consumed or not.
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
    * From MEikMenuObserver Initializes items on the menu
    *
    * @param aMenuId    Current menu's resource Id
    * @param aMenuPane  Pointer to menupane
    */
    void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
		//hanldes orientation changes.
    void HandleResourceChange( TInt aType );
    void SizeChanged();

private:        
		/**
		* Initializes the dialog's controls before the dialog is sized and 
		* layed out. Empty by default.
		*/
		void PreLayoutDynInitL();
		
		/**
		 * Initializes the dialog's controls after the dialog has been sized 
		 * but before it has been activated.
		 */
		void PostLayoutDynInitL();
		/**
		* Symbian 2nd phase constructor.
		* 
		* @param aURI
		*/
		void ConstructL();
		
		/**
		 * Constructs the Alf Environment and display
		 */
		void ConstructAlfEnvL();
		
		/**
		* Initalise the resource
		*
		*/
		void InitResourceL();   
		
		/** 
		* To deal with any specific commands
		*
		* @param aCommand The command to respond to
		* @return ETrue if the command has been handled.
		*
		*/
		TBool HandleViewCommandL(TInt aCommand);
		
		/**
		* Returns reference to the current media list 
		*
		*/
		MGlxMediaList& MediaList();
        
private:

		void Draw( const TRect& /*aRect*/ ) const;    
		void HandlePointerEventL(const TPointerEvent& aPointerEvent);   	
		/**
		* SetTitleL
		* Sets title for view
		* @param aTitleText title of view to be set
		*/
		void SetTitleL(const TDesC& aTitleText);    
		/**
		* SetPreviousTitleL
		* Sets title of previous view
		*/
		void SetPreviousTitleL();
			/**
		* Find out the items nature to set the corresponding options
		*/
		void OnLocationEditL();
		void AddTagL();
		void AddAlbumL();    
		void SetDetailsDlgToolbarVisibility(TBool aVisible);
		
public: // from MEikDialogPageObserver
    /**
    * @ref MEikDialogPageObserver::CreateCustomControlL
    */
    SEikControlInfo CreateCustomControlL(TInt aControlType);

private: //data
	
		TInt iResourceOffset;
		TBool iStatusPaneAvailable;
		CGlxMetadataContainer* iContainer;
		CGlxMetadataCommandHandler*     iMetadataCmdHandler;//own
		CGlxUiUtility* iUiUtility;		
		HBufC* iPreviousTitle;    
		const TDesC& iUri;
		CGlxCommandHandlerAddToContainer*  iAddToTag;
		CGlxCommandHandlerAddToContainer*  iAddToAlbum;
		/*
		 * Store previous pointer event point
		 */ 
		TPoint iPrev;
		
		/*
		 * Check if the dragging got beyond kinetic drag threshold & screen 
		 * movement will start, if set to true 
		 */
		TBool iViewDragged;
		
		/*
		 * Store the value of kinetic drag threshold
		 */
		TInt iKineticDragThreshold;
    };

#endif  // GLXMETADATADIALOG_H

// End of File

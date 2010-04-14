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
* Description:    Dialog class for fetcher plugin app
*
*/




#ifndef GLXFETCHERDIALOG_H_
#define GLXFETCHERDIALOG_H_

//  EXTERNAL INCLUDES
#include <AknDialog.h>  		        // for CAknDialog
#include <glxfilterfactory.h>         	// CMPXFilter
#include <coedef.h>
#include <alf/alfscreenbuffer.h>

#include "mglxeventobserver.h"
#include "glxmmcnotifier.h"

// Forward Declerations
class CGlxFetcherContainer;
class CGlxUiUtility;
class MMGFetchVerifier;
class CAlfEnv;


//  INTERNAL INCLUDES

NONSHARABLE_CLASS (CGlxFetcherDialog) : public CAknDialog ,
                                        public MGlxEventObserver,
                                        public MStorageNotifierObserver 
	{
	 public:  // Constructors and destructor
		/**
		* Two-phased constructor.
		* @param reference to an array where the selected files are set
        * @param selection verifier interface
        * @param filtertype, either images or videos
        * @param the title for the dialog
		*/
        static CGlxFetcherDialog* NewL(
            CDesCArray& aSelectedFiles, 
            MMGFetchVerifier* aVerifier,
            TGlxFilterItemType aFilterType,
            const TDesC& aTitle,
            TBool aMultiSelectionEnabled);

        /**
        * Destructor.
        */
        ~CGlxFetcherDialog();

	public: // from MEikCommandObserver
		/**
		* @ref MEikCommandObserver
		*/
		void ProcessCommandL(TInt aCommandId);

    protected: // Functions CEikDialog
    
        /**
         * @ref CEikDialog
         */
        TBool OkToExitL(TInt aKeycode);
    
        /**  
         *   From MEikMenuObserver
         */
        void  DynInitMenuPaneL (TInt aResourceId, CEikMenuPane *aMenuPane) ;
        
	protected: // From CAknDialog
        /**  
         *   From MEikMenuObserver
         */
        TKeyResponse OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/);
    
	private:
		/**
        * Symbian 2nd phase constructor
		*/
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CGlxFetcherDialog(
            CDesCArray& aSelectedFiles, MMGFetchVerifier* aVerifier,
            TGlxFilterItemType aFilterType, const TDesC& aTitle, 
            TBool aMultiSelectionEnabled);

	private: // from CCoeControl
		/**
		* @ref CCoeControl::SizeChanged
		*/
		void SizeChanged();
	
	private:	
        /**
        * Initializes the dialog's controls before the dialog is sized and 
        * layed out.
        */
		void PreLayoutDynInitL();

        /**
         * Initializes the dialog's controls after the dialog has been sized 
         * but before it has been activated.
         */
		void PostLayoutDynInitL();
    
    private: // from MEikDialogPageObserver
        /**
         * @ref MEikDialogPageObserver::CreateCustomControlL
         */
        SEikControlInfo CreateCustomControlL(TInt aControlType);
    
    private: // CAknDialog
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
    private:// From MGlxEventObserver
    
        virtual void HandleDoubleTapEventL(TInt aCommandId);          
        virtual void HandleMarkEventL();
	
	public:// to exit the fetcher dialog 
		void CallCancelFetcherL(TInt aCommandId);
	    /**
	     * handle MMC insertion
	     */
	    void HandleMMCInsertionL(){};
	    /**
	     * handle MMC removal
	     */
	    void HandleMMCRemovalL();

    
    private:
        CGlxFetcherContainer* iFetcherContainer ;              //Container Object
        CGlxUiUtility* iUiUtility;                             // UiUtility
        CAlfEnv* iEnv;                                         // Alf Environment
    
        // Selected items array
        // Not Own. Owned by the client
        CDesCArray& iSelectedFiles;
    
        MMGFetchVerifier* iVerifier;
    
        TGlxFilterItemType iFilterType;                         // GlxFilterType 
    
        /// Ref: the title
        const TDesC& iTitle;
    
        TBool iMultiSelectionEnabled;
        
        // Flag to check for mark started or not
        TBool iMarkStarted;
        
        TBool iFetchUri;                                        // This is to check if uri is being fetcher or not
       
        //mmc notifier
        CGlxMMCNotifier* iMMCNotifier;
	};

#endif /*GLXFETCHERDIALOG_H_*/

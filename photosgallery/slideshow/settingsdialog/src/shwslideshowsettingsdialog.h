/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The slideshow settings dialog header file
 *
*/





#ifndef SHWSLIDESHOWSETTINGSDLG_H
#define SHWSLIDESHOWSETTINGSDLG_H

//  EXTERNAL INCLUDES
#include <AknDialog.h>  		// for CAknDialog

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CShwSlideShowSettingsList;
class CShwSettingsModel;
class CAknSettingItemArray;
class CEikStatusPane;
class CAknTitlePane;

// CLASS DECLARATION

/**
 * Slideshow Setting Dialog allows the user adjust basic slide
 * runtime settings
 * class CShwSlideshowSettingsDialog
 * @author Loughlin Spollen 
 * @internal reviewed 08/06/2007 by Kimmo Hoikka
 */

NONSHARABLE_CLASS (CShwSlideshowSettingsDialog) : public CAknDialog
	{
	 public:  // Constructors and destructor
		/**
		* Two-phased constructor.
		*/
        static CShwSlideshowSettingsDialog* NewL();

        /**
        * Destructor.
        */
        ~CShwSlideshowSettingsDialog();

	private:
		/**
        * Symbian 2nd phase constructor
		*/
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CShwSlideshowSettingsDialog();

        void SetTitleL();
        /**
         * To Set the previous Title in the status pane , used in slideshow settings only
         */
        void SetPreviousTitleL();
        void SetSlShwToolbarVisibility(TBool aVisible);

	public: // from MEikDialogPageObserver
		/**
		* @ref MEikDialogPageObserver::CreateCustomControlL
		*/
		SEikControlInfo CreateCustomControlL(TInt aControlType);
		
	public: // from CCoeControl
		/**
		* @ref CCoeControl::SizeChanged
		*/
		void SizeChanged();
		
		/**
		* @ref CCoeControl::OfferKeyEventL
		*/
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);
		/**
		* @ref CCoeControl::FocusChanged
		*/                
        void FocusChanged(TDrawNow aDrawNow);  

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


	private: // Data
		TInt iResourceOffset;
		
		// Not Owned: Akn control item list 
		CShwSlideShowSettingsList* iItemList; 

		// Owned: Settings model
		CShwSettingsModel* iShwSettings;
		
		// Status Pane instance
		CEikStatusPane* iStatusPane;
		// Title Pane instance
		CAknTitlePane* iTitlePane;
		
		// Flag to be used at status pane enabling and disabling for FS view
		TBool iStatusPaneChanged;
		
		// To Store the Previous Status Pane Title
	    HBufC* iPreviousTitle;
		    
	};

#endif //SHWSLIDESHOWSETTINGSDLG_H

/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Slideshow settings dialog implementation
*
*/





#ifndef SHWSLIDESHWSETTINGSLIST_H
#define SHWSLIDESHWSETTINGSLIST_H

//  EXTERNAL INCLUDES
#include <eiklbo.h>					// for MEikListBoxObserver
#include <aknsettingitemlist.h>		// for CAknSettingItem
#include <MMGFetchVerifier.h>		// for MMGFetchVerifier
#include <caf/content.h>			// for ContentAccess::CContent
#include <caf/data.h>				// for ContentAccess::CData

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CRepository;
class CShwSettingsModel;
class TShwEffectInfo;

/**
 * @author Loughlin Spollen
 * The slide show settings list - contained with the settings dialog
 * @lib shwslideshowengine.lib
 * @internal reviewed 08/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS (CShwSlideShowSettingsList) : public CAknSettingItemList,
								 			    public MMGFetchVerifier
	{
	public:  // Constructors and destructor
		/**
        * Symbian default constructor.
        * @param aShwSettings setting values for music state, default path 
        *		 and track default effect ECOM plugin UID and effect index 
        *		 within the effect ECOM plugin
        */
		static CShwSlideShowSettingsList* NewL(CShwSettingsModel& aShwSettings);
	
		/**
		* Destructor
		*/
		~CShwSlideShowSettingsList();
													 
	public: //from CAknSettingItemList
	
		// @ref CAknSettingItemList::CreateSettingItemL
		CAknSettingItem* CreateSettingItemL(TInt aIdentifier);
		
		// @ref CAknSettingItemList::HandleListBoxEventL
		void HandleListBoxEventL(CEikListBox* aListBox,
				       			  	  TListBoxEvent aEventType);

	public: 

		/**
		* Update the list box.
		* @param The List box item index to be updated
		* @param Whether the event originated from a key press or menu command
		*/	
        void UpdateListBoxL(TInt aListBoxIndex, TInt aKeyEvent = EFalse);
		
		/**
		* Update the music on/off control and the coupled
		* music file name.
		*/	
		void UpdateMusicStateL();

		/**
		* Update the music file name and path
		*/			
		void UpdateMusicFileNamePathL();
		
		/**
		* Update the transition delay between images
		*/	
		void UpdateTransDelayL();

		/**
		* Update the transition effect between images
		*/	
		void UpdateTransEffectL(TBool aKeyEvent);


    private:	
		/**
        * C++ default constructor.
        * @param aShwSettings setting values for music state, default path 
        *		 and track default effect ECOM plugin UID and effect index 
        *		 within the effect ECOM plugin
        */
		CShwSlideShowSettingsList(CShwSettingsModel& aShwSettings);

		/**
		* 2nd stage constructor
		*/
        void ConstructL();
	
	
	private: 

	
		/**
        * Launch MGFetch for filename fetch
        */
		void FetchFileNameFromUserL();		
		
		/**
        * Remove the music settings and set the default setting
        * values
        */
		void ResetMusicDefaultsL();

		
		/**
        * Validate the DRM rights for the music track file
		* @return TBool ETrue if valid for track selection
        */
		TBool ValidateDRML();

		/**
        * Show error note
		* @param aResourceId resource id of text string
		*/
		void ShowErrorNoteL(TInt aResourceId) const;


		/**
        * Check if iFileName exists
		* @return ETrue if exists
        */
		TBool FileNameExistsL();		
		
		/**
        * Change the state of the music enabled setting
        */
		void SetMusicStateL(TBool aNewMusicState);
		
		/**
        * Change the direction/order of slide show play
        */
		void SetPlayDirectionL(TBool aNewPlayDirection);
		
    	/**
    	* Ask for user confirmation of a given query using a dialog
    	* @param aQueryResourceId The query resource string ID
    	* @return True if the user confirmed the query, otherwise false
    	*/
    	TBool ConfirmationQueryL(TInt aQueryStrId);

        /**
        * Check if the only files that can be played are DRM protected
        * @return ETrue if the only file that can be played are DRM protected
        *       otherwise EFalse
        */
        TBool PlayOnlyDRMProtectedFilesL();
        
        /**
        * Read the values present in rss file and do required conversion
        * @param aResourceId resource string ID
        * @return converted value
        */
        HBufC* ReadLBufStringL(const TInt aResourceId);

	private: // from MMGFetchVerifier

		/**
		* @ref MMGFetchVerifier::VerifySelectionL
		*/
        TBool VerifySelectionL(const MDesCArray* aSelectedFiles);

	private: //data
	
		class CShwTransEffectEnumPopupSettingItem;
		class CShwTransEffectBinaryPopupSettingItem;
		// Add our own setting page and item to fix
		// EEBD-79UL8Q - "no naviscroll support in slideshow settings"
		class CShwSliderSettingPage;
		class CShwSliderSettingItem;
		
		// Music On/Off Setting
		TBool iMusicState;
		
		//Play direction
		TBool iPlayDirection;
		
		// Number of transition effects
		TUint iEffectsCount;
		
		// Array of Transition effects
		RArray<TShwEffectInfo> iEffects;
		
		// Music filename Setting
		RBuf iMusicNamePath;
		
		// The transition delay between images
		TInt iTransDelay;
				
		// Not Owned: Slideshow settings 
		CShwSettingsModel& iShwSettings;
        		
        // Owned: The Central Repository
        CRepository* iRepository;
        
	};

#endif // SHWSLIDESHWSETTINGSLIST_H

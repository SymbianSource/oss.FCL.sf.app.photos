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
* Description:    Slideshow settings list implementation
*
*/




//EXTERNAL INCLUDES
#include <aknappui.h>
#include <StringLoader.h>
#include <featmgr.h>					// for FeatureManager
#include <MGFetch.h>
#include <aknnotewrappers.h>  			// for CAknInformationNote
#include <centralrepository.h> 			// for CRepository
#include <coeutils.h>					// for ConeUtils
#include <musicplayerdomaincrkeys.h>	// for KPlaybackRestrictedMimeTypes
#include <caf/caf.h>					// for ContentAccess::CContent
#include <centralrepository.h>			// for CRepository
#include <AknQueryDialog.h>             // for querydialog

#include <shwsettingsdialog.rsg>
#include <glxassert.h>  // for GLX_ASSERT_DEBUG
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <f32file.h>
#include <aknslidersettingpage.h>

//INTERNAL INCLUDES
#include <glxlog.h>
#include <glxtracer.h>
#include "shwslideshowsettingslist.h"
#include "shwsettingsmodel.h"
#include "shwslideshowengine.h"
#include "shweffectinfo.h"
#include "shwslideshowsettings.hrh"
#include "shwconstants.h"
#include "shwconstants.hrh"

 _LIT(KResourceFile, "z:shwsettingsdialog.rsc");

// Add our own setting page
// EEBD-79UL8Q - "no naviscroll support in slideshow settings"
/**
*  CShwSliderSettingPage
*  CShwSliderSettingPage class for creating a slider setting page
*/
NONSHARABLE_CLASS( CShwSlideShowSettingsList::CShwSliderSettingPage )
    : public CAknSliderSettingPage
    {
    public:
	    /**
	    * C++ Constructor.
	    * @param aSettingPageResourceId The setting page's resource ID
	    * @param aSliderValue The initial value of the slider
	    */
        CShwSliderSettingPage( TInt aSettingPageResourceId, TInt& aSliderValue );
	    
	    /**
	    * C++ Constructor.
	    * @param aSettingTitleText The setting page's title
	    * @param aSettingNumber The setting page's number
	    * @param aControlType The setting page's control type
	    * @param aEditorResourceId The setting page's editor resource ID
	    * @param aSettingPageResourceId The setting page's resource ID
	    * @param aSliderValue The initial value of the slider
	    */
        CShwSliderSettingPage( const TDesC* aSettingTitleText, 
                                TInt aSettingNumber, 
                                TInt aControlType,
                                TInt aEditorResourceId, 
                                TInt aSettingPageResourceId,
                                TInt& aSliderValue );
	    /**
	    * Destructor.
	    */
        ~CShwSliderSettingPage();

	public: // from CCoeControl
	    /**
	    *   @ref CCoeControl::OfferKeyEventL
	    */
	    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
  
    };

// Add our own setting item to fix
// EEBD-79UL8Q - "no naviscroll support in slideshow settings"
/**
*  CShwSliderSettingItem
*  CShwSliderSettingItem class for creating a slider setting item
*/
NONSHARABLE_CLASS( CShwSlideShowSettingsList::CShwSliderSettingItem )
    : public CAknSliderSettingItem
    {
    public:
	    /**
	    * Default C++ Constructor.
	    * @param aIdentifier The resource ID
	    * @param aExternalSliderValue The initial value of the slider
	    */
        CShwSliderSettingItem( TInt aIdentifier, TInt& aExternalSliderValue );

	    /**
	    * Destructor.
	    */
        ~CShwSliderSettingItem();

	protected: // from CAknSliderSettingItem
	    /**
	    * @ref CAknSliderSettingItem::CreateSettingPageL
	    */
	    CAknSettingPage* CreateSettingPageL();    
    };

/**
*  CShwTransEffectEnumPopupSettingItem
*  CShwTransEffectEnumPopupSettingItem class for creating a enum Popup SettingsItem
*/
NONSHARABLE_CLASS( CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem )
								: public CAknEnumeratedTextPopupSettingItem
	{
	public:

	    /**
	    * Default C++ Constructor.
	    * @param aIdentifier The resource ID
	    * @param aShwSettings The Slideshow settings model
	    * @param aEffects Array of Transition effects
	    */
		CShwTransEffectEnumPopupSettingItem(TInt aIdentifier, 
										CShwSettingsModel& aShwSettings,
                                    	RArray<TShwEffectInfo>& aEffects);
	    /**
	    * Destructor.
	    */
		~CShwTransEffectEnumPopupSettingItem();

		/**
		* Retrieve the currently selected transition effect
		* @param [out] The selected effects ECOM Uid
		* @param [out] The selected effects index within the ECOM library
		*/
		void SelectedEffectL(TUid& aEffectUid, TUint& aEffectIndex);
				
	public: // from CAknEnumeratedTextPopupSettingItem
	    
	    /**
	    * @ref CAknEnumeratedTextPopupSettingItem::LoadL
	    */
		void LoadL(); 

	    /**
	    * @ref CAknEnumeratedTextPopupSettingItem::StoreL
	    */
		void StoreL(); 

	    /**
	    * @ref CAknEnumeratedTextPopupSettingItem::CompleteConstructionL
	    */
		void CompleteConstructionL();

       	/**
	    * @ref CAknEnumeratedTextPopupSettingItem::ExternalValue
	    */
        inline TInt ExternalValue()
            {
            return CAknEnumeratedTextPopupSettingItem::ExternalValue();
            };

	private:

		// Slideshow settings data
		CShwSettingsModel& iShwSettings;
		
		// Array of Transition effects
		RArray<TShwEffectInfo>& iEffects;

		// The current index within the iEffects array
		TInt iTransTypeIndex;
	};




/**
*  CShwTransEffectBinaryPopupSettingItem
*  CShwTransEffectBinaryPopupSettingItem class for creating a binary 
*  Popup SettingsItem
*/
NONSHARABLE_CLASS( CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem )
								: public CAknBinaryPopupSettingItem
	{
	public:

	    /**
	    * Default C++ Destructor.
	    * @param aIdentifier The resource ID
	    * @param aShwSettings The Slideshow settings model
	    * @param aEffects Array of Transition effects
	    */
		CShwTransEffectBinaryPopupSettingItem(TInt aIdentifier, 
										CShwSettingsModel& aShwSettings,
										RArray<TShwEffectInfo>& aEffects);
	    /**
	    * Destructor.
	    */
		~CShwTransEffectBinaryPopupSettingItem();

		/**
		* Retrieve the currently selected transition effect
		* @param [out] The selected effects ECOM Uid
		* @param [out] The selected effects index within the ECOM library
		*/
		void SelectedEffectL(TUid& aEffectUid, TUint& aEffectIndex);
				
	public: // from CAknEnumeratedTextPopupSettingItem
	    
	    /**
	    * @ref CAknEnumeratedTextPopupSettingItem::LoadL
	    */
		void LoadL(); 

	    /**
	    * @ref CAknEnumeratedTextPopupSettingItem::StoreL
	    */
		void StoreL(); 

	    /**
	    * @ref CAknEnumeratedTextPopupSettingItem::CompleteConstructionL
	    */
		void CompleteConstructionL();

	    /**
	    * @ref CAknEnumeratedTextPopupSettingItem::SetExternalValue
	    */
        inline void SetExternalValue( TInt aNewValue )
            {
            CAknBinaryPopupSettingItem::SetExternalValue( aNewValue );
            };

	private:
		
		// Slideshow settings data
		CShwSettingsModel& iShwSettings;
		
		// Array of Transition effects
		RArray<TShwEffectInfo>& iEffects;
				
		// The current index within the iEffects array
		TInt iTransTypeIndex;
	};


//-----------------------------------------------------------------------------
// C++ constructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwSliderSettingPage
    ::CShwSliderSettingPage( TInt aSettingPageResourceId, TInt& aSliderValue )
    : CAknSliderSettingPage( aSettingPageResourceId, aSliderValue )
    {
    }

//-----------------------------------------------------------------------------
// C++ constructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwSliderSettingPage
    ::CShwSliderSettingPage( const TDesC* aSettingTitleText, 
                            TInt aSettingNumber, 
                            TInt aControlType,
                            TInt aEditorResourceId, 
                            TInt aSettingPageResourceId,
                            TInt& aSliderValue )
    : CAknSliderSettingPage( aSettingTitleText, aSettingNumber, aControlType,
                        aEditorResourceId, aSettingPageResourceId, aSliderValue )
    {
    }

//-----------------------------------------------------------------------------
// C++ destructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwSliderSettingPage::~CShwSliderSettingPage()
    {
    }

//-----------------------------------------------------------------------------
// CShwSliderSettingPage::OfferKeyEventL
//-----------------------------------------------------------------------------    
TKeyResponse CShwSlideShowSettingsList::CShwSliderSettingPage::
    OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    TRACER("CShwSlideShowSettingsList::CShwSliderSettingPage::OfferKeyEventL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::CShwSliderSettingPage::OfferKeyEventL"); 
    // Fix for EEBD-79UL8Q - "No Naviscroll support in Slideshow settings
    // options menu"
    TKeyEvent localKeyEvent = aKeyEvent;
    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            // Handle the previous and next navi-scroll events and modify the 
            // key code to the corresponding values used by the dialog
            case EKeyPrevious:
                localKeyEvent.iCode = EKeyLeftArrow;
                break;
            case EKeyNext:
                localKeyEvent.iCode = EKeyRightArrow;
                break;
            default:
                // nothing to change
                break;
            }
        }
    
    // pass the key event up to the parent to handle
    return CAknSliderSettingPage::OfferKeyEventL( localKeyEvent, aType );    
    }


//-----------------------------------------------------------------------------
// C++ default constructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwSliderSettingItem::CShwSliderSettingItem
    ( TInt aIdentifier, TInt& aExternalSliderValue )
        : CAknSliderSettingItem( aIdentifier, aExternalSliderValue )
    {
    }

//-----------------------------------------------------------------------------
// C++ destructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwSliderSettingItem::~CShwSliderSettingItem()
	{
	}

//-----------------------------------------------------------------------------
// CShwSliderSettingItem::CreateSettingPageL
//-----------------------------------------------------------------------------
CAknSettingPage* CShwSlideShowSettingsList::CShwSliderSettingItem::
    CreateSettingPageL()
    {
    TRACER("CShwSlideShowSettingsList::CShwSliderSettingPage::OfferKeyEventL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::CShwSliderSettingPage::OfferKeyEventL");
    TPtrC text = SettingName();
    return new( ELeave ) CShwSlideShowSettingsList::CShwSliderSettingPage(
        &text, SettingNumber(), EAknCtSlider, SettingEditorResourceId(),
            SettingPageResourceId(), InternalSliderValue() );
    }


//-----------------------------------------------------------------------------
// C++ default constructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem
    ::CShwTransEffectEnumPopupSettingItem( TInt aIdentifier,
        CShwSettingsModel& aShwSettings, RArray<TShwEffectInfo>& aEffects ) 
        : CAknEnumeratedTextPopupSettingItem(aIdentifier, iTransTypeIndex),
            iShwSettings(aShwSettings), iEffects(aEffects)
	{
	}

//-----------------------------------------------------------------------------
// C++ destructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem
						 ::~CShwTransEffectEnumPopupSettingItem()
	{
	}


//-----------------------------------------------------------------------------
// CShwTransEffectEnumPopupSettingItem::SelectedEffectL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::
									SelectedEffectL(TUid& aEffectUid, TUint& aEffectIndex)
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::SelectedEffectL");
	GLX_LOG_INFO("CShwTransEffectEnumPopupSettingItem::SelectedEffectL");
	GLX_ASSERT_DEBUG(
	    ( 0 <= iTransTypeIndex )&&( iTransTypeIndex < iEffects.Count() ),
	    Panic( EGlxPanicIndexOutOfRange ),
	    "Slideshow effect index is out of range" );
	aEffectUid   = iEffects[iTransTypeIndex].iId.iPluginUid;
	aEffectIndex = iEffects[iTransTypeIndex].iId.iIndex;
	}



//-----------------------------------------------------------------------------
// CShwTransEffectEnumPopupSettingItem::CompleteConstructionL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem
							  ::CompleteConstructionL()
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::::CompleteConstructionL");
	GLX_LOG_INFO("CShwTransEffectEnumPopupSettingItem::CompleteConstructionL");
	CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();

	TUint max = iEffects.Count();

	// Construct a new EnumeratedTextArray and an empty PopupText array	
	CArrayPtrFlat<CAknEnumeratedText>* enumeratedTextArray =
				 new (ELeave) CArrayPtrFlat<CAknEnumeratedText>(max);
	CleanupStack::PushL(enumeratedTextArray);
	
	CArrayPtrFlat<HBufC>* poppedUpTextArray = 
				 new (ELeave) CArrayPtrFlat<HBufC>(max);
	CleanupStack::PushL(poppedUpTextArray);

    // transfers the ownership of the arrays
	SetEnumeratedTextArrays(enumeratedTextArray, poppedUpTextArray);
    HandleTextArrayUpdateL(); 
	CleanupStack::Pop(poppedUpTextArray);
	CleanupStack::Pop(enumeratedTextArray);
	}


//-----------------------------------------------------------------------------
// CShwTransEffectEnumPopupSettingItem::LoadL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::LoadL()
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::LoadL");
	GLX_LOG_INFO("CShwTransEffectEnumPopupSettingItem::LoadL");
 	// Clear out old array contents
	CArrayPtr<CAknEnumeratedText>* enumTxtArray = EnumeratedTextArray();
	enumTxtArray->ResetAndDestroy();

	// Create new array of transition types
	TUint max = iEffects.Count();
	CAknEnumeratedText* enumeratedTxt = NULL;
	HBufC* 				transitionTxt = NULL;

	for (TInt ii = 0; ii < max; ++ii)
		{
		transitionTxt  = iEffects[ii].iName.AllocLC();
		enumeratedTxt = new (ELeave) CAknEnumeratedText(ii, transitionTxt);
		CleanupStack::Pop(transitionTxt);
		CleanupStack::PushL(enumeratedTxt);
		enumTxtArray->AppendL(enumeratedTxt);
		CleanupStack::Pop(enumeratedTxt);
		}

	// Get persisted default value
	TUid  effectUid     = {0};
	TUint effectIndex   = 0;
	iTransTypeIndex = KErrNotFound;
	iShwSettings.TransitionTypeL(effectUid, effectIndex);

	for (TInt ii = 0; ii < max; ++ii)
		{
		if (iEffects[ii].iId.iPluginUid == effectUid && 
				iEffects[ii].iId.iIndex == effectIndex)
			{
			iTransTypeIndex = ii;
			continue;
			}
		}

	User::LeaveIfError(iTransTypeIndex); // should never be KErrNotFound
	User::LeaveIfError(CAknEnumeratedTextSettingItem::IndexFromValue
															(iTransTypeIndex));
	SetExternalValue(iTransTypeIndex);
	CAknEnumeratedTextPopupSettingItem::LoadL();
	}

//-----------------------------------------------------------------------------
// CShwTransEffectEnumPopupSettingItem::StoreL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::StoreL()
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::StoreL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::CShwTransEffectEnumPopupSettingItem::StoreL");
	CAknEnumeratedTextPopupSettingItem::StoreL();
	}



//-----------------------------------------------------------------------------
// C++ default constructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem
						 ::CShwTransEffectBinaryPopupSettingItem(TInt aIdentifier, 
											CShwSettingsModel& aShwSettings,
										    RArray<TShwEffectInfo>& aEffects) 
			  				: CAknBinaryPopupSettingItem(aIdentifier, iTransTypeIndex),
    						iShwSettings(aShwSettings), iEffects(aEffects)
	{
	}

//-----------------------------------------------------------------------------
// C++ destructor
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem
						 ::~CShwTransEffectBinaryPopupSettingItem()
	{
	}



//-----------------------------------------------------------------------------
// CShwTransEffectBinaryPopupSettingItem::SelectedEffectL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem::
									SelectedEffectL(TUid& aEffectUid, TUint& aEffectIndex)
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem::SelectedEffectL");
	GLX_LOG_INFO("CShwTransEffectBinaryPopupSettingItem::SelectedEffectL");
	GLX_ASSERT_DEBUG(
	    ( 0 <= iTransTypeIndex )&&( iTransTypeIndex < iEffects.Count() ),
	    Panic( EGlxPanicIndexOutOfRange ),
	    "Slideshow effect index is out of range" );

	aEffectUid   = iEffects[iTransTypeIndex].iId.iPluginUid;
	aEffectIndex = iEffects[iTransTypeIndex].iId.iIndex;
	}



//-----------------------------------------------------------------------------
// CShwTransEffectBinaryPopupSettingItem::CompleteConstructionL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem
							  ::CompleteConstructionL()
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem::CompleteConstructionL");
	GLX_LOG_INFO("CShwTransEffectBinaryPopupSettingItem::CompleteConstructionL");

	// Retrieve all available effects
	TUint max = iEffects.Count();

	// Construct a new EnumeratedTextArray and an empty PopupText array	
	CArrayPtrFlat<CAknEnumeratedText>* enumeratedTextArray =
				 new (ELeave) CArrayPtrFlat<CAknEnumeratedText>(max);
	CleanupStack::PushL(enumeratedTextArray);
	
	CArrayPtrFlat<HBufC>* poppedUpTextArray = 
				 new (ELeave) CArrayPtrFlat<HBufC>(max);
	CleanupStack::PushL(poppedUpTextArray);
    
    // Transfers ownership
	SetEnumeratedTextArrays(enumeratedTextArray, poppedUpTextArray);
	HandleTextArrayUpdateL(); 
	CleanupStack::Pop(poppedUpTextArray);
	CleanupStack::Pop(enumeratedTextArray);
	}


//-----------------------------------------------------------------------------
// CShwTransEffectBinaryPopupSettingItem::LoadL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem::LoadL()
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem::LoadL");
	GLX_LOG_INFO("CShwTransEffectBinaryPopupSettingItem::LoadL");
 	// Clear out old array contents
	CArrayPtr<CAknEnumeratedText>* enumTxtArray = EnumeratedTextArray();
	enumTxtArray->ResetAndDestroy();

	// Create new array of transition types
	TUint max = iEffects.Count();
	CAknEnumeratedText* enumeratedTxt = NULL;
	HBufC* 				transitionTxt = NULL;

	for (TInt ii = 0; ii < max; ++ii)
		{
		transitionTxt  = iEffects[ii].iName.AllocLC();
		enumeratedTxt = new (ELeave) CAknEnumeratedText(ii, transitionTxt);
		CleanupStack::Pop(transitionTxt);
		CleanupStack::PushL(enumeratedTxt);
		enumTxtArray->AppendL(enumeratedTxt);
		CleanupStack::Pop(enumeratedTxt);
		}

	// Get persisted default value
	TUid  effectUid     = {0};
	TUint effectIndex   = 0;
	iTransTypeIndex = KErrNotFound;
	iShwSettings.TransitionTypeL(effectUid, effectIndex);

	for (TInt ii = 0; ii < max; ++ii)
		{
		if (iEffects[ii].iId.iPluginUid == effectUid && 
				iEffects[ii].iId.iIndex == effectIndex)
			{
			iTransTypeIndex = ii;
			continue;
			}
		}

	User::LeaveIfError(iTransTypeIndex); // should never be KErrNotFound
	User::LeaveIfError(CAknEnumeratedTextSettingItem::IndexFromValue
															(iTransTypeIndex));
	SetExternalValue(iTransTypeIndex);

	CAknBinaryPopupSettingItem::LoadL();
	}

//-----------------------------------------------------------------------------
// CShwTransEffectBinaryPopupSettingItem::StoreL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem::StoreL()
	{
	TRACER("CShwSlideShowSettingsList::CShwTransEffectBinaryPopupSettingItem::StoreL");
	GLX_LOG_INFO("CShwTransEffectBinaryPopupSettingItem::StoreL");
	CAknBinaryPopupSettingItem::StoreL();
	}


//-----------------------------------------------------------------------------
// CShwTransEffectEnumPopupSettingItem::NewL
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList* CShwSlideShowSettingsList::NewL(CShwSettingsModel& 
                                                                aShwSettings)
    {
    TRACER("CShwSlideShowSettingsList::NewL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::NewL");
    CShwSlideShowSettingsList* self = new (ELeave) CShwSlideShowSettingsList
                                                            (aShwSettings);
    CleanupStack::PushL(self);                                                        
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;                                                            
    }



//-----------------------------------------------------------------------------
// destructor.
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::~CShwSlideShowSettingsList()
	{
	TRACER("CShwSlideShowSettingsList::~CShwSlideShowSettingsList");
	GLX_LOG_INFO("CShwSlideShowSettingsList::~CShwSlideShowSettingsList");	
	delete iRepository;
	iMusicNamePath.Close();
	iEffects.Close();
	}

//-----------------------------------------------------------------------------
// C++ default constructor.
//-----------------------------------------------------------------------------
CShwSlideShowSettingsList::CShwSlideShowSettingsList(
        CShwSettingsModel& aShwSettings ) 
    : iShwSettings( aShwSettings )
	{
	TRACER("CShwSlideShowSettingsList::CShwSlideShowSettingsList");
	GLX_LOG_INFO("CShwSlideShowSettingsList::CShwSlideShowSettingsList");
	}

//-----------------------------------------------------------------------------
// Second stage constructor.
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::ConstructL()
    {
    TRACER("CShwSlideShowSettingsList::ConstructL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::ConstructL");
    iMusicNamePath.CreateL(KMaxFileName);
    }

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::CreateSettingItemL
//-----------------------------------------------------------------------------
CAknSettingItem* CShwSlideShowSettingsList::CreateSettingItemL(TInt 
																aResourceId)
	{
	TRACER("CShwSlideShowSettingsList::CreateSettingItemL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::CreateSettingItemL");		
	CAknSettingItem* settingItem = NULL; // No need to push onto cleanup stack
	switch(aResourceId)
		{
		case EPlayDirectionItem:
			{
			iPlayDirection = iShwSettings.PlayOrderL();
			settingItem = new (ELeave) CAknBinaryPopupSettingItem(aResourceId, 
																iPlayDirection);
			
			break;													
			}
		case EMusicEnabledItem:
			{
			iMusicState = iShwSettings.MusicOnL();
			settingItem = new (ELeave) CAknBinaryPopupSettingItem(aResourceId, 
																iMusicState);
			break;
			}
		case EFileNamePathItem:
			{
			// get the track from settings
			iShwSettings.MusicNamePathL( iMusicNamePath );
			if(!ConeUtils::FileExists( iMusicNamePath ) )
			    {
			    HBufC* noneTxt = ReadLBufStringL( R_SHW_SLIDESHOW_SETTINGS_TRACK_NONE );
			    iMusicNamePath = *noneTxt;
			    iShwSettings.SaveMusicNamePathL( *noneTxt );
			    delete noneTxt;
			    }
			settingItem = new (ELeave) CAknTextSettingItem( 
			        aResourceId, iMusicNamePath );

			// The music should be disabled when the file does not exist
			// Note: this change is not persisted. Same check is performed
			// 		 each time the Settings Dialog is launched
			if( FileNameExistsL() )
                {
                // Ensure that the DRM is still valid. If it has become 
                // invalid while after being used in slideshow, the user 
                // should be informed now.
                ValidateDRML();
                }
			else
                {
                // file does not exist, set music off
                SetMusicStateL( EFalse );
                }
				
			break;
			}
		case ETransDelayItem:
			{
			// need to reverse as slider is left to right, range is 
			// from minimum to maximum
			iTransDelay = KMinTransDelay + KMaxTransDelay - iShwSettings.TransDelayL();
    		// Use our own slider setting item to fix
	    	// EEBD-79UL8Q - "no naviscroll support in slideshow settings"
    		settingItem = new (ELeave) CShwSliderSettingItem( aResourceId, 
												iTransDelay );
			break;
			}
		case ETransEffectItem:
			{
			// Retrieve all available effects
        	CShwSlideshowEngine::AvailableEffectsL(iEffects);
  			settingItem = new (ELeave) 
    			        CShwTransEffectBinaryPopupSettingItem(aResourceId, 
    			                                    iShwSettings, iEffects);
    	    // As of now, Slideshow have only one effect. Thats why setting pan not 
    	    // require in setting dialog. 
    	    // Whenever other effect will be implemented, comment line No. 722 and 
    	    // 738 and uncomment line No. 723 to 727 and line No. 739 to 743    	    
    	    settingItem->SetHidden(ETrue);
            /*if (iEffectsCount > 2) // hide this item - the TransEffectListItem
                {                  // is used instead
                settingItem->SetHidden(ETrue);
                HandleChangeInItemArrayOrVisibilityL();
                }*/

			break;	
			}
	    case ETransEffectListItem:
	        {
        	CShwSlideshowEngine::AvailableEffectsL(iEffects);
        	iEffectsCount = iEffects.Count();
        	settingItem = new (ELeave) CShwTransEffectEnumPopupSettingItem(
									    aResourceId, iShwSettings, iEffects);
			
	        settingItem->SetHidden(ETrue);						    
            /*if (iEffectsCount <= 2) // hide this item - used only with menu 
                {
                settingItem->SetHidden(ETrue);
                HandleChangeInItemArrayOrVisibilityL();
                }*/
	        break;
	        }
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	return settingItem;  // return ownership to framework
	}

	


//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::VerifySelectionL
//-----------------------------------------------------------------------------
TBool CShwSlideShowSettingsList::VerifySelectionL(
											const MDesCArray* aSelectedFiles)
    {
    TRACER("CShwSlideShowSettingsList::VerifySelectionL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::VerifySelectionL");		    
    return (aSelectedFiles && aSelectedFiles->MdcaCount() > 0);
    }




//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::HandleListBoxEventL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::HandleListBoxEventL(CEikListBox*  aListBox,
													TListBoxEvent aEventType)
	{
	TRACER("CShwSlideShowSettingsList::HandleListBoxEventL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::HandleListBoxEventL");		    
	if ((aEventType == EEventEnterKeyPressed) || 
		(aEventType == EEventEditingStarted) ||
		(aEventType == EEventItemSingleClicked))
        {
        TInt index = aListBox->CurrentItemIndex();
    	GLX_ASSERT_DEBUG(
    	    index < (*SettingItemArray()).Count(),
    	    Panic( EGlxPanicIndexOutOfRange ),
    	    "Slideshow listbox index is out of range" );
        
        UpdateListBoxL(index);
		}
	}


//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::UpdateListBoxL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::UpdateListBoxL( TInt aListBoxIndex, 
                                                TBool aKeyEvent )
    {
    TRACER("CShwSlideShowSettingsList::UpdateListBoxL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::UpdateListBoxL"); 
	switch(aListBoxIndex)
		{
		case EPlayDirectionItem:
			{
			SetPlayDirectionL(!iPlayDirection);
			break;
			}
		case EMusicEnabledItem:
			{
			UpdateMusicStateL();
			break;
			}
		case EFileNamePathItem:
			{
			UpdateMusicFileNamePathL();
			break;
			}
		case ETransDelayItem:
			{
			UpdateTransDelayL();
			break;
			}
		case ETransEffectItem:
		case ETransEffectListItem:
			{
			UpdateTransEffectL(aKeyEvent);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	SettingItemArray()->RecalculateVisibleIndicesL();
	HandleChangeInItemArrayOrVisibilityL();	
    }



//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::UpdateMusicStateL()
//-----------------------------------------------------------------------------

void CShwSlideShowSettingsList::UpdateMusicStateL()
	{
	TRACER("CShwSlideShowSettingsList::UpdateMusicStateL"); 
	GLX_LOG_INFO("CShwSlideShowSettingsList::UpdateMusicStateL");
	// if music was disabled (iMusicState member variable has not yet 
	//    been updated) and a valid file does not exist
	
	
	RBuf fileName;
	User::LeaveIfError(fileName.Create(KMaxFileName));
	fileName.CleanupClosePushL();
	iShwSettings.MusicNamePathL(fileName);
	
	// if music was disabled (iMusicState member variable has not yet 
	//    been updated) 
	
	if (!iMusicState )
		{
	    HBufC* defVal  = ReadLBufStringL( R_SHW_SLIDESHOW_SETTINGS_TRACK_NONE );
	    CleanupStack::PushL(defVal);
		// if file not defined (None)
		if ( fileName.operator ==( *defVal ) )
			{
			if (ConfirmationQueryL(R_SHW_QTN_LGAL_NOTE_TRACK_DEFINE))
				{
				// only change the music state if the update of the music file name
				// and path was successful
				// The music state is updated inside this method if the Filename is
				// valid
				UpdateMusicFileNamePathL();
				}
			}
		else
			{
			// if file defined but not exists	
			if (!FileNameExistsL())
				{
				// Display the information note	
				HBufC* noteBuf = StringLoader::LoadLC( R_SHW_QTN_LGAL_NOTE_SLIDESHOW_DEFINE );
				CAknInformationNote* note = new( ELeave ) CAknInformationNote;
				CleanupStack::Pop( noteBuf );
				note->ExecuteLD( *noteBuf );
				}
			else
				{
				SetMusicStateL( !iMusicState );
				}
			}
		CleanupStack::PopAndDestroy( defVal );
		}
	else
		{
	 	SetMusicStateL( !iMusicState );
	 	}
	CleanupStack::PopAndDestroy( &fileName );
	}

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::UpdateMusicFileNamePathL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::UpdateMusicFileNamePathL()
	{
	TRACER("CShwSlideShowSettingsList::UpdateMusicFileNamePathL"); 
	GLX_LOG_INFO("CShwSlideShowSettingsList::UpdateMusicFileNamePathL");
	
	CAknSettingItem* item = (*SettingItemArray())[EFileNamePathItem];

	// Retrieve & persist the new file path
	FetchFileNameFromUserL();
	
	if( ConeUtils::FileExists( iMusicNamePath ) )
		{	
		if( ValidateDRML() )
			{
			// persist new settings value
			item->LoadL(); // externalise the new value to iMusicNamePath
			iShwSettings.SaveMusicNamePathL( iMusicNamePath );
			// Redraw the control
			ListBox()->DrawItem( EFileNamePathItem );
			// turn music [enabled] on
			SetMusicStateL( ETrue );
			return;
			}
		}
	else
		{
		ResetMusicDefaultsL();
		}
	SetMusicStateL( EFalse );
	}

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::UpdateTransDelayL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::UpdateTransDelayL()
	{
	TRACER("CShwSlideShowSettingsList::UpdateTransDelayL"); 
	GLX_LOG_INFO("CShwSlideShowSettingsList::UpdateTransDelayL");	
	CAknSettingItem* item = (*SettingItemArray())[ETransDelayItem];
	
	item->EditItemL(EFalse);   
	item->StoreL();
	
	// The slider control has a fast (2) value drawn to the right and a 
	// slow value (30) drawn to the left. However, the slider only allows 
	// ascending values from left to right so the delay value has to be 
	// inverted. Range is from minimum to maximum
	iShwSettings.SaveTransDelayL( KMinTransDelay + KMaxTransDelay - iTransDelay );

	// Redraw the control
	ListBox()->DrawItem(ETransDelayItem);
	}



//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::UpdateTransEffectL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::UpdateTransEffectL(TBool aKeyEvent)
	{
	TRACER("CShwSlideShowSettingsList::UpdateTransEffectL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::UpdateTransEffectL");	
	CAknSettingItem* effectItem = (*SettingItemArray())[ETransEffectItem];
	
	TUint effectIndex = 0;
	TUid  effectUid  = {0};
    const TUint KToggleEvents = 2;


    // should always toggle if there are two entries    
    if (aKeyEvent && iEffectsCount <= KToggleEvents) 
        {
    	effectItem->EditItemL(EFalse);
	    effectItem->StoreL();
        static_cast<CShwTransEffectBinaryPopupSettingItem*>(effectItem)
			    	            ->SelectedEffectL(effectUid, effectIndex);
        }
    else
        {
        // always display listbox when there are > 2 effects or when it's 
        // opened from the menu
        CAknSettingItem* effectListItem = 
                            (*SettingItemArray())[ETransEffectListItem];
    	effectListItem->EditItemL(EFalse);
	    effectListItem->StoreL();
        static_cast<CShwTransEffectEnumPopupSettingItem*>(effectListItem)
		                		->SelectedEffectL(effectUid, effectIndex);
		
		static_cast<CShwTransEffectBinaryPopupSettingItem*>(effectItem)
		  ->SetExternalValue(static_cast<CShwTransEffectEnumPopupSettingItem*>
		    (effectListItem)->ExternalValue());
		static_cast<CAknBinaryPopupSettingItem*>(effectItem)->
		    CAknBinaryPopupSettingItem::LoadL();
        }
	iShwSettings.SaveTransitionTypeL(effectUid, effectIndex);

	// Redraw the control
	ListBox()->DrawItem(ETransEffectItem);
	}



//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::FetchFileNameFromUserL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::FetchFileNameFromUserL()
    {
    TRACER("CShwSlideShowSettingsList::FetchFileNameFromUserL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::FetchFileNameFromUserL");
    CDesCArray* files = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(files);

    HBufC* title  = ReadLBufStringL( R_SHW_QTN_POPUP_SELECT_TRACK );
    
    CleanupStack::PushL( title );
    HBufC* noneTxt = ReadLBufStringL( R_SHW_SLIDESHOW_SETTINGS_TRACK_NONE );
   	CleanupStack::PushL( noneTxt );
    	
	TBool fetchReturn = MGFetch::RunL(*files, EAudioFile, EFalse, KNullDesC(), *title, this);
	
	if(fetchReturn)
		{
		iMusicNamePath = (*files)[0];
		}
	// Fix for Bug EJQZ-7EMCS7 - If user doesnt select a file from list, It shows the same file selected
	else if (iMusicNamePath !=*noneTxt && !fetchReturn)
		{
		// Do Nothing , If the user doesnt select a file from the list,
		// it shows the same file as before and thus do nothing.
		}
	else 
	    {
    	// load "none" from resource
    	iMusicNamePath = *noneTxt;
	    }
   	CleanupStack::PopAndDestroy(noneTxt);
    CleanupStack::PopAndDestroy( title );
    CleanupStack::PopAndDestroy( files );
    }

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::ResetMusicDefaultsL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::ResetMusicDefaultsL()
	{
	TRACER("CShwSlideShowSettingsList::ResetMusicDefaultsL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::ResetMusicDefaultsL");	
	// load "none" from resource
	HBufC* noneTxt = ReadLBufStringL( R_SHW_SLIDESHOW_SETTINGS_TRACK_NONE );
	CleanupStack::PushL( noneTxt );
	iMusicNamePath = *noneTxt;
	(*SettingItemArray())[EFileNamePathItem]->LoadL();
	// save empty string to settings so that the dialog will pick up 
	// empty text from the resource to show
    // this fixes error when the old localised "none" text
	// was shown for track (MKER-7BJCE5)
	iShwSettings.SaveMusicNamePathL( *noneTxt );
	CleanupStack::PopAndDestroy( noneTxt );

	// turn music [enabled] off
	SetMusicStateL( EFalse );
	ListBox()->DrawItem( EFileNamePathItem );
	}

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::ValidateDRML
//-----------------------------------------------------------------------------
TBool CShwSlideShowSettingsList::ValidateDRML()
	{
	TRACER("CShwSlideShowSettingsList::ValidateDRML");
	GLX_LOG_INFO("CShwSlideShowSettingsList::ValidateDRML");	

	// check that DRM exists
	// create a CContent object. CAF will figure out the appropriate agent	
	ContentAccess::CContent* content = 
	    ContentAccess::CContent::NewL(iMusicNamePath);
	CleanupStack::PushL( content );

	// create a CData object to read the content, while also
    ContentAccess::CData* data = NULL;
	// verifying that the intent is supported by the current rights 
	TRAPD( err, data = content->OpenContentL( ContentAccess::EPlay ); );
    delete data;
    CleanupStack::PopAndDestroy( content ); 

	if( KErrCA_LowerLimit <= err && err <= KErrCA_UpperLimit )
		{
		// if there are no play rights for the file and we're only allowed
		// play files that are DRM protected
		if( err == KErrCANoRights && PlayOnlyDRMProtectedFilesL() )		
			{
			ShowErrorNoteL( R_SHW_NOT_ALLOWED_TXT );
    		return EFalse;	// can't play file (rights don't exist)
			}
		else 
			{
			ShowErrorNoteL( R_SHW_SETTINGSLIST_DRM_TXT );	
    		return EFalse;	// can't play file (rights are invalid)
			}
		}
	else
		{
		User::LeaveIfError( err );
		}
    return ETrue;	
	}


//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::ShowErrorNoteL
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::ShowErrorNoteL(TInt aResourceId) const
	{
	TRACER("CShwSlideShowSettingsList::ShowErrorNoteL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::ShowErrorNoteL");	
	HBufC* errText = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* note = new (ELeave) CAknInformationNote( EFalse );
    note->ExecuteLD( *errText );
    CleanupStack::PopAndDestroy( errText );
	}



//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::FileNameExistsL
//-----------------------------------------------------------------------------
TBool CShwSlideShowSettingsList::FileNameExistsL()
	{
	TRACER("CShwSlideShowSettingsList::FileNameExistsL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::FileNameExistsL");	
	RBuf fileName;
	User::LeaveIfError(fileName.Create(KMaxFileName));
	fileName.CleanupClosePushL();
	iShwSettings.MusicNamePathL(fileName);
    TBool retVal = EFalse;
    
	if (fileName.Length() > 0)
		{
		retVal = ConeUtils::FileExists(fileName);
		}
	CleanupStack::PopAndDestroy(&fileName);	
	return retVal;
	}

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::SetMusicStateL()
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::SetMusicStateL(TBool aNewMusicState)
	{
	TRACER("CShwSlideShowSettingsList::SetMusicStateL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::SetMusicStateL");	
    if (aNewMusicState != iMusicState)
        {
    	// retrieve music on/off setting
    	(*SettingItemArray())[EMusicEnabledItem]->EditItemL(EFalse);
    	// updates Music State
    	iMusicState = aNewMusicState;
    	// internalise new state to settings item
    	(*(SettingItemArray()))[EMusicEnabledItem]->StoreL();
    	// persist music state
    	iShwSettings.SaveMusicStateL(iMusicState);
    	// redraw music on/off control
    	ListBox()->DrawItem(EMusicEnabledItem);
        }
	}
//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::SetPlayDirectionL()
//-----------------------------------------------------------------------------
void CShwSlideShowSettingsList::SetPlayDirectionL(TBool aNewPlayDirection)
	{
	TRACER("CShwSlideShowSettingsList::SetPlayDirectionL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::SetMusicStateL");	
    if (aNewPlayDirection != iPlayDirection)
        {
    	// retrieve play direction setting
    	(*SettingItemArray())[EPlayDirectionItem]->EditItemL(EFalse);
    	// updates play direction 
    	iPlayDirection = aNewPlayDirection;
    	// internalise new state to settings item
    	(*(SettingItemArray()))[EPlayDirectionItem]->StoreL();
    	
    	// persist direction
    	iShwSettings.SavePlayOrderL(iPlayDirection);
    	//@TODO cenrep update
    	
    	// redraw music on/off control
    	ListBox()->DrawItem(EPlayDirectionItem);
        }
	}
//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::ConfirmationQueryL
//-----------------------------------------------------------------------------
TBool CShwSlideShowSettingsList::ConfirmationQueryL(TInt aQueryStrId)
    {
    TRACER("CShwSlideShowSettingsList::ConfirmationQueryL");
	GLX_LOG_INFO("CShwSlideShowSettingsList::ConfirmationQueryL");						    
    HBufC* titleText = StringLoader::LoadLC(aQueryStrId);
    CAknQueryDialog* query = CAknQueryDialog::NewL();
    CleanupStack::PushL(query);
    query->SetPromptL(*titleText);
    CleanupStack::Pop(query);
	TInt retVal = query->ExecuteLD(R_SHW_CONFIRMATION_QUERY);
	CleanupStack::PopAndDestroy(); //titleText
    return (retVal == EAknSoftkeyOk || retVal == EAknSoftkeyYes);
    }

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::PlayOnlyDRMProtectedFiles
//-----------------------------------------------------------------------------
TBool CShwSlideShowSettingsList::PlayOnlyDRMProtectedFilesL()
    {
    TRACER("CShwSlideShowSettingsList::PlayOnlyDRMProtectedFilesL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::PlayOnlyDRMProtectedFilesL");  
	TBool playOnlyDRMProtectedFiles = EFalse;
	if (!iRepository)
		{
		iRepository = CRepository::NewL(KCRUidMusicPlayerFeatures);
		}
    // Check if DRM protected files are the only files that be played
	TInt err = iRepository->Get(KRequireDRMInPlayback, 
	                                             playOnlyDRMProtectedFiles);
    if (err == KErrNotFound) // there maybe no KRequireDRMInPlayback entry
        {
	    return EFalse;
        }
    else
        {
        User::LeaveIfError(err);
        }

    return  playOnlyDRMProtectedFiles;
    }

//-----------------------------------------------------------------------------
// CShwSlideShowSettingsList::ReadLBufStringL
//-----------------------------------------------------------------------------
HBufC* CShwSlideShowSettingsList::ReadLBufStringL( const TInt aResourceId )
    {
    TRACER("CShwSlideShowSettingsList::ReadLBufStringL");
    GLX_LOG_INFO("CShwSlideShowSettingsList::ReadLBufStringL");
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TParse parse;
    parse.Set( KResourceFile, &KDC_APP_RESOURCE_DIR, NULL );

    TFileName resFile;
    resFile.Copy( parse.FullName() );
    BaflUtils::NearestLanguageFile( fs, resFile );

    RResourceFile rFile; 
    rFile.OpenL( fs, resFile );
    CleanupClosePushL( rFile );
        
    // Set pointer to starting
    rFile.ConfirmSignatureL( 0 );    

    HBufC8* buf = rFile.AllocReadLC( aResourceId ); 

    TResourceReader resReader;
    resReader.SetBuffer( buf );
    
    // get the string from resource reader
    HBufC* defVal = resReader.ReadHBufCL();
    
    // pop and destroy stack data
    CleanupStack::PopAndDestroy( buf );	
    CleanupStack::PopAndDestroy( &rFile );
    CleanupStack::PopAndDestroy( &fs );
    
    return defVal;
    }
    
//  End of File

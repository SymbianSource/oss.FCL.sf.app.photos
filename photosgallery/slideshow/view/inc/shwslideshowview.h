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
* Description:    Slideshow view implementation
*
*/


#ifndef C_SHWSLIDESHOWVIEW_H
#define C_SHWSLIDESHOWVIEW_H

// INCLUDES
#include <glxviewbase.h>
#include <mglxmedialistobserver.h>
#include <AknProgressDialog.h>
#include <AknWsEventObserver.h>
#include <gestureobserver.h>
#include <gesturehelper.h>
#include "shwengineobserver.h"
#include "shwmusicobserver.h"
#include "shwconstants.h"
#include "shwtickobserver.h"
#include "shwgestureobserver.h"
#include "gesturecontrol.h"
#include "glxmmcnotifier.h"
#include <mglxtvobserver.h>
#include <glxtv.h>
#include <mglxhdmidecoderobserver.h>

// FORWARD DECLARATIONS
class CAlfEnv;
class CAlfDisplay;
class CAlfDisplayCoeControl;
class CAlfControlGroup;
class CAlfSoftKeyControl;
class CGlxUiUtility;
class CGlxMediaListManager;
class MGlxMediaList;
class CShwSlideshowEngine;
class CAknWaitDialog;
class CShwSlideshowVolumeControl;
class CShwSlideshowBackLightTimer;
class CShwSlideShowPauseHandler;
class CShwSlideShowKeyHandler;
class CShwTelephoneHandler;
class CMPXCollectionPath;
class CEikButtonGroupContainer;
class CShwGestureControl;
class CShwTicker;
class CShwMediaKeyUtility;
class CGestureControl;
class CGlxHdmiController;
/**
 *  Slideshow view.
 *
 * @internal reviewed 07/06/2007 by Kimmo Hoikka  
 */
NONSHARABLE_CLASS(CShwSlideshowView) : public CGlxViewBase,
                                       public MGlxMediaListObserver,
                                       public MShwEngineObserver,
                                       public MShwMusicObserver,
                                       public MProgressDialogCallback,
                                       public MShwTickObserver,
                                       public MShwGestureObserver,
                                       public MStorageNotifierObserver,
                                       public MGlxTvObserver,
									   public MGlxHDMIDecoderObserver,
									   public MAknWsEventObserver
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @return Pointer to newly created object.
         */
        static CShwSlideshowView* NewLC();

        /**
         * Destructor.
         */
        ~CShwSlideshowView();
    
    public: // Callback functions
    	/**
    	 * Show the progress dialog
    	 * this needs to be public as it is a callback.
    	 * @return TInt, a non-zero value if it is intended to
         *      be called again, otherwise it should return zero.
    	 */
        TInt ShowProgressDialogL();
        

    	/**
    	 * Start the engine
    	 * this needs to be public as it is a callback.
    	 * @return TInt, a non-zero value if it is intended to
         *      be called again, otherwise it should return zero.
    	 */    
        TInt StartEngineL();

    	/**
    	 * Populate the media list
    	 * this needs to be public as it is a callback.
    	 * @return TInt, a non-zero value if it is intended to
         *      be called again, otherwise it should return zero.
    	 */    
        TInt PopulateListL();

    private:
   
        /**
         * C++ default constructor.
         */
        CShwSlideshowView();

        /**
         * 2nd phase constructor
         */
    	void ConstructL();
     
    private: // from base class CAknView

        /** 
         * @ref CAknView::Id
         */
        TUid Id() const;

        /** 
         * @ref CAknView::HandleForegroundEventL
         */
        void HandleForegroundEventL(TBool aForeground);

    public: // From MAknWsEventObserver
        void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);

    public: // From CGlxViewBase
    	
    	/**
    	 * @ref CGlxViewBase::DoViewActivateL
    	 */	
        void DoViewActivateL( const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId, const TDesC8& aCustomMessage );
    	/**
    	 * @ref CGlxViewBase::DoViewDeactivate
    	 */	
        void DoViewDeactivate();

   public://From MGlxHDMIDecoderObserver
	   /**
		* Handle notification of HDMI Image Decoder.
		*/    
	   void HandleHDMIDecodingEventL(THdmiDecodingStatus aStatus);

    private: // from MGlxMediaListObserver

    	/// @ref MGlxMediaListObserver::HandleItemAddedL
        void HandleItemAddedL( TInt aStartIndex,
        					   TInt aEndIndex,
        					   MGlxMediaList* aList );
    	/// @ref MGlxMediaListObserver::HandleMediaL
        void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );
    	/// @ref MGlxMediaListObserver::HandleItemRemovedL
        void HandleItemRemovedL( TInt aStartIndex,
        						TInt aEndIndex,
        						MGlxMediaList* aList );
    	/// @ref MGlxMediaListObserver::HandleItemModifiedL
        void HandleItemModifiedL( const RArray<TInt>& aItemIndexes,
        						  MGlxMediaList* aList );
     	/// @ref MGlxMediaListObserver::HandleAttributesAvailableL
        void HandleAttributesAvailableL( TInt aItemIndex,
        								 const RArray<TMPXAttribute>& aAttributes,
        								 MGlxMediaList* aList );
     	/// @ref MGlxMediaListObserver::HandleFocusChangedL
        void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType,
        						 TInt aNewIndex,
        						 TInt aOldIndex,
        						 MGlxMediaList* aList );    
     	/// @ref MGlxMediaListObserver::HandleItemSelectedL
        void HandleItemSelectedL( TInt aIndex,
        						 TBool aSelected,
        						 MGlxMediaList* aList );    
    	/// @ref MGlxMediaListObserver::HandleMessageL
        void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );
	    
    	/// @ref MGlxMediaListObserver::HandlePopulatedL
    	void HandlePopulatedL( MGlxMediaList* aList );

    	// From MGlxTVObserver
    	virtual void HandleTvStatusChangedL ( TTvChangeType aChangeType );
    	
    	//form CAknView
		void ProcessCommandL(TInt aCommandId);
    private: // From MShwEngineObserver

    	/**
    	 * @ref MShwEngineObserver::EngineStartedL
    	 */
    	void EngineStartedL();
    	
    	/**
    	 * @ref MShwEngineObserver::EnginePausedL
    	 */
    	void EnginePausedL();
    	
    	/**
    	 * @ref MShwEngineObserver::EngineResumedL
    	 */	
    	void EngineResumedL();
    	
    	/**
         * @ref MShwEngineObserver::EngineToggleUiStateL
         */ 
        void EngineToggleUiStateL();
        /**
         * @ref MShwEngineObserver::EngineLSKPressedL
         */ 
        void EngineLSKPressedL();

    	/**
    	 * @ref MShwEngineObserver::ErrorDuringSlideshowL
    	 */	
        void ErrorDuringSlideshowL();

    private: // from MShwMusicObserver

    	/**
    	 * @ref MShwMusicObserver::MusicOn
    	 */
        void MusicOnL();
    	
    	/**
    	 * @ref MShwMusicObserver::MusicOff
    	 */
        void MusicOff();
    	
    	/**
    	 * @ref MShwMusicObserver::MusicOn
    	 */
        void MusicVolumeL(TInt aCurrentVolume, TInt aMaxVolume);

    	/**
    	 * @ref MShwMusicObserver::ErrorWithTrackL
    	 */
        void ErrorWithTrackL( TInt aErrorCode );

    private: // from MProgressDialogCallback

        void DialogDismissedL( TInt aButtonId );

    private: // Helper functions
        
    	/**
    	 * Setup the screen furniture
    	 */
        void SetupScreenFurnitureL();
        
    	/**
    	 * Method that creates a local, ordered copy of the media list
    	 * @param aData from which to extract the play direction and the path
    	 */
        void GetPathAndPlaybackDirectionL( const TDesC8& aData );
        
    	/**
    	 * Helper function to set the focus in the filtered list, depending
    	 * on the selection or focus of the original list.
    	 */
        void SetListFocusL();
    	
    	/**
    	 * @ref MShwTickObserver::HandleTickL
    	 */
        void HandleTickL();

        /**
    	 * @ref MShwTickObserver::HandleTickCancelled
    	 */
        void HandleTickCancelled();
    	
    	/**
    	 * @ref MShwGestureObserver::HandleShwGestureEventL
    	 */
        void HandleShwGestureEventL(MShwGestureObserver::TShwGestureEventType aType);
		
		/**
		 * Initializes the screen furniture for the slide show view
		 */
        void InitializeShwFurnitureL();

        /**
    	 * Initializes the soft keys
    	 */
		void InitializeCbaL();

		/**
    	 * Hides the screen furniture
    	 */
		void HideShwFurniture();

		/**
    	 * Makes the screen furniture visible
    	 */
		void ShowShwFurnitureL();

		/**
    	 * Replaces an existing command set with a new one
    	 *@
    	 */
		void ReplaceCommandSetL(TInt aNewComandId, TInt aOldCommandSet );

		/**
         * returns the index of item for which texture can be removed for cleanup
         * The index will be out the iterator offsets w.r.t focssed index.
         */
        TInt GetIndexToBeRemoved();

        /**
         * Remove the fullscreen texture.
         */
        void RemoveTexture();
        
        /**
         * Set the current Item to HDMI.
         */
        void SetItemToHDMIL();
        /**
         * HandleMMCInsertionL.
         */
        void HandleMMCInsertionL();
        /**
         * HandleMMCRemovalL.
         */
        void HandleMMCRemovalL();
		/**
		 * Returns the index of the next item 
		 * for which HDMI advance decoding will be done
		 */
		TInt GetNextIndex();
		/**
         * Returns the application foreground status.
		 * Foreground status is found using WindowsGroupID
		 */
		TBool IsAppInForegroundL();
	public:
	//to keep in track which of the command set is active/on top
		enum TShwState
			{
			EShwPause = 1 ,
			EShwPlay,
			EShwExiting
			};	
    private: // Data

        TInt iResourceOffset;

    	/**
         * HUIToolkit environment. 
         * not owned
         */
    	CAlfEnv* iEnv;	

    	CAlfDisplay* iDisplay;	// not owned

        /** Slideshow Engine */
        CShwSlideshowEngine* iEngine; // owned
       
        CAlfControlGroup* iVolumeControlGroup; // owned

        CAknWaitDialog*	iWaitDialog; // owned

        /// Own: Pause handler
        CShwSlideShowPauseHandler* iPauseHandler;

        /// Own: Key handler
        CShwSlideShowKeyHandler* iKeyHandler;
        
        CShwSlideshowVolumeControl* iVolumeControl; // owned

        TInt iCurrentVolume;// owned

        CShwSlideshowBackLightTimer* iBackLightTimer;	// owned

        // This flag is used to determine whether or not the slideshow
        // should resume when it returns to the foreground
        TBool iPauseOnForeground; // owned

        MGlxMediaList* iFilteredList;	// owned
        MGlxMediaList* iMediaList;		// owned
        /// Own: Flag to tell that input list is ready to be used
        TBool iInputListReady;
        /// Own: Flag to tell that play list is ready to be used
        TBool iPlayListReady;

        /// Own: asynch callback needed for engine start
        CAsyncCallBack* iAsyncCallBack;

        // handle interruptions from incoming telephone calls
        CShwTelephoneHandler* iTelephoneHandler;

        CMPXCollectionPath* iCollectionPath;            // owned
        NShwSlideshow::TPlayDirection iPlayDirection;   // owned
    	
        /// Own: The TV connection monitor
    	CGlxTv *  iTvConnection;
		
        TBool iEngineStartFailed;

        /// Own: asynch callback needed for media list population
        CAsyncCallBack* iPopulateListCallBack;
		CEikButtonGroupContainer* iShwCba;
		
		//Flag for event generated by MSK
		TBool iMSKPressed;
		
		//Flag for event generated by LSK
		TBool iLSKPressed;
		
	//to keep track of whether the furniture is visible or not
	//and to toggle on tap	
	enum TShwFurniture
		{
		EFurnitureVisible = 1,
		EFurnitureHidden
		};
		
	TShwState iShwState;
	TShwFurniture iShwFurniture;	
	CShwGestureControl* iShwGestureControl;
	GestureHelper::CGestureControl* iGestureControl;
	CAlfControlGroup* iGestureControlGroup;
	CShwTicker* iTicker;
	CShwMediaKeyUtility* iMediaKeyHandler;
	CGlxHdmiController* iHdmiController;
	TBool iHdmiActive;
	CGlxMMCNotifier* iMMCNotifier;
	TBool iMMCState;
	TBool iIsForegrnd;
	TSize iScrnSize;
	TSize iGridIconSize;
	//to check if slideshow is paused in BG 
	//and after that we are bringing to foreground
	TBool iPrevNotInBackground;

	TInt iHdmiWidth;
    TInt iHdmiHeight;
    CAknWsEventMonitor* iAknEventMonitor;// not owned
    };

#endif  // C_SHWSLIDESHOWVIEW_H

// End of File

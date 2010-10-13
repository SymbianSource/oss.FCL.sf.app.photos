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
* Description:    Implementation of Fullscreen view
*
*/

#ifndef C_CGLXFULLSCREENIMP_C
#define C_CGLXFULLSCREENIMP_C

#include <alf/ialfwidgeteventhandler.h>
#include <mglxuicommandhandler.h>
#include <gesturehelper.h>
#include <gestureobserver.h>
#include <glximageviewermanager.h>
#include <alf/alfcompositionutility.h>

#include "glxfullscreenview.h"
#include "glxfullscreenbindingsetfactory.h"
#include "glxfullscreenview.hrh"
#include "glxmmcnotifier.h"
#include "glxtv.h"
#include "mglxhdmidecoderobserver.h" //for MGlxHDMIDecoderObserver

//Gesture Helper namespace 
namespace GestureHelper
    {
    class CGestureHelper;
    }

// FORWARD DECLARATIONS
class CAlfEnv;
class CGlxMediaListMulModelProvider;
class TGlxMulFullScreenBindingSetFactory;
class CEikButtonGroupContainer;
class CGlxActiveMediaListRegistry;
class CGlxZoomControl;
class CGlxSingleLineMetaPane;
class CGlxCommandHandlerAiwShowMapHardKey;
class CGlxDRMUtility;
class CGlxHdmiController;
class CGestureHelper;
class TGlxMedia;
class CGlxFullScreenBusyIcon;

namespace Alf
	{
   class IMulCoverFlowWidget;
   class IMulModel;
   class IMulSliderWidget;
   class IMulSliderModel;
	}
/*
 Defines the swipe direction
 */ 
enum TSwipe
    {
        EForward,EBackward
    };
    
NONSHARABLE_CLASS (CGlxFullScreenViewImp): public CGlxFullScreenView, 
                                            public IAlfWidgetEventHandler,
 											public MGlxUiCommandHandler,
			                                public MStorageNotifierObserver,
			                                public CAlfEffectObserver::MAlfEffectObserver,
											public MGlxTvObserver,
											public MGlxHDMIDecoderObserver
    {
public:    
    /**
    * Two-phased constructor.`
    *
    * @return Pointer to newly created object.
    */
    static CGlxFullScreenViewImp* NewL(MGlxMediaListFactory* aMediaListFactory,
                                 const TFullScreenViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle);

    /**
    * Two-phased constructor.
    *
    * @return Pointer to newly created object.
    */
    static CGlxFullScreenViewImp* NewLC(MGlxMediaListFactory* aMediaListFactory,
            const TFullScreenViewResourceIds& aResourceIds,TInt aViewUID, const TDesC& aTitle);
    
    /**
     * Destructor.
     */
    virtual ~CGlxFullScreenViewImp();

public:// CGlxViewBase
    virtual TBool HandleViewCommandL(TInt aCommand);

    void HandleCommandL(TInt aCommandId, CAlfControl* aControl) ;

    void HandleResourceChangeL (TInt aType);   

public:// CAknView
    void HandleForegroundEventL(TBool aForeground);

    TUid Id() const;

    void DoMLViewActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
        const TDesC8& aCustomMessage);
    void DoMLViewDeactivate();
    
      //Shows the exipry note/dialog for expired DRM files
      void ShowDrmExpiryNoteL();
      
	  // Callback for periodic timer, static, 
	  static TInt PeriodicCallback( TAny* aPtr );
	  //nonstatic func called from periodic timer
	  void CallPeriodicCallback();
public:
    //From IAlfWidgetEventHandler
    bool accept( CAlfWidgetControl& aControl, const TAlfEvent& aEvent ) const;
    
    AlfEventStatus offerEvent( CAlfWidgetControl& aControl, const TAlfEvent& aEvent );
    
    void setEventHandlerData( const AlfWidgetEventHandlerInitData& aData );
    
    AlfWidgetEventHandlerInitData* eventHandlerData();
    
    void setActiveStates( unsigned int aStates );
    
    IAlfInterfaceBase* makeInterface( const IfId& aType );
    
    AlfEventHandlerType eventHandlerType() ;

    AlfEventHandlerExecutionPhase eventExecutionPhase() ;
	
	//From MAlfEffectObserver
    void HandleEffectCallback(TInt aType, TInt aHandle, TInt aStatus);

public:
    //From MGlxHDMIDecoderObserver
    /*
     * Handle completion notification of HDMI Image Decoder.
     */
    void HandleHDMIDecodingEventL(THdmiDecodingStatus aStatus);

private:
    /*
     * Constructor 
     */
    CGlxFullScreenViewImp(const TFullScreenViewResourceIds& aResourceIds,TInt aViewUID);

    /*
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(MGlxMediaListFactory* aMediaListFactory,const TDesC& aTitle );
    
    /*
     *  Creates a coverflow widget
     */
    void CreateCoverflowWidgetL();
    
    /*
     *  Destroys Widgets, Coverflow and also calls DestroySliderWidget()
     */
     void DestroyWidgets();
     
    /*
     *  Destroys Slider widget
     */
     void DestroySliderWidget();
     
    /*
     * Activate zoomcontrol
     * @param:aTap: specifies whether zoom control is launched by tapping or by pressing volume keys
     */
    void ActivateZoomControlL(TZoomStartMode aStartMode, TPoint* aZoomFocus = NULL); 
    
    /*
     * DeActivate zoomcontrol
     */
    void DeactivateZoomControlL();
    
    /*
     * DeactivateFullScreen
     */
    void DeactivateFullScreen();
    
    /*
     * ActivateFullScreen
     */
    void ActivateFullScreenL();
    
    /*
    * Trun the UI on
    * @param aStartTimer:Incase of starting the zoom :we shouldn't be starting the timer.
    * else we  need to start the Ui timer to hide the screen furniture
    * so for the rest of the cases we need to have  aStartTimer = ETrue
    */
    void ShowUiL(TBool aStartTimer);
    
    /*
    * Hide the UI off
    * @param aHideSlider:When we start zooming using the slider widget,for continous zooming from the 
    * fullscreen view to zoom control, we need to have the slider visible so in this case we shouldnt 
    * be hiding the slider so pass EFalse,else case asliderstatus = ETrue
    */     
    void HideUi(TBool aHideSlider);
    
    /*
    * Sets the UI state
    */     
    void SetUiState (TUiState  aState);
    
    /*
    * returns the Ui state
    */     
    TUiState GetUiState();
    
    /*
    * Call back function for the CPeriodic
    */
    static TInt TimeOut(TAny* aSelf);
      
    /*
    * Calculates and returns the Initial Zoom factor
    */		
    TInt GetInitialZoomLevel();
    
    /*
    * Calls GetInitialZoomLevel() to get the Zoom Level and Sets the Value to the
    * Slider Level
    */
    void SetSliderToMin();
    
    /*
    * Creates and Initializes the slider model/widget
    */
    void CreateSliderWidgetL();
    
    /**
     * This function doesnt add any value, added to remove codescanner warnings
     */
    AlfEventStatus OfferEventL(const TAlfEvent& aEvent);
    /**
     * returns the swipe direction of coverflow
     */
    TSwipe GetSwipeDirection();
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
     * Consume DRM rights
     */  
    void ConsumeDRMRightsL(const TGlxMedia& aMedia);
    /**
     * Set the image to external display - HDMI
     */
    void SetItemToHDMIL();
    /**
     * handle MMC insertion
     */
    void HandleMMCInsertionL();
    /**
     * handle MMC removal
     */
    void HandleMMCRemovalL();
    
    /**
     * navigate to main list
     */
    void NavigateToMainListL();
	
    /**
     * Disable/enable the fullscreen toolbar
     */
    void EnableFSToolbar(TBool aEnable);
    
	/**
     * Create Image Viewer manager Instance
     */
    void CreateImageViewerInstanceL();
    
    /**
     * Delete Image Viewer manager Instance
     */
    void DeleteImageViewerInstance();
	
	/*
	 * Get the HDMI cable insert/removal notification.
     */
	void HandleTvStatusChangedL( TTvChangeType aChangeType );

    /**
     * Checks if the slider to be show for the focus index
     */
    TBool CheckIfSliderToBeShownL();

    /**
     * Update coverflow items after zoom/foreground event
     */
    void UpdateItems();

private:
    /** Softkey resource id's */
    TFullScreenViewResourceIds iResourceIds; 
    // /** Avkon unique ViewId */ 
    TInt iViewUid;

    /** Alf environment (not owned) */
    CAlfEnv* iEnv;

    /// Active media list registry stores the pointer to the active media list
    /// Allows UPnP to know when list has changed
    CGlxActiveMediaListRegistry* iActiveMediaListRegistry;

    /** Data Provider */
    CGlxMediaListMulModelProvider* iMediaListMulModelProvider;
    TGlxMulFullScreenBindingSetFactory iFullScreenBindingSet;

    //creating instance of viewWidget
    IAlfViewWidget* iViewWidget;
    
    /** The 2D Coverflow Widget ( owned )*/
    IMulCoverFlowWidget* iCoverFlowWidget;
    IMulSliderWidget*   iSliderWidget;
    IMulSliderModel*    iSliderModel;

    //creating zoomcontrol
    CGlxZoomControl* iZoomControl;
    
    // Holds the state of the Ui in Fullscreen
    TUiState iUiState;

    // used to turn the Ui off,if the screen is inactive for 10 sec
    CPeriodic* iTimer;
    CEikButtonGroupContainer* iZoomButtonGroup;
	CGlxCommandHandlerAiwShowMapHardKey* iShowOnMapHardKeyhandler;
    
    CGlxScreenFurniture* iScreenFurniture;
    CGlxHdmiController* iHdmiController;
	    
    CGlxDRMUtility* iDrmUtility;
    CPeriodic* iPeriodic;
    CGlxFullScreenBusyIcon* iBusyIcon;
	
    GestureHelper::CGestureHelper* iGestureHelper;
    TBool   iMultiTouchGestureOngoing   ;
    
    // stores the initial zoom percentage
    TInt iInitialZoomRatio ;
    //Previous focused index
    TInt iOldFocusIndex;
    TBool iImgViewerMode;
	CGlxMMCNotifier* iMMCNotifier;
	TBool iMMCState;
	TSize iScrnSize;   // ScrnSize
	TSize iGridIconSize; // grid icon size
    TBool iIsDialogLaunched;
    TBool iIsMMCRemoved;
	CAlfEffectObserver* iAlfEffectObs;
	TInt iEffectHandle;
    // For image viewer, not own
    CGlxImageViewerManager* iImageViewerInstance;
	CGlxTv*  iGlxTvOut;
	TInt iHdmiWidth;
	TInt iHdmiHeight;
    };

#endif


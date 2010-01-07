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
* Description:    Definition of CGlxZoomControl
 *
*/


#ifndef C_GLXZOOMCONTROL_H
#define C_GLXZOOMCONTROL_H

// INCLUDES

#include <w32std.h>            // For TEventCode

#include <alf/alfcontrol.h>
#include <alf/ialfwidgeteventhandler.h>
#include <gestureobserver.h>
#include <gesturehelper.h>

#include <mglxtextureobserver.h>
#include <mglxmedialist.h>
#include <mglxzoomeventhandlers.h>
#include <glxfullscreenview.hrh>
#include <glxzoomeventhandler.h>
#include <mglxtvobserver.h>     // for inteface MGlxTvObserver
//Gesture Helper namespace
namespace GestureHelper
    {
    class CGestureHelper;
    }


namespace Alf
    {
    class IAlfViewWidget;
    class IMulSliderWidget;
    class IMulSliderModel;
    class IAlfWidgetEventHandler;
    }
// FORWARD DECLARATIONS
class CGlxDefaultAttributeContext;
class CGlxUiUtility;
class MGlxUiCommandHandler;
class CAlfImageVisual;
class CAlfViewportLayout;
class TAlfImage;
class CEikButtonGroupContainer;
class CGlxTextureManager;
class CGestureHelper;
class MGestureObserver;

class CGlxTv;
using namespace Alf;

// Commands sent to the view in response to zoom keys
const TInt KGlxZoomInCommand    = 1 ;
const TInt KGlxZoomOutCommand   = 2 ;
const TInt KGlxVerticalImageOrientationThreshold   = 4 ;

/**
 * Control for zooming in the FullScreen view.
 * @ingroup glx_view_zoomed_view
 */
class CGlxZoomControl : public CAlfControl,
                        public IAlfWidgetEventHandler,
                        public MGlxTextureObserver,
                        public MGlxZoomEventHandlers,
                        public GestureHelper::MGestureObserver,
                        public MGlxTvObserver
    {
public:
    // Constructors and destructor
    /**
    * Two-phased constructor.
    * @param aCommandHandler: Handler for commands.
    * @param aMediaList     : Reference to media list.
    * @param aZoomKeys      : Reference to Back Key.
    * @param aSliderWidget  : Reference to  slider widget created in fullscreen view.
    */
    IMPORT_C static CGlxZoomControl* NewL(
    MGlxUiCommandHandler& aCommandHandler,
    MGlxMediaList& aMediaList,CEikButtonGroupContainer& aZoomKeys,
    IMulSliderWidget& aSliderWidget, GestureHelper::CGestureHelper* aGestureHelper);

    /**
    * Destructor.
    */
    ~CGlxZoomControl();

    /**
    * Show the control and begin zooming in.
    * @param aIntialZoomLevel:The Initial Zoom Level shown when zoom is active from full screen view.
    * @param aTap            :if the zoom was launched by tapping or by pressing volume key.
    */
    IMPORT_C void ActivateL(TInt aIntialZoomRatio,TZoomStartMode aStartMode,TInt aFocusIndex, 
                                    TGlxMedia& aItem,  TPoint* aZoomFocus = NULL);

    /**
    * Hide the control and disable zooming.
    */
    IMPORT_C void Deactivate();

    /**
    * to check if the zoom  control is activated?
    */
    IMPORT_C TBool Activated();

    /**
    * Zoom Foreground event handling function
    * @param aForeground: value signify if it is in foreground or not.
    */
    IMPORT_C void HandleZoomForegroundEvent(TBool aForeground);

    void UpdateViewPort(
            TPoint& aViewPortTopLeft ,
            TInt aTransitionTime ,
            TSize  *apViewPortDimension = NULL,
            TInt aPrimarySliderLevel = -1);
private:// From MGlxTvObserver
    virtual void HandleTvStatusChangedL ( TTvChangeType aChangeType );

private:  // From CAlfControl
    TBool OfferEventL(const TAlfEvent &aEvent);

    void VisualLayoutUpdated(CAlfVisual &aVisual);

private:  //From IAlfWidgetEventHandler
    AlfEventStatus offerEvent( CAlfWidgetControl& /*aControl*/, const TAlfEvent& aEvent );

    bool accept( CAlfWidgetControl& aControl, const TAlfEvent& aEvent ) const;

    void setEventHandlerData( const AlfWidgetEventHandlerInitData& aData );

    AlfWidgetEventHandlerInitData* eventHandlerData();

    void setActiveStates( unsigned int aStates );

    IAlfInterfaceBase* makeInterface( const IfId& aType );

    AlfEventHandlerType eventHandlerType() ;

    AlfEventHandlerExecutionPhase eventExecutionPhase() ;

private: // From MGlxTextureObserver
    void TextureContentChangedL( TBool aHasContent , CAlfTexture* aNewTexture);

private: // From MGlxZoomEventHandlers
    void HandleViewPortParametersChanged(TPoint& aViewPortTopLeft ,
            TInt aTransitionTime ,
            TSize  *apViewPortDimension = NULL,
            TInt aPrimarySliderLevel = -1);

    void HandleShowUi(TBool aShow= EFalse) ;

    void HandleZoomOutL(TInt aCommandId);
private:
        /*
        * Call back function for the CPeriodic
        */
       static TInt TimeOut(TAny* aSelf);
       void ActivateFullscreen();
       void StartZoomAnimation();

private:
    /**
    * C++ default constructor.
    */
    CGlxZoomControl(MGlxUiCommandHandler& aCommandHandler,
            MGlxMediaList& aMediaList,CEikButtonGroupContainer& aZoomKeys,
            IMulSliderWidget& aSliderWidget, GestureHelper::CGestureHelper* aGestureHelper);
    /**
    * Does the necessary Initialization of iUiUtility,control group and the Media list.
    */
    void ConstructL();
    /**
    * Creation of zoomvisual:creates the zoom visual and sets the Black Background behind the image
    * and also sets the ViewPort Layout.
    */
    void CreateZoomVisualL();
    /**
    * Turn the UI/Screen Furniture[Back Key/Slider] on
    */
    void ShowUi(TBool aShow);

    /**
    * Cleanup function resets all the memeber variable on deactivating the ZoomControl
    */
    void CleanUpVisual();

    /*
    * Get Orientation of the file ,
    * This code is DUPLICATE and second copy implemented in bitmapdecoder,
    * Got to plan for removing this duplicacy
    */
    TUint16 GetOrientationL(const TDesC& aFileName) ;

    /*
    * This Function Shows/Hides the zoom
    * by setting the level of Opacity
    */
    void ShowZoom(TBool aShow);

    /*
    * Retrieves the Screensize with the help of Alf utility
    */
    TSize ScreenSize();

    void HandleGestureL( const GestureHelper::MGestureEvent& aEvent );

    TBool HandlePointerEventsL(const TAlfEvent &aEvent);
    
    TInt GetInitialZoomLevel(TSize& aSize );

private:    // Data
    CAlfEnv* iEnv;                                      // AlfEnv
    MGlxMediaList& iMediaList;                          // Medialist (not owned)
    GestureHelper::CGestureHelper* iGestureHelper;                     // Attribute context for image dimensions
    IMulSliderModel* iZoomSliderModel;                  // Zoom slider model
    IMulSliderWidget& iZoomSliderWidget;                // Zoom slider Widget
    CEikButtonGroupContainer* iZoomBackKey;             // Back zoom soft key(not owned)
    CGlxDefaultAttributeContext* iAttributeContext;     // Attribute context for image dimensions

    CAlfDisplay* iDisplay;                              // Alf Display
    CGlxUiUtility* iUiUtility;                          // UI utility
    CGlxTextureManager* iTextureMgr;                    // TextureManager
    CAlfControlGroup* iControlGroup;                    // Control group for the Zoom control

    CAlfViewportLayout* iViewPort;                      // Parent Layout for Zooming/Panning
    CAlfImageVisual* iImageVisual;                      // Child visual for normal image items (not owned)
    CAlfTexture* iImageTexture;                         // Texture to be zoomed

    TBool iIsdrag;                                      // To determine if drag occured to evaluate the single tap
    TBool iZoomActive;                                  // This variable is updated when Zoom is activated.
    TSize iOriginalDimensions;

    MGlxUiCommandHandler& iCommandHandler;              // For Handling ZoomOut commands
    CGlxZoomPanEventHandler* iEventHandler;             // The event handler for the Zoom Pan Engine

    TSize iScreenSize;
    TBool iMultiTouchGestureOngoing;
    CGlxTv*  iGlxTvOut;
    CPeriodic* iTimer;
    TBool iZoomIn;
    };

#endif  // C_GLXZOOMCONTROL_H

// End of File

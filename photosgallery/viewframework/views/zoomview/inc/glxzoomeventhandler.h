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
* Description:    Zoom Event Handler
*
*/

#ifndef GLXZOOMEVENTHANDLER_H_
#define GLXZOOMEVENTHANDLER_H_

#include "glxzoomview.hrh"
#include "glxfullscreenview.hrh"
#include "glxzoommathsengine.h"

//Alf Headers
#include <alf/alfevent.h>                   //  For Key Events
#include <alf/alftimedvalue.h>              // for TAlfRealPoint
#include <alf/alftypes.h>

#include <alf/alftypes.h>
#include <gesturehelper.h>
#include <gestureobserver.h>

// FORWARD DECLARATIONS
class CGlxZoomControl;
class TGlxZoomAndPanMathsEngine; 
class MGlxZoomEventHandlers;

class CGlxZoomPanEventHandler : public CBase
    {
public:
    /**
    * Two-phased constructor.
    * @param aZoomEventHandler: Handler for ZoomControl commands.
    */
    static CGlxZoomPanEventHandler* NewL(MGlxZoomEventHandlers& aZoomEventHandler );
    
    /**
    * Destructor.
    */
    ~CGlxZoomPanEventHandler();

    /**
    * Two stop shop to handle all alf events. [PRASH] Make this a one stop shop
    */     
    TBool HandleEventL(const TAlfEvent &aEvent );
    
    TBool HandleEvent( const TAlfEvent& aEvent );

    /**
    * Cancel the timer for continous zooming
    */     
    void CancelZoomPanTimer();
    
    /*
     * ActivateZoom from the Zoom control
     */
    void ActivateZoom(TInt initialZoomRatio, 
                        TSize imageSize, 
                        TZoomStartMode aStartMode, 
                        TInt aMinSliderRange, 
                        TInt aMaxSliderRange,
                        TSize& aOriginalDimensions, 
                        TPoint* aZoomFocus);
    
    /*
     * DeactivateZoom from the Zoom control
     */
    void DeactivateZoom();
    
    /*
     * OrientationChanged
     */
    void OrientationChanged(const TRect& aNewScreenRect) ;
    
    /*
     * ZoomIsActivated
     * This is called when the zoom is activated/deactivated from Zoom control
     */
    void SetZoomActivated(TBool aIsActivated = EFalse);
    
    /*
     * ZoomUiState 
     * This is called when the zoom state in Zoom control is changed
     */
    void SetZoomUiState(TUiState aZoomUiState);
    
    /*
     * ZoomUiState 
     * This is called when the zoom state in Zoom control is changed
     */
    TUiState ZoomUiState();
    
    /**
    * Handle the drag events for panning
    */
    void HandleDragEvent(const GestureHelper::MGestureEvent& aEvent );
    
    void HandlePinchEventL(const GestureHelper::MGestureEvent& aEvent);
    
    void HandleDoubleTap(const GestureHelper::MGestureEvent& aEvent );
    
    void HandleSingleTap(const GestureHelper::MGestureEvent& aEvent );
    
    void HandleGestureReleased(const GestureHelper::MGestureEvent& aEvent );
    
    void SetPreviousEventCode(const GestureHelper::TGestureCode code );
    
    void HandleMultiTouchReleased();
    
private:
    /*
     * Constructor
     */
    CGlxZoomPanEventHandler(MGlxZoomEventHandlers& aZoomEventHandler);
    
    /*
     * ConstructL
     */
    void ConstructL();

    /**
    *  This Function handles all the KeyEvents 
    */
    TBool HandlekeyEvents(const TAlfEvent &aEvent);
    
    /**
    * Callback function for zoom 
    */     
    static TInt ZoomIntervalExpired(TAny* aPtr);
    
    /**
    * Callback function for pan
    */ 
    static TInt PanIntervalExpired(TAny* aPtr);
    
    /**
    * Start zoom in/out continously
    */     
    void DoZoom();
    
    // Zoom by the next zoom increment.
    // aZoomMode is ignored if aExpectedZoomLevel has a positive value
    void Zoom(TInt aExpectedZoomLevel, TInt aRelativeZoomFactor, TZoomMode aZoomMode = EZoomIn, TPoint* aZoomFocus = NULL);
    /**
    * Start Panning continously
    */     
    void DoPan();
    
    /**
    * Starts the timer for continous zooming
    */     
    void StartZoomTimer();
    
    /**
    * Starts the timer for continous Panning
    */ 
    void StartPanTimer();
    
    /**
    * Stops the timer for showing the UI. 
    */ 
    void CancelUITimer();
    
    /**
    * Starts the timer for showing the UI.
    */ 
    void StartUITimer(TTimeIntervalMicroSeconds32 aDelay,
            TTimeIntervalMicroSeconds32 anInterval,
            TCallBack aCallBack) ;
    
    /**
    *Call back function to Turn off UI/Screen Furniture[Back Key/Slider].
    */
    static TInt UiTimeOut(TAny* aSelf);
    
    /*
     * Timer callback for zoomout 
     */
    static TInt ZoomOutTimerL(TAny* aSelf);
    
    /**
    * Handle zoom key pressed or released.
    * @param aZoomIn    :ETrue for Zoom In key, EFalse for Zoom Out key
    * @param aEventCode :Type of key event.
    */
    void HandleZoomKey(TZoomMode aZoomMode, const TEventCode aEventCode);
    
    /**
    * Handle the key events for panning
    * @param aPanDirection: Specifies the pan direction
    * @param aEvent       : Specifies the event values.
    */
    TBool HandlePanKey(const TAlfEvent &aEvent);
    
    /**
    * Handle zoom stripe key pressed.
    * @param aZoomIn    :ETrue for Zoom In key, EFalse for Zoom Out key
    * @param aEventCode :Type of key event.
    */
    void HandleZoomStripeAction(TZoomMode aZoomMode ,TEventCode aEventCode);
    
    /**
    * Handle the drag events for panning
    */
    void HandleDragEvent(const TPoint &aTapPoint);
    
    /*
     * ShowSliderTillTimeout
     */
    void ShowSliderTillTimeout() ;
    
    /*
     * ShowScreenFurniture
     * aTimeout == 0 means no timeout.
     */
    void ShowScreenFurniture(TTimeIntervalMicroSeconds32 aTimeout);
    
    /*
     * HideScreenFurniture
     */
    void HideScreenFurniture();
    
    /*
     * SetupPanOperation
     */
    void SetupPanOperation(TPoint& aPanDirection);

    void CallZoomOutL();    

    void SetupAnimatedZoom(TZoomMode aZoomMode, TPoint * aZoomFocus = NULL);
    
    static TInt ActivationIntervalElapsed(TAny* aPtr) ;
    
    void NextStepAnimatedZoom();
        
    void SetupAnimatedPan() ; 
    
    static TInt PanInertiaFrameElapsed(TAny* aPtr);

    void NextStepInerticPan();

    void CancelAnimationTimer();
    

    
private:
    MGlxZoomEventHandlers&  iZoomEventHandler; 

    TTime               iPanTime;                       // The Purpose of this Variable is to determine the Time,for how long the key was held
    TZoomMode           iZoomMode;                      // Says whether we are zooming in or out.  
    TPoint              iPanDirection;                  // This gives the direction of the panning that has to be done
    TInt                iMinZoomRatio;                // The minimum value that the Zoom Slider will have  
    TInt                iMaxZoomRatio;                // The maximum value that the Zoom Slider will have

    CPeriodic*          iUiTimer;                       // The Timer used to Hide the UI/Screen Furniture[Back Key/Slider] after 2 seconds
    CPeriodic*          iZoomPanTimer;                  // The Timer used  for Zooming/Panning the Image Exponentially
    TPoint              iStartDisplayPoint;             // The variable holds the starting pointer posistion to determine the Drag Distance
    TTime               iDoubleTap;                     // This Variable is used to determine the double tap,by using the difference of the time 
                                                        // between subsequent single Tap.
    // [todo] do we need this variable to be global
    TInt                iZoomRatio;
    TRect               iDoubleTapRect;                 // This varaible holds the rect for the Double Tap
    TBool               iDragOngoing;                   // Is a drag event ongoing
    TBool               iZoomActivated;                 // To Denote if zoom is activated
    TUiState            iZoomUiState;                   // To Denote if the Ui is On in zoom
    
    TPoint              iPreviousPointerPosition;
    TPoint              iPreviousDragStartPosition;

    // for the animated zoom to 50 % allowed. 
    TBool               iIsZoomingInAnimatedState;
    TAnimationMode      iAnimatedZoomMode;
    CPeriodic*          iZoomAnimationTimer;
    TInt                iZoomPerInterval;
    TInt                iTargetAnimatedZoomRatio;
    // for the animated zoom to 50 % allowed. END 

    // For considering Zoom Focus. 
    TPoint              iZoomFocus;            // Not used right now. WIll soon carry the coordinates of the zoom focus. 
    // For considering Zoom Focus. END 
    
    // For Pan Inertia.
    GestureHelper::TGestureCode        iPreviousGestureCode ;
    // For Pan Inertia. END
    
    
    
    TGlxZoomAndPanMathsEngine iMathsEngine;             // The new Maths engine at the core of the Zoom Component
    
    
    };


#endif /* GLXZOOMEVENTHANDLER_H_ */
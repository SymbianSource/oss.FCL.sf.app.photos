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
* Description:    Implementation of Zoom Event Handler
*
*/


// Avkon 
#include <eikon.hrh>

// Antariksh
#include <alf/alfutil.h>                    // AlfUtil
#include <mul/mulevent.h>                   //  For Slider events

//Photos Headers
#include <glxtracer.h>                      //  For Tracer
#include <glxlog.h>                         //  For Log

#include <mglxuicommandhandler.h>           //  For MGlxUiCommandHandler
#include <glxzoomeventhandler.h>
#include <glxzoommathsengine.h>
#include <glxzoomcontrol.h>
#include <glxzoomeventhandler.h>


// LOCAL CONSTANTS AND MACROS
const TInt KTimerLengthInMicroseconds   = 100000    ;
const TInt KGlxScreenTimeout            = 2000000	;      // 2 seconds timer for UI On/Off

const TInt KGlxAnimationTimeDrag        = 30        ;
const TInt KGlxAnimationTimekeyPan      = 50	    ;

const TInt KGlxZoomPanInc               = 5         ; // Min number of pixels panned in one keypress. This value is incremented exponentially by multiples of iPanFactor 

// Anime Speed changes in inverse proportion to KGlxAnimeFrameInmS. 
// Steps increase with KGlxAnimeFrameCount.
// Total time taken = (KGlxAnimeFrameInmS * KGlxAnimeFrameCount)microseconds.
const TInt KGlxAnimeFrameInmS           = 10000     ;
const TInt KGlxAnimeFrameCount          = 10        ;                 

const TInt KGlxPanInertiaFrameInmS      = 20000     ;

// The (neutral) zoom factor above which all zooms caused are zoom-ins and below , zoom-outs.
const TInt KGlxNeutralZoomFactor = 100;

using namespace GestureHelper;

// ============================ CGlxZoomPanEventHandler===============================
// ============================ MEMBER FUNCTIONS ===============================

//----------------------------------------------------------------------------------
// NewL
//----------------------------------------------------------------------------------
//
CGlxZoomPanEventHandler* CGlxZoomPanEventHandler::NewL(
        MGlxZoomEventHandlers& aZoomEventHandler)
    {
    TRACER("CGlxZoomPanEventHandler::NewL");
    CGlxZoomPanEventHandler* self = new (ELeave) CGlxZoomPanEventHandler(
            aZoomEventHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

//----------------------------------------------------------------------------------
// CGlxZoomPanEventHandler constructor
//----------------------------------------------------------------------------------
//
CGlxZoomPanEventHandler::CGlxZoomPanEventHandler(
        MGlxZoomEventHandlers& aZoomEventHandler) :
    iZoomEventHandler(aZoomEventHandler), iPrevPinchPercentage(
            KGlxNeutralZoomFactor)
    {
    TRACER("CGlxZoomPanEventHandler::CGlxZoomPanEventHandler()");
    // No Implementation
    }

//----------------------------------------------------------------------------------
// CGlxZoomPanEventHandlerDestructor
//----------------------------------------------------------------------------------
//
CGlxZoomPanEventHandler::~CGlxZoomPanEventHandler()
    {
    TRACER("CGlxZoomPanEventHandler::~CGlxZoomPanEventHandler()");
    if(iZoomPanTimer)
    	{
        CancelZoomPanTimer();
        delete iZoomPanTimer;
        iZoomPanTimer = NULL;
        }
    if (iUiTimer)
        {
        CancelUITimer();
        delete iUiTimer;
        iUiTimer = NULL;
  		}
    if(iZoomAnimationTimer)
        {
        CancelAnimationTimer();
        delete iZoomAnimationTimer;
        iZoomAnimationTimer = NULL;
        }
    }

//----------------------------------------------------------------------------------
// CGlxZoomPanEventHandler Constructl
//----------------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::ConstructL()
    {
    TRACER("CGlxZoomPanEventHandler::ConstructL()");
    if(!iUiTimer)
        {
        iUiTimer = CPeriodic::NewL(EPriorityNormal);
        }
    if(!iZoomPanTimer)
        {
        iZoomPanTimer = CPeriodic::NewL(EPriorityNormal);
        }
    if(!iZoomAnimationTimer)
        {
        iZoomAnimationTimer = CPeriodic::NewL(EPriorityNormal);
        }
    }

//----------------------------------------------------------------------------------
// CGlxZoomPanEventHandler SetupAnimatedZoom
//----------------------------------------------------------------------------------
//
// Todo: Combine logics of setting up animated zoom and pan.
void CGlxZoomPanEventHandler::SetupAnimatedZoom(TZoomMode aZoomMode, 
        TPoint* /*aZoomFocus*/)
    {
    TRACER("CGlxZoomPanEventHandler::SetupAnimatedZoom");

    iTargetAnimatedZoomRatio = (iMaxZoomRatio + iMinZoomRatio) / 2;

    CancelAnimationTimer();
    CancelUITimer();

    //ToDo: Verify this with images slightly smaller or slightly larger than fullscreen size. 
    if (EZoomIn == aZoomMode)
        {
        // the '1+' is to take care of the situation when there the rest of the equation returns something betn 0 and 1
        iZoomPerInterval = 1 + (iTargetAnimatedZoomRatio - iZoomRatio)
                / KGlxAnimeFrameCount;
        }
    else
        {
        iZoomPerInterval = 1 + (iZoomRatio - iMinZoomRatio)
                / KGlxAnimeFrameCount;
        }

    if (1 < iZoomPerInterval)
        {
        iIsZoomingInAnimatedState = ETrue;

        switch (aZoomMode)
            {
            case EZoomIn:
                iAnimatedZoomMode = EAnimationModeZoomIn;
                break;
            case EZoomOut:
                iAnimatedZoomMode = EAnimationModeZoomOut;
                break;
            }

        iZoomAnimationTimer->Start(0, KGlxAnimeFrameInmS, TCallBack(
                ActivationIntervalElapsed, (TAny*) this));
        }
    else
    // Cant zoom in/out at less than 1 percent. so directly jump to the target zoom ratio.
    // This happens in cases where there is not much difference between the source and target zoom levels
        {
        TInt targetZoomRatio = 0;
        if (EZoomIn == aZoomMode)
            {
            targetZoomRatio = iTargetAnimatedZoomRatio;
            }
        else
            {
            targetZoomRatio = iMinZoomRatio;
            }
        Zoom(targetZoomRatio, 0);
        }
    }

//----------------------------------------------------------------------------------
// CGlxZoomPanEventHandler ActivationIntervalElapsed
//----------------------------------------------------------------------------------
//
TInt CGlxZoomPanEventHandler::ActivationIntervalElapsed(TAny* aPtr)
    {
    TRACER("CGlxZoomPanEventHandler::ActivationIntervalElapsed");

    CGlxZoomPanEventHandler* self = static_cast<CGlxZoomPanEventHandler*>(aPtr);
    
    self->NextStepAnimatedZoom();
    return 0;
    }

//----------------------------------------------------------------------------------
// CGlxZoomPanEventHandler NextStepAmimatedZoom
//----------------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::NextStepAnimatedZoom()
    {
    TRACER("CGlxZoomPanEventHandler::NextStepAmimatedZoom");
    
    TInt targetZoomLevel = 0 ;
    
    if (EAnimationModeZoomIn == iAnimatedZoomMode)
        {
        targetZoomLevel = iZoomRatio + iZoomPerInterval;
        }
    else
        {
        targetZoomLevel = iZoomRatio - iZoomPerInterval;
        }
            
    Zoom(targetZoomLevel, 0, EZoomIn, &iZoomFocus );

    // The boundary conditions. 
    if (((targetZoomLevel >= iTargetAnimatedZoomRatio)
            && (EAnimationModeZoomIn == iAnimatedZoomMode))
            || ((targetZoomLevel <= iMinZoomRatio) 
                    && (EAnimationModeZoomOut == iAnimatedZoomMode)))
        {
        iIsZoomingInAnimatedState = EFalse;
        CancelAnimationTimer();
        TSize screensize = iMathsEngine.ScreenSize();
        iZoomFocus = TPoint(screensize.iWidth>>1,screensize.iHeight>>1) ;
        
        //start the timer here after the animation is complete
        if(EUiOn == iZoomUiState && iZoomActivated)
            {
            ShowScreenFurniture(KGlxScreenTimeout);
            }
        }
    }

//----------------------------------------------------------------------------------
// StartPanTimer:Starts the Pan timer for continous Panning
//----------------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::StartPanTimer()
    {
    TRACER("CGlxZoomPanEventHandler::StartPanTimer ");

    //After Panning in one direction for a long time by holding the key and 
    //then if panning is started in another direction,the panning will be done in more pixels.
    
    GLX_LOG_INFO("CGlxZoomPanEventHandler::StartPanTimer: Cancelling timers ");

    if (iZoomPanTimer->IsActive())
        {
        iZoomPanTimer->Cancel();
        }
    iZoomPanTimer->Start( 0,  
            KTimerLengthInMicroseconds, 
            TCallBack( PanIntervalExpired,(TAny*)this) );

    }
    
//----------------------------------------------------------------------------------
// CancelZoomPanTimer: Cancels the Zoom timer for continous zoom
//----------------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::CancelZoomPanTimer()
    {
    TRACER("CGlxZoomPanEventHandler::CancelZoomPanTimer ");
    
    if (iZoomPanTimer->IsActive())
        {
        iZoomPanTimer->Cancel();
        }
    }

//----------------------------------------------------------------------------------
// CancelUITimer
//----------------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::CancelUITimer()
    {
    TRACER("CGlxZoomPanEventHandler::CancelUITimer ");
    
    if (iUiTimer->IsActive())
        {
        iUiTimer->Cancel();
        }
    }

//----------------------------------------------------------------------------------
// CancelUITimer
//----------------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::CancelAnimationTimer()
    {
    TRACER("CGlxZoomPanEventHandler::CancelAnimationTimer ");
    
    // This will set the timer to false if it is being used for zoom. For Pan
    // this flag is immaterial
    iIsZoomingInAnimatedState = EFalse;
    if (iZoomAnimationTimer->IsActive())
        {
        iZoomAnimationTimer->Cancel();
        }
    }

//----------------------------------------------------------------------------------
// StartUITimer
//----------------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::StartUITimer(TTimeIntervalMicroSeconds32 aDelay,
        TTimeIntervalMicroSeconds32 anInterval,
        TCallBack aCallBack)
    {
    TRACER("CGlxZoomPanEventHandler::StartUITimer ");
    
    if (iUiTimer->IsActive())
        {
        iUiTimer->Cancel();
        }
    iUiTimer->Start(aDelay,anInterval,aCallBack);
    }

// -----------------------------------------------------------------------------
// HandlekeyEvents:This function handles the key Events.
// -----------------------------------------------------------------------------
//
TBool CGlxZoomPanEventHandler::HandlekeyEvents(const TAlfEvent &aEvent)
    {
    TRACER("CGlxZoomPanEventHandler::HandlekeyEvents()");
    GLX_LOG_INFO1("CGlxZoomPanEventHandler::HandlekeyEvents.Scancode = %d", 
            aEvent.KeyEvent().iScanCode);

    TBool consumed = EFalse;
    
    if (!iIsZoomingInAnimatedState)
        {
        switch (aEvent.KeyEvent().iScanCode)
            {
            case EStdKeyDevice10: //Listen to EStdKeyDevice10 as EKeyLeftUpArrow key is mapped to TKeyCode::EKeyDevice10 for which TStdScancode is EStdKeyDevice10
            case EKeyLeftUpArrow :
            case EStdKeyDevice13: //Listen to EStdKeyDevice13 as EKeyLeftDownArrow key is mapped to TKeyCode::EKeyDevice13 for which TStdScancode is EStdKeyDevice13    
            case EKeyLeftDownArrow :
            case EStdKeyDevice11://Listen to EStdKeyDevice11 as EKeyRightUpArrow key is mapped to TKeyCode::EKeyDevice11 for which TStdScancode is EStdKeyDevice11            
            case EKeyRightUpArrow :
            case EStdKeyDevice12: //Listen to EStdKeyDevice12 as EKeyRightDownArrow key is mapped to TKeyCode::EKeyDevice12 for which TStdScancode is EStdKeyDevice12
            case EKeyRightDownArrow :
            case EStdKeyLeftArrow :
            case EStdKeyRightArrow :
			case EStdKeyUpArrow :
			case EStdKeyDownArrow :
                {
                HandlePanKey(aEvent);
                consumed = ETrue;
                }
                break;
            case EStdKeyDevice0:
            case EStdKeyDevice1:                
            case EStdKeyDevice3:
                {
                //Center button/MSK changes the UI State
                if (aEvent.Code() == EEventKeyDown)
                    {
                    if (EUiOff == iZoomUiState )
                        {
                        ShowScreenFurniture(KGlxScreenTimeout);
                        }
                    else
                        {
                        HideScreenFurniture();
                        }
                    }
                consumed = ETrue;
                }
                break;
    
            //Listen EStdKeyApplicationC as EKeyZoomIn key is mapped to TKeyCode:: EKeyApplicationC for which TStdScancode is EStdKeyApplicatoinC
            case EStdKeyApplicationC :
                {
                HandleZoomKey(EZoomIn ,aEvent.Code()) ;
                consumed = ETrue;
                break ;
                }
            //Listen EStdKeyApplicationD as EKeyZoomOut key is mapped to TKeyCode:: EKeyApplicationD for which TStdScancode is EStdKeyApplicatoinD
            case EStdKeyApplicationD :
                {
                HandleZoomKey(EZoomOut,aEvent.Code());
                consumed = ETrue;
                break ;
                }                
            default:
                break;
            }
        }

    return consumed;
    }

// -----------------------------------------------------------------------------
// HandleZoomKey:Zooms the image on zoom stripe action.
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HandleZoomKey(TZoomMode aZoomMode,
        TEventCode aEventCode)
    {
    TRACER("CGlxZoomPanEventHandler::HandleZoomKey ");
    if ( iZoomActivated )
        {
        switch(aEventCode)
            {
            case EEventKey :
                {
                iZoomMode = aZoomMode ;
                Zoom(0, 0, aZoomMode) ;

                ShowScreenFurniture(KGlxScreenTimeout);
                break ;
                }
            default :
            break ;
            }
        }
    }

// -----------------------------------------------------------------------------
// SetupPanOperation: start the pan operation for Key based pan. 
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::SetupPanOperation(TPoint& aPanDirection)
    {
    TRACER("CGlxZoomPanEventHandler::SetupPanOperation ");
    
    iPanDirection = TPoint(aPanDirection);
    if(EUiOn == iZoomUiState )
        {
        HideScreenFurniture();
        }
    
    StartPanTimer();
    
    iMathsEngine.SetupPanOperation();
    }

// -----------------------------------------------------------------------------
// HandlePanKey :handles the keys related to pan and starts the pan Timer
// -----------------------------------------------------------------------------
//
TBool CGlxZoomPanEventHandler::HandlePanKey( const TAlfEvent &aEvent )
    {
    TRACER("CGlxZoomPanEventHandler::HandlePanKey ");
    
    TSize imageVirtualSize   = iMathsEngine.ImageVirtualSize();
    TSize screenSize         = iMathsEngine.ScreenSize();
    TBool safeToPan          = EFalse ;
    TPoint panDirection(0,0);
    TBool handled = EFalse;

    if ( aEvent.Code() == EEventKeyDown )
        {
        //if height and/or width of the zoomed image is greater than the screen height and screen width ,
        //then this paning should be posssible 
        switch(aEvent.KeyEvent().iScanCode )
            {
            case EStdKeyDevice10:
            case EKeyLeftUpArrow:
                {
                if ((imageVirtualSize.iHeight>screenSize.iHeight)
                        &&(imageVirtualSize.iWidth > screenSize.iWidth))
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(-KGlxZoomPanInc, -KGlxZoomPanInc);
                    }
                }
                break;    
            case EStdKeyDevice11:
            case EKeyRightUpArrow:
                {
                if ((imageVirtualSize.iHeight>screenSize.iHeight)
                        &&(imageVirtualSize.iWidth > screenSize.iWidth))
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(KGlxZoomPanInc, -KGlxZoomPanInc);
                    }
                }
                break;    
            case EStdKeyDevice12:
            case EKeyLeftDownArrow:
                {
                if ((imageVirtualSize.iHeight>screenSize.iHeight)
                        &&(imageVirtualSize.iWidth > screenSize.iWidth))
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(-KGlxZoomPanInc, KGlxZoomPanInc);
                    }
                }
                break;    
            case EStdKeyDevice13:
            case EKeyRightDownArrow:
                {
                if ((imageVirtualSize.iHeight>screenSize.iHeight)
                        &&(imageVirtualSize.iWidth > screenSize.iWidth))
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(KGlxZoomPanInc, KGlxZoomPanInc);
                    }
                }
                break;    
            case EStdKeyUpArrow:
                {
                if( (imageVirtualSize.iHeight ) > screenSize.iHeight )
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(0, -KGlxZoomPanInc);
                    }
                }
                break;      
            case EStdKeyDownArrow:
                {
                if( (imageVirtualSize.iHeight ) > screenSize.iHeight )
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(0, KGlxZoomPanInc);
                    }
                }
                break;      
            case EStdKeyLeftArrow:
                {
                if( (imageVirtualSize.iWidth ) > screenSize.iWidth )
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(-KGlxZoomPanInc, 0);
                    }
                }
                break;      
            case EStdKeyRightArrow:
                {
                if ((imageVirtualSize.iWidth ) > screenSize.iWidth )
                    {
                    safeToPan = ETrue;
                    panDirection = TPoint(KGlxZoomPanInc, 0);
                    }
                }
                break;        
            default:
                break;
            }
        if (safeToPan)
            {
            SetupPanOperation(panDirection);
            handled = ETrue;
            }
        }
    else if ( aEvent.Code() == EEventKeyUp)
        {
        // destroy the Pan infrastructure
        // Reset the PanDirection and cancel the pan timers.
        CancelZoomPanTimer();
        iPanDirection = TPoint(0, 0);
        handled = ETrue;
        }
    return handled;
    }


// -----------------------------------------------------------------------------
// HandleDragEvent
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HandleDragEvent(
        const GestureHelper::MGestureEvent& aEvent)
    {
    TRACER("CGlxZoomPanEventHandler::HandleDragEvent(GestureHelper::MGestureEvent&)");
    
    // Ignore events when we are animating in Zoom
    if (iIsZoomingInAnimatedState)
        {
        return;
        }
    
    TPoint startPos = aEvent.StartPos(); 
    TPoint currPos  = aEvent.CurrentPos();

    
    // This means a new gesture has just started.
    if (startPos != iPreviousDragStartPosition)
        {
        iPreviousPointerPosition = startPos;
        }
    
    TPoint offset((iPreviousPointerPosition.iX - currPos.iX),
            (iPreviousPointerPosition.iY - currPos.iY));
    
    HideScreenFurniture();

    TPoint topLeftCorner(0,0);    
    iMathsEngine.Pan(offset, topLeftCorner, EGlxPanIncrementUniform);
    
    iZoomEventHandler.HandleViewPortParametersChanged(topLeftCorner,
            KGlxAnimationTimeDrag);
    
    iPreviousPointerPosition = currPos ;
    iPreviousDragStartPosition = startPos;
    }

// -----------------------------------------------------------------------------
// HandleGestureReleased
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HandleGestureReleased(
        const GestureHelper::MGestureEvent& /*aEvent*/)
    {
    TRACER("CGlxZoomPanEventHandler::HandleGestureReleasedEvent(const GestureHelper::MGestureEvent& )");
    
    if ( (EGestureSwipeLeft  == iPreviousGestureCode)
          ||(EGestureSwipeRight == iPreviousGestureCode)
          ||(EGestureSwipeUp    == iPreviousGestureCode)
          ||(EGestureSwipeDown  == iPreviousGestureCode)
          )
        {
        GLX_LOG_INFO1("CGlxZoomPanEventHandler::HandleGestureReleased: Previous Gesture Code [%d]" , iPreviousGestureCode);
        SetupAnimatedPan();
        }
    }

void CGlxZoomPanEventHandler::SetupAnimatedPan()
    {
    TRACER("CGlxZoomPanEventHandler::SetupAnimatedPan");

    CancelAnimationTimer();
    
    iZoomAnimationTimer->Start( KGlxPanInertiaFrameInmS,  
            KGlxPanInertiaFrameInmS, 
            TCallBack( PanInertiaFrameElapsed,(TAny*)this) );
    
    }


TInt CGlxZoomPanEventHandler::PanInertiaFrameElapsed(TAny* aPtr)
    {
    TRACER("CGlxZoomPanEventHandler::PanInertiaFrameElapsed");

    CGlxZoomPanEventHandler* self = static_cast<CGlxZoomPanEventHandler*>(aPtr);
    
    self->NextStepInerticPan();
    return 0;

    }


void CGlxZoomPanEventHandler::NextStepInerticPan()
    {
    TRACER("CGlxZoomPanEventHandler::NextStepInerticPan");
    
    TPoint inertiaOffset = iMathsEngine.LastPanOffset();
    
    if ( (10 >= Abs(inertiaOffset.iX )) && (10 >= Abs(inertiaOffset.iY) ))
        {
        CancelAnimationTimer();
        }
    else
        {
        TPoint topLeftCorner(0, 0);
        TBool thresholdReached = EFalse;
        iMathsEngine.Pan(inertiaOffset, topLeftCorner,
                EGlxPanIncrementInertic, &thresholdReached);

        iZoomEventHandler.HandleViewPortParametersChanged(topLeftCorner,
                KGlxAnimationTimeDrag);

        // we dont want to continue animated PAN if we have reached one end of the image.
        if (thresholdReached)
            {
            CancelAnimationTimer();
            }
        }
    }

void CGlxZoomPanEventHandler::SetPreviousEventCode(const TGestureCode aCode )
    {
    TRACER("CGlxZoomPanEventHandler::SetPreviousEventCode(const TGestureCode aCode )");
    
    iPreviousGestureCode = aCode;
    }

// -----------------------------------------------------------------------------
// HandlePinchEvent
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HandlePinchEventL(
        const GestureHelper::MGestureEvent& aEvent)
    {
    TRACER("CGlxZoomPanEventHandler::HandlePinchEvent(GestureHelper::MGestureEvent&)");

    // Ignore events when we are animating in Zoom
    if (iIsZoomingInAnimatedState)
        {
        return;
        }
    
    TPoint pinchFocus       = aEvent.PinchCentrePoint();
    // Wrong convention in variable nomenclature but better than 
    // ratioInPercentOfChangeInPinchDistance which is incidentally correct 
    TInt    pinchPercentage     = aEvent.PinchPercent();  
    GLX_LOG_INFO1("CGlxZoomPanEventHandler::HandlePinchEventL: Percentage [%d]" , pinchPercentage);
    GLX_LOG_INFO2("CGlxZoomPanEventHandler::HandlePinchEventL: Pinch Focus [%d, %d]" , pinchFocus.iX, pinchFocus.iY);

	if (( iPrevPinchPercentage >= KGlxNeutralZoomFactor ) && 
        ( pinchPercentage < KGlxNeutralZoomFactor ))
        {
		iPrevPinchPercentage = pinchPercentage ;
		GLX_LOG_INFO1("CGlxZoomPanEventHandler::HandlePinchEventL: Pinch Ignored. Previous pinch factor = %d", iPrevPinchPercentage );
		//This will result in we ignoring this event
        pinchPercentage = KGlxNeutralZoomFactor;
        }
	else
        {
        iPrevPinchPercentage = pinchPercentage ;
        }
	
    // pinchPercentage == 100 => No change in finger distance => No Zoom. 
    // A negative Pinch percentage signifies an error in calculations. 
	// So NOT handling these
    if ( (pinchPercentage != KGlxNeutralZoomFactor)
            && (pinchPercentage > 0) )
        {
        Zoom(0, pinchPercentage, EZoomIn, &pinchFocus);
        }
    
    HideScreenFurniture();
    }

// -----------------------------------------------------------------------------
// HandleDoubleTap
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HandleDoubleTap(const GestureHelper::MGestureEvent& /*aEvent*/ )
    {
    TRACER("CGlxZoomPanEventHandler::HandleDoubleTap(GestureHelper::MGestureEvent&)");

    // Ignore events when we are animating in Zoom
    if (iIsZoomingInAnimatedState)
        {
        return;
        }
    
    SetupAnimatedZoom(EZoomOut);
    }

// -----------------------------------------------------------------------------
// HandleSingleTap
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HandleSingleTap(const GestureHelper::MGestureEvent& /*aEvent*/ )
    {
    TRACER("CGlxZoomControl::HandleSingleTap(GestureHelper::MGestureEvent&)");

    // Ignore events when we are animating in Zoom
    if (iIsZoomingInAnimatedState)
        {
        return;
        }

    if (EUiOff == iZoomUiState)
        {
        ShowScreenFurniture(KGlxScreenTimeout);
        }
    else{
        HideScreenFurniture();
        }
    }

// ---------------------------------------------------------------------------
// UiTimeOut: Hides the screen furniture once the Timeout happens
// ---------------------------------------------------------------------------
//  
TInt CGlxZoomPanEventHandler::UiTimeOut(TAny* aSelf)
    {
    TRACER("CGlxZoomPanEventHandler::UiTimeOut");
    if(aSelf)
        {
        CGlxZoomPanEventHandler* self = static_cast <CGlxZoomPanEventHandler*> (aSelf);
        //retreive the UI state.
        if(EUiOn == self->iZoomUiState)
            {
            self->HideScreenFurniture();
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ZoomOutTimerL
// ---------------------------------------------------------------------------
//  
TInt CGlxZoomPanEventHandler::ZoomOutTimerL(TAny* aSelf)
    {
    TRACER("CGlxZoomPanEventHandler::ZoomOutTimerL");
    if(aSelf)
        {
        CGlxZoomPanEventHandler* self = static_cast <CGlxZoomPanEventHandler*> (aSelf);
        self->CallZoomOutL();
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CallZoomOutL
// ---------------------------------------------------------------------------
//  
void CGlxZoomPanEventHandler::CallZoomOutL()
    {
    TRACER("CGlxZoomPanEventHandler::CallZoomOutL()");
    iZoomEventHandler.HandleZoomOutL(KGlxZoomOutCommand);
    }

//----------------------------------------------------------------------------------
// PanIntervalExpired:Callback function for the pan timer
//----------------------------------------------------------------------------------
//
TInt CGlxZoomPanEventHandler::PanIntervalExpired(TAny* aPtr)
    {
    TRACER("CGlxZoomPanEventHandler::PanIntervalExpired ");
    GLX_LOG_INFO("PanIntervalExpired ");
    CGlxZoomPanEventHandler* self = static_cast<CGlxZoomPanEventHandler*>(aPtr);
    self->DoPan();
    return 0; // Timer has finished
    }


//----------------------------------------------------------------------------------
// DoPan:calls Panning function.
//----------------------------------------------------------------------------------
//    
void CGlxZoomPanEventHandler::DoPan()
    {
    TRACER("CGlxZoomPanEventHandler::DoPan ");

    TBool atPanThreshold = EFalse;
    TPoint topLeftCorner;
    iMathsEngine.Pan(iPanDirection, topLeftCorner,
            EGlxPanIncrementExponential, &atPanThreshold);

    iZoomEventHandler.HandleViewPortParametersChanged(topLeftCorner,
            KGlxAnimationTimekeyPan);
    
    if ( atPanThreshold )
        {
        CancelZoomPanTimer();
        }
    
    iMathsEngine.UpdatePanFactor();
    }


//----------------------------------------------------------------------------------
// OrientationChanged 
//----------------------------------------------------------------------------------
//    
void CGlxZoomPanEventHandler::OrientationChanged(const TRect& aNewScreenRect)
    {
    TRACER("CGlxZoomPanEventHandler::OrientationChanged ");
    
    iMathsEngine.OrientationChanged(aNewScreenRect);
    iMinZoomRatio = iMathsEngine.MinimumZoomRatio();
    //Keep the relative Zoom Ratio same while changing orientation
    Zoom(0, KGlxNeutralZoomFactor, iZoomMode) ;
    }

// -----------------------------------------------------------------------------
// ShowSlider
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::ShowScreenFurniture(TTimeIntervalMicroSeconds32 aTimeout)
    {
    TRACER("CGlxZoomPanEventHandler::ShowScreenFurniture()");
    
    if (EUiOff == iZoomUiState )
        {
        // make visible if not already visible. 
        // the timer will ofcourse get restarted. 
        iZoomEventHandler.HandleShowUi(ETrue);
        }
    
    if (aTimeout.Int())
        {
        StartUITimer(aTimeout,aTimeout,TCallBack( UiTimeOut,this ));
        }
    }


// -----------------------------------------------------------------------------
// HideScreenFurniture
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HideScreenFurniture()
    {
    TRACER("CGlxZoomPanEventHandler::HideScreenFurniture()");
    if (EUiOn == iZoomUiState )
        {
        iZoomEventHandler.HandleShowUi(EFalse);
        CancelUITimer();
        }
    }


// -----------------------------------------------------------------------------
// ActivateZoom
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::ActivateZoom( TInt aInitialZoomRatio,  
        TSize aImageSize, 
        TZoomStartMode aStartMode, 
        TInt aMinZoomRatio, 
        TInt aMaxZoomRatio,
        TPoint* aZoomFocus)
    {
    TRACER("CGlxZoomPanEventHandler::ActivateZoom");
    
    TPoint center(aImageSize.iWidth/2 ,aImageSize.iHeight/2 );
    TSize  imageSize(aImageSize.iWidth, aImageSize.iHeight);

    TSize  screenSize = TSize(AlfUtil::ScreenSize());
    
    GLX_LOG_INFO2("ActivateZoom : Center = [%d,%d],   ", 
            TInt(center.iX), 
            TInt(center.iY)  
              );
    
    // Minimum and Maximum Zoom Ratio     
    iZoomRatio      = aInitialZoomRatio; 
    iMinZoomRatio   = aMinZoomRatio    ;
    iMaxZoomRatio   = aMaxZoomRatio    ;
    
    iMathsEngine.Initialize(center,
            screenSize,
            imageSize,
            iZoomRatio,
            aMinZoomRatio
            );
    
    //initially show the slider,so set the state to slider visible
    TPoint   viewPortTopLeft(0,0);
    TSize    viewPortDimension(0,0);
    iZoomMode = EZoomIn;

    switch(aStartMode)
        {
        case EZoomStartKey :
            {
            // Fix for issue EPKA-7ZX8FR: Vasco_wk03: Zoom In Key (Volume Up Key),
            // gradually Zooms the image to full extent on a single click.
            
            // We are not getting keyup event which cancle the timer so not
            // starting timer to do zoom
            Zoom(0, 0, iZoomMode);          
            }
            break;
        case EZoomStartDoubleTap :
            {
            if (!iIsZoomingInAnimatedState)
                {
                if (aZoomFocus)
                    {
                    iZoomFocus.iX = aZoomFocus->iX ;
                    iZoomFocus.iY = aZoomFocus->iY ;
                    }
                SetupAnimatedZoom(EZoomIn, &iZoomFocus);
                }
            }
            break;
        case EZoomStartPinch:
            {
            iZoomRatio = iMathsEngine.Zoom(EZoomIn, aMinZoomRatio,  
                    viewPortTopLeft, viewPortDimension);

            iZoomEventHandler.HandleViewPortParametersChanged(viewPortTopLeft , 0, 
                    &viewPortDimension, iZoomRatio);
            iZoomEventHandler.HandleShowUi(EFalse);
            }
            break;
        case EZoomStartSlider:
            {
            Zoom(aInitialZoomRatio, 0, iZoomMode);
            }
            break;
        default:
            break;
        }
    
    }

// -----------------------------------------------------------------------------
// DeactivateZoom
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::DeactivateZoom()
    {
    TRACER("CGlxZoomPanEventHandler::DeactivateZoom()");
    
    iZoomMode = EZoomOut;
    iIsZoomingInAnimatedState = EFalse ;
    CancelZoomPanTimer();
    CancelUITimer();
    CancelAnimationTimer();
    }

// -----------------------------------------------------------------------------
// HandleEvent
// -----------------------------------------------------------------------------
//
// Todo: Combine both these HandleEvents
TBool CGlxZoomPanEventHandler::HandleEvent( const TAlfEvent& aEvent )
    {
    TRACER("CGlxZoomPanEventHandler::HandleEvent()");
    
    TBool eventHandledState = EFalse;
    
    if (iZoomActivated)
        {
        if(!aEvent.IsCustomEvent())
            {
            eventHandledState = EFalse;
            }
        else
            {
            GLX_LOG_INFO("CGlxZoomPanEventHandler::HandleEvent: Custom Event.");
            TInt EventID = aEvent.CustomParameter();

            switch(EventID)
                {
                case ETypePrimaryValueChange:
                    {
                    GLX_LOG_INFO("CGlxZoomPanEventHandler::HandleEvent:ETypePrimaryValueChange");

                    GLX_LOG_INFO1("CGlxZoomPanEventHandler::HandleEvent:ETypePrimaryValueChange, iMinZoomRatio = [%d]", iMinZoomRatio);
                    GLX_LOG_INFO1("CGlxZoomPanEventHandler::HandleEvent:ETypePrimaryValueChange, iMaxZoomRatio = [%d]", iMaxZoomRatio);

                    MulSliderPos* dataPtr = (MulSliderPos*)(aEvent.CustomEventData());  
                    TInt currentSliderValue = dataPtr->mCurrentValue;
                    GLX_LOG_INFO1("CGlxZoomPanEventHandler::HandleEvent:ETypePrimaryValueChange, currentSliderValue = [%d]", currentSliderValue );

                    // Is current value within acceptable range? 
					// If yes, then zoom in or zoom out as needed.
                    if ( currentSliderValue > iMinZoomRatio 
                            &&  currentSliderValue <= iMaxZoomRatio )
                        {
                        Zoom(currentSliderValue, 0, EZoomIn);
                        }
                    else if (currentSliderValue <= iMinZoomRatio)
                        {
                        CallZoomOutL();
                        }
                    eventHandledState = ETrue;
                    }
                    break;

                case ECustomEventIconClick :
                     {
                     //The Slider is held by user,so cancel the UI Timer
                     //When the slider is held ,the screen furniture shouldn't disappear
                     GLX_LOG_INFO("CGlxZoomPanEventHandler::HandleEvent, ECustomEventIconClick");
                     CancelUITimer();    
                     eventHandledState = ETrue;           
                     }
                     break;

                case ECustomEventIconRelease:
                     {
                     //The slider is not held, by the user,start the ui timer to hide the screen furniture
                     GLX_LOG_INFO( " CGlxZoomPanEventHandler::offerEvent,ECustomEventIconRelease");
                     StartUITimer(KGlxScreenTimeout, KGlxScreenTimeout,
                            TCallBack(UiTimeOut, this));
                     eventHandledState = ETrue;
                     }
                     break;

                default:
                    {
                    GLX_LOG_INFO(" CGlxZoomPanEventHandler::HandleEvent default");
                    eventHandledState = EFalse;
                    break;
                    }
                }
            }
        }
    return eventHandledState ;    
    }

// -----------------------------------------------------------------------------
// HandleEvent
// -----------------------------------------------------------------------------
//
TBool CGlxZoomPanEventHandler::HandleEventL(const TAlfEvent &aEvent)
    {
    TRACER("CGlxZoomPanEventHandler::HandleEventL()");
    
    TBool consumed = EFalse;

    if (iZoomActivated)
        {
        if (aEvent.IsKeyEvent() )
            {
            GLX_LOG_INFO(" CGlxZoomPanEventHandler::HandleEvent KeyEvent");
            consumed = HandlekeyEvents(aEvent);
            }
        else if(aEvent.IsPointerEvent() )
            {
            GLX_LOG_INFO(" CGlxZoomPanEventHandler::HandleEvent PointerEvent ");
            consumed = iZoomEventHandler.HandlePointerEventsL(aEvent);
            }
        }
    return consumed;
    }


// -----------------------------------------------------------------------------
// ZoomToMinimum
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::ZoomToMinimumL()
    {
    TRACER("CGlxZoomPanEventHandler::ZoomToMinimumL( )");
    
    Zoom(iMinZoomRatio, 0, EZoomOut);
    CallZoomOutL();
    }


// -----------------------------------------------------------------------------
// Zoom
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::Zoom(TInt aExpectedZoomLevel,
        TInt aRelativeZoomFactor, TZoomMode aZoomMode, TPoint* aZoomFocus)
    {
    TRACER("CGlxZoomPanEventHandler::Zoom()");
    
    TPoint          viewPortTopLeft(0,0);
    TSize           viewPortDimension(0,0);
    TBool           atZoomThreshold = EFalse;
    
    iZoomRatio = iMathsEngine.Zoom(aZoomMode, 
            aExpectedZoomLevel, 
            viewPortTopLeft, 
            viewPortDimension, 
            &atZoomThreshold,
            aZoomFocus, 
            aRelativeZoomFactor);

    if( atZoomThreshold )
        {
        GLX_LOG_INFO("CGlxZoomPanEventHandler::Zoom Threshold Reached");
        CancelZoomPanTimer();
        if (iZoomRatio <= iMinZoomRatio)
            {
            CallZoomOutL();
            return;
            }
        }
    iZoomEventHandler.HandleViewPortParametersChanged(viewPortTopLeft, 0, 
            &viewPortDimension, iZoomRatio);
    }

// -----------------------------------------------------------------------------
// ZoomIsActivated
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::SetZoomActivated(TBool aIsActivated)
    {
    TRACER("CGlxZoomPanEventHandler::ZoomIsActivated");
    iZoomActivated = aIsActivated;
    }

// -----------------------------------------------------------------------------
// SetZoomUiState
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::SetZoomUiState(TUiState aZoomUiState)
    {
    TRACER("CGlxZoomPanEventHandler::ZoomIsActivated");
    iZoomUiState = aZoomUiState;
    GLX_LOG_INFO1("CGlxZoomPanEventHandler::SetZoomUiState :=%d",iZoomUiState);
    }

// -----------------------------------------------------------------------------
// ZoomUiState
// -----------------------------------------------------------------------------
//
TUiState CGlxZoomPanEventHandler::ZoomUiState()
    {
    TRACER("CGlxZoomPanEventHandler::ZoomUiState");
    GLX_LOG_INFO1("CGlxZoomPanEventHandler::ZoomUiState :=%d",iZoomUiState);
    return iZoomUiState ;
    }

// -----------------------------------------------------------------------------
// HandleMultiTouchReleased
// -----------------------------------------------------------------------------
//
void CGlxZoomPanEventHandler::HandleMultiTouchReleased()
    {
    TRACER("CGlxZoomPanEventHandler::HandleMultiTouchReleased");
    //Do nothing
    }

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
* Description:     Declaration of Maths Engine
*
*/

#ifndef GLXZOOMMATHSENGINE_H_
#define GLXZOOMMATHSENGINE_H_


// INCLUDE FILES
#include <e32base.h>

#include <glxzoomview.hrh>

NONSHARABLE_CLASS( TGlxZoomAndPanMathsEngine )
    {
public:
    
    /**
     * Starts panning with keys
     *@ param aOffset:Specifies the offset value with which image has to be shifted/panned
     */              
    void Pan(TPoint aOffset, 
            TPoint& aViewPortTopLeft,
            TGlxPanIncrementType aPanIncrement,
            TBool* aThresholdReached = NULL);

    /**
     * Starts zooming In/out with slider/Keys
     * @ param aMfactor:The Zooming factor with which image has to be zoomed
     */              
    TInt Zoom( TZoomMode aZoomMode,         // Ignored when aExpectedZoomRatio has a value 
                TInt aExpectedZoomRatio,    // only for slider based zoom. = 0 for key based zoom. 
                TPoint& aViewPortTopLeft, 
                TSize& aViewPortDimension,
                TBool* apThresholdReached = NULL,
                TPoint* apZoomFocus = NULL, 
                TInt aRelativeZoomFactor = 0);
    
    // Initialize 
    void Initialize(TPoint& aCenter, 
            TSize& aScreenSize, 
            TSize& aImageSize,
            TSize& aOriginalZoomedDimensions, 
            TUint8 aInitialZoomRatio);
    
    void UpdatePanFactor(TTime& aPanTime);
    
    // this can have a more generic name. but now it informs only abt screen orientation changes. and hence the name 
    void OrientationChanged(const TRect& aNewScreenRect);
    
    TSize ImageVirtualSize();
    
    TSize ScreenSize();
    
    TInt NewCenterCoordinate(TInt Center, 
            TInt Offset, 
            TUint16 HalfScreenDimension, 
            TInt MinimumCoordinate, 
            TInt MaximumCoordinate, 
            TBool *aLimitReached = NULL) ;
    
    void SetupPanOperation();
    
    TPoint LastPanOffset();

private:

    /**
     * Get the next Zoomlevel on the basis of Zoom In/Out
     *  @ param aZoomMode:Specifies the zoomode,whether it is Zoom In/Out
     *  
     *  We ignore aZoommode and aRelativeZoomFactor if we have a aExpectedZoomRatio.
     *  We ignore aZoommode if we have a aRelativeZoomFactor.
     *  
     */          
    TInt NewZoomRatio( 
            TInt aExpectedZoomRatio,
            TInt aRelativeZoomFactor,
            TZoomMode aZoomMode,
            TBool *aThresholdReached = NULL) ;
    
    TPoint          iCenter             ;
    TSize           iScreenSize         ;
    TSize           iImageVirtualSize   ;
    TSize           iActualImageSize    ;
    
    // The Purpose of this Variable is to set the No:of pixels, by which it should Pan,
    // when the Key is held for long time.
    TUint8          iPanFactor          ;       
    // No of continuous pan operations that have happened. This is used in calculating the pan factor  
    TInt            iContinuousPanOperations;
                                                 
    // These are the thresholds
    TUint8          iMaxZoomRatio;
    TUint8          iMinZoomRatio   ;
    TSize           iInitialImageSize   ;

    // This is what our current zoom ratio is in percentage
    TReal						iZoomRatio;
    
    TPoint          iPanSpeed;          //  Pixels Panned per pan operation in vector form.  
    TPoint          iLastPanOffset;     //  Pixels Panned during the last pan operation.
    
    // [TODO]:Tsize might not be an exact match for this. using this only since it has exactly all the parameters that are required.   
    TSize           iBorderWidth;

    };



#endif //GLXZOOMMATHSENGINE

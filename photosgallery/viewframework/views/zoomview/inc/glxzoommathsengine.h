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
     * Recalculates the new TL and Dimension values for the viewport to be drawn after a new Zoom Operation
     * @ param aZoomMode :          Are we Zooming In our out. Ignored when aExpectedZoomRatio 
     *                                      has a non zero value
     * @ param aExpectedZoomRatio : When we need zooming to a particular finite value. 
     *                                      Generally for Slider based zoom.
     * @ param aViewPortTopLeft:    The new TL location of the viewport after zooming
     * @ param aViewPortDimension:  The new dimensions of the viewport after zooming
     * @ param apThresholdReached : Optional parameter. If present, indicates whether we 
     *                                      have reached a limit while zooming. 
     * @ param apZoomFocus:         Optional parameter. If present, indicates the point 
     *                                      around wich zooming happens, (offcenter zoom)
     * @ param aRelativeZoomFactor: Percentage of zoom in relation to the previous value. 
     *                                      useful in pinch zoom. If this is greater than 0, 
     *                                      the first 2 parameters are ignored 
     */              
    TInt Zoom( TZoomMode aZoomMode,         // Ignored when aExpectedZoomRatio has a value 
                TInt aExpectedZoomRatio,    // only for slider based zoom. = 0 for key based zoom. 
                TPoint& aViewPortTopLeft, 
                TSize& aViewPortDimension,
                TBool* apThresholdReached = NULL,
                TPoint* apZoomFocus = NULL, 
                TInt aRelativeZoomFactor = 0);
    
    /**
     * Initialize the Maths engine
     * @ param aCenter:         The center of the VP(View Port)
     * @ param aScreenSize:     Screensize of the device. 
     * @ param aImageSize:      Size of the image.
     * @ param aOriginalZoomedDimensions:The Original zoomed dimension.
     * @ param aInitialZoomRatio:Initial zoom ratio.
     */              
    void Initialize(TPoint& aCenter, 
            TSize& aScreenSize, 
            TSize& aImageSize,
            TSize& aOriginalZoomedDimensions, 
            TUint8 aInitialZoomRatio);
    
    /**
     * UpdatePanFactor [Useful only in Key-based Panning]
     */              
    void UpdatePanFactor();
    
    /**
     * Indicates change of orientation. 
     * @ param aNewScreenRect: New Screen size
     */              
    // this can have a more generic name. but now it informs only abt screen orientation changes. and hence the name 
    void OrientationChanged(const TRect& aNewScreenRect);
    
    /**
     * Retrieves the Image Virtual Size
     */              
    TSize ImageVirtualSize();
    
    /**
     * Retrieves the Image Virtual Size
     */              
    TSize ScreenSize();
    
    /**
     * Retrieves the New Center Coordinate on one axis. 
     * Needs to be called twice for both axes.
     * @ param aCenter: The Center of the VP.
     * @ param aOffset: The offset along one axis if we have paned 
     * @ param aHalfScreenDimension: Half the screen dimension along this axis.
     * @ param aMinimumCoordinate: Minimum possible coordinate along this axis. Usually 0.
     * @ param aMaximumCoordinate: Maximum possible coordinate along this axis. 
     * @ param aLimitReached:   Indicates if we have reached a limit while zooming/Panning.
     */              
    TInt NewCenterCoordinate(TInt aCenter, 
            TInt aOffset, 
            TUint16 aHalfScreenDimension, 
            TInt aMinimumCoordinate, 
            TInt aMaximumCoordinate, 
            TBool *aLimitReached = NULL) ;
    
    /**
     * Begin the pan operation
     */              
    void SetupPanOperation();
    
    /**
     * Retrieves the latest ofset by which we panned
     */              
    TPoint LastPanOffset();

    /**
     * Retrieves the minimum zoom ratio with which we stay in zoomode
     */              
    TInt MinimumZoomRatio();
    
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
    TUint8          iMinZoomRatio;
    TSize           iInitialImageSize;

    // This is what our current zoom ratio is in percentage
    TReal           iZoomRatio;
    
    TPoint          iLastPanOffset;     //  Pixels Panned during the last pan operation.
    
    // [TODO]:Tsize might not be an exact match for this. using this only since it has exactly all the parameters that are required.   
    TSize           iBorderWidth;

    };



#endif //GLXZOOMMATHSENGINE

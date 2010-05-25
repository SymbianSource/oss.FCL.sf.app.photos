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
* Description:    Implementation of Zoom Maths engine
*
*/


//includes 
#include "glxzoomeventhandler.h"
#include "glxzoommathsengine.h"
#include "glxzoomcontrol.h"

// Includes for Logs
#include <glxtracer.h>                      //  For Tracer
#include <glxlog.h>                         //  For Log

#define BORDER_HEIGHT (iBorderWidth.iHeight*(100-((newZoomRatio-iMinZoomRatio)*100/(iMaxZoomRatio-iMinZoomRatio))))/100
#define BORDER_WIDTH (iBorderWidth.iWidth*(100-((newZoomRatio-iMinZoomRatio)*100/(iMaxZoomRatio-iMinZoomRatio))))/100
// LOCAL CONSTANTS AND MACROS
const TInt KGlxLargeImageMaximumZoomRatio   =   100     ;
const TInt KGlxSmallImageMaximumZoomRatio   =   150     ;

const TInt KGlxMinPanFactor                 =   1       ; // The min multiple of pixles to pan 
const TInt KGlxPanInertiaFactor             =   10      ; 
const TInt KGlxPanFactorUpdateMultiple      =   5       ; // The number of pan operations after which panning speed is increased by an order of magnitude 
const TInt KGlxMaxPanUpdateMultiple         =   6       ;
const TInt KGlxOrigin                       =   0       ;

const TInt KGlxMinRelativeZoomPercent       =   85       ;
const TInt KGlxMaxRelativeZoomPercent       =   115      ;

const TInt KGlxZoomPanInc                   =   10      ; // Min number of pixels panned in one keypress. This value is incremented exponentially by multiples of iPanFactor 

// ============================ MEMBER FUNCTIONS ===============================

void TGlxZoomAndPanMathsEngine::Initialize(TPoint& aCenter, 
        TSize& aScreenSize, 
        TSize& aImageSize,
        TSize& /*aOriginalZoomedDimensions*/,
        TUint8 aInitialZoomRatio)
    {
    TRACER("void TGlxZoomAndPanMathsEngine::Initialize()");

    iCenter             = aCenter;
    iScreenSize         = aScreenSize;

    // This will only be set at initialization/constructiron. Never afterwards.
    // [TODO] Is there a way to make this constant.
    iActualImageSize    = aImageSize;
    
    iMinZoomRatio   = aInitialZoomRatio;
    iZoomRatio          = aInitialZoomRatio;
    iMaxZoomRatio = ((aInitialZoomRatio == KGlxLargeImageMaximumZoomRatio) ? 
        KGlxSmallImageMaximumZoomRatio:KGlxLargeImageMaximumZoomRatio);

    iPanFactor          = KGlxMinPanFactor;

    iImageVirtualSize.iHeight   = aImageSize.iHeight * aInitialZoomRatio /100;
    iImageVirtualSize.iWidth    = aImageSize.iWidth  * aInitialZoomRatio /100;
    
    iCenter.iX = iImageVirtualSize.iWidth/2 ;
    iCenter.iY = iImageVirtualSize.iHeight/2;
    
    iBorderWidth.iWidth     = (iScreenSize.iWidth  - iImageVirtualSize.iWidth )/2  ; 
    iBorderWidth.iHeight    = (iScreenSize.iHeight - iImageVirtualSize.iHeight)/2 ;
    
    }

TInt TGlxZoomAndPanMathsEngine::NewCenterCoordinate(TInt aCenter, 
                                TInt aOffset, 
                                TUint16 aHalfScreenDimension, 
                                TInt aMinimumCoordinate, 
                                TInt aMaximumCoordinate, 
                                TBool *aThresholdReached)
    {
    TRACER("TGlxZoomAndPanMathsEngine::NewCenterCoordinate");
    if(aOffset)
        {
        //Add the No:of Pixels that has to be panned,the No:of pixels added will be more
        //if the pan key is held for long time,the PanFactor determines that
        aCenter = aCenter + (aOffset*iPanFactor);
        
        // For Checking the boundary condition
        if( (aCenter - aHalfScreenDimension)  < aMinimumCoordinate )
            {
            //This shows the black background on boundaries of the image and reset to the start of the image
            aCenter  = aHalfScreenDimension;
            // Set the variable as true, if the there EXISTS a Variable at that address 
            if (NULL != aThresholdReached)
                {
                *aThresholdReached = ETrue;
                }
            }
        if(aCenter + aHalfScreenDimension > aMaximumCoordinate )
            {
            //This shows the black background on boundaries of the image and reset to the end of the image
            aCenter  = aMaximumCoordinate - aHalfScreenDimension;
            if (NULL != aThresholdReached)
                {
                *aThresholdReached = ETrue;
                }
            }
        }
    return aCenter;
    }

    
void TGlxZoomAndPanMathsEngine::Pan(TPoint aOffset, 
        TPoint& aViewPortTopLeft, 
        TGlxPanIncrementType aPanIncrement,
        TBool * aThresholdReached)
    {
    TRACER("TGlxZoomAndPanMathsEngine::Pan");
    
    GLX_LOG_INFO2("Pan: Center before PAN= [%d,%d]   ", iCenter.iX, iCenter.iY  );
    GLX_LOG_INFO2("Pan: Pan Offset = [%d,%d]   ", TInt(aOffset.iX), TInt(aOffset.iY));
    
    TPoint panOffset = aOffset; 
    TUint16 halfScreenWidth     = iScreenSize.iWidth>>1;
    TUint16 halfScreenHeight    = iScreenSize.iHeight>>1;
    
    // if we are dragging or something like that the caller might want a uniform increase in panning factor.
    if (EGlxPanIncrementUniform == aPanIncrement)
        {
        iPanFactor = KGlxMinPanFactor ; 
        }
    else if ( EGlxPanIncrementInertic == aPanIncrement)
        {
        if (panOffset.iX > 0 )
            {
            panOffset.iX = panOffset.iX - KGlxPanInertiaFactor ;
            }
        else
            {
            panOffset.iX = panOffset.iX + KGlxPanInertiaFactor ;
            }

        if (panOffset.iY > 0 )
            {
            panOffset.iY = panOffset.iY - KGlxPanInertiaFactor ;
            }
        else
            {
            panOffset.iY = panOffset.iY + KGlxPanInertiaFactor ;
            }
        }

    GLX_LOG_INFO1("Pan: Pan Factor = %d   ", iPanFactor  );
    
    // dont pan on a dimension if image is smaller on that dimension than the screen. 
    if (iImageVirtualSize.iWidth > iScreenSize.iWidth)
        {
        iCenter.iX = NewCenterCoordinate(   iCenter.iX, panOffset.iX, halfScreenWidth,KGlxOrigin, 
                iImageVirtualSize.iWidth,   aThresholdReached)   ;
        }
    
    if (iImageVirtualSize.iHeight > iScreenSize.iHeight)
        {
        TBool thresholdReached = EFalse;
        iCenter.iY = NewCenterCoordinate(   iCenter.iY, panOffset.iY, halfScreenHeight,KGlxOrigin, 
                iImageVirtualSize.iHeight,  &thresholdReached)   ;

        if (NULL != aThresholdReached)
            {
            *aThresholdReached = *aThresholdReached || thresholdReached ; // if we reach the threshold along either axis this means we have reached the threshold
            }
        }
    
    aViewPortTopLeft.iX = iCenter.iX - halfScreenWidth;
    aViewPortTopLeft.iY = iCenter.iY - halfScreenHeight;
    
    iLastPanOffset = panOffset; 
    
    GLX_LOG_INFO2("Pan: Center after PAN= [%d,%d]   ", 
            TInt(iCenter.iX), TInt(iCenter.iY)  );
    GLX_LOG_INFO2("Pan: aViewPortTopLeft after PAN= [%d,%d]   ", 
            TInt(aViewPortTopLeft.iX), TInt(aViewPortTopLeft.iY)  );
    }


TInt TGlxZoomAndPanMathsEngine::Zoom(TZoomMode aZoomMode,
            TInt aExpectedZoomRatio,    // only for slider based zoom. = 0 for key based zoom
            TPoint& aViewPortTopLeft, 
            TSize& aViewPortDimension,
            TBool* aThresholdReached,
            TPoint* apZoomFocus, 
            TInt aRelativeZoomFactor)
    {
    TRACER("TGlxZoomAndPanMathsEngine::Zoom ");
    
    //[TODO] Unlike the above method this method is a very very very very very small method. 
    //[TODO] Consider making this larger. [:|]
    TUint16 halfScreenWidth     = iScreenSize.iWidth>>1;
    TUint16 halfScreenHeight    = iScreenSize.iHeight>>1;
    TPoint  centerTranslationfactor(0,0);
    
    //[TODO] try to make sure that this variable does not get used in calculations. it always should be the center and
    //existing and new screen and virtual image dimensions should be calculated using this one.
    TPoint viewPortTopLeft(TPoint(iCenter).iX - halfScreenWidth, 
            TPoint(iCenter).iY - halfScreenHeight) ;
    GLX_LOG_INFO2(" Zoom(): Center before Zoom= [%d,%d] ", iCenter.iX, iCenter.iY );
    GLX_LOG_INFO2(" Zoom(): viewPortTopLeft  Before Zoom  = [%d,%d] ", TInt(viewPortTopLeft.iX),
            TInt(viewPortTopLeft.iY)  );
    
    TInt oldZoomRatio = iZoomRatio ;  
    TInt newZoomRatio = NewZoomRatio(aExpectedZoomRatio, aRelativeZoomFactor, aZoomMode, aThresholdReached);

    // we have an offcenter focus for our zoom in certain cases ( e.g pinch operation) or a double tap at an offcenter point. 
    // In such a case the zoom happens around that point. So we need to translate (move) the 
    // center in such a way that the relative position of the zoom facus does not change. 
    // centerTranslationfactor is the vector representation of that amount. 
    if (NULL != apZoomFocus)
        {
        centerTranslationfactor.iX = (((apZoomFocus->iX - halfScreenWidth) * newZoomRatio)/oldZoomRatio) + (halfScreenWidth - apZoomFocus->iX);         
        centerTranslationfactor.iY = (((apZoomFocus->iY - halfScreenHeight) * newZoomRatio)/oldZoomRatio) + (halfScreenHeight - apZoomFocus->iY);
        }

    GLX_LOG_INFO1(" Zoom: newZoomRatio = %d.   ", newZoomRatio );
    
    TSize imageDimension = TSize(iActualImageSize);
    imageDimension.iWidth =  (imageDimension.iWidth  * newZoomRatio)/100;
    imageDimension.iHeight = (imageDimension.iHeight * newZoomRatio)/100;
    
    // Check if image size is bigger than screen size AND image is panned to such an extent that 
    // the an edge of the image comes within the screen rectange. If so go ahead,   
    // for everything else there is mastercard. I mean the else condition below. 
    // [TODO] The master card however needs more simplification. 
    if(
            ((iImageVirtualSize.iHeight > iScreenSize.iHeight) 
                    && (iImageVirtualSize.iWidth > iScreenSize.iWidth)) 
            &&  ((iCenter.iX != iImageVirtualSize.iWidth/2) 
                    || (iCenter.iY != iImageVirtualSize.iHeight/2))
            )
        {
        //Calculate the new Center posistion accordingly to the new zoom factor
        // [TODO] Suggest the use of overloaded operators so that points can be operated upon by vectors. 
        // specifically in this case to enable a point to be multiplied by a dimension resulting from the ratio of two dimension  
        iCenter.iX = (iCenter.iX * imageDimension.iWidth )/iImageVirtualSize.iWidth  ;
        iCenter.iY = (iCenter.iY * imageDimension.iHeight)/iImageVirtualSize.iHeight ;

        
        //HEIGHT Calculation
        // TODO: Hive center corrections into an different function.
        // If the image might have become smaller than the screen DUE to or AFTER the above calculations
        if((imageDimension.iHeight < iScreenSize.iHeight - BORDER_HEIGHT))
            {
            iCenter.iY=(imageDimension.iHeight/2);
            }
        else
            {
            TInt weightedBorderHeight = (iBorderWidth.iHeight*(100-((newZoomRatio-iMinZoomRatio)*100/(iMaxZoomRatio-iMinZoomRatio))))/100 ;
            // Is Center positioned such that the top end of the image is inside the 
            // screen. 
            if( iCenter.iY < (halfScreenHeight - weightedBorderHeight )) 
                {
                iCenter.iY = halfScreenHeight - weightedBorderHeight ;
                }
            // Is Center positioned such that the Bottom end of the image is inside the 
            // screen. 
            else if((iCenter.iY + (halfScreenHeight - weightedBorderHeight ))> imageDimension.iHeight)
                {
                // if so pan the image so that the edge of the image and screen coincide.
                iCenter.iY = imageDimension.iHeight - (halfScreenHeight - weightedBorderHeight)  ;
                }
            }

        //WIDTH Calculation
        if((imageDimension.iWidth < iScreenSize.iWidth - BORDER_WIDTH))
            {
            iCenter.iX=(imageDimension.iWidth/2);
            }
        else
            {
            TInt weightedBorderWidth = (iBorderWidth.iWidth*(100-((newZoomRatio-iMinZoomRatio)*100/(iMaxZoomRatio-iMinZoomRatio))))/100 ;
            if( iCenter.iX < (halfScreenWidth - weightedBorderWidth )) 
                {
                iCenter.iX = (halfScreenWidth - weightedBorderWidth );
                }
            else if((iCenter.iX + (halfScreenWidth - weightedBorderWidth ))> imageDimension.iWidth)
                {
                iCenter.iX = imageDimension.iWidth - (halfScreenWidth - weightedBorderWidth ) ;
                }
            }
        //Update the TopLeft corner and then re align to the center in the below code
        viewPortTopLeft.iY = iCenter.iY - halfScreenHeight ;
        viewPortTopLeft.iX = iCenter.iX - halfScreenWidth ;

        iImageVirtualSize = imageDimension;
        }
    else
        {
        //Conditions to Check.
        //1.Image Height is Smaller than screen Size and Width is Bigger than  screen Size.
        //2.Image Height is Bigger than screen Size and Width is Smaller than  screen Size.
        //3.Both Image Height and Width is  Smaller than screen Size 
        //4.Image Height is Bigger than screen Size and it is panned.
        //4.Image Height is Bigger than screen Size and it is not panned at all.
        //5.Image Width is Bigger than screen Size and it is panned.
        //6.Image Width is Bigger than screen Size and it is not panned at all.
        //7. First Time Zoom operation is performed.
        
        if( (imageDimension.iHeight <= iScreenSize.iHeight) ||
                ( (imageDimension.iHeight > iScreenSize.iHeight) && 
                        (iCenter.iY == (iImageVirtualSize.iHeight/2)) ) ||
                ( ((iCenter.iY + halfScreenHeight) > imageDimension.iHeight)&&
                        ((iCenter.iY - halfScreenHeight) < KGlxOrigin) ))
            {
            //The Image is not panned along Y axis, so the center of the image is the center co-ordinate on this axis.
            iCenter.iY=(imageDimension.iHeight/2);    
            }
        else
            {
            //The image is panned and shift the center posistion
            //Calculate the new Center posistion accordingly to the new zoom factor
            iCenter.iY = (iCenter.iY * imageDimension.iHeight)/iImageVirtualSize.iHeight;          
            if( (iCenter.iY + halfScreenHeight) > imageDimension.iHeight)   
                {
                //Stick the Image to bottom side and then re-posistion the center 
                iCenter.iY = imageDimension.iHeight - halfScreenHeight;
                }
            // New DIM is less than Old one. and all above conditions fail. 
            // This means that new DIM is smaller than VP DIM. So keep center 'centered'.
            // DIM = dimension
            else if(iImageVirtualSize.iHeight >  imageDimension.iHeight)      
                {
                //This is executed in the Zoom Out Case,In ZoomIn Case the Image is widened.
                iCenter.iY=(imageDimension.iHeight/2);
                }
            }

        if((imageDimension.iWidth <= iScreenSize.iWidth) ||
                ( (imageDimension.iWidth > iScreenSize.iWidth) && (iCenter.iX == 
                (iImageVirtualSize.iWidth/2)) ) || ( (iCenter.iX + halfScreenWidth) > 
                imageDimension.iWidth )&&((iCenter.iX - halfScreenWidth) < KGlxOrigin) )
            {
            //The Image is not panned along X axis, ,so the center of the image is the center co-ordinate.
            iCenter.iX=(imageDimension.iWidth/2);
            }
        else
            {
            //The image is panned and shift the center posistion
            //Calculate the new Center posistion accordingly to the new zoom factor
            iCenter.iX = (iCenter.iX * imageDimension.iWidth )/iImageVirtualSize.iWidth ;
            if( (iCenter.iX + halfScreenWidth) > imageDimension.iWidth )
                {
                //Stick the Image to right side and then re-posistion the center 
                iCenter.iX = imageDimension.iWidth - halfScreenWidth ;
                }
            else if(iImageVirtualSize.iWidth >imageDimension.iWidth )
                {
                //The Image is panned and while zooming out ,the center has to be possistioned to center of the screen.
                iCenter.iX =(imageDimension.iWidth/2);
                }
            }

        viewPortTopLeft.iX = iCenter.iX - halfScreenWidth;
        viewPortTopLeft.iY = iCenter.iY - halfScreenHeight;
        iImageVirtualSize = imageDimension;
        
        }

    // if centerTranslationfactor exists that means we have an off center zoom, then 
    // pan that way to get the new center.
    Pan(centerTranslationfactor, viewPortTopLeft, EGlxPanIncrementUniform) ;
        
    aViewPortDimension  = imageDimension    ;
    aViewPortTopLeft    = viewPortTopLeft   ;
    
    GLX_LOG_INFO2("Zoom(): Center After Zoom = [%d,%d]   ", TInt(iCenter.iX), 
            TInt(iCenter.iY)  );
    GLX_LOG_INFO2("Zoom(): aViewPortDimension After Zoom  = [%d,%d]   ", 
            TInt(aViewPortDimension.iWidth), TInt(aViewPortDimension.iHeight)  );
    GLX_LOG_INFO2("Zoom(): viewPortTopLeft  After Zoom  = [%d,%d]   ", 
            TInt(viewPortTopLeft.iX), TInt(viewPortTopLeft.iY)  );

    return newZoomRatio;
    }

//-------------------------------------------------------------------------------------
// UpdatePanFactor: Calculates the Pan Factor based on time the key was pressed
//-------------------------------------------------------------------------------------
//
void TGlxZoomAndPanMathsEngine::UpdatePanFactor()
    {
    TRACER("void TGlxZoomAndPanMathsEngine::UpdatePanFactor()");
    
    iContinuousPanOperations++;
    
    TInt  elapsedPanOperationSetCount = iContinuousPanOperations/KGlxPanFactorUpdateMultiple;
    
    // Double the pan factor for every multiple of KGlxPanFactorUpdateMultiple that the key was pressed till
    // we have crossed 'KGlxMaxTimeMultiple' orders of magnitude  
    if (KGlxMaxPanUpdateMultiple > elapsedPanOperationSetCount )
        {
        iPanFactor = KGlxMinPanFactor << elapsedPanOperationSetCount ;
        }
    GLX_LOG_INFO1("iPanFactor %d", iPanFactor);
    }


//-------------------------------------------------------------------------------------
// NewZoomRatio: Calculates the Zoom Ratio 
//-------------------------------------------------------------------------------------
//
TInt TGlxZoomAndPanMathsEngine::NewZoomRatio( 
        TInt aExpectedZoomRatio,
        TInt aRelativeZoomFactor,
        TZoomMode aZoomMode,
        TBool *aThresholdReached)  
    {
    TRACER("TGlxZoomAndPanMathsEngine::NewZoomRatio ");
    GLX_LOG_INFO1("NewZoomRatio: Old Zoom Ratio = %d .   ",TInt(iZoomRatio)   );
    GLX_LOG_INFO1("NewZoomRatio: Expected Zoom Ratio  = %d .   ",aExpectedZoomRatio    );
    GLX_LOG_INFO1("NewZoomRatio: Relative Zoom Factor = %d .   ",aRelativeZoomFactor   );
    
    // We ignore zoommode and aRelativeZoomFactor if we have a aExpectedZoomRatio.
    // We ignore zoommode if we have a aRelativeZoomFactor.
    if (aExpectedZoomRatio > 0)
        {
        iZoomRatio = aExpectedZoomRatio    ;
        }
    else if (aRelativeZoomFactor > 0)
        {
        //Pruning extreme values. Not allowing more than 15% change in zoom ratio.
        TInt normalizedRelativeZoomFactor = aRelativeZoomFactor ;
        if (normalizedRelativeZoomFactor < KGlxMinRelativeZoomPercent)
            {
            normalizedRelativeZoomFactor = KGlxMinRelativeZoomPercent;
            }
        else if (normalizedRelativeZoomFactor > KGlxMaxRelativeZoomPercent)
            {
            normalizedRelativeZoomFactor = KGlxMaxRelativeZoomPercent;
            }
        
        iZoomRatio =  (iZoomRatio * normalizedRelativeZoomFactor)/100 ;
        }
    else
        {
        if(EZoomIn == aZoomMode)
            {
            iZoomRatio += KGlxZoomPanInc;
            }
        else  
            {
            iZoomRatio -= KGlxZoomPanInc;
            }
        }

    // if iZoomRatio crosses max or minimum limits, then peg them to those limits. 
    if (iZoomRatio >= iMaxZoomRatio) 
        {
        iZoomRatio = iMaxZoomRatio;
        if (NULL != aThresholdReached)
            {
            GLX_LOG_INFO1("NewZoomRatio: Max Threshold Reached iMaxZoomRatio = %d .",iMaxZoomRatio);
            *aThresholdReached = ETrue;
            }
        }
    else if( iZoomRatio <= iMinZoomRatio )
        {
        iZoomRatio = iMinZoomRatio;
        if (NULL != aThresholdReached)
            {
            GLX_LOG_INFO1("NewZoomRatio: Min Threshold Reached iInitialZoomRatio = %d .", iMinZoomRatio );
            *aThresholdReached = ETrue;
            }
        }

    GLX_LOG_INFO1("NewZoomRatio: New Zoom Ratio = %d.   ",TInt(iZoomRatio)   );
    return iZoomRatio;
    }


//-------------------------------------------------------------------------------------
// OrientationChanged:  
//-------------------------------------------------------------------------------------
//
void TGlxZoomAndPanMathsEngine::OrientationChanged(const TRect& aNewScreenRect)
    {
    TRACER("void TGlxZoomAndPanMathsEngine::OrientationChanged()");
    iScreenSize.iWidth          = aNewScreenRect.Width();    
    iScreenSize.iHeight         = aNewScreenRect.Height();
	
    //Choose the minimum of the below.
    TReal32 imageWidthRatio  = ((TReal32)iScreenSize.iWidth  / iActualImageSize.iWidth  )*100.0F;
    TReal32 imageHeightRatio = ((TReal32)iScreenSize.iHeight / iActualImageSize.iHeight )*100.0F;
	iMinZoomRatio = Min(imageWidthRatio, imageHeightRatio);
    GLX_LOG_INFO1("NewZoomRatio: New minimum Zoom Ratio = %d", TInt(iMinZoomRatio));		
    }

//-------------------------------------------------------------------------------------
// ImageVirtualSize: retieves the virtual size of the image.    
//-------------------------------------------------------------------------------------
//
TSize TGlxZoomAndPanMathsEngine::ImageVirtualSize()
    {
    TRACER("TGlxZoomAndPanMathsEngine::ImageVirtualSize()");
    return iImageVirtualSize;
    }


//-------------------------------------------------------------------------------------
// ScreenSize: retrieves the screen size .    
//-------------------------------------------------------------------------------------
//
TSize TGlxZoomAndPanMathsEngine::ScreenSize()
    {
    TRACER("TGlxZoomAndPanMathsEngine::ScreenSize()");
    return iScreenSize;
    }

//-------------------------------------------------------------------------------------
// SetupPanOperation: Starts the Pan operations from the maths Engine's pont of view.    
//-------------------------------------------------------------------------------------
//
void TGlxZoomAndPanMathsEngine::SetupPanOperation()
    {
    TRACER("TGlxZoomAndPanMathsEngine::SetupPanOperation()");
    iContinuousPanOperations = 0 ; 
    }

//-------------------------------------------------------------------------------------
// LastPanOffset: retrieves the last pan Offset.    
//-------------------------------------------------------------------------------------
//
TPoint TGlxZoomAndPanMathsEngine::LastPanOffset()
    {
    TRACER("TGlxZoomAndPanMathsEngine::LastPanOffset()");
    return iLastPanOffset;
    }

//-------------------------------------------------------------------------------------
// MinimumZoomRatio: Retrieves the minimum Zoom threshold.    
//-------------------------------------------------------------------------------------
//
TInt TGlxZoomAndPanMathsEngine::MinimumZoomRatio()
    {
    TRACER("TGlxZoomAndPanMathsEngine::MinimumZoomRatio");
	return iMinZoomRatio;
    }

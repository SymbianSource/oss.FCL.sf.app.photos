/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Slideshow view volume control.
*
*/





#ifndef C_SHWSLIDESHOWVOLUMECONTROL_H
#define C_SHWSLIDESHOWVOLUMECONTROL_H


// INCLUDES
#include <alf/alfcontrol.h>

// FORWARD DECLARATION
class CAlfImageVisual;
class CAlfTextVisual;
class CFbsBitmap;
class CGlxUiUtility;

class CAlfAnchorLayout;
class CAlfBorderBrush;
class CAlfLineVisual;
class CAlfCurvePathLayout;
class CShwSlideshowVolumeControl;
class CAlfImageBrush;
class CAlfGridLayout;
class CAlfDeckLayout;
class CAlfAnchorLayout;

// CLASS DECLARATION
/**
 *  Volume control declaration.
 * @internal reviewed 07/06/2007 by Kimmo Hoikka
 */
// ---------------------------------------------------------------------------
// CShwSlideshowVolumeControl declaration
// --------------------------------------------------------------------------- 
NONSHARABLE_CLASS( CShwSlideshowVolumeControl ) : public CAlfControl
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
     static CShwSlideshowVolumeControl* NewL(
        CAlfEnv& aEnv,
        CGlxUiUtility& aUtility,
        TTimeIntervalMicroSeconds32 aInterval = 0);

    /**
     * Destructor.
     */
    virtual ~CShwSlideshowVolumeControl();

    /**
     * Called when the volume is changed.
     * @param aVolume  The new volume.
     * @param aMaxVolume  The maximum volume.
     */
    void SetVolume(TInt aVolume, TInt aMaxVolume);

    /**
     * Notifies the control that its visible has been changed on a display.
     * This is the earliest time when the control knows the dimensions of
     * the display it is being shown on.
     *
     * @param aIsVisible  ETrue, if the control is now visible on the
     *			display.   EFalse, if the control is about to the hidden 
     *			on the display.
     * @param aDisplay    The display on which the control's visibility 
     * 			is changing.
     * @see CHuiControl
     */	
    void NotifyControlVisibility(TBool aIsVisible, CAlfDisplay& aDisplay);

    /**
     * Called when the control should be redrawn,
     * e.g. after a change in volume.
     */
    void RefreshL();

    /**
     * Called when the control should be hidden,
     * e.g. after a change in volume.
     */
    void Hide();
    
    /**
     * Called when the control should be shown.
     * e.g. Key press and tap event
     */
    TBool ShowControlL();

    /**
     * Method timer calls after time out.
     */
    TInt TimerCallback();

private:

    /**
     * C++ default constructor.
     */
    CShwSlideshowVolumeControl( CAlfEnv& aEnv,
                                CGlxUiUtility& aUtility,
                                TTimeIntervalMicroSeconds32 aInterval);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( );

    /**
     * Helper function to retrieve and position the control's visuals
     */
    void SetAndLayoutVisualsL();

    /**
     * Helper function to convert the current volume to a percentage
     */	
    TInt CurrentVolumeAsPercentage();

    /**
     * Helper function to setup the visuals when volume is muted
     */	
    void SetMuteVisualVisibility();

    /**
     * Helper function to setup the visuals when volume is
     * neither muted nor maximum
     */	
    void SetValueVisualVisibilityL(TInt aVolume);

    /**
     * Helper function to setup the visuals when volume is maximum
     */	
    void CreateFullVolumeVisualsL(TInt aVolume);

    /**
     * Helper function to remove the visuals from the layout
     */	
    void RemoveLayoutVisuals();

    /**
     * Helper function to remove the visuals from the layout
     */	
    void RemoveControlVisuals();
	
private:
	
    CGlxUiUtility& iUtility; // not owned
    
    /** main parent layout*/
    CAlfAnchorLayout* iMainVisual;
    
    /* deck layout holds the backgroung visual, mute visual and the value grid */
    CAlfDeckLayout* iVisualDeck;
    
    /**holds the speaker icon and the percentage text*/
    CAlfGridLayout* iValueGridVisual;
    
    CAlfImageBrush* iBrush; // owned

    // The visual elements of the control
    CAlfImageVisual* iBackgroundImageVisual; // owned
    CAlfImageVisual* iMuteImageVisual;		// owned
    CAlfImageVisual* iSpeakerImageVisual;   // owned
    CAlfTextVisual* iPercentTextVisual;   	// owned

    TInt iVolume;		// owned
    TInt iMaxVolume;	//owned

    CPeriodic*	iTimer;	// owned
    TTimeIntervalMicroSeconds32 iInterval; // owned

    TSize iControlSize; // owned
    CAlfEnv& iAlfEnv; //not owned
    };

#endif  // C_SHWSLIDESHOWVOLUMECONTROL_H

// End of File

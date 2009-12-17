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
* Description:    Black out control class 
*
*/




#ifndef C_SHWBLACKOUTCONTROL_H
#define C_SHWBLACKOUTCONTROL_H

// INCLUDES
#include <uiacceltk/huiControl.h>

// FORWARD DECLARATIONS
class CGlxHuiUtility;
class CHuiAnchorLayout;
class CShwSlideshowView;
class CHuiTexture;
class CHuiImageVisual;

/**
 *  CShwBlackoutControl
 *
 *  Black out control
 *
 *  @lib shwslideshowviewplugin.lib
 * @internal reviewed 07/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS( CShwBlackoutControl ) : public CHuiControl
    {
public:
	/**
	 * Factory function constructor
	 */
	static CShwBlackoutControl* NewL( CHuiEnv& aEnv, CHuiDisplay& aDisplay);
	
	/**
	 * Destructor
	 */
    ~CShwBlackoutControl();
 
public:
	/**
	 * Activates the blackout control:
	 */
    void ActivateL();
    
    /**
     * Deactivates the blackout control.
     */
    void Deactivate();
    
private:
    CShwBlackoutControl(CHuiEnv& aEnv, CHuiDisplay& aDisplay);
    void ConstructL();
	
private:
    CHuiDisplay& iDisplay; // not owned
    CHuiTexture* iBlackoutTexture;	// owned
    CHuiAnchorLayout* iBlackoutLayout;	// not owned
    CHuiImageVisual* iBlackoutImageVisual;    // not owned
    TBool iActivated;	// owned
    };


#endif // C_SHWBLACKOUTCONTROL_H

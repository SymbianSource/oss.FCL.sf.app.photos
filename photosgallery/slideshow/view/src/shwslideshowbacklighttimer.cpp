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
* Description:    Slideshow backlight timer management..
*
*/




#include "shwslideshowbacklighttimer.h"
#include <centralrepository.h>// for peripheral display timeout setting
#include <settingsinternalcrkeys.h> // display timeout setting keys
#include <glxlog.h>
#include <glxtracer.h>
#include "shwcallback.h"

namespace
	{
	const TInt KMicroSecondsInASecond = 1000000;
    
	// @ref: Minimum slideshow transition delay
	const TInt KMinTimeoutDelay = 2; // secs 
	}

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::CShwSlideshowBackLightTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CShwSlideshowBackLightTimer::CShwSlideshowBackLightTimer()
    {
    }

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CShwSlideshowBackLightTimer* CShwSlideshowBackLightTimer::NewL()
	{
	TRACER("CShwSlideshowBackLightTimer::NewL");
	GLX_LOG_INFO("CShwSlideshowBackLightTimer::NewL");
	CShwSlideshowBackLightTimer* self = new( ELeave )
		CShwSlideshowBackLightTimer();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CShwSlideshowBackLightTimer::~CShwSlideshowBackLightTimer()
    {
    TRACER("CShwSlideshowBackLightTimer::~CShwSlideshowBackLightTimer");
    GLX_LOG_INFO("CShwSlideshowBackLightTimer::~CShwSlideshowBackLightTimer");
	delete iPeriodic;
	}

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CShwSlideshowBackLightTimer::ConstructL()
	{
	TRACER("CShwSlideshowBackLightTimer::ConstructL");
	GLX_LOG_ENTRY_EXIT("CShwSlideshowBackLightTimer::ConstructL");
	
    CRepository* lightsRepo = CRepository::NewLC( KCRUidPeripheralSettings );
    // Get display light's timeout value (in seconds ) 
    lightsRepo->Get( KSettingsDisplayLightsTimeout, iSettingsDelay );
    GLX_LOG_INFO1("CShwSlideshowBackLightTimer, lights timeout = %d",
        iSettingsDelay );
    CleanupStack::PopAndDestroy( lightsRepo );
    
    CRepository* scrSaverRepo = CRepository::NewLC( KCRUidPersonalizationSettings );
    // Get screen saver's timeout value (in seconds)
    TInt screenSaverTimeout = 0;
    scrSaverRepo->Get( KSettingsScreenSaverPeriod, screenSaverTimeout );
    GLX_LOG_INFO1("CShwSlideshowBackLightTimer, scr saver timeout = %d",
        screenSaverTimeout );
    CleanupStack::PopAndDestroy( scrSaverRepo );
    
    // Take the smaller of the two timeout values
    iSettingsDelay = Min( iSettingsDelay, screenSaverTimeout );
    
    // Set the timeout delay to minimum value as it can not be zero
    if (iSettingsDelay == 0)
        {
        iSettingsDelay = KMinTimeoutDelay;
        }
    
    // Convert the value to microseconds
    iSettingsDelay *= KMicroSecondsInASecond;
    // Halve the value to ensure out timer kicks beforehand
    iSettingsDelay /= 2;
    
    // Create a timer
    iPeriodic = CPeriodic::NewL( CPeriodic::EPriorityStandard );
	}

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::Tick - handler code for the timer
// -----------------------------------------------------------------------------
//
TInt CShwSlideshowBackLightTimer::Tick()
    {
	// Timer must be reset to prevent screen saver to appear
	User::ResetInactivityTime();
	iStartTime.HomeTime();

	return 1; // continues always
    }

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::StartL
// -----------------------------------------------------------------------------
//
void CShwSlideshowBackLightTimer::StartL()
	{
	TRACER("CShwSlideshowBackLightTimer::StartL()");
    GLX_LOG_INFO("CShwSlideshowBackLightTimer::StartL" );
	iStartTime.HomeTime();
	User::ResetInactivityTime();
    iPeriodic->Cancel(); // in case it actually was running
    iPeriodic->Start(iSettingsDelay, iSettingsDelay,
            TShwCallBack<CShwSlideshowBackLightTimer,
                    &CShwSlideshowBackLightTimer::Tick> (this));
	}

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::Cancel
// -----------------------------------------------------------------------------
//
void CShwSlideshowBackLightTimer::Cancel()
	{
	TRACER("CShwSlideshowBackLightTimer::Cancel()");
	GLX_LOG_INFO("CShwSlideshowBackLightTimer::Cancel" );
	User::ResetInactivityTime();
	iPeriodic->Cancel();
	}

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::IsRunning
// -----------------------------------------------------------------------------
//
TBool CShwSlideshowBackLightTimer::IsRunning()
	{
	TRACER("CShwSlideshowBackLightTimer::IsRunning()");
	GLX_LOG_INFO("CShwSlideshowBackLightTimer::IsRunning" );
	return iPeriodic->IsActive();
	}

// -----------------------------------------------------------------------------
// CShwSlideshowBackLightTimer::Delay
// -----------------------------------------------------------------------------
//
TInt CShwSlideshowBackLightTimer::Delay()
	{
	TRACER("CShwSlideshowBackLightTimer::Delay()");
	GLX_LOG_INFO("CShwSlideshowBackLightTimer::Delay" );
	return iSettingsDelay;
	}

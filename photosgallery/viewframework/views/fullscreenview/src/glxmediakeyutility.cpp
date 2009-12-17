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
* Description:    Command object factory
*
*/




#include <glxtracer.h>
#include <COEMAIN.H>
#include <ipvideo/ViaPlayerMediaKeyObserver.h>  // CViaPlayerMediaKeyObserver
#include <e32Keys.h>
#include <e32event.h>

#include "glxmediakeyutility.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//   
CGlxMediaKeyUtility::CGlxMediaKeyUtility()
    {
    TRACER("CGlxMediaKeyUtility::CGlxMediaKeyUtility()");
    }
    
// ---------------------------------------------------------------------------
// Destructor.  Remove itself as a listner from the MediaKeyObserver
// ---------------------------------------------------------------------------
//   
CGlxMediaKeyUtility::~CGlxMediaKeyUtility()
    {
    TRACER("CGlxMediaKeyUtility::~CGlxMediaKeyUtility()");
    TRAP_IGNORE( CViaPlayerMediaKeyObserver::StaticL().RemoveListener( this ); );
    }
    
// ---------------------------------------------------------------------------
// NewLC - Standard two phase constructor placing itself on the cleanup stack
// ---------------------------------------------------------------------------
//   
CGlxMediaKeyUtility* CGlxMediaKeyUtility::NewLC()
    {
    TRACER("CGlxMediaKeyUtility::NewLC()");
    CGlxMediaKeyUtility* self = new (ELeave) CGlxMediaKeyUtility();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// NewL - Standard two phase constructor
// ---------------------------------------------------------------------------
//   
CGlxMediaKeyUtility* CGlxMediaKeyUtility::NewL()
    {
    TRACER("CGlxMediaKeyUtility::NewL()");
    CGlxMediaKeyUtility* self = CGlxMediaKeyUtility::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
// ---------------------------------------------------------------------------
// ConstructL - Adds itsself as a media key listener               
// ---------------------------------------------------------------------------
//   
void CGlxMediaKeyUtility::ConstructL()
    {
    TRACER("CGlxMediaKeyUtility::ConstructL()");
    CViaPlayerMediaKeyObserver::StaticL().AddListenerL( this );
    }
    
// ---------------------------------------------------------------------------
// MediaKeyEventL - The ViaPlayerMediaKeyObserver sends all RemCon key events
//                  here, where they get translated and dispatched
// ---------------------------------------------------------------------------
//   
void CGlxMediaKeyUtility::MediaKeyEventL(TRemConCoreApiOperationId aOperationId, 
                                         TRemConCoreApiButtonAction aButtonAct )
    {
    TRACER("CGlxMediaKeyUtility::MediaKeyEventL()");
    TStdScanCode scanCode = TranslateKeyEvent(aOperationId);
    DispatchKeyEvent(aButtonAct, scanCode);
    }
    
    
// ---------------------------------------------------------------------------
// DoSimulateKeyEvent - Add key event to application queue
// ---------------------------------------------------------------------------
//   
void CGlxMediaKeyUtility::DoSimulateKeyEvent(TRawEvent::TType aKeyCode, TStdScanCode aScanCode )
	{
    TRACER("CGlxMediaKeyUtility::DoSimulateKeyEvent()");
    TRawEvent rawEvent;
    rawEvent.Set(aKeyCode, aScanCode);
    UserSvr::AddEvent(rawEvent);  
	}

// ---------------------------------------------------------------------------
// TranslateKeyEvent - Translates RemCon keys into standard scan codes
// ---------------------------------------------------------------------------
//   
TStdScanCode CGlxMediaKeyUtility::TranslateKeyEvent(TRemConCoreApiOperationId aOperationId)
    {
    TRACER("CGlxMediaKeyUtility::TranslateKeyEvent()");
    
    TStdScanCode scanCode = EStdKeyNull;
    
    switch (aOperationId)
        {
    case ERemConCoreApiVolumeUp:
        scanCode = EStdKeyIncVolume;
        break;
        
    case ERemConCoreApiVolumeDown:
        scanCode = EStdKeyDecVolume;
  
        break;
        
    default:
        break;
        }
        
    return scanCode;
    }
    
// ---------------------------------------------------------------------------
// DispatchKeyEvent - Interperets the button action and acts accordingly
// ---------------------------------------------------------------------------
//   
void CGlxMediaKeyUtility::DispatchKeyEvent(TRemConCoreApiButtonAction aButtonAct, TStdScanCode aScanCode)
    {
    TRACER("CGlxMediaKeyUtility::DispatchKeyEvent()");

    if (EStdKeyNull == aScanCode)
        {
        return;
        }
        
    switch (aButtonAct)
        {
    case ERemConCoreApiButtonPress:
        DoSimulateKeyEvent(TRawEvent::EKeyDown, aScanCode);
        break;
        
    case ERemConCoreApiButtonRelease:
        DoSimulateKeyEvent(TRawEvent::EKeyUp, aScanCode);
        break;
        
    case ERemConCoreApiButtonClick:
        DoSimulateKeyEvent(TRawEvent::EKeyDown, aScanCode);
        DoSimulateKeyEvent(TRawEvent::EKeyUp, aScanCode);
        break;
        }
    }


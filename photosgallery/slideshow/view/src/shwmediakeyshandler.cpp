/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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




#include <glxlog.h>
#include <glxtracer.h>
#include <coemain.h>
//#include <ViaPlayerMediaKeyObserver.h>  // CViaPlayerMediaKeyObserver
#include <e32keys.h>
#include <e32event.h>

#include "shwmediakeyshandler.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//   
CShwMediaKeyUtility::CShwMediaKeyUtility()
    {
    TRACER("CShwMediaKeyUtility::CShwMediaKeyUtility");
    GLX_LOG_ENTRY_EXIT("CShwMediaKeyUtility::CShwMediaKeyUtility()");
    }
    
// ---------------------------------------------------------------------------
// Destructor.  Remove itself as a listner from the ViaPlayerMediaKeyObserver
// ---------------------------------------------------------------------------
//   
CShwMediaKeyUtility::~CShwMediaKeyUtility()
    {
    TRACER("CShwMediaKeyUtility::~CShwMediaKeyUtility");
    GLX_LOG_ENTRY_EXIT("CShwMediaKeyUtility::~CShwMediaKeyUtility()");
    delete iInterfaceSelector;

    }
    
// ---------------------------------------------------------------------------
// NewLC - Standard two phase constructor placing itself on the cleanup stack
// ---------------------------------------------------------------------------
//   
CShwMediaKeyUtility* CShwMediaKeyUtility::NewLC()
    {
    TRACER("CShwMediaKeyUtility::NewLC");
    GLX_LOG_ENTRY_EXIT("CShwMediaKeyUtility::NewLC()");
    CShwMediaKeyUtility* self = new (ELeave) CShwMediaKeyUtility();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// NewL - Standard two phase constructor
// ---------------------------------------------------------------------------
//   
CShwMediaKeyUtility* CShwMediaKeyUtility::NewL()
    {
    TRACER("CShwMediaKeyUtility::NewL");
    GLX_LOG_ENTRY_EXIT_LEAVE_L("CShwMediaKeyUtility::NewL()");
    CShwMediaKeyUtility* self = CShwMediaKeyUtility::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
    
// ---------------------------------------------------------------------------
// ConstructL - Adds istself as a media key listener to the
//              ViaPlayerMediaKeyObserver
// ---------------------------------------------------------------------------
//   
void CShwMediaKeyUtility::ConstructL()
    {
    TRACER("CShwMediaKeyUtility::ConstructL");
    GLX_LOG_ENTRY_EXIT_LEAVE_L("CShwMediaKeyUtility::ConstructL()");
    iInterfaceSelector = CRemConInterfaceSelector::NewL();

    iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this);
    iInterfaceSelector->OpenTargetL();
    }
    
// ----------------------------------------------------------------------------
// MrccatoCommand()
// Receives events (press/click/release) from the following buttons:
// 'Play/Pause', 'Volume Up', 'Volume Down', 'Stop', 'Rewind', 'Forward'
// ----------------------------------------------------------------------------
//   
void CShwMediaKeyUtility::MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
                                     TRemConCoreApiButtonAction aButtonAct)
    {
    TRACER("CShwMediaKeyUtility::MrccatoCommand");
    GLX_LOG_ENTRY_EXIT("CShwMediaKeyUtility::MediaKeyEventL()");
    TStdScanCode scanCode = TranslateKeyEvent(aOperationId);
    DispatchKeyEvent(aButtonAct, scanCode);
    }
    
    
// ---------------------------------------------------------------------------
// DoSimulateKeyEvent - Add key event to application queue
// ---------------------------------------------------------------------------
//   
void CShwMediaKeyUtility::DoSimulateKeyEvent(TRawEvent::TType aKeyCode, 
                                                        TStdScanCode aScanCode )
	{
	TRACER("CShwMediaKeyUtility::DoSimulateKeyEvent");
    GLX_LOG_ENTRY_EXIT("CShwMediaKeyUtility::DoSimulateKeyEvent()");
    TRawEvent rawEvent;
    rawEvent.Set(aKeyCode, aScanCode);
    UserSvr::AddEvent(rawEvent);  
	}

// ---------------------------------------------------------------------------
// TranslateKeyEvent - Translates RemCon keys into standard scan codes
// ---------------------------------------------------------------------------
//   
TStdScanCode CShwMediaKeyUtility::TranslateKeyEvent(TRemConCoreApiOperationId aOperationId)
    {
    TRACER("CShwMediaKeyUtility::TranslateKeyEvent");
    GLX_LOG_ENTRY_EXIT("CShwMediaKeyUtility::TranslateKeyEvent()");
    
    TStdScanCode scanCode = EStdKeyNull;
    TRequestStatus status;
    
    switch (aOperationId)
        {
    case ERemConCoreApiVolumeUp:
        scanCode = EStdKeyIncVolume;
        break;
        
    case ERemConCoreApiVolumeDown:
        scanCode = EStdKeyDecVolume;
        break;
    case ERemConCoreApiPausePlayFunction:
        scanCode = EStdKeyDictaphonePlay;
        iCoreTarget->PausePlayFunctionResponse(status, KErrNone); 
        User::WaitForRequest(status);
  
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
void CShwMediaKeyUtility::DispatchKeyEvent(TRemConCoreApiButtonAction aButtonAct,
                                                            TStdScanCode aScanCode)
    {
    TRACER("CShwMediaKeyUtility::DispatchKeyEvent");
    GLX_LOG_ENTRY_EXIT("CShwMediaKeyUtility::DispatchKeyEvent()");

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


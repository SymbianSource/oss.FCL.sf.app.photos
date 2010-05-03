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
* Description:    Implementation of Tv connection Observer class 
*
*/

#include "glxtv.h"                      // for CGlxTv
#include "glxtvconnectionobserver.h"
#include "glxtvoutwrapper_p.h"


// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
CGlxConnectionObserver* CGlxConnectionObserver::NewL(GlxTvOutWrapperPrivate* aTvWrapperPrivate)
    {
    CGlxConnectionObserver* self = new (ELeave)CGlxConnectionObserver(aTvWrapperPrivate);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CGlxConnectionObserver::ConstructL()
    {
    iGlxTvOut = CGlxTv::NewL(*this);    
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CGlxConnectionObserver::CGlxConnectionObserver(GlxTvOutWrapperPrivate* aTvWrapperPrivate):
            iTvWrapperPrivate(aTvWrapperPrivate)
    {
    // ctor
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CGlxConnectionObserver::~CGlxConnectionObserver()
    {
    if (iGlxTvOut){
        delete iGlxTvOut;
        iGlxTvOut = NULL;
        }
    }

// -----------------------------------------------------------------------------
// HandleTvStatusChangedL
// -----------------------------------------------------------------------------
void CGlxConnectionObserver::HandleTvStatusChangedL(TTvChangeType aChangeType)
    {
    if ( aChangeType == ETvConnectionChanged ){
        if ( iGlxTvOut->IsHDMIConnected() ){
            iTvWrapperPrivate->HandleConnectionChange(ETrue);
            // emit HDMI connected signal
            }
        else{
            iTvWrapperPrivate->HandleConnectionChange(EFalse);
            // emit HDMI disconnected
            }
        }
    }

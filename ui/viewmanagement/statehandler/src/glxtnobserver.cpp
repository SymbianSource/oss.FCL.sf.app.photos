/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/

#include <glxtnobserver.h>
#include <e32property.h>

const TUid KTAGDPSNotification = { 0x2001FD51 };
const TInt KForceBackgroundGeneration = 0x00000010;
const TInt KItemsleft = 0x00000008;

GlxTNObserver::GlxTNObserver() : mTNMonitor( NULL )
{
    RProperty::Set( KTAGDPSNotification, KForceBackgroundGeneration, ETrue );
}

void GlxTNObserver::updateTNCount(int &count)
{
    emit leftTNCount( count );
}

int GlxTNObserver::getTNLeftCount()
{
    TInt leftVariable =  0;
    RProperty::Get( KTAGDPSNotification, KItemsleft, leftVariable );
    //To:Do error handling
    return leftVariable;
}

void GlxTNObserver::startTNObserving()
{
    mTNMonitor = CGlxTNMonitor::NewL( this );
}

GlxTNObserver::~GlxTNObserver()
{
    delete mTNMonitor;
}

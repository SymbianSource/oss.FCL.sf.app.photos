/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Stub for ADIS processing methods
*
*/



#include <AdisDisplayChain.h>

EXPORT_C TInt CAdisDisplayChain::ParSetsAvailable()
    {
    return 2;
    }

EXPORT_C CAdisDisplayChain* CAdisDisplayChain::NewL(
    TInt aParSetIndex )
    {
    CAdisDisplayChain* self = NewLC(aParSetIndex);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CAdisDisplayChain* CAdisDisplayChain::NewLC(
    TInt /*aParSetIndex*/ )
    {
    CAdisDisplayChain* self = new (ELeave) CAdisDisplayChain;
    CleanupStack::PushL(self);
    return self;
    }

CAdisDisplayChain::CAdisDisplayChain()
    {
    }

EXPORT_C CAdisDisplayChain::~CAdisDisplayChain()
    {
    }

EXPORT_C TBool CAdisDisplayChain::IsSupported( TADCPar /*aParID*/ )
    {
    return ETrue;
    }

EXPORT_C void CAdisDisplayChain::ProcessL( 
    const CFbsBitmap* /*aScalerInputBitmap*/, 
    CFbsBitmap&       /*aBitmap*/ )
    {
    // Do nothing
    }

EXPORT_C void CAdisDisplayChain::ProcessL( 
    TRequestStatus&   aRequestStatus, 
    const CFbsBitmap* aScalerInputBitmap, 
    CFbsBitmap&       aBitmap )
    {
    aRequestStatus = KRequestPending;

    ProcessL(aScalerInputBitmap, aBitmap);

    TRequestStatus* status = &aRequestStatus;
    User::RequestComplete(status, KErrNone);
    }

EXPORT_C void CAdisDisplayChain::Cancel()
    {
    // Do nothing
    }

EXPORT_C void CAdisDisplayChain::SetParL( const TAny* /*aPar*/, TADCPar /*aParID*/ )
    {
    // Do nothing
    }

EXPORT_C void CAdisDisplayChain::GetParL( TAny* /*aPar*/, TADCPar /*aParID*/ )
    {
    // Do nothing
    }

//  END OF FILE

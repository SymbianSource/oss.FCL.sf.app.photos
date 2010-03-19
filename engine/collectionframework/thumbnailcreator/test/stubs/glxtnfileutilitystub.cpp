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
* Description:   Utility for thumbnail tasks handling files
*
*/



#include <glxtnfileutility.h>

/*static*/ CGlxtnFileUtility* CGlxtnFileUtility::NewL()
    {
    CGlxtnFileUtility* self = new (ELeave) CGlxtnFileUtility;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

CGlxtnFileUtility::~CGlxtnFileUtility()
    {
	iFs.Close();
    }

RFs& CGlxtnFileUtility::FsSession()
    {
    return iFs;
    }

void CGlxtnFileUtility::CheckBadFileListL(const TDesC& /*aFilename*/)
    {
    }

void CGlxtnFileUtility::ClearBadFileMarker()
    {
    }

TBool CGlxtnFileUtility::IsPersistentSize(const TSize& /*aSize*/)
    {
    return EFalse;
    }

CGlxtnFileUtility::CGlxtnFileUtility()
    {
    }

void CGlxtnFileUtility::ConstructL()
    {
	User::LeaveIfError(iFs.Connect());
    }

//  END OF FILE

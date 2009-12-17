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
* Description:    Collection info class
*
*/




#include "glxcollectioninfo.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxCollectionInfo* CGlxCollectionInfo::NewL(TUint32 aCollectionId)
    {
    CGlxCollectionInfo* self = new (ELeave) CGlxCollectionInfo(aCollectionId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxCollectionInfo::~CGlxCollectionInfo()
    {
    delete iTitle;
    }

// -----------------------------------------------------------------------------
// Id
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CGlxCollectionInfo::Id() const
    {   
    return iItemId;
    }

// -----------------------------------------------------------------------------
// Exists
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxCollectionInfo::Exists() const
    {
    return iExists;
    }

// -----------------------------------------------------------------------------
// Title
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CGlxCollectionInfo::Title() const
    {
    if (iTitle)
    	{
        return *iTitle;
    	}
    else
    	{
    	return KNullDesC;
    	}
    }

// -----------------------------------------------------------------------------
// SetExists
// -----------------------------------------------------------------------------
//
void CGlxCollectionInfo::SetExists(TBool aExists)
    {
    iExists = aExists;
    }

// -----------------------------------------------------------------------------
// SetTitleL
// -----------------------------------------------------------------------------
//
void CGlxCollectionInfo::SetTitleL(const TDesC& aTitle)
    {
    delete iTitle;
    iTitle = NULL;
    iTitle = aTitle.AllocL();
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxCollectionInfo::CGlxCollectionInfo(TInt aCollectionId)
    : iItemId(aCollectionId)
    {
    }

// -----------------------------------------------------------------------------
// Second stage constructor
// -----------------------------------------------------------------------------
//
void CGlxCollectionInfo::ConstructL()
    {
    }

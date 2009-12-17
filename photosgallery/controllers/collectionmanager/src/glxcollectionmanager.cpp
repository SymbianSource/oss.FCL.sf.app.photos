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
* Description:    Implementation of collection manager API for external use
*
*/




#include "glxcollectionmanager.h"
#include "glxcollectionmanagerimpl.h"

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxCollectionManager*  CGlxCollectionManager::NewL()
	{
	CGlxCollectionManager* self = new (ELeave) CGlxCollectionManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxCollectionManager::CGlxCollectionManager()
    {

    }
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxCollectionManager::~CGlxCollectionManager()
    {
    delete iImplementation;
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManager::AddToCollectionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionManager::AddToCollectionL(const TDesC& aUri, 
                        TCollection aCollection, TRequestStatus& aRequestStatus)
    {
    iImplementation->AddToContainerL(aUri, aCollection, aRequestStatus);
    }
// -----------------------------------------------------------------------------
// CGlxCollectionManager::AddToCollectionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionManager::AddToCollectionL(const TDesC& aUri, 
                        TUint32 aCollectionId, TRequestStatus& aRequestStatus)
    {
    iImplementation->AddToContainerL(aUri, aCollectionId, aRequestStatus);
    }
// -----------------------------------------------------------------------------
// CGlxCollectionManager::CollectionInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionManager::CollectionInfoL(CGlxCollectionInfo& aInfo,
                        TRequestStatus& aRequestStatus)
    {
    iImplementation->CollectionInfoL(aInfo, aRequestStatus);
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManager::Cancel
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxCollectionManager::Cancel()
    {
    iImplementation->Cancel();
    }

// -----------------------------------------------------------------------------
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CGlxCollectionManager::ConstructL()
	{
	iImplementation = new (ELeave) 
			CGlxCollectionManagerImpl(); 
	}

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
* Description:    Implementation of list view
*
*/




// INCLUDE FILES
#include "glxlistview.h"
#include "glxlistviewimp.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
// 
EXPORT_C CGlxListView* CGlxListView::NewL(
        MGlxMediaListFactory* aMediaListFactory, 
        TInt aViewUid,  
        TListViewResourceIds& aResourceIds, 
        const TDesC& aTitle)
    {
    return CGlxListViewImp::NewL(aMediaListFactory, aViewUid, aResourceIds,	aTitle);
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxListView* CGlxListView::NewLC(
        MGlxMediaListFactory* aMediaListFactory, 
        TInt aViewUid,  
        TListViewResourceIds& aResourceIds,         
        const TDesC& aTitle)
    {
    return CGlxListViewImp::NewLC(aMediaListFactory, aViewUid, 
    							aResourceIds, aTitle);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxListView::~CGlxListView()
    {
    // Do nothing
    }
//  End of File


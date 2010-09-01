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
* Description:    Implementation of Grid view
*
*/





// INCLUDE FILES
#include "glxgridview.h" 
#include "glxgridviewimp.h"
#include <glxlog.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CGlxGridView* CGlxGridView::NewL(MGlxMediaListFactory* aMediaListFactory,
                                 const TGridViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle)
    {
    GLX_LOG_INFO("CGlxGridView::NewL()");
    return CGlxGridViewImp::NewL(aMediaListFactory,
                                 aResourceIds,
                                 aViewUID,
                                 aTitle);
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxGridView* CGlxGridView::NewLC(MGlxMediaListFactory* aMediaListFactory,
                                 const TGridViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle)
    {
    GLX_LOG_INFO("CGlxGridView::NewLC()");
    return CGlxGridViewImp::NewLC(aMediaListFactory,
                                  aResourceIds,
                                  aViewUID,
                                  aTitle);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxGridView::~CGlxGridView()
    {
    GLX_LOG_INFO("CGlxGridView::~CGlxGridView()");
    // Do nothing
    }

//  End of File

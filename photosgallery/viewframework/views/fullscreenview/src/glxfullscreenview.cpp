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
* Description:    Implementation of fULL-SCREEN view
*
*/





// INCLUDE FILES
#include "glxfullscreenview.h" 
#include "glxfullscreenviewimp.h"
#include <glxtracer.h>
#include <glxlog.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CGlxFullScreenView* CGlxFullScreenView::NewL(MGlxMediaListFactory* aMediaListFactory,
                                 const TFullScreenViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle)
    {
    TRACER("CGlxFullScreenView::NewL()");
    return CGlxFullScreenViewImp::NewL(aMediaListFactory,
                                 aResourceIds,
                                 aViewUID,
                                 aTitle);
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxFullScreenView* CGlxFullScreenView::NewLC(MGlxMediaListFactory* aMediaListFactory,
                                 const TFullScreenViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle)
    {
    GLX_LOG_INFO("CGlxFullScreenView::NewLC()");
    return CGlxFullScreenViewImp::NewLC(aMediaListFactory,
                                  aResourceIds,
                                  aViewUID,
                                  aTitle);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxFullScreenView::~CGlxFullScreenView()
    {
    GLX_LOG_INFO("CGlxFullScreenView::~CGlxFullScreenView()");
    // Do nothing
    }

//  End of File

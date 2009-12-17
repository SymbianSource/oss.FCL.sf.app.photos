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
* Description:  Implementation of Metadata view
*
*/


// INCLUDE FILES
#include "glxmetadataview.h" 
#include "glxmetadataviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CGlxMetadataView* CGlxMetadataView::NewL( MGlxMediaListFactory* aMediaListFactory )
    {
    return CGlxMetadataViewImp::NewL( aMediaListFactory );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxMetadataView* CGlxMetadataView::NewLC( MGlxMediaListFactory* aMediaListFactory )
    {
    return CGlxMetadataViewImp::NewLC( aMediaListFactory );
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMetadataView::~CGlxMetadataView()
    {
    // Do nothing
    }

//  End of File

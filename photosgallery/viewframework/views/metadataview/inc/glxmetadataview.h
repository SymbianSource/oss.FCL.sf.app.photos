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
* Description:  Metadata view implementation
*
*/


#ifndef C_GLXMETADATAVIEW_H
#define C_GLXMETADATAVIEW_H


// INCLUDES
#include <glxmedialistviewbase.h>

// CLASS DECLARATION

/**
 *  Metadata view
 *
 */
NONSHARABLE_CLASS(CGlxMetadataView) :public CGlxMediaListViewBase
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    IMPORT_C static CGlxMetadataView* NewL( MGlxMediaListFactory* aMediaListFactory );

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    IMPORT_C static CGlxMetadataView* NewLC( MGlxMediaListFactory* aMediaListFactory );

    /**
     * Destructor.
     */
    virtual ~CGlxMetadataView();
    
    };

#endif  // C_GLXMETADATAVIEW_H

// End of File


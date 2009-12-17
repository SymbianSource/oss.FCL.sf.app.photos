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
* Description:    Tile view implementation
*
*/





#ifndef C_GLXFULLSCREENVIEW_H
#define C_GLXFULLSCREENVIEW_H


// INCLUDES
#include <glxmedialistviewbase.h>


    class TFullScreenViewResourceIds 
        {
    public:
        TInt           iViewId;     // a AVKON_VIEW
        TInt           iMenuId;     // a MENU_PANE
        TInt           iCbaId;      // a CBA
        };

// CLASS DECLARATION

/**
 *  Tile view
 *
 *  @lib glxtileview.lib
 */
NONSHARABLE_CLASS(CGlxFullScreenView) : public CGlxMediaListViewBase
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CGlxFullScreenView* NewL(MGlxMediaListFactory* aMediaListFactory,
                                 const TFullScreenViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle = KNullDesC());

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CGlxFullScreenView* NewLC(MGlxMediaListFactory* aMediaListFactory,
                                 const TFullScreenViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle = KNullDesC());

    /**
     * Destructor.
     */
    virtual ~CGlxFullScreenView();
    };
#endif  // C_GLXFULLSCREENVIEW_H

// End of File

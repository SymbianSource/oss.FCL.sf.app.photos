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





#ifndef C_GLXGRIDVIEW_H
#define C_GLXGRIDVIEW_H


// INCLUDES
#include <glxmedialistviewbase.h>

class MGlxMediaListFactory;

    class TGridViewResourceIds 
        {
    public:
        TInt           iViewId;     // a AVKON_VIEW
        TInt           iMenuId;     // a MENU_PANE
        TInt           iCbaId;      // a CBA
        TInt           iEmptyViewTextId;       // a TBUF
        TInt           iOkOptionsMenuId;
        };

// CLASS DECLARATION

/**
 *  Tile view
 *
 *  @lib glxtileview.lib
 */
NONSHARABLE_CLASS(CGlxGridView) : public CGlxMediaListViewBase//, public MGlxLayoutOwner
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CGlxGridView* NewL(MGlxMediaListFactory* aMediaListFactory,
                                 const TGridViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle = KNullDesC());

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CGlxGridView* NewLC(MGlxMediaListFactory* aMediaListFactory,
                                 const TGridViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle = KNullDesC());

    /**
     * Destructor.
     */
    virtual ~CGlxGridView();
    };

#endif  // C_GLXTILEVIEW_H

// End of File

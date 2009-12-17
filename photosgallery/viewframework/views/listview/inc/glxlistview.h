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
* Description:    List view implementation
*
*/




#ifndef C_GLXLISTVIEW_H
#define C_GLXLISTVIEW_H

// INCLUDES

#include <glxmedialistviewbase.h>

// CLASS DECLARATION

/**
 * Struct to hold resource ids
 *
 * @ingroup glx_view_list_view
 */
struct TListViewResourceIds
    {
    /// Avkon view resource ID.
    TInt iViewId;
    /// menu bar resource ID
    TInt iMenuId;
    /// Softkey resource ID
    TInt iCbaId;
    /// String resource ID for text to display when the list is empty
    TInt iEmptyTextId;
    };


/**
 *  View which displays a vertically scrolling list control.
 *
 */
class CGlxListView : public CGlxMediaListViewBase
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aMediaListFactory object providing a media list
     * @param aViewUid plugin implementation uid
     * @param aResourceIds containing view specific resource ids
     * @param aTitle title of the view
     *
     * @return pointer to created view
     */
    IMPORT_C static CGlxListView* NewL(MGlxMediaListFactory* aMediaListFactory, 
                                       TInt aViewUid,  
                                       TListViewResourceIds& aResourceIds,
                                       const TDesC& aTitle = KNullDesC());


    /**
     * Two-phased constructor.
     *
     * @param aMediaListFactory object providing a media list
     * @param aViewUid plugin implementation uid
     * @param aResourceIds containing view specific resource ids
     * @param aTitle title of the view
     *
     * @return pointer to created view
     */
    IMPORT_C static CGlxListView* NewLC(MGlxMediaListFactory* aMediaListFactory, 
                                        TInt aViewUid,  
                                        TListViewResourceIds& aResourceIds,
                                        const TDesC& aTitle = KNullDesC());


    /**
     * Destructor.
     */
    virtual ~CGlxListView();
    };

#endif  // C_GLXLISTVIEW_H

// End of File

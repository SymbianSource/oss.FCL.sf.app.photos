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
* Description:    Visual list window
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#ifndef C_GLXVISUALLISTWINDOW_H
#define C_GLXVISUALLISTWINDOW_H

#include <glxlayout.h>

#include "glxlistwindow.h"
#include "glxitemvisual.h"
#include "mglxmedialist.h"

class CGlxVisualListControl;
class CGlxVisualObject;
//class TGlxBlendLayout;
class MGlxMediaList;
class CGlxUiUtility;

/**
 *  CGlxVisualListWindow
 *
 *  Item visual list window
 *
 * @author Aki Vanhatalo
 */
NONSHARABLE_CLASS( CGlxVisualListWindow ) : public CGlxListWindow,
        public MGlxWindowObjectFactory,
        public MGlxVisualObjectParameterFactory
    {
    public:
        /**
         * Constructor.
         * @param aControl Owner control for visuals
         * @param aLayout Parent layout for visuals
         * @param aMediaList Media list of items the visuals represent
         * @param aHuiUtility HUI utility
         * @param aThumbnailScaleMode Scale mode to use for the visuals
         */
        static CGlxVisualListWindow* NewL(
            CGlxVisualListControl* aControl,            
            MGlxMediaList* aMediaList, 
            CGlxUiUtility* aUiUtility,
            CAlfImageVisual::TScaleMode aThumbnailScaleMode );


    public: 

        CGlxVisualObject* GetObjectL( TInt aListIndex );
        void CleanupObject( TInt aWindowIndex );
        void PostObjectsAdded( RArray<TInt>& aAddedAtListIndexes );
        void SetFocusIndexL( TInt aIndex );
        void UpdatePositions();
        CBase* CreateObjectL() const;
        void SetupObject(TInt aListIndex, CBase& aObject );
        void CleanupObject(TInt /*aListIndex*/, CBase& /*aObject*/ ); 
		void SetupObjectL( TInt aIndex, CBase& aObject );

    public:	// from MGlxVisualObjectParameterFactory
    
        CAlfLayout* Layout();
        CAlfControl& VisualOwner();
        CAlfBrush* BorderBrush();
        CAlfImageVisual::TScaleMode ThumbnailScaleMode();
        CGlxVisualObject* ObjectByIndex( TInt aIndex ); 

    private:
        
 		CGlxVisualListWindow();
        /** Owner control for visuals (not owned) */
        CGlxVisualListControl* iControl;
        /** Media list of items the visuals represent (not owned) */
        MGlxMediaList* iMediaList;
        /** UI utility (not owned) */
        CGlxUiUtility* iUiUtility;
        /** Scale mode to use for the visuals */
        CAlfImageVisual::TScaleMode iScaleMode;

    };

 
#endif // C_GLXVISUALLISTWINDOW_H





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
* Description:    Grid view implementation
*
*/

#ifndef C_GLXGRIDLVIEWIMP_H
#define C_GLXGRIDLVIEWIMP_H

// INCLUDES
#include <eikspmod.h>
#include <eikclb.h>

#include <glxuistd.h>
#include <mglxvisuallistobserver.h>                 // for NGlxListDefs::TFocusChangeType

// INCLUDES
#include "glxgridview.h"
#include "glxgridviewcontainer.h"

// FORWARD DECLARATIONS
class CGlxActiveMediaListRegistry;
class MGlxMediaListFactory;
class CGlxDefaultAttributeContext;

// CLASS DECLARATION
/**
 *  MPX collection view.
 *
 *  @lib glxgridview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS(CGlxGridViewImp) : public CGlxGridView
                                     ,public MGlxGridEventObserver 
    {
public:
    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CGlxGridViewImp* NewL(MGlxMediaListFactory* aMediaListFactory,
                                 const TGridViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle);

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CGlxGridViewImp* NewLC(MGlxMediaListFactory* aMediaListFactory,
                                 const TGridViewResourceIds& aResourceIds,
                                 TInt aViewUID,
                                 const TDesC& aTitle);

    /**
     * Destructor.
     */
    virtual ~CGlxGridViewImp();

public: // From CGlxViewBase
    TBool HandleViewCommandL(TInt aCommand);

public: // From CGlxMediaListViewBase    
    void DoMLViewActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
        const TDesC8& aCustomMessage);
    
    void DoMLViewDeactivate();
    
public:    // from MGlxGridEventObserver
    void HandleGridEventsL(TInt aCmd);
    void HandleLatchToolbar();
   
private:// from base class CAknView
    TUid Id() const;
    
    void HandleForegroundEventL(TBool aForeground);

private:
    /**
     * C++ default constructor.
     */
    CGlxGridViewImp(const TGridViewResourceIds& aResourceIds, TInt aViewUID);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(MGlxMediaListFactory* aMediaListFactory,
                    const TDesC& aTitle);

	void DestroyGridWidget();
    
private:// Data

    TGridViewResourceIds  iResourceIds;
    /// UID of the plugin creating the Grid view
    TInt iViewUID;
    
    /// Active media list registry stores the pointer to the active media list
    /// Allows UPnP to know when list has changed
    CGlxActiveMediaListRegistry* iActiveMediaListRegistry;

    /// Previous navigation
    NGlxListDefs::TFocusChangeType iPreviousFocusChangeType;

    // save title text 
    HBufC* iTitletext;
    
    //Create control
    CGlxGridViewContainer* iGlxGridViewContainer;
    };

#endif  // C_GLXGRIDLVIEWIMP_H

// End of File

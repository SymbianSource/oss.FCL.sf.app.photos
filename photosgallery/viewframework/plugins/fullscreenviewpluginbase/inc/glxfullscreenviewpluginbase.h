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
* Description:    MPX Tile view plugin definition.
*
*/





#ifndef C_GLXFULLSCREENVIEWPLUGINBASE_H
#define C_GLXFULLSCREENVIEWPLUGINBASE_H


// INCLUDES
#include <mpxaknviewplugin.h>
#include "glxmedialistfactory.h"
#include "glxfullscreenview.h"   // for TTileViewResourceIds

// CLASS DECLARATION

/**
 *  MPX Collection view plugin definition.
 *
 *  @lib mpxcollectionviewplugin.lib
 *  @since S60 v3.0
 */
class CGlxFullScreenViewPluginBase  : public CMPXAknViewPlugin, public MGlxMediaListFactory
    {
public:

    /**
 
    /**
     * Destructor.
     */
IMPORT_C    virtual ~CGlxFullScreenViewPluginBase();

protected:

    /**
     * From CMPXAknViewPlugin
     * Construct Avkon view.
     *
     * @since S60 v3.0
     * @return Pointer to a newly created Avkon view.
     */
IMPORT_C   CAknView* ConstructViewLC();

    /**
     * Override to add command handlers to the view
     *
     */
virtual void AddCommandHandlersL();

    /**
     * Override to define resource file 
     *
     */
virtual void GetResourceFilenameL(TFileName& aResFile);

public:

    /**
     * C++ default constructor.
     */
IMPORT_C    CGlxFullScreenViewPluginBase();

private:
    /**
     * Callback for periodic timer, non-static, and hence allows this
     * pointer to be used
     */
    static TInt PeriodicCallback( TAny* aPtr );
    /**
     * Callback for periodic timer, non-static, and hence allows this
     * pointer to be used
     */
    void PeriodicCallback();
    
    
protected:  
    // Pointer to the created tile view, not owned  
    CGlxFullScreenView* iFullScreenView;

    TFullScreenViewResourceIds  iResourceIds;
    TInt   iResourceOffset;
    TInt   iViewUID;
    //View Specific Widget Ids
    TViewWidgetIds iWidgetIds;
	CPeriodic* iPeriodic; 
    };

#endif  // C_GLXFULLSCREENVIEWPLUGINBASE_H

// End of File

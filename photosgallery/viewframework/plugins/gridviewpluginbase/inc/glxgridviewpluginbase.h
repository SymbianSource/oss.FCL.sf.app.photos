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
* Description:    MPX Grid view plugin definition.
*
*/





#ifndef C_GLXGRIDVIEWPLUGINBASE_H
#define C_GLXGRIDVIEWPLUGINBASE_H


// INCLUDES
#include <mpxaknviewplugin.h>
#include "glxmedialistfactory.h"
#include "glxgridview.h"   // for TGridViewResourceIds

// CLASS DECLARATION

/**
 *  MPX Collection view plugin definition.
 *
 *  @lib mpxcollectionviewplugin.lib
 *  @since S60 v3.0
 */
class CGlxGridViewPluginBase  : public CMPXAknViewPlugin, public MGlxMediaListFactory
    {
public:

    /**
     * Destructor.
     */
IMPORT_C    virtual ~CGlxGridViewPluginBase();

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
IMPORT_C virtual void AddCommandHandlersL();

    /**
     * Override to define resource file 
     *
     */
IMPORT_C virtual void GetResourceFilenameL(TFileName& aResFile);

public:

    /**
     * C++ default constructor.
     */
IMPORT_C    CGlxGridViewPluginBase();

    
    
protected:  
    // Pointer to the created Grid view, not owned  
    CGlxGridView* iGridView;

    TGridViewResourceIds  iResourceIds;
    TInt   iResourceOffset;
    TInt   iViewUID;
    //View Specific Widget Ids
        TViewWidgetIds iWidgetIds;
    };

#endif  // C_GLXGRIDVIEWPLUGINBASE_H

// End of File

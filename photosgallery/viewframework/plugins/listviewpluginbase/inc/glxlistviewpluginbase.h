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
* Description:    MPX List view plugin definition.
*
*/




#ifndef C_GLXLISTVIEWPLUGINBASE_H
#define C_GLXLISTVIEWPLUGINBASE_H

// INCLUDES
#include <mpxaknviewplugin.h>

#include <glxmedialistfactory.h>
#include <glxlistview.h>   						// ConstructListViewLC

class TListViewResourceIds;

// CLASS DECLARATION

/**
 *  MPX Collection view plugin definition.
 *
 *  @lib glxlistviewpluginbase.lib
 */
class CGlxListViewPluginBase : public CMPXAknViewPlugin, 
                               public MGlxMediaListFactory
    {
public:
    /**
     * C++ default constructor.
     */
    IMPORT_C CGlxListViewPluginBase();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CGlxListViewPluginBase();

protected:
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

private:
    /**
     * From CMPXAknViewPlugin
     * Construct Avkon view.
     *
     * @return Pointer to a newly created Avkon view.
     */
    IMPORT_C CAknView* ConstructViewLC();

    /**
     * Construct GLX list view.
     *
     * @param aMediaListFactory object providing media list
     *
     * @return Pointer to a newly created list view.
     */
    virtual CGlxListView* ConstructListViewLC(MGlxMediaListFactory* aMediaListFactory) = 0;

protected:
    // Created list view - not owned
    CGlxListView* iListView;

    // View specific resource ids
    TListViewResourceIds iResourceIds;
    
    //View Specific Widget Ids
    TViewWidgetIds iWidgetIds;

    // Resource offset
    TInt iResourceOffset;
    };

#endif  // C_GLXLISTVIEWPLUGINBASE_H

// End of File

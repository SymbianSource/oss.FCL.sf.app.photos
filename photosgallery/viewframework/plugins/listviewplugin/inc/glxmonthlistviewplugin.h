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
* Description:    List view plugin definition.
*
*/




#ifndef C_GLXMONTHLISTVIEWPLUGIN_H
#define C_GLXMONTHLISTVIEWPLUGIN_H

// INCLUDES
#include <glxlistviewpluginbase.h>

// CLASS DECLARATION

/**
 * Gallery month list view plugin.
 *
 * @ingroup list_view
 */
NONSHARABLE_CLASS( CGlxMonthListViewPlugin ) : public CGlxListViewPluginBase
    {
public:
    /**
     * Two-phased constructor for month list view plugin.
     */
    static CGlxMonthListViewPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CGlxMonthListViewPlugin();

private: // From CGlxListViewPluginBase
    /**
     * Construct the list view
     *
     * @param aMediaListFactory object providing media list
     *
     * @return pointer to created list view
     */
    CGlxListView* ConstructListViewLC(MGlxMediaListFactory* aMediaListFactory);

    /**
     * Add view specific command handlers
     */
    void AddCommandHandlersL();

private:
    /**
     * C++ default constructor.
     */
    CGlxMonthListViewPlugin();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_GLXMONTHLISTVIEWPLUGIN_H

// End of File

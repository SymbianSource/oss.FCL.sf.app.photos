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




#ifndef C_GLXMAINLISTVIEWPLUGIN_H
#define C_GLXMAINLISTVIEWPLUGIN_H

// INCLUDES
#include <glxlistviewpluginbase.h>

// CLASS DECLARATION

/**
 * Gallery main list view plugin.
 *
 * @ingroup list_view
 */
NONSHARABLE_CLASS( CGlxMainListViewPlugin ) : public CGlxListViewPluginBase
    {
public:
    /**
     * Two-phased constructor for main list view plugin.
     */
    static CGlxMainListViewPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CGlxMainListViewPlugin();

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
    CGlxMainListViewPlugin();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_GLXMAINLISTVIEWPLUGIN_H

// End of File

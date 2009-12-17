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





#ifndef C_GLXMAINGRIDVIEWPLUGIN_H
#define C_GLXMAINGRIDVIEWPLUGIN_H


// INCLUDES
#include <glxgridviewpluginbase.h>


// CLASS DECLARATION

/**
 *  MPX Collection view plugin definition.
 *
 *  @lib mpxcollectionviewplugin.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CGlxMainGridViewPlugin ) : public CGlxGridViewPluginBase
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CGlxMainGridViewPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CGlxMainGridViewPlugin();


private:

    /**
      * Add plugin specific command handlers
      */     
    void AddCommandHandlersL();
 
    /**
     * From CGlxGridViewPluginBase
     *
     */
    
    void GetResourceFilenameL(TFileName& aResFile);



private:

    /**
     * C++ default constructor.
     */
    CGlxMainGridViewPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


    };

#endif  // C_GLXMAINGRIDVIEWPLUGIN_H

// End of File
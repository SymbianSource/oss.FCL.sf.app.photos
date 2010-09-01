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
* Description:    MPX FullScreen view plugin definition.
*
*/




// INCLUDES
#include <glxfullscreenviewpluginbase.h>

#ifndef C_GLXTESTFULLSCREENVIEWPLUGIN_H
#define C_GLXTESTFULLSCREENVIEWPLUGIN_H

// CLASS DECLARATION


/**
 * CGlxTagFullScreenViewPlugin
 */
 /*  MPX Collection view plugin definition.
 *
 *  @lib mpxcollectionviewplugin.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CGlxTagFullScreenViewPlugin ) : public CGlxFullScreenViewPluginBase
    {
     //Unit Testing
            
     friend class ut_CGlxTagFullScreenViewPlugin;
public:
    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CGlxTagFullScreenViewPlugin* NewL();
 
    /**
     * Destructor.
     */
    virtual ~CGlxTagFullScreenViewPlugin();

private:
	   //From CGlxFullScreenViewPluginBase	
     /**
     * Adds Command Handlers to the view
     */
    void AddCommandHandlersL();
    /**
     * Define the resource file
     * @param filename
     */    
    void GetResourceFilenameL(TFileName& aResFile);
    
private:
    /**
     * C++ default constructor.
     */
    CGlxTagFullScreenViewPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();     
    };

#endif  // C_GLXTESTFULLSCREENVIEWPLUGIN_H

// End of File

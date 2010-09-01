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




// INCLUDES
#include <glxgridviewpluginbase.h>

#ifndef C_GLXTAGGRIDVIEWPLUGIN_H
#define C_GLXTAGGRIDVIEWPLUGIN_H

// CLASS DECLARATION
/**
 * @internal reviewed 12/06/2007 by Alex Birkett
 */

/**
 * CGlxTagGridViewPlugin
 * @author Ranjini Srinivasa
 */
 /*  MPX Collection view plugin definition.
 *
 *  @lib mpxcollectionviewplugin.lib
 *  @since S60 v3.0
 *  
 */
NONSHARABLE_CLASS( CGlxTagGridViewPlugin ) : public CGlxGridViewPluginBase
    {
     //Unit Testing
            
     friend class ut_CGlxTagGridViewPlugin;
public:
    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CGlxTagGridViewPlugin* NewL();
 
    /**
     * Destructor.
     */
    virtual ~CGlxTagGridViewPlugin();

private:
	   //From CGlxGridViewPluginBase	
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
    CGlxTagGridViewPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();     
    };

#endif  // C_GLXTESTGRIDVIEWPLUGIN_H

// End of File

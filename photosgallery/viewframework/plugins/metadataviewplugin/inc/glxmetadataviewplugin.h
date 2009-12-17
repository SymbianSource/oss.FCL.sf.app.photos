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
*  Description : Metadata view plugin definition.
*
*/


#ifndef C_GLXMETADATAVIEWPLUGIN_H
#define C_GLXMETADATAVIEWPLUGIN_H


// INCLUDES
#include <mpxaknviewplugin.h>

#include "glxmedialistfactory.h"

// CLASS DECLARATION

/**
 *  MPX Collection view plugin definition.
 *
 *  @lib glxunifiedmetadataviewplugin.lib
 */
NONSHARABLE_CLASS( CGlxMetadataViewPlugin ) : public CMPXAknViewPlugin 
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    static CGlxMetadataViewPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CGlxMetadataViewPlugin();
    
private:

    /**
     * From CMPXAknViewPlugin
     * Construct Avkon view.
     *
     * @return Pointer to a newly created Avkon view.
     */
    CAknView* ConstructViewLC();
    
private:

    /**
     * C++ default constructor.
     */
    CGlxMetadataViewPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_GLXMETADATAVIEWPLUGIN_H

// End of File

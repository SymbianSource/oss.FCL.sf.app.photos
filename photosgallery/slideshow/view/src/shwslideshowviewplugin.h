/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Slideshow view plugin definition.
*
*/





#ifndef C_SHWSLIDESHOWVIEWPLUGIN_H
#define C_SHWSLIDESHOWVIEWPLUGIN_H


// INCLUDES
#include <mpxaknviewplugin.h>

// CLASS DECLARATION

/**
 *  MPX Collection view plugin definition.
 *
 *  @lib mpxcollectionviewplugin.lib
 * @internal reviewed 07/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS( CShwSlideshowViewPlugin ) : public CMPXAknViewPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0 // @todo
     * @return Pointer to newly created object.
     */
    static CShwSlideshowViewPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CShwSlideshowViewPlugin();

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
    CShwSlideshowViewPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_SHWSLIDESHOWVIEWPLUGIN_H

// End of File

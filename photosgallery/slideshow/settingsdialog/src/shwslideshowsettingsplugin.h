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
* Description:    Slideshow settings dialog implementation
*
*/




#ifndef SHWSLIDESHOWSETTINGPLGIN_H
#define SHWSLIDESHOWSETTINGPLGIN_H


// INCLUDES
#include <mpxakndialogplugin.h>



// CLASS DECLARATION

/**
 *  Slideshow settings playback dialog plugin definition.
 *
 *  @lib shwslideshowsettingsplugin.lib
 *  @since S60 v3.2
 * @internal reviewed 08/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS(CShwSlideshowSettingsPlugin) : public CMPXAknDialogPlugin
    {

public:
    /**
     * Two-phased constructor.
     * @return Pointer to newly created object.
     */
    static CShwSlideshowSettingsPlugin* NewL();

    /**
     * Destructor.
     */
    ~CShwSlideshowSettingsPlugin();

private:
    /**
     * C++ default constructor.
     */
    CShwSlideshowSettingsPlugin();

private: // from CMPXAknDialogPlugin
    /**
     * Construct Avkon dialog.
     * @return Pointer to a newly created Avkon dialog.
     */
    CAknDialog* ConstructDialogL();

	// @ref CMPXAknDialogPlugin
	TInt ResourceId();
    };


#endif  // SHWSLIDESHOWSETTINGPLGIN_H

// End of File

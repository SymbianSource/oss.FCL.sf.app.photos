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
* Description:    Thumbnail storage cleanup task implementation
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef GLXTNCLEANUPTASK_H
#define GLXTNCLEANUPTASK_H

// INCLUDES

#include "glxtntask.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Background task to clean thumbnail storage of thumbnails for files which no
* longer exist.
*
* @author Dan Rhodes
* @ingroup glx_thumbnail_creator
*/
NONSHARABLE_CLASS(CGlxtnCleanupTask) : public CGlxtnTask
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aStorage Client's storage interface.
        */
        static CGlxtnCleanupTask* NewL(MGlxtnThumbnailStorage* aStorage);

        /**
        * Destructor.
        */
        virtual ~CGlxtnCleanupTask();

    private:    // From CGlxtnTask
        TBool DoStartL(TRequestStatus& aStatus);
    	void DoCancel(); 
    	TBool DoRunL(TRequestStatus& aStatus);
        TBool DoRunError(TInt aError);

    private:
        /**
        * C++ default constructor.
        * @param aStorage Client's storage interface.
        */
        CGlxtnCleanupTask(MGlxtnThumbnailStorage* aStorage);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
    };

#endif  // GLXTNCLEANUPTASK_H

// End of File

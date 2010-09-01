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
* Description:    Definition of CGlxtnDeleteThumbnailsTask
*
*/




/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef C_GLXTNDELETETHUMBNAILSTASK_H
#define C_GLXTNDELETETHUMBNAILSTASK_H

// INCLUDES

#include "glxtntask.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CGlxtnFileInfo;

// CLASS DECLARATION

/**
* Task to delete all stored thumbnails for a media item.
*
* @author Dan Rhodes
* @ingroup glx_thumbnail_creator
*/
NONSHARABLE_CLASS(CGlxtnDeleteThumbnailsTask) : public CGlxtnClientTask
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param iItemId Media ID of item whose thumbnails should be deleted.
        * @param aClient Client initiating the request.
        */
        static CGlxtnDeleteThumbnailsTask* NewL(const TGlxMediaId& aItemId,
                                        MGlxtnThumbnailCreatorClient& aClient);

        /**
        * Destructor.
        */
        virtual ~CGlxtnDeleteThumbnailsTask();

    protected:  // From CGlxtnTask

        TBool DoStartL(TRequestStatus& aStatus);
    	void DoCancel(); 
    	TBool DoRunL(TRequestStatus& aStatus);
        TBool DoRunError(TInt aError);

    private:

        /**
        * C++ default constructor.
        * @param iItemId Media ID of item whose thumbnails should be deleted.
        * @param aClient Client initiating the request.
        */
        CGlxtnDeleteThumbnailsTask(const TGlxMediaId& aItemId,
                                    MGlxtnThumbnailCreatorClient& aClient);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /** File information for item whose thumbnails are to be deleted */
        CGlxtnFileInfo* iInfo;
        /** Whether delete operation has been started */
        TBool iDeleting;
    };

#endif  // C_GLXTNDELETETHUMBNAILSTASK_H

// End of File

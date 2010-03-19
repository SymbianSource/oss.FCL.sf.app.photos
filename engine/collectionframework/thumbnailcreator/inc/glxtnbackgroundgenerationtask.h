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
* Description:   Thumbnail background generation task implementation
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

#ifndef GLXTNBACKGROUNDGENERATIONTASK_H
#define GLXTNBACKGROUNDGENERATIONTASK_H

// INCLUDES

#include "glxtnsavethumbnailtask.h"
#include <glxsettingsmodel.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CGlxtnFileUtility;
class CGlxtnImageUtility;
class CGlxtnVideoUtility;

// CLASS DECLARATION

/**
* Background task to generate a thumbnail.
*
* @author Dan Rhodes
* @ingroup glx_thumbnail_creator
*/
NONSHARABLE_CLASS(CGlxtnBackgroundGenerationTask) : public CGlxtnSaveThumbnailTask
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param iItemId Media ID of item to thumbnail.
        * @param aFileUtility File utility for use by the task.
        * @param aClient Client initiating the request.
        */
        static CGlxtnBackgroundGenerationTask* NewL(
                    const TGlxMediaId& iItemId, CGlxtnFileUtility& aFileUtility,
                    MGlxtnThumbnailCreatorClient& aClient, 
                    CGlxSettingsModel::TSupportedOrientations aSupportedOrientations);

        /**
        * Destructor.
        */
        virtual ~CGlxtnBackgroundGenerationTask();

    private:    // From CGlxtnTask
        TBool DoStartL(TRequestStatus& aStatus);
    	void DoCancel(); 
    	TBool DoRunL(TRequestStatus& aStatus);
        TBool DoRunError(TInt aError);

    private:
        /**
        * C++ default constructor.
        * @param iItemId Media ID of item to thumbnail.
        * @param aFileUtility File utility for use by the task.
        * @param aClient Client initiating the request.
        */
        CGlxtnBackgroundGenerationTask(const TGlxMediaId& iItemId,
                                    CGlxtnFileUtility& aFileUtility,
                                    MGlxtnThumbnailCreatorClient& aClient, 
                    CGlxSettingsModel::TSupportedOrientations aSupportedOrientations);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Check whether next size already exists or needs to be generated.
        * @param aStatus Request status for the asynchronous operation.
        * @return ETrue if the task has issued an asyncronous request.
        */
        TBool CheckNextL(TRequestStatus& aStatus);

        /**
        * Create the next size to be generated.
        * @param aStatus Request status for the asynchronous operation.
        * @return ETrue if the task has issued an asyncronous request.
        */
        TBool ProcessNextL(TRequestStatus& aStatus);

        /**
        * Filter the thumbnail for the next required size.
        * @param aStatus Request status for the asynchronous operation.
        */
        void FilterThumbnailL(TRequestStatus& aStatus);

        /**
        * Scale the thumbnail to the next required size.
        * @param aStatus Request status for the asynchronous operation.
        */
        void ScaleThumbnailL(TRequestStatus& aStatus);

        /**
        * Decode the image for the required size.
        * @param aStatus Request status for the asynchronous operation.
        */
        void DecodeImageL(TRequestStatus& aStatus);

    private:    // Data
        /** Pointer to client, or NULL after client goes away */
        MGlxtnThumbnailCreatorClient* iClient;
        /** File utility */
        CGlxtnFileUtility& iFileUtility;
        /** Array of sizes to check if need to generate */
        RArray<TSize> iSizes;
        /** Array of sizes to generate */
        RArray<TSize> iTodoSizes;
        /** Utility to get thumbnail from image */
        CGlxtnImageUtility* iImageUtility;
        /** Utility to get thumbnail from video */
        CGlxtnVideoUtility* iVideoUtility;
        /** Image loaded from file */
        CFbsBitmap* iImage;
       /** Image loaded from file */
        CFbsBitmap* iFilteredImage;
        /** Flag to show BadFileMarker not created so no need to delete **/
        TBool iBadFileMarkerNotNeededFlag;
        /** Flag to show whether image has been decoded yet **/
        TBool iImageDecoded;
        /** Flag to show which orientations are supported **/
        CGlxSettingsModel::TSupportedOrientations iSupportedOrientations;
        
    };

#endif  // GLXTNBACKGROUNDGENERATIONTASK_H

// End of File

/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Cropped thumbnail generation task unit tests.
*
*/



#ifndef __UT_CGLXTNZOOMEDIMAGETASK_H__
#define __UT_CGLXTNZOOMEDIMAGETASK_H__

//  EXTERNAL INCLUDES

#include <CEUnitTestSuiteClass.h>
#include <f32file.h>

//  INTERNAL INCLUDES

#include "mglxtnthumbnailcreatorclient.h"

//  FORWARD DECLARATIONS

class CGlxtnFileUtility;
class CGlxtnZoomedImageTask;
class CFbsBitmap;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CGlxtnZoomedImageTask )
     : public CEUnitTestSuiteClass, public MGlxtnThumbnailCreatorClient
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxtnZoomedImageTask* NewL();
        static UT_CGlxtnZoomedImageTask* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxtnZoomedImageTask();

    private:    // Constructors and destructors

        UT_CGlxtnZoomedImageTask();
        void ConstructL();

    private:    // From MGlxtnThumbnailCreatorClient
        void ThumbnailFetchComplete(const TGlxMediaId& aItemId,
                            TGlxThumbnailQuality aQuality, TInt aErrorCode);
        void ThumbnailDeletionComplete(const TGlxMediaId& aItemId, TInt aErrorCode);
        void FilterAvailableComplete( const RArray<TGlxMediaId>& aIdArray,
                                        TInt aErrorCode );
        void FetchFileInfoL(CGlxtnFileInfo* aInfo, const TGlxMediaId& aItemId,
                        TRequestStatus* aStatus);
        void CancelFetchUri(const TGlxMediaId& aItemId);
        MGlxtnThumbnailStorage* ThumbnailStorage();

    private:    // New methods

        void SetupL();
        void Teardown();

        void UT_CGlxtnZoomedImageTask_ConstructL();
        void UT_CGlxtnZoomedImageTask_GenerateL();

        void DoGenerateL(const TDesC& aUri);

    private:    // Data

        CGlxtnFileUtility* iFileUtility;
        CGlxtnZoomedImageTask* iZoomedImageTask;
        CFbsBitmap* iBitmap;
        TPtrC iUri;
    	TGlxThumbnailQuality iQuality;
    	TInt iError;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif  // __UT_CGLXTNZOOMEDIMAGETASK_H__

// End of file

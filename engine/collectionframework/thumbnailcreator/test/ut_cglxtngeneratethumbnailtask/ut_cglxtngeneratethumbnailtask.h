/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Thumbnail generation task unit tests.
*
*/



#ifndef __UT_CGLXTNGENERATETHUMBNAILTASK_H__
#define __UT_CGLXTNGENERATETHUMBNAILTASK_H__

//  EXTERNAL INCLUDES

#include <CEUnitTestSuiteClass.h>
#include <f32file.h>

//  INTERNAL INCLUDES

#include "mglxtnthumbnailcreatorclient.h"

//  FORWARD DECLARATIONS

class CGlxtnFileUtility;
class CGlxtnGenerateThumbnailTask;
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
NONSHARABLE_CLASS( UT_CGlxtnGenerateThumbnailTask )
     : public CEUnitTestSuiteClass, public MGlxtnThumbnailCreatorClient
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxtnGenerateThumbnailTask* NewL();
        static UT_CGlxtnGenerateThumbnailTask* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxtnGenerateThumbnailTask();

    private:    // Constructors and destructors

        UT_CGlxtnGenerateThumbnailTask();
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
        
         void UT_CGlxtnGenerateThumbnailTask_ConstructL();
         void UT_CGlxtnGenerateThumbnailTask_GenerateL();
         void UT_CGlxtnGenerateThumbnailTask_GenerateTallL();
         void UT_CGlxtnGenerateThumbnailTask_GenerateWideL();
         void UT_CGlxtnGenerateThumbnailTask_ErrorCorruptFileL();
         void UT_CGlxtnGenerateThumbnailTask_ErrorFileNotFoundL();

        void DoGenerateL(const TDesC& aUri);

    private:    // Data

        CGlxtnFileUtility* iFileUtility;
        CGlxtnGenerateThumbnailTask* iGenerateTask;
        CFbsBitmap* iBitmap;
        TPtrC iUri;
    	TSize iSizeClass;
    	TGlxThumbnailQuality iQuality;
    	TInt iError;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CGLXTNGENERATETHUMBNAILTASK_H__

// End of file

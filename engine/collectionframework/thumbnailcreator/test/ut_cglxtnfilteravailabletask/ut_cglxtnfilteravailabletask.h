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
* Description:   Thumbnail availability filter task unit tests.
*
*/



#ifndef __UT_CGLXTNFILTERAVAILABLETASK_H__
#define __UT_CGLXTNFILTERAVAILABLETASK_H__

//  EXTERNAL INCLUDES

#include <CEUnitTestSuiteClass.h>
#include <f32file.h>

//  INTERNAL INCLUDES

#include "mglxtnstorage.h"
#include "mglxtnthumbnailcreatorclient.h"

//  FORWARD DECLARATIONS

class CGlxtnFilterAvailableTask;

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
NONSHARABLE_CLASS( UT_CGlxtnFilterAvailableTask )
     : public CEUnitTestSuiteClass, public MGlxtnThumbnailCreatorClient,
        public MGlxtnThumbnailStorage
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxtnFilterAvailableTask* NewL();
        static UT_CGlxtnFilterAvailableTask* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxtnFilterAvailableTask();

    private:    // Constructors and destructors

        UT_CGlxtnFilterAvailableTask();
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

    public: // Functions from MGlxtnThumbnailStorage
        void LoadThumbnailDataL(HBufC8*& aData, TGlxImageDataFormat& aFormat,
                    const TGlxMediaId& aId, const CGlxtnFileInfo& aFileInfo,
                    const TSize& aSize, TRequestStatus* aStatus);
        void SaveThumbnailDataL(const TDesC8& aData,
                    TGlxImageDataFormat aFormat, const TGlxMediaId& aId,
                    const CGlxtnFileInfo& aFileInfo, const TSize& aSize,
                    TRequestStatus* aStatus);
        void DeleteThumbnailsL(const TGlxMediaId& aId,
                    const CGlxtnFileInfo& aFileInfo, TRequestStatus* aStatus);
        void CleanupThumbnailsL(TRequestStatus* aStatus);
        void IsThumbnailAvailableL(const TGlxMediaId& aId,
                    const CGlxtnFileInfo& aFileInfo,
                    const TSize& aSize, TRequestStatus* aStatus);
        void StorageCancel();
        void NotifyBackgroundError(const TGlxMediaId& aId, TInt aError);

    private:    // New methods

        void SetupL();
        void Teardown();

        void UT_CGlxtnFilterAvailableTask_ConstructL();
        void UT_CGlxtnFilterAvailableTask_FilterL();

    private:    // Data

        CGlxtnFilterAvailableTask* iFilterAvailableTask;
    	TSize iSize;
    	TInt iError;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif  //  __UT_CGLXTNFILTERAVAILABLETASK_H__

// End of file

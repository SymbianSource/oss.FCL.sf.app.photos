/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Delete thumbnails task unit tests.
*
*/



#ifndef __UT_CGLXTNDELETETHUMBNAILSTASK_H__
#define __UT_CGLXTNDELETETHUMBNAILSTASK_H__

//  EXTERNAL INCLUDES

#include <CEUnitTestSuiteClass.h>
#include <f32file.h>

//  INTERNAL INCLUDES

#include "mglxtnstorage.h"
#include "mglxtnthumbnailcreatorclient.h"

//  FORWARD DECLARATIONS

class CGlxtnDeleteThumbnailsTask;

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
NONSHARABLE_CLASS( UT_CGlxtnDeleteThumbnailsTask )
     : public CEUnitTestSuiteClass, public MGlxtnThumbnailCreatorClient,
        public MGlxtnThumbnailStorage
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxtnDeleteThumbnailsTask* NewL();
        static UT_CGlxtnDeleteThumbnailsTask* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxtnDeleteThumbnailsTask();

    private:    // Constructors and destructors

        UT_CGlxtnDeleteThumbnailsTask();
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

        void UT_CGlxtnDeleteThumbnailsTask_ConstructL();
        void UT_CGlxtnDeleteThumbnailsTask_DeleteL();

    private:    // Data

        CGlxtnDeleteThumbnailsTask* iDeleteThumbnailsTask;
    	TUint iIdDeleted;
    	TInt iError;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif  //  __UT_CGLXTNDELETETHUMBNAILSTASK_H__

// End of file

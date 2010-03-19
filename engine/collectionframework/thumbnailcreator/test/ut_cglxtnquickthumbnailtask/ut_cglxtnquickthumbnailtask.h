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
* Description:   Quick thumbnail generation task unit tests.
*
*/



#ifndef __UT_CGLXTNQUICKTHUMBNAILTASK_H__
#define __UT_CGLXTNQUICKTHUMBNAILTASK_H__

//  EXTERNAL INCLUDES

#include <CEUnitTestSuiteClass.h>
#include <f32file.h>

//  INTERNAL INCLUDES

#include "mglxtnthumbnailcreatorclient.h"

//  FORWARD DECLARATIONS

class CGlxtnFileUtility;
class CGlxtnQuickThumbnailTask;
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
NONSHARABLE_CLASS( UT_CGlxtnQuickThumbnailTask )
     : public CEUnitTestSuiteClass, public MGlxtnThumbnailCreatorClient
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CGlxtnQuickThumbnailTask* NewL();
        static UT_CGlxtnQuickThumbnailTask* NewLC();
        /**
         * Destructor
         */
        ~UT_CGlxtnQuickThumbnailTask();

    private:    // Constructors and destructors

        UT_CGlxtnQuickThumbnailTask();
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
        
        void UT_CGlxtnQuickThumbnailTask_ConstructL();
        void UT_CGlxtnQuickThumbnailTask_GenerateL();
        void UT_CGlxtnQuickThumbnailTask_GenerateNoExifL();
        void UT_CGlxtnQuickThumbnailTask_ErrorCorruptFileL();
        void UT_CGlxtnQuickThumbnailTask_ErrorFileNotFoundL();

        void DoGenerateL(const TDesC& aUri);

    private:    // Data

        CGlxtnFileUtility* iFileUtility;
        CGlxtnQuickThumbnailTask* iQuickThumbnailTask;
        CFbsBitmap* iBitmap;
        TPtrC iUri;
    	TSize iSizeClass;
    	TGlxThumbnailQuality iQuality;
    	TInt iError;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CGLXTNQUICKTHUMBNAILTASK_H__

// End of file

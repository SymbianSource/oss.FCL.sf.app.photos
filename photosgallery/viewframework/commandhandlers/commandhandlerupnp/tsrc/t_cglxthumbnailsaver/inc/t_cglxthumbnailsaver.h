/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Thumbnail saver for CommandHandler UPnP unit test case
*
*/




#ifndef __T_CGLXTHUMBNAILSAVER_H__
#define __T_CGLXTHUMBNAILSAVER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>

//  INTERNAL INCLUDES
#include "glxupnpthumbnailsaver.h"

//  FORWARD DECLARATIONS
class CGlxThumbnailSaver;

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( t_cglxthumbnailsaver )
	: public CEUnitTestSuiteClass,public MGlxThumbnailSaveComplete
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static t_cglxthumbnailsaver* NewL();
        static t_cglxthumbnailsaver* NewLC();
        /**
         * Destructor
         */
        ~t_cglxthumbnailsaver();

    private:    // Constructors and destructors

        t_cglxthumbnailsaver();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_TestL();
         
         void T_CreateVideoIconL();
         void T_CreateVideoIconL_Cancel();
         
         void T_CreateDefaultVideoIconL();
         
         void SetupL_FillMemory();
         
         
         //From MGlxThumbnailSaveComplete
         
         void HandleFileSaveCompleteL(const TDesC& aPath);
        

    private:    // Data
		

        EUNIT_DECLARE_TEST_TABLE; 
        
        CGlxThumbnailSaver* iThumbnailSaver;
        
        CFbsBitmap* iThumbnail; 
        

    };

#endif      //  __T_CGLXTHUMBNAILSAVER_H__

// End of file

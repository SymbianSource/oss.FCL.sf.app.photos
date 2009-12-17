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
* Description:    Show commmand handler unit test
*
*/




#ifndef __T_CGlxCommandHandlerFilterImagesOrVideos_H__
#define __T_CGlxCommandHandlerFilterImagesOrVideos_H__

/**
 * @internal reviewed 08/02/2008 by Rhodri Byles
 */

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include <mglxmedialistprovider.h>
#include <EIKMOBS.H>
#include <tmglxmedialist_stub.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxCommandHandlerFilterImagesOrVideos;

//  CLASS DEFINITION
/**
 * EUnit test suite for CGlxCommandHandlerFilterImagesOrVideos
 *
 */
NONSHARABLE_CLASS( T_CGlxCommandHandlerFilterImagesOrVideos )
	: public CEUnitTestSuiteClass,
	  public MEikMenuObserver, 
	  public MGlxMediaList_Stub_Observer,
	  public MGlxMediaListProvider
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxCommandHandlerFilterImagesOrVideos* NewL();
        static T_CGlxCommandHandlerFilterImagesOrVideos* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxCommandHandlerFilterImagesOrVideos();

    private:    // From MGlxMediaList_Stub_Observer
        void MGlxMediaList_MethodCalled( TMGlxMediaListMethodId aMethodId );
        
    private:    // From MGlxMediaListProvider

        MGlxMediaList& MediaList();
        
    private:    // Constructors and destructors

        T_CGlxCommandHandlerFilterImagesOrVideos();
        void ConstructL();

    private:    // From MEikMenuObserver
	    void SetEmphasis(CCoeControl* aMenuControl,TBool aEmphasis);
    	void ProcessCommandL(TInt aCommandId);

    private:    // New methods
    
         void SetupL();
        
         void Teardown();

         void TestConstructionL(  );
         
         // helper methods to check the filter
         void TestForImages();
         void TestForVideos();
         void TestForAll();

         
    private: // Test methods

         void TestSetAllFromNoneL();
         void TestSetImagesFromNoneL();
         void TestSetVideosFromNoneL();
         void TestSetAllFromImagesL();
         void TestSetVideosFromImagesL();
         void TestSetAllFromVideosL();
         void TestSetImagesFromVideosL();
         void TestSetVideosFromAllL();
         void TestSetImagesFromAllL();
         void TestNavigateForwards();
         void TestNavigateBackwards();
         
    private:    // Data
		
        CGlxCommandHandlerFilterImagesOrVideos* iCommandHandlerShowItems;
        
        TMGlxMediaList_Stub*  iStubMediaList;
        
        TMGlxMediaListMethodId iMethodId;

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CGlxCommandHandlerFilterImagesOrVideos_H__

// End of file

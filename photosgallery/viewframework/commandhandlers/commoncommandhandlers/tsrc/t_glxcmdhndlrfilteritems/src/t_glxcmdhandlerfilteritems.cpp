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




/**
 * @internal reviewed 08/02/2008 by Rhodri Byles
 */
 
//  CLASS HEADER
#include "t_glxcmdhandlerfilteritems.h"

//  EXTERNAL INCLUDES
#include <avkon.hrh>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxmediageneraldefs.h>	// For Attribute constants
#include <glxcommandhandlers.hrh>
#include <mglxmedialist.h>
//  INTERNAL INCLUDES
#include "glxcommandhandlerfilterimagesorvideos.h"
#include "glxhuiutility_stub.h"

// CONSTRUCTION
T_CGlxCommandHandlerFilterImagesOrVideos* T_CGlxCommandHandlerFilterImagesOrVideos::NewL()
    {
    T_CGlxCommandHandlerFilterImagesOrVideos* self = T_CGlxCommandHandlerFilterImagesOrVideos::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

T_CGlxCommandHandlerFilterImagesOrVideos* T_CGlxCommandHandlerFilterImagesOrVideos::NewLC()
    {
    T_CGlxCommandHandlerFilterImagesOrVideos* self = new( ELeave ) T_CGlxCommandHandlerFilterImagesOrVideos();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
T_CGlxCommandHandlerFilterImagesOrVideos::~T_CGlxCommandHandlerFilterImagesOrVideos()
    {
    }

// Default constructor
T_CGlxCommandHandlerFilterImagesOrVideos::T_CGlxCommandHandlerFilterImagesOrVideos()
    {
    }

// Second phase construct
void T_CGlxCommandHandlerFilterImagesOrVideos::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::MGlxMediaList_MethodCalled(TMGlxMediaListMethodId aMethodId)
    {
    iMethodId = aMethodId;
    }

MGlxMediaList& T_CGlxCommandHandlerFilterImagesOrVideos::MediaList()
    {
    return *iStubMediaList;
    }

   
//  METHODS
void T_CGlxCommandHandlerFilterImagesOrVideos::SetupL(  )
    {
    iStubMediaList = new (ELeave)TMGlxMediaList_Stub(this);
    iCommandHandlerShowItems = CGlxCommandHandlerFilterImagesOrVideos::NewL( this );
    }


void T_CGlxCommandHandlerFilterImagesOrVideos::Teardown(  )
    {
    delete iCommandHandlerShowItems;
	iCommandHandlerShowItems = NULL:
    delete iStubMediaList;
	iStubMediaList = NULL;
    }

 // From MEikMenuObserver
void T_CGlxCommandHandlerFilterImagesOrVideos::SetEmphasis(
    CCoeControl* /* aMenuControl */,TBool /* aEmphasis */)
    {
    
    }
 // From MEikMenuObserver
void T_CGlxCommandHandlerFilterImagesOrVideos::ProcessCommandL(
    TInt /* aCommandId */)
    {
    }

void T_CGlxCommandHandlerFilterImagesOrVideos::TestConstructionL(  )
    {
    // Only testing for leaks during setup/teardown here
    EUNIT_ASSERT(ETrue);
    }
    
// 
void T_CGlxCommandHandlerFilterImagesOrVideos::TestForImages()
    {
    // Test for images
    CMPXFilter* filter = iStubMediaList->Filter();
    EUNIT_ASSERT_DESC( filter, "Filter is added to the medialist" );
    EUNIT_ASSERT_DESC( filter->IsSupported( KGlxFilterGeneralItemType ), "filter is KGlxFilterGeneralItemType" );
    TGlxFilterItemType itemType = *(filter->Value<TGlxFilterItemType>(KGlxFilterGeneralItemType));
    EUNIT_ASSERT_DESC( itemType == EGlxFilterImage, "Filter is EGlxFilterImage" );
    }

void T_CGlxCommandHandlerFilterImagesOrVideos::TestForVideos()
    {
    // Test for videos
    CMPXFilter* filter = iStubMediaList->Filter();
    EUNIT_ASSERT(filter);
    EUNIT_ASSERT(filter->IsSupported(KGlxFilterGeneralItemType));
    TGlxFilterItemType itemType = *(filter->Value<TGlxFilterItemType>(KGlxFilterGeneralItemType));
    EUNIT_ASSERT(itemType == EGlxFilterVideo);
    }

void T_CGlxCommandHandlerFilterImagesOrVideos::TestForAll()
    {
    // Test for videos
    CMPXFilter* filter = iStubMediaList->Filter();
    EUNIT_ASSERT(filter);
    EUNIT_ASSERT(filter->IsSupported(KGlxFilterGeneralItemType));
    TGlxFilterItemType itemType = *(filter->Value<TGlxFilterItemType>(KGlxFilterGeneralItemType));
    EUNIT_ASSERT(itemType == EGlxFilterVideoAndImages);
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetAllFromNoneL(  )
    {
    CMPXFilter* filter = NULL;
    iStubMediaList->RemoveFilter();

    // Test for All when no filter is currently set
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowAll);
    filter = iStubMediaList->Filter();
    // As no filter was set previously and EGlxCmdShowAll was requested no filter should be set now.
    EUNIT_ASSERT( !filter );
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetImagesFromNoneL(  )
    {
    iStubMediaList->RemoveFilter();

    // Test for All when no filter is currently set
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    TestForImages();
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetVideosFromNoneL(  )
    {
    iStubMediaList->RemoveFilter();

    // Test for Videos when no filter is currently set
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowVideos);
    TestForVideos();
    }

void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetAllFromImagesL(  )
    {
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowAll);
    TestForAll();
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetVideosFromImagesL(  )
    {
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowVideos);
    TestForVideos();
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetAllFromVideosL(  )
    {
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowVideos);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowAll);
    TestForAll();
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetImagesFromVideosL(  )
    {
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowVideos);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    TestForImages();
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetVideosFromAllL(  )
    {
    // The only way to have an All Filter set is to set ALL with
    // either an Images or Videos filter set.
    // Set one create an Image filter
    // Then set the All Filter
    // Then set the Videos filter
    
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowAll);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowVideos);
    TestForVideos();
    }
    
void T_CGlxCommandHandlerFilterImagesOrVideos::TestSetImagesFromAllL(  )
    {
    // The only way to have an All Filter set is to set ALL with
    // either an Images or Videos filter set.
    // Set one create an Video filter
    // Then set the All Filter
    // Then set the images filter
    
    CMPXFilter* filter = NULL;
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowVideos);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowAll);
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    TestForImages();
    }

 void T_CGlxCommandHandlerFilterImagesOrVideos::TestNavigateForwards( )
    {
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    TestForImages();
    // Navigating forwards should reset filter to All
    g_NavigationDirection = EGlxNavigationForwards;
    // upcast to get the public ActivateL
    static_cast<CGlxMediaListCommandHandler*>( 
        iCommandHandlerShowItems )->DoActivateL( 1 ); // Dummy view id
    TestForAll();
    }

 void T_CGlxCommandHandlerFilterImagesOrVideos::TestNavigateBackwards()
    {
    iStubMediaList->RemoveFilter();
    iCommandHandlerShowItems->ExecuteL(EGlxCmdShowImages);
    TestForImages();
    // Navigating backwards should not change the filter
    g_NavigationDirection = EGlxNavigationBackwards;
    // upcast to get the public ActivateL
    static_cast<CGlxMediaListCommandHandler*>( 
        iCommandHandlerShowItems )->DoActivateL( 1 ); // Dummy view id
    TestForImages();
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_CGlxCommandHandlerFilterImagesOrVideos,
    "Unit test suite for CGlxCommandHandlerFilterImagesOrVideos",
    "UNIT" )

EUNIT_TEST(
    "Test Construction",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestConstructionL, Teardown)

EUNIT_TEST(
    "Set All from none",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetAllFromNoneL, Teardown)

EUNIT_TEST(
    "Set Images from none",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetImagesFromNoneL, Teardown)

EUNIT_TEST(
    "Set Videos from none",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetVideosFromNoneL, Teardown)

EUNIT_TEST(
    "Set All from Images",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetAllFromImagesL, Teardown)

EUNIT_TEST(
    "Set Video from Images",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetVideosFromImagesL, Teardown)
    
EUNIT_TEST(
    "Set All from Videos",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetAllFromVideosL, Teardown)
    
EUNIT_TEST(
    "Set Images from Videos",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetImagesFromVideosL, Teardown)
    
EUNIT_TEST(
    "Set Videos from All",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetVideosFromAllL, Teardown)
    
EUNIT_TEST(
    "Set Images from All",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestSetImagesFromAllL, Teardown)
    
EUNIT_TEST(
    "Navigate Forwards",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestNavigateForwards, Teardown)
    
EUNIT_TEST(
    "Navigate Backwards",
    "CGlxCommandHandlerFilterImagesOrVideos",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestNavigateBackwards, Teardown)
    
    
EUNIT_END_TEST_TABLE

//  END OF FILE

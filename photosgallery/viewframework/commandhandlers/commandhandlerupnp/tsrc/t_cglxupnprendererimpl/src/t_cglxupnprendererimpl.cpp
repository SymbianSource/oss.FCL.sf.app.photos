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
* Description:  CommandHandlerUPnP RENDERER unit test case
*
*/




//  CLASS HEADER
#include "t_cglxupnprendererimpl.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
#ifdef FF_UPNP_FRAMEWORK_2_0
#include <upnpshowcommand.h>
#include <upnpcommandobserver.h>
#endif


//  INTERNAL INCLUDES
#include "glxupnprendererimpl.h"

// CONSTRUCTION
t_cglxupnprendererimpl* t_cglxupnprendererimpl::NewL()
    {
    t_cglxupnprendererimpl* self = t_cglxupnprendererimpl::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

t_cglxupnprendererimpl* t_cglxupnprendererimpl::NewLC()
    {
    t_cglxupnprendererimpl* self = new( ELeave ) t_cglxupnprendererimpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
t_cglxupnprendererimpl::~t_cglxupnprendererimpl()
    {
    }

// Default constructor
t_cglxupnprendererimpl::t_cglxupnprendererimpl()
    {
    }

// Second phase construct
void t_cglxupnprendererimpl::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void t_cglxupnprendererimpl::SetupL(  )
    {
    iUpnpRendererImpl = CGlxUpnpRendererImpl:: NewL();
    }
    

void t_cglxupnprendererimpl::Teardown(  )
    {
    delete iUpnpRendererImpl;
	iUpnpRendererImpl = NULL;
    }
    

void t_cglxupnprendererimpl::T_TestL(  )
    {
    EUNIT_ASSERT_DESC( iUpnpRendererImpl, "Failed to create");
    }
    
void t_cglxupnprendererimpl::T_IsSupported_Available(  )
    {
	#ifdef FF_UPNP_FRAMEWORK_2_0
    //Set the upnpShowcommand as available
    CUpnpShowCommand::SetUpIsAvailable(ETrue); 
    #endif
    EUNIT_ASSERT_EQUALS(ETrue,CGlxUpnpRendererImpl::IsSupported());
    }
    
void t_cglxupnprendererimpl::T_IsSupported_NotAvailable(  )
    {
    #ifdef FF_UPNP_FRAMEWORK_2_0
    //Set the upnpShowcommand as not available
    CUpnpShowCommand::SetUpIsAvailable(EFalse); 
	#endif
    EUNIT_ASSERT_EQUALS(EFalse,CGlxUpnpRendererImpl::IsSupported());
    }    
    
void t_cglxupnprendererimpl::T_TestL1(  )
    {
    
    }
        
void t_cglxupnprendererimpl::T_HandleAttributesAvailable()
    {
    iMediaListStub = new(ELeave) TMGlxMediaList_Stub( this );
    
    //iMediaListStub->AddMediaListObserverL(this); iUpnpRendererImpl
    iMediaListStub->AddMediaListObserverL(iUpnpRendererImpl); 
    
    iMediaListStub->iCount = 10;
    iMediaListStub->iFocus = 1;
    
    iMediaListStub->NotifyAttributesAvailableL(iMediaListStub->iFocus);
    } 
    
        
void t_cglxupnprendererimpl::MGlxMediaList_MethodCalled(TMGlxMediaListMethodId /*aMethodId*/)
    {
    //Do Nothing
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_cglxupnprendererimpl,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Creation of Impl",
    "CGlxUpnpRendererImpl",
    "NewL",
    "UNIT",
    SetupL, T_TestL, Teardown)
    
EUNIT_TEST(
    "Available",
    "CGlxUpnpRendererImpl",
    "IsSupported",
    "UNIT",
    T_IsSupported_Available, T_TestL1, T_TestL1)    
    
EUNIT_TEST(
    "Not-Available",
    "CGlxUpnpRendererImpl",
    "IsSupported",
    "UNIT",
    T_IsSupported_NotAvailable, T_TestL1,T_TestL1)  
    
/*EUNIT_TEST(
    "HandleAttributes Available",
    "CGlxUpnpRendererImpl",
    "HandleAttributesAvailable",
    "UNIT",
    SetupL,T_HandleAttributesAvailable , Teardown)*/
EUNIT_END_TEST_TABLE

//  END OF FILE


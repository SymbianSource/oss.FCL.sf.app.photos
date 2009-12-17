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
* Description:  CommandHandlerUpNP unit test cases for upnp renderer
*
*/




//  CLASS HEADER
#include "t_glxupnprenderer.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>

//  INTERNAL INCLUDES
#include "glxupnprenderer.h"
#include <upnpshowcommand.h>

// CONSTRUCTION
t_glxupnprenderer* t_glxupnprenderer::NewL()
    {
    t_glxupnprenderer* self = t_glxupnprenderer::NewLC();
    CleanupStack::Pop();

    return self;
    }

t_glxupnprenderer* t_glxupnprenderer::NewLC()
    {
    t_glxupnprenderer* self = new( ELeave ) t_glxupnprenderer();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
t_glxupnprenderer::~t_glxupnprenderer()
    {
    }

// Default constructor
t_glxupnprenderer::t_glxupnprenderer()
    {
    }

// Second phase construct
void t_glxupnprenderer::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void t_glxupnprenderer::T_StartShowingL(  )
    {
    GlxUpnpRenderer::StartShowingL();
    }
    

void t_glxupnprenderer::T_StopShowingL(  )
    {
    GlxUpnpRenderer::StopShowingL();
    }
    

void t_glxupnprenderer::T_TestL(  )
    {
    
    }
    
void t_glxupnprenderer::T_Status_Active(  )
    {
    //The pointer is created so it is active    
    T_StartShowingL();        
    EUNIT_ASSERT_EQUALS(NGlxUpnpRenderer::EActive,GlxUpnpRenderer::Status())
    T_StopShowingL();     
    }
    
 void t_glxupnprenderer::T_Status_Active_Renderer(  )
    {
    //UPNPShowComamndAvailable as Available
    T_StartShowingL();    
    CUpnpShowCommand::SetUpIsAvailable(ETrue);    
    //If it is available and the pointer is created then EActive
    EUNIT_ASSERT_EQUALS(NGlxUpnpRenderer::EActive,GlxUpnpRenderer::Status())    
    T_StopShowingL();    
    }
        
 
 void t_glxupnprenderer::T_Status_AvailableNotActive(  )
    {
    //UPNPShowComamndAvailable as Available
    CUpnpShowCommand::SetUpIsAvailable(ETrue);    
    //If it is available and the pointer is not created then EAvailableNotActive
    EUNIT_ASSERT_EQUALS(NGlxUpnpRenderer::EAvailableNotActive,GlxUpnpRenderer::Status())    
    }
    

void t_glxupnprenderer::T_Status_NotAvailable(  )
    {    
    //UPNPShowComamndAvailable not available
    CUpnpShowCommand::SetUpIsAvailable(EFalse);     
    EUNIT_ASSERT_EQUALS(NGlxUpnpRenderer::ENotAvailable,GlxUpnpRenderer::Status())       
    }

    
void t_glxupnprenderer::T_ShowVideoL(  )
    {
    GlxUpnpRenderer::ShowVideoL();
    }
    
    
void t_glxupnprenderer::TearDown(  )  
    {
    
    }
        
//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_glxupnprenderer,
    "Testing the GlxUpnpRenderer.",
    "UNIT" )

EUNIT_TEST(
    "Creation of Impl",
    "GlxUpnpRenderer",
    "StartShowingL",
    "UNIT",
    T_StartShowingL, T_TestL, T_StopShowingL)
    
EUNIT_TEST(
    "Status-Active",
    "GlxUpnpRenderer",
    "Status",
    "UNIT",
    T_Status_Active, T_TestL, TearDown)
    
EUNIT_TEST(
    "Status-Active-Renderer",
    "GlxUpnpRenderer",
    "Status",
    "UNIT",
    T_Status_Active_Renderer, T_TestL, TearDown)    
    
EUNIT_TEST(
    "Status-AvailableNotActive",
    "GlxUpnpRenderer",
    "Status",
    "UNIT",
    T_Status_AvailableNotActive, T_TestL, TearDown)
    
EUNIT_TEST(
    "Status-NotAvailable",
    "GlxUpnpRenderer",
    "Status",
    "UNIT",
    T_Status_NotAvailable, T_TestL, TearDown) 

    
EUNIT_TEST(
    "ShowVideo",
    "GlxUpnpRenderer",
    "ShowVideoL",
    "UNIT",
    T_ShowVideoL,T_TestL,TearDown)    
 

EUNIT_END_TEST_TABLE

//  END OF FILE

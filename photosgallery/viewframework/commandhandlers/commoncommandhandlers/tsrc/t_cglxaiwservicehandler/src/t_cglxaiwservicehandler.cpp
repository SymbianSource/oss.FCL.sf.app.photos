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
* Description:  AIW Service Handler stub implementation.
*
*/




//  CLASS HEADER
#include "t_CGlxAiwServiceHandler.h"

//  EXTERNAL INCLUDES
#include <avkon.hrh>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxmediageneraldefs.h>	// For Attribute constants
#include <glxcommandhandlers.hrh>
#include <mglxmedialist.h>
#include <AiwGenericParam.hrh>
#include <AiwGenericParam.h>
#include <aiwvariant.h>
//  INTERNAL INCLUDES
#include "glxaiwservicehandler.h"

// CONSTRUCTION
t_CGlxAiwServiceHandler* t_CGlxAiwServiceHandler::NewL()
    {
    t_CGlxAiwServiceHandler* self = t_CGlxAiwServiceHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

t_CGlxAiwServiceHandler* t_CGlxAiwServiceHandler::NewLC()
    {
    t_CGlxAiwServiceHandler* self = new( ELeave ) t_CGlxAiwServiceHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
t_CGlxAiwServiceHandler::~t_CGlxAiwServiceHandler()
    {
    }

// Default constructor
t_CGlxAiwServiceHandler::t_CGlxAiwServiceHandler()
    {
    }

// Second phase construct
void t_CGlxAiwServiceHandler::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    
   
//  METHODS
void t_CGlxAiwServiceHandler::SetupL(  )
    {
    iAiwServiceHandler = CGlxAiwServiceHandler::InstanceL();
    }


void t_CGlxAiwServiceHandler::Teardown(  )
    {
    iAiwServiceHandler->Close();
    }

void t_CGlxAiwServiceHandler::TestConstructionL(  )
    {
    // Only testing for leaks during setup/teardown here
    EUNIT_ASSERT(ETrue);
    }
    
void t_CGlxAiwServiceHandler::OneImageUniqueParamsL()
    {
    _LIT(KImageUri ,"c:\\Data\\Images\\Image01.jpg");
    _LIT(KImageMimeType, "image/jpeg");
    
    // First Create a Dummy Menu pane (for use later)
    CEikMenuPane* dummyMenuPane = new (ELeave) CEikMenuPane(this);
    CleanupStack::PushL(dummyMenuPane);
    
    // Create a Media Id
    TGlxMedia Image(TGlxMediaId(1));
    
    // Add the Uri and MimeType parameters to internal array
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    
    // Now transfer the parameters from our array to the InParams
    iAiwServiceHandler->HandleSubmenuL(*dummyMenuPane);
    
    CleanupStack::PopAndDestroy(dummyMenuPane);
    
    // Now get the InParams to see if it contains what it should
    CAiwGenericParamList& InParams = iAiwServiceHandler->GetInParams();

    // Check the count is 2
    TInt paramCount = InParams.Count();
    EUNIT_ASSERT( paramCount == 2);
    
    // Check the Uri is correct
    TPtrC uri;
    InParams[0].Value().Get(uri);
    EUNIT_ASSERT(0 == uri.Compare(KImageUri));
    
    // Check the MimeType is correct
    TPtrC mimeType;
    InParams[1].Value().Get(mimeType);
    EUNIT_ASSERT(0 == mimeType.Compare(KImageMimeType));
    }

void t_CGlxAiwServiceHandler::OneImageDuplicateParamsL()
    {
    _LIT(KImageUri ,"c:\\Data\\Images\\Image01.jpg");
    _LIT(KImageMimeType, "image/jpeg");
    
    // First Create a Dummy Menu pane (for use later)
    CEikMenuPane* dummyMenuPane = new (ELeave) CEikMenuPane(this);
    CleanupStack::PushL(dummyMenuPane);
    
    // Create a Media Id
    TGlxMedia Image(TGlxMediaId(1));
    
    // Add the Uri and MimeType parameters to internal array
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));

    // Now add duplicate parameters (lots of them)
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
        
    // Now transfer the parameters from our array to the InParams
    iAiwServiceHandler->HandleSubmenuL(*dummyMenuPane);
    
    CleanupStack::PopAndDestroy(dummyMenuPane);
    
    // Now get the InParams to see if it contains what it should
    CAiwGenericParamList& InParams = iAiwServiceHandler->GetInParams();

    // Check the count is 2
    TInt paramCount = InParams.Count();
    EUNIT_ASSERT( paramCount == 2);
    
    // Check the Uri is correct
    TPtrC uri;
    InParams[0].Value().Get(uri);
    EUNIT_ASSERT(0 == uri.Compare(KImageUri));
    
    // Check the MimeType is correct
    TPtrC mimeType;
    InParams[1].Value().Get(mimeType);
    EUNIT_ASSERT(0 == mimeType.Compare(KImageMimeType));
    }

void t_CGlxAiwServiceHandler::TwoItemsUniqueParamsL()
    {
    _LIT(KImageUri ,"c:\\Data\\Images\\Image01.jpg");
    _LIT(KImageMimeType, "image/jpeg");
    _LIT(KVideoUri ,"c:\\Data\\Videos\\Fireworks.mp4");
    _LIT(KVideoMimeType, "video/mp4v-es");
    
    // First Create a Dummy Menu pane (for use later)
    CEikMenuPane* dummyMenuPane = new (ELeave) CEikMenuPane(this);
    CleanupStack::PushL(dummyMenuPane);
    
    // Create a Media Id
    TGlxMedia Image(TGlxMediaId(1));
    TGlxMedia Video(TGlxMediaId(2));
    
    // Add the Uri and MimeType parameters to internal array
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));
    
    // Now transfer the parameters from our array to the InParams
    iAiwServiceHandler->HandleSubmenuL(*dummyMenuPane);
    
    CleanupStack::PopAndDestroy(dummyMenuPane);
    
    // Now get the InParams to see if it contains what it should
    CAiwGenericParamList& InParams = iAiwServiceHandler->GetInParams();

    // Check the count is correct
    TInt paramCount = InParams.Count();
    EUNIT_ASSERT( paramCount == 4);
    
    // Check the Uri is correct
    TPtrC uri;
    TPtrC mimeType;

    InParams[0].Value().Get(uri);
    EUNIT_ASSERT(0 == uri.Compare(KVideoUri));
    
    // Check the MimeType is correct
    InParams[1].Value().Get(mimeType);
    EUNIT_ASSERT(0 == mimeType.Compare(KVideoMimeType));

    InParams[2].Value().Get(uri);
    EUNIT_ASSERT(0 == uri.Compare(KImageUri));
    
    // Check the MimeType is correct
    InParams[3].Value().Get(mimeType);
    EUNIT_ASSERT(0 == mimeType.Compare(KImageMimeType));
    }

void t_CGlxAiwServiceHandler::TwoItemsDuplicateParamsL()
    {
    _LIT(KImageUri ,"c:\\Data\\Images\\Image01.jpg");
    _LIT(KImageMimeType, "image/jpeg");
    _LIT(KVideoUri ,"c:\\Data\\Videos\\Fireworks.mp4");
    _LIT(KVideoMimeType, "video/mp4v-es");
    
    // First Create a Dummy Menu pane (for use later)
    CEikMenuPane* dummyMenuPane = new (ELeave) CEikMenuPane(this);
    CleanupStack::PushL(dummyMenuPane);
    
    // Create a Media Id
    TGlxMedia Image(TGlxMediaId(1));
    TGlxMedia Video(TGlxMediaId(2));
    
    // Add the Uri and MimeType parameters to internal array
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));

    // Now add duplicate parameters (lots of them)
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KImageUri ) ));
    iAiwServiceHandler->AddParamL(Image.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KImageMimeType ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamFile, TAiwVariant( KVideoUri ) ));
    iAiwServiceHandler->AddParamL(Video.Id(), TAiwGenericParam( EGenericParamMIMEType, TAiwVariant( KVideoMimeType ) ));
    
    // Now transfer the parameters from our array to the InParams
    iAiwServiceHandler->HandleSubmenuL(*dummyMenuPane);
    
    CleanupStack::PopAndDestroy(dummyMenuPane);
    
    // Now get the InParams to see if it contains what it should
    CAiwGenericParamList& InParams = iAiwServiceHandler->GetInParams();

    // Check the count is correct
    TInt paramCount = InParams.Count();
    EUNIT_ASSERT( paramCount == 4);
    
    // Check the Uri is correct
    TPtrC uri;
    TPtrC mimeType;

    InParams[0].Value().Get(uri);
    EUNIT_ASSERT(0 == uri.Compare(KVideoUri));
    
    // Check the MimeType is correct
    InParams[1].Value().Get(mimeType);
    EUNIT_ASSERT(0 == mimeType.Compare(KVideoMimeType));

    InParams[2].Value().Get(uri);
    EUNIT_ASSERT(0 == uri.Compare(KImageUri));
    
    // Check the MimeType is correct
    InParams[3].Value().Get(mimeType);
    EUNIT_ASSERT(0 == mimeType.Compare(KImageMimeType));
    }
    
 // From MEikMenuObserver
void t_CGlxAiwServiceHandler::SetEmphasis(CCoeControl* /* aMenuControl */,TBool /* aEmphasis */)
    {
    
    }
 // From MEikMenuObserver
void t_CGlxAiwServiceHandler::ProcessCommandL(TInt /* aCommandId */)
    {
    
    }


//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_CGlxAiwServiceHandler,
    "Unit test suite for CGlxAiwServiceHandler",
    "UNIT" )

EUNIT_TEST(
    "Test Construction",
    "CGlxAiwServiceHandler",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TestConstructionL, Teardown)

EUNIT_TEST(
    "1 Item Unique Params",
    "CGlxAiwServiceHandler",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, OneImageUniqueParamsL, Teardown)

EUNIT_TEST(
    "1 Item Duplicate Params",
    "CGlxAiwServiceHandler",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, OneImageDuplicateParamsL, Teardown)

EUNIT_TEST(
    "2 Items Unique Ps",
    "CGlxAiwServiceHandler",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TwoItemsUniqueParamsL, Teardown)

EUNIT_TEST(
    "2 Items Duplicate Ps",
    "CGlxAiwServiceHandler",
    "InstanceL",
    "FUNCTIONALITY",
    SetupL, TwoItemsDuplicateParamsL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE

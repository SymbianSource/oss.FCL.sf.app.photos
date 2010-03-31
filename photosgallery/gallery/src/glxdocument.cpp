/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Document class 
*
*/

#include <mpxviewutility.h>

#include "glxappui.h"
#include "glxdocument.h"
#include <glximageviewermanager.h>
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxDocument* CGlxDocument::NewL(CEikApplication& aApp)
    {
    TRACER("CGlxDocument::NewL");
    CGlxDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CGlxDocument* CGlxDocument::NewLC(CEikApplication& aApp)
    {
    TRACER("CGlxDocument::NewLC");
    CGlxDocument* self = new (ELeave) CGlxDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxDocument::ConstructL()
    {
    TRACER("CGlxDocument::ConstructL");
    iViewUtility = MMPXViewUtility::UtilityL();
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();
    }    

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxDocument::CGlxDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxDocument::~CGlxDocument()
    {
    TRACER("CGlxDocument::~CGlxDocument()");
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }
    if ( iImageViewerInstance )
        {
        iImageViewerInstance->DeleteInstance();
        }
	}

// -----------------------------------------------------------------------------
// OpenFileL()
// Open document.
// -----------------------------------------------------------------------------
CFileStore* CGlxDocument::OpenFileL( TBool /*aDoOpen*/,
                                    const TDesC& aFilename, RFs& /*aFs*/ )
    {
    TRACER("CFileStore* CGlxDocument::OpenFileL");
	// Reset old data
    ResetDocument();
    iImageViewerInstance->SetImageUriL(aFilename);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CGlxDocument::OpenFileL()
// Open document.
// -----------------------------------------------------------------------------
void CGlxDocument::OpenFileL( CFileStore*& /*aFileStore*/, RFile& aFile )
	{	
	TRACER("CGlxDocument::OpenFileL()");
	// Make sure that aFile is closed in leave situation
	CleanupClosePushL( aFile );
	
	// Reset old data
	ResetDocument();    
	iImageViewerInstance->SetImageFileHandleL(aFile);	
	CleanupStack::PopAndDestroy( &aFile );
	}

// ----------------------------------------------------------------------------
// CGlxDocument::ResetDocument
// Reset the document
// ----------------------------------------------------------------------------
//
void CGlxDocument::ResetDocument()
    {	
    TRACER("CGlxDocument::ResetDocument()");
    // Set document to NULL
    iImageViewerInstance->Reset();    
    }

// -----------------------------------------------------------------------------
// CreateAppUiL
// -----------------------------------------------------------------------------
//
CEikAppUi* CGlxDocument::CreateAppUiL()
    {
    TRACER("CGlxDocument::CreateAppUiL()");
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CGlxAppUi();
    return appUi;
    }


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

#include "glxivwrappui.h"
#include "glxivwrdocument.h"
#include <glximageviewermanager.h>
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxIVwrDocument* CGlxIVwrDocument::NewL(CEikApplication& aApp)
    {
    TRACER("CGlxIVwrDocument::NewL");
    CGlxIVwrDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CGlxIVwrDocument* CGlxIVwrDocument::NewLC(CEikApplication& aApp)
    {
    TRACER("CGlxIVwrDocument::NewLC");
    CGlxIVwrDocument* self = new (ELeave) CGlxIVwrDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxIVwrDocument::ConstructL()
    {
    TRACER("CGlxIVwrDocument::ConstructL");
    iViewUtility = MMPXViewUtility::UtilityL();
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();
    }    

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxIVwrDocument::CGlxIVwrDocument(CEikApplication& aApp) : CAknDocument(aApp) 
            {
            }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxIVwrDocument::~CGlxIVwrDocument()
    {
    TRACER("CGlxIVwrDocument::~CGlxDocument()");
    if (iViewUtility != NULL)
        {
        iViewUtility->Close();
        }
    if ( NULL != iImageViewerInstance)
        {
        iImageViewerInstance->DeleteInstance();
        }
    }

// -----------------------------------------------------------------------------
// OpenFileL()
// Open document.
// -----------------------------------------------------------------------------
CFileStore* CGlxIVwrDocument::OpenFileL( TBool /*aDoOpen*/,
        const TDesC& aFilename, RFs& /*aFs*/ )
    {
    TRACER("CFileStore* CGlxIVwrDocument::OpenFileL");
    // Reset old data
    ResetDocument();
    iImageViewerInstance->SetImageUriL(aFilename);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CGlxDocument::OpenFileL()
// Open document.
// -----------------------------------------------------------------------------
void CGlxIVwrDocument::OpenFileL( CFileStore*& /*aFileStore*/, RFile& aFile )
    {	
    TRACER("CGlxIVwrDocument::OpenFileL()");
    // Make sure that aFile is closed in leave situation
    CleanupClosePushL( aFile );

    // Reset old data
    ResetDocument();    
    iImageViewerInstance->SetImageFileHandleL(aFile);	
    CleanupStack::PopAndDestroy(); // Close aFile
    }

// ----------------------------------------------------------------------------
// CGlxDocument::ResetDocument
// Reset the document
// ----------------------------------------------------------------------------
//
void CGlxIVwrDocument::ResetDocument()
    {	
    TRACER("CGlxIVwrDocument::ResetDocument()");
    // Set document to NULL
    iImageViewerInstance->Reset();    
    }

// -----------------------------------------------------------------------------
// CreateAppUiL
// -----------------------------------------------------------------------------
//
CEikAppUi* CGlxIVwrDocument::CreateAppUiL()
    {
    TRACER("CGlxIVwrDocument::CreateAppUiL()");
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CGlxIVwrAppUi();
    return appUi;
    }


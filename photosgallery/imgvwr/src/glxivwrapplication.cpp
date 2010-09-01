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
 * Description:    Symbian application class
 *
 */




#include "glxivwrapplication.h"
#include "glxivwrdocument.h"

#include <glxtracer.h>

// UID for the application, 
// this should correspond to the uid defined in the mmp file
static const TUid KUidGlxIvwrApp = {0x200104E7};

// -----------------------------------------------------------------------------
// CreateDocumentL
// -----------------------------------------------------------------------------
//
CApaDocument* CGlxIVwrApplication::CreateDocumentL()
    {
    TRACER("CApaIVwrDocument* CGlxApplication::CreateDocumentL()");
    // Create a Viewer document, and return a pointer to it
    CApaDocument* document = CGlxIVwrDocument::NewL(*this);
    return document;
    }

// -----------------------------------------------------------------------------
// AppDllUid
// -----------------------------------------------------------------------------
//
TUid CGlxIVwrApplication::AppDllUid() const
    {
    // Return the UID for the Viewer application
    return KUidGlxIvwrApp;
    }

#include <eikstart.h>

#ifdef __UI_FRAMEWORKS_V2__

// -----------------------------------------------------------------------------
// NewApplication
// -----------------------------------------------------------------------------
//
CApaApplication* NewApplication()
    {
    return new CGlxIVwrApplication;
    }

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

///////////////////////////////////////////////////////////////////////////////
//
// The following is required for wins on EKA1 (using the exedll target)
//
#if defined(__WINS__) && !defined(EKA2)
EXPORT_C TInt WinsMain(TDesC* aCmdLine)
    {
    return EikStart::RunApplication(NewApplication, aCmdLine);
    }

TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

#else // __UI_FRAMEWORKS_V2__

// Create an application, and return a pointer to it
EXPORT_C CApaApplication* NewApplication()
      {
      return new CGlxIVwrApplication;
      }

// DLL entry point, return that everything is ok
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }

#endif // __UI_FRAMEWORKS_V2__


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
* Description:    Download commmand handler
*
*/





/**
 * @internal reviewed 26/06/2007 by Rowland Cook
 */

//  CLASS HEADER
#include "glxcommandhandlerdownload.h"


//  EXTERNAL INCLUDES
#include <FavouritesLimits.h>               // for favourites context IDs
#include <BrowserLauncher.h>

//  INTERNAL INCLUDES
#include <glxcommandhandlers.hrh>           // for EGlxCmdDownloadImages,
                                            // and EGlxCmdDownloadVideos
#include <glxlog.h>                         // for debug logging



// LOCAL FUNCTIONS AND CONSTANTS



// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerDownload* CGlxCommandHandlerDownload::NewL()
    {
    CGlxCommandHandlerDownload* self = 
                        new (ELeave) CGlxCommandHandlerDownload();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// C++ default constructor 
// ----------------------------------------------------------------------------
CGlxCommandHandlerDownload::CGlxCommandHandlerDownload():CGlxCommandHandler()
    {
    // nothing to do
    }           

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
void CGlxCommandHandlerDownload::ConstructL()
    {
     // No implemenation required
    }   
    

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerDownload::~CGlxCommandHandlerDownload()
    {
     // No implemenation required
    }
    
    

// ----------------------------------------------------------------------------
// ExecuteL
// ----------------------------------------------------------------------------
TBool CGlxCommandHandlerDownload::ExecuteL(TInt aCommandId)
    {
    GLX_FUNC("CGlxCommandHandlerDownload::ExecuteL");
    TInt contextId = KFavouritesNullContextId;
    if(aCommandId == EGlxCmdDownloadImages)
        {
        contextId = KFavouritesImageContextId;
        }
    else if(aCommandId == EGlxCmdDownloadVideos)
        {
        contextId = KFavouritesVideoContextId;
        }
        
    if(contextId != KFavouritesNullContextId)
        {
        
        // Launches the browser with specific context id
        CBrowserLauncher* browserLauncher = CBrowserLauncher::NewLC();
        browserLauncher->LaunchBrowserSyncEmbeddedL( contextId ) ;
        CleanupStack::PopAndDestroy(browserLauncher); // browserLauncher;
        
        return ETrue;
        }
        
    return EFalse;
    }




// End of file


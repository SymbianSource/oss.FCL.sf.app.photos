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
* Description:    provide synchronized access to file systema and metadata source
*
*/




#include <aknViewAppUi.h>				// CAknViewAppUi 
#include <avkon.rsg>
#include <data_caging_path_literals.hrh>		// KDC_APP_RESOURCE_DIR			
#include <eikapp.h>						// CEikApplication 
#include <glxfetcherdialog.rsg>           // FOR GETTING VIEW ID AND RESOURCE ID
#include <glxcollectionpluginall.hrh>	// All item collection plugin id
#include <glxlog.h> 					// Logging macros
#include <glxresourceutilities.h>       // for CGlxResourceUtilities
#include <glxcommandhandlerhelp.h>
#include <mglxmedialist.h>				// MGlxMediaList, CMPXCollectionPath

#include "glxfetcher.hrh"				// For implementation id
#include "glxfetcherdialog.h"           // CGlxFetcherDialog
#include "glxfetcher.h"                 // CGlxFetcher
#include "glxtracer.h"
#include "glxlog.h"

_LIT(KFetcherGridViewResource,"glxfetcherdialog.rsc"); //FetcherGridDialog Resource File
    
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxFetcher* CGlxFetcher::NewL()
    {
    TRACER("CGlxFetcher::NewL");
    CGlxFetcher* self = new( ELeave ) CGlxFetcher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxFetcher::ConstructL()
    {
    TRACER("CGlxFetcher::ConstructL");
    // No implementation required
    }
    
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CGlxFetcher::CGlxFetcher()
    {
    TRACER("CGlxFetcher::CGlxFetcher");
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxFetcher::~CGlxFetcher()
    {
    TRACER("CGlxFetcher::~CGlxFetcher");

    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
	if(iDialog)
		{
		iDialog = NULL;
		}
    delete iSelectionSoftkey;
    delete iTitle;
    
    }
    
// ---------------------------------------------------------------------------
// SetMultiSelectionL
// ---------------------------------------------------------------------------
//
void CGlxFetcher::SetMultiSelectionL( TBool aMultiSelect )
    {
    TRACER("CGlxFetcher::SetMultiSelectionL");
    iIsMultiSelection = aMultiSelect;
    }

// ---------------------------------------------------------------------------
// SetMimeTypesL
// ---------------------------------------------------------------------------
//
void CGlxFetcher::SetMimeTypesL( const MDesCArray& /*aMimeTypes*/ )
    {
    TRACER("CGlxFetcher::SetMimeTypesL (not supported)");
    // No implementation required (not supported)
    }

// ---------------------------------------------------------------------------
// SetSelectionSoftkeyL
// ---------------------------------------------------------------------------
//
void CGlxFetcher::SetSelectionSoftkeyL( const TDesC& aSelectionSoftkey )
    {
    TRACER("CGlxFetcher::SetSelectionSoftkeyL");
	
	HBufC* tmp = aSelectionSoftkey.AllocL();
	delete iSelectionSoftkey;
	iSelectionSoftkey = tmp;
    }

// ---------------------------------------------------------------------------
// SetHeadingL
// ---------------------------------------------------------------------------
//
void CGlxFetcher::SetHeadingL( const TDesC& aTitle )
    {
    TRACER("CGlxFetcher::SetHeadingL");
	
	HBufC* tmp = aTitle.AllocL();
	delete iTitle;
	iTitle = tmp;
    }

// ---------------------------------------------------------------------------
// SetVerifierL
// ---------------------------------------------------------------------------
//
void CGlxFetcher::SetVerifierL( MMGFetchVerifier& aVerifier )
    {
    TRACER("CGlxFetcher::SetVerifierL");

	iVerifier = &aVerifier;
    }

// ---------------------------------------------------------------------------
// GetCancelerL
// ---------------------------------------------------------------------------
//
void CGlxFetcher::GetCancelerL( MMGFetchCanceler*& aCanceler )
    {
    TRACER("CGlxFetcher::GetCancelerL");
	
	aCanceler = this;
    }


// ---------------------------------------------------------------------------
// LaunchL
// ---------------------------------------------------------------------------
//
TBool CGlxFetcher::LaunchL( CDesCArray& aSelectedFiles,
                            TMediaFileType aMediaType )
   	{
   	TRACER("CGlxFetcher::LaunchL");
	TInt buttonId;
    TFileName resFile;
    TParse parse;
    parse.Set(KFetcherGridViewResource, &KDC_APP_RESOURCE_DIR, NULL);

    resFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(resFile);
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resFile);
	
	// Configure heading and filter type
	switch ( aMediaType )
	    {
	    case EImageFile:
	        {
            iTitle = TitleForImageFetcherL();
            iFilterType = EGlxFilterImage;
            break;
	        }
	    case EVideoFile:
	        {
            iTitle = TitleForVideoFetcherL();
            iFilterType = EGlxFilterVideo;
            break;
	        }
	    default:
	        {
            GLX_LOG_INFO("CGlxFetcher::LaunchL  - Error: Unsupported media type, Leaving");
            User::Leave( KErrNotSupported );
	        }
	    }
	// create the dialog first
	iDialog =
	    CGlxFetcherDialog::NewL( aSelectedFiles, iVerifier ,iFilterType ,*iTitle, iIsMultiSelection );
	// Returns zero when Fetcher is cancelled by User.
	
	if( iIsMultiSelection )
		{
		buttonId = iDialog->ExecuteLD( R_MODAL_MULTI_DIALOG );
		}
	else
		{
		buttonId = iDialog->ExecuteLD( R_MODAL_SINGLE_DIALOG );	
		}
    // Return false if the fetcher was canceled by user
	return ( 0 != buttonId && aSelectedFiles.Count()!=0);
  	}

// ---------------------------------------------------------------------------
// Return Dialog title
// ---------------------------------------------------------------------------
//
HBufC* CGlxFetcher::TitleForImageFetcherL() const
    {
    TRACER("CGlxFetcher::TitleForImageFetcherL()");
    return TitleL( R_FETCHER_DIALOG_TITLE_IMAGE, R_FETCHER_DIALOG_TITLE_IMAGES );
    }

// ---------------------------------------------------------------------------
// Return Dialog title
// ---------------------------------------------------------------------------
//
HBufC* CGlxFetcher::TitleForVideoFetcherL() const
    {
    TRACER("CGlxFetcher::TitleForVideoFetcherL()");
    return TitleL( R_FETCHER_DIALOG_TITLE_VIDEO, R_FETCHER_DIALOG_TITLE_VIDEOS );
    } 

// ---------------------------------------------------------------------------
// Return Dialog title
// ---------------------------------------------------------------------------
//
HBufC* CGlxFetcher::TitleL( TInt aSingleSelectionTitleResourceId, 
        TInt aMultiSelectionTitleResourceId ) const
    {
    TRACER("CGlxFetcher::TitleL()");
    if( !iTitle )
        {
        TInt resourceId = aSingleSelectionTitleResourceId;
        if ( iIsMultiSelection )	
            {
            resourceId = aMultiSelectionTitleResourceId;
            }
        return CCoeEnv::Static()->AllocReadResourceL( resourceId );
        }
    return iTitle;
    }

// -----------------------------------------------------------------------------
// From MMGFetchCanceler
// -----------------------------------------------------------------------------
//	
void CGlxFetcher::CancelFetcherL()
    {
    TRACER("CGlxFetcher::CancelFetcherL");
	//pass EAknCmdExit  as parameter to exit the fetcher dialog
	if(iDialog)
		{
		iDialog->CallCancelFetcherL(EAknCmdExit );	
		}
	}

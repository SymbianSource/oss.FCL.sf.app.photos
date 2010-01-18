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
* Description:    Upload commmand handler
*
*/

//  CLASS HEADER
#include "glxcommandhandlerupload.h"

//  EXTERNAL INCLUDES
#include <AiwGenericParam.h>                // for passing data between applications
#include <aknbutton.h>                      // for getting the button state
#include <akntoolbar.h>                     // for accessing currently active toolbar
#include <centralrepository.h>              // for checking the ShareOnline version
#include <data_caging_path_literals.hrh> 	// for directory and file names
#include <utf.h>							// for CnvUtfConverter


//  INTERNAL INCLUDES
#include <glxattributecontext.h>			// for  creating medialist context
#include <glxattributeretriever.h>			// for retrieving the attributes
#include <glxcommandhandlers.hrh>			// for EGlxCmdUpload
#include <glxfetchcontextremover.h>         // for TGlxFetchContextRemover
#include <glxtracer.h>                      // for TRACER logs
#include <glxuiutilities.rsg>               // for Share AIW interest resource
#include <mglxmedialist.h>                  // for accessing the media items

// CONSTANTS AND DEFINITIONS
namespace
    {
    // ShareOnline application UID
    const TUid KShareOnlineUid = { 0x2000BB53 };
    // Shareonline Application version
    const TUint32 KShareApplicationVersion = 0x01010020;
    // Buffer to maintain the ShareOnline version number in use
    const TInt KPhotosShareOnlineVersionBufLen = 12;
    // Minimum version required for OneClickUpload to work
    const TVersion KShareOnlineMinimumVersion( 4, 3, 0 );
    // OneClickUpload command
    const TUid KOpenModeOneClick = { 2 };
    // Command to request for the tooltip
    const TUid KCmdGetOneClickToolTip = { 15 };    
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerUpload* CGlxCommandHandlerUpload::NewL(
									MGlxMediaListProvider* aMediaListProvider, 
									TBool aHasToolbarItem)
	{
	TRACER("CGlxCommandHandlerUpload::NewL");
	CGlxCommandHandlerUpload* self = 
						new (ELeave) CGlxCommandHandlerUpload(aMediaListProvider,aHasToolbarItem);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// C++ default constructor 
// ----------------------------------------------------------------------------
CGlxCommandHandlerUpload::CGlxCommandHandlerUpload(MGlxMediaListProvider* 
													aMediaListProvider,
													TBool aHasToolbarItem)
			 :CGlxMediaListCommandHandler(aMediaListProvider,aHasToolbarItem)
	{
	TRACER("CGlxCommandHandlerUpload::CGlxCommandHandlerUpload");
	
	// nothing to do
	}			

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
void CGlxCommandHandlerUpload::ConstructL()
	{
	TRACER("CGlxCommandHandlerUpload::ConstructL");
	iUiUtility = CGlxUiUtility::UtilityL();
		
	// iUploadSupported is zeroed by CBase constructor.
	// If a leave occurs here, it is left as EFalse.
	// If InitializeOneClickUploadL() succeeds, it is set to ETrue.
	TRAP_IGNORE(
	        {
	        CheckVersionL();
	        InitializeOneClickUploadL();
	        iUploadSupported = ETrue;
	        } );
	
	
	// Add the upload command
   	TCommandInfo info(EGlxCmdUpload);
   	AddCommandL(info);	
	}	
	
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerUpload::~CGlxCommandHandlerUpload()
	{
	TRACER("CGlxCommandHandlerUpload::~CGlxCommandHandlerUpload");
	
	if (iUiUtility)
        {
        iUiUtility->Close();
        }
	delete iServiceHandler;
	}
	
// InitializeAIWForShareOnlineL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::InitializeOneClickUploadL()
    {
    TRACER("CGlxCommandHandlerUpload::InitializeOneClickUploadL");
    
    iServiceHandler = CAiwServiceHandler::NewL();

    // Attach the AIW Resource defined in uiutilities.rss
    iServiceHandler->AttachL( R_AIW_SHARE_BASE_INTEREST );
    }

// Check Share Online version
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::CheckVersionL()
    {    
    TRACER("CGlxCommandHandlerUpload::CheckVersionL");
      
    CRepository* rep = CRepository::NewLC( KShareOnlineUid );
    //
    TBuf<KPhotosShareOnlineVersionBufLen> versionBuf;
    // Query the ShareOnline version in the build
    User::LeaveIfError( rep->Get( KShareApplicationVersion, versionBuf ) );
    
    // Initialize version to zero
    TVersion version( 0, 0, 0 );
    TLex lex( versionBuf );
    User::LeaveIfError( lex.Val( version.iMajor ) );
    if ( lex.Get() != TChar('.') )
        {
        User::Leave( KErrCorrupt );
        }
    User::LeaveIfError( lex.Val( version.iMinor ) );
    if ( lex.Get() != TChar('.') )
        {
        User::Leave( KErrCorrupt );
        }
    User::LeaveIfError( lex.Val( version.iBuild ) ); 
    
    // Compare version number and leave if the detected
    // version is less than KShareOnlineMinimumVersion.
    if ( version.iMajor < KShareOnlineMinimumVersion.iMajor )
        {
        User::LeaveIfError( KErrNotSupported );
        }
    else if ( version.iMajor == KShareOnlineMinimumVersion.iMajor )
        {
        if ( version.iMinor < KShareOnlineMinimumVersion.iMinor )
            {
            User::LeaveIfError( KErrNotSupported );
            }
        else if ( version.iMinor == KShareOnlineMinimumVersion.iMinor )
            {
            if ( version.iBuild < KShareOnlineMinimumVersion.iBuild )
                {
                User::LeaveIfError( KErrNotSupported );
                }
            else
                {
                // Version is supported, fall through
                }
            }
        else
            {
            // Version is supported, fall through
            }
        }
    else
        {
        // Version is supported, fall through
        } 
    CleanupStack::PopAndDestroy( rep ); 
        
    }

// ----------------------------------------------------------------------------
// DoExecuteL
// ----------------------------------------------------------------------------
TBool CGlxCommandHandlerUpload::DoExecuteL(TInt aCommandId, 
													MGlxMediaList& /*aList*/)
	{
	TRACER("CGlxCommandHandlerUpload::DoExecuteL");
	
	TBool handled = EFalse;
	// handle the upload command
	if (iUploadSupported && ( EGlxCmdUpload == aCommandId ))
	    { 

	    CAiwGenericParamList& inputParams = iServiceHandler->InParamListL();
	    
	    TAiwVariant variant( KOpenModeOneClick ); //For one click photo upload
	    TAiwGenericParam param( EGenericParamModeActivation, variant );
	    inputParams.AppendL( param );

	    // Append all the files that are selected to iServiceHandler->InParamListL()
	    AppendSelectedFilesL(inputParams);

	    // Execute the KAiwCmdUpload command  
	    iServiceHandler->ExecuteServiceCmdL( KAiwCmdUpload, 
	            inputParams, iServiceHandler->OutParamListL() );       

	    handled = ETrue;
	    }	
	return handled;
	}

// ----------------------------------------------------------------------------
// DoActivateL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerUpload::DoActivateL(TInt aViewId)
	{
	TRACER("CGlxCommandHandlerUpload::DoActivateL");
    iViewId = aViewId;   
    
	}
	
// ----------------------------------------------------------------------------
// Deactivate
// ----------------------------------------------------------------------------
void CGlxCommandHandlerUpload::Deactivate()
    {
    TRACER("CGlxCommandHandlerUpload::Deactivate");
    
    }	

// ----------------------------------------------------------------------------
// AppendSelectedFilesL()
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::AppendSelectedFilesL(CAiwGenericParamList& aInputParams)
    {    
    TRACER("CGlxCommandHandlerUpload::AppendSelectedFilesL");
    
    HBufC* fileName = HBufC::NewLC(KMaxFileName);
        
    // get the media list reference
    MGlxMediaList& mediaList = MediaList();
    
    //retrieve the file name and path using selection-based fetch contexts
    CGlxDefaultAttributeContext* attributeContext = 
    									CGlxDefaultAttributeContext::NewL();
    CleanupStack::PushL(attributeContext);
    attributeContext->AddAttributeL(KMPXMediaGeneralUri);
    mediaList.AddContextL(attributeContext, KGlxFetchContextPriorityBlocking);
   
    // TGlxContextRemover will remove the context when it goes out of scope
    // Used here to avoid a trap and still have safe cleanup    
    TGlxFetchContextRemover contextRemover (attributeContext, mediaList);    
    CleanupClosePushL( contextRemover );

    // Leaving method returns an err code
    User::LeaveIfError(GlxAttributeRetriever::RetrieveL(*attributeContext, 
            mediaList, ETrue));

    // context off the list
    CleanupStack::PopAndDestroy( &contextRemover ); 
   
    // Extract the filename/s from selected item/s in the media list and append
    // it to AIW InParamList
    TGlxSelectionIterator iterator;
    iterator.SetToFirst(&mediaList);
    TInt index = KErrNotFound;
    
    while ( (index = iterator++) != KErrNotFound )
        {
        // does not return ownership.
        const CGlxMedia* media = mediaList.Item(index).Properties();
        if(media)
            {
            if(media->IsSupported(KMPXMediaGeneralUri))
                {
                // Retrieve the file URI
                fileName->Des() = media->ValueText(KMPXMediaGeneralUri);
                __ASSERT_DEBUG(fileName->Length() <= KMaxFileName, 
                        Panic(EGlxPanicRequiredItemNotFound));
                TAiwVariant fileVariant( fileName );
                TAiwGenericParam fileParam( EGenericParamFile, fileVariant );
                aInputParams.AppendL( fileParam );             
   			}
   		}
        else
            {
            User::Leave(KErrNotReady);
            }   
        }

    CleanupStack::PopAndDestroy(attributeContext);
    CleanupStack::PopAndDestroy(fileName);
    }// contextRemover goes out of scope and removes the context from media list

// ----------------------------------------------------------------------------
// PopulateToolbar
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::PopulateToolbarL()
	{
	TRACER( "CGlxCommandHandlerUpload::PopulateToolbarL" );
		   
	if( iUploadSupported )
	    {
	    SetToolTipL();
	    } 
	}

// ----------------------------------------------------------------------------
// GetToolTipL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::GetToolTipL( HBufC*& aToolTipText )
    {
    TRACER("CGlxCommandHandlerUpload::GetToolTipL");
    
    CAiwGenericParamList& inputParams = iServiceHandler->InParamListL();
    
    // Insert Command parameter that tells provider that tooltip is required        
    TAiwVariant variant(KCmdGetOneClickToolTip);
    TAiwGenericParam param(EGenericParamModeActivation, variant);
    inputParams.AppendL(param);
    
    //Get a reference to output parameter list
    CAiwGenericParamList& outputParams = iServiceHandler->OutParamListL();
    iServiceHandler->ExecuteServiceCmdL(KAiwCmdUpload, inputParams, outputParams);
    
    //Tooltip is returned as a parameter in output list
    for( TInt i = 0 ; i < outputParams.Count(); i++ )
        {
        const TAiwGenericParam& param = outputParams[i];        
        if(EGenericParamNoteItem == param.SemanticId())
            {            
            TAiwVariant value = param.Value();
            TPtrC toolTipToBeReturned = value.AsDes();
            aToolTipText = toolTipToBeReturned.AllocLC();           
            }        
        }   
    }

// ----------------------------------------------------------------------------
// SetToolTipL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::SetToolTipL()
    {
    TRACER("CGlxCommandHandlerUpload::SetToolTipL");

    CAknToolbar* toolbar = iAvkonAppUi->CurrentFixedToolbar();
    if (!toolbar)
        {
        return;
        }

    CAknButton* uploadButton =
            static_cast<CAknButton*> (toolbar->ControlOrNull(EGlxCmdUpload));

    if (uploadButton && iUploadSupported)
        {
        // Get the tooltip text from AIW ShareOnline application
        HBufC* toolTipText = NULL;

        // GetToolTipL might allocate memory. Hence toolTipText should 
        // be popped and destroyed if present.
        GetToolTipL(toolTipText);

        if (toolTipText)
            {
            // Get current button state and set the help text(tool tip)             
            CAknButtonState* currentState = uploadButton->State();

            TBool dimmed = uploadButton->IsDimmed();
            if (dimmed)
                {
                uploadButton->SetDimmed(EFalse);
                currentState->SetHelpTextL(toolTipText->Des());
                uploadButton->SetDimmed(ETrue);
                }
            else
                {
                currentState->SetHelpTextL(toolTipText->Des());
                }
            CleanupStack::PopAndDestroy(toolTipText);
            }
        else
            {
            User::Leave(KErrArgument);
            }
        } // if(uploadButton && iUploadSupported)
    }

// End of file


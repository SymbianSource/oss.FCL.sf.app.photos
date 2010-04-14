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
#include "glxuploadcenrepwatcher.h"
#include <mglxmedialist.h>

//  EXTERNAL INCLUDES
#include <AiwGenericParam.h>                // for passing data between applications
#include <aknbutton.h>                      // for getting the button state
#include <akntoolbar.h>                     // for accessing currently active toolbar
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

#include <glxnavigationalstate.h>
#include <glxcollectionpluginimageviewer.hrh>

#include <thumbnaildata.h>
#include <gulicon.h>

// CONSTANTS AND DEFINITIONS
namespace
    {
    // ShareOnline application UID    
    const TUid KShareOnlineUid = { 0x2002CC1F };
    // Shareonline Application version
    const TUint32 KShareApplicationVersion = 0x1010020;
    // Buffer to maintain the ShareOnline version number in use
    const TInt KPhotosShareOnlineVersionBufLen = 12;
    // Minimum version required for OneClickUpload to work
    const TVersion KShareOnlineMinimumVersion( 5, 0, 0 );
    // OneClickUpload command
    const TUid KOpenModeOneClick = { 2 };
    // Command to request for the tooltip
    const TUid KCmdGetOneClickToolTip = { 15 };    
    // OneClickUpload default image icon
    const TUint32 KUploadImageServiceIconFileName = 0x00000002;
    // OneClickUpload default video icon
    const TUint32 KUploadVideoServiceIconFileName = 0x00000003;
    // OneClickUpload default video and image icon
    const TUint32 KUploadImageAndVideoServiceIconFileName = 0x00000004; 
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
	
	iSelectedImageCount = 0;
	iSelectedVideoCount = 0;
	iCurrentCenRepMonitor = EMonitorNone;
	iTnmRequestID = KErrNotFound;
	
	iIsFullScreenView = IsFullScreenViewL();
	
    //Check for fullscreen here since we dont get the activate call in FS.
    if(iIsFullScreenView)
        {
        //Giving the viewid as zero, since its not used anywhere.
        iToolbar = iAvkonAppUi->CurrentFixedToolbar();        
        ActivateL(0);
        UpdateFSUploadIconL();
        }
    
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
	delete iTnEngine;
	
	if (iUiUtility)
        {
        iUiUtility->Close();
        }
	delete iServiceHandler;
	delete iUploadCenRepWatcher;	
	}
	
// InitializeAIWForShareOnlineL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::InitializeOneClickUploadL()
    {
    TRACER("CGlxCommandHandlerUpload::InitializeOneClickUploadL");
    
    iServiceHandler = CAiwServiceHandler::NewL();

    // Attach the AIW Resource defined in uiutilities.rss
    iServiceHandler->AttachL( R_GLX_AIW_SHARE_BASE_INTEREST );
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
	    inputParams.Reset();
	    
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

    // get media list from provider and add observer

    MediaList().AddMediaListObserverL( this );
    
    //Get the grid toolbar here as it wont be created yet in
    //constructor
    if(!iIsFullScreenView)
        {    
        iToolbar = iUiUtility->GetGridToolBar();        
        }
	}
	
// ----------------------------------------------------------------------------
// Deactivate
// ----------------------------------------------------------------------------
void CGlxCommandHandlerUpload::Deactivate()
    {
    TRACER("CGlxCommandHandlerUpload::Deactivate");
	
	//Remove the Medialist observer
    MediaList().RemoveMediaListObserver( this );

	//Reset the tracking variables here    
    iSelectedImageCount = 0;
    iSelectedVideoCount = 0;
    iCurrentCenRepMonitor = EMonitorNone;
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
    
	//When the Upload is not supported or if we are in grid view
	//and none of the item is selected Dim the Upload icon
	if(!iUploadSupported || (!iIsFullScreenView && 
                                   (MediaList().SelectionCount()== 0)))
        {        
        DisableUploadToolbarItem(ETrue);
        }
    
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
    inputParams.Reset();
    
    // Insert Command parameter that tells provider that tooltip is required        
    TAiwVariant variant(KCmdGetOneClickToolTip);
    TAiwGenericParam param(EGenericParamModeActivation, variant);
    inputParams.AppendL(param);
    
    //Get a reference to output parameter list
    CAiwGenericParamList& outputParams = iServiceHandler->OutParamListL();
    outputParams.Reset();
    
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

    if (!iToolbar)
        {
        return;
        }

    CAknButton* uploadButton =
            static_cast<CAknButton*> (iToolbar->ControlOrNull(EGlxCmdUpload));

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


// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
                                        MGlxMediaList* /*aList*/)
    {
    
    }
	
	
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//	
void CGlxCommandHandlerUpload::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    
    }


// ----------------------------------------------------------------------------
// HandleItemRemovedL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
                                    MGlxMediaList* /*aList*/)
    {
    
    }
	
	
// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//	
void CGlxCommandHandlerUpload::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
                                    MGlxMediaList* /*aList*/)
    {
    
    }
	
	
// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandleAttributesAvailableL(TInt /*aItemIndex*/,     
    const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
    {
	TRACER("CGlxCommandHandlerUpload::HandleAttributesAvailableL");
    }
	
	
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
                    TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
	TRACER("CGlxCommandHandlerUpload::HandleFocusChangedL");
    //In Fullscreen change the icons based on current focused icon
    if(iUploadSupported && iIsFullScreenView)
        {
        UpdateFSUploadIconL();
        }
    }


// ----------------------------------------------------------------------------
// HandleItemSelectedL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList)
    {
	TRACER("CGlxCommandHandlerUpload::HandleItemSelectedL");
	//In grid if an item is selected update the toolbar icon based on
	//the mime types of items
    if(iUploadSupported && !iIsFullScreenView)
        {        
        if(aList->SelectionCount() >= 1 )
            {        
            TFileName uploadIconFileName;
            UpdateSelectionCount(aIndex, aSelected, aList);
            GetIconNameL(uploadIconFileName);
            if(uploadIconFileName.Length())
                {
                DecodeIconL(uploadIconFileName);
                }
			//Enable the toolbar only once to avoid performance prb
	        if(aList->SelectionCount() == 1 )
				{
            	DisableUploadToolbarItem(EFalse);
				}
            }
        else
            {
            iSelectedImageCount = 0;
            iSelectedVideoCount = 0;
            iCurrentCenRepMonitor = EMonitorNone;
            delete iUploadCenRepWatcher;
            iUploadCenRepWatcher = NULL;
            DisableUploadToolbarItem(ETrue);
            }        
        }    
    }


// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandleMessageL(const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/)
    {
    
    }


// ----------------------------------------------------------------------------
// HandlePopulatedL
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandlePopulatedL(MGlxMediaList* aList)
    {
    if(aList && aList->Count() > 0)
        {
        if(!iUploadSupported || !iIsFullScreenView)
            {
            DisableUploadToolbarItem(ETrue);
            }
        }
    }

//----------------------------------------------------------------------------
// Check for current view mode .Grid/fullscreen/imgviewer
//----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerUpload::IsFullScreenViewL()
    {
	TRACER("CGlxCommandHandlerUpload::IsFullScreenViewL");
    TBool fullscreenViewingMode = EFalse;
             
     CGlxNavigationalState* navigationalState = CGlxNavigationalState::InstanceL();
	 CleanupClosePushL( *navigationalState );
     CMPXCollectionPath* naviState = navigationalState->StateLC();
     
     if ( naviState->Levels() >= 1)
         {
         if (navigationalState->ViewingMode() == NGlxNavigationalState::EBrowse) 
             {
             // For image viewer collection, goto view mode
             if (naviState->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
                 {
                 // current view mode is img vwr
                 fullscreenViewingMode = ETrue;
                 }
             else
                 {
                 //current view mode is Grid 
                 fullscreenViewingMode = EFalse;
                 }
             } 
         else 
             {
             //current view mode is Fullscreen
             fullscreenViewingMode = ETrue;
             }                
         }
     CleanupStack::PopAndDestroy( naviState );
     CleanupStack::PopAndDestroy( navigationalState );
     return fullscreenViewingMode;
    }

// ----------------------------------------------------------------------------
// DisableUploadToolbarItem
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::DisableUploadToolbarItem(TBool aDimmed)
    {        
	TRACER("CGlxCommandHandlerUpload::DisableUploadToolbarItem");
    
    if(iToolbar)
        {
        iToolbar->SetItemDimmed(EGlxCmdUpload, aDimmed, ETrue);
        //DrawNow must be called since SetDimmed does not redraw the toolbar
        iToolbar->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerUpload::ThumbnailPreviewReady()
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::ThumbnailPreviewReady(MThumbnailData& /*aThumbnail*/,
        TThumbnailRequestId /*aId*/)
    {
    TRACER("CGlxCommandHandlerUpload::ThumbnailPreviewReady");
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerUpload::ThumbnailReady()
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::ThumbnailReady(TInt aError,
        MThumbnailData& aThumbnail, TThumbnailRequestId /*aId*/)
    {
    TRACER("CGlxCommandHandlerUpload::ThumbnailReady");
    
    if(aError == KErrNone)
        {
        TRAP_IGNORE(SetDecodedUploadIconL(aThumbnail));        
        }
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerUpload::SetDecodedUploadIconL()
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::SetDecodedUploadIconL(MThumbnailData& aThumbnail)
    {
    CAknButton* uploadButton =
            static_cast<CAknButton*> (iToolbar->ControlOrNull(EGlxCmdUpload));

	if(uploadButton)
		{
	    CAknButtonState* currentState = uploadButton->State();    
	    CFbsBitmap* normalBmp = aThumbnail.DetachBitmap();    
	    CFbsBitmap* pressedBmp = new (ELeave) CFbsBitmap;
	    pressedBmp->Duplicate(normalBmp->Handle());
	    //Ownership of the icon is transferred here    
	    currentState->SetIcon(CGulIcon::NewL(normalBmp));
	    currentState->SetPressedIcon(CGulIcon::NewL(pressedBmp));
	    iToolbar->DrawNow();
	  	}
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerUpload::UpdateSelectionCount()
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::UpdateSelectionCount(TInt aIndex, TBool aSelected, MGlxMediaList* aList)
    {
    TRACER("CGlxCommandHandlerUpload::UpdateSelectionCount");
    if(!aList)
        {
        return;
        }    
    
    TGlxMedia media = aList->Item(aIndex);
    
    switch(media.Category())
        {
        case EMPXImage:
            aSelected?iSelectedImageCount++:iSelectedImageCount--;            
            break;
        case EMPXVideo:
            aSelected?iSelectedVideoCount++:iSelectedVideoCount--;
            break;
        default:
            break;
        }       
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerUpload::GetIconNameL()
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::GetIconNameL(TDes& aUplaodIconNmae)
    {
    TRACER("CGlxCommandHandlerUpload::GetIconNameL");
    TUint32 serviceIconId = KErrNone;
    
    if(iSelectedImageCount && iSelectedVideoCount)
        {
        if(iCurrentCenRepMonitor != EImageVideoMonitor)
            {
            serviceIconId = KUploadImageAndVideoServiceIconFileName;
            iCurrentCenRepMonitor = EImageVideoMonitor;
            }
        }
    else if(iSelectedImageCount)
        {
        if(iCurrentCenRepMonitor != EImageMonitor)
            {
            serviceIconId = KUploadImageServiceIconFileName;
            iCurrentCenRepMonitor = EImageMonitor;
            }    
        }
    else if(iSelectedVideoCount)
        {
        if(iCurrentCenRepMonitor != EVideoMonitor)
            {
            serviceIconId = KUploadVideoServiceIconFileName;
            iCurrentCenRepMonitor = EVideoMonitor;
            }
        }
    
    if(serviceIconId != KErrNone)
        {
        delete iUploadCenRepWatcher;
        iUploadCenRepWatcher = NULL;
        
        iUploadCenRepWatcher = CGlxUploadCenRepWatcher::NewL(*this, KShareOnlineUid,
                                                                serviceIconId );
        iUploadCenRepWatcher->KeyValueL(aUplaodIconNmae);        
        }
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerUpload::DecodeIconL()
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::DecodeIconL(const TDes& aUplaodIconNmae)
    {
    TRACER("CGlxCommandHandlerUpload::DecodeIconL");
    
    if(!iTnEngine)
        {
        iTnEngine = CThumbnailManager::NewL( *this);
        iTnEngine->SetDisplayModeL( EColor16M );
        }
    
    if(iTnmRequestID != KErrNotFound)
        {
        //Cancel any outstanding request
        iTnEngine->CancelRequest(iTnmRequestID);
        }
                
    iTnEngine->SetFlagsL(CThumbnailManager::EDefaultFlags);

    CAknButton* uploadButton =
                           static_cast<CAknButton*> (iToolbar->ControlOrNull(EGlxCmdUpload));
    if(uploadButton)
    	{
	    CAknButtonState* currentState = uploadButton->State();
	    const CGulIcon *icon = currentState->Icon();    
	    iTnEngine->SetThumbnailSizeL(icon->Bitmap()->SizeInPixels());
	    iTnEngine->SetQualityPreferenceL(CThumbnailManager::EOptimizeForQuality);
	    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(aUplaodIconNmae, 0);
	    iTnmRequestID = iTnEngine->GetThumbnailL(*source);
	    CleanupStack::PopAndDestroy(source);
	    }
    }


// -----------------------------------------------------------------------------
// CGlxCommandHandlerUpload::UpdateFSUploadIconL()
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::UpdateFSUploadIconL()
    {
    if(!iUploadSupported)
        {
        return;
        }
    
    // get the media list reference
    MGlxMediaList& mediaList = MediaList();
    if(mediaList.Count() )
        {
        TGlxMedia media = mediaList.Item(mediaList.FocusIndex());
        iSelectedImageCount = 0;
        iSelectedVideoCount = 0;
        if(media.Category() == EMPXImage)
            {
            iSelectedImageCount++;
            }
        else if(media.Category() == EMPXVideo)
            {
            iSelectedVideoCount++;
            }
        
        if(iSelectedImageCount || iSelectedVideoCount)
            {
            TFileName uploadIconFileName;
            GetIconNameL(uploadIconFileName);

            if(uploadIconFileName.Length())
                {
                DecodeIconL(uploadIconFileName);
                }
            }
        }
    }

//-----------------------------------------------------------------------------
// From class MGlxUploadIconObserver.
// Called when upload icon changes
//-----------------------------------------------------------------------------
//
void CGlxCommandHandlerUpload::HandleUploadIconChangedL( )
    {
    TRACER("CGlxCommandHandlerUpload::HandleUploadIconChangedL");
    if(iUploadSupported && iUploadCenRepWatcher)
        {
        TFileName uploadIconFileName;
        iUploadCenRepWatcher->KeyValueL(uploadIconFileName);
        if(uploadIconFileName.Length())
            {
            DecodeIconL(uploadIconFileName);
            }
        }
    }
// End of file


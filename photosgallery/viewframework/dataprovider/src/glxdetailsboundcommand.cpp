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
* Description:    Implementation of Command Binding Class's
*
*/




#include    <e32base.h>                                 // Base class for all classes to be instantiated on the heap
#include    <ExifModify.h>                              //For CExifModify
#include    "glxdetailsboundcommand.hrh"
#include    "glxdetailsboundcommand.h"                  // Handles user commands
#include    <glxmetadatadialog.rsg>
#include    <glxuiutilities.rsg>
#include    <glxuiutilities.rsg>
#include    <glxcommandhandleraddtocontainer.h>	        // For CGlxCommandHandlerAddToContainer
#include    <glxcommandhandlers.hrh> 			        // for command handler id
#include    <glxdrmutility.h>	                        //For launching DRM details pane
#include    <glxmedialist.h>                            // Interface for reading lists of media items
#include    <glxcommandfactory.h>			            //for command factory
#include    <mpxcommandgeneraldefs.h>                   // Content ID identifying general category of content provided
#include    <glxtextentrypopup.h>                       //for testing
#include    <glxgeneraluiutilities.h>                   // General utilties class definition
#include    <StringLoader.h>                            // Class provides methods to load and format resource strings
#include    <glxlog.h>                                  // Logging
#include    <glxtracer.h>
#include    <glxviewbase.rsg>
#include    "glxuiutility.h"
#include <glxscreenfurniture.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
/** Error Id EMPY-7MKDHP **/ 
#include 	"mglxmetadatadialogobserver.h" 				//for call back to dailog


//CONSTANTS
const TInt KMaxMediaPopupTitleLength = 0x100;

// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxTitleBoundCommand* CGlxTitleBoundCommand::NewL()
	{
	TRACER("CGlxTitleBoundCommand::NewL");
	CGlxTitleBoundCommand* self = CGlxTitleBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxTitleBoundCommand* CGlxTitleBoundCommand::NewLC()
	{
	TRACER("CGlxTitleBoundCommand::NewLC");
	CGlxTitleBoundCommand* self = new ( ELeave ) CGlxTitleBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxTitleBoundCommand::~CGlxTitleBoundCommand()
    {
    
    }
// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxTitleBoundCommand::OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist )
	{
	TRACER("CGlxTitleBoundCommand::OfferCommandL");
	TBool consumed = EFalse;
	iMedialist = &aMedialist;
	const CGlxMedia* media = iMedialist->Item(0).Properties();
	if( media )
	    {
	    if ( aCommandId == KGlxEditBoundMenuCommandId  && media->IsSupported( KMPXMediaGeneralTitle))
	        {
	        ///@todo - remove popup when editable template is avaliable
	        //Popup for testing
	        HBufC* textBuf = HBufC::NewLC( KMaxMediaPopupTitleLength );
	        (textBuf->Des()).Copy((media->ValueText(KMPXMediaGeneralTitle)));
	        TPtr textPtr = textBuf->Des();
	        /// bug fix - EMPY-7MCKD6 
	        TBuf<KMaxMediaPopupTitleLength> titleText(*textBuf);
	        /// bug fix - EMPY-7MCKD6
	        ///@todo - remove literal for popup title when resource string is avaliable
			HBufC *buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_TITLE_NSERIES);
	        CGlxTextEntryPopup* popup = CGlxTextEntryPopup::NewL( *buf, textPtr );
			CleanupStack::PopAndDestroy(buf);
            if ( popup->ExecuteLD() == EEikBidOk )
                {
                // Text entry was successful
                /// bug fix - EMPY-7MCKD6
                //Compare if there is any change in the entry
                if(0 != (titleText.Compare(*textBuf)))
                /// bug fix - EMPY-7MCKD6
                 ModifyTitleL( *textBuf );
                }
	        CleanupStack::PopAndDestroy( textBuf ); 
	        consumed = ETrue;
	        }
	    }
	return consumed;
	}
	
// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxTitleBoundCommand::InitMenuL( CEikMenuPane& aMenu ) const
	{
	TRACER("CGlxTitleBoundCommand::InitMenuL");
	aMenu.SetItemTextL( KGlxEditBoundMenuCommandId, R_GLX_METADATA_VIEW_OPTIONS_EDIT );
	aMenu.SetItemDimmed( KGlxEditBoundMenuCommandId, EFalse );
	}

// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxTitleBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxTitleBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }

// -----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::ModifyTitleL
// -----------------------------------------------------------------------------
//  
void CGlxTitleBoundCommand::ModifyTitleL(const TDesC& aData )
    {    
    TRACER("CGlxTitleBoundCommand::ModifyTitleL");
    
    if( iMedialist->Count() > 0 )
        {
        iMedialist->SetFocusL(NGlxListDefs::EAbsolute,0);//set focus to first item
        CMPXCollectionPath* path = iMedialist->PathLC();
        CMPXCommand* command = TGlxCommandFactory::RenameCommandLC(aData,*path);
        
        command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, static_cast<TAny*>(this));
        
        iMedialist->CommandL(*command);
         
        CleanupStack::PopAndDestroy(command);
        CleanupStack::PopAndDestroy(path);
        }
    }
	
// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxTitleBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxTitleBoundCommand::ChangeMskL()
    {
    TRACER("CGlxTitleBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeySelect,R_GLX_METADATA_MSK_EDIT);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxTitleBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxTitleBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}

// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxTagsBoundCommand* CGlxTagsBoundCommand::NewL()
	{
	TRACER("CGlxTagsBoundCommand::NewL");
	CGlxTagsBoundCommand* self = CGlxTagsBoundCommand::NewLC();
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxTagsBoundCommand* CGlxTagsBoundCommand::NewLC()
	{
	TRACER("CGlxTagsBoundCommand::NewLC");
	CGlxTagsBoundCommand* self = new ( ELeave ) CGlxTagsBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}
	

// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::ConstructL
// ----------------------------------------------------------------------------
//
void CGlxTagsBoundCommand::ConstructL()
	{
	TRACER("CGlxTagsBoundCommand::ConstructL");
	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());
	//create command handler
	iAddToTag = CGlxCommandHandlerAddToContainer::NewL(this, EGlxCmdAddTag,
			EFalse, uiutilitiesrscfile);
	}
	
// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::~CGlxTagsBoundCommand
// ----------------------------------------------------------------------------
//
CGlxTagsBoundCommand::~CGlxTagsBoundCommand()
    {
    if(iAddToTag)
        {
        delete iAddToTag;
        }
    }

// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxTagsBoundCommand::OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist)
	{
	TRACER("CGlxTagsBoundCommand::OfferCommandL");
	SetMediaList(&aMedialist);
	TBool consumed = EFalse;
	if ( aCommandId == KGlxEditBoundMenuCommandId )
		{
		iAddToTag->ExecuteL(EGlxCmdAddTag);
		consumed = ETrue;
		}
	return consumed;
	}
	
// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxTagsBoundCommand::InitMenuL( CEikMenuPane& aMenu ) const
	{
	TRACER("CGlxTagsBoundCommand::InitMenuL");
	aMenu.SetItemTextL( KGlxEditBoundMenuCommandId, R_GLX_METADATA_VIEW_OPTIONS_EDIT ); 
	aMenu.SetItemDimmed( KGlxEditBoundMenuCommandId, EFalse );
	}

// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxTagsBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxTagsBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }
	
// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxTagsBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxTagsBoundCommand::ChangeMskL()
    {
    TRACER("CGlxTagsBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                EAknSoftkeySelect,R_GLX_METADATA_MSK_ADD);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxTagsBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}


// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::SetMediaList
// ----------------------------------------------------------------------------
//
void CGlxTagsBoundCommand::SetMediaList(MGlxMediaList* aMedialist)
	{
	TRACER("CGlxTagsBoundCommand::SetMediaList");
	iMedialist = aMedialist;
	}
	
// ----------------------------------------------------------------------------
// CGlxTagsBoundCommand::MediaList
// ----------------------------------------------------------------------------
//
 MGlxMediaList& CGlxTagsBoundCommand::MediaList()
	 {
	 TRACER("CGlxTagsBoundCommand::MediaList");
	 return *iMedialist;
	 }

// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxDescriptionBoundCommand* CGlxDescriptionBoundCommand::NewL()
	{
	TRACER("CGlxDescriptionBoundCommand::NewL");
	CGlxDescriptionBoundCommand* self = CGlxDescriptionBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxDescriptionBoundCommand* CGlxDescriptionBoundCommand::NewLC()
	{
	TRACER("CGlxDescriptionBoundCommand::NewLC");
	CGlxDescriptionBoundCommand* self = new ( ELeave ) CGlxDescriptionBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxDescriptionBoundCommand::~CGlxDescriptionBoundCommand()
    {
    
    }

// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxDescriptionBoundCommand::OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist )
	{
	TRACER("CGlxDescriptionBoundCommand::OfferCommandL");
	TBool consumed = EFalse;
	iMedialist = &aMedialist;
	const CGlxMedia* media = iMedialist->Item(0).Properties();
	if ( aCommandId == KGlxEditBoundMenuCommandId )
		{
		///@todo - remove popup when editable template is avaliable
		//Popup for testing
		HBufC* textBuf = HBufC::NewLC( KMaxMediaPopupTitleLength );
		(textBuf->Des()).Copy((media->ValueText(KMPXMediaGeneralComment)));
		TPtr textPtr = textBuf->Des();
		/// bug fix - EMPY-7MCKD6
		TBuf<KMaxMediaPopupTitleLength> descText(*textBuf);
		/// bug fix - EMPY-7MCKD6
		///@todo - remove literal for popup title when resource string is avaliable
		
		HBufC *buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_DESCRIPTION_NSERIES);
	    CGlxTextEntryPopup* popup = CGlxTextEntryPopup::NewL( *buf, textPtr );
		CleanupStack::PopAndDestroy(buf);
		
		
	    if ( popup->ExecuteLD() == EEikBidOk )
    		{
    		// Text entry was successful
    		/// bug fix - EMPY-7MCKD6
    		//Compare if there is any change in the entry
    		if(0 != (descText.Compare(*textBuf)))
    		/// bug fix - EMPY-7MCKD6
    		ModifyDescriptionL( *textBuf );
    	    }
		CleanupStack::PopAndDestroy( textBuf );
		consumed = ETrue;
		}
	return consumed;
	}
	
// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxDescriptionBoundCommand::InitMenuL( CEikMenuPane& aMenu ) const
	{
	TRACER("CGlxDescriptionBoundCommand::InitMenuL");
	aMenu.SetItemTextL( KGlxEditBoundMenuCommandId, R_GLX_METADATA_VIEW_OPTIONS_EDIT ); 
	aMenu.SetItemDimmed( KGlxEditBoundMenuCommandId, EFalse );
	}

// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxDescriptionBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxDescriptionBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }
	
// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxDescriptionBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxDescriptionBoundCommand::ChangeMskL()
    {
    TRACER("CGlxDescriptionBoundCommand::ChangeMskL");
    
    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeySelect,R_GLX_METADATA_MSK_EDIT);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxDescriptionBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}


// -----------------------------------------------------------------------------
// CGlxDescriptionBoundCommand::ModifyDescriptionL
// -----------------------------------------------------------------------------
//	
void CGlxDescriptionBoundCommand::ModifyDescriptionL(const TDesC& aData )
	{	 
	TRACER("CGlxDescriptionBoundCommand::ModifyDescriptionL");
	if( iMedialist->Count() > 0 )
        { 
        iMedialist->SetFocusL(NGlxListDefs::EAbsolute,0);//set focus to first item
    	CMPXCollectionPath* path =  iMedialist->PathLC();
    	CMPXCommand* command = TGlxCommandFactory::SetDescriptionCommandLC(aData,*path);
    	command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, static_cast<TAny*>(this));
    	
    	iMedialist->CommandL(*command);
     	CleanupStack::PopAndDestroy(command);
        CleanupStack::PopAndDestroy(path);
        }
	}
	
// ----------------------------------------------------------------------------
// CGlxLocationBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxLocationBoundCommand* CGlxLocationBoundCommand::NewL()
	{
	TRACER("CGlxLocationBoundCommand::NewL");
	CGlxLocationBoundCommand* self = CGlxLocationBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxLocationBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxLocationBoundCommand* CGlxLocationBoundCommand::NewLC()
	{
	TRACER("CGlxLocationBoundCommand::NewLC");
	CGlxLocationBoundCommand* self = new ( ELeave ) CGlxLocationBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxLocationBoundCommand::~CGlxLocationBoundCommand()
    {
    iDialogObserver = NULL ;
    }

// ----------------------------------------------------------------------------
// CGlxLocationBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxLocationBoundCommand::OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist )
	{
	TRACER("CGlxLocationBoundCommand::OfferCommandL");
	iMedialist = &aMedialist;
	TBool consumed = EFalse;
	if ( aCommandId == KGlxEditBoundMenuCommandId )
		{
		iOnNaviKeySelect = ETrue;
		//get the media item
		const TGlxMedia& media = iMedialist->Item( 0 );
		// Test to see if the coordinate is present
		TCoordinate coordinate;
		TBool isSupported = media.GetCoordinate(coordinate);
		if(!isSupported)
			{
			HBufC *buf = StringLoader::LoadLC(R_GLX_METADATA_NOTE_INFO_NO_LOCATION);
	        GlxGeneralUiUtilities::ShowInfoNoteL(*buf,ETrue);
	        CleanupStack::PopAndDestroy(buf);
	        consumed = ETrue;
			}
		else
			{
			/** Show menu pane options **/
			if(iDialogObserver)
				{
				TRAP_IGNORE(iDialogObserver->OnLocationEditL());
				}
			}
		}
	else
	    {
        iOnNaviKeySelect = EFalse;	    
	    }
	if ( aCommandId == KGlxDeleteBoundMenuCommandId )
		{
		RemoveLocationL();
		consumed = ETrue;
		}
	return consumed;
	}
	
// ----------------------------------------------------------------------------
// CGlxLocationBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxLocationBoundCommand::InitMenuL( CEikMenuPane& aMenu ) const
	{
	TRACER("CGlxLocationBoundCommand::InitMenuL");
	aMenu.SetItemTextL( EGlxCmdAiwShowMap, R_GLX_METADATA_SHOW_ON_MAP_OPTIONS_VIEW ); 
	aMenu.SetItemDimmed( EGlxCmdAiwShowMap, EFalse );
	aMenu.SetItemTextL( KGlxDeleteBoundMenuCommandId, R_GLX_METADATA_VIEW_OPTIONS_DELETE ); 
	aMenu.SetItemDimmed( KGlxDeleteBoundMenuCommandId, EFalse );
	if( iOnNaviKeySelect )
	    {
	    aMenu.SetItemDimmed( EAknCmdHelp, ETrue );
	    }
	}

// ----------------------------------------------------------------------------
// CGlxLocationBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxLocationBoundCommand::HideLocationMenuItem(CEikMenuPane& /*aMenu*/) const
    {
    TRACER("CGlxLocationBoundCommand::HideLocationMenuItem");
    }
	
// ----------------------------------------------------------------------------
// CGlxLocationBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxLocationBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxLocationBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxLocationBoundCommand::ChangeMskL()
    {
    TRACER("CGlxLocationBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeySelect,R_GLX_METADATA_MSK_BLANK);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// CGlxLocationBoundCommand::RemoveLocationL
// -----------------------------------------------------------------------------
//	
void CGlxLocationBoundCommand::RemoveLocationL()	
	{
	TRACER("CGlxLocationBoundCommand::RemoveLocationL");
	
       // get the media item
    const TGlxMedia& media = iMedialist->Item( 0 );
    
    // Test to see if the coordinate is present
    TCoordinate coordinate;
    TBool isSupported = media.GetCoordinate(coordinate);
    
    if( !isSupported )
        {
        HBufC *buf = StringLoader::LoadLC(R_GLX_METADATA_NOTE_INFO_NO_LOCATION);
        GlxGeneralUiUtilities::ShowInfoNoteL(*buf,ETrue);
        CleanupStack::PopAndDestroy(buf);
        }
    else
        {
        HBufC *buf = StringLoader::LoadLC(R_GLX_METADATA_NOTE_DELETE_LOCATION);
        TBool response = GlxGeneralUiUtilities::ConfirmQueryL(R_GLX_QUERY_YES_NO,*buf);
        CleanupStack::PopAndDestroy(buf);
   
        if( response )
            {
            //user selected yes, so delete location     
            //send command for delete location after successfull removal of command remove this pane
            if( iMedialist->Count() > 0 )
                {       
                iMedialist->SetFocusL(NGlxListDefs::EAbsolute,0);//set focus to first item
                
                // Deleting location information from image file

                RFs rFs;
                User::LeaveIfError( rFs.Connect() );
                CleanupClosePushL( rFs );
                RFile rFile;
                
                User::LeaveIfError( rFile.Open(rFs,iMedialist->Item(0).Uri(),EFileWrite ));
                CleanupClosePushL( rFile );
                
                TInt imageFileSize = 0; // Image File Size
                User::LeaveIfError( rFile.Size( imageFileSize ) );
                HBufC8* imageData = HBufC8::NewL( imageFileSize );  //Actual Image Data
                CleanupStack::PushL( imageData );
                
                TPtr8 myImagePtr = imageData->Des();
                TInt readError = rFile.Read( myImagePtr );
                if ( readError != KErrNone )
                    {
                    User::Leave( KErrGeneral );
                    }
                    
                //CExifModify Interface class for modifying existing Exif v2.2 (or prior) 
                //file format or creating Exif v2.2 file format using valid Jpeg image     
                CExifModify* exifWriter = CExifModify::NewL( imageData->Des(), CExifModify::EModify, CExifModify::ENoJpegParsing );
                CleanupStack::PushL( exifWriter );
                //Removes the specified IFD structure and all its tags from the Exif data
                exifWriter->DeleteIfd  ( EIfdGps  );
                
                HBufC8* modifiedExif = exifWriter->WriteDataL( imageData->Des() );  //Modified Image Data
                CleanupStack::PushL( modifiedExif );
                
                const TUint32 fileSize = modifiedExif->Des().Length();  //Size of Modified File
                TInt oldSize;
                rFile.Size( oldSize );
                // set position to begin of file & write the Modified data (Without Location Information)
                TInt pos = 0;
                User::LeaveIfError( rFile.Seek( ESeekStart, pos ) );
                User::LeaveIfError( rFile.Write( modifiedExif->Des(), fileSize ) );

                TTime lastModified;
                lastModified.UniversalTime();   

                User::LeaveIfError( rFile.SetModified( lastModified ) );    //Change the Modified Time

                CleanupStack::PopAndDestroy( modifiedExif);
                CleanupStack::PopAndDestroy( exifWriter);
                CleanupStack::PopAndDestroy( imageData );
                CleanupStack::PopAndDestroy( &rFile );
                CleanupStack::PopAndDestroy( &rFs );

                // Deleting location information from MDS database

                CMPXCollectionPath* path =  iMedialist->PathLC();
                CMPXCommand* command = TGlxCommandFactory::DeleteLocationCommandLC(*path);
            
                command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, static_cast<TAny*>(this));
            
                iMedialist->CommandL(*command);
            
                CleanupStack::PopAndDestroy(command);
                CleanupStack::PopAndDestroy(path);
                }
            }       
        }
	}

// -----------------------------------------------------------------------------
// CGlxLocationBoundCommand::SetDetailsDailogObserver
// -----------------------------------------------------------------------------
//
void CGlxLocationBoundCommand::SetDetailsDailogObserver(MGlxMetadataDialogObserver& aObserver)
	{
	/** Error Id EMPY-7MKDHP **/
	iDialogObserver = &aObserver ;
	}

// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxAlbumsBoundCommand* CGlxAlbumsBoundCommand::NewL()
	{
	TRACER("CGlxAlbumsBoundCommand::NewL");
	CGlxAlbumsBoundCommand* self = CGlxAlbumsBoundCommand::NewLC();
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxAlbumsBoundCommand* CGlxAlbumsBoundCommand::NewLC()
	{
	TRACER("CGlxAlbumsBoundCommand::NewLC");
	CGlxAlbumsBoundCommand* self = new ( ELeave ) CGlxAlbumsBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}
	

// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::ConstructL
// ----------------------------------------------------------------------------
//
void CGlxAlbumsBoundCommand::ConstructL()
	{
	TRACER("CGlxAlbumsBoundCommand::ConstructL");
	TFileName uiutilitiesrscfile;
	uiutilitiesrscfile.Append(
			CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());
	//create command handler
	iAddToAlbum = CGlxCommandHandlerAddToContainer::NewL(this,
			EGlxCmdAddToAlbum, EFalse, uiutilitiesrscfile);
	}
	
// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::~CGlxAlbumsBoundCommand
// ----------------------------------------------------------------------------
//
CGlxAlbumsBoundCommand::~CGlxAlbumsBoundCommand()
    {
    if(iAddToAlbum)
        {
        delete iAddToAlbum;
        }
    }

// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxAlbumsBoundCommand::OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist)
	{
	TRACER("CGlxAlbumsBoundCommand::OfferCommandL");
	SetMediaList(&aMedialist);
	TBool consumed = EFalse;
	if ( aCommandId == KGlxEditBoundMenuCommandId )
		{
		iAddToAlbum->ExecuteL(EGlxCmdAddToAlbum);
		consumed = ETrue;
		}
	return consumed;
	}
	
// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxAlbumsBoundCommand::InitMenuL( CEikMenuPane& aMenu ) const
	{
	TRACER("CGlxAlbumsBoundCommand::InitMenuL");
	aMenu.SetItemTextL( KGlxEditBoundMenuCommandId, R_GLX_METADATA_VIEW_OPTIONS_EDIT ); 
	aMenu.SetItemDimmed( KGlxEditBoundMenuCommandId, EFalse );
	}

// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxAlbumsBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxAlbumsBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }

// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxAlbumsBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxAlbumsBoundCommand::ChangeMskL()
    {
    TRACER("CGlxAlbumsBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeySelect,R_GLX_METADATA_MSK_ADD);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxAlbumsBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}


// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::SetMediaList
// ----------------------------------------------------------------------------
//
void CGlxAlbumsBoundCommand::SetMediaList(MGlxMediaList* aMedialist)
	{
	TRACER("CGlxAlbumsBoundCommand::SetMediaList");
	iMedialist = aMedialist;
	}
	
// ----------------------------------------------------------------------------
// CGlxAlbumsBoundCommand::MediaList
// ----------------------------------------------------------------------------
//
 MGlxMediaList& CGlxAlbumsBoundCommand::MediaList()
	 {
	 TRACER("CGlxAlbumsBoundCommand::MediaList");
	 return *iMedialist;
	 }

// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxResolutionBoundCommand* CGlxResolutionBoundCommand::NewL()
	{
	TRACER("CGlxResolutionBoundCommand::NewL");
	CGlxResolutionBoundCommand* self = CGlxResolutionBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxResolutionBoundCommand* CGlxResolutionBoundCommand::NewLC()
	{
	TRACER("CGlxResolutionBoundCommand::NewLC");
	CGlxResolutionBoundCommand* self = new ( ELeave ) CGlxResolutionBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxResolutionBoundCommand::~CGlxResolutionBoundCommand()
    {
    
    }

// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxResolutionBoundCommand::OfferCommandL( TInt /*aCommandId*/ , MGlxMediaList& /*aMedialist*/ )
	{
	TRACER("CGlxResolutionBoundCommand::OfferCommandL");
	return ETrue;
	}
	
// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxResolutionBoundCommand::InitMenuL( CEikMenuPane& /*aMenu*/ ) const
	{
	// no implementation
	}

// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxResolutionBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxResolutionBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }

// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxResolutionBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxResolutionBoundCommand::ChangeMskL()
    {
    TRACER("CGlxResolutionBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeyEmpty,R_GLX_METADATA_MSK_BLANK);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxResolutionBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxResolutionBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}

// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::NewL(
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxUsageRightsBoundCommand* CGlxUsageRightsBoundCommand::NewL()
	{
	TRACER("CGlxUsageRightsBoundCommand::NewL");
	CGlxUsageRightsBoundCommand* self = CGlxUsageRightsBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxUsageRightsBoundCommand* CGlxUsageRightsBoundCommand::NewLC()
	{
	TRACER("CGlxUsageRightsBoundCommand::NewLC");
	CGlxUsageRightsBoundCommand* self = new ( ELeave ) CGlxUsageRightsBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxUsageRightsBoundCommand::~CGlxUsageRightsBoundCommand()
    {
    
    }

// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxUsageRightsBoundCommand::OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist)
	{
	TRACER("CGlxUsageRightsBoundCommand::OfferCommandL");
	SetMediaList(&aMedialist);
	TBool consumed = EFalse;
	if ( aCommandId == KGlxViewBoundMenuCommandId || aCommandId == KGlxEditBoundMenuCommandId )
		{
		LaunchDRMViewL();
		consumed = ETrue;
		}
	return consumed;
	}
	
// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxUsageRightsBoundCommand::InitMenuL( CEikMenuPane& aMenu ) const
	{
	TRACER("CGlxUsageRightsBoundCommand::InitMenuL");
	aMenu.SetItemTextL( KGlxViewBoundMenuCommandId, R_GLX_METADATA_VIEW_OPTIONS_VIEW ); 
	aMenu.SetItemDimmed( KGlxViewBoundMenuCommandId, EFalse );
	}

// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxUsageRightsBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxUsageRightsBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }

// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxUsageRightsBoundCommand::Close()
    {
    delete this;
    }	

// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxUsageRightsBoundCommand::ChangeMskL()
    {
    TRACER("CGlxUsageRightsBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeySelect,R_GLX_METADATA_MSK_BLANK);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxUsageRightsBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}

// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::SetMediaList
// ----------------------------------------------------------------------------
//
void CGlxUsageRightsBoundCommand::SetMediaList(MGlxMediaList* aMedialist)
	{
	TRACER("CGlxUsageRightsBoundCommand::SetMediaList");
	iMedialist = aMedialist;
	}
	
// ----------------------------------------------------------------------------
// CGlxUsageRightsBoundCommand::MediaList
// ----------------------------------------------------------------------------
//
 MGlxMediaList& CGlxUsageRightsBoundCommand::MediaList()
	 {
	 return *iMedialist;
	 }
	 
// -----------------------------------------------------------------------------
// LaunchDRMViewL
// -----------------------------------------------------------------------------
//		
void CGlxUsageRightsBoundCommand::LaunchDRMViewL()	
	{
	TRACER("CGlxUsageRightsBoundCommand::LaunchDRMViewL");
	CGlxDRMUtility* drmUtility = CGlxDRMUtility::InstanceL();
    CleanupClosePushL(*drmUtility);
    TInt count = MediaList().Count();
    if( count > 0)
    	{
    	const CGlxMedia* media = MediaList().Item(0).Properties();
    	const TDesC& uri = media->ValueText(KMPXMediaGeneralUri);
    	drmUtility->ShowDRMDetailsPaneL(uri);
    	}  
    CleanupStack::PopAndDestroy(drmUtility);  
	}
	
// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxDurationBoundCommand* CGlxDurationBoundCommand::NewL()
	{
	TRACER("CGlxDurationBoundCommand::NewL");
	CGlxDurationBoundCommand* self = CGlxDurationBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxDurationBoundCommand* CGlxDurationBoundCommand::NewLC()
	{
	TRACER("CGlxDurationBoundCommand::NewLC");
	CGlxDurationBoundCommand* self = new ( ELeave ) CGlxDurationBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxDurationBoundCommand::~CGlxDurationBoundCommand()
    {
    
    }
// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxDurationBoundCommand::OfferCommandL( TInt /*aCommandId*/ , MGlxMediaList& /*aMedialist*/ )
	{
	TRACER("CGlxDurationBoundCommand::OfferCommandL");
	return ETrue;
	}
	
// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxDurationBoundCommand::InitMenuL( CEikMenuPane& /*aMenu*/ ) const
	{
	// no implementation
	}

// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxDurationBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxDurationBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }
	
// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxDurationBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxDurationBoundCommand::ChangeMskL()
    {
    TRACER("CGlxDurationBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeyEmpty,R_GLX_METADATA_MSK_BLANK);

    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxDurationBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxDurationBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}


// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxDateAndTimeBoundCommand* CGlxDateAndTimeBoundCommand::NewL()
	{
	TRACER("CGlxDateAndTimeBoundCommand::NewL");
	CGlxDateAndTimeBoundCommand* self = CGlxDateAndTimeBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxDateAndTimeBoundCommand* CGlxDateAndTimeBoundCommand::NewLC()
	{
	TRACER("CGlxDateAndTimeBoundCommand::NewLC");
	CGlxDateAndTimeBoundCommand* self = new ( ELeave ) CGlxDateAndTimeBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxDateAndTimeBoundCommand::~CGlxDateAndTimeBoundCommand()
    {
    
    }
// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxDateAndTimeBoundCommand::OfferCommandL( TInt /*aCommandId*/ , MGlxMediaList& /*aMedialist*/ )
	{
	TRACER("CGlxDateAndTimeBoundCommand::OfferCommandL");
	return ETrue;
	}
	
// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxDateAndTimeBoundCommand::InitMenuL( CEikMenuPane& /*aMenu*/ ) const
	{
	// no implementation
	}

// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxDateAndTimeBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxDateAndTimeBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }
	
// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxDateAndTimeBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxDateAndTimeBoundCommand::ChangeMskL()
    {
    TRACER("CGlxDateAndTimeBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeyEmpty,R_GLX_METADATA_MSK_BLANK);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxDateAndTimeBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxDateAndTimeBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}

// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxFileSizeBoundCommand* CGlxFileSizeBoundCommand::NewL()
	{
	TRACER("CGlxFileSizeBoundCommand::NewL");
	CGlxFileSizeBoundCommand* self = CGlxFileSizeBoundCommand::NewLC();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::NewLC
// ----------------------------------------------------------------------------
//
CGlxFileSizeBoundCommand* CGlxFileSizeBoundCommand::NewLC()
	{
	TRACER("CGlxFileSizeBoundCommand::NewLC");
	CGlxFileSizeBoundCommand* self = new ( ELeave ) CGlxFileSizeBoundCommand;
	CleanupStack::PushL( self );
	return self;
	}

CGlxFileSizeBoundCommand::~CGlxFileSizeBoundCommand()
    {
    
    }

// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::OfferCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxFileSizeBoundCommand::OfferCommandL( TInt /*aCommandId*/ , MGlxMediaList& /*aMedialist*/ )
	{
	TRACER("CGlxFileSizeBoundCommand::OfferCommandL");
	return ETrue;
	}
	
// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::InitMenuL
// ----------------------------------------------------------------------------
//
void CGlxFileSizeBoundCommand::InitMenuL( CEikMenuPane& /*aMenu*/ ) const
	{
	// no implementation
	}

// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxFileSizeBoundCommand::HideLocationMenuItem( CEikMenuPane& aMenu ) const
    {
    TRACER("CGlxFileSizeBoundCommand::HideLocationMenuItem");
    aMenu.SetItemDimmed( EGlxCmdAiwBase, ETrue );
    }
	
// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::Close
// ----------------------------------------------------------------------------
//
void CGlxFileSizeBoundCommand::Close()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::ChangeMskL
// ----------------------------------------------------------------------------
//
void CGlxFileSizeBoundCommand::ChangeMskL()
    {
    TRACER("CGlxFileSizeBoundCommand::ChangeMskL");

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeyEmpty,R_GLX_METADATA_MSK_BLANK);
    if ( uiUtility )
        {
        uiUtility->Close();
        }
    }

// ----------------------------------------------------------------------------
// CGlxFileSizeBoundCommand::SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxFileSizeBoundCommand::SetDetailsDailogObserver( MGlxMetadataDialogObserver& /*aObserver*/ )
	{

	}


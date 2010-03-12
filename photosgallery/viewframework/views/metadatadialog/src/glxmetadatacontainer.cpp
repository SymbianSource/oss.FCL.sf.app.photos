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
* Description:    Implementation of Metadata dialog
*
*/

#include  "glxmetadatacontainer.h"
#include  "glxustringconverter.h"        		 // converts the symbian types to UString type
#include  <AknUtils.h>
#include  <StringLoader.h>
#include  <glxmetadatadialog.rsg>
#include  <glxviewbase.rsg>
#include  <glxlog.h>
#include  <glxtracer.h>
#include  <glxscreenfurniture.h>
#include  <glxdetailsmulmodelprovider.h>  		  //Details data provider
#include  <glxdetailsboundcommand.hrh>
#include  <glxcommandhandleraddtocontainer.h>     // For CGlxCommandHandlerAddToContainer
#include  <glxcommandhandlers.hrh>                // for command handler id
#include  <glxcommandfactory.h>                   //for command factory
#include  <mpxcommandgeneraldefs.h>               // Content ID identifying general category of content provided
#include  "mglxmetadatadialogobserver.h"
#include  <glxtextentrypopup.h>    
#include  <glxcollectionpluginall.hrh>
#include  <glxuistd.h>
#include  <glxcollectionplugintags.hrh>       	  // tag collection plugin uid
#include  <glxthumbnailattributeinfo.h>           // KGlxMediaIdThumbnail
#include  <glxattributeretriever.h>               // CGlxAttributeReteiver
#include  <aknQueryControl.h>
#include  <glxdrmutility.h>                       //For launching DRM details pane
#include  <glxgeneraluiutilities.h>               // General utilties class definition
#include  <ExifModify.h>    
#include  <glxuiutilities.rsg>                    //For CExifModify
#include  <mpxmediadrmdefs.h>
#include  <glxfilterfactory.h>
#include  <caf/manager.h>						  //For Filesystem

//marquee

#include <eikfrlb.h>
#include <eikfrlbd.h>
const TInt KMaxMediaPopupTitleLength = 0x100;
const TInt KMediaListId  			 = 0x2000D248;
const TInt KOffsets					 = 50;
const TInt KMarqueeLoopCount         = 3;
const TInt KMarqueeScrollAmount      = 20;
const TInt KMarqueeScrollDelay       = 1000000;
const TInt KMarqueeScrollInterval    = 200000;
_LIT( KGlxTextSetter, "");
_LIT( KGlxComma, ",");

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//	
CGlxMetadataContainer* CGlxMetadataContainer::NewL( const TRect& aRect,
                                                    MGlxMetadataDialogObserver& aDialogObserver,
                                                    const TDesC& item,MToolbarResetObserver& aResetToolbarObs)
	{
	TRACER("CGlxMetadataContainer::NewL");	
	CGlxMetadataContainer* self = CGlxMetadataContainer::NewLC( aRect,
                                                                aDialogObserver,item,aResetToolbarObs);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------
// NewLC
// ---------------------------------------------------------
//	
CGlxMetadataContainer* CGlxMetadataContainer::NewLC( const TRect& aRect,
													 MGlxMetadataDialogObserver&  aDialogObserver,
													 const TDesC& aUri,MToolbarResetObserver& aResetToolbarObs)
	{
	TRACER("CGlxMetadataContainer::NewLC");	
	CGlxMetadataContainer* self = new(ELeave) CGlxMetadataContainer(aDialogObserver,aUri,aResetToolbarObs);
	CleanupStack::PushL(self);
	self->ConstructL( aRect);
	return self;
	}

// ---------------------------------------------------------
// CGlxMetadataContainer
// ---------------------------------------------------------
//	
CGlxMetadataContainer::CGlxMetadataContainer(MGlxMetadataDialogObserver& aDialogObserver,
                                              const TDesC& aUri,MToolbarResetObserver& aResetToolbarObs)
					  :iDialogObesrver ( aDialogObserver ),iUri(aUri),iResetToolbarObs(aResetToolbarObs)
    {
    // No implementation
    }

// ---------------------------------------------------------
// CGlxMetadataContainer::ConstructL
// ---------------------------------------------------------
//	
void CGlxMetadataContainer::ConstructL( const TRect& /*aRect*/ )
	{	
	//Creating the RBuf texts for all the items except tags & albums
	//which would be updated as whne the item is edited
	iTextSetter.CreateL(KMaxFileName);
	
	//RBuf text which would be updated as when a tag is edited for the item.
	iTagSetter.CreateL(KMaxFileName);
	
	//RBuf text which would be updated as when a album is edited for the item.
	iAlbumSetter.CreateL(KMaxFileName);
	
	//Create medialist filtered by uri - iUri
	CreateMediaListForSelectedItemL();
	
	//Setting the iVideo flag to EFalse initially	
	iVideo = EFalse;
	
	//Setting the iMarquee flag to EFalse initially
    iMarquee = EFalse;
    
    //check when Remove location information is selected.
    iLocationinfo = EFalse;
    
	//Creating a CGlxMetadataAsyncUpdate object
	iAsyncRequest = CGlxMetadataAsyncUpdate::NewL(*this);
	
	//Initializing to NULL
	iModifiedUri = NULL;	
	
	//Flag to check if rename is completed successfully.
	iRenameCompleted = EFalse;
	}

// ---------------------------------------------------------
// ~CGlxMetadataContainer
// ---------------------------------------------------------
//	
CGlxMetadataContainer::~CGlxMetadataContainer()
	{
	TRACER("CGlxMetadataContainer::~CGlxMetadataContainer");
	if( iItemMediaList ) 
		{
		iItemMediaList->RemoveContext(iMainListAttributecontext);
		iItemMediaList->RemoveMediaListObserver(this);
		iItemMediaList->Close();
		iItemMediaList = NULL;  
		} 
    if ( iTagMediaList ) 
		{
		iTagMediaList->RemoveContext(iTagContext);
		iTagMediaList->RemoveMediaListObserver(this); 
		iTagMediaList->Close();
		iTagMediaList = NULL;
		}
    if ( iAlbumMediaList ) 
		{
		iAlbumMediaList->RemoveContext(iAlbumContext);
		iAlbumMediaList->RemoveMediaListObserver(this); 
		iAlbumMediaList->Close();
		iAlbumMediaList = NULL;
		}
    if( iTagContext )
		{
        delete iTagContext;
        iTagContext = NULL;
		}    
    if( iAlbumContext )
		{
        delete iAlbumContext;
        iAlbumContext = NULL;
		}
    if( iMainListAttributecontext )
        {
        delete iMainListAttributecontext;
        iMainListAttributecontext = NULL;
        }       
    if( IsVisible() )
	    {
	    MakeVisible(EFalse);
	    }
    iTextSetter.Close();
    iTagSetter.Close();
    iAlbumSetter.Close();
    if(iAsyncRequest) 	
	    {
    	delete iAsyncRequest;
	    }
	if(iModifiedUri)
		{
		delete iModifiedUri;
		}
	}
	
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::MediaList
//-----------------------------------------------------------------------------
MGlxMediaList& CGlxMetadataContainer::MediaList()
    {
    //returns the active medialist.
    return *iItemMediaList;    
    }
    
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::CreateSettingItemL
//-----------------------------------------------------------------------------
CAknSettingItem* CGlxMetadataContainer::CreateSettingItemL(TInt aResourceId)
    {
    TRACER("CGlxMetadataContainer::CreateSettingItemL");      
    CAknSettingItem* settingItem = NULL; // No need to push onto cleanup stack
    iTextSetter.Copy(KGlxTextSetter);  
    
    //Creating a empty Settings list box which will  be populated with metadata in handleattributeavailable
    
    switch(aResourceId)
        {
        case ENameItem:
        case EDateAndTimeItem:
        case EDescriptionItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem( 
                                             aResourceId, iTextSetter );

            break;          
            }
        case ETagsItem:
            {
             iTagSetter.Copy(KGlxTextSetter);
             settingItem = new (ELeave) CAknTextSettingItem(
                                                             aResourceId, iTagSetter );
            break;          
            }
        case EAlbumsItem:
            {            
            iAlbumSetter.Copy(KGlxTextSetter);
            settingItem = new (ELeave) CAknTextSettingItem( 
                                                  aResourceId, iAlbumSetter );
            break;          
            }
        case ELocationItem:
        case ESizeItem:            
        case EResolutionItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem( 
                                                  aResourceId, iTextSetter );
            
            break;          
            }            
        case EDurationItem:
        case ElicenseItem:
            {
              settingItem = new (ELeave) CAknTextSettingItem( 
                                                         aResourceId, iTextSetter );
							//Hide the item until we get the attributes
							//where in we check for the usage rights.                                                                                                            
              settingItem->SetHidden(ETrue);
              //Required to refresh the listbox when any items visiblity is changed
              this->HandleChangeInItemArrayOrVisibilityL();
             }
            break;
           
        default:
            {
            break;    
            }
        }
    return settingItem;
    }
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::IsItemModifiable
//-----------------------------------------------------------------------------
TBool CGlxMetadataContainer::IsItemModifiable()
    {
    //Only items like name , description, tag and albums are modifiable
    //The check is for the items from ENameItem(0) tille ETagsItem(4)
    if(ListBox()->CurrentItemIndex()<=ETagsItem)
        {
        return EFalse;
        }
    //return ETrue to dim the item
    return ETrue;
    
    }
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::IsLicenseItem
//-----------------------------------------------------------------------------
TBool CGlxMetadataContainer::IsLicenseItem()
    {
    //Checks the item for DRMProtection.
    //if item is a video item index should be ELicense else check for EDuration Item.
    //because License Item index would become EDuration as the duration item is hidden in case of inage file.
    if((!iVideo && ListBox()->CurrentItemIndex()== EDurationItem) 
            || (ListBox()->CurrentItemIndex()== ElicenseItem))
        {
        return EFalse;
        }
    return ETrue;
    }
    
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::IsLocationItem
//-----------------------------------------------------------------------------
TBool CGlxMetadataContainer::IsLocationItem()
    {
    //if its location item - enable the delete option
    if (iItemMediaList->Count() && ListBox()->CurrentItemIndex()
            == ELocationItem)
        {
        const TGlxMedia& item = iItemMediaList->Item(0);
        TCoordinate coordinate;
        if (item.GetCoordinate(coordinate))
            {
            return EFalse;
            }
        return ETrue;
        }
    //return ETrue to dim the item
    return ETrue;
    }
    
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::HandleListBoxEventL
//-----------------------------------------------------------------------------
void CGlxMetadataContainer::HandleListBoxEventL(CEikListBox*  /*aListBox*/,
                                                    TListBoxEvent aEventType)
    {
    GLX_LOG_INFO("CGlxMetadataContainer::HandleListBoxEventL");         
    if ((aEventType == EEventEnterKeyPressed) || 
        (aEventType == EEventEditingStarted) ||
        (aEventType == EEventItemSingleClicked))
        {
        //handle edit functionality if items when useer selects via touch
        HandleListboxChangesL();
        }
    }
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::HandleListboxChangesL
//-----------------------------------------------------------------------------
void CGlxMetadataContainer::HandleListboxChangesL()
    {
    if(iItemMediaList->Count() == 0)
    	{
    	return;
    	}
    
    TInt index = ListBox()->CurrentItemIndex();
    
    switch(index)
        {
        case ENameItem:        
        case EDescriptionItem:
            {
            SetNameDescriptionL(index);
            break;
            }
        case ETagsItem:
			{
			//Set the focus of the item	
			iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute,0);
			//Launch add to container commandhandler via dialog observer.
			iDialogObesrver.AddTagL();
			break;         
			}
        case EAlbumsItem:
			{
			//Set the focus of the item	
			iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute,0);
			//Launch add to container commandhandler via dialog observer.
			iDialogObesrver.AddAlbumL();
			break;          
			}
		case ELocationItem:
			{
			// Get the Media Item
			const TGlxMedia& media = iItemMediaList->Item(0);    
			// Test to see if the Coordinate is Present
			TCoordinate coordinate;
			if( !media.GetCoordinate(coordinate) )
				{
				HBufC *noLocationBuf = StringLoader::LoadLC(R_GLX_METADATA_NOTE_INFO_NO_LOCATION);
				GlxGeneralUiUtilities::ShowInfoNoteL(*noLocationBuf,ETrue);
				CleanupStack::PopAndDestroy(noLocationBuf);
				}
			break;
			}
        case EDurationItem:
			{
			//This is condition is useful when the license item is selected for a image file
			if(iVideo)
				{
				break;
				}
			}
        case ElicenseItem:
			{
			//Create DRM utility
			CGlxDRMUtility* drmUtility = CGlxDRMUtility::InstanceL();
			CleanupClosePushL(*drmUtility);
			drmUtility->ShowDRMDetailsPaneL(iItemMediaList->Item(0).Uri());
			CleanupStack::PopAndDestroy(drmUtility);
			}  
			break;
        default:
			{
			break;    
			}
        }
    }
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::CreateMediaListForSelectedItemL
//-----------------------------------------------------------------------------
void CGlxMetadataContainer::CreateMediaListForSelectedItemL(TBool aIsRename)
    {
    TRACER("CGlxMetadataContainer::CreateMediaListForSelectedItemL");
   
    //create the collection path for the medialist to be created
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    //set the all collection path as the details dialog can be launched from any of the grid views and filter with URI
    path->AppendL(KGlxCollectionPluginAllImplementationUid);
    //create the filter with the URI
    CMPXFilter* filter = NULL;
    if(aIsRename)
	    {
	    //create the filter with the modified URI after Rename happens
	    filter = TGlxFilterFactory::CreateURIFilterL(iModifiedUri->Des());
	    }
	else
		{
		filter  = TGlxFilterFactory::CreateURIFilterL(iUri);
		}
    CleanupStack::PushL( filter );
    //create the medialist   
    iItemMediaList = MGlxMediaList::InstanceL(*path,TGlxHierarchyId(KMediaListId),filter);   
  
    //Add the attributes which are required to be displayed.
    iMainListAttributecontext = new (ELeave) CGlxAttributeContext(&iSelectionIterator);
    iMainListAttributecontext->AddAttributeL(KMPXMediaDrmProtected);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralCategory);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralSize);
    iMainListAttributecontext->AddAttributeL(KGlxMediaGeneralDimensions); 
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralTitle);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralDate);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralComment); 
    iMainListAttributecontext->AddAttributeL(KGlxMediaGeneralLocation);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralDuration);    
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralUri);
    
    //Add Context so that we get the handleattributes call once the medialist is populated with above mentioned attributes.
    iItemMediaList->AddContextL( iMainListAttributecontext,
            KGlxFetchContextPriorityBlocking );
    
    //add to observer for callbacks.
    iItemMediaList->AddMediaListObserverL(this);
    
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( path ); 
    }

//-----------------------------------------------------------------------------
// CGlxMetadataContainer::CreateTagsMediaList
//-----------------------------------------------------------------------------
void CGlxMetadataContainer::CreateTagsMediaListL()
    {
    TRACER("CGlxMetadataContainer::CreateTagsMediaListL");
    	//create the collection path for the medialist to be created
      CMPXCollectionPath* path = CMPXCollectionPath::NewL();
      CleanupStack::PushL( path );
      //Set the Tags collection for the particular item
      path->AppendL(KGlxTagCollectionPluginImplementationUid);
      //get the media item for which we require the tags collection
      TGlxMedia item = iItemMediaList->Item(0);            
      //create the medialist filter with media ID
      CMPXFilter* filter1  = 
              TGlxFilterFactory::CreateExcludeContainersWithoutItemFilterL(item.Id());
      CleanupStack::PushL(filter1);
      //set the array order as required, here its alphabetical
      TGlxFilterProperties filterProperty;
      filterProperty.iSortOrder = EGlxFilterSortOrderAlphabetical;
      filterProperty.iSortDirection = EGlxFilterSortDirectionAscending;
      //combine the filter with filterProperty
      CMPXFilter* filter  = TGlxFilterFactory::CreateCombinedFilterL( filterProperty,
                                                                      filter1,
                                                                      EFalse);
      CleanupStack::PushL(filter); 
      
      //create the medialist to get the tags array.
      iTagMediaList = MGlxMediaList::InstanceL(*path,TGlxHierarchyId(KMediaListId),filter);
      
      //add the attribute KMPXMediaGeneralTitle to the context to get the tag name
      iTagContext = CGlxDefaultAttributeContext::NewL();
      iTagContext->AddAttributeL(KMPXMediaGeneralTitle); 
      iTagContext->SetRangeOffsets(KOffsets,KOffsets);
      
      //Add Context so that we get the handleattributes call once the medialist is populated with tags.
      iTagMediaList->AddContextL( iTagContext,
                                      KGlxFetchContextPriorityUMPViewTagPane );
                                      
			//add to observer for callbacks.                                      
      iTagMediaList->AddMediaListObserverL(this);
      
      CleanupStack::PopAndDestroy(filter);
      CleanupStack::PopAndDestroy(filter1);
      CleanupStack::PopAndDestroy(path);
  
    }
//-----------------------------------------------------------------------------
// CGlxMetadataContainer::CreateAlbumsMediaListL
//-----------------------------------------------------------------------------
void CGlxMetadataContainer::CreateAlbumsMediaListL()
    {
    TRACER("CGlxMetadataContainer::CreateAlbumsMediaListL");
    	//create the collection path for the medialist to be created
      CMPXCollectionPath* path = CMPXCollectionPath::NewL();
      CleanupStack::PushL( path );
      //Set the albums collection for the particular item
      path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
      //get the media item for which we require the tags collection
      TGlxMedia item = iItemMediaList->Item(0);     
      //create the medialist filter with media ID
      CMPXFilter* filter  =
              TGlxFilterFactory::CreateExcludeContainersWithoutItemFilterL(item.Id());
      CleanupStack::PushL(filter);    
      //create the albums medialist.
      iAlbumMediaList = MGlxMediaList::InstanceL(*path,
                                                  TGlxHierarchyId(KMediaListId),
                                                  filter);
                                                  
      //add the attribute KMPXMediaGeneralTitle to the context to get the album name
      iAlbumContext = CGlxDefaultAttributeContext::NewL();
      iAlbumContext->AddAttributeL(KMPXMediaGeneralTitle);
      iAlbumContext->SetRangeOffsets(KOffsets,KOffsets);
      //Add Context to the medialist so that we get the handleattributes call once the medialist is populated with albums.
      iAlbumMediaList->AddContextL( iAlbumContext, 
                              KGlxFetchContextPriorityUMPViewAlbumPane );
      
      //add to observer for callbacks. 
      iAlbumMediaList->AddMediaListObserverL(this);        
       
      CleanupStack::PopAndDestroy(filter);   
      CleanupStack::PopAndDestroy(path); 
  
    }


// ----------------------------------------------------------------------------
// CGlxMetadataContainer::ViewDynInitMenuPaneL
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::ViewDynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
    {
    if( aMenuId == R_METADATA_MENU )
        {
        //Set dim the options based on the utem selected
        //Viewdetails option will be availble only for the license item
        //Delete option will be available only for the location item	
        aMenuPane->SetItemDimmed(KGlxViewBoundMenuCommandId,IsLicenseItem());
        //location info will be enabled if the item has a location info
		aMenuPane->SetItemDimmed(KGlxDeleteBoundMenuCommandId,IsLocationItem());
        aMenuPane->SetItemDimmed(EGlxCmdAiwShowMap,IsLocationItem());
        }
        
    
    }
// ----------------------------------------------------------------------------
// CGlxMetadataContainer::RemoveLocationL
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::RemoveLocationL() 
    {
    TRACER("CGlxMetadataContainer::RemoveLocationL");    
    
    iLocationinfo = ETrue;
    // get the media item
    const TGlxMedia& media = iItemMediaList->Item(0);    
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
            if( iItemMediaList->Count() > 0 )
                {       
                //set focus to first item	
                iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute,0);
                
                // Deleting location information from image file
                RFs rFs;
                User::LeaveIfError( rFs.Connect() );
                CleanupClosePushL( rFs );
                RFile rFile;                
                User::LeaveIfError( rFile.Open(rFs,
                                               iItemMediaList->Item(0).Uri()
                                               ,EFileWrite ));
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
                CExifModify* exifWriter = NULL;
                TRAPD(err,exifWriter = CExifModify::NewL( imageData->Des()));
                CleanupStack::PushL( exifWriter );
                if(err == KErrNone)
                    {
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
                    }
                CleanupStack::PopAndDestroy( exifWriter);
                CleanupStack::PopAndDestroy( imageData );
                CleanupStack::PopAndDestroy( &rFile );
                CleanupStack::PopAndDestroy( &rFs );

                //Deleting location information from MDS database
                CMPXCollectionPath* path =  iItemMediaList->PathLC();
                CMPXCommand* command = TGlxCommandFactory::DeleteLocationCommandLC(*path);
            
                command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId,
                                                 static_cast<TAny*>(this));
            
                iItemMediaList->CommandL(*command);            
                CleanupStack::PopAndDestroy(command);
                CleanupStack::PopAndDestroy(path);
                }
            }       
        }
    }
// ----------------------------------------------------------------------------
// CGlxMetadataContainer::EditItemL
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::EditItemL(TInt aIndex, TBool /*aCalledFromMenu*/)
    {
	TRACER("CGlxMetadataContainer::EditItemL");   
    CAknSettingItem* settingsitem = NULL;
    settingsitem = (*SettingItemArray())[aIndex];
    settingsitem->LoadL();
    settingsitem->UpdateListBoxTextL();
    settingsitem->StoreL();
    ListBox()->DrawNow();
    }
// ----------------------------------------------------------------------------
// CGlxMetadataContainer::SetAttributes
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::SetAttributesL(TMPXAttribute attribute)
	{
    TRACER("CGlxMetadataContainer::SetAttributesL");    
    
    //create the tags and albums medialist once the item medialist is populated
    //Tags and albums medialist can be created only with media ID.
    if(!iTagMediaList)
		{
		CreateTagsMediaListL();
		}		
    if(!iAlbumMediaList)
		{
		CreateAlbumsMediaListL();	
		}
    if(!iSetVisible)
		{
		iSetVisible = ETrue;
		SetDurationLIicenseItemVisibilityL();
		}
    TGlxMedia item = iItemMediaList->Item(0);
    //Create the string convertor instance 
    //String convertor class with provide the specific format for date,location and duration and size.
    CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
    CleanupStack::PushL(stringConverter );
    HBufC* string = NULL;    
    
    //if attribute is date and time we need to pass the format it as R_QTN_DATE_USUAL_WITH_ZERO else null
    if(attribute == KMPXMediaGeneralDate)
        {
        stringConverter->AsStringL(item, 
                                   attribute,
                                   R_QTN_DATE_USUAL_WITH_ZERO, string );
        }    
    else
        {
        stringConverter->AsStringL(item,
                                   attribute,0, string );
        }
    //get the settings item based on the attribute and set the text.
    if ( string )
		{
		iTextSetter.Copy(KGlxTextSetter);
		iTextSetter.Append(*string);
		if(attribute == KMPXMediaGeneralSize)
			{
			EditItemL(ESizeItem,EFalse);
			}
		else if(attribute == KMPXMediaGeneralDuration)
			{
			EditItemL(EDurationItem,EFalse);
			}
		else if(attribute == KMPXMediaGeneralTitle)
			{
			EditItemL(ENameItem,EFalse);
			}
		else if(attribute == KMPXMediaGeneralDate)
			{  
			EditItemL(EDateAndTimeItem,EFalse);
			}
		else if(attribute == KMPXMediaGeneralComment)
			{
			EditItemL(EDescriptionItem,EFalse);
			}
		else if(attribute == KGlxMediaGeneralLocation)
			{
			EditItemL(ELocationItem,EFalse);
			}
		else if(attribute == KGlxMediaGeneralDimensions)
			{
			EditItemL(EResolutionItem,EFalse);
			}    
		else
			{
    
			} 
		delete string;
		string = NULL;
		}
    CleanupStack::PopAndDestroy(stringConverter );
	}

// ----------------------------------------------------------------------------
// CGlxMetadataContainer::SetNameDescriptionL
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::SetNameDescriptionL(TInt aItem)
    {
    TRACER("CGlxMetadataContainer::SetNameDescriptionL");   
    //This functions i commn for updatng both name and description once modified    
    //get the item handcle to be modified    	
    CAknSettingItem* settingsitem = (*SettingItemArray())[aItem]; 
    HBufC* textBuf = HBufC::NewLC( KMaxMediaPopupTitleLength );
    (textBuf->Des()).Copy((settingsitem->SettingTextL()));
    TPtr textPtr = textBuf->Des();
    TBuf<KMaxMediaPopupTitleLength> titleText(*textBuf);
    HBufC *buf = NULL;
    if(aItem == ENameItem)
        {
        buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_TITLE_NSERIES);
        }
    else
        {
        buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_DESCRIPTION_NSERIES);
        }
    
    //Launch the text entry editor.
    CGlxTextEntryPopup* popup = CGlxTextEntryPopup::NewL( *buf, textPtr );
    CleanupStack::PopAndDestroy(buf);
    if(aItem == EDescriptionItem)
        {
        popup->SetLeftSoftKeyL(ETrue);
        }
    
    //action upon selecting ok from the editor 
    if ( popup->ExecuteLD() == EEikBidOk )
		{
		if(0 != (titleText.Compare(*textBuf)))
			{
			if ((aItem == ENameItem) &&
					(BaflUtils::FileExists(ControlEnv()->FsSession(), ParseFileName(*textBuf))))
				{
				//if changed title is same as existing one then showing the already use popup to user
				HBufC* info = StringLoader::LoadLC(R_GLX_NAME_ALREADY_USED, *textBuf);
				GlxGeneralUiUtilities::ShowInfoNoteL(*info, ETrue);
				CleanupStack::PopAndDestroy(info);
				}
			else
				{
				//Modify the MDS and setting list only if the entry is different from previous Item value
				iTextSetter.Copy(*textBuf);
				EditItemL(aItem,EFalse);
				iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute,0);//set focus to first item
				CMPXCollectionPath* path = iItemMediaList->PathLC();
				CMPXCommand* command = NULL;
				//Create the glx command based on the item
				if(aItem == ENameItem)
					{
					command = TGlxCommandFactory::RenameCommandLC(settingsitem->SettingTextL(),
					                                                  *path);
					}
				else
					{
					command = TGlxCommandFactory::SetDescriptionCommandLC(settingsitem->SettingTextL(),
					                                          *path);
					}
				command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, static_cast<TAny*>(this));
				//issue command to the medialist which further calls data source to update MDS
				iItemMediaList->CommandL(*command);
				CleanupStack::PopAndDestroy(command);
				CleanupStack::PopAndDestroy(path);
				}
			}
		}
    CleanupStack::PopAndDestroy( textBuf );
    
    //notify observer that some operation has happened. So refresh the toolbar area..
    iResetToolbarObs.HandleToolbarResetting(EFalse);
    }
// ----------------------------------------------------------------------------
// CGlxMetadataContainer::UpdateTagsL()
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::UpdateTagsL()
    {
    //Get the tag setting item handle to set the text
     CAknSettingItem* settingsitem =
                             (*SettingItemArray())[ETagsItem]; 
                             //Set the tag setter to empty string before filling in the data.                                                
     iTagSetter.Copy(KGlxTextSetter);
     //Loop to appened all the tags to the iTagSetter.
     for( TInt index = 0 ; index < iTagMediaList->Count() ; index++ )
		{
		if(iTagSetter.Length())
			{
			iTagSetter.Append(KGlxComma);
			}
		const TGlxMedia&  item = iTagMediaList->Item( index );
		const TDesC& title = item.Title();
		iTagSetter.Append(title);
		}    
     EditItemL(ETagsItem,EFalse);
     }
// ----------------------------------------------------------------------------
// CGlxMetadataContainer::UpdateAlbumsL()
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::UpdateAlbumsL()
    {
    //Get the tag setting item handle to set the text
    CAknSettingItem* settingsitem = 
                        (*SettingItemArray())[EAlbumsItem];     
    //Set the tag setter to empty string before filling in the data.
    iAlbumSetter.Copy(KGlxTextSetter);
    //Loop to appened all the tags to the iAlbumSetter.
    for( TInt index = 0 ; index < iAlbumMediaList->Count() ; index++ )
		{
        if(iAlbumSetter.Length())
			{
			iAlbumSetter.Append(KGlxComma);
			}
		const TGlxMedia&  item = iAlbumMediaList->Item( index );
		const TDesC& title = item.Title();
		iAlbumSetter.Append(title);
		}
	EditItemL(EAlbumsItem,EFalse);
    }
// ----------------------------------------------------------------------------
// CGlxMetadataContainer::SetDurationLIicenseItemVisibilityL()
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::SetDurationLIicenseItemVisibilityL()
    {
    //get the media item.
    const TGlxMedia& item = iItemMediaList->Item(0);
    const CGlxMedia* media = item.Properties();

    //in order to check for video category and drm rights
   
    CAknSettingItem* hiddenItem = NULL;
    if( item.Category() == EMPXVideo)
        {  
        if(!item.IsDrmProtected())
	        {
	        hiddenItem = (*SettingItemArray())[EDurationItem];        
	        //Set the duration item visible
	        hiddenItem->SetHidden(EFalse);         
	        }              
        //set the video flag which would be used to enable/disable the view details option.               
        this->HandleChangeInItemArrayOrVisibilityL();
        iVideo = ETrue;
        }    
     if( media && media->IsSupported(KMPXMediaDrmProtected))
        {
		if(item.IsDrmProtected())
			{
			hiddenItem = (*SettingItemArray())[ElicenseItem];
			//Set the License item visible
			hiddenItem->SetHidden(EFalse);             
			//Required to refresh the listbox when any items visiblity is changed
			this->HandleChangeInItemArrayOrVisibilityL();
			}
        }
    }   
//Medialist callbacks.    
// ----------------------------------------------------------------------------
// CGlxMetadataContainer::HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::HandleAttributesAvailableL( TInt /*aItemIndex*/, 
    const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList )
    {
    TRACER("CGlxMetadataContainer::HandleAttributesAvailableL()");
    //generic medialist for the item for all the attributes required other than tags and albums.
    if(aList == iItemMediaList)
        {
        // Loop untill it checks for all the avialable attributes
        for (TInt i = aAttributes.Count() - 1; i >= 0; i--)
            {
            //set attributes to the items in the container
            SetAttributesL(aAttributes[i]);
            }
        if (iRenameCompleted)
            {
            iRenameCompleted = EFalse;
            iAvkonAppUi->ProcessCommandL(EGlxCmdRenameCompleted);
            }
        }
    //updation of tags and albums list based on the medialist callback.
    if(aList == iTagMediaList ||  aList == iAlbumMediaList)
        {
        for (TInt i = aAttributes.Count() - 1; i >= 0 ; i--)
            {
            TMPXAttribute titleAtrribute (KMPXMediaGeneralTitle);
            for (TInt i = aAttributes.Count() - 1; i >= 0 ; i--)
                { 
                if( titleAtrribute == aAttributes[i] )
                    {
                    if(aList == iTagMediaList)
						{     
						UpdateTagsL();
						}
					else if(aList == iAlbumMediaList)
						{
						UpdateAlbumsL();
						}          
                    }           
                }
          
            }
        }
    
    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
     MGlxMediaList* aList )
    {
    TRACER("CGlxMetadataContainer::HandleItemAddedL()");
    
    if(!iTagMediaList)
		{
        CreateTagsMediaListL();
		}       
    if(!iAlbumMediaList)
		{
        CreateAlbumsMediaListL();   
		}
    if(!iMarquee)
		{
        EnableMarqueingL();
		}
    SetDurationLIicenseItemVisibilityL();
    if(aList == iTagMediaList)
		{     
		UpdateTagsL();
		}
	else if(aList == iAlbumMediaList)
		{
		UpdateAlbumsL();
		}
    if(aList == iItemMediaList)
        {
        if(iItemMediaList->Count())
			{
			TGlxMedia item = iItemMediaList->Item(0);
			CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
			CleanupStack::PushL(stringConverter );
			for(TInt index = 0; index <= ElicenseItem ; index++)
				{
				HBufC* string = NULL;               
  
				if(index == ESizeItem)
					{
					stringConverter->AsStringL(item,
							KMPXMediaGeneralSize,0, string );              
					}
				else if(index == EDurationItem)
					{
					stringConverter->AsStringL(item, 
							KMPXMediaGeneralDuration,0, string );
					}
				else if(index == ENameItem)
					{
					stringConverter->AsStringL(item,
							KMPXMediaGeneralTitle,0, string );
					}
				else if(index == EDateAndTimeItem)
					{  
					stringConverter->AsStringL( item, 
							KMPXMediaGeneralDate,
							R_QTN_DATE_USUAL_WITH_ZERO,string );
					}
				else if(index == EDescriptionItem)
					{
					stringConverter->AsStringL(item,
							KMPXMediaGeneralComment,0, string ); 
					}
				else if(index == ELocationItem)
					{
					stringConverter->AsStringL(item,
							KGlxMediaGeneralLocation,0, string );
					}
				else if(index == EResolutionItem)
					{
					stringConverter->AsStringL(item,
							KGlxMediaGeneralDimensions,0, string );
					}    
				else if(index == ElicenseItem)
					{
					// If an item is DRM protected, License field in details
					// should display "View Details"
					string = StringLoader::LoadL(R_GLX_METADATA_VIEW_OPTIONS_VIEW);					
					} 				
				else
					{
					//no implementation
					} 
				if(string)
					{
					iTextSetter.Copy(KGlxTextSetter);
					iTextSetter.Append(*string);
					}	
				CleanupStack::PushL( string );
				EditItemL(index,EFalse);
				CleanupStack::PopAndDestroy(string );
				}
			CleanupStack::PopAndDestroy(stringConverter );
			}   
        }
    }
// ----------------------------------------------------------------------------
// EnableMarqueingL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::EnableMarqueingL()
    {
    TRACER("CGlxMetadataContainer::EnableMarqueingL()");    
    iMarquee = ETrue;
    ListBox()->UseLogicalToVisualConversion(ETrue);
    ListBox()->ItemDrawer()->ColumnData()->SetMarqueeParams (KMarqueeLoopCount,
            KMarqueeScrollAmount, KMarqueeScrollDelay, KMarqueeScrollInterval);
    ListBox()->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    }    
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::HandleCommandCompleteL(TAny* aSessionId, 
        CMPXCommand* /*aCommandResult*/, TInt aError, MGlxMediaList* aList)
    {
    TRACER("CGlxMetadataContainer::HandleCommandCompleteL()");
    //To rename the uri in File System
    if(aError == KErrNone)
		{
		TInt index = ListBox()->CurrentItemIndex();
		if(aList == iItemMediaList && index == ENameItem)
			{
			iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute,0);
			const TGlxMedia& media = iItemMediaList->Item(0);
			ContentAccess::CManager *manager = ContentAccess::CManager::NewL();
			CleanupStack::PushL(manager);
			TFileName fileName = ParseFileName(iTextSetter);
			if(iModifiedUri)
				{
				delete iModifiedUri;
				iModifiedUri = NULL;	
				}			
			iModifiedUri = fileName.AllocL();			
			iTextSetter.Copy(KGlxTextSetter);
			TInt error = manager->RenameFile(media.Uri(), iModifiedUri->Des());
			if(error == KErrNone)
				{
				iAsyncRequest->CompleteSelf();	
				}
			else
				{
				User::LeaveIfError(error);
				}		
			CleanupStack::PopAndDestroy(manager);
			}
		}
    //To update the location information once the delete operation is successful.
    if(aList == iItemMediaList && iLocationinfo 
            && static_cast<TAny*>( this ) == aSessionId)
		{
		TGlxMedia media = iItemMediaList->Item(0) ;
		media.DeleteLocationAttribute();
		iLocationinfo = EFalse;      
		if ( aError == KErrNone )
			{
			iTextSetter.Copy(KGlxTextSetter);
			EditItemL(ELocationItem,EFalse);
			}
		}
	}
          
// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleItemRemovedL(  TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/  )
    {
    TRACER("CGlxMetadataContainer::HandleItemRemovedL()");    
    }    
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleFocusChangedL( NGlxListDefs::
    TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/, 
    MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMetadataContainer::HandleFocusChangedL()");
    }
// ----------------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleItemSelectedL(TInt /*aIndex*/, 
    TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMetadataContainer::HandleItemSelectedL");
    }
// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
void CGlxMetadataContainer::HandleMessageL( const CMPXMessage& /*aMessage*/, 
    MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMetadataContainer::HandleMessageL()");
    }
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::HandleError( TInt /*aError*/ ) 
    {
    TRACER("CGlxMetadataContainer::HandleError()");
    TRAP_IGNORE(HandleErrorL());
    }

// ----------------------------------------------------------------------------
// HandleErrorL
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::HandleErrorL()
    {
    TRACER("CGlxMetadataContainer::HandleErrorL()");
    }
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, 
    TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMetadataContainer::HandleCommandCompleteL()");
    }
  
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMetadataContainer::HandleMediaL()");
    }
  
// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/,
    MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMetadataContainer::HandleItemModifiedL()");
    }
// ----------------------------------------------------------------------------
// ChangeMskL
// ----------------------------------------------------------------------------
//     
void CGlxMetadataContainer::ChangeMskL()
	{
	TRACER("CGlxMetadataContainer::ChangeMsk()");
	TInt index = ListBox()->CurrentItemIndex();
	CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();    	        
    switch(index)
        {
        case ENameItem:        
        case EDescriptionItem:
       	case ETagsItem:
        case EAlbumsItem:        
            {
            uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeyEdit,R_GLX_METADATA_MSK_EDIT);
            }        
        	break;        	
        case EDateAndTimeItem:
        case ELocationItem:
        case ESizeItem:
        case EDurationItem:
        case ElicenseItem:
        case EResolutionItem:
	        {
	        uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
            EAknSoftkeyEdit,R_GLX_METADATA_MSK_BLANK);            
	        }        
        	break;
        default:
	        {
	        break;    
	        }
        }
        
	if ( uiUtility )
		{
		uiUtility->Close();
		}	
	}
	
// ---------------------------------------------------------------------------
// Parse the drive, path & extension from the old uri,
// And return the modified uri by appending the new title
// ---------------------------------------------------------------------------
TFileName CGlxMetadataContainer::ParseFileName(const TDesC& aTitleText)
	{
	TRACER("CGlxMetadataContainer::ParseFileName()");
	const TGlxMedia& media = iItemMediaList->Item(0);
	TParsePtrC parsePtr(media.Uri());

	TFileName destinationFileName;
	destinationFileName.Append(parsePtr.DriveAndPath());
	destinationFileName.Append(aTitleText);
	destinationFileName.Append(parsePtr.Ext());

	return destinationFileName;
	}

// ---------------------------------------------------------------------------
// Create new MediaList with modified FileName.
// ---------------------------------------------------------------------------
void CGlxMetadataContainer::RefreshMediaListL()
	{
    TRACER("CGlxMetadataContainer::RefreshMediaList()");
    if (iItemMediaList)
        {
        iItemMediaList->RemoveContext(iMainListAttributecontext);
        iItemMediaList->RemoveMediaListObserver(this);
        iItemMediaList->Close();
        iItemMediaList = NULL;
        if (iMainListAttributecontext)
            {
            delete iMainListAttributecontext;
            iMainListAttributecontext = NULL;
            }
        CreateMediaListForSelectedItemL(ETrue);
        }
    iRenameCompleted = ETrue;
    }

// ---------------------------------------------------------------------------
// Completes the active object causing a call from the active scheduler to RunL()
// ---------------------------------------------------------------------------
//
void CGlxMetadataAsyncUpdate::CompleteSelf()
    {
    TRACER("CGlxMetadataAsyncUpdate::CompleteSelf");
    TRequestStatus* status=&iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();  	    
    }
    
// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//  
void CGlxMetadataAsyncUpdate::RunL()
    {
    TRACER("CGlxMetadataAsyncUpdate::RunL");
    if(iStatus.Int() == KErrNone)
    	{    	
    	iObserver.RefreshMediaListL();    	
    	}
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
// 
void CGlxMetadataAsyncUpdate::DoCancel()
    {
    TRACER("CGlxMetadataAsyncUpdate::DoCancel");
    // No need to do anything
    }    
    
// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//
CGlxMetadataAsyncUpdate* CGlxMetadataAsyncUpdate::NewL(CGlxMetadataContainer& aObserver)
	{
	TRACER("CGlxMetadataAsyncUpdate::NewL");
	CGlxMetadataAsyncUpdate* self = CGlxMetadataAsyncUpdate::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------
// NewLC
// ---------------------------------------------------------
//
CGlxMetadataAsyncUpdate* CGlxMetadataAsyncUpdate::NewLC(CGlxMetadataContainer& aObserver)
	{
	TRACER("CGlxMetadataAsyncUpdate::NewLC");
	CGlxMetadataAsyncUpdate* self = new(ELeave) CGlxMetadataAsyncUpdate(aObserver);
	CleanupStack::PushL(self);
	return self;
	}

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CGlxMetadataAsyncUpdate::CGlxMetadataAsyncUpdate(CGlxMetadataContainer& aObserver):
								CActive(EPriorityStandard),iObserver(aObserver)
    {
    TRACER("CGlxMetadataAsyncUpdate::CGlxMetadataAsyncUpdate()");
    CActiveScheduler::Add(this);
    }
	  
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//	
CGlxMetadataAsyncUpdate::~CGlxMetadataAsyncUpdate()
	{
	TRACER("CGlxMetadataAsyncUpdate::~CGlxMetadataAsyncUpdate");
	Cancel();	
	}
//End of file

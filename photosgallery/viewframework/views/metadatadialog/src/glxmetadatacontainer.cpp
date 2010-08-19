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
#include  "mglxmetadatadialogobserver.h"

#include  <StringLoader.h>
#include  <caf/manager.h>                         // For Filesystem
#include  <ExifModify.h>                          // For CExifModify 
#include  <mpxcommandgeneraldefs.h>               // Content ID identifying general category of content provided
#include  <mpxmediadrmdefs.h>
#include  <eikfrlb.h>                             // For marquee  
#include  <eikfrlbd.h>                            // For marquee

#include  <glxmetadatadialog.rsg>
#include  <glxscreenfurniture.h>
#include  <glxdetailsboundcommand.hrh>
#include  <glxcommandhandleraddtocontainer.h>     // For CGlxCommandHandlerAddToContainer
#include  <glxcommandhandlers.hrh>                // for command handler id
#include  <glxcommandfactory.h>                   // for command factory
#include  <glxtextentrypopup.h>
#include  <glxuistd.h>
#include  <glxcollectionplugintags.hrh>       	  // tag collection plugin uid
#include  <glxattributeretriever.h>               // CGlxAttributeReteiver
#include  <glxdrmutility.h>                       // For launching DRM details pane
#include  <glxgeneraluiutilities.h>               // General utilties class definition
#include  <glxuiutilities.rsg>                    
#include  <glxfilterfactory.h>
#include  <glxlog.h>
#include  <glxtracer.h>
#include <glxgeneraluiutilities.h>

const TInt KMediaListId  			 = 0x2000D248;
const TInt KOffsets					 = 50;
const TInt KMarqueeLoopCount         = 3;
const TInt KMarqueeScrollAmount      = 20;
const TInt KMarqueeScrollDelay       = 1000000;
const TInt KMarqueeScrollInterval    = 200000;

_LIT( KGlxComma, ",");

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//
CGlxMetadataContainer* CGlxMetadataContainer::NewL(const TRect& aRect,
        MGlxMetadataDialogObserver& aDialogObserver, const TDesC& item,
        MToolbarResetObserver& aResetToolbarObs)
    {
    TRACER("CGlxMetadataContainer::NewL");
    CGlxMetadataContainer* self = CGlxMetadataContainer::NewLC(aRect,
            aDialogObserver, item, aResetToolbarObs);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// NewLC
// ---------------------------------------------------------
//
CGlxMetadataContainer* CGlxMetadataContainer::NewLC(const TRect& aRect,
        MGlxMetadataDialogObserver& aDialogObserver, const TDesC& aUri,
        MToolbarResetObserver& aResetToolbarObs)
    {
    TRACER("CGlxMetadataContainer::NewLC");
    CGlxMetadataContainer* self = new (ELeave) CGlxMetadataContainer(
            aDialogObserver, aResetToolbarObs);
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aUri);
    return self;
    }

// ---------------------------------------------------------
// CGlxMetadataContainer
// ---------------------------------------------------------
//
CGlxMetadataContainer::CGlxMetadataContainer(
        MGlxMetadataDialogObserver& aDialogObserver,
        MToolbarResetObserver& aResetToolbarObs) :
    iDialogObesrver(aDialogObserver), iResetToolbarObs(aResetToolbarObs)
    {
    // No implementation
    }

// ---------------------------------------------------------
// CGlxMetadataContainer::ConstructL
// ---------------------------------------------------------
//
void CGlxMetadataContainer::ConstructL(const TRect& /*aRect*/,
        const TDesC& aUri)
    {
    TRACER("CGlxMetadataContainer::ConstructL");

    //media's uri
    iUri = aUri.AllocL();

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

    //Flag to indicate rename command is started
    iRenameStarted = EFalse;
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

	if(iUri)
		{
		delete iUri;
		iUri = NULL;
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
    iTextSetter.Zero();

    // Creating a empty Settings list box which will  be populated 
    // with metadata in handleattributeavailable
    switch (aResourceId)
        {
        case ENameItem:
        case EDateAndTimeItem:
        case EDescriptionItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aResourceId,
                    iTextSetter);

            break;
            }
        case ETagsItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aResourceId,
                    iTagSetter);
            break;
            }
        case EAlbumsItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aResourceId,
                    iAlbumSetter);
            break;
            }
        case ELocationItem:
        case ESizeItem:
        case EResolutionItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aResourceId,
                    iTextSetter);

            break;
            }
        case EDurationItem:
        case ElicenseItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aResourceId,
                    iTextSetter);
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
    TRACER("CGlxMetadataContainer::IsItemModifiable");   
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
// CGlxMetadataContainer::IsLocationItem
//-----------------------------------------------------------------------------
TBool CGlxMetadataContainer::IsLocationItem()
    {
    TRACER("CGlxMetadataContainer::IsLocationItem");
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
    TRACER("CGlxMetadataContainer::HandleListboxChangesL");

    //dont Edit Item's details if medialist is empty
    //OR Rename command is in progress
    if (iItemMediaList->Count() == 0 || iRenameStarted)
        {
        GLX_LOG_INFO("MediaList empty or Rename command started");
        return;
        }

    TInt index = ListBox()->CurrentItemIndex();

    switch (index)
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
            iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);
            //Launch add to container commandhandler via dialog observer.
            iDialogObesrver.AddTagL();
            break;
            }
        case EAlbumsItem:
            {
            //Set the focus of the item
            iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);
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
            if (!media.GetCoordinate(coordinate))
                {
                HBufC *noLocationBuf = StringLoader::LoadLC(
                        R_GLX_METADATA_NOTE_INFO_NO_LOCATION);
                GlxGeneralUiUtilities::ShowInfoNoteL(*noLocationBuf, ETrue);
                CleanupStack::PopAndDestroy(noLocationBuf);
                }
			else
				{
				CAknSettingItem* settingsitem = (*SettingItemArray())[index]; 
			    GlxGeneralUiUtilities::ShowInfoNoteL(settingsitem->SettingTextL(),ETrue);
				}
            break;
            }
        case EDurationItem:
            {
            //This is condition is useful when the license item is selected for a image file
            if (iVideo)
                {
                break;
                }
            }
        case EResolutionItem:
        	{
			//This is condition is useful when the license item is selected for a DRM Video file
			if (!iVideo || !iItemMediaList->Item(0).IsDrmProtected())
				{
				break;
				}
			}
        case ElicenseItem:
            {
            const TGlxMedia& item = iItemMediaList->Item(0);
            if (item.IsDrmProtected())
                {
                //Create DRM utility
                CGlxDRMUtility* drmUtility = CGlxDRMUtility::InstanceL();
                CleanupClosePushL(*drmUtility);

                // check if rights have expired
                TBool expired = EFalse;
                expired = !drmUtility->ItemRightsValidityCheckL(item.Uri(),
                        EMPXImage == item.Category());

                if (expired)
                    {
                    drmUtility->ShowRightsInfoL(item.Uri());
                    }
                else
                    {
                    drmUtility->ShowDRMDetailsPaneL(item.Uri());
                    }
                CleanupStack::PopAndDestroy(drmUtility);
                }
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
void CGlxMetadataContainer::CreateMediaListForSelectedItemL( )
    {
    TRACER("CGlxMetadataContainer::CreateMediaListForSelectedItemL");

    //create the collection path for the medialist to be created
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    //set the all collection path as the details dialog can be launched from any of the grid views and filter with URI
    path->AppendL(KGlxCollectionPluginAllImplementationUid);
    //create the filter with the URI
    CMPXFilter* filter = TGlxFilterFactory::CreateURIFilterL(*iUri);
    CleanupStack::PushL(filter);
    //create the medialist
    iItemMediaList = MGlxMediaList::InstanceL(*path, TGlxHierarchyId(
            KMediaListId), filter);

    //Add the attributes which are required to be displayed.
    iMainListAttributecontext = new (ELeave) CGlxAttributeContext(
            &iSelectionIterator);
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
    iItemMediaList->AddContextL(iMainListAttributecontext,
            KGlxFetchContextPriorityBlocking);

    //add to observer for callbacks.
    iItemMediaList->AddMediaListObserverL(this);

    CleanupStack::PopAndDestroy(filter);
    CleanupStack::PopAndDestroy(path);
    }

//-----------------------------------------------------------------------------
// CGlxMetadataContainer::CreateTagsMediaList
//-----------------------------------------------------------------------------
void CGlxMetadataContainer::CreateTagsMediaListL()
    {
    TRACER("CGlxMetadataContainer::CreateTagsMediaListL");
    //create the collection path for the medialist to be created
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    //Set the Tags collection for the particular item
    path->AppendL(KGlxTagCollectionPluginImplementationUid);
    //get the media item for which we require the tags collection
    TGlxMedia item = iItemMediaList->Item(0);
    //create the medialist filter with media ID
    CMPXFilter* filter1 =
            TGlxFilterFactory::CreateExcludeContainersWithoutItemFilterL(
                    item.Id());
    CleanupStack::PushL(filter1);
    //set the array order as required, here its alphabetical
    TGlxFilterProperties filterProperty;
    filterProperty.iSortOrder = EGlxFilterSortOrderAlphabetical;
    filterProperty.iSortDirection = EGlxFilterSortDirectionAscending;
    //combine the filter with filterProperty
    CMPXFilter* filter = TGlxFilterFactory::CreateCombinedFilterL(
            filterProperty, filter1, EFalse);
    CleanupStack::PushL(filter);

    //create the medialist to get the tags array.
    iTagMediaList = MGlxMediaList::InstanceL(*path, TGlxHierarchyId(
            KMediaListId), filter);

    //add the attribute KMPXMediaGeneralTitle to the context to get the tag name
    iTagContext = CGlxDefaultAttributeContext::NewL();
    iTagContext->AddAttributeL(KMPXMediaGeneralTitle);
    iTagContext->SetRangeOffsets(KOffsets, KOffsets);

    //Add Context so that we get the handleattributes call once the medialist is populated with tags.
    iTagMediaList->AddContextL(iTagContext,
            KGlxFetchContextPriorityUMPViewTagPane);

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
    CleanupStack::PushL(path);
    //Set the albums collection for the particular item
    path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
    //get the media item for which we require the tags collection
    TGlxMedia item = iItemMediaList->Item(0);
    //create the medialist filter with media ID
    CMPXFilter* filter =
            TGlxFilterFactory::CreateExcludeContainersWithoutItemFilterL(
                    item.Id());
    CleanupStack::PushL(filter);
    //create the albums medialist.
    iAlbumMediaList = MGlxMediaList::InstanceL(*path, TGlxHierarchyId(
            KMediaListId), filter);

    //add the attribute KMPXMediaGeneralTitle to the context to get the album name
    iAlbumContext = CGlxDefaultAttributeContext::NewL();
    iAlbumContext->AddAttributeL(KMPXMediaGeneralTitle);
    iAlbumContext->SetRangeOffsets(KOffsets, KOffsets);
    //Add Context to the medialist so that we get the handleattributes call once the medialist is populated with albums.
    iAlbumMediaList->AddContextL(iAlbumContext,
            KGlxFetchContextPriorityUMPViewAlbumPane);

    //add to observer for callbacks. 
    iAlbumMediaList->AddMediaListObserverL(this);

    CleanupStack::PopAndDestroy(filter);
    CleanupStack::PopAndDestroy(path);
    }

// ----------------------------------------------------------------------------
// CGlxMetadataContainer::ViewDynInitMenuPaneL
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::ViewDynInitMenuPaneL(TInt aMenuId,
        CEikMenuPane* aMenuPane)
    {
    if (aMenuId == R_METADATA_MENU)
        {
        //Set dim the options based on the item selected
        //location info will be enabled if the item has a location info
        aMenuPane->SetItemDimmed(KGlxDeleteBoundMenuCommandId,
                IsLocationItem());
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
    if (!isSupported)
        {
        HBufC *buf = StringLoader::LoadLC(
                R_GLX_METADATA_NOTE_INFO_NO_LOCATION);
        GlxGeneralUiUtilities::ShowInfoNoteL(*buf, ETrue);
        CleanupStack::PopAndDestroy(buf);
        }
    else
        {
        HBufC *buf =
                StringLoader::LoadLC(R_GLX_METADATA_NOTE_DELETE_LOCATION);
        TBool response = GlxGeneralUiUtilities::ConfirmQueryL(
                R_GLX_QUERY_YES_NO, *buf);
        CleanupStack::PopAndDestroy(buf);

        if (response)
            {
            //user selected yes, so delete location     
            //send command for delete location after successfull removal of command remove this pane
            if (iItemMediaList->Count() > 0)
                {
                //set focus to first item	
                iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);

                // Deleting location information from image file
                RFs rFs;
                User::LeaveIfError(rFs.Connect());
                CleanupClosePushL(rFs);
                RFile rFile;
                User::LeaveIfError(rFile.Open(rFs,
                        iItemMediaList->Item(0).Uri(), EFileWrite));
                CleanupClosePushL(rFile);

                TInt imageFileSize = 0; // Image File Size
                User::LeaveIfError(rFile.Size(imageFileSize));
                HBufC8* imageData = HBufC8::NewL(imageFileSize); //Actual Image Data
                CleanupStack::PushL(imageData);

                TPtr8 myImagePtr = imageData->Des();
                TInt readError = rFile.Read(myImagePtr);
                if (readError != KErrNone)
                    {
                    User::Leave(KErrGeneral);
                    }

                //CExifModify Interface class for modifying existing Exif v2.2 (or prior) 
                //file format or creating Exif v2.2 file format using valid Jpeg image     
                CExifModify* exifWriter = NULL;
                TRAPD(err,exifWriter = CExifModify::NewL( imageData->Des()));
                CleanupStack::PushL(exifWriter);
                if (err == KErrNone)
                    {
                    //Removes the specified IFD structure and all its tags from the Exif data
                    exifWriter->DeleteIfd(EIfdGps);

                    HBufC8* modifiedExif = exifWriter->WriteDataL(
                            imageData->Des()); //Modified Image Data
                    CleanupStack::PushL(modifiedExif);

                    const TUint32 fileSize = modifiedExif->Des().Length(); //Size of Modified File
                    TInt oldSize;
                    rFile.Size(oldSize);
                    // set position to begin of file & write the Modified data (Without Location Information)
                    TInt pos = 0;
                    User::LeaveIfError(rFile.Seek(ESeekStart, pos));
                    User::LeaveIfError(rFile.Write(modifiedExif->Des(),
                            fileSize));

                    TTime lastModified;
                    lastModified.UniversalTime();
                    User::LeaveIfError(rFile.SetModified(lastModified)); //Change the Modified Time

                    CleanupStack::PopAndDestroy(modifiedExif);
                    }
                CleanupStack::PopAndDestroy(exifWriter);
                CleanupStack::PopAndDestroy(imageData);
                CleanupStack::PopAndDestroy(&rFile);
                CleanupStack::PopAndDestroy(&rFs);

                //Deleting location information from MDS database
                CMPXCollectionPath* path = iItemMediaList->PathLC();
                CMPXCommand* command =
                        TGlxCommandFactory::DeleteLocationCommandLC(*path);

                command->SetTObjectValueL<TAny*> (
                        KMPXCommandGeneralSessionId,
                        static_cast<TAny*> (this));

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
// CGlxMetadataContainer::SetAttributesL
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::SetAttributesL(TMPXAttribute attribute)
	{
    TRACER("CGlxMetadataContainer::SetAttributesL");

    //create the tags and albums medialist once the item medialist is populated
    //Tags and albums medialist can be created only with media ID.
    if (!iTagMediaList)
        {
        CreateTagsMediaListL();
        }
    if (!iAlbumMediaList)
        {
        CreateAlbumsMediaListL();
        }
    if (!iSetVisible)
        {
        iSetVisible = ETrue;
        SetDurationLIicenseItemVisibilityL();
        }
    TGlxMedia item = iItemMediaList->Item(0);
    //Create the string convertor instance 
    //String convertor class with provide the specific format for date,location and duration and size.
    CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
    CleanupStack::PushL(stringConverter);
    HBufC* string = NULL;

    //if attribute is date and time we need to pass the format it as R_QTN_DATE_USUAL_WITH_ZERO else null
    if (attribute == KMPXMediaGeneralDate)
        {
        stringConverter->AsStringL(item, attribute,
                R_QTN_DATE_USUAL_WITH_ZERO, string);
        }
    else
        {
        stringConverter->AsStringL(item, attribute, 0, string);
        }
    //get the settings item based on the attribute and set the text.
    if (string)
        {
        iTextSetter.Zero();
        iTextSetter.Append(*string);
        if (attribute == KMPXMediaGeneralSize)
            {
            EditItemL(ESizeItem, EFalse);
            }
        else if (attribute == KMPXMediaGeneralDuration)
            {
            EditItemL(EDurationItem, EFalse);
            }
        else if (attribute == KMPXMediaGeneralTitle)
            {
            EditItemL(ENameItem, EFalse);
            }
        else if (attribute == KMPXMediaGeneralDate)
            {
            EditItemL(EDateAndTimeItem, EFalse);
            }
        else if (attribute == KMPXMediaGeneralComment)
            {
            EditItemL(EDescriptionItem, EFalse);
            }
        else if (attribute == KGlxMediaGeneralLocation)
            {
            EditItemL(ELocationItem, EFalse);
            }
        else if (attribute == KGlxMediaGeneralDimensions)
            {
            EditItemL(EResolutionItem, EFalse);
            }
        else
            {
            }
        delete string;
        string = NULL;
        }
    CleanupStack::PopAndDestroy(stringConverter);
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
    HBufC* textBuf = HBufC::NewLC(KMaxMediaPopupTextLength);
    const TDesC& popupText = settingsitem->SettingTextL();
    (textBuf->Des()).Copy(popupText.Left(KMaxMediaPopupTextLength));
    TPtr textPtr = textBuf->Des();
    //Remove preceeding & trailing spaces
    textPtr.Trim();
    HBufC* buf = NULL;

    if(aItem == ENameItem)
        {
        buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_TITLE_NSERIES);
        }
    else
        {
        buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_DESCRIPTION_NSERIES);
        }

    //Launch the text entry editor.
    CGlxTextEntryPopup* popup = CGlxTextEntryPopup::NewL(*buf, textPtr);
    CleanupStack::PopAndDestroy(buf);
    if (aItem == EDescriptionItem)
        {
        popup->SetLeftSoftKeyL(ETrue);
        }

    //action upon selecting ok from the editor 
    if (popup->ExecuteLD() == EEikBidOk)
        {
		if(0 != (popupText.Compare(*textBuf)))
            {
            TFileName fileName = ParseFileName(*textBuf);
            //check If filename already exists
            if ((aItem == ENameItem) && (BaflUtils::FileExists(
                    ControlEnv()->FsSession(), fileName)))
                {
                //if changed title is same as existing one then showing the already use popup to user
                HBufC* info = StringLoader::LoadLC(R_GLX_NAME_ALREADY_USED,
                        *textBuf);
                GlxGeneralUiUtilities::ShowInfoNoteL(*info, ETrue);
                CleanupStack::PopAndDestroy(info);
                }
            // Check if the filename is valid
            else if ((aItem == ENameItem)
                    && (!ControlEnv()->FsSession().IsValidName(*textBuf)))
                {
                //Show illegal characters error note 
                HBufC* info = StringLoader::LoadLC(
                        R_GLX_QTN_FLDR_ILLEGAL_CHARACTERS);
                GlxGeneralUiUtilities::ShowInfoNoteL(*info, ETrue);
                CleanupStack::PopAndDestroy(info);
                }
            else
                {
                //Modify the MDS and setting list only if the entry is different from previous Item value
                iTextSetter.Zero();
                iTextSetter.Copy(*textBuf);
                EditItemL(aItem, EFalse);
                iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);//set focus to first item

                if (aItem == ENameItem)
                    {
                    //indicate Rename command is started
                    iRenameStarted = ETrue;
                    //set Setting List Box to Dimmed status
                    SetDimmed(iRenameStarted);

                    const TGlxMedia& media = iItemMediaList->Item(0);
                    ContentAccess::CManager *manager =
                            ContentAccess::CManager::NewL();
                    CleanupStack::PushL(manager);
                    HBufC* modifiedName = fileName.AllocLC();

                    //rename the media
                    TInt error = manager->RenameFile(media.Uri(),
                            *modifiedName);
                    if (KErrNone == error)
                        {
                        //Redundant call But needed in case FileSystem is too slow 
                        //to notify MDS for updating title.
                        //Create the glx command for updating Title in MDS
                        CMPXCollectionPath* path = iItemMediaList->PathLC();
                        CMPXCommand* command =
                                TGlxCommandFactory::RenameCommandLC(
                                        settingsitem->SettingTextL(), *path);
                        command->SetTObjectValueL<TAny*> (
                                KMPXCommandGeneralSessionId,
                                static_cast<TAny*> (this));
                        //issue command to the medialist which further 
                        //calls data source to update MDS
                        iItemMediaList->CommandL(*command);
                        CleanupStack::PopAndDestroy(command);
                        CleanupStack::PopAndDestroy(path);
                        }
                    else
                        {
                        //Renaming commmand failed
                        iRenameStarted = EFalse;
                        //reset Setting Items to undim status
                        SetDimmed(iRenameStarted);
                        //Reset the EName Settings field
                        iTextSetter.Zero();
                        iTextSetter.Copy(media.Title());
                        EditItemL(ENameItem, EFalse);
						GlxGeneralUiUtilities::ShowErrorNoteL(error);
                        }
                    CleanupStack::PopAndDestroy(modifiedName);
                    CleanupStack::PopAndDestroy(manager);
                    }
                else
                    {
                    //Create the glx command for changing description
                    CMPXCollectionPath* path = iItemMediaList->PathLC();
                    CMPXCommand* command =
                            TGlxCommandFactory::SetDescriptionCommandLC(
                                    settingsitem->SettingTextL(), *path);
                    command->SetTObjectValueL<TAny*> (
                            KMPXCommandGeneralSessionId,
                            static_cast<TAny*> (this));
                    //issue command to the medialist which further 
                    //calls data source to update MDS
                    iItemMediaList->CommandL(*command);
                    CleanupStack::PopAndDestroy(command);
                    CleanupStack::PopAndDestroy(path);
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(textBuf);

    //notify observer that some operation has happened. So refresh the toolbar area..
    iResetToolbarObs.HandleToolbarResetting(EFalse);
    }

// ----------------------------------------------------------------------------
// CGlxMetadataContainer::UpdateTagsL()
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::UpdateTagsL()
    {
    TRACER("CGlxMetadataContainer::UpdateTagsL");
    //Get the tag setting item handle to set the text
    CAknSettingItem* settingsitem = (*SettingItemArray())[ETagsItem];

    //Set the tag setter to empty string before filling in the data.                                                
    iTagSetter.Zero();
     
    //Loop to appened all the tags to the iTagSetter.
    for (TInt index = 0; index < iTagMediaList->Count(); index++)
        {
        if (iTagSetter.Length())
            {
            iTagSetter.Append(KGlxComma);
            }
        const TGlxMedia& item = iTagMediaList->Item(index);
        const TDesC& title = item.Title();
        iTagSetter.Append(title);
        }
    EditItemL(ETagsItem, EFalse);
    }

// ----------------------------------------------------------------------------
// CGlxMetadataContainer::UpdateAlbumsL()
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::UpdateAlbumsL()
    {
    TRACER("CGlxMetadataContainer::UpdateAlbumsL");	
    //Get the tag setting item handle to set the text
    CAknSettingItem* settingsitem = (*SettingItemArray())[EAlbumsItem];

    //Set the tag setter to empty string before filling in the data.
    iAlbumSetter.Zero();
    
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
    TRACER("CGlxMetadataContainer::SetDurationLIicenseItemVisibilityL");		
    //get the media item.
    const TGlxMedia& item = iItemMediaList->Item(0);
    const CGlxMedia* media = item.Properties();

    //in order to check for video category and drm rights

    CAknSettingItem* hiddenItem = NULL;
    if (item.Category() == EMPXVideo)
        {
        if (!item.IsDrmProtected())
            {
            hiddenItem = (*SettingItemArray())[EDurationItem];
            //Set the duration item visible
            hiddenItem->SetHidden(EFalse);
            }
        else
            {
            hiddenItem = (*SettingItemArray())[EResolutionItem];
            //Set the Resolution item in-visible for DRM protected Video
            hiddenItem->SetHidden(ETrue);
            }
        //set the video flag which would be used to enable/disable the view details option.               
        this->HandleChangeInItemArrayOrVisibilityL();
        iVideo = ETrue;
        }
    if (media && media->IsSupported(KMPXMediaDrmProtected))
        {
        if (item.IsDrmProtected())
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
void CGlxMetadataContainer::HandleAttributesAvailableL(TInt /*aItemIndex*/,
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList)
    {
    TRACER("CGlxMetadataContainer::HandleAttributesAvailableL()");
    //generic medialist for the item for all the attributes required other than tags and albums.
    if (aList == iItemMediaList)
        {
        // Loop untill it checks for all the avialable attributes
        for (TInt i = aAttributes.Count() - 1; i >= 0; i--)
            {
            //set attributes to the items in the container
            SetAttributesL(aAttributes[i]);
            }

        //Here following 2 cases are handled:-
        //1) Rename from Photos application or Post-Capture view
        //- HandleAttributeavailable - new title & new uri at same time.
        //May come twice here since Redundant Rename CMD sent to MDS.
        //2) Rename from Filemanager.
        //- HandleAttributeavailable - new title & new uri at same time.
        //Check if media's uri(i.e 'aModifiedUri') is different from 'iUri'
        //i.e media is Renamed then Refesh Media list.
        TMPXAttribute uriAttrib(KMPXMediaGeneralUri);
        TIdentityRelation<TMPXAttribute> match(&TMPXAttribute::Match);
        TInt index = aAttributes.Find(uriAttrib, match);
        if (KErrNotFound != index)
            {
            HBufC* modifiedUri = NULL;
            TGlxMedia item = iItemMediaList->Item(0);
            //Create the string convertor instance
            CGlxUStringConverter* stringConverter =
                    CGlxUStringConverter::NewL();
            CleanupStack::PushL(stringConverter);

            //fetch media uri
            stringConverter->AsStringL(item, aAttributes[index], 0,
                    modifiedUri);
            CleanupStack::PopAndDestroy(stringConverter);

            //Check if media item was renamed
            if (modifiedUri && modifiedUri->Compare(*iUri) != 0)
                {
                //Set rename command as started since
                //Rename is also possible from File Manager
                iRenameStarted = ETrue;
                CleanupStack::PushL(modifiedUri);
                RefreshMediaListL(*modifiedUri);
                CleanupStack::PopAndDestroy(modifiedUri);
                }
            }

        }

    TMPXAttribute titleAttrib(KMPXMediaGeneralTitle);
    TIdentityRelation<TMPXAttribute> match(&TMPXAttribute::Match);

    if (KErrNotFound != aAttributes.Find(titleAttrib, match))
        {
        if (aList == iTagMediaList)
            {
            UpdateTagsL();
            }
        else if (aList == iAlbumMediaList)
            {
            UpdateAlbumsL();
            }
        }
    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::HandleItemAddedL(TInt /*aStartIndex*/,
        TInt /*aEndIndex*/, MGlxMediaList* aList)
    {
    TRACER("CGlxMetadataContainer::HandleItemAddedL()");

    if (!iTagMediaList)
        {
        CreateTagsMediaListL();
        }
    if (!iAlbumMediaList)
        {
        CreateAlbumsMediaListL();
        }
    if (!iMarquee)
        {
        EnableMarqueingL();
        }
    SetDurationLIicenseItemVisibilityL();
    if (aList == iTagMediaList)
        {
        UpdateTagsL();
        }
    else if (aList == iAlbumMediaList)
        {
        UpdateAlbumsL();
        }
    if (aList == iItemMediaList)
        {
        if (iItemMediaList->Count())
            {
            TGlxMedia item = iItemMediaList->Item(0);
            CGlxUStringConverter* stringConverter =
                    CGlxUStringConverter::NewL();
            CleanupStack::PushL(stringConverter);
            for (TInt index = 0; index <= EDurationItem; index++)
                {
                HBufC* string = NULL;
                iTextSetter.Zero();

                if (index == ESizeItem)
                    {
                    stringConverter->AsStringL(item, KMPXMediaGeneralSize, 0,
                            string);
                    }
                else if (index == EDurationItem)
                    {
                    stringConverter->AsStringL(item,
                            KMPXMediaGeneralDuration, 0, string);
                    }
                else if (index == ENameItem)
                    {
                    stringConverter->AsStringL(item, KMPXMediaGeneralTitle,
                            0, string);
                    }
                else if (index == EDateAndTimeItem)
                    {
                    stringConverter->AsStringL(item, KMPXMediaGeneralDate,
                            R_QTN_DATE_USUAL_WITH_ZERO, string);
                    }
                else if (index == EDescriptionItem)
                    {
                    stringConverter->AsStringL(item, KMPXMediaGeneralComment,
                            0, string);
                    }
                else if (index == ELocationItem)
                    {
                    stringConverter->AsStringL(item,
                            KGlxMediaGeneralLocation, 0, string);
                    }
                else if (index == EResolutionItem)
                    {
                    stringConverter->AsStringL(item,
                            KGlxMediaGeneralDimensions, 0, string);
                    }
                else if (index == ElicenseItem)
                    {
                    // If an item is DRM protected, License field in details
                    // should display "View Details"
                    string = StringLoader::LoadL(
                            R_GLX_METADATA_VIEW_OPTIONS_VIEW);
                    }
                else
                    {
                    //no implementation
                    }
                if (string)
                    {
                    iTextSetter.Append(*string);
                    }
                CleanupStack::PushL(string);
                EditItemL(index, EFalse);
                CleanupStack::PopAndDestroy(string);
                }
            CleanupStack::PopAndDestroy(stringConverter);

            //Reopening Media list is completed
            //& Rename Command is also completed
            if (iRenameStarted)
                {
                iRenameStarted = EFalse;
                //reset Setting Items to undimmed status
                SetDimmed(iRenameStarted);
                iAvkonAppUi->ProcessCommandL(EGlxCmdRenameCompleted);
                }
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
    ListBox()->ItemDrawer()->ColumnData()->SetMarqueeParams(
            KMarqueeLoopCount, KMarqueeScrollAmount, KMarqueeScrollDelay,
            KMarqueeScrollInterval);
    ListBox()->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);

    //Fetch the current item index
    TInt index = ListBox()->CurrentItemIndex();

    //Reset the disable marquee flag, so that marquee effect can continue (this is normally reset by 
    //base class of glxmetaDatadialog::HandlePointerEventL()
    ListBox()->ItemDrawer()->ClearFlags(CListItemDrawer::EDisableMarquee);

    //This is the function which actually starts marquee effect. It is anyway being called from base
    //implementation of OfferKeyEventL(), but for pointer event, we have to call
    //this function
    ListBox()->DrawItem(index);
    }

// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
// 
void CGlxMetadataContainer::HandleCommandCompleteL(TAny* aSessionId,
        CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList)
    {
    TRACER("CGlxMetadataContainer::HandleCommandCompleteL()");

    //Callback from MDS when rename the Title
    if (aError == KErrNone)
        {
        if (aList == iItemMediaList && aCommandResult->IsSupported(
                KMPXMediaGeneralTitle))
            {
            GLX_LOG_INFO("RenameCMD to MDS completed");
            //Since RenameCommand to MDS is redundant and FileSystem has 
            //already renamed the file, so there is no need to do anything here
            }
        }

    //To update the location information once the delete operation is successful.
    if (aList == iItemMediaList && iLocationinfo && static_cast<TAny*> (this)
            == aSessionId)
        {
        TGlxMedia media = iItemMediaList->Item(0);
        media.DeleteLocationAttribute();
        iLocationinfo = EFalse;
        if (aError == KErrNone)
            {
            iTextSetter.Zero();
            EditItemL(ELocationItem, EFalse);
            }
        }
    }

// ----------------------------------------------------------------------------
// HandleItemRemovedL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleItemRemovedL(TInt /*aStartIndex*/,
        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMetadataContainer::HandleItemRemovedL()");
    if (iItemMediaList->Count() == 0)
        {
        iDialogObesrver.HandleItemRemovedL();
        }
    }

// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/,
        TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMetadataContainer::HandleFocusChangedL()");
    }

// ----------------------------------------------------------------------------
// HandleItemSelectedL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleItemSelectedL(TInt /*aIndex*/,
        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMetadataContainer::HandleItemSelectedL");
    }

// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
void CGlxMetadataContainer::HandleMessageL(const CMPXMessage& /*aMessage*/,
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMetadataContainer::HandleMessageL()");
    }

// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxMetadataContainer::HandleError(TInt /*aError*/)
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
void CGlxMetadataContainer::HandleCommandCompleteL(
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMetadataContainer::HandleCommandCompleteL()");
    }

// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMetadataContainer::HandleMediaL()");
    }

// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxMetadataContainer::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMetadataContainer::HandleItemModifiedL()");
    }

// ----------------------------------------------------------------------------
// ChangeMskL
// ----------------------------------------------------------------------------
//     
void CGlxMetadataContainer::ChangeMskL()
    {
    TRACER("CGlxMetadataContainer::ChangeMskL()");
    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL(*uiUtility);
    switch (ListBox()->CurrentItemIndex())
        {
        case ENameItem:
        case EDescriptionItem:
        case ETagsItem:
        case EAlbumsItem:
            {
            uiUtility->ScreenFurniture()->ModifySoftkeyIdL(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    EAknSoftkeyEdit, R_GLX_METADATA_MSK_EDIT);
            }
            break;
        case EDateAndTimeItem:
        case ELocationItem:
        case ESizeItem:
        case EDurationItem:
        case ElicenseItem:
        case EResolutionItem:
            {
            uiUtility->ScreenFurniture()->ModifySoftkeyIdL(
                    CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    EAknSoftkeyEdit, R_GLX_METADATA_MSK_BLANK);
            }
            break;
        default:
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy(uiUtility);
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
// Refresh MediaList with modified FileName.
// ---------------------------------------------------------------------------
void CGlxMetadataContainer::RefreshMediaListL(const TDesC& aModifiedUri)
    {
    //Refresh media list since media is renamed
    TRACER("CGlxMetadataContainer::RefreshMediaListL()");
    GLX_LOG_URI("CGlxMetadataContainer::RefreshMediaListL(%S)", &aModifiedUri);
    if (iUri)
        {
        delete iUri;
        iUri = NULL;
        }
    //always points to current media name
    iUri = aModifiedUri.AllocL();
    CMPXFilter* filter = TGlxFilterFactory::CreateURIFilterL(*iUri);
    CleanupStack::PushL(filter);

    //Update media list's filter
    iItemMediaList->SetFilterL(filter);
    CleanupStack::PopAndDestroy(filter);
    }

//End of file

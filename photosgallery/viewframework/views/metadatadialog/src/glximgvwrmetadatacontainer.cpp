/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Image viewer metadata dialog implementation
 */

#include  "glximgvwrmetadatacontainer.h"
#include  "glxustringconverter.h"                // converts the symbian types to UString type
#include  <StringLoader.h>
#include  <glxmetadatadialog.rsg>
#include  <glxlog.h>
#include  <glxtracer.h>
#include  <glxscreenfurniture.h>
#include  <glxdetailsmulmodelprovider.h>          //Details data provider
#include  <glxdetailsboundcommand.hrh>
#include  <mpxcommandgeneraldefs.h>               // Content ID identifying general category of content provided
#include  <glxattributeretriever.h>               // CGlxAttributeReteiver
#include  <glxdrmutility.h>                       //For launching DRM details pane
#include  <glxuiutilities.rsg>
#include  <mpxmediadrmdefs.h>
#include  <glxfilterfactory.h>
#include  <glxcollectionpluginimageviewer.hrh>
#include  <eikfrlb.h>                              // For marquee 
#include  <eikfrlbd.h>                             // For marquee

const TInt KMediaListId = 0x2000D248;
const TInt KMarqueeLoopCount = 3;
const TInt KMarqueeScrollAmount = 20;
const TInt KMarqueeScrollDelay = 1000000;
const TInt KMarqueeScrollInterval = 200000;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer* CGlxImgVwrMetadataContainer::NewL(
        const TRect& aRect, const TDesC& item)
    {
    TRACER("CGlxImgVwrMetadataContainer::NewL");
    CGlxImgVwrMetadataContainer* self = CGlxImgVwrMetadataContainer::NewLC(
            aRect, item);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// NewLC
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer* CGlxImgVwrMetadataContainer::NewLC(
        const TRect& aRect, const TDesC& aUri)
    {
    TRACER("CGlxImgVwrMetadataContainer::NewLC");
    CGlxImgVwrMetadataContainer* self =
            new (ELeave) CGlxImgVwrMetadataContainer(aUri);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

// ---------------------------------------------------------
// CGlxImgVwrMetadataContainer
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer::CGlxImgVwrMetadataContainer(
        const TDesC& aUri) : iUri(aUri)
    {
    // No implementation
    }

// ---------------------------------------------------------
// CGlxImgVwrMetadataContainer::ConstructL
// ---------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::ConstructL(const TRect& /*aRect*/)
    {
    TRACER("CGlxMetadataContainer::ConstructLL()");
    //Creating the RBuf texts for all the items except tags & albums
    //which would be updated as whne the item is edited
    iTextSetter.CreateL(KMaxFileName);
    CreateMediaListForSelectedItemL();
    }

// ---------------------------------------------------------
// ~CGlxImgVwrMetadataContainer
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer::~CGlxImgVwrMetadataContainer()
    {
    TRACER("CGlxImgVwrMetadataContainer::~CGlxImgVwrMetadataContainer");
    if (iItemMediaList)
        {
        iItemMediaList->RemoveContext(iMainListAttributecontext);
        iItemMediaList->RemoveMediaListObserver(this);
        iItemMediaList->Close();
        iItemMediaList = NULL;
        }
    if (iMainListAttributecontext)
        {
        delete iMainListAttributecontext;
        iMainListAttributecontext = NULL;
        }
    if (IsVisible())
        {
        MakeVisible(EFalse);
        }
    iTextSetter.Close();
    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::MediaList
//-----------------------------------------------------------------------------
MGlxMediaList& CGlxImgVwrMetadataContainer::MediaList()
    {
    TRACER("CGlxMetadataContainer::HandleAttributesAvailableL()");
    //returns the active medialist.
    return *iItemMediaList;
    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::CreateSettingItemL
//-----------------------------------------------------------------------------
CAknSettingItem* CGlxImgVwrMetadataContainer::CreateSettingItemL(
        TInt aResourceId)
    {
    TRACER("CGlxImgVwrMetadataContainer::CreateSettingItemL");
    CAknSettingItem* settingItem = NULL; // No need to push onto cleanup stack
    iTextSetter.Zero();

    //Creating a empty Settings list box which will  be populated with metadata in handleattributeavailable
    switch (aResourceId)
        {
        case EImgVwrNameItem:
        case EImgVwrDateAndTimeItem:
        case EImgVwrMimeTypeItem:
        case EImgVwrSizeItem:
        case EImgVwrResolutionItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem(aResourceId,
                    iTextSetter);

            break;
            }

        case EImgVwrlicenseItem:
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
// CGlxImgVwrMetadataContainer::HandleListBoxEventL
//-----------------------------------------------------------------------------
void CGlxImgVwrMetadataContainer::HandleListBoxEventL(
        CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleListBoxEventL()");
    if (aEventType == EEventItemSingleClicked)
        {
        if (iItemMediaList->Count() == 0)
            {
            GLX_LOG_INFO("CGlxImgVwrMetadataContainer:: NO Items");
            return;
            }
        TInt index = ListBox()->CurrentItemIndex();
        if (EImgVwrlicenseItem == index)
            {
            GLX_LOG_INFO("CGlxImgVwrMetadataContainer::Licence item");
            CGlxDRMUtility* drmUtility = CGlxDRMUtility::InstanceL();
            CleanupClosePushL(*drmUtility);
            CreateImageViewerInstanceL();
            if (iImageViewerInstance->IsPrivate())
                {
                RFile64& fileHandle = iImageViewerInstance->ImageFileHandle();
                // check if rights have expired
                TBool expired = EFalse;
                expired = !drmUtility->ItemRightsValidityCheckL(fileHandle,
                        ETrue);

                if (expired)
                    {
                    drmUtility->ShowRightsInfoL(fileHandle);
                    }
                else
                    {
                    drmUtility->ShowDRMDetailsPaneL(fileHandle);
                    }
                }
            else
                {
                const TDesC& uri = iItemMediaList->Item(0).Uri();
                // check if rights have expired
                TBool expired = EFalse;
                expired = !drmUtility->ItemRightsValidityCheckL(uri, ETrue);

                if (expired)
                    {
                    drmUtility->ShowRightsInfoL(uri);
                    }
                else
                    {
                    drmUtility->ShowDRMDetailsPaneL(uri);
                    }
                }
            CleanupStack::PopAndDestroy(drmUtility);
            DeleteImageViewerInstance();
            }
        }
    }

//Medialist callbacks.    
// ----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::HandleAttributesAvailableL(
        TInt /*aItemIndex*/, const RArray<TMPXAttribute>& aAttributes,
        MGlxMediaList* aList)
    {
    TRACER("CGlxMetadataContainer::HandleAttributesAvailableL()");
    //generic medialist for the item for all the attributes required other than tags and albums.
    TInt x = aAttributes.Count();
    if (aList == iItemMediaList)
        {
        // Loop untill it checks for all the avialable attributes
        for (TInt i = aAttributes.Count() - 1; i >= 0; i--)
            {
            //set attributes to the items in the container
            SetAttributesL(aAttributes[i]);
            }
        }
    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxImgVwrMetadataContainer::HandleItemAddedL(TInt /*aStartIndex*/,
        TInt /*aEndIndex*/, MGlxMediaList* aList)
    {
    TRACER("CGlxMetadataContainer::HandleItemAddedL()");
    if (!iMarquee)
        {
        EnableMarqueingL();
        }
    SetLicenseItemVisibilityL();
    if (aList == iItemMediaList)
        {
        if (iItemMediaList->Count())
            {
            TGlxMedia item = iItemMediaList->Item(0);
            CGlxUStringConverter* stringConverter =
                    CGlxUStringConverter::NewL();
            CleanupStack::PushL(stringConverter);
            for (TInt index = 0; index <= EImgVwrlicenseItem; index++)
                {
                HBufC* string = NULL;
                iTextSetter.Zero();

                if (index == EImgVwrSizeItem)
                    {
                    stringConverter->AsStringL(item, KMPXMediaGeneralSize, 0,
                            string);
                    }
                else if (index == EImgVwrNameItem)
                    {
                    stringConverter->AsStringL(item, KMPXMediaGeneralTitle,
                            0, string);
                    }
                else if (index == EImgVwrDateAndTimeItem)
                    {
                    stringConverter->AsStringL(item,
                            KGlxMediaGeneralLastModifiedDate,
                            R_QTN_DATE_USUAL_WITH_ZERO, string);
                    }
                else if (index == EImgVwrMimeTypeItem)
                    {
                    stringConverter->AsStringL(item,
                            KMPXMediaGeneralMimeType, 0, string);
                    }
                else if (index == EImgVwrResolutionItem)
                    {
                    stringConverter->AsStringL(item,
                            KGlxMediaGeneralDimensions, 0, string);
                    }
                else if (index == EImgVwrlicenseItem)
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
                    iTextSetter.Zero();
                    iTextSetter.Append(*string);
                    }
                CleanupStack::PushL(string);
                EditItemL(index, EFalse);
                CleanupStack::PopAndDestroy(string);
                }
            CleanupStack::PopAndDestroy(stringConverter);
            }
        }
    }

// ----------------------------------------------------------------------------
// EnableMarqueingL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::EnableMarqueingL()
    {
    TRACER("CGlxImgVwrMetadataContainer::EnableMarqueingL()");
    iMarquee = ETrue;
    ListBox()->UseLogicalToVisualConversion(ETrue);
    ListBox()->ItemDrawer()->ColumnData()->SetMarqueeParams(
            KMarqueeLoopCount, KMarqueeScrollAmount, KMarqueeScrollDelay,
            KMarqueeScrollInterval);
    ListBox()->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    }

// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
// 
void CGlxImgVwrMetadataContainer::HandleCommandCompleteL(
        TAny* /*aSessionId*/, CMPXCommand* /*aCommandResult*/,
        TInt /*aError*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleCommandCompleteL()");
    }

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleItemRemovedL(TInt /*aStartIndex*/,
        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleItemRemovedL()");
    }

// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/,
        TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleFocusChangedL()");
    }

// ----------------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleItemSelectedL(TInt /*aIndex*/,
        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleItemSelectedL");
    }

// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
void CGlxImgVwrMetadataContainer::HandleMessageL(
        const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleMessageL()");
    }

// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::HandleError(TInt /*aError*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleError()");
    }

// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleCommandCompleteL(
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleCommandCompleteL()");
    }

// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleMediaL(TInt /*aListIndex*/,
        MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleMediaL()");
    }

// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleItemModifiedL()");
    }

// ----------------------------------------------------------------------------
// ChangeMskL
// ----------------------------------------------------------------------------    
void CGlxImgVwrMetadataContainer::ChangeMskL()
    {
    TRACER("CGlxImgVwrMetadataContainer::ChangeMskL()");
    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL(*uiUtility);
    switch (ListBox()->CurrentItemIndex())
        {
        case EImgVwrNameItem:
        case EImgVwrMimeTypeItem:
        case EImgVwrDateAndTimeItem:
        case EImgVwrSizeItem:
        case EImgVwrResolutionItem:
        case EImgVwrlicenseItem:
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

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::CreateMediaListForSelectedItemL
//-----------------------------------------------------------------------------
void CGlxImgVwrMetadataContainer::CreateMediaListForSelectedItemL()
    {
    TRACER("CGlxMetadataContainer::CreateMediaListForSelectedItemL");

    //create the collection path for the medialist to be created
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    // Set the Image viewer collection path as the details dialog 
    // can be launched from private or user data path
    path->AppendL(KGlxCollectionPluginImageViewerImplementationUid);
    //create the filter with the URI
    CMPXFilter* filter = TGlxFilterFactory::CreateURIFilterL(iUri);
    CleanupStack::PushL(filter);
    //create the medialist   
    iItemMediaList = MGlxMediaList::InstanceL(*path, TGlxHierarchyId(
            KMediaListId), filter);

    //Add the attributes which are required to be displayed.
    iMainListAttributecontext = new (ELeave) CGlxAttributeContext(
            &iSelectionIterator);
    iMainListAttributecontext->AddAttributeL(KMPXMediaDrmProtected);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralSize);
    iMainListAttributecontext->AddAttributeL(KGlxMediaGeneralDimensions);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralTitle);
    iMainListAttributecontext->AddAttributeL(KGlxMediaGeneralLastModifiedDate);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralMimeType);

    //Add Context so that we get the handleattributes call once the medialist is populated with above mentioned attributes.
    iItemMediaList->AddContextL(iMainListAttributecontext,
            KGlxFetchContextPriorityBlocking);

    //add to observer for callbacks.
    iItemMediaList->AddMediaListObserverL(this);

    CleanupStack::PopAndDestroy(filter);
    CleanupStack::PopAndDestroy(path);
    }

// ----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::SetAttributesL
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::SetAttributesL(TMPXAttribute attribute)
    {
    TRACER("CGlxImgVwrMetadataContainer::SetAttributesL");

    if (!iSetVisible)
        {
        iSetVisible = ETrue;
        SetLicenseItemVisibilityL();
        }
    TGlxMedia item = iItemMediaList->Item(0);
    //Create the string convertor instance 
    //String convertor class with provide the specific format for date,location and duration and size.
    CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
    CleanupStack::PushL(stringConverter);
    HBufC* string = NULL;

    //if attribute is date and time we need to pass the format it as R_QTN_DATE_USUAL_WITH_ZERO else null
    if (attribute == KGlxMediaGeneralLastModifiedDate)
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
            EditItemL(EImgVwrSizeItem, EFalse);
            }
        else if (attribute == KMPXMediaGeneralTitle)
            {
            EditItemL(EImgVwrNameItem, EFalse);
            }
        else if (attribute == KGlxMediaGeneralLastModifiedDate)
            {
            EditItemL(EImgVwrDateAndTimeItem, EFalse);
            }
        else if (attribute == KMPXMediaGeneralMimeType)
            {
            EditItemL(EImgVwrMimeTypeItem, EFalse);
            }
        else if (attribute == KGlxMediaGeneralDimensions)
            {
            EditItemL(EImgVwrResolutionItem, EFalse);
            }
        else if(attribute == KMPXMediaDrmProtected)
            {
            EditItemL(EImgVwrlicenseItem, EFalse);
            }
        else
            {
            //no implementation
            }
        delete string;
        string = NULL;
        }
    CleanupStack::PopAndDestroy(stringConverter);
    }

// ----------------------------------------------------------------------------
// CGlxMetadataContainer::EditItemL
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::EditItemL(TInt aIndex, TBool /*aCalledFromMenu*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::EditItemL");
    CAknSettingItem* settingsitem = NULL;
    settingsitem = (*SettingItemArray())[aIndex];
    settingsitem->LoadL();
    settingsitem->UpdateListBoxTextL();
    settingsitem->StoreL();
    ListBox()->DrawNow();
    }

// ----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::SetLicenseItemVisibilityL()
// ----------------------------------------------------------------------------
void CGlxImgVwrMetadataContainer::SetLicenseItemVisibilityL()
    {
    TRACER("CGlxMetadataContainer::SetLicenseItemVisibilityL()");
    //get the media item.
    const TGlxMedia& item = iItemMediaList->Item(0);
    const CGlxMedia* media = item.Properties();

    // Check for DRM protection
    if (media && media->IsSupported(KMPXMediaDrmProtected))
        {
        if (item.IsDrmProtected())
            {
            CAknSettingItem* hiddenItem =
                    (*SettingItemArray())[EImgVwrlicenseItem];
            //Set the License item visible
            hiddenItem->SetHidden(EFalse);
            //Refresh the listbox when any items visiblity is changed
            this->HandleChangeInItemArrayOrVisibilityL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CreateImageViewerInstanceL
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::CreateImageViewerInstanceL()
    {
    TRACER("CGlxImgVwrMetadataContainer::CreateImageViewerInstanceL");
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();
    __ASSERT_ALWAYS(iImageViewerInstance, Panic(EGlxPanicNullPointer));
    }

// -----------------------------------------------------------------------------
// DeleteImageViewerInstance
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::DeleteImageViewerInstance()
    {
    TRACER("CGlxImgVwrMetadataContainer::DeleteImageViewerInstance");
    if (iImageViewerInstance)
        {
        iImageViewerInstance->DeleteInstance();
        }
    }

//End of file

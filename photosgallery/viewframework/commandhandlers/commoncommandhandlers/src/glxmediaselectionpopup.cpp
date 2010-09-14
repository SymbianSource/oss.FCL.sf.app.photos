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
* Description:    Generic selection popup class
*
*/

// INCLUDE FILES

#include "glxmediaselectionpopup.h"

#include <data_caging_path_literals.hrh>// for KDC_APP_RESOURCE_DIR

#include <aknconsts.h>
#include <AknIconArray.h>
#include <aknlists.h>	
#include <AknQueryDialog.h>
#include <avkon.mbg>
#include <bautils.h>
#include <eikclbd.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxcollectiongeneraldefs.h>
#include <glxcommoncommandhandlers.rsg>
#include <glxfetchcontextremover.h>
#include <glxgeneraluiutilities.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxuistd.h>
#include <mpxcollectionmessage.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <StringLoader.h>
#include <glxuiutility.h>

#include "glxcommandhandlernewmedia.h"

_LIT(KUnselectedIconIndex, "1\t");
_LIT(KAlbumIconIndex, "2\t");
_LIT(KBlankIconIndex, "3\t"); // No icon

_LIT(KGlxCommonCommandHandlerResource, "glxcommoncommandhandlers.rsc");

const TInt KMediaSelectionPopupMediaListHierarchyId = 0x2000A77A;
const TInt KIconArrayGranularity = 2;
const TInt KNewItemId = 12345;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CGlxMediaListAdaptor::CGlxMediaListAdaptor(
        const MGlxMediaList* aMediaList, TBool aMultiSelection) :
    iMediaList(aMediaList), iMultiSelection(aMultiSelection),
			iStaticItemSelected(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// CGlxMediaListAdaptor::SetEnabled()
// ---------------------------------------------------------------------------
//   
void CGlxMediaListAdaptor::SetEnabled(TBool aEnabled)
    {
    TRACER("CGlxMediaListAdaptor::SetEnabled");
    iEnabled = aEnabled;
    }

// ---------------------------------------------------------------------------
// CGlxMediaListAdaptor::MediaList()
// ---------------------------------------------------------------------------
//   
const MGlxMediaList* CGlxMediaListAdaptor::MediaList()
    {
    TRACER("CGlxMediaListAdaptor::MediaList");
    return iMediaList;
    }

// ---------------------------------------------------------------------------
// CGlxMediaListAdaptor::MultiSelectionEnabled()
// ---------------------------------------------------------------------------
//   
TBool CGlxMediaListAdaptor::MultiSelectionEnabled()
    {
    return iMultiSelection;
    }

// ---------------------------------------------------------------------------
// CGlxMediaListAdaptor::MdcaCount()
// ---------------------------------------------------------------------------
//   
EXPORT_C TInt CGlxMediaListAdaptor::MdcaCount() const
    {
    TRACER("CGlxMediaListAdaptor::MdcaCount");
    __ASSERT_DEBUG(iMediaList, Panic(EGlxPanicNullPointer));
    if (iEnabled)
        {
        return iMediaList->Count();
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// CGlxMediaListAdaptor::MdcaPoint()
// ---------------------------------------------------------------------------
//   
EXPORT_C TPtrC CGlxMediaListAdaptor::MdcaPoint(TInt aIndex) const
    {
    TRACER("CGlxMediaListAdaptor::MdcaPoint");
    const TGlxMedia& item = iMediaList->Item(aIndex);

    TPtrC title = item.Title();
    if (item.IsStatic() || title.Length() == 0)
        {
        // The item is a static item or the item's title length is 0
        iCurrentTitleString = KBlankIconIndex;
        }
    else
        {
        if (iMultiSelection)
            {
            iCurrentTitleString = KUnselectedIconIndex;
            }
        else
            {
            iCurrentTitleString = KAlbumIconIndex;
            }
        }

    // iCurrentTitleString contains a tab character
    iCurrentTitleString.Append(title.Left(KMaxTitleStringLength
            - iCurrentTitleString.Length()));

    return TPtrC(iCurrentTitleString);
    }

// ---------------------------------------------------------------------------
// CGlxMediaListAdaptor::IsStaticItemSelected
// ---------------------------------------------------------------------------
TBool CGlxMediaListAdaptor::IsStaticItemSelected()
	{
	TRACER("CGlxMediaListAdaptor::IsStaticItemSelected");
	return iStaticItemSelected;
	}

// ---------------------------------------------------------------------------
// CGlxMediaListAdaptor::SetStaticItemSelected
// ---------------------------------------------------------------------------
void CGlxMediaListAdaptor::SetStaticItemSelected(TBool aSelected)
	{
	TRACER("CGlxMediaListAdaptor::SetStaticItemSelected");
	iStaticItemSelected = aSelected;
	}

//---------------------------------------------------------------------------
// CGlxSingleGraphicPopupMenuStyleList::NewL
//---------------------------------------------------------------------------
CGlxSingleGraphicPopupMenuStyleList* CGlxSingleGraphicPopupMenuStyleList::NewL(
		CEikListBox* aListBox, TInt aCbaResource,
		AknPopupLayouts::TAknPopupLayouts aType)
	{
	TRACER("CGlxSingleGraphicPopupMenuStyleList::NewL");

	CGlxSingleGraphicPopupMenuStyleList* self =
		new (ELeave) CGlxSingleGraphicPopupMenuStyleList;
	CleanupStack::PushL(self);
	self->ConstructL(aListBox, aCbaResource, aType);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CGlxSingleGraphicPopupMenuStyleList::HandleListBoxEventL()
// ---------------------------------------------------------------------------
//
void CGlxSingleGraphicPopupMenuStyleList::HandleListBoxEventL(
		CEikListBox *aListBox, TListBoxEvent aEventType)
    {
    TRACER("CGlxSingleGraphicPopupMenuStyleList::HandleListBoxEventL");

    CAknSingleGraphicPopupMenuStyleListBox* listBox =
			static_cast<CAknSingleGraphicPopupMenuStyleListBox*> (aListBox);

    CGlxMediaListAdaptor
			* mediaListAdaptor =
					static_cast<CGlxMediaListAdaptor*> (listBox->Model()->ItemTextArray());

    TBool staticItemSelected = EFalse;
    TInt currItemIndx = listBox->View()->CurrentItemIndex();
    GLX_LOG_INFO1("Glx Pop-up listbox - HandleListBoxEventL() currItemIndx=%d",
            currItemIndx);

    switch (aEventType)
        {
        case EEventItemClicked:
        case EEventItemSingleClicked:
            {
            if (mediaListAdaptor->MultiSelectionEnabled() && currItemIndx
                    >= 0)
                {
                const TGlxMedia& item = mediaListAdaptor->MediaList()->Item(
						currItemIndx);

                //Check if media item is user-defined or static
                staticItemSelected = item.IsStatic();
                mediaListAdaptor->SetStaticItemSelected(staticItemSelected);
                if (!staticItemSelected)
                    {
					//Mark or UnMark the user-defined item
                    TBool isMarked = listBox->View()->ItemIsSelected(
                            currItemIndx);
                    (isMarked == (TBool) ETrue) ? (listBox->View()->DeselectItem(
                                                  currItemIndx))
                                                : (listBox->View()->SelectItemL(currItemIndx));
                    }

                //Show Command Set based on selected items
                TInt selectCount =
                        listBox->View()->SelectionIndexes()->Count();
                CEikButtonGroupContainer* cbaContainer =
                        ButtonGroupContainer();
				//Show 'OK' only if a static item or more than
				//1 user-defined item is selected
                if (staticItemSelected || selectCount)
					{
					cbaContainer->SetCommandSetL(R_GLX_SOFTKEYS_OK_CANCEL);
					}
				else
					{
					cbaContainer->SetCommandSetL(R_AVKON_SOFTKEYS_CANCEL);
					}
                //LSK / SK1’s visibility is determined by the screen context, if single click support enabled.
                //As suggested by Avkon, disable the CEikCba’s item specific softkey default behavior.
				CEikCba* eikCba = static_cast<CEikCba*>( cbaContainer->ButtonGroup() );
				if( eikCba )
					{
					eikCba->EnableItemSpecificSoftkey( EFalse );
					}
				cbaContainer->DrawDeferred();
                }
            listBox->DrawDeferred();

            //Forward for default processing, if static item is selected(i.e. For Tags)
			//or if List Box's Multiple selection is Disabled(i.e. For Albums)
			if (staticItemSelected
					|| (!mediaListAdaptor->MultiSelectionEnabled()))
				{
                CAknPopupList::HandleListBoxEventL(aListBox, aEventType);
				}

			//After Scrolling, then Select "New Tag" i.e Static item is selected
			if (staticItemSelected)
				{
				ProcessCommandL(EAknSoftkeyOk);
				}

            break;
            }
        case EEventEnterKeyPressed:
            {
            // Check for multiselection is disbaled(i.e. for Albums) 
            // and valid index
            if (!(mediaListAdaptor->MultiSelectionEnabled()) && currItemIndx
                    >= 0)
                {
                //Set if its a static item
                const TGlxMedia& item = mediaListAdaptor->MediaList()->Item(
                        currItemIndx);
                mediaListAdaptor->SetStaticItemSelected(item.IsStatic());
                }
            CAknPopupList::HandleListBoxEventL(aListBox, aEventType);
            break;
            }
        default:
        	{
            CAknPopupList::HandleListBoxEventL(aListBox, aEventType);
        	break;
        	}
        }
    }

// ---------------------------------------------------------------------------
// CGlxSingleGraphicPopupMenuStyleList::HandlePointerEventL()
// ---------------------------------------------------------------------------
//
void CGlxSingleGraphicPopupMenuStyleList::HandlePointerEventL(
		const TPointerEvent& aPointerEvent)
    {
	TRACER("CGlxSingleGraphicPopupMenuStyleList::HandlePointerEventL");

    CAknSingleGraphicPopupMenuStyleListBox* listBox =
			static_cast<CAknSingleGraphicPopupMenuStyleListBox*> (ListBox());
	CGlxMediaListAdaptor
			* mediaListAdaptor =
					static_cast<CGlxMediaListAdaptor*> (listBox->Model()->ItemTextArray());

	//Check for MultipleSelection is Disbaled
    if ( !(mediaListAdaptor->MultiSelectionEnabled()) )
    	{
		//checking if the index has changed & has valid item available
		TInt changedItemIndex = KErrNotFound;
        TBool isItemAvailable = listBox->View()->XYPosToItemIndex(
				aPointerEvent.iPosition, changedItemIndex);
        if(isItemAvailable)
			{
			//Check if Selected item is static
			const TGlxMedia& item = mediaListAdaptor->MediaList()->Item(
					changedItemIndex);
			mediaListAdaptor->SetStaticItemSelected( item.IsStatic() );
			}
        }
    //Forward for default processing
    CAknPopupList::HandlePointerEventL( aPointerEvent);
    }

// ---------------------------------------------------------------------------
// CGlxSingleGraphicPopupMenuStyleListBox::OfferKeyEventL
// ---------------------------------------------------------------------------
//  
TKeyResponse CGlxSingleGraphicPopupMenuStyleListBox::OfferKeyEventL(
		const TKeyEvent& aKeyEvent, TEventCode aType)
	{
    TRACER("CGlxSingleGraphicPopupMenuStyleListBox::OfferKeyEventL");
    //Note: To add custom implementation to OfferKeyEventL(), change 
    //CAknSingleGraphicPopupMenuStyleListBox to CGlxSingleGraphicPopupMenuStyleListBox.
    
    //Based on the selected item index, disable the MultipleSelection flag
    //to stop the flickering of 'marked box', when Highlighted 'New Tag' is selected.
    CGlxMediaListAdaptor* mediaListAdaptor =
            static_cast<CGlxMediaListAdaptor*> (Model()->ItemTextArray());
    TInt currItemIndx = View()->CurrentItemIndex();
    GLX_LOG_INFO1("Glx Pop-up listbox - OfferKeyEventL(1) currItemIndx=%d",
            currItemIndx);

    if (mediaListAdaptor->MultiSelectionEnabled() && currItemIndx >= 0)
        {
        const TGlxMedia& oldItem = mediaListAdaptor->MediaList()->Item(
                currItemIndx);
        if (oldItem.IsStatic())
            {
            iListBoxFlags &= (~EMultipleSelection); // turn off multiple selection
            }
        else
            {
            iListBoxFlags |= EMultipleSelection; // turn on multiple selection
            }
        }

    //Forward for default processing
    TKeyResponse response =
            CAknSingleGraphicPopupMenuStyleListBox::OfferKeyEventL(aKeyEvent,
                    aType);

    currItemIndx = View()->CurrentItemIndex();
    GLX_LOG_INFO1("Glx Pop-up listbox - OfferKeyEventL(2) currItemIndx=%d",
            currItemIndx);
    //Check if 'Enter'/'Ok' key was consumed for 'MultipleSelection' List Box i.e for Tags
    if ((response == EKeyWasConsumed) && ((aKeyEvent.iCode == EKeyEnter)
            || (aKeyEvent.iCode == EKeyOK))
            && mediaListAdaptor->MultiSelectionEnabled() && currItemIndx >= 0)
        {
        //current selected item
        const TGlxMedia& item = mediaListAdaptor->MediaList()->Item(
                currItemIndx);
        TBool staticItemSelected = item.IsStatic();
        mediaListAdaptor->SetStaticItemSelected(staticItemSelected);

        TInt selectCount = View()->SelectionIndexes()->Count();
        CEikButtonGroupContainer* cbaContainer =
                CEikButtonGroupContainer::Current();
        //Check if selected Item is static or other item is selected
        if (staticItemSelected || selectCount)
            {
            cbaContainer->SetCommandSetL(R_GLX_SOFTKEYS_OK_CANCEL);
            }
        //no item is selected
        else
            {
            cbaContainer->SetCommandSetL(R_AVKON_SOFTKEYS_CANCEL);
            }
        cbaContainer->DrawDeferred();

        //Enter Key is Pressed and Static Item is selected
        if (staticItemSelected)
            {
            //Report 'Enter' key is pressed to ListBox observers
            ReportListBoxEventL(MEikListBoxObserver::EEventEnterKeyPressed);
            }
        }

    return response;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//  
EXPORT_C CGlxMediaSelectionPopup::CGlxMediaSelectionPopup() :
    CActive(EPriorityStandard)
    {
    TRACER("GlxMediaSelectionPopup::CGlxMediaSelectionPopup");
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// CGlxMediaSelectionPopup::RunL()
// ---------------------------------------------------------------------------
//  
void CGlxMediaSelectionPopup::RunL()
    {
    TRACER("CGlxMediaSelectionPopup::RunL");
    // The media list adaptor is enabled after the popup list is invoked
    // to ensure that the popup list is laid out correctly.
    iMediaListAdaptor->SetEnabled(ETrue);
    AddNewMediaCreationItemL();
    }

// ---------------------------------------------------------------------------
// CGlxMediaSelectionPopup::DoCancel()
// ---------------------------------------------------------------------------
//  
void CGlxMediaSelectionPopup::DoCancel()
    {
    // no implementation required
    }

// ---------------------------------------------------------------------------
// CGlxMediaSelectionPopup::ExecuteLD()
// ---------------------------------------------------------------------------
//  
EXPORT_C CMPXCollectionPath* CGlxMediaSelectionPopup::ExecuteLD(
        CMPXCollectionPath& aPath, TBool& aAccepted, TBool aMultiSelection,
        TBool aEnableContainerCreation, CMPXFilter* aFilter)
    {
    TRACER("CGlxMediaSelectionPopup::ExecuteLD");
    CleanupStack::PushL(this); // if something leaves delete 'this' object

    iEnableContainerCreation = aEnableContainerCreation;

    iCollectionId = TGlxMediaId(aPath.Id(0));

    FetchTitlesL();

    iMediaList = MGlxMediaList::InstanceL(aPath,
            KMediaSelectionPopupMediaListHierarchyId, aFilter);

    iMediaList->AddMediaListObserverL(this);

    iMediaListAdaptor = new (ELeave) CGlxMediaListAdaptor(iMediaList,
            aMultiSelection);

    SetupAttributeContextL();

    AddResourceFileL();
    ConstructPopupListL(aMultiSelection);
    InitIconsL();

    CompleteSelf(); // This will cause RunL() to be called by the active scheduler.

    // Invoke the dialog. 
    // The media list adaptor MdcaCount() method must return 0 at this point in order
    // for the popup to be displayed correctly. If MdcaCount() returns 1 then the popup will contain only
    // a single row.
    // iPopupList->ExecuteLD() starts a nested active scheduler so acvtive objects will be executed before
    // iPopupList->ExecuteLD() completes.
    aAccepted = iPopupList->ExecuteLD();

    if (iListBox->CurrentItemIndex() < 0)
        {
        // There is no current item therefore there is nothing to select
        // Fix for error ID: ERBS-7BSKFV Application crashes if user attempts to add a tag when in full screen view.
        aAccepted = EFalse;
        }

    iMediaList->RemoveMediaListObserver(this); // We no longer require any callbacks from the media list

    CMPXCollectionPath* path = NULL;
    if (aAccepted)
        {
		//Check if a static item is selected
		if (iMediaListAdaptor->IsStaticItemSelected())
            {
			TFileName uiutilitiesrscfile;
			uiutilitiesrscfile.Append(
					CGlxResourceUtilities::GetUiUtilitiesResourceFilenameL());
            CGlxCommandHandlerNewMedia* commandHandlerNewMedia =
			CGlxCommandHandlerNewMedia::NewL(this,uiutilitiesrscfile);
            TGlxMediaId newMediaId;
            TInt error = commandHandlerNewMedia->ExecuteLD(newMediaId);
            if (error == KErrNone)
                {
                path = CMPXCollectionPath::NewL(aPath);
                CleanupStack::PopAndDestroy(this);
                CleanupStack::PushL(path);
                path->AppendL(newMediaId.Value());
                CleanupStack::Pop(path);
                }
            else if (error == KErrCancel)
                {
                // The user has cancelled the popup list
                aAccepted = EFalse;
                CleanupStack::PopAndDestroy(this);
                }
            else
                {
                // The error is neither KErrNone or KErrCancel, leave.
                User::Leave(error);
                }
            }
        else
            {
            iMediaList->SetFocusL(NGlxListDefs::EAbsolute,
                    iListBox->CurrentItemIndex());

            const CListBoxView::CSelectionIndexArray* selectionIndices =
                    iListBox->SelectionIndexes();

            TInt count = selectionIndices->Count();
            for (TInt i = 0; i < count; i++)
                {
                iMediaList->SetSelectedL(selectionIndices->At(i), ETrue);
                }

            path = iMediaList->PathLC(NGlxListDefs::EPathFocusOrSelection);
            CleanupStack::Pop(path);
            CleanupStack::PopAndDestroy(this);
            }
        }
    else
        {
        CleanupStack::PopAndDestroy(this);
        }

    return path;
    }

// ---------------------------------------------------------------------------
// CGlxMediaSelectionPopup::ListBoxItemsChanged()
// ---------------------------------------------------------------------------
//  
void CGlxMediaSelectionPopup::ListBoxItemsChanged(CEikListBox* /*aListBox*/)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxMediaSelectionPopup::~CGlxMediaSelectionPopup()
    {
    TRACER("CGlxMediaSelectionPopup::~CGlxMediaSelectionPopup");
    Cancel();

    delete iMediaListAdaptor;
    delete iListBox;

    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }

    if (iMediaList)
        {
        if (iAttributeContext)
            {
            iMediaList->RemoveContext(iAttributeContext);
            }
        iMediaList->RemoveMediaListObserver(this); // This is required in case ExecuteLD() left.

        iMediaList->Close();
        }

    delete iAttributeContext;
    delete iSelectMediaPopupTitle;
    delete iNewMediaItemTitle;
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleItemAddedL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleItemAddedL(TInt aStartIndex,
        TInt aEndIndex, MGlxMediaList* aList)
    {
    TRACER("CGlxMediaSelectionPopup::HandleItemAddedL");
    // The call to HandleItemAdditionL() should be deferred if all of the new items
    // don't have title attributes as unnecessary calls to  HandleItemAdditionL will 
    // cause the dialog to flicker.
    if (iListBox)
        {
        TBool handleItemAdditionRequired = EFalse;
        for (TInt i = aStartIndex; i <= aEndIndex; i++)
            {
            TGlxMedia item = aList->Item(i);
            if (item.Title().Length() > 0)
            // The title length is greater than 0, i.e. there is a title.
                {
                handleItemAdditionRequired = ETrue;
                break;
                }
            }

        if (handleItemAdditionRequired)
            {
            iListBox->HandleItemAdditionL();
            }
        else
            {
            iHandleItemAdditionRequired = ETrue; // defer the call until we have title attributes
            }
        }

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL(*uiUtility);
    if ((iMediaListAdaptor && !iMediaListAdaptor->MultiSelectionEnabled())
            || !uiUtility->IsPenSupported())
        {
        CEikButtonGroupContainer* cbaContainer =
                iPopupList->ButtonGroupContainer();
        cbaContainer->SetCommandSetL(R_GLX_SOFTKEYS_OK_CANCEL);
        cbaContainer->DrawDeferred();
        }
    CleanupStack::PopAndDestroy(uiUtility);

    ///@todo if required: update the existing selection when items are added
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleMediaL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleMediaL(TInt /*aListIndex*/,
        MGlxMediaList* /*aList*/)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleItemRemovedL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleItemRemovedL(TInt /*aStartIndex*/,
        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMediaSelectionPopup::HandleItemRemovedL");
    if (iListBox)
        {
        iListBox->HandleItemRemovalL();
        }
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleItemModifiedL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleItemModifiedL(
        const RArray<TInt>& aItemIndexes, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMediaSelectionPopup::HandleItemModifiedL");
    if (iListBox)
        {
        for (TInt i = 0; i < aItemIndexes.Count(); i++)
            {
            if (IsListBoxItemVisible(aItemIndexes[i]))
                {
                iListBox->RedrawItem(aItemIndexes[i]);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleAttributesAvailableL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleAttributesAvailableL(TInt aItemIndex,
        const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMediaSelectionPopup::HandleAttributesAvailableL");
    if (iListBox)
        {
        if (iHandleItemAdditionRequired)
            {
            iListBox->HandleItemAdditionL();
            iHandleItemAdditionRequired = EFalse;
            }
        if (IsListBoxItemVisible(aItemIndex))
			{
            iListBox->RedrawItem(aItemIndex);
            }
        }
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleFocusChangedL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/,
        TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleItemSelectedL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleItemSelectedL(TInt /*aIndex*/,
        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleMessageL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleMessageL(const CMPXMessage& /*aMessage*/,
        MGlxMediaList* /*aList*/)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::HandleError()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::HandleError(TInt /*aError*/)
    {
    ///@todo implement
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::MediaList()
// -----------------------------------------------------------------------------
//
MGlxMediaList& CGlxMediaSelectionPopup::MediaList()
    {
    // Provides a media list to CGlxCommandHandlerNewMedia
    return *iMediaList;
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::AddResourceFileL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::AddResourceFileL()
    {
    TRACER("CGlxMediaSelectionPopup::AddResourceFileL");
    // Load resource
    TParse parse;
    parse.Set(KGlxCommonCommandHandlerResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::SetupAttributeContextL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::SetupAttributeContextL()
    {
    TRACER("CGlxMediaSelectionPopup::SetupAttributeContextL");
    iAttributeContext = new (ELeave) CGlxAttributeContext(&iIterator);
    iAttributeContext->AddAttributeL(KMPXMediaGeneralTitle);
    iMediaList->AddContextL(iAttributeContext, KGlxFetchContextPriorityLow);
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::SetupAttributeContextL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::InitIconsL()
    {
    TRACER("CGlxMediaSelectionPopup::InitIconsL");
    CAknIconArray* iconArray = new (ELeave) CAknIconArray(
            KIconArrayGranularity);
    CleanupStack::PushL(iconArray);

    // Sets graphics as ListBox icon.
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);

    CleanupStack::Pop(iconArray); // iconArray

    ///@todo use mgallery icons when available
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CGulIcon* icon =
            AknsUtils::CreateGulIconL(skin, KAknsIIDQgnPropCheckboxOn,
                    KAvkonBitmapFile, EMbmAvkonQgn_prop_checkbox_on,
                    EMbmAvkonQgn_prop_checkbox_on_mask);
    CleanupStack::PushL(icon);
    iconArray->AppendL(icon);
    CleanupStack::Pop(icon);

    icon = AknsUtils::CreateGulIconL(skin, KAknsIIDQgnPropCheckboxOff,
            KAvkonBitmapFile, EMbmAvkonQgn_prop_checkbox_off,
            EMbmAvkonQgn_prop_checkbox_off_mask);
    CleanupStack::PushL(icon);
    iconArray->AppendL(icon);
    CleanupStack::Pop(icon);

    iconArray->AppendFromResourceL(R_GLX_ALBUM_SELECTION_ICONS);

    ///@todo Find a more elegant way of not painting an icon.
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    icon = CGulIcon::NewL(bitmap);
    CleanupStack::Pop(bitmap);
    CleanupStack::PushL(icon);
    iconArray->AppendL(icon);
    CleanupStack::Pop(icon);
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::AddNewMediaCreationItemL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::AddNewMediaCreationItemL()
    {
    TRACER("CGlxMediaSelectionPopup::AddNewMediaCreationItemL");
    if (iEnableContainerCreation)
        {
        CGlxMedia* newMediaCreationStaticItem = new (ELeave) CGlxMedia(
                TGlxMediaId(KNewItemId));
        CleanupStack::PushL(newMediaCreationStaticItem);
        __ASSERT_DEBUG(iNewMediaItemTitle, Panic(EGlxPanicNullPointer));
        newMediaCreationStaticItem->SetTextValueL(KMPXMediaGeneralTitle,
                *iNewMediaItemTitle);
        iMediaList->AddStaticItemL(newMediaCreationStaticItem,
                NGlxListDefs::EInsertFirst);
        CleanupStack::Pop(newMediaCreationStaticItem);
        }
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::CompleteSelf()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::CompleteSelf()
    {
    TRACER("CGlxMediaSelectionPopup::CompleteSelf");
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::FetchTitlesL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::FetchTitlesL()
    {
    TRACER("CGlxMediaSelectionPopup::FetchTitlesL");
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);

    MGlxMediaList* rootList = MGlxMediaList::InstanceL(*path);
    CleanupClosePushL(*rootList);

    TGlxSpecificIdIterator iter(KGlxIdSpaceIdRoot, iCollectionId);
    CGlxAttributeContext* attributeContext =
            new (ELeave) CGlxAttributeContext(&iter);
    CleanupStack::PushL(attributeContext);
    attributeContext->AddAttributeL(
            KGlxMediaCollectionPluginSpecificNewMediaItemTitle);
    attributeContext->AddAttributeL(
            KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle);
    rootList->AddContextL(attributeContext, KGlxFetchContextPriorityBlocking);

    // Media list must not have been deleted when the destructor of 
    // TGlxContextRemover is called while going out-of-scope.
        {
        // TGlxContextRemover will remove the context when it goes out of scope
        // Used here to avoid a trap and still have safe cleanup   
        TGlxFetchContextRemover contextRemover(attributeContext, *rootList);
        CleanupClosePushL(contextRemover);
        User::LeaveIfError(GlxAttributeRetriever::RetrieveL(
                *attributeContext, *rootList, EFalse));
        // context off the list
        CleanupStack::PopAndDestroy(&contextRemover);
        } // Limiting scope of contextRemover

    TInt index = rootList->Index(KGlxIdSpaceIdRoot, iCollectionId);

    __ASSERT_DEBUG(index> KErrNotFound, Panic(EGlxPanicRequiredItemNotFound));
    TGlxMedia item = rootList->Item(index);

    const CGlxMedia* media = item.Properties();
    if (media)
        {
        iNewMediaItemTitle = media->ValueText(
                KGlxMediaCollectionPluginSpecificNewMediaItemTitle).AllocL();
        iSelectMediaPopupTitle
                = media->ValueText(
                        KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle).AllocL();
        }

    CleanupStack::PopAndDestroy(attributeContext);
    CleanupStack::PopAndDestroy(rootList);
    CleanupStack::PopAndDestroy(path);
    }

// -----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::ConstructPopupListL()
// -----------------------------------------------------------------------------
//
void CGlxMediaSelectionPopup::ConstructPopupListL(TBool aMultiSelection)
    {
    TRACER("CGlxMediaSelectionPopup::ConstructPopupListL");
    // create the list box
    iListBox = new (ELeave) CGlxSingleGraphicPopupMenuStyleListBox;

    // create the popup list
    iPopupList = CGlxSingleGraphicPopupMenuStyleList::NewL( iListBox, R_AVKON_SOFTKEYS_CANCEL) ;

    // set the title of the popup
    __ASSERT_DEBUG(iSelectMediaPopupTitle, Panic(EGlxPanicNullPointer));

    iPopupList->SetTitleL(*iSelectMediaPopupTitle);

    iListBox ->ConstructL(iPopupList,
            aMultiSelection
                            ?  EAknListBoxMultiselectionList
                               : EAknListBoxMenuList);
    iListBox->CreateScrollBarFrameL();
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    // create the listbox model
    iListBox->Model()->SetItemTextArray(iMediaListAdaptor);
    iListBox->View()->CalcBottomItemIndex();
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

    //set 'iPopupList' as List Box observer
    iListBox->SetListBoxObserver(iPopupList);
    }

//-----------------------------------------------------------------------------
// CGlxMediaSelectionPopup::IsListBoxItemVisible
//-----------------------------------------------------------------------------
TBool CGlxMediaSelectionPopup::IsListBoxItemVisible(TInt aIndex)
	{
	TRACER("CGlxMediaSelectionPopup::IsListBoxItemVisible");
	return iListBox && (iListBox->TopItemIndex() <= aIndex &&
			aIndex <= iListBox->BottomItemIndex());
	}

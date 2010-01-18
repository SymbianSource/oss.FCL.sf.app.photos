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
 * Description:    Implementation of Grid view control
 *
 */
#include <AknUtils.h>                               	// For AknLayoutUtils to calculate rect
#include <glxtracer.h>
#include <StringLoader.h>
#include <AknsBasicBackgroundControlContext.h>
#include <caf/caferr.h>

// Ganes Headers
#include <gulicon.h>                                    // Gul Icons
#include <ganes/HgItem.h>                               // HG Items
#include <ganes/HgGrid.h>                               // Hg Grid Widget

// Framework
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>                       // For EGlxCmdFullScreenBack
#include <glxcollectionpluginall.hrh> 	            	// All item collection plugin id


#include <glxnavigationalstate.h>                       // For Navigational State
#include <mglxmedialist.h>				            	// MGlxMediaList, CMPXCollectionPath
#include <glxuiutility.h>                               // For UiUtility instance
#include <glxdrmutility.h>                              // DRM utility class to provide DRM-related functionality
#include <glxerrormanager.h>                            // For CGlxErrormanager
#include <glxthumbnailattributeinfo.h>                  // KGlxMediaIdThumbnail
#include <glxgridviewdata.rsg>                          // Gridview resource
#include <glxicons.mbg>                                 // For Corrupted and not created icons

#include "glxgridviewcontainer.h"
#include "glxgridviewimp.h"
#include "glxgridviewmlobserver.h"                      // medialist observer for Hg Grid

const TInt KNoOfPages(3);
const TInt KBufferTresholdSize(3); 						// in rows

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxGridViewContainer* CGlxGridViewContainer::NewL(MGlxMediaList *aMediaList,
														CGlxUiUtility* aUiUtility,
																	MGlxGridEventObserver& aObserver)
	{
	TRACER("CGlxGridViewContainer::NewL");
	CGlxGridViewContainer* self = CGlxGridViewContainer::NewLC(aMediaList,aUiUtility,aObserver) ;
	CleanupStack::Pop(self) ;
	return self ;
	}
	
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxGridViewContainer* CGlxGridViewContainer::NewLC(MGlxMediaList *aMediaList,
														CGlxUiUtility* aUiUtility,
																	MGlxGridEventObserver& aObserver)
	{
	TRACER("CGlxGridViewContainer::NewLC");
	CGlxGridViewContainer* self = new(ELeave) CGlxGridViewContainer(aMediaList,aUiUtility,aObserver);
	CleanupStack::PushL(self ) ;
	self->ConstructL() ;
	return self ;
	}
	
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxGridViewContainer::~CGlxGridViewContainer()
	{
	TRACER("CGlxGridViewContainer::~CGlxGridViewContainer");
	if(iBgContext)
		{
		delete iBgContext;	
		}
	if(iDRMUtility)
		{
		iDRMUtility->Close();
		}
	if (iGlxGridMLObserver)
		{
		delete iGlxGridMLObserver;
		iGlxGridMLObserver = NULL;
		}
	if (iMediaList)
		{
		iMediaList->RemoveContext(iThumbnailContext);
		delete iThumbnailContext;
		}
	if(iNavigationalstate)
		{
		iNavigationalstate->Close();
		}
	if (iHgGrid)
		{
		delete iHgGrid;
		iHgGrid = NULL;
		}
	//reset the flag
	iBackwardActivation = EFalse;
	}
	
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxGridViewContainer::CGlxGridViewContainer(MGlxMediaList *aMediaList,CGlxUiUtility* aUiUtility,
																MGlxGridEventObserver& aObserver)
					: iMediaList(aMediaList),iUiUtility(aUiUtility),iGlxGridViewObserver(aObserver)
		{
		TRACER("CGlxGridViewContainer::CGlxGridViewContainer");
		}
	
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::ConstructL()
	{
	TRACER("CGlxGridViewContainer::ConstructL()");

	//create the container window.
	CreateWindowL();

	// Making the default FSActivation as false
	iFullscreenViewActivated = EFalse;

	iNavigationalstate = CGlxNavigationalState::InstanceL();

	//check the navigational direction
	if (iUiUtility->ViewNavigationDirection ()== EGlxNavigationBackwards)
		{
		iBackwardActivation = ETrue;
		}

	// making the default mode of marking as false
	iMultipleMarkingActive = EFalse;

	iGridIconSize = iUiUtility->GetGridIconSize();

	iItemsPerPage = iUiUtility->VisibleItemsInPageGranularityL();

	// For DRM Utility
	iDRMUtility = CGlxDRMUtility::InstanceL();

	// background Skin Context for the skin support
	TRect apRect = iEikonEnv->EikAppUi()->ApplicationRect();
	iBgContext = CAknsBasicBackgroundControlContext::NewL(
			KAknsIIDQsnBgScreen,apRect,ETrue);

	iQualityTnAttrib = TMPXAttribute (KGlxMediaIdThumbnail,
			GlxFullThumbnailAttributeId( ETrue,  iGridIconSize.iWidth,
					iGridIconSize.iHeight ) );

	iSpeedTnAttrib = TMPXAttribute (KGlxMediaIdThumbnail,
			GlxFullThumbnailAttributeId( EFalse,  iGridIconSize.iWidth,
					iGridIconSize.iHeight ) );
	CreateGridL();
	}

// ---------------------------------------------------------------------------
// From OfferKeyEventL
// Default implementation,
// ---------------------------------------------------------------------------
//
TKeyResponse CGlxGridViewContainer::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
	{
	TRACER("CGlxGridViewContainer::OfferKeyEventL()");
	TKeyResponse response = EKeyWasNotConsumed;
	CCoeControl::OfferKeyEventL(aKeyEvent,aType);
	if(aType == EEventKey)
		{
		switch(aKeyEvent.iCode)
			{
			case EKeyBackspace:
				{
				iGlxGridViewObserver.HandleGridEventsL(EGlxCmdDelete);
				response = EKeyWasConsumed;
				break;
				}
			default:
				response = EKeyWasNotConsumed;
				break;
			}
		}
	
	//send the unconsumed events from grid container to hg-grid to handle related key events
	if(response == EKeyWasNotConsumed)
		{
		response = iHgGrid->OfferKeyEventL(aKeyEvent,aType);
		}
	return response ;
	}

// ---------------------------------------------------------------------------
// Release
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::Release(TInt /*aBufferStart*/, TInt/* aBufferEnd*/)
	{
	TRACER("CGlxGridViewContainer::Release()");
	// This has to do nothing on default
	}

// ---------------------------------------------------------------------------
// Request
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::Request(TInt aRequestStart, TInt aRequestEnd,
		THgScrollDirection /*aDirection*/)
	{
	TRACER("CGlxGridViewContainer::Request()");
	RequestL(aRequestStart, aRequestEnd);
	}

// ---------------------------------------------------------------------------
// RequestL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::RequestL(TInt aRequestStart, TInt aRequestEnd)
	{
	TRACER("CGlxGridViewContainer::RequestL()");
	TInt visIndex = 0;
	TInt mediaCount = iMediaList->Count();
	aRequestStart = (aRequestStart<0 ? 0 : aRequestStart) ;
	aRequestEnd = (aRequestEnd>=mediaCount? (mediaCount-1) : aRequestEnd);

	GLX_LOG_INFO3("CGlxGridViewContainer::RequestL - aRequestStart(%d), "
			"aRequestEnd(%d), FirstIndexOnScreen(%d)", aRequestStart,
			aRequestEnd, iHgGrid->FirstIndexOnScreen());
	GLX_LOG_INFO1("CGlxGridViewContainer::Request - mediaCount(%d)",mediaCount);

	visIndex = iHgGrid->FirstIndexOnScreen();

	if(visIndex >= mediaCount )
		{
		visIndex = mediaCount-1;
		}
	if(visIndex<0 || mediaCount<=0)
		{
		visIndex = 0;
		}
	GLX_LOG_INFO1("CGlxGridViewContainer::Request - SetVisibleWindowIndex"
			" visIndex(%d)", visIndex);
	iMediaList->SetVisibleWindowIndexL(visIndex);

	if(mediaCount>0)
		{
		//Set the icons and refresh the screen only when the items are present
		for (TInt i=aRequestStart; i<= aRequestEnd; i++)
			{
			SetIconsL(i);
			TInt firstIndex = iHgGrid->FirstIndexOnScreen();
			firstIndex = (firstIndex<0 ? 0 : firstIndex);
			TInt lastOnScreen = firstIndex + iHgGrid->ItemsOnScreen() - 1;
			lastOnScreen = (lastOnScreen >mediaCount-1? mediaCount-1:lastOnScreen);
			if (i == lastOnScreen || (i == aRequestEnd && i < lastOnScreen))
				{
				if (HasRelevantThumbnail(i))
					{
					GLX_LOG_INFO1("CGlxGridViewContainer::Request - RefreshScreen"
							"  lastOnScreen / aRequestEnd i(%d)", i);
					iHgGrid->RefreshScreen(i);
					}
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// HasRelevantThumbnail
// ----------------------------------------------------------------------------
//
TBool CGlxGridViewContainer::HasRelevantThumbnail(TInt aIndex)
	{
	TRACER("CGlxGridViewContainer::HasRelevantThumbnail()");
	const TGlxMedia& item = iMediaList->Item( aIndex );
	const CGlxThumbnailAttribute* qualityTn = item.ThumbnailAttribute(
			iQualityTnAttrib );
	const CGlxThumbnailAttribute* speedTn = item.ThumbnailAttribute(
			iSpeedTnAttrib );
	if ( qualityTn || speedTn )
		{
		GLX_LOG_INFO("CGlxGridViewContainer::HasRelevantThumbnail() - TN avail");
		return ETrue;
		}
	return EFalse;
	}
    
// ---------------------------------------------------------------------------
// HandleSelectL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::HandleSelectL( TInt aIndex )
	{
    TRACER("CGlxGridViewContainer::HandleSelectL()");
	// Map the index of HG Grid and Medialist
	// Make sure that the Selection Index is inside medialist count
	if (aIndex <iMediaList->Count() && aIndex >=0)
		{
		iMediaList->SetFocusL(NGlxListDefs::EAbsolute, aIndex);
		}
	}

// ---------------------------------------------------------------------------
// HandleOpenL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::HandleOpenL( TInt aIndex )
	{
    TRACER("CGlxGridViewContainer::HandleOpenL()");

	// Make sure that the Selection Index is inside medialist count
	if (aIndex <iMediaList->Count() && aIndex >=0)
		{
		if (!(iHgGrid->Flags() && CHgScroller::EHgScrollerSelectionMode))
			{
			// If not in marking mode, then open FS view
			iUiUtility->SetViewNavigationDirection(EGlxNavigationForwards);
			//Navigate to the next view
			//Item is selected, go to next view
			HandleEnterKeyEventL((TInt)EAknCmdOpen);
			}
		}
	}
    
// -----------------------------------------------------------------------------
// HandleEnterKeyEventL
// -----------------------------------------------------------------------------
//
void CGlxGridViewContainer::HandleEnterKeyEventL(TInt aCommand)
	{
	TRACER("CGlxGridViewContainer::HandleEnterKeyEventL()");
	HandleViewCommandL(aCommand);
	}
    
// ---------------------------------------------------------------------------
// HandleMarkingL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::HandleMarkingL( TInt aIndex, TBool aMarked )
	{
	TRACER("CGlxGridViewContainer::HandleMarkingL()");
	HandleMultipleMarkingL(aIndex,aMarked);
	}

// ---------------------------------------------------------------------------
// CreateHgGridWidgetL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::CreateHgGridWidgetL()
	{
	TRACER("CGlxGridViewContainer::CreateHgGridWidgetL()");

	TInt mediaCount = iMediaList->Count();
	if (!iHgGrid)
		{
        TSize tnSize = CHgGrid::PreferredImageSize();
        GLX_DEBUG3("GlxGrid: CHgGrid::PreferredImageSize() w(%d) h(%d)", 
                tnSize.iWidth, tnSize.iHeight);
		TFileName resFile(KDC_APP_BITMAP_DIR);
		resFile.Append(KGlxIconsFilename);
		CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
				EMbmGlxiconsQgn_prop_image_notcreated);
        AknIconUtils::SetSize(bitmap, tnSize);
		TRect rect = iEikonEnv->EikAppUi()->ClientRect();
		// Create Hg grid object
		iHgGrid = CHgGrid::NewL (rect,mediaCount,CGulIcon::NewL(bitmap));
		}

	// Setting to MopParent to update background skin
	iHgGrid->SetMopParent(this);
	// Setting Selction observer for getting callback on key event change
	iHgGrid->SetSelectionObserver(*this);

	// This Displays the scrollbar at the opening of the Grid view
	iHgGrid->SetScrollBarTypeL(CHgScroller::EHgScrollerTimeStrip );

	// Enable Buffer support
	iHgGrid->EnableScrollBufferL(*this, (KNoOfPages * iItemsPerPage),
			KBufferTresholdSize);

	// Enable Marking support
	iHgGrid->SetMarkingObserver(*this);
	iHgGrid->SetFocus(ETrue);
	/*
	 * note: don't push hggrid to the control stack
	 */
	}

// ---------------------------------------------------------------------------
// CreateGridL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::CreateGridL()
	{
	TRACER("CGlxGridViewContainer::CreateGridL()");
	// Set the Grid thumbnail context and iterator
	SetGridThumbnailContextL();
	// Create HG Grid widget
	CreateHgGridWidgetL();
	// Create an observer and pass MediaList and HGGrid objects
	CreateGridMediaListObserverL();
	// Create Grid once again after returning from FS as No calls for handleItem added.
	CreateGridAfterFSDeactivatedL();
	}
	
// ---------------------------------------------------------------------------
// SetGridThumbnailContextL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::SetGridThumbnailContextL()
	{
	TRACER("CGlxGridViewContainer::SetGridThumbnailContextL()");
	// show static items again
	iMediaList->SetStaticItemsEnabled(ETrue);

	iThumbnailContext = CGlxThumbnailContext::NewL( &iBlockyIterator ); // set the thumbnail context
	iThumbnailContext->SetDefaultSpec( iGridIconSize.iWidth,iGridIconSize.iHeight );
	iMediaList->AddContextL(iThumbnailContext, KGlxFetchContextPriorityNormal );
	}
    
// ---------------------------------------------------------------------------
// CreateGridMediaListObserverL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::CreateGridMediaListObserverL()
	{
	TRACER("CGlxGridViewContainer::CreateGridMediaListObserverL()");
	// Creating the Medialist observer for HG Grid
	iGlxGridMLObserver = CGlxGridViewMLObserver::NewL(*iMediaList, iHgGrid);
	}
    
// ---------------------------------------------------------------------------
// CreateGridAfterFSDeactivatedL
// On Returning from the FS View, There is no calls to the handle attribut available,
// So repeating the code to view Grid on FS return
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::CreateGridAfterFSDeactivatedL()
	{
	TRACER("CGlxGridViewContainer::CreateGridAfterFSDeactivatedL()");
	if (iBackwardActivation)
		{
		TInt mlCount = iMediaList->Count();
		GLX_LOG_INFO1("CreateGridAfterFSDeactivatedL() mlCount=%d", mlCount);

		// Setting the Empty Text
		HBufC* emptyText =
		StringLoader::LoadLC(R_GRID_EMPTY_VIEW_TEXT);
		iHgGrid->SetEmptyTextL(*emptyText);
		CleanupStack::PopAndDestroy(emptyText);

		if (mlCount <= 0)
			{
			GLX_LOG_INFO("CreateGridAfterFSDeactivatedL() - SetEmptyTextL()");
			iHgGrid->DrawNow();
			}

		TInt focusIndex = iMediaList->FocusIndex();
		TSize setSize = CHgGrid::PreferredImageSize();
		TFileName resFile(KDC_APP_BITMAP_DIR);
		resFile.Append(KGlxIconsFilename);

		for (TInt index=0; index<mlCount; index++)
			{
			SetIconsL(index);
			}
			
		iHgGrid->SetSelectedIndex(focusIndex);
		iHgGrid->RefreshScreen(focusIndex);
		}
	iBackwardActivation = EFalse;
	}
 
// ---------------------------------------------------------------------------
// SetIconsL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::SetIconsL(TInt index)
	{
	TRACER("CGlxGridViewContainer::SetIconsL()");
	const TGlxMedia& item = iMediaList->Item(index);
	TSize setSize = CHgGrid::PreferredImageSize();
	TFileName resFile(KDC_APP_BITMAP_DIR);
	resFile.Append(KGlxIconsFilename);
	TIconInfo icon;
	TInt tnError = GlxErrorManager::HasAttributeErrorL(
			item.Properties(), KGlxMediaIdThumbnail );

	const CGlxThumbnailAttribute* qualityTn = item.ThumbnailAttribute( iQualityTnAttrib );
	const CGlxThumbnailAttribute* speedTn = item.ThumbnailAttribute( iSpeedTnAttrib );

	if (qualityTn)
		{
		CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
		bitmap->Duplicate( qualityTn->iBitmap->Handle());
		iHgGrid->ItemL(index).SetIcon(CGulIcon::NewL(bitmap));
		GLX_LOG_INFO1("### CGlxGridViewContainer::SetIconsL qualityTn-Index is %d",index);
		}
	else if (speedTn)
		{
		CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
		bitmap->Duplicate( speedTn->iBitmap->Handle());
		iHgGrid->ItemL(index).SetIcon(CGulIcon::NewL(bitmap));
		GLX_LOG_INFO1("### CGlxGridViewContainer::SetIconsL speedTn-Index is %d",index);
		}
	else if ( KErrNone != tnError && KErrNotSupported != tnError && KErrCANoRights !=tnError)
		{
		GLX_LOG_INFO2("CGlxGridViewContainer::SetIconsL - image_corrupted tnError(%d), i(%d)",
				tnError, index);
		CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
				EMbmGlxiconsQgn_prop_image_corrupted);
		AknIconUtils::SetSize(bitmap, setSize);
		iHgGrid->ItemL(index).SetIcon(CGulIcon::NewL(bitmap));
		}
	else if (KErrCANoRights ==tnError)
		{
		/*fix for EABI-7RKHDG
		 * this is a safe code added to show default
		 * TNM returns -17452 in case SD DRM files
		 * this code is added as coming from other view to
		 * gridview,it draws broken TN then the callback goes to
		 * glxgridviewobserver and redraws a not-created TN.
		 * with this part of code that TN swich will not be visible
		 */
		GLX_LOG_INFO2("CGlxGridViewContainer::SetIconsL - image_defaultthumbnail tnError(%d), i(%d)",
				tnError, index);
		CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
				EMbmGlxiconsQgn_prop_image_notcreated);
		AknIconUtils::SetSize(bitmap, setSize);
		iHgGrid->ItemL(index).SetIcon(CGulIcon::NewL(bitmap));
		}
	else if(KErrNotSupported == tnError)
		{
		GLX_LOG_INFO2("CGlxGridViewContainer::SetIconsL - image_defaultthumbnail tnError(%d), i(%d)",
				tnError, index);
		CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
				EMbmGlxiconsQgn_prop_image_corrupted);
		AknIconUtils::SetSize(bitmap, setSize);
		iHgGrid->ItemL(index).SetIcon(CGulIcon::NewL(bitmap));
		}

	if (item.IsDrmProtected())
		{
		const TDesC& uri = item.Uri();
		if( uri.Length() > 0)
			{
			if(iDRMUtility->IsForwardLockedL(uri))
				{
				/*
				 * fix for EABI-7RKHDG
				 * to show the invalid DRM icon
				 */
				TMPXGeneralCategory  cat = item.Category();
				TBool checkViewRights = (cat==EMPXImage);

				if(iDRMUtility->CheckOpenRightsL(uri, checkViewRights))
					{
					iHgGrid->ItemL(index).SetFlags(CHgItem::EHgItemFlagsDrmRightsValid);
					}
				else
					{
					iHgGrid->ItemL(index).SetFlags(CHgItem::EHgItemFlagsDrmRightsExpired);
					}
				}
			else
				{
				TMPXGeneralCategory  cat = item.Category();
				TBool checkViewRights = (cat==EMPXImage);

				if(iDRMUtility->CheckOpenRightsL(uri, checkViewRights))
					{
					iHgGrid->ItemL(index).SetFlags(CHgItem::EHgItemFlagsDrmRightsValid);
					}
				else
					{
					iHgGrid->ItemL(index).SetFlags(CHgItem::EHgItemFlagsDrmRightsExpired);
					}
				}
			}
		}
	TTime time(0);
	if (item.GetDate(time))
		{
		iHgGrid->ItemL(index).SetTime(time);
		}

	if (item.Category() == EMPXVideo)
		{
		iHgGrid->ItemL(index).SetFlags(CHgItem::EHgItemFlagsVideo);
		}
	}
   
        
// -----------------------------------------------------------------------------
// MopSupplyObject
// To handle Skin support
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CGlxGridViewContainer::MopSupplyObject(TTypeUid aId)
	{
	TRACER("CGlxGridViewContainer::MopSupplyObject()");
	if (iBgContext)
		{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
		}
	return CCoeControl::MopSupplyObject( aId );
	}

// ---------------------------------------------------------------------------
// HandleMultipleMarkingL
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::HandleMultipleMarkingL(TInt aIndex , TBool aMarked )
	{
	TRACER("CGlxGridViewContainer::HandleMultipleMarkingL()");
	//@ fix for EIZU-7RE43S
	if(!iMediaList->Item(aIndex).IsStatic() && iHgGrid)
		{
		if(aMarked)
			{
			iGlxGridViewObserver.HandleGridEventsL(EAknCmdMark) ;

			}
		else
			{
			iGlxGridViewObserver.HandleGridEventsL(EAknCmdUnmark) ;

			}
		}

	// This check is only implicated when multiple marking mode and we mark static items
	// As Hg Grid marks the items on the screen, we have to unmark the same.
	if (iMediaList->Item(aIndex).IsStatic())
		{
		iHgGrid->UnMark(aIndex);
		}
	}

// ---------------------------------------------------------------------------
// HandleViewCommandL
// Command handling function.
// ---------------------------------------------------------------------------
TBool CGlxGridViewContainer::HandleViewCommandL(TInt aCommand)
	{
	TRACER("CGlxGridViewContainer::HandleViewCommandL()");
	GLX_LOG_INFO1( "CGlxGridViewContainer::HandleViewCommandL(%x) entering", aCommand );

	TBool retVal = EFalse;
	TInt focusIndex = iMediaList->FocusIndex();
	switch (aCommand)
		{
		case EAknCmdOpen:
			{
			GLX_LOG_INFO( "EAknCmdOpen" );
			// Activate Fullscreen
			GLX_LOG_INFO("CGlxGridViewContainer::HandleViewCommandL - EAKnCmdOpen received");
			if ( !iFullscreenViewActivated && !iMultipleMarkingActive &&
					focusIndex != KErrNotFound)
				{
				const TGlxMedia& item = iMediaList->Item(focusIndex);
				const TDesC& uri = item.Uri();

				if ( item.IsStatic() )
					{
					TInt cmd = 0;
					if( item.GetStaticItemCommand(cmd) )
						{
						iGlxGridViewObserver.HandleGridEventsL(cmd) ;
						}
					}
				else if(item.Category() == EMPXVideo)
					{
					//Fix for ESLM-7U58CG - here since we get
					//multiple HandleOpen( ) event on multiple tap
					//we need to guard the event by a flag.
					if(!iIsFSVideoViewActivating)
						{
						iIsFSVideoViewActivating = ETrue;
						iFullscreenViewActivated = ETrue;
						iIsFSVideoViewActivating = EFalse;
						iGlxGridViewObserver.HandleGridEventsL(EGlxCmdPlay) ;
						}
					}
				else
					{
					iFullscreenViewActivated = ETrue;
					iNavigationalstate->SetToViewMode();
					}
				}

			//@ fix for EIZU-7RE43S
			if(iMultipleMarkingActive)
				{
				//If in marking mode Mark/Unmark the item
				//appropriately.
				if(iMediaList->IsSelected(focusIndex))
					{
					iGlxGridViewObserver.HandleGridEventsL(EAknCmdUnmark) ;
					}
				else
					{
					iGlxGridViewObserver.HandleGridEventsL(EAknCmdMark) ;
					}
				}
			retVal = ETrue;
			break;
			}

		case EAknCmdMark:
			{
			iHgGrid->SetFlags( CHgScroller::EHgScrollerSelectionMode );
			//Start Marking mode on grid.
			iMultipleMarkingActive = ETrue;
			//@ fix for EIZU-7RE43S
			iHgGrid->Mark(focusIndex);
			iHgGrid->RefreshScreen(focusIndex);
			retVal = ETrue;
			break;
			}

		case EAknCmdUnmark:
			{
			//@ fix for EIZU-7RE43S
			iHgGrid->UnMark(focusIndex);
			iHgGrid->RefreshScreen(focusIndex);
			retVal = ETrue;
			break;
			}

		case EAknMarkAll:
			{
			//Start Marking mode on grid.
			iMultipleMarkingActive = ETrue;
			iHgGrid->SetFlags( CHgScroller::EHgScrollerSelectionMode );
			iHgGrid->MarkAll();

			// If the 0th item is static, then it must be downloads,
			// so unmark 0th and 1st item
			// corresponding to image and vieo icons
			const TGlxMedia& item = iMediaList->Item(0);
			if (item.IsStatic())
				{
				// Assuming that the first two items are static
				iHgGrid->UnMark(0);
				iHgGrid->UnMark(1);
				}
			// When mark all after some item is marked using toolbar
			// it directly goes to commandhandler, so it returns not
			// consumed after marking medialist
			// and view marks on the display
			// The Check prevents looping from view to mark and vice versa
			// @ fix for ESLM-7TQGMP
			if (iMediaList->SelectionCount() < 1)
				{
				iGlxGridViewObserver.HandleGridEventsL(aCommand) ;
				}
			retVal = ETrue;
			break;
			}

		case EAknUnmarkAll:
			{
			if(iHgGrid)
				{
				iHgGrid->UnMarkAll();
				iHgGrid->RefreshScreen(iMediaList->Count()-1);
				if (!iMultipleMarkingActive)
					{
					iHgGrid->ClearFlags( CHgScroller::EHgScrollerSelectionMode );
					}
				}
			break;
			}

		case EGlxCmdEndMultipleMarking:
			{
			iMultipleMarkingActive = EFalse;
			iHgGrid->ClearFlags( CHgScroller::EHgScrollerSelectionMode );
			// Turn off the marking mode only if the event is from toolbar marking button.
			//Unlatch the mark toolbar button upon exiting the marking mode
			if(iEikonEnv->AppUiFactory()->ToolBar())
				{
				// This will unmark the items after adding an album/tag
				// Have to find a better solution and unmark only the items
				// marked and not all, might be a perf hit
				if(iHgGrid)
					{
					iHgGrid->UnMarkAll();
					}
				iGlxGridViewObserver.HandleLatchToolbar();
				}
			retVal = ETrue;
			break;
			}

		case EGlxCmdStartMultipleMarking:
			{
			if( iHgGrid->Flags() & CHgScroller::EHgScrollerSelectionMode )
				{
				iHgGrid->ClearFlags( CHgScroller::EHgScrollerSelectionMode );
				// Refreshing the items selected after unmark
				for (TInt i= 0;i<iMediaList->SelectionCount();i++)
					{
					TInt selectedItemIndex = iMediaList->SelectedItemIndex(i);
					iHgGrid->UnMark(selectedItemIndex);
					iHgGrid->RefreshScreen(selectedItemIndex);
					}
				iGlxGridViewObserver.HandleGridEventsL(EGlxCmdEndMultipleMarking) ;
				}
			else
				{
				//Start Marking mode on grid.
				iMultipleMarkingActive = ETrue;
				iHgGrid->SetFlags(CHgScroller::EHgScrollerSelectionMode);
				}
			retVal = ETrue;
			break;
			}

		case EAknSoftkeyCancel:
			{
			//@ fix for EIZU-7RE43S
			if( iHgGrid->Flags() & CHgScroller::EHgScrollerSelectionMode )
				{
				iGlxGridViewObserver.HandleGridEventsL(EGlxCmdEndMultipleMarking) ;
				}
			retVal = ETrue;
			break;

			}
		case EGlxCmdResetView:
			{
			//@ fix for ESLM-7VRGKH
			if(iHgGrid)
				{
				iHgGrid->InitScreenL(iEikonEnv->EikAppUi()->ClientRect());
				}
			retVal = ETrue;
			break;
			}
		default:
			break;
		}
	GLX_LOG_INFO("CGlxGridViewContainer::HandleViewCommandL() exiting");
	return retVal;
	}

// ---------------------------------------------------------------------------
// HandleResourceChange
// Handle BG screen context && Hg grid orientaion
// ---------------------------------------------------------------------------
//
void CGlxGridViewContainer::HandleResourceChange(TInt aId)
	{
	TRACER("CGlxGridViewContainer::HandleResourceChange()");
	CCoeControl::HandleResourceChange(aId);
	if ( iBgContext )
		{
		TRect apRect = iEikonEnv->EikAppUi()->ApplicationRect();
		iBgContext->SetRect( apRect );
		}
	if(iHgGrid)
		{
		TRAP_IGNORE (iHgGrid->InitScreenL(iEikonEnv->EikAppUi()->ClientRect()); )
		}
	}
//end of file

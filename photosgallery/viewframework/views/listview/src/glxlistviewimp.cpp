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
* Description:    Implementation of list view
*
*/




// INCLUDE FILES

#include "glxlistviewimp.h"
#include "glxpreviewthumbnailbinding.h"

#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <glxcollectiongeneraldefs.h>
#include <ganes/HgDoubleGraphicListFlat.h>
#include <ganes/HgItem.h>
#include <data_caging_path_literals.hrh>
#include <glxuistd.h>
#include <glxicons.mbg>
#include <gulicon.h>
#include <AknsBasicBackgroundControlContext.h>
#include <mglxmedialist.h>					//MGlxMediaList
#include <glxtracer.h>
#include <glxlog.h>							//Glx Logs
#include <glxlistviewplugin.rsg>
#include <aknViewAppUi.h>
#include <StringLoader.h>					//StringLoader		
#include <glxsetappstate.h>
#include <mglxcache.h>
#include <glxerrormanager.h>             // For CGlxErrormanager
#include <glxthumbnailcontext.h>
#include <glxthumbnailattributeinfo.h>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionplugintype.hrh>
#include <glxnavigationalstate.h>
#include <glxfiltergeneraldefs.h>

#include <akntranseffect.h>  // For transition effects
#include <gfxtranseffect/gfxtranseffect.h>  // For transition effects
#include "glxgfxtranseffect.h"  // For transition effects

const TInt KListDataWindowSize(8); // Visible page
const TInt KListNonVisibleDataWindowSize(32); // Visible page + 3 pages
const TInt KNoOfPages(4);

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CGlxListViewImp::CGlxListViewImp(TInt aViewUid,
        const TListViewResourceIds& aResourceIds) :
    iViewUid(aViewUid), iResourceIds(aResourceIds), iIsRefreshNeeded(EFalse),
    iBackwardNavigation(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxListViewImp* CGlxListViewImp::NewL(MGlxMediaListFactory* aMediaListFactory,
                                       TInt aViewUid, 
                                       const TListViewResourceIds& aResourceIds,                                        
                                       const TDesC& aTitle)
    {
    TRACER("CGlxListViewImp::NewL");
    
    CGlxListViewImp* self = CGlxListViewImp::NewLC(aMediaListFactory, aViewUid,
                                                                aResourceIds, aTitle);			
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxListViewImp* CGlxListViewImp::NewLC(MGlxMediaListFactory* aMediaListFactory,
                                        TInt aViewUid, 
                                        const TListViewResourceIds& aResourceIds,                                         
                                        const TDesC& aTitle)
    {	
    TRACER("CGlxListViewImp::NewLC");
    
    CGlxListViewImp* self = new (ELeave) CGlxListViewImp(aViewUid, aResourceIds);
    CleanupStack::PushL(self);
    self->ConstructL(aMediaListFactory, aTitle);
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CGlxListViewImp::ConstructL(MGlxMediaListFactory* aMediaListFactory, 
                                                            const TDesC& aTitle)
    {
    TRACER("CGlxListViewImp::ConsrtuctL");
    
    BaseConstructL(iResourceIds.iViewId);
    ViewBaseConstructL();
    MLViewBaseConstructL(aMediaListFactory, aTitle);
    
    // Create navigational state 
    iNavigationalState = CGlxNavigationalState::InstanceL();
    iNavigationalState->AddObserverL( *this );
 
    CAknToolbar* toolbar = Toolbar();
    if(toolbar)
        {
        toolbar->DisableToolbarL(ETrue);  
        }

    iGridIconSize = iUiUtility->GetGridIconSize();

    //Register the view to recieve toolbar events. ViewBase handles the events    
    SetToolbarObserver(this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxListViewImp::~CGlxListViewImp()
    {    
    TRACER("CGlxListViewImp::~CGlxListViewImp");
    
    if ( iNavigationalState )
        {
        iNavigationalState->RemoveObserver(*this);
        iNavigationalState->Close();
        }
    
    delete iTitletext;  
    }

// ---------------------------------------------------------------------------
// ControlGroupId
// ---------------------------------------------------------------------------
//
TInt CGlxListViewImp::ControlGroupId() const
    {
    TRACER("CGlxListViewImp::ControlGroupId()");
    return reinterpret_cast<TInt>(this);
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CGlxListViewImp::Id() const
    {
    TRACER("CGlxListViewImp::Id()");
    return TUid::Uid(iViewUid);
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CGlxListViewImp::DoMLViewActivateL(const TVwsViewId& /* aPrevViewId */, 
        TUid /* aCustomMessageId */, const TDesC8& /* aCustomMessage */)
    {
    TRACER("CGlxListViewImp::DoMLViewActivateL");  
    
    TUint transitionID = (iUiUtility->ViewNavigationDirection()==
         EGlxNavigationForwards)?KActivateTransitionId:KDeActivateTransitionId;
		
    //Do the activate animation only for views other than mainlist view and
	//on backward navigation from any other views to main list view, since 
	//for the app start the animation effect is by default provided.
    if (iMediaList->IdSpaceId(0) != KGlxIdSpaceIdRoot || 
           transitionID == KDeActivateTransitionId) 
        {
    	GfxTransEffect::BeginFullScreen( transitionID, TRect(),
                                   AknTransEffect::EParameterType, 
                         AknTransEffect::GfxTransParam( KPhotosUid, 
                         AknTransEffect::TParameter::EEnableEffects) );   
    	GfxTransEffect::EndFullScreen();
    	}
    
    iNextViewActivationEnabled = ETrue;
    if(StatusPane())
        {
        StatusPane()->MakeVisible(ETrue);
        CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
        CAknTitlePane* titlePane = ( CAknTitlePane* )statusPane->ControlL(
                TUid::Uid( EEikStatusPaneUidTitle ));     
        const TDesC* titletext = titlePane->Text();
        HBufC* temptitle = titletext->AllocLC();
        TPtr titleptr = temptitle->Des();
        titleptr.Trim();
        if(!temptitle->Length())
            {
            if( iTitletext )
                {
                // Set the required Title
                titlePane->SetTextL( *iTitletext );
                }
            }
        CleanupStack::PopAndDestroy(temptitle);        
        }

	GlxSetAppState::SetState(EGlxInListView);

    // Separate contexts so that some attributes are loaded before others
    // No need to fetch the rear pages as we do not support looping	
    iThumbnailIterator.SetRangeOffsets(0, KListDataWindowSize);

    iTitleAttributeContext = CGlxDefaultAttributeContext::NewL();
    iTitleAttributeContext->SetRangeOffsets(0, KMaxTInt);
    iTitleAttributeContext->AddAttributeL(KMPXMediaGeneralTitle);

    iSubtitleAttributeContext = new (ELeave) CGlxAttributeContext(
            &iThumbnailIterator);
    iSubtitleAttributeContext->AddAttributeL(
            KGlxMediaCollectionPluginSpecificSubTitle);

    iMediaList->AddContextL(iTitleAttributeContext, KMaxTInt);
    iMediaList->AddContextL(iSubtitleAttributeContext, KGlxFetchContextPriorityNormal);
    if (iMediaList->IdSpaceId(0) != KGlxIdSpaceIdRoot)
        {
        iNonVisibleThumbnailIterator.SetRangeOffsets(KListDataWindowSize,
                KListNonVisibleDataWindowSize);
        iNonVisibleSubtitleAttributeContext
                = new (ELeave) CGlxAttributeContext(
                        &iNonVisibleThumbnailIterator);
        iNonVisibleSubtitleAttributeContext->AddAttributeL(
                KGlxMediaCollectionPluginSpecificSubTitle);
        iMediaList->AddContextL(iNonVisibleSubtitleAttributeContext,
                KGlxFetchContextPriorityLow);

        CMPXCollectionPath* path = iMediaList->PathLC(
                NGlxListDefs::EPathParent);
        if (path->Id(0) == KGlxCollectionPluginAlbumsImplementationUid)
            {
            iOtherAttribsContext = new (ELeave) CGlxAttributeContext(
                &iThumbnailIterator);
            iOtherAttribsContext->AddAttributeL(KGlxMediaGeneralSystemItem);
            iOtherAttribsContext->AddAttributeL(
                    KGlxMediaGeneralSlideshowableContent);
            iMediaList->AddContextL(iOtherAttribsContext,
                    KGlxFetchContextPriorityNormal);

            iNonVisibleOtherAttribsContext = new (ELeave) CGlxAttributeContext(
                &iNonVisibleThumbnailIterator);
            iNonVisibleOtherAttribsContext->AddAttributeL(
                    KGlxMediaGeneralSystemItem);
            iNonVisibleOtherAttribsContext->AddAttributeL(
                    KGlxMediaGeneralSlideshowableContent);
            iMediaList->AddContextL(iNonVisibleOtherAttribsContext,
                    KGlxFetchContextPriorityLow);
            }
        CleanupStack::PopAndDestroy(path);

        iThumbnailContext = new (ELeave) CGlxAttributeContext(
                &iThumbnailIterator);
        TMPXAttribute tnAttr(KGlxMediaIdThumbnail,
                GlxFullThumbnailAttributeId(ETrue, iGridIconSize.iWidth,
                        iGridIconSize.iHeight));
        iThumbnailContext->SetDefaultSpec(iGridIconSize.iWidth,
                iGridIconSize.iHeight);
        iThumbnailContext->AddAttributeL(tnAttr);
        iMediaList->AddContextL(iThumbnailContext,
                KGlxFetchContextPriorityNormal);

        iNonVisibleThumbnailContext = new (ELeave) CGlxAttributeContext(
                &iNonVisibleThumbnailIterator);
        iNonVisibleThumbnailContext->SetDefaultSpec(iGridIconSize.iWidth,
                iGridIconSize.iHeight);
        iNonVisibleThumbnailContext->AddAttributeL(tnAttr);
        iMediaList->AddContextL(iNonVisibleThumbnailContext,
                KGlxFetchContextPriorityLow);
        }

    iMediaList->AddMediaListObserverL(this);
    TRect apRect = iEikonEnv->EikAppUi()->ApplicationRect();
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgScreen,apRect,ETrue);
    
    iBackwardNavigation = (iUiUtility->ViewNavigationDirection()
            == EGlxNavigationBackwards);
    GLX_LOG_INFO1("CGlxListViewImp::DoMLViewActivateL() - "
            "iBackwardNavigation(%d)", iBackwardNavigation);

    iPreviewTNBinding = CGlxPreviewThumbnailBinding::NewL(*this);
    CreateListL();
    iProgressIndicator = CGlxProgressIndicator::NewL(*this);
    iMMCNotifier = CGlxMMCNotifier::NewL(*this);
    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CGlxListViewImp::DoMLViewDeactivate()
    {   
    TRACER("CGlxListViewImp::DoMLViewDeactivate");  
    if(StatusPane())
       {
       if(iTitletext)
          {
          delete iTitletext;
          iTitletext = NULL;
          }
       CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
       TRAP_IGNORE(CAknTitlePane* titlePane = ( CAknTitlePane* )statusPane->ControlL(
               TUid::Uid( EEikStatusPaneUidTitle ));        
       iTitletext = titlePane->Text()->AllocL());
       }

    RemoveContext();
	iMediaList->RemoveMediaListObserver( this );
	delete iPreviewTNBinding;
	iPreviewTNBinding = NULL;
	DestroyListViewWidget();
	if(iProgressIndicator)
	    {
        delete iProgressIndicator;
        iProgressIndicator = NULL;
	    }
	
	delete iMMCNotifier;
	iMMCNotifier = NULL;
	}

// ---------------------------------------------------------------------------
// RemoveContext
// ---------------------------------------------------------------------------
//
void CGlxListViewImp::RemoveContext()
    {
    TRACER("CGlxListViewImp::DoMLViewDeactivate");

    if (iTitleAttributeContext)
        {
        iMediaList->RemoveContext(iTitleAttributeContext);
        delete iTitleAttributeContext;
        iTitleAttributeContext = NULL;
        }

    if (iSubtitleAttributeContext)
        {
        iMediaList->RemoveContext(iSubtitleAttributeContext);
        delete iSubtitleAttributeContext;
        iSubtitleAttributeContext = NULL;
        }

    if (iMediaList->IdSpaceId(0) != KGlxIdSpaceIdRoot)
        {
        if (iOtherAttribsContext)
            {
            iMediaList->RemoveContext(iOtherAttribsContext);
            delete iOtherAttribsContext;
            iOtherAttribsContext = NULL;
            }

        if (iThumbnailContext)
            {
            iMediaList->RemoveContext(iThumbnailContext);
            delete iThumbnailContext;
            iThumbnailContext = NULL;
            }

        if (iNonVisibleSubtitleAttributeContext)
            {
            iMediaList->RemoveContext(iNonVisibleSubtitleAttributeContext);
            delete iNonVisibleSubtitleAttributeContext;
            iNonVisibleSubtitleAttributeContext = NULL;
            }

        if (iNonVisibleOtherAttribsContext)
            {
            iMediaList->RemoveContext(iNonVisibleOtherAttribsContext);
            delete iNonVisibleOtherAttribsContext;
            iNonVisibleOtherAttribsContext = NULL;
            }

        if (iNonVisibleThumbnailContext)
            {
            iMediaList->RemoveContext(iNonVisibleThumbnailContext);
            delete iNonVisibleThumbnailContext;
            iNonVisibleThumbnailContext = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// Destroys the Widget
// ---------------------------------------------------------------------------
//
void CGlxListViewImp::DestroyListViewWidget()
	{
	TRACER("CGlxListViewImp::DestroyListViewWidget");
	 
    if (iList)
        {
   	    AppUi()->RemoveFromStack(iList);
        delete iList;
        iList = NULL;
        }
	delete iBgContext;
	iBgContext = NULL;
	if(iBackwardNavigation)
		{
		iLastFocusedIndex = 0;
		}
	}

// ---------------------------------------------------------------------------
// From CGlxViewBase
// Command handling function.
// ---------------------------------------------------------------------------
//
TBool CGlxListViewImp::HandleViewCommandL(TInt aCommand)
    {
	TRACER("CGlxListViewImp::HandleViewCommandL");  
    
    if (aCommand == EAknCmdOpen)
    	{
    	//Navigational State to be used
        if( iNextViewActivationEnabled ) 
            {
            iNextViewActivationEnabled = EFalse;
            iUiUtility->SetViewNavigationDirection(EGlxNavigationForwards); 
            //Navigate to the next view
            CMPXCollectionPath* path =
            iMediaList->PathLC( NGlxListDefs::EPathFocusOrSelection );
            iCollectionUtility->Collection().OpenL(*path);
            CleanupStack::PopAndDestroy(path);  
            }
        return ETrue;
    	}
    else
    	{
    	return EFalse;
    	}
    }

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CGlxListViewImp::HandleStatusPaneSizeChange ( )
    {
    if ( iBgContext )
        {
        TRect apRect = iEikonEnv->EikAppUi()->ApplicationRect();
        iBgContext->SetRect( apRect );
        }
    
    if(iList)
        {
        TRAP_IGNORE (iList->InitScreenL(ClientRect()); )
        }
    }

TTypeUid::Ptr CGlxListViewImp::MopSupplyObject(TTypeUid aId)
	{   
    if (iBgContext && aId.iUid == MAknsControlContext::ETypeId)
    	{
        return MAknsControlContext::SupplyMopObject(aId, iBgContext );
    	}
    
    return CAknView::MopSupplyObject(aId);
	}

void CGlxListViewImp::Request(TInt aRequestStart, TInt aRequestEnd, 
        THgScrollDirection /*aDirection*/)
    {
    TRACER("CGlxListViewImp::Request");
    //Doing this to avoid codescanner warning.
    TRAP_IGNORE(RequestL(aRequestStart, aRequestEnd));    
    }

// ---------------------------------------------------------------------------
// RequestL
// ---------------------------------------------------------------------------
//
void CGlxListViewImp::RequestL(TInt aRequestStart, TInt aRequestEnd)
    {
    TRACER("CGlxListViewImp::RequestL()");
    TInt mediaCount = iMediaList->Count();
    GLX_LOG_INFO1("CGlxListViewImp::RequestL - mediaCount(%d)", mediaCount);
    if(mediaCount<=0)
        {
        return;
        }
    
    TInt visIndex = iList->FirstIndexOnScreen();
    aRequestStart = (aRequestStart < 0 ? 0 : aRequestStart) ;
    aRequestEnd = (aRequestEnd >= mediaCount ? (mediaCount-1) : aRequestEnd);
    GLX_LOG_INFO3("CGlxListViewImp::RequestL - aRequestStart(%d), "
       "aRequestEnd(%d), visIndex(%d)", aRequestStart, aRequestEnd, visIndex);

    if (visIndex >= mediaCount )
        {
        visIndex = mediaCount-1;
        }
    if (visIndex < 0 || mediaCount <= 0)
        {
        visIndex = 0;
        }

    for (TInt i=aRequestStart; i<= aRequestEnd; i++)
        {
        const TGlxMedia& item = iMediaList->Item(i);
        iList->ItemL(i).SetTitleL(item.Title());
        iList->ItemL(i).SetTextL(item.SubTitle());
        if(iMediaList->IdSpaceId(0) != KGlxIdSpaceIdRoot)
            {
            UpdatePreviewL(i);
            }
        }
    
    if (iMediaList->IdSpaceId(0) == KGlxIdSpaceIdRoot && iBackwardNavigation)
        {
        iMediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);
        CMPXCollectionPath* path = iMediaList->PathLC(
                NGlxListDefs::EPathFocusOrSelection);
        GLX_LOG_INFO("CGlxListViewImp::RequestL() - HandleItemChangedL()");
        iPreviewTNBinding->HandleItemChangedL(*path, iIsRefreshNeeded,
                iBackwardNavigation);
        CleanupStack::PopAndDestroy(path);
        }
    else
        {
        iMediaList->SetFocusL(NGlxListDefs::EAbsolute, visIndex);
        }
    }

void CGlxListViewImp::Release(TInt /*aBufferStart*/, TInt /*aBufferEnd*/)
    {
    TRACER("CGlxListViewImp::Release");
    }

void CGlxListViewImp::HandleSelectL(TInt aIndex)
    {
    TRACER("CGlxListViewImp::HandleSelectL");
    if (0 <= aIndex && aIndex < iMediaList->Count())
        {
        iMediaList->SetFocusL(NGlxListDefs::EAbsolute, aIndex);
        }
    }

void CGlxListViewImp::HandleOpenL( TInt aIndex )
    {
    TRACER("CGlxListViewImp::HandleOpenL");
    
    GLX_LOG_INFO1("CGlxListViewImp RProperty::Get leftVariable %d",
            (iUiUtility->GetItemsLeftCount()));

    if ((iUiUtility->GetItemsLeftCount() == KErrNotReady)
            || (iUiUtility->GetItemsLeftCount()))
        {
        if (!iProgressIndicator)
            {
            iProgressIndicator = CGlxProgressIndicator::NewL(*this);
            }
        iProgressIndicator->ShowProgressbarL();
        if (iSchedulerWait)
            {
            delete iSchedulerWait;
            iSchedulerWait = NULL;
            }
        iSchedulerWait = new (ELeave) CActiveSchedulerWait();
        iSchedulerWait->Start();
        }

    if (iNextViewActivationEnabled && (aIndex >= 0 && aIndex
            < iMediaList->Count()) && (iUiUtility->GetItemsLeftCount() == 0))
        {
        iMediaList->CancelPreviousRequests();
        //Delete the PreviewTNMBinding as in forward navigation
        //we do not get the medialist callback.
        delete iPreviewTNBinding;
        iPreviewTNBinding = NULL;
        
        iMediaList->SetFocusL(NGlxListDefs::EAbsolute,aIndex);
        iLastFocusedIndex = iMediaList->FocusIndex();
	    iNextViewActivationEnabled = EFalse;
		
		//Navigate to the next view
        iUiUtility->SetViewNavigationDirection(EGlxNavigationForwards);
        GLX_LOG_INFO("CGlxListViewImp::HandleOpenL()- EGlxNavigationForwards!");
        CMPXCollectionPath* path = iMediaList->PathLC(
                NGlxListDefs::EPathFocusOrSelection);

        // When a collection is opened for browsing, 
        // there are two queries executed with similar filter. 
        // First query to open the collection from list / cloud view.
        // Second one from grid/list view construction. To improve the grid/list 
        // opening performance, the first query will be completed with empty Id list.
        RArray<TMPXAttribute> attributeArray;
        CleanupClosePushL(attributeArray);
        attributeArray.AppendL(KGlxFilterGeneralNavigationalStateOnly);
        iCollectionUtility->Collection().OpenL(*path, attributeArray.Array());
        CleanupStack::PopAndDestroy(&attributeArray);
        CleanupStack::PopAndDestroy(path);
	    }
    }

void CGlxListViewImp::PreviewTNReadyL(CFbsBitmap* aBitmap, CFbsBitmap* 
        /*aMask*/)
    {
    TRACER("CGlxListViewImp::PreviewTNReadyL");

    iPreviewTNBinding->StopTimer();

    if (iMediaList->FocusIndex() != EGlxListItemAll || iMediaList->IdSpaceId(
            0) != KGlxIdSpaceIdRoot)
        {
        GLX_LOG_INFO("CGlxListViewImp::PreviewTNReadyL()- Ignore!");
        return;
        }

    if (aBitmap)
        {
        iList->ItemL(EGlxListItemAll).SetIcon(CGulIcon::NewL(aBitmap));
        }
    else
        {
        // In main list view, default thumbnails will be set according 
        // to the list items.
        //Displays default thumbnail if aBitmap is NULL 
        SetDefaultThumbnailL(EGlxListItemAll);
        }

    iList->RefreshScreen(EGlxListItemAll);
    }
    
// ----------------------------------------------------------------------------
// CreateListL
// ----------------------------------------------------------------------------
// 
void CGlxListViewImp::CreateListL()
    {
    TRACER("CGlxListViewImp::CreateListL");
    
	TInt mediaCount = iMediaList->Count();
    GLX_DEBUG2("CGlxListViewImp::CreateListL() mediaCount=%d", mediaCount);
	
	if (!iList)
		{
	    TFileName resFile(KDC_APP_BITMAP_DIR);
		resFile.Append(KGlxIconsFilename);
        
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        TInt bitmapId = 0;
        TInt maskId = 0;
        CMPXCollectionPath* path = iMediaList->PathLC(
                NGlxListDefs::EPathParent);
        if (path->Id() == KGlxCollectionPluginAlbumsImplementationUid)
            {           
            bitmapId = EMbmGlxiconsQgn_prop_photo_album_large;
            maskId = EMbmGlxiconsQgn_prop_photo_album_large_mask;
            }
        else
            {
            bitmapId = EMbmGlxiconsQgn_prop_image_notcreated;
            maskId = EMbmGlxiconsQgn_prop_image_notcreated_mask;
            }

        AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), KAknsIIDNone,
                bitmap, mask, resFile, bitmapId, maskId);
        __ASSERT_DEBUG(bitmap, Panic(EGlxPanicNullPointer));
        __ASSERT_DEBUG(mask, Panic(EGlxPanicNullPointer));

        AknIconUtils::SetSize(bitmap,
                CHgDoubleGraphicListFlat::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);
        AknIconUtils::SetSize(mask,
                CHgDoubleGraphicListFlat::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);

        iList = CHgDoubleGraphicListFlat::NewL(ClientRect(), mediaCount,
                CGulIcon::NewL(bitmap, mask));
        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap); 
	    iList->SetMopParent(this);
	    
		iList->SetSelectionObserver(*this);
		
		TInt items = iList->ItemsOnScreen();
        // Enable Buffer support
        iList->EnableScrollBufferL(*this, KNoOfPages * items, items);
	
        if (iBackwardNavigation)
            {
            GLX_DEBUG1("CGlxListViewImp::CreateListL() - SetEmptyTextL()");			
            //set the text to be shown if the list is empty.
            HBufC* emptyText = StringLoader::LoadLC(R_LIST_EMPTY_VIEW_TEXT); 
            iList->SetEmptyTextL(*emptyText);
            CleanupStack::PopAndDestroy(emptyText);
            
            //While coming back to main listview
            TGlxIdSpaceId id = iMediaList->IdSpaceId(0);
            if((id == KGlxIdSpaceIdRoot) && (mediaCount > 0))
                {            
                for (TInt i = 0; i < mediaCount; i++)
                    {
                    SetDefaultThumbnailL(i);
                    }
                }            
            }		
        
		//Fix for ESLM-7SAHPT::Clear Flag to Disable QWERTY search input in list view
		iList->ClearFlags(CHgScroller::EHgScrollerSearchWithQWERTY ); 
		
        // Set the scrollbar type for albums list
        if (path->Id() == KGlxCollectionPluginAlbumsImplementationUid)
            {
            iList->SetScrollBarTypeL(CHgScroller::EHgScrollerLetterStripLite );
            }
        CleanupStack::PopAndDestroy(path);
        
		AppUi()->AddToStackL(iList);
		}
		
	if (mediaCount)
	    {
		for (TInt i=0; i<mediaCount; i++)
			{
		   	const TGlxMedia& item = iMediaList->Item(i);
			iList->ItemL(i).SetTitleL(item.Title());
			iList->ItemL(i).SetTextL(item.SubTitle());
			}
		GLX_DEBUG3("CGlxListViewImp::CreateListL() Medialist Count = %d, "
		        "iLastFocusIndex %d",mediaCount,iLastFocusedIndex);
        if(iLastFocusedIndex >= mediaCount)
            {
            iLastFocusedIndex = (mediaCount - 1);
            }
		iMediaList->SetFocusL(NGlxListDefs::EAbsolute, iLastFocusedIndex);
		iList->SetSelectedIndex(iLastFocusedIndex);
		}
    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxListViewImp::HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, 
     MGlxMediaList* aList )
    {
    TRACER("CGlxListViewImp::HandleItemAddedL");    
    
    if (iList && aList)
        {            
        iList->ResizeL(aList->Count());            
        for (TInt i = aStartIndex; i<= aEndIndex; i++)
            {
            const TGlxMedia& item = iMediaList->Item(i);                
            iList->ItemL(i).SetTitleL(item.Title());
            iList->ItemL(i).SetTextL(item.SubTitle());
            if (iMediaList->IdSpaceId(0) == KGlxIdSpaceIdRoot)
                {
                SetDefaultThumbnailL(i);
                }
            }            
        
        if(aStartIndex == aEndIndex )
            {
            iLastFocusedIndex = aStartIndex;
            iMediaList->SetFocusL(NGlxListDefs::EAbsolute, iLastFocusedIndex);			
            iList->SetSelectedIndex(iLastFocusedIndex); 
            iList->RefreshScreen(iLastFocusedIndex);
            }
        }
    }

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, 
     MGlxMediaList* aList )
	{
	TRACER("CGlxListViewImp::HandleItemRemovedL");
	
	if(iMediaList == aList)
		{
		for(TInt i = aEndIndex ;i >= aStartIndex; i-- )
			{
			iList->RemoveItem(i);
			}

		if(aEndIndex >= iList->ItemCount())
			{
			iList->SetSelectedIndex(iList->ItemCount()-1);
			}
        iList->RefreshScreen(iList->FirstIndexOnScreen());
		}
	}

// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::HandleAttributesAvailableL( TInt aItemIndex, 
	const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/)
	{
	TRACER("CGlxListViewImp::HandleAttributesAvailableL");

	TMPXAttribute titleAttrib(KMPXMediaGeneralTitle);
	TMPXAttribute subTitleAttrib(KGlxMediaCollectionPluginSpecificSubTitle);
	TIdentityRelation<TMPXAttribute> match(&TMPXAttribute::Match);

	const TGlxMedia& item = iMediaList->Item(aItemIndex);

	if (KErrNotFound != aAttributes.Find(titleAttrib, match))
		{
		iList->ItemL(aItemIndex).SetTitleL(item.Title());
        if (iMediaList->IdSpaceId(0) != KGlxIdSpaceIdRoot)
            {
            iList->RefreshScreen(aItemIndex);
            }		
		}

	if (KErrNotFound != aAttributes.Find(subTitleAttrib, match))
		{
		iList->ItemL(aItemIndex).SetTextL(item.SubTitle());
		
	    if (iMediaList->IdSpaceId(0) == KGlxIdSpaceIdRoot)
            {
            RefreshList(aItemIndex);
            }
        else
            {
            iList->RefreshScreen(aItemIndex);
            }
		
		// If there is some modified in grid/fullscreen view,
		// HandleAttributesAvailableL will get called. Here we are setting
		// iIsRefreshNeeded flag to ETrue to cleanup once!		
		if (!iIsRefreshNeeded && iBackwardNavigation)
			{
            if (iMediaList->IdSpaceId(0) != KGlxIdSpaceIdRoot)
                {
                iIsRefreshNeeded = ETrue;
                CleanUpL();
                }
            }
		// Refresh thumbnail, if subtitle is updated for the All collection
		if (iMediaList->IdSpaceId(0) == KGlxIdSpaceIdRoot &&
		        iPreviewTNBinding && aItemIndex == 0)
            {
            if (iBackwardNavigation)
                {
                iIsRefreshNeeded = ETrue;
                }
            iMediaList->SetFocusL(NGlxListDefs::EAbsolute, 0);
			CMPXCollectionPath* path = iMediaList->PathLC(
					NGlxListDefs::EPathFocusOrSelection);
			GLX_LOG_INFO("CGlxListViewImp::HandleAttributesAvailableL() - HandleItemChangedL()");
			iPreviewTNBinding->HandleItemChangedL(*path, iIsRefreshNeeded,
                    iBackwardNavigation);
			CleanupStack::PopAndDestroy(path);		            	
			}
		}

	if (iMediaList->IdSpaceId(0) != KGlxIdSpaceIdRoot)
		{
        TMPXAttribute countAttrib(KMPXMediaGeneralCount);
        TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail,
                GlxFullThumbnailAttributeId(ETrue, iGridIconSize.iWidth,
                        iGridIconSize.iHeight));
        if (iIsRefreshNeeded && KErrNotFound != aAttributes.Find(
                thumbnailAttribute, match))
            {
            iIsRefreshNeeded = EFalse;
            }

        if ((KErrNotFound != aAttributes.Find(thumbnailAttribute, match))
                || (KErrNotFound != aAttributes.Find(countAttrib, match)))
            {
            UpdatePreviewL(aItemIndex);
            }
        }
	}
	
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::HandleFocusChangedL( NGlxListDefs::
	TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/,
	MGlxMediaList* /*aList*/ )
	{
	}

// ----------------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::HandleItemSelectedL(TInt /*aIndex*/, 
	TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
	{
	}

// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
 void CGlxListViewImp::HandleMessageL( const CMPXMessage& /*aMessage*/, 
	MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxListViewImp::HandleError( TInt aError ) 
    {
	TRACER( "CGlxListViewImp::HandleError");
	TRAP_IGNORE( DoHandleErrorL( aError ) );
    }

// ----------------------------------------------------------------------------
// DoHandleErrorL
// ----------------------------------------------------------------------------
//
void CGlxListViewImp::DoHandleErrorL(TInt /*aError*/)
    {
    TRACER( "CGlxListViewImp::DoHandleErrorL");
    for (TInt i = 0; i < iMediaList->Count(); i++)
        {
        const TGlxMedia& item = iMediaList->Item(i);
        TInt tnError = GlxErrorManager::HasAttributeErrorL(item.Properties(),
                KGlxMediaIdThumbnail);
        if (tnError != KErrNone)
            {
            SetDefaultThumbnailL(i);
            }
        }
    }
  
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxListViewImp::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, 
	TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxListViewImp::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandlePopulatedL
// ----------------------------------------------------------------------------
//
void CGlxListViewImp::HandlePopulatedL( MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxListViewImp::HandlePopulatedL()");
    if (iList)
        {
        //set the text to be shown if the list is empty.
        HBufC* emptyText = StringLoader::LoadLC(R_LIST_EMPTY_VIEW_TEXT); 
        iList->SetEmptyTextL(*emptyText);
        CleanupStack::PopAndDestroy(emptyText);

        GLX_DEBUG2("CGlxListViewImp::HandlePopulatedL() MediaList Count()=%d",
                                                        iMediaList->Count());
        if (iMediaList->Count() <= 0)
            {
            GLX_DEBUG1("CGlxListViewImp::HandlePopulatedL() - SetEmptyTextL()");
            iList->DrawNow();
            }
        }
    }

// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxListViewImp::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/,
	MGlxMediaList* /*aList*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// SetDefaultThumbnailL
// ----------------------------------------------------------------------------
// 		
void CGlxListViewImp::SetDefaultThumbnailL(TInt aIndex)
    {
    TRACER("CGlxListViewImp::SetDefaultThumbnail");
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TInt bitmapId = 0;
    TInt maskId = 0;
    
    TFileName resFile(KDC_APP_BITMAP_DIR);
    resFile.Append(KGlxIconsFilename);
    
    TGlxIdSpaceId id = iMediaList->IdSpaceId(0);        
    // In main list view, default thumbnails will be set according 
    // to the list items.
    if(id == KGlxIdSpaceIdRoot)
        {
        GLX_LOG_INFO1("CGlxListViewImp::SetDefaultThumbnail - For main list view "
                "aIndex %d",aIndex);
        // Mapping between switch, index and order of colleciton done on 
        // the base of TGlxCollectionPluginPriority
        switch(aIndex)
            {
            case EGlxListItemAll:
                {
                bitmapId = EMbmGlxiconsQgn_prop_photo_all_large;
                maskId = EMbmGlxiconsQgn_prop_photo_all_large_mask;
                }
                break;
            case EGlxListItemAlbum:
                {
                bitmapId = EMbmGlxiconsQgn_prop_photo_album_large;
                maskId = EMbmGlxiconsQgn_prop_photo_album_large_mask;
                }
                break;
            case EGlxLIstItemTag:
                {
                bitmapId = EMbmGlxiconsQgn_prop_photo_tag_large;
                maskId = EMbmGlxiconsQgn_prop_photo_tag_large_mask;
                }
                break;
            default:
                break;                    
            }        
        }
    else
        {
        GLX_LOG_INFO1("CGlxListViewImp::SetDefaultThumbnail - For other list view "
                "aIndex %d",aIndex);
        CMPXCollectionPath* path = iMediaList->PathLC(
                NGlxListDefs::EPathParent);
        if(path->Id(0) == KGlxCollectionPluginAlbumsImplementationUid)
            {
            bitmapId = EMbmGlxiconsQgn_prop_photo_album_large;
            maskId = EMbmGlxiconsQgn_prop_photo_album_large_mask;
            }
        else
            {
            bitmapId = EMbmGlxiconsQgn_prop_image_notcreated;
            maskId = EMbmGlxiconsQgn_prop_image_notcreated_mask;
            }
        CleanupStack::PopAndDestroy(path);
        }

    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), KAknsIIDNone, bitmap,
            mask, resFile, bitmapId, maskId);
    __ASSERT_DEBUG(bitmap, Panic(EGlxPanicNullPointer));
    __ASSERT_DEBUG(mask, Panic(EGlxPanicNullPointer));

    AknIconUtils::SetSize(bitmap,
            CHgDoubleGraphicListFlat::PreferredImageSize(),
            EAspectRatioPreservedAndUnusedSpaceRemoved);
    AknIconUtils::SetSize(mask,
            CHgDoubleGraphicListFlat::PreferredImageSize(),
            EAspectRatioPreservedAndUnusedSpaceRemoved);

    iList->ItemL(aIndex).SetIcon(CGulIcon::NewL(bitmap, mask));
    CleanupStack::Pop(mask);
    CleanupStack::Pop(bitmap); 
    }

// ----------------------------------------------------------------------------
// HandleDialogDismissedL
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::HandleDialogDismissedL()
    {
    TRACER("CGlxListViewImp::HandleDialogDismissedL()");
    if(iSchedulerWait)
        {
        iSchedulerWait->AsyncStop();    
        delete iSchedulerWait;
        iSchedulerWait = NULL;
        }
    }

// ---------------------------------------------------------------------------
// HandleMMCInsertionL
// 
// ---------------------------------------------------------------------------
void CGlxListViewImp::HandleMMCInsertionL()
    {
    TRACER("CGlxListViewImp::HandleMMCInsertionL()");
    iMMCState = ETrue;
    // Set PS key value to KErrNotRready as TNM takes some
    // time to write the value.Will be overwritten by TNM later.
    iUiUtility->SetTNMDaemonPSKeyvalue();
	NavigateToMainListL();
    }

// ---------------------------------------------------------------------------
// HandleMMCRemovalL
// 
// ---------------------------------------------------------------------------
void CGlxListViewImp::HandleMMCRemovalL()
    {
    TRACER("CGlxListViewImp::HandleMMCRemovalL()");
	//Dismiss the dialog before env destruction starts.
	//Otherwise dialog's ProcessFinishedL() Panics.
    iProgressIndicator->DismissProgressDialog();
    ProcessCommandL(EAknSoftkeyExit);
    }

// ---------------------------------------------------------------------------
// HandleForegroundEventL
// 
// ---------------------------------------------------------------------------
void CGlxListViewImp::HandleForegroundEventL(TBool aForeground)
    {
    TRACER("CGlxListViewImp::HandleForegroundEventL()");
    GLX_DEBUG2("CGlxListViewImp::HandleForegroundEventL(%d)", aForeground);
    CAknView::HandleForegroundEventL(aForeground);
    if (iMMCState)
        {
        iMMCState = EFalse;
        NavigateToMainListL();
        }

    TInt leftCount = iUiUtility->GetItemsLeftCount();
    if (iProgressIndicator && (leftCount == KErrNotReady || leftCount))
        {
        if (!iUiUtility->GetKeyguardStatus())
            {
            iProgressIndicator->ControlTNDaemon(aForeground);
            }

        if (aForeground)
            {
            iProgressIndicator->ShowProgressbarL();
            }
        else
            {
            iProgressIndicator->DismissProgressDialog();
            }
        }
    }

// ---------------------------------------------------------------------------
// NavigateToMainListL
// 
// ---------------------------------------------------------------------------
void CGlxListViewImp::NavigateToMainListL()
    {
    TRACER("CGlxListViewImp::NavigateToMainListL()");
    ProcessCommandL(EAknSoftkeyClose);
    }

// ----------------------------------------------------------------------------
// CleanUpL
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::CleanUpL()
    {
    GLX_LOG_INFO("CGlxListViewImp::CleanUpL()");
    MGlxCache* cacheManager = MGlxCache::InstanceL();
    for (TInt i = 0; i < iMediaList->Count(); i++)
        {
        cacheManager->ForceCleanupMedia(iMediaList->IdSpaceId(0),
                iMediaList->Item(i).Id());
        }
    cacheManager->Close();
    }

// ----------------------------------------------------------------------------
// UpdatePreviewL
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::UpdatePreviewL(TInt aIndex)
    {
    TRACER("CGlxListViewImp::UpdatePreviewL()");
    if (aIndex >= 0 && aIndex < iMediaList->Count())
        {
        TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail,
                GlxFullThumbnailAttributeId(ETrue, iGridIconSize.iWidth,
                        iGridIconSize.iHeight));
        GLX_LOG_INFO1("CGlxListViewImp::UpdatePreviewL(aIndex = %d)", aIndex);
        const TGlxMedia& item = iMediaList->Item(aIndex);

        TMPXAttribute attrCount(KMPXMediaGeneralCount);
        TInt usageCount = 0;
        const CGlxMedia* media = item.Properties();
        if (media)
            {
            if (media->IsSupported(attrCount))
                {
                usageCount = media->ValueTObject<TInt> (attrCount);
                GLX_LOG_INFO1("CGlxListViewImp::UpdatePreviewL() - UsageCount(%d)", usageCount);
                }
            }

        const CGlxThumbnailAttribute* value = item.ThumbnailAttribute(
                thumbnailAttribute);
        if (usageCount && value)
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bitmap);
            iPreviewTNBinding->ScaleBitmapToListSizeL(value->iBitmap, bitmap);
            iList->ItemL(aIndex).SetIcon(CGulIcon::NewL(bitmap));
            CleanupStack::Pop(bitmap);
            iList->RefreshScreen(aIndex);
            }
        }
    }

// ----------------------------------------------------------------------------
// RefreshList
// ----------------------------------------------------------------------------
//  
void CGlxListViewImp::RefreshList(TInt aIndex)
    {
    TRACER("CGlxListViewImp::RefreshList()");
    GLX_DEBUG2("CGlxListViewImp::RefreshList(%d)", aIndex);	
    __ASSERT_ALWAYS(iList, Panic(EGlxPanicNullPointer));
    TInt firstIndex = iList->FirstIndexOnScreen();
    TInt itemsOnScreen = iList->ItemsOnScreen();
    TInt lastOnScreen = firstIndex + itemsOnScreen - 1;
    TInt mediaCount = iMediaList->Count();
    if (lastOnScreen > (mediaCount - 1))
        {
        lastOnScreen = mediaCount - 1;
        }

    if (aIndex == lastOnScreen)
        {
        GLX_DEBUG2("CGlxListViewImp::RefreshList() RefreshScreen(%d)", aIndex);
        iList->RefreshScreen(aIndex);
        }
    }
//  End of File

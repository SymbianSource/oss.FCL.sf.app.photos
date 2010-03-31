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
#include <glxthumbnailattributeinfo.h>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginmonths.hrh>
#include <glxcollectionplugintype.hrh>
#include <glxnavigationalstate.h>

#include <AknTransEffect.h>  // For transition effects
#include <GfxTransEffect/GfxTransEffect.h>  // For transition effects
#include "glxgfxtranseffect.h"  // For transition effects

const TInt KListDataWindowSize(25);
const TInt KNoOfPages(2);
const TInt KBufferTresholdSize(6);
const TInt KGlxCollectionRootLevel = 1;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CGlxListViewImp::CGlxListViewImp(TInt aViewUid,
        const TListViewResourceIds& aResourceIds) :
    iViewUid(aViewUid), iResourceIds(aResourceIds), iIsRefreshNeeded(EFalse),
    iBackwardNavigation(EFalse), isTnGenerationComplete(ETrue)
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
    
    //Register the view to recieve toolbar events. ViewBase handles the events    
    SetToolbarObserver(this);
	iMMCNotifier = CGlxMMCNotifier::NewL(*this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxListViewImp::~CGlxListViewImp()
    {    
    TRACER("CGlxListViewImp::~CGlxListViewImp");
    delete iMMCNotifier;
    iMMCNotifier = NULL;
    
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

    GfxTransEffect::BeginFullScreen( transitionID, TRect(),
                                   AknTransEffect::EParameterType, 
                         AknTransEffect::GfxTransParam( KPhotosUid, 
                                 AknTransEffect::TParameter::EEnableEffects) );   
    GfxTransEffect::EndFullScreen();
    
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
    iTitleAttributeContext = CGlxDefaultAttributeContext::NewL();
    iTitleAttributeContext->SetRangeOffsets( KListDataWindowSize, 
            KListDataWindowSize );
    iTitleAttributeContext->AddAttributeL( KMPXMediaGeneralTitle );

    iSubtitleAttributeContext = CGlxDefaultAttributeContext::NewL();
    iSubtitleAttributeContext->SetRangeOffsets( KListDataWindowSize, 
            KListDataWindowSize );
    iSubtitleAttributeContext->AddAttributeL( 
            KGlxMediaCollectionPluginSpecificSubTitle );

    iMediaList->AddContextL( iTitleAttributeContext, KMaxTInt );
    iMediaList->AddContextL( iSubtitleAttributeContext, KMaxTInt );

	iMediaList->AddMediaListObserverL(this);
    TRect apRect = iEikonEnv->EikAppUi()->ApplicationRect();
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgScreen,apRect,ETrue);
    
    if(iUiUtility->ViewNavigationDirection() == EGlxNavigationBackwards )
        {
        iBackwardNavigation = ETrue;
        }

    iPreviewTNBinding = CGlxPreviewThumbnailBinding::NewL(*this);
    CreateListL();
    if (iPreviewTNBinding && iBackwardNavigation && iMediaList->FocusIndex()
            == 0)
        {
        GLX_LOG_INFO("CGlxListViewImp::DoMLViewActivateL() - HandleItemChangedL()");        
        iPopulateListTNs = ETrue;
        CMPXCollectionPath* path = iMediaList->PathLC(
                NGlxListDefs::EPathFocusOrSelection);
        iPreviewTNBinding->HandleItemChangedL(*path, iPopulateListTNs,
                iIsRefreshNeeded, iBackwardNavigation);
        CleanupStack::PopAndDestroy(path);
        }
    iProgressIndicator = CGlxProgressIndicator::NewL(*this);
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

    iMediaList->RemoveContext( iTitleAttributeContext );
   	delete iTitleAttributeContext;
   	iTitleAttributeContext = NULL;

    iMediaList->RemoveContext( iSubtitleAttributeContext );
   	delete iSubtitleAttributeContext;
   	iSubtitleAttributeContext = NULL;

	iMediaList->RemoveMediaListObserver( this );
	delete iPreviewTNBinding;
	iPreviewTNBinding = NULL;
	DestroyListViewWidget();
#ifndef __WINSCW__ 
	if(iProgressIndicator)
	    {
        delete iProgressIndicator;
        iProgressIndicator = NULL;
	    }
#endif
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
	if(iUiUtility->ViewNavigationDirection() == EGlxNavigationBackwards )
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
    if (iBgContext)
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
        }
    
    iList->RefreshScreen(visIndex);
    
    if ( visIndex != iLastFocusedIndex && !iPopulateListTNs)
        {
        GLX_LOG_INFO1("CGlxListViewImp::RequestL - SetFocusL()"
                " iPopulateListTNs(%d)", iPopulateListTNs);      
        GLX_LOG_INFO1("CGlxListViewImp::RequestL - SetFocusL()"
                " visIndex(%d)", visIndex);
        iPopulateListTNs = ETrue;
        iStartIndex = KErrNotFound;
        iMediaList->SetFocusL(NGlxListDefs::EAbsolute, visIndex);
        }
    }


void CGlxListViewImp::Release(TInt /*aBufferStart*/, TInt /*aBufferEnd*/)
    {
    TRACER("CGlxListViewImp::Release");
    }

void CGlxListViewImp::HandleSelectL( TInt aIndex )
    {
    TRACER("CGlxListViewImp::HandleSelectL");
    if(0 <= aIndex && aIndex < iMediaList->Count())
    	{
    	iPopulateListTNs = EFalse;
    	iMediaList->SetFocusL(NGlxListDefs::EAbsolute, aIndex);			
    	}
    }

void CGlxListViewImp::HandleOpenL( TInt aIndex )
    {
    TRACER("CGlxListViewImp::HandleOpenL");
    
#ifndef __WINSCW__ 
    TInt leftVariable = 0;
    TRAPD(err,leftVariable = iUiUtility->GetItemsLeftCountL());
    GLX_LOG_INFO1("CGlxListViewImp RProperty::Get leftVariable %d",leftVariable);
    isTnGenerationComplete = (leftVariable)?EFalse:ETrue;
    GLX_LOG_INFO1("CGlxListViewImp isTnGenerationComplete %d",isTnGenerationComplete);
    if(err != KErrNone)
        {
        GLX_LOG_INFO1("CGlxListViewImp RProperty::Get errorcode %d",err);
        }
    if(!isTnGenerationComplete)
        {
        if(iProgressIndicator)
            {
            delete iProgressIndicator;
            iProgressIndicator = NULL;
            }
        iProgressIndicator = CGlxProgressIndicator::NewL(*this);
        if(iSchedulerWait)
            {
            delete iSchedulerWait;
            iSchedulerWait = NULL;
            }
        iSchedulerWait = new (ELeave) CActiveSchedulerWait();
        iSchedulerWait->Start();
        }
#endif
	if( iNextViewActivationEnabled && ( aIndex >= 0 && aIndex < 
	        iMediaList->Count()) && isTnGenerationComplete )
	    {
	    	//Delete the PreviewTNMBinding as in forward navigation
		//we do not get the medialist callback.
        delete iPreviewTNBinding;
        iPreviewTNBinding = NULL;
        
        iMediaList->SetFocusL(NGlxListDefs::EAbsolute,aIndex);
        iLastFocusedIndex = iMediaList->FocusIndex();
	    iNextViewActivationEnabled = EFalse;
        iUiUtility->SetViewNavigationDirection(EGlxNavigationForwards); 
        //Navigate to the next view
        CMPXCollectionPath* path =
             iMediaList->PathLC( NGlxListDefs::EPathFocusOrSelection );
        iCollectionUtility->Collection().OpenL(*path);
        CleanupStack::PopAndDestroy(path);  
        
	    }
    }

void CGlxListViewImp::PreviewTNReadyL(CFbsBitmap* aBitmap, CFbsBitmap* 
        /*aMask*/)
    {
    TRACER("CGlxListViewImp::PreviewTNReadyL");

    if (!iPopulateListTNs)
        {
        GLX_LOG_INFO("CGlxListViewImp::PreviewTNReadyL()- Ignore!");
        return;
        }
    iPreviewTNBinding->StopTimer();

	TInt focusIndex = iMediaList->FocusIndex();
    TInt mediaCount = iMediaList->Count();
    GLX_LOG_INFO1("CGlxListViewImp::PreviewTNReadyL() focusIndex(%d)",
            focusIndex);      
    
    TGlxIdSpaceId id = iMediaList->IdSpaceId(0);        
    if (aBitmap)
	    {
        iList->ItemL(focusIndex).SetIcon(CGulIcon::NewL(aBitmap));
        if (id == KGlxIdSpaceIdRoot && focusIndex == EGlxListItemAll)
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            bitmap->Duplicate(aBitmap->Handle());
            iList->ItemL(EGlxListItemMonth).SetIcon(CGulIcon::NewL(bitmap));
            }
        else if (iBackwardNavigation && id == KGlxIdSpaceIdRoot && focusIndex
                == EGlxListItemMonth)
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            bitmap->Duplicate(aBitmap->Handle());
            iList->ItemL(EGlxListItemAll).SetIcon(CGulIcon::NewL(bitmap));
            }
        }
    else
    	{
	    // In main list view, default thumbnails will be set according 
	    // to the list items.
    	//Displays default thumbnail if aBitmap is NULL 
        SetDefaultThumbnailL(focusIndex);
        if(id == KGlxIdSpaceIdRoot && focusIndex == EGlxListItemAll)
            {
            SetDefaultThumbnailL(EGlxListItemMonth);
            }
    	}
    
    TInt firstIndex = iList->FirstIndexOnScreen();
    TInt itemsOnScreen = iList->ItemsOnScreen();
    TInt lastOnScreen = firstIndex + itemsOnScreen - 1;
    if (lastOnScreen > (mediaCount - 1))
	    {
		lastOnScreen = mediaCount;	    	
	    }
    
    GLX_DEBUG2("CGlxListViewImp::PreviewTNReadyL()"
            " RefreshScreen - focusIndex(%d)", focusIndex);
    iList->RefreshScreen(focusIndex);
	
	if (iPopulateListTNs)
		{
		if(iStartIndex == KErrNotFound)
			{
			iStartIndex = focusIndex;
			}
		
		TInt count = iMediaList->Count();
		if(count)
		    {
            TInt focus = (focusIndex + 1) % count;
            if (!iBackwardNavigation && id == KGlxIdSpaceIdRoot && focus
                    == EGlxListItemMonth)
                {
                focus += 1;
                }
            if (focus != iStartIndex)
                {
                iMediaList->SetFocusL(NGlxListDefs::EAbsolute, focus);
                }
            else
                {
                iPopulateListTNs = EFalse;	
                iIsRefreshNeeded = EFalse;
                
                // iStartIndex holds the focus index of the item.
                // If the focus is not on 0th index and if USB is Connected/Disconnected
                // or some files are deleted from file manager, 
                // Once the updation of all items is done, the focus should be set to
                // previous focus index(not to 0th index). 
                               
                iMediaList->SetFocusL(NGlxListDefs::EAbsolute, 
                		iStartIndex);
                iList->SetSelectedIndex(iStartIndex);
                iList->RefreshScreen(iStartIndex);
                
                iStartIndex = KErrNotFound;
                
                /* The medialist doesn't give the callback for focuschanged if the count is one.
                 * So we have to explicitly start the timer of 2 seconds after populating the list 
                 * TN's ie., after gettting the first thumbnail.
                 * The callback is not given because the setfocus index is same as the focused index
                 * in medialist. 
                 */
                if(count==1)
                    {                        
                    CMPXCollectionPath* path = iMediaList->PathLC( 
                            NGlxListDefs::EPathFocusOrSelection );
                    GLX_LOG_INFO("CGlxListViewImp::PreviewTNReadyL() - HandleItemChangedL()");                    
                    iPreviewTNBinding->HandleItemChangedL(*path, 
                            iPopulateListTNs, iIsRefreshNeeded, iBackwardNavigation);
                    CleanupStack::PopAndDestroy( path );
                    }
                }
			}
		}
    }
    
// ----------------------------------------------------------------------------
// CreateListL
// ----------------------------------------------------------------------------
// 
void CGlxListViewImp::CreateListL()
    {
    TRACER("CGlxListViewImp::CreateListL");
    
	iStartIndex = KErrNotFound;
	TInt mediaCount = iMediaList->Count();
    GLX_DEBUG2("CGlxListViewImp::CreateListL() mediaCount=%d", mediaCount);
	
	if (!iList)
		{
	    TFileName resFile(KDC_APP_BITMAP_DIR);
		resFile.Append(KGlxIconsFilename);
        
        CFbsBitmap* bitmap;
        CMPXCollectionPath* path = iMediaList->PathLC( NGlxListDefs::EPathParent );
        if (path->Id() == KGlxCollectionPluginAlbumsImplementationUid)
            {           
            bitmap = AknIconUtils::CreateIconL(resFile,
                    EMbmGlxiconsQgn_prop_photo_album_large);
            AknIconUtils::SetSize(bitmap, 
                    CHgDoubleGraphicListFlat::PreferredImageSize());
            }
        else if (path->Id() == KGlxCollectionPluginMonthsImplementationUid)
            {
            bitmap = AknIconUtils::CreateIconL(resFile,
                    EMbmGlxiconsQgn_prop_photo_calendar_large);
            AknIconUtils::SetSize(bitmap, 
                    CHgDoubleGraphicListFlat::PreferredImageSize());
            }
        else
            {
            bitmap = AknIconUtils::CreateIconL(resFile,
   						 EMbmGlxiconsQgn_prop_image_notcreated);
		AknIconUtils::SetSize(bitmap, 
				CHgDoubleGraphicListFlat::PreferredImageSize());
            }
		
	    iList = CHgDoubleGraphicListFlat::NewL(
	            ClientRect(), 
	            mediaCount, 
	            CGulIcon::NewL(bitmap) );

	    iList->SetMopParent(this);
	    
		iList->SetSelectionObserver(*this);
		
	    // Enable Buffer support
		iList->EnableScrollBufferL(*this, (KNoOfPages * KBufferTresholdSize),
	            KBufferTresholdSize);
		
        if (iUiUtility->ViewNavigationDirection() == EGlxNavigationBackwards)
            {
            GLX_DEBUG1("CGlxListViewImp::CreateListL() - SetEmptyTextL()");			
            //set the text to be shown if the list is empty.
            HBufC* emptyText = StringLoader::LoadLC(R_LIST_EMPTY_VIEW_TEXT); 
            iList->SetEmptyTextL(*emptyText);
            CleanupStack::PopAndDestroy(emptyText);
            }		
        
		//Fix for ESLM-7SAHPT::Clear Flag to Disable QWERTY search input in list view
		iList->ClearFlags(CHgScroller::EHgScrollerSearchWithQWERTY ); 
		
		//While coming back to main listview
		TGlxIdSpaceId id = iMediaList->IdSpaceId(0);
		if((id == KGlxIdSpaceIdRoot) && (mediaCount > 0))
		    {            
            for (TInt i = 0; i < mediaCount; i++)
                {
                SetDefaultThumbnailL(i);
                }
		    }
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
		iList->RefreshScreen(iLastFocusedIndex);
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
            SetDefaultThumbnailL(i);
            }            
        
        if(aStartIndex == aEndIndex )
            {
            iLastFocusedIndex = aStartIndex;
            iPopulateListTNs = EFalse;
            iMediaList->SetFocusL(NGlxListDefs::EAbsolute, iLastFocusedIndex);			
            }
        
        iList->SetSelectedIndex(iLastFocusedIndex);	
        iList->RefreshScreen(iLastFocusedIndex);
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
	const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList)
	{
	TRACER("CGlxListViewImp::HandleAttributesAvailableL");

	TMPXAttribute titleAttrib(KMPXMediaGeneralTitle);
	TMPXAttribute subTitleAttrib(KGlxMediaCollectionPluginSpecificSubTitle);
	TIdentityRelation<TMPXAttribute> match(&TMPXAttribute::Match);

	const TGlxMedia& item = iMediaList->Item(aItemIndex);

	if (KErrNotFound != aAttributes.Find(titleAttrib, match))
		{
		iList->ItemL(aItemIndex).SetTitleL(item.Title());
		iList->RefreshScreen(aItemIndex);
		}

	if (KErrNotFound != aAttributes.Find(subTitleAttrib, match))
		{
		iList->ItemL(aItemIndex).SetTextL(item.SubTitle());
		iList->RefreshScreen(aItemIndex);

		// If there is some modified in grid/fullscreen view,
		// HandleAttributesAvailableL will get called. Here we are setting
		// iIsRefreshNeeded flag to ETrue		
		if (!iIsRefreshNeeded && iUiUtility->ViewNavigationDirection()
				== EGlxNavigationBackwards)
			{
			iIsRefreshNeeded = ETrue;
			}

		// Set iPopulateListTNs to ETrue and refresh all the items in
		// list view if subtitle is updated
		if (iPreviewTNBinding && ((!iPopulateListTNs && aItemIndex
                == aList->Count() - 1) || (iIsRefreshNeeded)))
            {
			iPopulateListTNs = ETrue;
			iStartIndex = KErrNotFound;
			CMPXCollectionPath* path = iMediaList->PathLC(
					NGlxListDefs::EPathFocusOrSelection);
            GLX_LOG_INFO("CGlxListViewImp::HandleAttributesAvailableL() - HandleItemChangedL()");		
			iPreviewTNBinding->HandleItemChangedL(*path, iPopulateListTNs,
					iIsRefreshNeeded, iBackwardNavigation);
			CleanupStack::PopAndDestroy(path);
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
	TRACER( "CGlxListViewImp::HandleFocusChangedL");
	 
	if (iPreviewTNBinding && iPopulateListTNs)
		{
        CMPXCollectionPath* path = iMediaList->PathLC(
                NGlxListDefs::EPathFocusOrSelection);
        if (!iBackwardNavigation && path->Levels() == KGlxCollectionRootLevel
                && path->Id(0) == KGlxCollectionPluginMonthsImplementationUid)
            {
            CleanupStack::PopAndDestroy(path);
            return;
            }
        GLX_LOG_INFO("CGlxListViewImp::HandleFocusChangedL() - HandleItemChangedL()");        
        iPreviewTNBinding->HandleItemChangedL(*path, iPopulateListTNs,
                iIsRefreshNeeded, iBackwardNavigation);
        CleanupStack::PopAndDestroy(path);
        }
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
void CGlxListViewImp::HandleError( TInt /*aError*/ ) 
    {
    
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
    CFbsBitmap* defaultBitmap = NULL;
    
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
                defaultBitmap = AknIconUtils::CreateIconL(resFile,
                        EMbmGlxiconsQgn_prop_photo_all_large);
                AknIconUtils::SetSize(defaultBitmap, 
                        CHgDoubleGraphicListFlat::PreferredImageSize());            
                break;
            case EGlxListItemMonth:
                defaultBitmap = AknIconUtils::CreateIconL(resFile,
                        EMbmGlxiconsQgn_prop_photo_calendar_large);
                AknIconUtils::SetSize(defaultBitmap, 
                        CHgDoubleGraphicListFlat::PreferredImageSize());
                break;
            case EGlxListItemAlbum:
                defaultBitmap = AknIconUtils::CreateIconL(resFile,
                        EMbmGlxiconsQgn_prop_photo_album_large);
                AknIconUtils::SetSize(defaultBitmap, 
                        CHgDoubleGraphicListFlat::PreferredImageSize());
                break;
            case EGlxLIstItemTag:
                defaultBitmap = AknIconUtils::CreateIconL(resFile,
                        EMbmGlxiconsQgn_prop_photo_tag_large);
                AknIconUtils::SetSize(defaultBitmap, 
                        CHgDoubleGraphicListFlat::PreferredImageSize());
                break;
            default:
                break;                    
            }        
        }
    else
        {
        GLX_LOG_INFO1("CGlxListViewImp::SetDefaultThumbnail - For other list view "
                "aIndex %d",aIndex);
        CMPXCollectionPath* path = iMediaList->PathLC( NGlxListDefs::EPathParent );
        if(path->Id(0) == KGlxCollectionPluginAlbumsImplementationUid)
            {
            defaultBitmap = AknIconUtils::CreateIconL(resFile,
                    EMbmGlxiconsQgn_prop_photo_album_large);
            AknIconUtils::SetSize(defaultBitmap, 
                    CHgDoubleGraphicListFlat::PreferredImageSize());
            }
        else if(path->Id(0) == KGlxCollectionPluginMonthsImplementationUid)
            {
            defaultBitmap = AknIconUtils::CreateIconL(resFile,
                    EMbmGlxiconsQgn_prop_photo_calendar_large);
            AknIconUtils::SetSize(defaultBitmap, 
                    CHgDoubleGraphicListFlat::PreferredImageSize());
            }
        else
            {
            defaultBitmap = AknIconUtils::CreateIconL(resFile,
                    EMbmGlxiconsQgn_prop_image_notcreated);
            AknIconUtils::SetSize(defaultBitmap, 
                    CHgDoubleGraphicListFlat::PreferredImageSize());
            }
        CleanupStack::PopAndDestroy(path);
        }
    iList->ItemL(aIndex).SetIcon(CGulIcon::NewL(defaultBitmap));
    }


// ----------------------------------------------------------------------------
// HandleDialogDismissedL
// ----------------------------------------------------------------------------
//	
void CGlxListViewImp::HandleDialogDismissedL()
    {
    TRACER("CGlxListViewImp::HandleDialogDismissedL()");
    isTnGenerationComplete = ETrue;
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
    NavigateToMainListL();
    
    }

// ---------------------------------------------------------------------------
// HandleMMCRemovalL
// 
// ---------------------------------------------------------------------------
void CGlxListViewImp::HandleMMCRemovalL()
    {
    TRACER("CGlxListViewImp::HandleMMCRemovalL()");
    ProcessCommandL(EAknSoftkeyExit);
    }

// ---------------------------------------------------------------------------
// HandleForegroundEventL
// 
// ---------------------------------------------------------------------------
void CGlxListViewImp::HandleForegroundEventL(TBool aForeground)
    {
    TRACER("CGlxListViewImp::HandleForegroundEventL()");
    CAknView::HandleForegroundEventL(aForeground);
    if(iMMCState)
        {
        iMMCState = EFalse;
        NavigateToMainListL();
        }
    }

// ---------------------------------------------------------------------------
// NavigateToMainListL
// 
// ---------------------------------------------------------------------------
void CGlxListViewImp::NavigateToMainListL()
    {
    TRACER("CGlxListViewImp::NavigateToMainListL()");
    CMPXCollectionPath* naviState = iNavigationalState->StateLC();
    GLX_LOG_INFO1("From listview level = %d",naviState->Levels());
//    if ( naviState->Levels() >= 1)
        {
        ProcessCommandL(EAknSoftkeyClose);
        }
    CleanupStack::PopAndDestroy(naviState);
    }
//  End of File

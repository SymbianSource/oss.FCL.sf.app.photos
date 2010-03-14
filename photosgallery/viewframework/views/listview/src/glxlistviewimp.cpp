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
#include <glxmedialistmulmodelprovider.h>	//CGlxMediaListMulModelProvider
#include <aknViewAppUi.h>
#include <StringLoader.h>					//StringLoader		
#include <glxsetappstate.h>
#include <glxthumbnailattributeinfo.h>
#include <glxcollectionpluginalbums.hrh>

const TInt KListDataWindowSize(25);
const TInt KNoOfPages(2);
const TInt KBufferTresholdSize(6);


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CGlxListViewImp::CGlxListViewImp(TInt aViewUid, const TListViewResourceIds& aResourceIds):        
        iViewUid(aViewUid),
        iResourceIds(aResourceIds)  
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
    CAknToolbar* toolbar = Toolbar();
    if(toolbar)
        {
        toolbar->DisableToolbarL(ETrue);  
        }
    
    //Register the view to recieve toolbar events. ViewBase handles the events    
    SetToolbarObserver(this);
    
    iBackwardNavigation = EFalse;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxListViewImp::~CGlxListViewImp()
    {    
    TRACER("CGlxListViewImp::~CGlxListViewImp");
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
    if (iPreviewTNBinding)
        {
        iPopulateListTNs  = ETrue;
        CMPXCollectionPath* path = iMediaList->PathLC( 
                NGlxListDefs::EPathFocusOrSelection );
        iPreviewTNBinding->HandleItemChangedL(*path, 
                iPopulateListTNs, iBackwardNavigation);
        CleanupStack::PopAndDestroy( path );        
        }

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
	if( iNextViewActivationEnabled && ( aIndex >= 0 && aIndex < 
	        iMediaList->Count()))
	    {
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
        /*aMask*/, TInt /*aIndex*/)
    {
    TRACER("CGlxListViewImp::PreviewTNReadyL");

    iPreviewTNBinding->StopTimer();

	TInt focusIndex = iMediaList->FocusIndex();
    TInt mediaCount = iMediaList->Count();
    
    if (aBitmap)
	    {	
	    iList->ItemL(focusIndex).SetIcon(CGulIcon::NewL(aBitmap));
	    }
    else
    	{
    	//Displays default thumbnail if aBitmap is NULL 
    	
    	TFileName resFile(KDC_APP_BITMAP_DIR);
		resFile.Append(KGlxIconsFilename);
		CFbsBitmap* defaultBitmap = AknIconUtils::CreateIconL(resFile,
						 EMbmGlxiconsQgn_prop_image_notcreated);
		AknIconUtils::SetSize(defaultBitmap, 
				CHgDoubleGraphicListFlat::PreferredImageSize());
		
    	iList->ItemL(focusIndex).SetIcon(CGulIcon::NewL(defaultBitmap));
    	}
    
    TInt firstIndex = iList->FirstIndexOnScreen();
    TInt itemsOnScreen = iList->ItemsOnScreen();
    TInt lastOnScreen = firstIndex + itemsOnScreen - 1;
    if (lastOnScreen > (mediaCount - 1))
	    {
		lastOnScreen = mediaCount;	    	
	    }
    
    if (!iPopulateListTNs || focusIndex == lastOnScreen)
        {
        GLX_DEBUG2("CGlxListViewImp::PreviewTNReadyL()"
               " RefreshScreen - focusIndex(%d)", focusIndex);        
		iList->RefreshScreen(focusIndex);
	    } 
	
	if (iPopulateListTNs)
		{
		if(iStartIndex == KErrNotFound)
			{
			iStartIndex = focusIndex;
			}
		
		TInt count = iMediaList->Count();
		if(count)
		    {
            TInt focus = (focusIndex+1)%count;
            if (focus != iStartIndex)
                {
                iMediaList->SetFocusL(NGlxListDefs::EAbsolute, focus);			
                }
            else
                {
                iPopulateListTNs = EFalse;	
                
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
                    iPreviewTNBinding->HandleItemChangedL(*path, 
                            iPopulateListTNs, iBackwardNavigation);
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
    
	iPopulateListTNs = ETrue;
	iStartIndex = KErrNotFound;
	TInt mediaCount = iMediaList->Count();
    GLX_DEBUG2("CGlxListViewImp::CreateListL() mediaCount=%d", mediaCount);
	
	if (!iList)
		{
	    TFileName resFile(KDC_APP_BITMAP_DIR);
		resFile.Append(KGlxIconsFilename);
    	CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
   						 EMbmGlxiconsQgn_prop_image_notcreated);
		AknIconUtils::SetSize(bitmap, 
				CHgDoubleGraphicListFlat::PreferredImageSize());

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
		
        // Set the scrollbar type for albums list
        CMPXCollectionPath* path = iMediaList->PathLC( NGlxListDefs::EPathParent );
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
	const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/ )
	{
	TRACER("CGlxListViewImp::HandleAttributesAvailableL");
	
	TMPXAttribute titleAttrib(KMPXMediaGeneralTitle);
	TMPXAttribute subTitleAttrib(KGlxMediaCollectionPluginSpecificSubTitle);
    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
	    
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
		
		// Set iPopulateListTNs to ETrue and refresh all the items in
		// list view if subtitle is updated
		if (iPreviewTNBinding && !iPopulateListTNs)
			{
			iPopulateListTNs = ETrue;
			iStartIndex = KErrNotFound;
			CMPXCollectionPath* path = iMediaList->PathLC( 
					NGlxListDefs::EPathFocusOrSelection );
	        iPreviewTNBinding->HandleItemChangedL(*path, 
	                iPopulateListTNs, iBackwardNavigation);
			CleanupStack::PopAndDestroy( path ); 		
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
	 
	if (iPreviewTNBinding)
		{
		CMPXCollectionPath* path = iMediaList->PathLC( 
		        NGlxListDefs::EPathFocusOrSelection );
        iPreviewTNBinding->HandleItemChangedL(*path, 
                iPopulateListTNs, iBackwardNavigation);
		CleanupStack::PopAndDestroy( path );		
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
		
//  End of File

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
* Description:    Container for Fecther Dialog
*
*/




// System includes
#include <avkon.rsg>                    
#include <AknUtils.h>                               // For AknLayoutUtils to calculate rect
#include <data_caging_path_literals.hrh>            // KDC_APP_RESOURCE_DIR			
#include <StringLoader.h>                           // For Empty text reading from resource
#include <akntitle.h>                               // Title Pane 

#include <mpxcollectionutility.h>                   // Mpx Collection path for medialist craetion
#include <gulicon.h>                                // Gul Icon 
#include <ganes/HgItem.h>                           // HG Item 
#include <ganes/HgGrid.h>                           // Hg Grid Widget
#include <AknsBasicBackgroundControlContext.h>      // Background Context

#include <glxuistd.h>                               // KGlxFetchontextPriorityNormal and KGlxIconsFilename           
#include <glxcollectionpluginall.hrh> 	            // All item collection plugin id
#include <glxfilterfactory.h>                       // For Filters required in Medilaits
#include <glxresourceutilities.h>                   // for CGlxResourceUtilities
#include <glxfetcherdialog.rsg>                     // FOR GETTING Dialog ID AND RESOURCE ID
#include <glxattributecontext.h>                    // TMPXAttribute
#include <glxattributeretriever.h>  	            // CGlxAttributeRetriever
#include <glxfetchcontextremover.h>                 // TGlxfetchContextRemover
#include <glxuiutility.h>                           // AknIconUtils and EikStatusPane
#include <glxtracer.h>                              // For Tracer logs
#include <glxlog.h>                                 // Glx Logger
#include <glxgridviewmlobserver.h>                  // Medialistobserver for HgGrid
#include <glxthumbnailcontext.h>                    // Thumbnailcontext for HgGrid
#include <glxicons.mbg>                             // Glx Icons for HgGrid                     
#include <mglxmedialist.h>				            // MGlxMediaList, CMPXCollectionPath
#include <glxcommandhandlers.hrh>                   // For EGlxCmdEndMultiplemarking and EGlxCmdStartMultiplemarking 
#include <glxthumbnailattributeinfo.h>              // KGlxMediaIdThumbnail
#include <glxerrormanager.h>
#include <glxdrmutility.h>                          // DRM utility class to provide DRM-related functionality

// User includes
#include "glxfetchercontainer.h"
#include "mglxeventobserver.h"
#include "glxfetcher.hrh"
#include "glxfetchercommandhandler.h"

const TInt KPadding(7);
const TInt KNoOfPages(4);
const TInt KBufferTreshold(1);
const TInt KItemsPerPage(18);
const TInt KBufferSize(KNoOfPages * KItemsPerPage);
const TInt KBufferTresholdSize(KBufferTreshold * KItemsPerPage);

//-----------------------------------------------------------------------------
// Two-phased constructor.
//-----------------------------------------------------------------------------
CGlxFetcherContainer* CGlxFetcherContainer::NewL(
        TGlxFilterItemType aFilterType, const TDesC& aTitle, 
        MGlxEventObserver& aEventObserver, TBool aMultiSelectionEnabled ) 
	{
	TRACER("CGlxFetcherContainer::NewL");
	CGlxFetcherContainer* self = 
	    new( ELeave ) CGlxFetcherContainer( aFilterType ,aTitle, 
	            aEventObserver, aMultiSelectionEnabled);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self ); 
	return self;
	}

//-----------------------------------------------------------------------------
// C++ default constructor.
//-----------------------------------------------------------------------------
CGlxFetcherContainer::CGlxFetcherContainer(
        TGlxFilterItemType aFilterType ,const TDesC& aTitle,
        MGlxEventObserver& aEventObserver, TBool aMultiSelectionEnabled )
    : iFilterType( aFilterType ),
    iTitle( aTitle ),iEventObserver(aEventObserver),
    iMultiSelectionEnabled(aMultiSelectionEnabled)      
    {
    TRACER("CGlxFetcherContainer::CGlxFetcherContainer");
    // No implementation required
    }

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
void CGlxFetcherContainer::ConstructL()
    {
    TRACER("CGlxFetcherContainer::ConstructL");
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgAreaMain, Rect(), EFalse );
    iUiUtility = CGlxUiUtility::UtilityL();
    iGridIconSize = iUiUtility->GetGridIconSize();

    // For DRM Utility
    iDRMUtility = CGlxDRMUtility::InstanceL();    
    SetFileAttached(EFalse);
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGlxFetcherContainer::~CGlxFetcherContainer()
    {
    TRACER("CGlxFetcherContainer::~CGlxFetcherContainer");
    if (iDRMUtility)
        {
        iDRMUtility->Close();
        }            
    delete iBgContext;
    delete iPreviousTitle;
    delete iFetcherCommandHandler;
    delete iGlxGridMLObserver;  
    if (iHgGrid)
        {
        iHgGrid->SetFocus(EFalse);
        delete iHgGrid;
        iHgGrid = NULL;
        }
    if (iMediaList)
        {
        iMediaList->RemoveContext(iThumbnailContext);
        delete iThumbnailContext;
        iMediaList->Close();
        }
    if (iUiUtility)
        {
        iUiUtility->Close() ;
        }
    }


// ---------------------------------------------------------------------------
// CreateAndDisplayGridL
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::CreateAndDisplayGridL()
	{
	TRACER("CGlxFetcherContainer::CreateAndDisplayGridL");
    //Create Medialist
    CreateMediaListL();
    
	// Create the control here
    CreateHgGridWidgetL();
    
    CreateGridMediaListObserverL();
	
	// If multiple image selection is allowed, then set marking flag on grid widget.
	if(iMultiSelectionEnabled)
	    {
	    // Fetcher CommandHandler must be created after creating the medialist
	    iFetcherCommandHandler = CGlxFetcherCommandHandler::NewL( this , this );
	    
	    iHgGrid->SetFlags( CHgScroller::EHgScrollerSelectionMode );
	    iMultipleMarkNotStarted = EFalse;
	    }
	}
// ---------------------------------------------------------------------------
// CreateMediaListL()
// Creates a collection path
// Create a filter as requested filtertype
// Creates the medialist
// ---------------------------------------------------------------------------
void CGlxFetcherContainer::CreateMediaListL() 
	{
	TRACER("CGlxFetcherContainer::CreateMediaListL");
   		
	// Create path to the list of images and videos
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	path->AppendL(KGlxCollectionPluginAllImplementationUid);	// All item collection plugin  
	
	// Create filter to filter out either images or videos
	GLX_LOG_INFO1("CGlxFetcherContainer::MediaListL  - Filter Type = %d",iFilterType);
	CMPXFilter* filter = TGlxFilterFactory::CreateItemTypeFilterL(iFilterType);  
	CleanupStack::PushL(filter);
	
	// Create the media list
	iMediaList =  MGlxMediaList::InstanceL( *path, 
	                  TGlxHierarchyId(KGlxFetcherDllUid), filter );
	CleanupStack::PopAndDestroy( filter );
	
	GLX_LOG_INFO1("CGlxFetcherContainer::MediaListL  - Path level = %d",
	                                                     path->Levels());
	
	CleanupStack::PopAndDestroy( path );
	}
// ---------------------------------------------------------------------------
// CreateHgGridWidgetL
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::CreateHgGridWidgetL()
	{
    TRACER("CGlxFetcherContainer::CreateHgGridWidgetL");
    //Setting the Title
    SetTitleL( iTitle);

    iThumbnailContext = CGlxThumbnailContext::NewL( &iBlockyIterator ); // set the thumbnail context
    iThumbnailContext->SetDefaultSpec( iUiUtility->GetGridIconSize().iWidth,
            iUiUtility->GetGridIconSize().iHeight );

    iMediaList->AddContextL(iThumbnailContext, KGlxFetchContextPriorityNormal );

    if (!iHgGrid)
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, rect);
        TSize iconSize (iUiUtility->GetGridIconSize().iWidth - (2*KPadding),
                iUiUtility->GetGridIconSize().iHeight -(2*KPadding));
        TFileName resFile(KDC_APP_BITMAP_DIR);
        resFile.Append(KGlxIconsFilename);
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), KAknsIIDNone,
                bitmap, mask, resFile, EMbmGlxiconsQgn_prop_image_notcreated,
                EMbmGlxiconsQgn_prop_image_notcreated_mask);
        __ASSERT_DEBUG(bitmap, Panic(EGlxPanicNullPointer));
        __ASSERT_DEBUG(mask, Panic(EGlxPanicNullPointer));

        AknIconUtils::SetSize(bitmap, CHgGrid::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);
        AknIconUtils::SetSize(mask, CHgGrid::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);

        // Create Hg grid object
        iHgGrid = CHgGrid::NewL(rect, iMediaList->Count(), CGulIcon::NewL(
                bitmap, mask));
        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap);
        }

	// Set the grid to use different layout for landscape mode in fetcher
    iHgGrid->SetToolbarVisibility(EFalse);
    iHgGrid->SetSelectedIndex(iMediaList->FocusIndex());
    // Setting to MopParent to update background skin
    iHgGrid->SetMopParent(this);
    // Setting Selction observer for getting callback on key event change
    iHgGrid->SetSelectionObserver(*this);
    // This Displays the scrollbar at the opening of the Grid view
    iHgGrid->SetScrollBarTypeL(CHgScroller::EHgScrollerTimeStrip );
    // Enable Buffer support
    iHgGrid->EnableScrollBufferL(*this, KBufferSize, KBufferTresholdSize);
    // Enable Marking support
    iHgGrid->SetMarkingObserver(*this);

	iHgGrid->SetFocus(ETrue);
	}

// ---------------------------------------------------------------------------
// CreateGridMediaListObserverL
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::CreateGridMediaListObserverL()
    {
    iGlxGridMLObserver = CGlxGridViewMLObserver::NewL(*iMediaList, iHgGrid,
            iFilterType);
    }
    
// ---------------------------------------------------------------------------
// BufferPositionChanged
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::Request(TInt aRequestStart, TInt aRequestEnd,
        THgScrollDirection /*aDirection*/)
    {
    TRACER("CGlxFetcherContainer::Request()");
    TRAP_IGNORE(RequestL( aRequestStart, aRequestEnd ));
    }
// ---------------------------------------------------------------------------
// RequestL
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::RequestL(TInt aRequestStart, TInt aRequestEnd)
    {
    TRACER("CGlxFetcherContainer::RequestL()");
    TInt mediaCount = iMediaList->Count();

    aRequestStart = (aRequestStart<0 ? 0 : aRequestStart) ;
    aRequestEnd = (aRequestEnd>=mediaCount? (mediaCount -1): aRequestEnd);

    if (aRequestEnd - aRequestStart)
        {
        TInt visIndex = iHgGrid->FirstIndexOnScreen();
        if(visIndex >= mediaCount )
            {
            visIndex = mediaCount-1;
            }
        if(visIndex<0 || mediaCount<=0)
            {
            visIndex = 0;
            }
		
        GLX_LOG_INFO1("CGlxGridViewImp::Request - SetVisibleWindowIndex "
                "visIndex(%d)", visIndex);
        iMediaList->SetVisibleWindowIndexL(visIndex);
        }
    for (TInt i=aRequestStart; i<= aRequestEnd; i++)
        {
        SetIconsL(i);
        TInt firstIndex = iHgGrid->FirstIndexOnScreen();
        TInt lastOnScreen = firstIndex + iHgGrid->ItemsOnScreen() - 1;
        if (i >= firstIndex && i <= lastOnScreen)
            {
            GLX_LOG_INFO1("CGlxGridViewImp::Request - RefreshScreen i(%d)", i);
            iHgGrid->RefreshScreen(i);
            }
        }
    }
// ---------------------------------------------------------------------------
// SetIcons
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::SetIconsL(TInt aIndex)
    {
    TRACER("CGlxFetcherContainer::SetIconsL()");
    const TGlxMedia& item = iMediaList->Item(aIndex);
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  iGridIconSize.iWidth, 
                    iGridIconSize.iHeight ) );

    TFileName resFile(KDC_APP_BITMAP_DIR);
    resFile.Append(KGlxIconsFilename);
    TIconInfo icon;
    TInt tnError = GlxErrorManager::HasAttributeErrorL(
            item.Properties(), KGlxMediaIdThumbnail );

    const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
    if (value)
        {
        CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
        bitmap->Duplicate( value->iBitmap->Handle());
        iHgGrid->ItemL(aIndex).SetIcon(CGulIcon::NewL(bitmap));
        GLX_LOG_INFO1("### CGlxGridViewImp::Request value-Index is %d",aIndex);
        }
    else if (item.GetIconInfo(icon))
        {
        GLX_LOG_INFO1("CGlxGridViewImp::Request - icon(%d)", aIndex);
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), KAknsIIDNone,
                bitmap, mask, icon.bmpfile, icon.bitmapId, icon.maskId);
        __ASSERT_DEBUG(bitmap, Panic(EGlxPanicNullPointer));
        __ASSERT_DEBUG(mask, Panic(EGlxPanicNullPointer));

        AknIconUtils::SetSize(bitmap, CHgGrid::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);
        AknIconUtils::SetSize(mask, CHgGrid::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);

        iHgGrid->ItemL(aIndex).SetIcon(CGulIcon::NewL(bitmap, mask));
        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap);
        GLX_LOG_INFO1("### CGlxGridViewImp::Request GetIconInfo-Index is %d",
                aIndex);
        }
    else if (KErrNone != tnError && KErrNotSupported != tnError)
        {
        GLX_LOG_INFO2(
                "CGlxGridViewImp::Request - image_corrupted tnError(%d), "
                    "i(%d)", tnError, aIndex);

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), KAknsIIDNone,
                bitmap, mask, resFile, EMbmGlxiconsQgn_prop_image_corrupted,
                EMbmGlxiconsQgn_prop_image_corrupted_mask);
        __ASSERT_DEBUG(bitmap, Panic(EGlxPanicNullPointer));
        __ASSERT_DEBUG(mask, Panic(EGlxPanicNullPointer));

        AknIconUtils::SetSize(bitmap, CHgGrid::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);
        AknIconUtils::SetSize(mask, CHgGrid::PreferredImageSize(),
                EAspectRatioPreservedAndUnusedSpaceRemoved);

        iHgGrid->ItemL(aIndex).SetIcon(CGulIcon::NewL(bitmap, mask));
        CleanupStack::Pop(mask);
        CleanupStack::Pop(bitmap);
        }

    if (item.IsDrmProtected())
        {
        const TDesC& uri = item.Uri();
        if( uri.Length() > 0)
            {
            if(iDRMUtility->IsForwardLockedL(uri))
                {
                iHgGrid->ItemL(aIndex).SetFlags(
                        CHgItem::EHgItemFlagsDrmRightsValid);
                }
            else 
                {
                TMPXGeneralCategory  cat = item.Category();                  
                TBool checkViewRights = (cat==EMPXImage);

                if(iDRMUtility->ItemRightsValidityCheckL(uri, checkViewRights))
                    {
                    iHgGrid->ItemL(aIndex).SetFlags(
                            CHgItem::EHgItemFlagsDrmRightsValid);
                    }
                else
                    {
                    iHgGrid->ItemL(aIndex).SetFlags(
                            CHgItem::EHgItemFlagsDrmRightsExpired);
                    }
                }
            }
        }
    TTime time(0);
    if (item.GetDate(time))
        {
        iHgGrid->ItemL(aIndex).SetTime(time);
        }

    if (item.Category() == EMPXVideo)
        {
        iHgGrid->ItemL(aIndex).SetFlags(CHgItem::EHgItemFlagsVideo);
        }
    }

// ---------------------------------------------------------------------------
// Release
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::Release(TInt /*aReleaseStart*/, TInt /*aReleaseEnd*/)
    {
    TRACER("CGlxFetcherContainer::Release()");
    // This has to do nothing on default
    }

// ---------------------------------------------------------------------------
// HandleSelect
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::HandleSelectL( TInt aIndex )
    {
    TRACER("CGlxGridViewImp::HandleSelect()");
    // Make sure that the Selection Index is inside medialist count
    if (aIndex <iMediaList->Count() && aIndex >=0)
        {
        // Setting the focus of the medialist
        iMediaList->SetFocusL(NGlxListDefs::EAbsolute, aIndex);        
        }
    }

// ---------------------------------------------------------------------------
// HandleOpen
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::HandleOpenL( TInt aIndex )
    {
    TRACER("CGlxGridViewImp::HandleOpen()");
    // Make sure that the Selection Index is inside medialist count
    if (aIndex <iMediaList->Count() && aIndex >=0)
        {
        //ignore multiple tap events for single selection cases
        //if file has already been selected
        if(!iMultiSelectionEnabled && !iIsFileAttached)
	        {
	        iEventObserver.HandleDoubleTapEventL(EAknSoftkeySelect);
	        }
        }
    }

// ---------------------------------------------------------------------------
// HandleMarkingL
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::HandleMarkingL( TInt aIndex, TBool/* aMarked*/ )
    {
    TRACER("CGlxGridViewImp::HandleMarkingL()");
    HandleMultipleMarkingL(aIndex);
    iEventObserver.HandleMarkEventL();    
    }

// ---------------------------------------------------------------------------
// HandleMultipleMarkingL
// ---------------------------------------------------------------------------
//  
void CGlxFetcherContainer::HandleMultipleMarkingL(TInt aIndex )
    {
    TRACER("CGlxGridViewImp::HandleMultipleMarkingL()");
    if(iMediaList->IsSelected(aIndex))
        {
        if(iHgGrid)
            {
            if(iMultiSelectionEnabled)
	            {
	            iFetcherCommandHandler->DoExecuteL(EAknCmdUnmark, MediaList());	
	            }
            }
        }
    else
        {
        if(iHgGrid)
            {
            if(iUiUtility->IsPenSupported())
                {
                //This is required for the touch because here when we select
                //we do not get the event for mark in processcommandL 
                //of FetcherDialog. So we explicitly send the event to ourselves 
                //to mark the item.
                if (!iMultipleMarkNotStarted)
                    {
                    DoExecuteL(EAknCmdMark);
                    }
                }
            if(iMultiSelectionEnabled)
	            {
	            iFetcherCommandHandler->DoExecuteL(EAknCmdMark, MediaList());	
	            }            	
            }
        }
    }

// ---------------------------------------------------------------------------
// DoExecuteL()
// ---------------------------------------------------------------------------
TBool CGlxFetcherContainer::DoExecuteL(TInt aCommandId)
    {
    TRACER("CGlxFetcherContainer::DoExecuteL");

    TBool retVal = EFalse;
    /*
    * fix for  EBYU-7UV4MW-- when user presses selection key
    * very fast that time if hggrid is not created and we are trying to 
    * access hghgrid for marking it is giving crash. So need to chk if 
    * hggrid id created or not.same will also happen if media list is 
    * not created
    */
    if(!iHgGrid || !iMediaList)
    	{
    	return retVal;
    	}
    	
    TInt focusIndex = iMediaList->FocusIndex();
    
    switch(aCommandId)
        {
        case EAknCmdMark:
        case EAknMarkAll:
        case EGlxCmdStartMultipleMarking:
            {
            //@ fix for ELWU-7RA7NX 
            if (iMultiSelectionEnabled )
                {
                if (!iMultipleMarkNotStarted)
                	{
                	//This will start multiple marking and not mark any items
					iFetcherCommandHandler->DoExecuteL(EAknCmdMark, MediaList());
                	iMultipleMarkNotStarted = ETrue ;
                	}
	    		//@ Mark in HG Grid
				iHgGrid->Mark(focusIndex);
				iHgGrid->RefreshScreen(focusIndex);
				
                }
            retVal = ETrue;
            break;
            }
        case EAknCmdUnmark:            
        case EAknUnmarkAll:   
            {
            //@ fix for ELWU-7RA7NX 
            if (iMultiSelectionEnabled)
                {
                //@ UnMark in HG Grid
                iHgGrid->UnMark(focusIndex);
                iHgGrid->RefreshScreen(focusIndex);
                }

            if( iMediaList->SelectionCount() <= 0 )
                {
                iMultipleMarkNotStarted = EFalse;
                }            
            retVal = ETrue;
            break;
            }
        case EGlxCmdEndMultipleMarking:
            {
            iHgGrid->ClearFlags( CHgScroller::EHgScrollerSelectionMode );                
            iFetcherCommandHandler->DoExecuteL(aCommandId, MediaList());              
            } 
            retVal = ETrue;
            break;
        default:
            break;
        }    
    return retVal;
    }


// -----------------------------------------------------------------------------
// Retrieve Uris
// -----------------------------------------------------------------------------
TBool CGlxFetcherContainer::RetrieveUrisL( CDesCArray& aSelectedFiles, TBool& aFetchUri)
    {
    TRACER("CGlxFetcherContainer::RetrieveUrisL");
    // check that the medialist is really constructed
    // we can get this call while MGlxMediaList::InstanceL has not yet returned
    // as it starts a new asynch wait loop and during that it is possible 
    // to send key events to the dialog.
    if( !iMediaList || !iHgGrid )
        {
        aFetchUri = EFalse;		
        SetFileAttached(EFalse);
        return iIsFileAttached;
        }
    //To Block call to HandleDoubleTapEventL() when control is in RetrieveUrisL()    
    SetFileAttached(ETrue);
    GLX_LOG_INFO1("CGlxFetcherContainer::RetrieveUrisL() Medialist count is %d",
                iMediaList->Count());
    TGlxSelectionIterator iterator;
	CGlxAttributeContext* context = new (ELeave) CGlxAttributeContext( &iterator );
	CleanupStack::PushL( context );
	
	// Configure the context to retrieve the URI attribute
	context->AddAttributeL( KMPXMediaGeneralUri );
	
	iMediaList->AddContextL( context, KGlxFetchContextPriorityBlocking );

	// Create remover object to remove the context from the list
	// even if a leave occurs
	TGlxFetchContextRemover remover( context, *iMediaList );
	// put to cleanupstack as cleanupstack is emptied before stack objects
	// are deleted
	CleanupClosePushL( remover );
    // block until all content retrieved (shows progress note)
    TInt error = GlxAttributeRetriever::RetrieveL( *context, 
            *iMediaList, ETrue ) ;
    GLX_LOG_INFO1("CGlxFetcherContainer::RetrieveUrisL() Error %d", error);
    // context off the list
    CleanupStack::PopAndDestroy( &remover );
    GLX_LOG_INFO1("CGlxFetcherContainer::RetrieveUrisL() Medialist count after GlxAttributeRetriever is %d",
                iMediaList->Count());
    
    // if error or if medialist count is 0 then do not proceed further with the iterator.
    if (error == KErrNone && iMediaList->Count() !=0)
        {
        GLX_LOG_INFO1("CGlxFetcherContainer::RetrieveUrisL() inside loop and medialist count is %d",
                iMediaList->Count());
        // Add all selected items to client's array
        iterator.SetToFirst( iMediaList );
        TInt index = KErrNotFound;
        while ( KErrNotFound != (index = iterator++) )
            {
            const TDesC& uri = iMediaList->Item( index ).Uri();
            if( uri != KNullDesC )
                {
                aSelectedFiles.AppendL( uri );
                SetFileAttached(ETrue);
                }   
            else
                {                
                SetFileAttached(EFalse);
                }
            }
        }
    CleanupStack::PopAndDestroy( context );
    // Set the Fetch uri value as false, as it could be a drm image and calling 
    // application might not allow that to be selected, thus fetcher could be launched again.
    aFetchUri = EFalse;
	return iIsFileAttached;
    }

//-----------------------------------------------------------------------------
// CGlxFetcherContainer::OfferKeyEventL
//-----------------------------------------------------------------------------
TKeyResponse CGlxFetcherContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,TEventCode aType )
    {
    TRACER("CGlxFetcherContainer::OfferKeyEventL");
	return iHgGrid->OfferKeyEventL(aKeyEvent,aType);
    }
    
// ---------------------------------------------------------------------------
// SetTitleL()
// ---------------------------------------------------------------------------
void CGlxFetcherContainer::SetTitleL(const TDesC& aTitleText)
    {
    TRACER("CGlxFetcherContainer::SetTitleL");
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CleanupStack::PushL(statusPane);
    CAknTitlePane* titlePane = ( CAknTitlePane* )statusPane->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ));       
    CleanupStack::PushL(titlePane);
    // To Store Title and to make sure, Previous Title is only stored once
    if (!iPreviousTitle)
        {
        iPreviousTitle = titlePane->Text()->AllocL();    
        }
    
    // Set the required Title
    titlePane->SetTextL( aTitleText );
    CleanupStack::Pop(titlePane);
    CleanupStack::Pop(statusPane);
    }

// ---------------------------------------------------------------------------
// SetPreviousTitleL()
// ---------------------------------------------------------------------------
void CGlxFetcherContainer::SetPreviousTitleL()
    {
    TRACER("CGlxFetcherContainer::SetPreviousTitleL");
    CEikStatusPane* prevStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CleanupStack::PushL(prevStatusPane);
    CAknTitlePane* prevTitlePane = ( CAknTitlePane* )prevStatusPane->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ));       
    CleanupStack::PushL(prevTitlePane);
    
    // Restore the Title back of the Calling Application
    prevTitlePane->SetTextL( *iPreviousTitle );
    CleanupStack::Pop(prevTitlePane);
    CleanupStack::Pop(prevStatusPane);
    }

// -----------------------------------------------------------------------------
// UpdateTitle
// -----------------------------------------------------------------------------
//
void CGlxFetcherContainer::UpdateTitleL()
    {
    TRACER("CGlxFetcherContainer::UpdateTitleL");
    SetTitleL( iTitle);
    }
 
// -----------------------------------------------------------------------------
// MediaList
// -----------------------------------------------------------------------------
//
MGlxMediaList& CGlxFetcherContainer::MediaList()
    {
    TRACER("CGlxFetcherContainer::MediaList");
    return *iMediaList;
    }

// -----------------------------------------------------------------------------
// HandleSizeChange
// -----------------------------------------------------------------------------
//
void CGlxFetcherContainer::HandleSizeChangeL ( )
    {
    TRACER("CGlxFetcherContainer::HandleSizeChangeL");
    SetTitleL(iTitle);
    TRect rect;
    if(iHgGrid)
        {
        AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, rect);
        iHgGrid->InitScreenL(rect);
        }
    if ( iBgContext )
        {
        AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EApplicationWindow, rect);
        iBgContext->SetRect( rect );
        }
    }

// -----------------------------------------------------------------------------
// MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CGlxFetcherContainer::MopSupplyObject(TTypeUid aId)
    {
    TRACER("CGlxFetcherContainer::MopSupplyObject");
    if (iBgContext)
        {
        return MAknsControlContext::SupplyMopObject(aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject(aId);
    }

// -----------------------------------------------------------------------------
// SetFileAttached
// -----------------------------------------------------------------------------
//
void CGlxFetcherContainer::SetFileAttached(TBool aIsFileAttached)
    {
    iIsFileAttached = aIsFileAttached;
    }

//END OF FILE

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
*  Description : Class defination for preview thumbnail in list view
*
*/


#include <mglxmedialist.h>
#include <glxthumbnailcontext.h>
#include <glxfilterfactory.h>                         // For TGlxFilterFactory
#include <glxthumbnailattributeinfo.h>
#include <glxuiutility.h>                               // For UiUtility instance
#include <glxtracer.h>
#include <glxlog.h>
#include <glxerrormanager.h>             // For CGlxErrormanager
#include <glxuistd.h>                    // Fetch context priority def'ns
#include <mglxcache.h> 
#include <ganes/HgDoubleGraphicList.h>

#include "glxpreviewthumbnailbinding.h"
#include "glxgeneraluiutilities.h"

const TInt KThumbnailsTimeTimeDelay(50000);
const TInt KWaitCount(30);
const TInt KPreviewThumbnailFetchCount(1);


// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CGlxPreviewThumbnailBinding* CGlxPreviewThumbnailBinding::NewL(
        MPreviewTNObserver& aObserver)
	{
	TRACER("CGlxPreviewThumbnailBinding::NewL");
	CGlxPreviewThumbnailBinding* self = CGlxPreviewThumbnailBinding::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;	
	}

// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//
CGlxPreviewThumbnailBinding* CGlxPreviewThumbnailBinding::NewLC(
        MPreviewTNObserver& aObserver)
	{
	TRACER("CGlxPreviewThumbnailBinding::NewLC");
	CGlxPreviewThumbnailBinding* self = new(ELeave)CGlxPreviewThumbnailBinding(aObserver);
	CleanupStack::PushL(self);
    self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxPreviewThumbnailBinding::CGlxPreviewThumbnailBinding(
        MPreviewTNObserver& aObserver)
: iObserver(aObserver)
	{
	}

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::ConstructL()
    {
    TRACER("CGlxPreviewThumbnailBinding::ConstructL");
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL(*uiUtility);
    iGridIconSize = uiUtility->GetGridIconSize();
    CleanupStack::PopAndDestroy(uiUtility);
    
    // Filter that filters out any GIF, corrupted images     
    iPreviewFilter = TGlxFilterFactory::CreatePreviewTNFilterL();
    iThumbnailIterator.SetRange(KPreviewThumbnailFetchCount);
    iThumbnailContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 
    TMPXAttribute tnAttr( KGlxMediaIdThumbnail,
                        GlxFullThumbnailAttributeId(ETrue,
                            iGridIconSize.iWidth,iGridIconSize.iHeight) );
    iThumbnailContext->SetDefaultSpec(iGridIconSize.iWidth,
                                                iGridIconSize.iHeight);
    iThumbnailContext->AddAttributeL(tnAttr);
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxPreviewThumbnailBinding::~CGlxPreviewThumbnailBinding()
	{
	TRACER("CGlxPreviewThumbnailBinding::~CGlxPreviewThumbnailBinding");
    // close any medialist that exist
	if( iMediaList )
		{
        iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iThumbnailContext);
        iMediaList->Close();
        iMediaList = NULL;
		}
    delete iThumbnailContext;
    delete iPreviewFilter;
	// cancel any outstanding request of the timer
	if(iTimer->IsActive())
	    {
	    iTimer->Cancel();
	    }
	delete iTimer;
	iTimer = NULL;	
	}

// ----------------------------------------------------------------------------
// TimerTickedL: we need to update only when required i.e, when the thumbnail count 
// has not reached till the max limit to be shown
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::TimerTickedL()
    {
    TRACER("CGlxPreviewThumbnailBinding::TimerTickedL");

    if (!iMediaList)
        {
        GLX_LOG_INFO("TimerTickedL() iMediaList == NULL!");
        return;
        }

    if (iMediaList->IsPopulated())
        {
        if (iMediaList->Count())
            {
            const TGlxMedia& item = iMediaList->Item(0);
            TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail,
                    GlxFullThumbnailAttributeId(ETrue, iGridIconSize.iWidth,
                            iGridIconSize.iHeight));

            const CGlxThumbnailAttribute* value = item.ThumbnailAttribute(
                    thumbnailAttribute);
            if (value)
                {
                CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
                CleanupStack::PushL(bitmap);
                ScaleBitmapToListSizeL(value->iBitmap, bitmap);
                GLX_LOG_INFO1("iObserver.PreviewTNReadyL() iTrialCount=%d", iTrialCount);
                iObserver.PreviewTNReadyL(bitmap, NULL);
                CleanupStack::Pop(bitmap);
                }
            else
                {
                TInt tnError = GlxErrorManager::HasAttributeErrorL(
                        item.Properties(), KGlxMediaIdThumbnail);
                if (tnError != KErrNone || iTrialCount == KWaitCount)
                    {
                    GLX_LOG_INFO1("TimerTickedL() tnError(%d) / Max Trialcount reached", tnError);
                    iObserver.PreviewTNReadyL(NULL, NULL);
                    iTrialCount = 0;
                    return;
                    }
                iTrialCount++;
                }
            }
        else
            {
            // Media count is zero, 
            // Notify immedialtely to jump to the next item in the list 
            GLX_LOG_INFO("TimerTickedL() iMediaList->Count() = 0");
            iObserver.PreviewTNReadyL(NULL, NULL);
            }
        }
    }

// ----------------------------------------------------------------------------
// IsTimeL callback function invoked when timer expires
// ----------------------------------------------------------------------------
//    
TInt CGlxPreviewThumbnailBinding::IsTimeL( TAny* aSelf )
    {
    TRACER("CGlxPreviewThumbnailBinding::IsTimeL");
    if (aSelf)
        {
        TRACER("CGlxPreviewThumbnailBinding::IsTimeL");
        CGlxPreviewThumbnailBinding* self = static_cast<CGlxPreviewThumbnailBinding*>(aSelf);
        if(self)
            {
            self->TimerTickedL();
           }
        }
    return KErrNone;        
    }


// ----------------------------------------------------------------------------
// HandleItemChangedL
// This function basically closes the old medialist if any for ex. Tags, 
// Captured.. and then opens a new medialist with teh required filter.
// ----------------------------------------------------------------------------
//    
void CGlxPreviewThumbnailBinding::HandleItemChangedL(const CMPXCollectionPath& aPath,
        TBool  aIsRefreshNeeded, TBool aBackwardNavigation)
    {
    TRACER("CGlxPreviewThumbnailBinding::HandleItemChangedL");
    GLX_LOG_INFO("CGlxPreviewThumbnailBinding::HandleItemChangedL()");
    
    iTimerTicked = EFalse;
    iIsRefreshNeeded = aIsRefreshNeeded;

    // remove and close old medialist   
    if( iMediaList )
	    {
	    // Reset the trialCount to 0 while deleting the medialist 
	    iTrialCount = 0;
	    iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iThumbnailContext);
        iMediaList->Close();
        iMediaList = NULL;
	    }
	    
    iMediaList = MGlxMediaList::InstanceL(aPath, KGlxIdNone, iPreviewFilter);
    iMediaList->AddContextL(iThumbnailContext, KGlxFetchContextPriorityNormal);
	// adding the medialist to observ any changes or updates done
	iMediaList->AddMediaListObserverL(this);

	if(aBackwardNavigation)
	    {
	    //On backward navigation start the timer manually, since we do not  
        //get the attribute callback.
        StartTimer();        
        }
    }

// ----------------------------------------------------------------------------
// StartTimer - Starts the timer based on the flag populateListTN's
// where the flag will be true, when initially the first thumbnails in the list
// are populated.
// ----------------------------------------------------------------------------
//    
void CGlxPreviewThumbnailBinding::StartTimer()
    {
    TRACER("CGlxPreviewThumbnailBinding::StartTimer");
    
    if (iTimer)
        {
        iTimer->Cancel();
        }
        
    iTimer->Start(KThumbnailsTimeTimeDelay, KThumbnailsTimeTimeDelay,
            TCallBack(IsTimeL, this));
    }

// ----------------------------------------------------------------------------
// StopTimer - Stop the timer
// ----------------------------------------------------------------------------
//    
void CGlxPreviewThumbnailBinding::StopTimer()
    {
    TRACER("CGlxPreviewThumbnailBinding::StopTimer");
        
    if (iTimer && iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxPreviewThumbnailBinding::HandleItemAddedL( TInt /*aStartIndex*/, TInt 
    /*aEndIndex*/, MGlxMediaList* /*aList*/ )
    {
    }

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleItemRemovedL( TInt /*aStartIndex*/, TInt 
    /*aEndIndex*/, MGlxMediaList* /*aList*/ )
    {

    }

// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// Inside this function we are going to append the index for which we have valid 
// thumbnails and when the number of available thumbnails become equal to the 
// number of slots where thumbnail to be shown we go for updating them to visual
// item.
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleAttributesAvailableL( TInt aItemIndex, 
    const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxPreviewThumbnailBinding::HandleAttributesAvailableL");

    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
        GlxFullThumbnailAttributeId( ETrue,  iGridIconSize.iWidth, 
                iGridIconSize.iHeight ) );
                                                         
    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
    
    if (KErrNotFound != aAttributes.Find( thumbnailAttribute, match ))
    	{
		const TGlxMedia& item = iMediaList->Item( aItemIndex );
        const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( 
                thumbnailAttribute );
        if (value)
	        {
	        GLX_LOG_INFO("CGlxPreviewThumbnailBinding::HandleAttributesAvailableL()");
           	StartTimer();
            }
        }
    }
    
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleFocusChangedL( NGlxListDefs::
    TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/, 
    MGlxMediaList* /*aList*/ )
    {
    
    }

// ----------   ------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleItemSelectedL(TInt /*aIndex*/, 
    TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
    {

    }

// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
 void CGlxPreviewThumbnailBinding::HandleMessageL( const CMPXMessage& /*aMessage*/, 
    MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::HandleError( TInt /*aError*/ ) 
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleCommandCompleteL( CMPXCommand* 
        /*aCommandResult*/, 
    TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleMediaL( TInt /*aListIndex*/, 
        MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleItemModifiedL( const RArray<TInt>& 
        /*aItemIndexes*/,
    MGlxMediaList* /*aList*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// HandlePopulatedL
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::HandlePopulatedL( MGlxMediaList* /*aList*/ )
    {
	TRACER("CGlxPreviewThumbnailBinding::HandlePopulatedL()");
    GLX_LOG_INFO("CGlxPreviewThumbnailBinding::HandlePopulatedL()");

	// Do cache cleanup. If iIsRefreshNeeded is set,
	// then clean up the item at 0th index 
	if (iIsRefreshNeeded)
		{
		if (iMediaList && iMediaList->Count() > 0)
			{
			GLX_LOG_INFO("**** Cache Cleanup ****");
			MGlxCache* cacheManager = MGlxCache::InstanceL();
			cacheManager->ForceCleanupMedia(iMediaList->IdSpaceId(0),
					iMediaList->Item(0).Id());
			cacheManager->Close();
			}
		}
	//Start the timer
	StartTimer();
	}

// ----------------------------------------------------------------------------
// ScaleBitmapToListSizeL
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::ScaleBitmapToListSizeL(
        CFbsBitmap* aSrcBitmap, CFbsBitmap* aDestBitmap)
    {
    TRACER("CGlxPreviewThumbnailBinding::ScaleBitmapToListSizeL()");
    TSize destSize = CHgDoubleGraphicList::PreferredImageSize();
    TSize srcSize = aSrcBitmap->SizeInPixels();
    if (destSize.iHeight * srcSize.iWidth < destSize.iWidth * srcSize.iHeight)
        {
        // Source has taller aspect than target so reduce target width
        destSize.iWidth = ((destSize.iHeight * srcSize.iWidth)
                / (srcSize.iHeight));
        }
    else
        {
        // Source has wider aspect than target so reduce target height
        destSize.iHeight = (destSize.iWidth * srcSize.iHeight)
                / srcSize.iWidth;
        }
    aDestBitmap->Create(destSize, aSrcBitmap->DisplayMode());
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(aDestBitmap);
    CleanupStack::PushL(bitmapDevice);
    CFbsBitGc* bitmapGc = CFbsBitGc::NewL();
    CleanupStack::PushL(bitmapGc);
    bitmapGc->Activate(bitmapDevice);
    bitmapGc->DrawBitmap(TRect(destSize), aSrcBitmap);
    CleanupStack::PopAndDestroy(bitmapGc);
    CleanupStack::PopAndDestroy(bitmapDevice);
    }

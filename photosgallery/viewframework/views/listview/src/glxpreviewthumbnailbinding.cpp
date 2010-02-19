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
#include "glxpreviewthumbnailbinding.h"
#include "glxgeneraluiutilities.h"
#include <glxuiutility.h>                               // For UiUtility instance

#include <glxtracer.h>
#include <glxlog.h>

#include <glxuistd.h>                    // Fetch context priority def'ns

#include <ganes/HgDoubleGraphicList.h>


const TInt KInitialThumbnailsTimeDelay(100000);
const TInt KWaitCount(5);
const TInt KThumbnailStartTimeDelay(250000);
const TInt KThumbnailIntervalTimeDelay(50000);
const TInt KPreviewThumbnailFetchCount(1);

// ----------------------------------------------------------------------------
// CWaitScheduler::NewL()
// ---------------------------------------------------------------------------- 
CGlxWaitScheduler* CGlxWaitScheduler::NewL()
    {
    TRACER("CGlxWaitScheduler::NewL()");    
    CGlxWaitScheduler* self = new( ELeave ) CGlxWaitScheduler();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

CGlxWaitScheduler::CGlxWaitScheduler()
: CActive( EPriorityStandard )
    {
    TRACER("CGlxWaitScheduler::CGlxWaitScheduler()");    
    CActiveScheduler::Add( this );
    }

void CGlxWaitScheduler::ConstructL()
    {
    TRACER("CGlxWaitScheduler::ConstructL()");
    // Do nothing
    }

CGlxWaitScheduler::~CGlxWaitScheduler()
    {
    TRACER("CGlxWaitScheduler::~CGlxWaitScheduler()");     
    Cancel();
    }

void CGlxWaitScheduler::WaitForRequest()
    {
    TRACER("CGlxWaitScheduler::WaitForRequest()");     
    SetActive();
    iScheduler.Start();
    }

void CGlxWaitScheduler::RunL()
    {
    TRACER("CGlxWaitScheduler::RunL()");     
    iScheduler.AsyncStop();
    }

void CGlxWaitScheduler::DoCancel()
    {
    TRACER("CGlxWaitScheduler::DoCancel()");   
    //Do nothing
    }

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
    iGridIconSize = uiUtility->GetGridIconSize();
    uiUtility->Close() ;
    
    // Filter that filters out any GIF, corrupted images     
    iPreviewFilter = TGlxFilterFactory::CreatePreviewFilterL();    
    iThumbnailIterator.SetRange(KPreviewThumbnailFetchCount);
    iThumbnailContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 
    TMPXAttribute tnAttr( KGlxMediaIdThumbnail,
                        GlxFullThumbnailAttributeId(ETrue,
                            iGridIconSize.iWidth,iGridIconSize.iHeight) );
    iThumbnailContext->SetDefaultSpec(iGridIconSize.iWidth,
                                                iGridIconSize.iHeight);
    iThumbnailContext->AddAttributeL(tnAttr);
    iBitmapScaler = CBitmapScaler::NewL();
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
	iPreviewItemCount.Close();
	
	if(iBitmapScaler)
	    {
        delete iBitmapScaler;
	    iBitmapScaler = NULL;
	    }
	}

// ----------------------------------------------------------------------------
// TimerTickedL: we need to update only when required i.e, when the thumbnail count 
// has not reached till the max limit to be shown
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::TimerTickedL()
    {
    TRACER("CGlxPreviewThumbnailBinding::TimerTickedL");
    
    if(iMediaList && iMediaList->Count() && iPreviewItemCount.Count() )
       	{
    	TInt thumbnailIndex = iPreviewItemCount[iProgressIndex];
    	if(thumbnailIndex < iMediaList->Count())
    	    {
            const TGlxMedia& item = iMediaList->Item(thumbnailIndex);
            TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
                GlxFullThumbnailAttributeId( ETrue,  iGridIconSize.iWidth,
                        iGridIconSize.iHeight ) );
            
            const CGlxThumbnailAttribute* value = item.ThumbnailAttribute(
                    thumbnailAttribute );
            if (value)
                {
                CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
				bitmap->Duplicate( value->iBitmap->Handle());
                //ScaleBitmapToListSizeL(value->iBitmap, bitmap);
                iObserver.PreviewTNReadyL(bitmap, NULL,iProgressIndex);
                }
    	    }
       	}
    else if (iPopulateListTNs && iMediaList && iMediaList->Count() == 0)
	    {
	    if (iTrial == KWaitCount)
		    {		   
		    iObserver.PreviewTNReadyL(NULL, NULL, KErrNotFound);
		    iTrial=0;
		    return;
		    }
	    iTrial++;
	    }
    else if(iMediaList && iMediaList->Count() )
    	{
		if(iProgressIndex > iMediaList->Count()-1 )
			{
			iProgressIndex = 0;
			}
		const TGlxMedia& item = iMediaList->Item(iProgressIndex);
		TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
			GlxFullThumbnailAttributeId( ETrue,  iGridIconSize.iWidth, 
			        iGridIconSize.iHeight ) );
		
		const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( 
		        thumbnailAttribute );
		if (value)
			{
			CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
			bitmap->Duplicate( value->iBitmap->Handle());
		    //ScaleBitmapToListSizeL(value->iBitmap, bitmap);
			iObserver.PreviewTNReadyL(bitmap, NULL,iProgressIndex);
			}
		else
			{
			if (iTrialCount == KWaitCount)
				{				
				iObserver.PreviewTNReadyL(NULL, NULL, KErrNotFound);
				iTrialCount=0;
				return;
				}
			iTrialCount++;
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
        TBool aPopulateListTNs, TBool aBackwardNavigation)
    {
    TRACER("CGlxPreviewThumbnailBinding::HandleItemChangedL");
    iTimerTicked = EFalse;
    iPopulateListTNs = aPopulateListTNs;
    iBackwardNavigation = aBackwardNavigation;
    iProgressIndex = KErrNone;

    // remove and close old medialist   
    if( iMediaList )
	    {
	    // Reset the trial and the trialCount to 0 while deleting the medialist 
	    iTrial = 0;
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
    }

// ----------------------------------------------------------------------------
// StartTimer - Starts the timer based on the flag populateListTN's
// where the flag will be true, when initially the first thumbnails in the list
// are populated.
// ----------------------------------------------------------------------------
//    
void CGlxPreviewThumbnailBinding::StartTimer(TBool aPopulateListTNs)
    {
    TRACER("CGlxPreviewThumbnailBinding::StartTimer");
    
    iPopulateListTNs = aPopulateListTNs;
    
    if (iTimer)
        {
        iTimer->Cancel();
        }
        
    if (iPopulateListTNs)
        {
        if (iBackwardNavigation)
            {
            iTimer->Start(KThumbnailIntervalTimeDelay, KThumbnailIntervalTimeDelay/KWaitCount,
                    TCallBack(IsTimeL,this));
            }
        else
            {
            iTimer->Start(KThumbnailStartTimeDelay, 
                    KInitialThumbnailsTimeDelay, TCallBack(IsTimeL,this));
            }
        }
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
            iPreviewItemCount.AppendL( aItemIndex );
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
    //Start the timer
    StartTimer(iPopulateListTNs);
    }

// ----------------------------------------------------------------------------
// ScaleBitmapToListSizeL
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::ScaleBitmapToListSizeL(
                             CFbsBitmap* aSrcBitmap, CFbsBitmap* aDestBitmap)
    {      
    TRACER("CGlxPreviewThumbnailBinding::ScaleBitmapToListSizeL()");      

    // Create the bitmap with the list preferred size
    aDestBitmap->Create(CHgDoubleGraphicList::PreferredImageSize(), EColor16MU);

    CGlxWaitScheduler* waitScheduler = CGlxWaitScheduler::NewL();
    CleanupStack::PushL( waitScheduler );

    iBitmapScaler->Scale(&waitScheduler->iStatus, *aSrcBitmap, 
            *aDestBitmap, ETrue);
    waitScheduler->WaitForRequest();

    CleanupStack::PopAndDestroy( waitScheduler );
    }

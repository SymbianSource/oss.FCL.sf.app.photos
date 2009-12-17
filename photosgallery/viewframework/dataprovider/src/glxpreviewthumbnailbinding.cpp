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
* Description:Implementation of preview thumbnail binding for the 
*             list view
*
*/




#include <mul/mulvisualitem.h>
#include <mglxmedialist.h>
#include <glxthumbnailcontext.h>
#include <glxfilterfactory.h>                         // For TGlxFilterFactory
#include <glxthumbnailattributeinfo.h>
#include "glxpreviewthumbnailbinding.h"
#include "glxuiutility.h"
#include "glxmulthumbnailvarianttype.h"
#include "glxgeneraluiutilities.h"

#include <glxtracer.h>
#include <glxlog.h>

#include <glxuistd.h>                    // Fetch context priority def'ns

const TInt KThumbnailStartTimeDelay(2000000);
const TInt KThumbnailIntervalTimeDelay(2000000);
const TInt KPreviewThumbnailProgressiveCount(14);  // iterates from -1 to < 14 thumbnail
const TInt KPreviewThumbnailFetchCount(15);       // const that is used to fetch fifteen thumbnail 
                                                 // attributes
const TInt KPotraitSlot(1);
const TInt KLandscapeSlot(3);

using namespace Alf;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CGlxPreviewThumbnailBinding* CGlxPreviewThumbnailBinding::NewL()
	{
	TRACER("CGlxPreviewThumbnailBinding::NewL");
	CGlxPreviewThumbnailBinding* self = CGlxPreviewThumbnailBinding::NewLC();
	CleanupStack::Pop(self);
	return self;	
	}

// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//
CGlxPreviewThumbnailBinding* CGlxPreviewThumbnailBinding::NewLC()
	{
	TRACER("CGlxPreviewThumbnailBinding::NewLC");
	CGlxPreviewThumbnailBinding* self = new(ELeave)CGlxPreviewThumbnailBinding;
    self->ConstructL();
	CleanupStack::PushL(self);
	return self;
	}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxPreviewThumbnailBinding::CGlxPreviewThumbnailBinding()
	{
    
	}

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::ConstructL()
    {
    TRACER("CGlxPreviewThumbnailBinding::ConstructL");
    iUiUtility = CGlxUiUtility::UtilityL();
	iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
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
        delete iThumbnailContext;
        iMediaList->Close();
        iMediaList = NULL;
		}
	if ( iUiUtility)
		{
		iUiUtility->Close ();
		}	
	// cancel any outstanding request of the timer
	if(iTimer->IsActive())
	    {
	    iTimer->Cancel();
	    }
	delete iTimer;
	iTimer = NULL;
	iPreviewItemCount.Close();
	       
	}


// ----------------------------------------------------------------------------
// CreateThumbnail
// ----------------------------------------------------------------------------
//
std::auto_ptr< GlxThumbnailVariantType > CGlxPreviewThumbnailBinding::CreateThumbnailL( TInt aIndex )const
	{
	TRACER("CGlxPreviewThumbnailBinding::CreateThumbnailL");
	const TGlxMedia& media = iMediaList->Item( aIndex );
	return auto_ptr< GlxThumbnailVariantType >( GlxThumbnailVariantType::NewL( media, 
	    iUiUtility->GetGridIconSize() ) );
	}
	
// ----------------------------------------------------------------------------
// PopulateT
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::PopulateT(Alf::MulVisualItem& aItem, const TGlxMedia& aMedia, 
		TBool aIsFocused )const
	{		
	TRACER("CGlxPreviewThumbnailBinding::PopulateT");
	 //T is used for throws as per C++ standard.Hence used instead of "L"
	 //Ignoring this for code scanner warnings - Leaving functions called in non-leaving functions.
	if ( aMedia.Properties() && aIsFocused  && iStartedShowingThumbnails && iProgressIndex != KErrNotFound )
	    {
	    if( !iCurrentOrientationLandscape )
	        {
	        
	        aItem.SetAttribute( Alf::mulvisualitem::KMulIcon1, 
	            CreateThumbnailL( MediaIndexBySlotIndex( iProgressIndex, 0 ) ).release() );
	        }
	    
	    // if orientation is landscape
	    else
	        {
	        // shows thumbnials with an interval of 2 sec at different slot
			if( iMediaList->Count() > iSlots && iPreviewItemCount.Count() >= iSlots && iTimerTicked )
				{
				GLX_LOG_INFO("inside if( iTimerTicked && iMediaList->Count() > KLandscapeSlot)");
				SetThumbnailAttributeL(aItem);
				}
			else 
			    {
			    // shows thumbnail simultaneously
			    GLX_LOG_INFO("inside else");
			    SetInitialThumbnailAttributeL(aItem, iSlots );
			    }
	         
	        } // end of else
	    
	    
	    } // end of if
	 
	}

// ----------------------------------------------------------------------------
// SetInitialThumbnailAttribute: setting initial thumbnails to slot to show 
//  simutaneously
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::SetInitialThumbnailAttributeL(Alf::MulVisualItem& aItem, 
            TInt aSlots ) const
	{
	TRACER("CGlxPreviewThumbnailBinding::SetInitialThumbnailAttribute");
	aSlots = Min( iPreviewItemCount.Count(),aSlots );
	switch( aSlots )
		{
		case 1:
			aItem.SetAttribute( Alf::mulvisualitem::KMulIcon1, 
	            CreateThumbnailL( iPreviewItemCount[0] ).release() );
			break;	
				
		case 2:
			aItem.SetAttribute( Alf::mulvisualitem::KMulIcon2, 
	            CreateThumbnailL( iPreviewItemCount[0] ).release() );
			aItem.SetAttribute( Alf::mulvisualitem::KMulIcon1, 
	            CreateThumbnailL(  iPreviewItemCount[1] ).release() );
			break;
			
		case 3:
			aItem.SetAttribute( Alf::mulvisualitem::KMulIcon3, 
	            CreateThumbnailL(  iPreviewItemCount[0] ).release() );
			aItem.SetAttribute( Alf::mulvisualitem::KMulIcon2, 
	            CreateThumbnailL(  iPreviewItemCount[1] ).release() );
			aItem.SetAttribute( Alf::mulvisualitem::KMulIcon1, 
	            CreateThumbnailL(  iPreviewItemCount[2] ).release() );
			break;
		}
	}

	
// function that sets the thumbnail to different slots
// ----------------------------------------------------------------------------
// SetThumbnailAttribute: setting thumbnial to visual item at appropriate slot
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::SetThumbnailAttributeL(Alf::MulVisualItem& aItem ) const
    {
    TRACER("CGlxPreviewThumbnailBinding::SetThumbnailAttribute");
    switch( iSlotIndex )
        {
        case 0:
            aItem.SetAttribute( Alf::mulvisualitem::KMulIcon3, 
                CreateThumbnailL( MediaIndexBySlotIndex( iProgressIndex, iSlotIndex ) ).release() );
            
            PreviewSlotIndex( iSlots, iSlotIndex ); 
            break;
     
        case 1:
            aItem.SetAttribute( Alf::mulvisualitem::KMulIcon2, 
                CreateThumbnailL( MediaIndexBySlotIndex( iProgressIndex, iSlotIndex ) ).release() );
            
            PreviewSlotIndex( iSlots, iSlotIndex );
            break;
            
        case 2:
            aItem.SetAttribute( Alf::mulvisualitem::KMulIcon1, 
                CreateThumbnailL( MediaIndexBySlotIndex( iProgressIndex, iSlotIndex ) ).release() );
                
            PreviewSlotIndex( iSlots, iSlotIndex );
            break;
        } 
    }

// ----------------------------------------------------------------------------
// PreviewSlotIndex: returns the next index where thumbails to be shown
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::PreviewSlotIndex( TInt aSlot, TInt aSlotIndex ) const
    {
    TRACER("CGlxPreviewThumbnailBinding::PreviewSlotIndex");
    if(aSlot > aSlotIndex + 1)
		{
		iSlotIndex++;
		}
	else
		{
		iSlotIndex = 0;
		}
    }

// ----------------------------------------------------------------------------
// MediaIndexBySlotIndex: returns the index for which to retrieve thumbnail
// ----------------------------------------------------------------------------
//
TInt CGlxPreviewThumbnailBinding::MediaIndexBySlotIndex(TInt aProgressIndex, 
    TInt /*aSlotIndex*/ ) const
	{
	TRACER("CGlxPreviewThumbnailBinding::MediaIndexBySlotIndex");
	  
    return aProgressIndex;
	}     


// ----------------------------------------------------------------------------
// TimerTicked: we need to update only when required i.e, when the thumbnail count 
// has not reached till the max limit to be shown
// ----------------------------------------------------------------------------
//
void CGlxPreviewThumbnailBinding::TimerTicked()
    {
    //iTimerTicked = ETrue;
    TRACER("CGlxPreviewThumbnailBinding::TimerTicked");
    iCurrentOrientationLandscape = GlxGeneralUiUtilities::IsLandscape();
    
         if(iMediaList)
            {
            iSlots = Min( iMediaList->Count(),
                     iCurrentOrientationLandscape ? KLandscapeSlot : KPotraitSlot );
            if( iPreviewItemCount.Count() >= iSlots && iProgressIndex < KPreviewThumbnailProgressiveCount
                && iProgressIndex < iMediaList->Count()-1)
                {
                iTimerTicked = ETrue;
                iProgressIndex++;
                Update();
                //iTimerTicked = EFalse;
                }
            else
                {
                iProgressIndex = KErrNotFound;
                //iStartedShowingThumbnails = EFalse;
                }
            }     
         
    }
    

// ----------------------------------------------------------------------------
// IsTime callback function invoked when timer expires
// ----------------------------------------------------------------------------
//    
TInt CGlxPreviewThumbnailBinding::IsTime( TAny* aSelf )
    {
    TRACER("CGlxPreviewThumbnailBinding::IsTime");
    if (aSelf)
        {
        TRACER("CGlxPreviewThumbnailBinding::IsTime");
        CGlxPreviewThumbnailBinding* self = static_cast<CGlxPreviewThumbnailBinding*>(aSelf);
        if(self)
            {
            self->TimerTicked();
           }
        
       
        }
    return KErrNone;        
    }

// ----------------------------------------------------------------------------
// HandleFocusChanged
// This function resets any existing timer and starts a new timer tick
// ----------------------------------------------------------------------------
//    
    
CGlxBinding::TResponse CGlxPreviewThumbnailBinding::HandleFocusChanged( TBool aIsGained )
    {
    TRACER("CGlxPreviewThumbnailBinding::HandleFocusChanged");
	if(aIsGained)
	    {
	    if(iTimer)
	        {
	        iTimer->Cancel();
	        }
	   
	     if(!iTimer->IsActive() ) 
	        {
	        iTimer->Start(KThumbnailStartTimeDelay, KThumbnailIntervalTimeDelay,
	            TCallBack(IsTime,this));
	      
	        }
	    }
	//return EUpdateRequested;
	return ENoUpdateNeeded;
    }
    

// ----------------------------------------------------------------------------
// HandleItemChangedL
// This function basically closes the old medialist if any for ex. Tags, 
// Captured.. and then opens a new medialist with teh required filter.
// ----------------------------------------------------------------------------
//    
void CGlxPreviewThumbnailBinding::HandleItemChangedL(const CMPXCollectionPath& aPath )
    {
    TRACER("CGlxPreviewThumbnailBinding::HandleItemChangedL");
    iStartedShowingThumbnails = EFalse;
    iTimerTicked = EFalse;
    iProgressIndex = KErrNotFound;
    iPreviewItemCount.Close();
    // remove and close old medialist   
    if( iMediaList )
	    {
	    iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iThumbnailContext);
        delete iThumbnailContext;
        iMediaList->Close();
        iMediaList = NULL;
	    }
	    
	// Filter that filters out any GIF, corrupted images    
    CMPXFilter* filter = NULL;
    filter = TGlxFilterFactory::CreatePreviewFilterL(); 
    CleanupStack::PushL( filter );
    // create new medialist with the required filter which filters out all DRM, GIFS and corrupt 
    // thumbnial
	iMediaList = MGlxMediaList::InstanceL( aPath ,KGlxIdNone, filter);
	iThumbnailContext = CGlxThumbnailContext::NewL( &iThumbnailIterator ); // set the thumbnail context
	iThumbnailIterator.SetRange( KPreviewThumbnailFetchCount ); // request for fifiteen thumbnails
	iThumbnailContext->SetDefaultSpec( iUiUtility->GetGridIconSize().iWidth,iUiUtility->GetGridIconSize().iHeight );
	iMediaList->AddContextL(iThumbnailContext ,KGlxFetchContextPriorityNormal );
	// adding the medialist to observ any changes or updates done
	iMediaList->AddMediaListObserverL(this);
	CleanupStack::PopAndDestroy( filter );
    }
    

// ----------------------------------------------------------------------------
// HasFirstThumbnail
// ----------------------------------------------------------------------------
// 
 TBool CGlxPreviewThumbnailBinding::HasFirstThumbnail( const RArray< TMPXAttribute >& aAttributes )
     {
     TRACER("CGlxPreviewThumbnailBinding::HasFirstThumbnail");
     TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
                                          GlxFullThumbnailAttributeId( ETrue, 
                                                          iUiUtility->GetGridIconSize().iWidth,iUiUtility->GetGridIconSize().iHeight) );
                                                          
     TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
     return ( KErrNotFound != aAttributes.Find( thumbnailAttribute, match ) );    
     }
// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxPreviewThumbnailBinding::HandleItemAddedL( TInt aStartIndex, TInt 
    aEndIndex, MGlxMediaList* /*aList*/ )
    {
	TRACER("CGlxPreviewThumbnailBinding::HandleItemAddedL");
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
                  GlxFullThumbnailAttributeId( ETrue,  iUiUtility->GetGridIconSize().iWidth, iUiUtility->GetGridIconSize().iHeight ) );
    for(TInt i = aStartIndex; i <= aEndIndex; i++)
           {
           const TGlxMedia& item = iMediaList->Item( i );
                   const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
                   if (value)
                       {
                       iPreviewItemCount.AppendL( i );
                       iStartedShowingThumbnails = ETrue;
                       iProgressIndex = 0;
                       }
           }
		   if(!iTimer->IsActive() && iMediaList && iMediaList->Count()) 
		   {
		      GLX_LOG_INFO("CGlxPreviewThumbnailBinding::HandleItemAddedL start timer");
		      iTimer->Start(KErrNone, KThumbnailIntervalTimeDelay, TCallBack(IsTime,this));
           }
           
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
    GLX_LOG_INFO1("CGlxPreviewThumbnailBinding::HandleAttributesAvailableL aItemIndex %d ", aItemIndex);
    TInt count = aAttributes.Count();
    // whether orientation is landscape or portrait
	iCurrentOrientationLandscape = GlxGeneralUiUtilities::IsLandscape();
	// islots has number as to how many slots need to be filled
    iSlots = Min( iMediaList->Count(),
         iCurrentOrientationLandscape ? KLandscapeSlot : KPotraitSlot );

    for(TInt i = 0; i< count; i++)
        {
        TMPXAttribute attribute = aAttributes[i];
        }
    if( !iStartedShowingThumbnails )
        {
        GLX_LOG_INFO(" inside if( !iStartedShowingThumbnails )");
        iStartedShowingThumbnails = HasFirstThumbnail(aAttributes);
        if( iStartedShowingThumbnails )
            {
            //if relevent thumbnial then add the index to an array
            GLX_LOG_INFO("inside if( iStartedShowingThumbnails");
            iStartedShowingThumbnails = EFalse;
            iPreviewItemCount.AppendL( aItemIndex );
            iProgressIndex++;
            // checks if the number of thumbnials are ready to display 	
            if(iPreviewItemCount.Count() == iSlots )
            	{
                GLX_LOG_INFO("inside if(iPreviewItemCount.Count() >= iSlots && iTimerTicked )");
                // update the thumbnails and set the flag iStartShowingThumbnails which willbe
                // checked at the time of populating teh visual item.
                iStartedShowingThumbnails = ETrue;
                Update();
        		
        		}// end of if(iPreviewItemCount.Count() == iSlots  )
            }// end of if(iStartedShowingThumbnails)
        }
    }
    
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleFocusChangedL( NGlxListDefs::
    TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/ )
    {
   // HandleFocusChanged(ETrue);
   	TRACER("CGlxPreviewThumbnailBinding::HandleFocusChangedL");
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
void CGlxPreviewThumbnailBinding::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, 
    TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxPreviewThumbnailBinding::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/,
    MGlxMediaList* /*aList*/ )
    {
    
    }

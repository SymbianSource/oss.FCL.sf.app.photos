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
* Description:    Implementation of Cloud view
 *
*/



#include "glxcontainerinfobubble.h" // class header


#include <glxtexturemanager.h> // For CGlxTextureManager
#include <glxuiutility.h>   // For CGlxUiUtility
#include <glxtracer.h>					
#include <glxlog.h>					// For Logs
#include <mpxmediageneraldefs.h>	// For Attribute constants
#include <glxcollectiongeneraldefs.h> //for KGlxMediaCollectionPluginSpecificSubTitle
#include <glxthumbnailattributeinfo.h>   // for 'KGlxMediaIdThumbnail'
#include <glxthumbnailcontext.h> //for thumbnail context
#include <glxattributecontext.h>  //for attribute context
#include <glxuistd.h>
#include <glxicons.mbg>
#include <mglxmedialist.h> //for medialist
#include <data_caging_path_literals.hrh>

// INCLUDE FILES
#include "glxerrormanager.h"

const TInt KThumbnailWidth = 50;
const TInt KThumbnailHeight = 50;

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxContainerInfoBubble::~CGlxContainerInfoBubble()
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::~CGlxContainerInfoBubble");

    if ( iTexture )
        {
        if( iUiUtility )
            {          
            iUiUtility->GlxTextureManager().RemoveTexture( *iTexture );
            }
        }

	if ( iTimer)
		{
		iTimer->Cancel ();//cancels any outstanding requests
		delete iTimer;
		}
	if ( iMediaTitle)
		{
		delete iMediaTitle;
		}
	if ( iMediaSubTitle)
		{
		delete iMediaSubTitle;
		}
	if ( iMediaList)
		{
		iMediaList->RemoveContext (iAttributeContext);
		iMediaList->RemoveContext (iThumbnailContext);
		iMediaList->RemoveMediaListObserver (this);
		}
	if ( iAttributeContext)
		{
		delete iAttributeContext;
		}
	if ( iThumbnailContext)
		{
		delete iThumbnailContext;
		}
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CGlxContainerInfoBubble *CGlxContainerInfoBubble::NewL(MGlxMediaList
		*aMediaList, CAlfEnv &aEnv, CAlfControl &aOwnerControl)
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::NewL");
	CGlxContainerInfoBubble *self = CGlxContainerInfoBubble::NewLC(aMediaList,aEnv, aOwnerControl);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxContainerInfoBubble *CGlxContainerInfoBubble::NewLC(MGlxMediaList
		*aMediaList, CAlfEnv &aEnv, CAlfControl &aOwnerControl)
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::NewLC");
	CGlxContainerInfoBubble *self = new(ELeave)CGlxContainerInfoBubble(aMediaList);
	CleanupStack::PushL(self);
	self->ConstructL(aEnv, aOwnerControl);
	return self;
	}

// ---------------------------------------------------------------------------
// ShowNow()
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::ShowNowL()
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::ShowNow");
	// if we got texture, titles and timer is complete, show the bubble
	if ( iTexture && iMediaTitle && iMediaSubTitle && iTimerComplete )
		{
		//last three items will come from medialist
		DisplayBubbleL (iPos,*iTexture, *iMediaTitle, *iMediaSubTitle); //last three items will come from medialist
		}
	}

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::ConstructL(CAlfEnv &aEnv, CAlfControl
&aOwnerControl)
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::ConstructL");
	CGlxInfoBubble::BaseConstructL (aOwnerControl, aEnv);
	iTimer = CGlxBubbleTimer::NewL (this);
	//medialist code
	iAttributeContext = CGlxDefaultAttributeContext::NewL ();
	iAttributeContext->AddAttributeL (KMPXMediaGeneralTitle);
	iAttributeContext->AddAttributeL (KGlxMediaCollectionPluginSpecificSubTitle);

	iThumbnailContext = CGlxDefaultThumbnailContext::NewL ();
	iThumbnailContext->SetDefaultSpec (50, 50);
	iThumbnailContext->SetRangeOffsets (5, 5);
	iAttributeContext->SetRangeOffsets (10, 10);

	iMediaList->AddContextL (iAttributeContext, 
	                            KGlxFetchContextPriorityCloudViewInfoBubble );
	                            
	iMediaList->AddMediaListObserverL ( this);
	iMediaList->AddContextL (iThumbnailContext, 
	                            KGlxFetchContextPriorityCloudViewInfoBubble );  
	                            
	TInt listCount = iMediaList->Count ();
	GLX_LOG_INFO1("GLX_CLOUD ::CGlxContainerInfoBubble::ConstructL iMediaList->FocusIndex  %d ", iMediaList->FocusIndex ());
	GLX_LOG_INFO1("GLX_CLOUD ::CGlxContainerInfoBubble::ConstructL MedialistCount  %d ",listCount);

	if ( listCount)
		{
		SetAttributesL ();
		}
	}

// ---------------------------------------------------------------------------
// SetAttributes()
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::SetAttributesL()
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::SetAttributes()");
	
    // Set the attribute context here for fetching the list data		
    TSize size(KThumbnailWidth, KThumbnailHeight);	
    	
	TMPXAttribute attrThumbnail(KGlxMediaIdThumbnail
	    ,GlxFullThumbnailAttributeId (ETrue, size.iWidth, size.iHeight)); 
    
	TGlxMedia media = iMediaList->Item (iMediaList->FocusIndex());
	
	TGlxIdSpaceId IdSpaceId = iMediaList->IdSpaceId(iMediaList->FocusIndex());
	
	// Get the Title, ignore return value
	GetMediaTitleL( media);

	// Get the subtitle, ignore return value
	GetMediaSubTitleL( media);
	
	//Get the thumbnail, ignore return value
	GetMediaThumbNailL( media, IdSpaceId, size, attrThumbnail );
	
	ShowNowL();
	}
// ---------------------------------------------------------------------------
// SetAttributes()
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::SetAttributesL(
    const RArray<TMPXAttribute>& aAttributes,
    const TInt& aAttributescount )
	{
    TRACER("GLX_CLOUD::CGlxContainerInfoBubble::SetAttributesL");
    // Set the attribute context here for fetching the list data
    TSize size(KThumbnailWidth, KThumbnailHeight);	
	TMPXAttribute attrThumbnail(KGlxMediaIdThumbnail,
		GlxFullThumbnailAttributeId (ETrue, size.iWidth, size.iHeight)); 
	
    const TGlxMedia media = iMediaList->Item (iMediaList->FocusIndex());

	TGlxIdSpaceId IdSpaceId = iMediaList->IdSpaceId(iMediaList->FocusIndex());	
	
	TBool updateBubble = EFalse;
	// Loop untill it checks for all the avialable attributes
	for (TInt i = aAttributescount - 1; i >= 0 ; i--)
	    {
	    //Get the Title if the fetched attribute is MediaTitle
	    if ( aAttributes[i] == KMPXMediaGeneralTitle )
	        {
            // update bubble if we changed the title
	        updateBubble = GetMediaTitleL(media);
	        }
	    
	    //Get the SubTitle if the fetched attribute is MediaSubTitle
    	else if ( aAttributes[i] == KGlxMediaCollectionPluginSpecificSubTitle)
	        {
            // update bubble if we changed the subtitle
	        updateBubble = GetMediaSubTitleL(media);
    	    }
    	//Get the ThumbNail if the fetched attribute is MediaThumbNail   
    	else if ( (attrThumbnail.ContentId() == aAttributes[i].ContentId())
    	     && (attrThumbnail.AttributeId() == aAttributes[i].AttributeId() ) )
	        {
            // update bubble if thumbnail changed
	        updateBubble = 
	            GetMediaThumbNailL( media, IdSpaceId, size, attrThumbnail );
   	        }
	     } 
	if ( updateBubble )
	    {
	    ShowNowL();
	    }
	}


// ---------------------------------------------------------------------------
// GetMediaTitle()
// ---------------------------------------------------------------------------
TBool CGlxContainerInfoBubble::GetMediaTitleL(const TGlxMedia& aMedia)
    {
    TRACER("GLX_CLOUD::CGlxContainerInfoBubble::GetMediaTitleL");
    // get current title
    const TDesC& title = aMedia.Title();
    // if we dont have a title
   	if( !iMediaTitle )
   	    {
   	    // we dont have a title so get the title
		iMediaTitle = title.AllocL();
		return ETrue;
	    }
    // if we have a title, but its different than new one (tag renamed)
    else if( iMediaTitle->Compare( title ) )
        {
        // allocate the new title
        HBufC* tempTitle = title.AllocL();
        // got new title so release old
        delete iMediaTitle;
        // assign new in place of old
        iMediaTitle = tempTitle;
        // did change title
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// GetMediaSubTitle()
// ---------------------------------------------------------------------------
TBool CGlxContainerInfoBubble::GetMediaSubTitleL(const TGlxMedia& aMedia)
    {
    TRACER("GLX_CLOUD::CGlxContainerInfoBubble::GetMediaSubTitleL");
    // get current subtitle
    const TDesC& title = aMedia.SubTitle();
    // if we dont have a subtitle
    if( !iMediaSubTitle )
        {
        // we dont have a subtitle so get the title
        iMediaSubTitle = title.AllocL();
        return ETrue;
        }
    // if we have a subtitle, but its different than new one (count changed)
    else if( iMediaSubTitle->Compare( title ) )
        {
        // allocate the new subtitle
        HBufC* tempTitle = title.AllocL();
        // got new so release old
        delete iMediaSubTitle;
        // assign new in place of old
        iMediaSubTitle = tempTitle;
        // did change subtitle
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// GetMediaThumbNailL()
// ---------------------------------------------------------------------------
//
TBool CGlxContainerInfoBubble::GetMediaThumbNailL(const TGlxMedia& aMedia,
    TGlxIdSpaceId& aIdSpaceId,TSize& aSize, 
    TMPXAttribute& aAttrThumbnail)
    {
    TRACER("GLX_CLOUD ::CGlxContainerInfoBubble::SetAttributesL Thumbnail available");

    //Get the thumbnail
    if ( aMedia.ThumbnailAttribute( aAttrThumbnail ) )
        {
        // take the address of the created texture
        iTexture = &( CreateThumbnailTextureL( aMedia, aIdSpaceId, aSize ) );
        return ETrue;
        }
    //If there is an error in thumbnail attribute i.e the thumbnail is broken then create broken thumbnail
    else if (GlxErrorManager::HasAttributeErrorL(aMedia.Properties(), KGlxMediaIdThumbnail) != KErrNone )
        {
        // create 'default' icon
        TFileName resFile(KDC_APP_BITMAP_DIR);
        resFile.Append(KGlxIconsFilename);

        TSize iconSize( KThumbnailWidth, KThumbnailHeight );    
        
        //Create the texture for broken thumbnail
        iTexture = &(iUiUtility->GlxTextureManager().CreateIconTextureL(
            EMbmGlxiconsQgn_prop_image_corrupted, resFile, iconSize ) );
        return ETrue;
        }
    return EFalse;
    }
    
        	
// ---------------------------------------------------------------------------
// TimerCompleteL()
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::TimerCompleteL()
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::TimerCompleteL");
	iTimerComplete =ETrue;
	TRAP_IGNORE (ShowNowL ());
	}

// ---------------------------------------------------------------------------
// SetFocus()
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::SetFocus(TPoint aPos)
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::SetFocus");
	DisappearBubble ();
	
	if ( iTexture )
	    {
	    iUiUtility->GlxTextureManager().RemoveTexture( *iTexture );
	    ResetImage();
	    iTexture = NULL;
	    }
	iPos = aPos;
	//fetch the attributes from medialsit
	iTimerComplete =EFalse;
	iTimer->Cancel ();//cancels any outstanding requests
	if ( iMediaTitle)
		{
		delete iMediaTitle;
		iMediaTitle =NULL;
		}
	if ( iMediaSubTitle)
		{
		delete iMediaSubTitle;
		iMediaSubTitle =NULL;
		}
	TRAP_IGNORE(SetAttributesL());
	iTimer->SetDelay (1500000);//1.5 seconds delay
	}

// ---------------------------------------------------------------------------
// Default C++ Constructor
// ---------------------------------------------------------------------------
//
CGlxContainerInfoBubble::CGlxContainerInfoBubble(MGlxMediaList *aMediaList) :
	CGlxInfoBubble(), iMediaList(aMediaList)
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::CGlxContainerInfoBubble");
	}

//medialist Observers

// ---------------------------------------------------------------------------
// HandleItemAddedL().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleItemAddedL(TInt /*aStartIndex*/,
		TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
	{
	}

// ---------------------------------------------------------------------------
// HandleMediaL().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleMediaL(
    TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
	{
	}

// ---------------------------------------------------------------------------
// HandleItemRemoved().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleItemRemovedL(TInt /*aStartIndex*/,
		TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
	{
	}

// ---------------------------------------------------------------------------
// HandleItemModifiedL().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleItemModifiedL(
		const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
	{
	}

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleAttributesAvailableL(TInt aItemIndex,
		const RArray<TMPXAttribute>& aAttributes, MGlxMediaList*/* aList*/)
	{
	TRACER("GLX_CLOUD::CGlxContainerInfoBubble::HandleAttributesAvailableL");

	TInt attCount = aAttributes.Count ();
	GLX_LOG_INFO1("GLX_CLOUD ::CGlxContainerInfoBubble::HandleAttributesAvailableL  count: %d ",attCount);
	GLX_LOG_INFO1("GLX_CLOUD ::CGlxContainerInfoBubble::HandleAttributesAvailableL  Item Index: %d ", aItemIndex);

	if ( ( attCount ) && (iMediaList->FocusIndex() == aItemIndex ) )
		{
		SetAttributesL( aAttributes, attCount );
		}
	}

// ---------------------------------------------------------------------------
// HandleFocusChangedL().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleFocusChangedL(
		NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/,
		TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
	{
	}

// ---------------------------------------------------------------------------
// HandleItemSelected().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleItemSelectedL(TInt /*aIndex*/,
		TBool /*aSelected*/, MGlxMediaList* /*aList*/)
	{
	}

// ---------------------------------------------------------------------------
// HandleMessageL().
// ---------------------------------------------------------------------------
//
void CGlxContainerInfoBubble::HandleMessageL(const CMPXMessage& /*aMessage*/,
		MGlxMediaList* /*aList*/)
	{
	}

//  End of File

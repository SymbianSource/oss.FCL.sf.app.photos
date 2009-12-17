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
* Description:    Implements rendering of the Image/Video
*
*/




#include "glxupnprendererimpl.h"

#include <glxattributecontext.h>         // CGlxAttributeContext
#include <glxattributeretriever.h>  	 // CGlxAttributeRetriever
#include <glxuiutility.h>				 // CGlxUiUtility
#include <glxtracer.h>                      
#include <glxlog.h>                      // GLX_LOG 
#include <glxmedialist.h>                // CGlxMediaList
#include <glxthumbnailattributeinfo.h>   // KGlxMediaIdThumbnail
#include <glxthumbnailcontext.h>         // CGlxThumbnailContext
#include <glxuistd.h>                    // Fetch context priority def'ns
#include <glxerrormanager.h>             // GlxErrorManager
#include <glxmedia.h>                    // TGlxMedia
#include "glxupnprenderer.h"    
// CONSTANTS
const TInt KVideoIconThumbnailWidth = 640;
const TInt KVideoIconThumbnailHeight = 480;

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
CGlxUpnpRendererImpl* CGlxUpnpRendererImpl::NewL()
    {
    TRACER("CGlxUpnpRendererImpl::NewL()");  
    
    CGlxUpnpRendererImpl* self = new(ELeave) CGlxUpnpRendererImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//---------------------------------------------------------------------------------	
//	Defalut Constructor
//---------------------------------------------------------------------------------	
CGlxUpnpRendererImpl::CGlxUpnpRendererImpl()
    {    
    //Do Nothing
    }
     
//---------------------------------------------------------------------------------	
//	2nd phase Constructor
//---------------------------------------------------------------------------------	
void CGlxUpnpRendererImpl::ConstructL()
    {    
    TRACER("CGlxUpnpRendererImpl::ConstructL()");        
    
    // Create the asynchronous callback for focus changes
    iHandleFocusChangeCallback = new ( ELeave )
        CAsyncCallBack( TCallBack( DoFocusChanged, this ),
                CActive::EPriorityStandard );

    //Create the showcommand to query the upnpframework 
    iUpnpShowCommand = CUpnpShowCommand::NewL(this);   
     

    if(iUpnpShowCommand)
	    {
	    iUpnpSupported = ETrue;	    
	    }
    //Allocate the UPNP resources to show the Image/Video	    
	if(iUpnpSupported)
		{
	    iUpnpShowCommand->StartShowingL();	
	    }    	
    
    iUpnpErrorStatus = NGlxUpnpRenderer::EUpnpKErrNone;    
    PrepareMediaListL(); 
        
    }
    
//---------------------------------------------------------------------------------	
//	Destructor
//---------------------------------------------------------------------------------	
CGlxUpnpRendererImpl::~CGlxUpnpRendererImpl()
    {
    TRACER("CGlxUpnpRendererImpl::~CGlxUpnpRendererImpl()");    

    // delete also cancels the callback
    delete iHandleFocusChangeCallback;
              
    if ( iActiveMediaList )                             //cleanup the current list 
        {
        iActiveMediaList->RemoveMediaListObserver(this);
        iActiveMediaList->RemoveContext( iAttributeContext );	
        iActiveMediaList->RemoveContext( iThumbnailContext );	        
        }        
    if (iActiveMediaListResolver)
        {
        iActiveMediaListResolver->Close( this );
        }        
    delete iAttributeContext;    
        
    delete iThumbnailContext;   
        
    delete iThumbnailSaver; 
    
    if( iUpnpShowCommand ) 
        {
        //To check if we can delete the showcommand without StopShowing
        TRAP_IGNORE(iUpnpShowCommand->StopShowingL ( ) );       
        delete iUpnpShowCommand;      
        }
    }	


//---------------------------------------------------------------------------------	
// If the access point is defined
//---------------------------------------------------------------------------------	
TBool CGlxUpnpRendererImpl::IsSupported()
    {
    TRACER("CGlxUpnpRendererImpl::IsSupported()");    
    
    TInt isSupported = EFalse;
    //need to trap as we cannot leave
    
    TRAP_IGNORE( isSupported = CUpnpShowCommand::IsAvailableL ( ) );
    
    return isSupported;
    }
//---------------------------------------------------------------------------------	
// Error Status when Show Image/Video
//---------------------------------------------------------------------------------		    
NGlxUpnpRenderer::TErrorStatus CGlxUpnpRendererImpl::UpnpErrorStatus()
	{
	TRACER("CGlxUpnpRendererImpl::UpnpErrorStatus()");
	
	//The UPnP Show Image/Video error status after the Show request to UPnP Framework
	return iUpnpErrorStatus;		
	}

//---------------------------------------------------------------------------------	
// Show the Video after a user action
//---------------------------------------------------------------------------------		
void CGlxUpnpRendererImpl::UpnpShowVideoL()
    {
    TRACER("CGlxUpnpRendererImpl::UpnpShowVideoL");
    User::LeaveIfError( GlxAttributeRetriever::RetrieveL ( *iAttributeContext,*iActiveMediaList ) );
    
    if(iUpnpSupported)
	    {
    	const TDesC& videoUri = iActiveMediaList->Item( iActiveMediaList->FocusIndex() ).Uri();
    	iUpnpShowCommand->ShowVideoL( videoUri );
	    }
    
    iIsVideoShow = ETrue;  
    //Hiding the HUI display,so that the UPnPFw will take the
    //view control and update the view for a video
    CGlxUiUtility::HideAlfDisplayL();
    }

//---------------------------------------------------------------------------------	
//	Preare the media list
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::PrepareMediaListL()
    {
    TRACER("CGlxUpnpRendererImpl::PrepareMediaListL");

    if ( !iActiveMediaListResolver ) //get the singleton instance for the resolver	
        {
        iActiveMediaListResolver= MGlxActiveMediaListResolver::InstanceL( this );
        }
    iActiveMediaList= iActiveMediaListResolver->ActiveMediaList();   //get the currently active medialist

    if ( iActiveMediaList )
        {
        CreateContextL();
        iActiveMediaList->AddContextL( iAttributeContext, 
                                            KGlxFetchContextPriorityNormal );
        iActiveMediaList->AddContextL( iThumbnailContext, 
                                            KGlxFetchContextPriorityNormal );
        iActiveMediaList->AddMediaListObserverL(this);
        }
    }


//---------------------------------------------------------------------------------	
// Create the context for the medialist
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::CreateContextL()
    {
    TRACER("CGlxUpnpRendererImpl::CreateContextL");

    iAttributeContext = CGlxDefaultAttributeContext::NewL( );
    iAttributeContext->AddAttributeL( KMPXMediaGeneralUri );
    
    iThumbnailContext = CGlxDefaultThumbnailContext::NewL( );          // set the thumbnail context
    iThumbnailContext->SetDefaultSpec( KVideoIconThumbnailWidth, KVideoIconThumbnailHeight );  // 640 pixels wide and 480 pixel height-VGA.
    }

//---------------------------------------------------------------------------------	
//	media object item added	     
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/,MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxUpnpRendererImpl::HandleItemAddedL");	
    }
	
//---------------------------------------------------------------------------------	
//	media object is now available for an item 
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxUpnpRendererImpl::HandleItemAddedL");	
    }
    
//---------------------------------------------------------------------------------	
//	media item was removed from the list
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/,MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxUpnpRendererImpl::HandleItemAddedL");	
    }
    
//---------------------------------------------------------------------------------	
//	Media item was changed
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/) 
    {
    TRACER("CGlxUpnpRendererImpl::HandleItemAddedL");	
    }

//---------------------------------------------------------------------------------	
//	Attribute is available	     
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::HandleAttributesAvailableL(TInt aItemIndex,const RArray<TMPXAttribute>& aAttributes,
                                                MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxUpnpRendererImpl::HandleAttributesAvailableL");	
    
    if ( aItemIndex == iActiveMediaList->FocusIndex() )
        {
        SendFocusedItemL(aItemIndex,aAttributes);   
        }
    }
	
//---------------------------------------------------------------------------------	
//	 Focus has moved
//---------------------------------------------------------------------------------	
void CGlxUpnpRendererImpl::HandleFocusChangedL(
    NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/,
        TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxUpnpRendererImpl::HandleFocusChangedL");
    // fix for ERBS-7CFCJE, make focus change handling asynchronous to prevent 
    // UPnP from blocking our garbage collection if the UPnP framework
    // displays a dialog (e.g. when showing a DRM-protected item).
    if ( iHandleFocusChangeCallback->IsActive() )
        {
        iHandleFocusChangeCallback->Cancel();
        }
    iHandleFocusChangeCallback->CallBack();
    } 
    
//---------------------------------------------------------------------------------	
//  Item has been selected
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxUpnpRendererImpl::HandleItemAddedL");	
    }
    
//---------------------------------------------------------------------------------	
//	Notification from the collection
//---------------------------------------------------------------------------------		
void CGlxUpnpRendererImpl::HandleMessageL(const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxUpnpRendererImpl::HandleItemAddedL");	
    }


//From MGlxActiveMediaListChangeObserver	
//---------------------------------------------------------------------------------	
//	Active media list pointer has changed.
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::HandleActiveMediaListChanged()
    {
    TRACER("CGlxUpnpRendererImpl::HandleActiveMediaListChanged()");
    
    if ( iActiveMediaList )                             //cleanup the current list 
        {
        iActiveMediaList->RemoveMediaListObserver( this );
        iActiveMediaList->RemoveContext( iAttributeContext );
        iActiveMediaList->RemoveContext( iThumbnailContext );	        
        }    
    TRAP_IGNORE(PrepareMediaListL());  // will give a new list 	
    }

	
//From MUpnpCommandObserver	
//---------------------------------------------------------------------------------	
// Show Command has been completed.
//---------------------------------------------------------------------------------	

void CGlxUpnpRendererImpl::CommandComplete( TInt aStatusCode ) 
    {
    TRACER("CGlxUpnpRendererImpl::CommandComplete");  
    
    TRAP_IGNORE( CommandCompleteL(aStatusCode) );
    }
  
 	
//---------------------------------------------------------------------------------	
// Show Command has been completed.
//---------------------------------------------------------------------------------	
//    
void CGlxUpnpRendererImpl::CommandCompleteL( TInt aStatusCode )   
    {
    TRACER("CGlxUpnpRendererImpl::CommandCompleteL");   
   
    if( KErrNone == aStatusCode )
        {
        GLX_LOG_INFO("Error code is KErrNone");        
        iUpnpErrorStatus = NGlxUpnpRenderer::EUpnpKErrNone;        
        }

    else if( KErrDisconnected == aStatusCode )   
        {
        GLX_LOG_INFO("Error code is KErrDisconnected");
        iUpnpErrorStatus = NGlxUpnpRenderer::EUpnpKErrDisconnected;  
           		
        iIsVideoShow = EFalse; 

        //Show the HUI display   
        CGlxUiUtility::ShowAlfDisplayL();
      	   	     	
        // Send state changed command
        GlxUpnpRenderer::ChangeCommandStateL(); 
        } 
	 
	 if(iUpnpSupported)
		 {
		 if( KUpnpCommandStatusStartPlayVideo  == aStatusCode )
	        {
	        GLX_LOG_INFO("KUpnpCommandStatusStartPlayVideo");
	        // Photos hiding their UI
	        CGlxUiUtility::HideAlfDisplayL();
	        }	
		 }
        
    //Check if the show on the rendering device was a video	        
    else if( ( iIsVideoShow ) && ( GlxUpnpRenderer::Status() == NGlxUpnpRenderer::EActive ) )
        {
        GLX_LOG_INFO("Video play is complete");        
        //Need to trap as we cannot leave
        ShowImageAfterVideoL();
        }             
    }
 
//---------------------------------------------------------------------------------	
// Display the Image after the Video play is Complete
//---------------------------------------------------------------------------------	    
  void CGlxUpnpRendererImpl::ShowImageAfterVideoL()
  	{
  	TRACER("CGlxUpnpRendererImpl::ShowVideoComplete");
  	
  	//Show the HUI display once the video play is completed    
    CGlxUiUtility::ShowAlfDisplayL();
     
    if(iUpnpSupported)
	     {
	     iUpnpShowCommand->ShowImageL( iVideoThumbnail );        	
	     }
	     
    iIsVideoShow = EFalse;  	
    }


//From MGlxThumbnailSaveComplete	
//---------------------------------------------------------------------------------	
// Call back for thumbnail save completion
//---------------------------------------------------------------------------------	
void CGlxUpnpRendererImpl::HandleFileSaveCompleteL(const TDesC& aPath)
    {
    TRACER("CGlxUpnpRendererImpl::HandleFileSaveCompleteL");
    
    iVideoThumbnail.Copy( aPath );
    
    if(iUpnpSupported)
	    {
	    iUpnpShowCommand->ShowImageL( iVideoThumbnail );	
	    }    
    }

//---------------------------------------------------------------------------------	
// Send the focussed item to the device
//---------------------------------------------------------------------------------	

void  CGlxUpnpRendererImpl::SendFocusedItemL(TInt aNewIndex)	
    {
    TRACER("CGlxUpnpRendererImpl::SendFocusedItemL");
    
    if(KErrNotFound!=aNewIndex)
        {
        const TGlxMedia& item = iActiveMediaList->Item(aNewIndex);       
        TMPXAttribute attrThumb(KGlxMediaIdThumbnail, 
        GlxFullThumbnailAttributeId(ETrue, KVideoIconThumbnailWidth, KVideoIconThumbnailHeight));
        if ( (iActiveMediaList) && (!item.IsStatic()) )
            {
            if( item.Uri().Length() > 0)
                {
                GetImageL(item);
                GetVideoL(item,attrThumb);
                }                
            }            
        }
    }
        
//---------------------------------------------------------------------------------	
// Send the focussed item to the device
//---------------------------------------------------------------------------------	
void  CGlxUpnpRendererImpl::SendFocusedItemL(TInt aNewIndex,const RArray<TMPXAttribute>& aAttributes)	
    {
    TRACER("CGlxUpnpRendererImpl::SendFocusedItemL");

    if( KErrNotFound != aNewIndex )
        {
        const TGlxMedia& item = iActiveMediaList->Item(aNewIndex);
        TMPXAttribute attrThumb(KGlxMediaIdThumbnail, 
        GlxFullThumbnailAttributeId(ETrue, KVideoIconThumbnailWidth, KVideoIconThumbnailHeight));

        // Loop untill it checks for all the avialable attributes
        for ( TInt i = aAttributes.Count() - 1; i >= 0 ; i-- )
            {
            if ( (iActiveMediaList) && (!item.IsStatic()) )
                {
                if( item.Uri().Length() > 0)               
                    {
                    //retrive the attribute for showing image                       
                    if( EMPXImage == item.Category())
                        {                    
                        GetImageL(item);
                        }
                    //retrive the attribute for showing Video                    
                    else if ( EMPXVideo == item.Category() )
                        {                  
                        GetVideoL(item,attrThumb);
                        }
                    }
                }
            }
        }
    }

//---------------------------------------------------------------------------------	
// Get the image attribute
//---------------------------------------------------------------------------------	
void  CGlxUpnpRendererImpl::GetImageL(const TGlxMedia& aMedia)
    {
    TRACER("CGlxUpnpRendererImpl::GetImageL");
    //retrive the attribute for showing image 
    const CGlxMedia* item = aMedia.Properties();
    if(EMPXImage == aMedia.Category())
    {
    if (!iThumbnailSaver)
        {
        iThumbnailSaver=CGlxThumbnailSaver::NewL( this );
        }
    if((GlxErrorManager::HasAttributeErrorL(item, KGlxMediaIdThumbnail) != KErrNone))
   		{
     	iThumbnailSaver->CreateDefaultImageIconL();
    	}
    if  ((iPreviouslySentMediaId != aMedia.Id( ) ) )
        {
        iPreviouslySentMediaId=aMedia.Id();        
        //show the image Via upnp device
        if(iUpnpSupported)
	        {
	        const TDesC& imageUri = aMedia.Uri();
	        iUpnpShowCommand->ShowImageL( imageUri );
	        }        
        }
    }
    }
    
//---------------------------------------------------------------------------------	
// Get the video attribute
//---------------------------------------------------------------------------------	  
void  CGlxUpnpRendererImpl::GetVideoL(const TGlxMedia& aMedia,TMPXAttribute& aAttrThumbnail)
    {
    TRACER("CGlxUpnpRendererImpl::GetVideoL");
    const CGlxMedia* item = aMedia.Properties();
    if(EMPXVideo == aMedia.Category())
    { 
    if (!iThumbnailSaver)
        {
        iThumbnailSaver=CGlxThumbnailSaver::NewL( this );
        }

    if ( ( aMedia.ThumbnailAttribute( aAttrThumbnail ) ) 
        &&  ( iPreviouslySentMediaId!=aMedia.Id( )) )
        { 
        iPreviouslySentMediaId=aMedia.Id();
        //get the thumbnail image
        const CGlxThumbnailAttribute* value = aMedia.ThumbnailAttribute( aAttrThumbnail );
        iThumbnailSaver->CreateVideoIconL( value->iBitmap );
        }
        else if ((GlxErrorManager::HasAttributeErrorL(item, KGlxMediaIdThumbnail) != KErrNone))
        {
          // send the default thumbnail
          iThumbnailSaver->CreateDefaultVideoIconL();
        }
    }
    }

//---------------------------------------------------------------------------------	
// DoFocusChanged callback
//---------------------------------------------------------------------------------	 
TInt CGlxUpnpRendererImpl::DoFocusChanged( TAny* aPtr )
    {
    TRACER("CGlxUpnpRendererImpl::DoFocusChanged");

    CGlxUpnpRendererImpl* self
                    = reinterpret_cast< CGlxUpnpRendererImpl* >( aPtr );
    TRAP_IGNORE( self->DoFocusChangedL() );
    
    return 0;
    }

//---------------------------------------------------------------------------------	
// DoFocusChangedL
//---------------------------------------------------------------------------------	 
void CGlxUpnpRendererImpl::DoFocusChangedL()
    {
    // SendFocusedItemL validates the index so no need to do it here too
    TInt index = iActiveMediaList->FocusIndex();
    SendFocusedItemL( index );
    }

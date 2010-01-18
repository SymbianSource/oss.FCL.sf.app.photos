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
 * Description:    Updates CPS storage for all collection
 *
 */

// INCLUDE FILES

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <liwservicehandler.h>

#include <glxuistd.h>
#include <glxicons.mbg>   // Glx Icons
#include <glxcollectionplugintags.hrh>
#include <glxthumbnailattributeinfo.h>  // for KGlxMediaIdThumbnail
#include <glxattributecontext.h>
#include <glxthumbnailcontext.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <mglxmedialist.h>       // for MGlxMediaList
#include <glxcollectiongeneraldefs.h>
#include <glxgallery.hrh>               // for KGlxGalleryApplicationUid
#include <glxmediaid.h>
#include <glxerrormanager.h>    

#include "glxcontentharvesterplugintags.h"
#include "glxcontentharvesterplugin.hrh"
#include "glxmapconstants.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginTags::CGlxContentHarvesterPluginTags( 
        MLiwInterface* aCPSInterface,
        MGlxContentHarvesterPlugin* aCHplugin )

    {
    TRACER( "CGlxContentHarvesterPluginTags::CGlxContentHarvesterPluginTags" );
    iCPSInterface = aCPSInterface;
    SetCHPlugin(aCHplugin);
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::ConstructL()
    {
    TRACER( "CGlxContentHarvesterPluginTags::ConstructL" );

    //Call the base class ConstructL to create default bitmap
    CGlxContentHarvesterPluginBase::ConstructL(EMbmGlxiconsQgn_prop_image_notcreated);

    iUriAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 
    iThumbnailAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator);

    //Register/Subscribe with matrix menu for the notifications 
    GetInterfaceForNotificationL();
    SetupPublisherL(KItemIndexTags);
    RequestCpsNotificationL(KItemIndexTags);
    HandleStateChangeL(KItemIndexTags);
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginTags* CGlxContentHarvesterPluginTags::NewLC( 
        MLiwInterface* aCPSInterface,
        MGlxContentHarvesterPlugin* aCHplugin )
    {
    TRACER( "CGlxContentHarvesterPluginTags::NewL" );
    CGlxContentHarvesterPluginTags* self = new ( ELeave )CGlxContentHarvesterPluginTags( aCPSInterface,aCHplugin );

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxContentHarvesterPluginTags::~CGlxContentHarvesterPluginTags()
    {
    TRACER( "CGlxContentHarvesterPluginTags::~CGlxContentHarvesterPluginTags" );

    DestroyMedialist();
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::UpdateDataL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::UpdateDataL() 
    {
    TRACER( "CGlxContentHarvesterPluginTags::UpdateDataL" );
    
    if (!iMediaList)
        {
        return;
        }

    if (iMediaList->Count())
        {
        TInt ret = UpdateItem(KPreviewItemIndex);
        if (ret != KErrNotFound)
            {
            //Updates the thumbnail in the collection 
            UpdateDataInCPSL(ret);
            }
        }
    else
        {
        // Show previous thumbnail until the new thumbnail is
        // fecthed.Added this check to avoid flicker
        if (!GetCHPlugin()->IsRefreshNeeded() || (iMediaList->Count() == 0))
            {
            //Don't Show the Thumbnail/Show nothing
            GLX_LOG_INFO("CGlxContentHarvesterPluginTags::UpdateDataL() --O");
            UpdateDataInCPSL(GetBitmapHandle());
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleNotifyL()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginTags::HandleNotifyL(
        TInt /* aCmdId*/,
        TInt/* aEventId */,
        CLiwGenericParamList& /*aEventParamList*/,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleNotifyL" );
    HandleStateChangeL(KItemIndexTags);
    return KErrNone;
    }


// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::UpdateDataInCPSL()
// ----------------------------------------------------------------------------
//  
void CGlxContentHarvesterPluginTags::UpdateDataInCPSL(TInt aHandle) 
    {
    TRACER( "CGlxContentHarvesterPluginTags::UpdateDataInCPSL" );
    //update data in CPS
    _LIT(KExamplePluginPub,"photossuite");
    _LIT(KContTypeText,"tags");
    _LIT(KContId1,"category1");

    if(iCPSInterface && iMediaList )
        {
        CLiwGenericParamList* inParamList = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outParamList = CLiwGenericParamList::NewLC();

        FillInputListWithDataL(inParamList, KExamplePluginPub, KContTypeText , 
                KContId1, aHandle);

        iCPSInterface->ExecuteCmdL( KAdd,  *inParamList, *outParamList );
        CleanupStack::PopAndDestroy(outParamList);
        CleanupStack::PopAndDestroy(inParamList);
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::UpdateItem()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginTags::UpdateItem(TInt aItemIndex)
    {
    TRACER( "CGlxContentHarvesterPluginTags::UpdateItem" );
    const TGlxMedia& item = iMediaList->Item(aItemIndex);
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );
    GLX_LOG_INFO1("CGlxContentHarvesterPluginTags::UpdateItem,aItemIndex=%d ",aItemIndex);
    const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
    if (value)
        {
        return value->iBitmap->Handle();
        }
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::ActivateL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::ActivateL( TBool aOn )
    {
    TRACER( "CGlxContentHarvesterPluginTags::ActivateL" );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginTags::ActivateL aOn =%d",aOn);
    SetFocus(aOn);

    if (IsMatrixMenuInForegroundL() && aOn )
        {
        if(GetCHPlugin()->IsRefreshNeeded())
            {
            //Update the rest of all the collections on receving the focus...
            GetCHPlugin()->UpdatePlugins(aOn);     
            }
        else
            {
            //As the collection is not updated by the contentharvester plugin
            //to update the thumbnails on the focus , need to call the below function
            UpdatePreviewThumbnailListL();
            }
        }
    else if ( !aOn )
        {
          if(!IsMatrixMenuInForegroundL())
            {
            //use case:Matrix Menu is exited, by entering into grid view,application view,capture mode...
            //Need to destroy all the collection's observers and context
            GLX_LOG_INFO1("CGlxContentHarvesterPluginTags::ActivateL !aOn =%d and matrix not in foreground",aOn);
            GetCHPlugin()->UpdatePlugins(aOn);    
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleItemAddedL
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleItemAddedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleMediaL(TInt /*aListIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleMediaL" );
    } 

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleItemRemovedL(TInt /*aStartIndex*/, 
        TInt /* aEndIndex */, MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleItemRemovedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleItemModifiedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleAttributesAvailableL(TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleAttributesAvailableL" );
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );

    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginTags::HandleAttributesAvailableL =%d ",aItemIndex);
    if (KErrNotFound != aAttributes.Find( thumbnailAttribute, match ))
        {
        const TGlxMedia& item = aList->Item( aItemIndex );
        const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
        if (value)
            {
            GLX_LOG_INFO("CGlxContentHarvesterPluginTags::HandleAttributesAvailableL Thumbnail is present ");

			// Update the preview thumbnail
			//remove the observer as client need not listen to the callbacks 
                GLX_LOG_INFO("CGlxContentHarvesterPluginTags::HandleAttributesAvailableL,one thumbnail fetched");
			UpdateDataInCPSL( value->iBitmap->Handle());           
			iMediaList->RemoveMediaListObserver( this );
            }//end of  check against value 
        }//end of  attribute match
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, 
        TInt /*aOldIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleFocusChangedL" );
    }


// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleItemSelectedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleMessageL(const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPluginTags::HandleMessageL" );
    }   

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::DestroyMedialist
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::DestroyMedialist()
    {
    TRACER( "CGlxContentHarvesterPluginTags::DestroyMedialist" );
    if( iMediaList )
        {
        GLX_LOG_INFO("CGlxContentHarvesterPluginTags::DestroyMedialist,media list deleted");
        RemoveContextAndObserver();
        
        delete iUriAttributeContext;
        iUriAttributeContext = NULL;
        delete iThumbnailAttributeContext;
        iThumbnailAttributeContext = NULL;
        
        iMediaList->Close();
        iMediaList = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::CreateMedialistL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::CreateMedialistL( )
    {
    TRACER( "CGlxContentHarvesterPluginTags::CreateMedialistL" );
    if(!iMediaList)
        {
            iThumbnailIterator.SetRange( KSinglePreviewThumbnail );
    
        iMediaList = CreateMedialistAndAttributeContextL( TGlxMediaId( 
        		KGlxTagCollectionPluginImplementationUid ),
        		iUriAttributeContext, iThumbnailAttributeContext); 
        
        AddContextAndObserverL();
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::UpdatePreviewThumbnailListL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::UpdatePreviewThumbnailListL( )
    {
    TRACER( "CGlxContentHarvesterPluginTags::UpdatePreviewThumbnailListL" );

    //when there is an update of content in the collection
    //this function is executed or when the collection recives the focus. 
    if(!iMediaList)
        {
        //media list is not created yet,create it.
        CreateMedialistL( );
        UpdateDataL();
        }
    else
        {        
        if(GetCHPlugin()->IsRefreshNeeded())
            {
            ContainerCacheCleanupL(iMediaList);
            }

	        //1.if the content is deleted,then creating a context doesn't fetch the attributes
	        //2.if the content is added and the content is not the latest as per the sorted order of the
	        // media list,then the thumbnails are not fetched.
	        // so show the first available thumbnail in the media list.

	        UpdateDataL();

	        //Adding the context doesn't gaurantee we get a call back for
	        //Handle attributes available,if the latest item is already fetched.
	        //and for the content added for this collection,if it is not latest
	        //we will not recieve the attributes .so show the first available thumbnail 
	        //in the media list.if there is any new latest content added,the thumbnail will be 
	        //fetched and shown.

	        iThumbnailIterator.SetRange( KSinglePreviewThumbnail );
	        RemoveContextAndObserver();
	        AddContextAndObserverL();
			}
	
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::AddContextAndObserverL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::AddContextAndObserverL()
    {
    TRACER( "CGlxContentHarvesterPluginTags::AddRemoveContextAndObserverL" );   
    if(iMediaList)
        {
        iMediaList->AddMediaListObserverL( this );
        iMediaList->AddContextL(iUriAttributeContext, KGlxFetchContextPriorityNormal);
        iMediaList->AddContextL(iThumbnailAttributeContext, KGlxFetchContextPriorityLow); 
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::RemoveContextAndObserver
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::RemoveContextAndObserver()
    {
    TRACER( "CGlxContentHarvesterPluginTags::RemoveContextAndObserver" );   
    if(iMediaList)
        {
        iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iUriAttributeContext);
        iMediaList->RemoveContext(iThumbnailAttributeContext); 
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::HandleError
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginTags::HandleError(TInt /*aError*/)
    {
    TRACER( "CGlxContentHarvesterPluginTags::HandleError" );    

#ifdef _DEBUG
    if(iMediaList)
        {
        TInt count=iMediaList->Count();
        GLX_LOG_INFO1("CGlxContentHarvesterPluginTags::HandleError,count=%d",count);
        for ( TInt i = 0; i < count ; i++ )
            {
            const TGlxMedia& item = iMediaList->Item( i );
            TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                    item.Properties(), KGlxMediaIdThumbnail );
            GLX_LOG_INFO1("CGlxContentHarvesterPluginTags::HandleError,Error=%d ",thumbnailError);

            }
        }
#endif
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginTags::Count
// ---------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginTags::Count()
    {
    TRACER( "CGlxContentHarvesterPluginTags::Count" );    
    TInt count = KErrNone;
    if(iMediaList)
        {
        count = iMediaList->Count();
        GLX_LOG_INFO1("GlxCHP:Tags::Count(%d)",count);
        }
    return count;
    }
//  End of File


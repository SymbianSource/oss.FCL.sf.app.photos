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
 * Description:    Updates CPS storage for albums collection
 *
 */

// INCLUDE FILES

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <liwservicehandler.h>

#include <glxuistd.h>
#include <glxicons.mbg>   // Glx Icons
#include <glxcollectionpluginalbums.hrh> //for  KGlxCollectionPluginAlbumsImplementationUid
#include <glxthumbnailattributeinfo.h>  // for KGlxMediaIdThumbnail
#include <glxattributecontext.h>
#include <glxthumbnailcontext.h>
#include <glxtracer.h>                //for tracer
#include <glxlog.h>                   // for log
#include <mglxmedialist.h>       // for MGlxMediaList
#include <glxcollectiongeneraldefs.h>
#include <glxgallery.hrh>               // for KGlxGalleryApplicationUid
#include <glxmediaid.h>
#include <glxerrormanager.h>      

#include "glxcontentharvesterpluginalbums.h"
#include "glxcontentharvesterplugin.hrh"
#include "glxmapconstants.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginAlbums::CGlxContentHarvesterPluginAlbums( 
        MLiwInterface* aCPSInterface,
        MGlxContentHarvesterPlugin* aCHplugin )
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::CGlxContentHarvesterPluginAlbums" );
    iCPSInterface = aCPSInterface;
    SetCHPlugin(aCHplugin);
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::ConstructL()
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::ConstructL" );
    
    //Call the base class ConstructL
    CGlxContentHarvesterPluginBase::ConstructL(EMbmGlxiconsQgn_prop_image_notcreated);
    iUriAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 
    iThumbnailAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 
    
    //Register/Subscribe with matrix menu for the notifications 
    GetInterfaceForNotificationL();
    SetupPublisherL(KItemIndexAlbums);
    RequestCpsNotificationL(KItemIndexAlbums);
    HandleStateChangeL(KItemIndexAlbums);
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginAlbums* CGlxContentHarvesterPluginAlbums::NewLC( 
        MLiwInterface* aCPSInterface,
        MGlxContentHarvesterPlugin* aCHplugin )
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::NewL" );
    CGlxContentHarvesterPluginAlbums* self = new ( ELeave ) CGlxContentHarvesterPluginAlbums( aCPSInterface,aCHplugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxContentHarvesterPluginAlbums::~CGlxContentHarvesterPluginAlbums()
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::~CGlxContentHarvesterPluginAlbums" );

    DestroyMedialist();
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::UpdateDataL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::UpdateDataL() 
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::UpdateDataL" );

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
            GLX_LOG_INFO("CGlxContentHarvesterPluginAlbums::UpdateDataL() --O");
            UpdateDataInCPSL(GetBitmapHandle());
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleNotifyL()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginAlbums::HandleNotifyL(
        TInt /* aCmdId*/,
        TInt/* aEventId */,
        CLiwGenericParamList& /*aEventParamList*/,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleNotifyL" );
    HandleStateChangeL(KItemIndexAlbums);
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::UpdateDataInCPSL()
// ----------------------------------------------------------------------------
// 
void CGlxContentHarvesterPluginAlbums::UpdateDataInCPSL(TInt aHandle) 
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::UpdateDataInCPSL" );
    //update data in CPS
    _LIT(KExamplePluginPub,"photossuite");
    _LIT(KContTypeText,"albums");
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
// CGlxContentHarvesterPluginAlbums::UpdateItem()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginAlbums::UpdateItem(TInt aItemIndex)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::UpdateItem" );
    const TGlxMedia& item = iMediaList->Item(aItemIndex);
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );
    GLX_LOG_INFO1("CGlxContentHarvesterPluginAlbums::UpdateItem,aItemIndex=%d ",aItemIndex);
    const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
    if (value)
        {
        return value->iBitmap->Handle();
        }
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::ActivateL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::ActivateL( TBool aOn )
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::ActivateL" );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginAlbums::ActivateL aOn =%d",aOn);
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
            GLX_LOG_INFO1("CGlxContentHarvesterPluginAlbums::ActivateL !aOn =%d and matrix not in foreground",aOn);
            GetCHPlugin()->UpdatePlugins(aOn);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleItemAddedL
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleItemAddedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleMediaL(TInt /*aListIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleMediaL" );
    } 

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleItemRemovedL(TInt /*aStartIndex*/, 
        TInt /* aEndIndex */, MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleItemRemovedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleItemModifiedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleAttributesAvailableL(TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleAttributesAvailableL" );
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );

    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginAlbums::HandleAttributesAvailableL =%d ",aItemIndex);
    
    if (KErrNotFound != aAttributes.Find( thumbnailAttribute, match ))
        {
        const TGlxMedia& item = aList->Item( aItemIndex );
        const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
        if (value)
            {
            GLX_LOG_INFO("CGlxContentHarvesterPluginAlbums::HandleAttributesAvailableL Thumbnail is present ");

			// Update the preview thumbnail
			//remove the observer as client need not listen to the callbacks
			GLX_LOG_INFO("CGlxContentHarvesterPluginAll::HandleAttributesAvailableL,one thumbnail fetched");
			UpdateDataInCPSL( value->iBitmap->Handle());
			iMediaList->RemoveMediaListObserver( this );
            }//end of  check against value 
        }//end of  attribute match
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, 
        TInt /*aOldIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleFocusChangedL" );
    }


// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleItemSelectedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleMessageL(const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleMessageL" );
    }   

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::DestroyMedialist
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::DestroyMedialist()
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::DestroyMedialist" );
    if( iMediaList )
        {
        GLX_LOG_INFO("CGlxContentHarvesterPluginAlbums::DestroyMedialist,media list deleted");
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
// CGlxContentHarvesterPluginAlbums::CreateMedialistL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::CreateMedialistL( )
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::CreateMedialistL" );
    if(!iMediaList)
        {
		iThumbnailIterator.SetRange( KSinglePreviewThumbnail );

        iMediaList = CreateMedialistAndAttributeContextL( TGlxMediaId( 
                        KGlxCollectionPluginAlbumsImplementationUid ),
                        iUriAttributeContext,iThumbnailAttributeContext);         
        
        AddContextAndObserverL();
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::UpdatePreviewThumbnailListL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::UpdatePreviewThumbnailListL( )
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::UpdatePreviewThumbnailListL" );

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
// CGlxContentHarvesterPluginAlbums::AddContextAndObserverL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::AddContextAndObserverL()
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::AddRemoveContextAndObserverL" );   
    if(iMediaList)
        {
        iMediaList->AddMediaListObserverL( this );
        iMediaList->AddContextL(iUriAttributeContext, KGlxFetchContextPriorityNormal);
        iMediaList->AddContextL(iThumbnailAttributeContext, KGlxFetchContextPriorityLow);        
        }
    }

// ---------------------------------------------------------------------------c
// CGlxContentHarvesterPluginAlbums::RemoveContextAndObserver
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::RemoveContextAndObserver()
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::RemoveContextAndObserver" );   
    if(iMediaList)
        {
        iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iUriAttributeContext);
        iMediaList->RemoveContext(iThumbnailAttributeContext);        
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::HandleError
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAlbums::HandleError(TInt /*aError*/)
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::HandleError" );    

#ifdef _DEBUG
    if(iMediaList)
        {
        TInt count=iMediaList->Count();
        GLX_LOG_INFO1("CGlxContentHarvesterPluginAlbums::HandleError,count=%d",count);
        for ( TInt i = 0; i < count ; i++ )
            {
            const TGlxMedia& item = iMediaList->Item( i );
            TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                    item.Properties(), KGlxMediaIdThumbnail );
            GLX_LOG_INFO1("CGlxContentHarvesterPluginAlbums::HandleError,Error=%d ",thumbnailError);

            }
        }
#endif
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::Count
// ---------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginAlbums::Count()
    {
    TRACER( "CGlxContentHarvesterPluginAlbums::Count" );    
    TInt count = KErrNone;
    if(iMediaList)
        {
        count = iMediaList->Count();
        GLX_LOG_INFO1("GlxCHP:Albums::Count(%d)",count);
        }
    return count;
    }
//  End of File


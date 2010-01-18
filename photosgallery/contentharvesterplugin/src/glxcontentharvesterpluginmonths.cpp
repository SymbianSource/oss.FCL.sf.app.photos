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
 * Description:    Updates CPS storage for months collection
 *
 */

// INCLUDE FILES

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <liwservicehandler.h>

#include <glxuistd.h>
#include <glxicons.mbg>   // Glx Icons
#include <glxcollectionpluginmonths.hrh>
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

#include "glxcontentharvesterpluginmonths.h"
#include "glxcontentharvesterplugin.hrh"
#include "glxmapconstants.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginMonths::CGlxContentHarvesterPluginMonths( 
        MLiwInterface* aCPSInterface,
        MGlxContentHarvesterPlugin *aCHplugin )

    {
    TRACER( "CGlxContentHarvesterPluginMonths::CGlxContentHarvesterPluginMonths" );
    iCPSInterface = aCPSInterface;
    SetCHPlugin(aCHplugin);
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::ConstructL()
    {
    TRACER( "CGlxContentHarvesterPluginMonths::ConstructL" );

    //Call the base class ConstructL to create default bitmap
    CGlxContentHarvesterPluginBase::ConstructL(EMbmGlxiconsQgn_prop_image_notcreated);

    iUriAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 
    iThumbnailAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 

    //Register/Subscribe with matrix menu for the notifications 
    GetInterfaceForNotificationL();
    SetupPublisherL(KItemIndexMonths);
    RequestCpsNotificationL(KItemIndexMonths);
    HandleStateChangeL(KItemIndexMonths);
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginMonths* CGlxContentHarvesterPluginMonths::NewLC( 
        MLiwInterface* aCPSInterface,
        MGlxContentHarvesterPlugin *aCHplugin )
    {
    TRACER( "CGlxContentHarvesterPluginMonths::NewL" );
    CGlxContentHarvesterPluginMonths* self = new ( ELeave ) CGlxContentHarvesterPluginMonths( aCPSInterface,aCHplugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxContentHarvesterPluginMonths::~CGlxContentHarvesterPluginMonths()
    {
    TRACER( "CGlxContentHarvesterPluginMonths::~CGlxContentHarvesterPluginMonths" );
    DestroyMedialist();
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::UpdateDataL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::UpdateDataL() 
    {
    TRACER( "CGlxContentHarvesterPluginMonths::UpdateDataL" );
    
    if (!iMediaList)
        {
        return;
        }
    
    if (iMediaList->Count())
        {
        TInt ret = UpdateItem(KPreviewItemIndex);
        if(ret != KErrNotFound)
            {
            //Updates the thumbnail in the collection 
            UpdateDataInCPSL(ret);
            }
        }
    else
        {
        // Show previous thumbnail until the new thumbnail is
        // fecthed.Added this check to avoid flicker
        if(!GetCHPlugin()->IsRefreshNeeded() || (iMediaList->Count() == 0))
        	{
        	//Don't Show the Thumbnail/Show nothing
        	GLX_LOG_INFO("CGlxContentHarvesterPluginMonths::UpdateDataL() --O");
        	UpdateDataInCPSL(GetBitmapHandle());
        	}
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleNotifyL()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginMonths::HandleNotifyL(
        TInt /* aCmdId*/,
        TInt/* aEventId */,
        CLiwGenericParamList& /*aEventParamList*/,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleNotifyL" );
    HandleStateChangeL(KItemIndexMonths);
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::UpdateDataInCPSL()
// ----------------------------------------------------------------------------
// 
void CGlxContentHarvesterPluginMonths::UpdateDataInCPSL(TInt aHandle) 
    {
    TRACER( "CGlxContentHarvesterPluginMonths::UpdateDataInCPSL" );
    //update data in CPS
    _LIT(KExamplePluginPub,"photossuite");
    _LIT(KContTypeText,"months");
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
// CGlxContentHarvesterPluginMonths::UpdateItem()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginMonths::UpdateItem(TInt aItemIndex)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::UpdateItem" );
    const TGlxMedia& item = iMediaList->Item(aItemIndex);
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );
    GLX_LOG_INFO1("CGlxContentHarvesterPluginMonths::UpdateItem,aItemIndex=%d ",aItemIndex);
    const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
    if (value)
        {
        return value->iBitmap->Handle();
        }
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::ActivateL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::ActivateL( TBool aOn )
    {
    TRACER( "CGlxContentHarvesterPluginMonths::ActivateL" );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginMonths::ActivateL aOn =%d",aOn);
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
            GLX_LOG_INFO1("CGlxContentHarvesterPluginMonths::ActivateL !aOn =%d and matrix not in foreground",aOn);
            GetCHPlugin()->UpdatePlugins(aOn);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleItemAddedL
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleItemAddedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleMediaL(TInt /*aListIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleMediaL" );
    } 

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleItemRemovedL(TInt /*aStartIndex*/, 
        TInt /* aEndIndex */, MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleItemRemovedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleItemModifiedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleAttributesAvailableL(TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleAttributesAvailableL" );
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );

    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginMonths::HandleAttributesAvailableL =%d ",aItemIndex);
    if (KErrNotFound != aAttributes.Find( thumbnailAttribute, match ))
        {
        const TGlxMedia& item = aList->Item( aItemIndex );
        const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
        if (value)
            {
            GLX_LOG_INFO("CGlxContentHarvesterPluginMonths::HandleAttributesAvailableL Thumbnail is present ");

			// Update the preview thumbnail
			//remove the observer as client need not listen to the callbacks 
                GLX_LOG_INFO("CGlxContentHarvesterPluginMonths::HandleAttributesAvailableL,one thumbnail fetched");
			UpdateDataInCPSL( value->iBitmap->Handle());           
			iMediaList->RemoveMediaListObserver( this );
            }//end of  check against value 
        }//end of  attribute match
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, 
        TInt /*aOldIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleFocusChangedL" );
    }


// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleItemSelectedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleMessageL(const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPluginMonths::HandleMessageL" );
    }   

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::DestroyMedialist
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::DestroyMedialist()
    {
    TRACER( "CGlxContentHarvesterPluginMonths::DestroyMedialist" );
    if( iMediaList )
        {
        GLX_LOG_INFO("CGlxContentHarvesterPluginMonths::DestroyMedialist,media list deleted");
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
// CGlxContentHarvesterPluginMonths::CreateMedialistL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::CreateMedialistL( )
    {
    TRACER( "CGlxContentHarvesterPluginMonths::CreateMedialistL" );
    if(!iMediaList)
        {
		iThumbnailIterator.SetRange( KSinglePreviewThumbnail );

        iMediaList = CreateMedialistAndAttributeContextL( TGlxMediaId( 
                KGlxCollectionPluginMonthsImplementationUid ), 
                iUriAttributeContext,iThumbnailAttributeContext);
        AddContextAndObserverL();
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::UpdatePreviewThumbnailListL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::UpdatePreviewThumbnailListL( )
    {
    TRACER( "CGlxContentHarvesterPluginMonths::UpdatePreviewThumbnailListL" );

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
void CGlxContentHarvesterPluginMonths::AddContextAndObserverL()
    {
    TRACER( "CGlxContentHarvesterPluginMonths::AddRemoveContextAndObserverL" );   
    if(iMediaList)
        {
        iMediaList->AddMediaListObserverL( this );
        iMediaList->AddContextL(iUriAttributeContext, KGlxFetchContextPriorityNormal);
        iMediaList->AddContextL(iThumbnailAttributeContext, KGlxFetchContextPriorityLow);  
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAlbums::RemoveContextAndObserver
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::RemoveContextAndObserver()
    {
    TRACER( "CGlxContentHarvesterPluginMonths::RemoveContextAndObserver" );   
    if(iMediaList)
        {
        iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iUriAttributeContext);
        iMediaList->RemoveContext(iThumbnailAttributeContext);     
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::HandleError
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginMonths::HandleError(TInt /*aError*/)
    {
    TRACER( "CGlxContentHarvesterPluginMonths::HandleError" );    

#ifdef _DEBUG
    if(iMediaList)
        {
        TInt count=iMediaList->Count();
        GLX_LOG_INFO1("CGlxContentHarvesterPluginMonths::HandleError,count=%d",count);
        for ( TInt i = 0; i < count ; i++ )
            {
            const TGlxMedia& item = iMediaList->Item( i );
            TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                    item.Properties(), KGlxMediaIdThumbnail );
            GLX_LOG_INFO1("CGlxContentHarvesterPluginMonths::HandleError,Error=%d ",thumbnailError);

            }
        }
#endif
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginMonths::Count
// ---------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginMonths::Count()
    {
    TRACER( "CGlxContentHarvesterPluginMonths::Count" );    
    TInt count = KErrNone;
    if(iMediaList)
        {
        count = iMediaList->Count();
        GLX_LOG_INFO1("GlxCHP:Months::Count(%d)",count);
        }
    return count;
    }
//  End of File


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
#include <glxthumbnailattributeinfo.h>   // for KGlxMediaIdThumbnail
#include <glxattributecontext.h>
#include <glxthumbnailcontext.h>
#include <glxcollectionpluginall.hrh>   //for  KGlxCollectionPluginAllImplementationUid
#include <glxtracer.h>                 //for  Traces 
#include <glxlog.h>                   //for Log
#include <mglxmedialist.h>           // for MGlxMediaList
#include <glxcollectiongeneraldefs.h>
#include <glxgallery.hrh>               // for KGlxGalleryApplicationUid
#include <glxmediaid.h>
#include <glxerrormanager.h>      

#include "glxcontentharvesterplugin.hrh"
#include "glxcontentharvesterpluginall.h"

#include "glxmapconstants.h"


// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginAll::CGlxContentHarvesterPluginAll(
        MLiwInterface* aCPSInterface,
        MGlxContentHarvesterPlugin *aCHplugin )
    {
    TRACER( "CGlxContentHarvesterPluginAll::CGlxContentHarvesterPluginAll" );
    iCPSInterface = aCPSInterface;
    SetCHPlugin(aCHplugin);
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::ConstructL()
    {
    TRACER( "CGlxContentHarvesterPluginAll::ConstructL" );
    
    //Call the base class ConstructL,to create the default thumbnail
    CGlxContentHarvesterPluginBase::ConstructL(EMbmGlxiconsQgn_prop_image_notcreated);

    iUriAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 
    iThumbnailAttributeContext = new (ELeave) CGlxAttributeContext(&iThumbnailIterator); 

    //Register/Subscribe with matrix menu for the notifications 
    GetInterfaceForNotificationL();
    SetupPublisherL(KItemIndexAll);
    RequestCpsNotificationL(KItemIndexAll);
    HandleStateChangeL(KItemIndexAll);
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPluginAll* CGlxContentHarvesterPluginAll::NewLC( 
        MLiwInterface* aCPSInterface,                                                                                               
        MGlxContentHarvesterPlugin *aCHplugin )
    {
    TRACER( "CGlxContentHarvesterPluginAll::NewL" );
    CGlxContentHarvesterPluginAll* self = new ( ELeave ) CGlxContentHarvesterPluginAll( aCPSInterface,aCHplugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxContentHarvesterPluginAll::~CGlxContentHarvesterPluginAll()
    {
    TRACER( "CGlxContentHarvesterPluginAll::~CGlxContentHarvesterPluginAll" );

    DestroyMedialist();
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::UpdateDataL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::UpdateDataL() 
    {
    TRACER( "CGlxContentHarvesterPluginAll::UpdateDataL" );

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
        if (iMediaList->Count() == 0)
            {
            //Don't Show the Thumbnail/Show nothing
            GLX_LOG_INFO("CGlxContentHarvesterPluginAll::UpdateDataL() --O");
            UpdateDataInCPSL(GetBitmapHandle());
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleNotifyL()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginAll::HandleNotifyL(
        TInt /* aCmdId*/,
        TInt/* aEventId */,
        CLiwGenericParamList& /*aEventParamList*/,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleNotifyL" );
    
    // First time when we enter matrix menu from shortcut or app. launcher, set
	// IsRefreshNeeded flag to ETrue. so that thumbnail attributes are fetched again
	// for all containers. This is to avoid preview thumbnails not getting shown in
	// matirix menu under low memory conditions.
	GetCHPlugin()->SetRefreshNeeded(ETrue);

    HandleStateChangeL(KItemIndexAll);
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::UpdateDataInCPSL()
// ----------------------------------------------------------------------------
//  
void CGlxContentHarvesterPluginAll::UpdateDataInCPSL(TInt aHandle) 
    {
    TRACER( "CGlxContentHarvesterPluginAll::UpdateDataInCPSL" );
    //update data in CPS
    _LIT(KExamplePluginPub,"photossuite");
    _LIT(KContentActivation,"allcs");
    _LIT(KContId1,"category1");

    if(iCPSInterface && iMediaList )
        {
        CLiwGenericParamList* inParamList = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outParamList = CLiwGenericParamList::NewLC();

        //Shows the thumbnail in the matrix view
        FillInputListWithDataL(inParamList, KExamplePluginPub, KContentActivation , 
                KContId1, aHandle);

        iCPSInterface->ExecuteCmdL( KAdd,  *inParamList, *outParamList );
        CleanupStack::PopAndDestroy(outParamList);
        CleanupStack::PopAndDestroy(inParamList);
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::UpdateItem()
// ----------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginAll::UpdateItem(TInt aItemIndex)
    {
    TRACER( "CGlxContentHarvesterPluginAll::UpdateItem" );
    const TGlxMedia& item = iMediaList->Item(aItemIndex);
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );
    GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::UpdateItem,aItemIndex=%d ",aItemIndex);
    const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
    if (value)
        {
        return value->iBitmap->Handle();
        }
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::ActivateL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::ActivateL( TBool aOn )
    {
    TRACER( "CGlxContentHarvesterPluginAll::ActivateL" );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::ActivateL aOn =%d",aOn);
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
            GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::ActivateL !aOn =%d and matrix not in foreground",aOn);
            GetCHPlugin()->UpdatePlugins(aOn);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleItemAddedL
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList*/*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleItemAddedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleMediaL(TInt /*aListIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleMediaL" );
    } 

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleItemRemovedL(TInt /*aStartIndex*/, 
        TInt /* aEndIndex */, MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleItemRemovedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleItemModifiedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleAttributesAvailableL(TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleAttributesAvailableL" );
    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );

    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );

    GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::HandleAttributesAvailableL =%d ",aItemIndex);
    if (KErrNotFound != aAttributes.Find( thumbnailAttribute, match ))
        {
        const TGlxMedia& item = aList->Item( aItemIndex );
        const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
        if (value)
            {
            GLX_LOG_INFO("CGlxContentHarvesterPluginAll::HandleAttributesAvailableL Thumbnail is present ");

			// Update the preview thumbnail
			//remove the observer as client need not listen to the callbacks
			GLX_LOG_INFO("CGlxContentHarvesterPluginAll::HandleAttributesAvailableL,one thumbnail fetched");
			UpdateDataInCPSL( value->iBitmap->Handle());
			iMediaList->RemoveMediaListObserver( this );
            }//end of  check against value 
        }//end of  attribute match
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, 
        TInt /*aOldIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleFocusChangedL" );
    }


// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleItemSelectedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleMessageL(const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPluginAll::HandleMessageL" );
    }   

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::DestroyMedialistL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::DestroyMedialist()
    {
    TRACER( "CGlxContentHarvesterPluginAll::DestroyMedialist" );
    if( iMediaList )
        {
        GLX_LOG_INFO("CGlxContentHarvesterPluginAll::DestroyMedialist,media list deleted");
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
// CGlxContentHarvesterPluginAll::CreateSingleItemMedialist
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::CreateMedialistL( )
    {
    TRACER( "CGlxContentHarvesterPluginAll::CreateMedialistL" );
    if(!iMediaList)
        {
        iThumbnailIterator.SetRange( KSinglePreviewThumbnail );

        iMediaList = CreateMedialistAndAttributeContextL( TGlxMediaId( 
                KGlxCollectionPluginAllImplementationUid ),
                iUriAttributeContext,iThumbnailAttributeContext);         

        AddContextAndObserverL();
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::UpdateMedialistContext
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::UpdatePreviewThumbnailListL( )
    {
    TRACER( "CGlxContentHarvesterPluginAll::UpdatePreviewThumbnailListL" );

    //when there is an update of content in the collection
    //this function is executed or when the collection recives the focus. 
    if(!iMediaList)
        {
        //medis list is not created yet,create it.
        CreateMedialistL( );
        UpdateDataL();
        }
    else
        {
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
// CGlxContentHarvesterPluginAll::AddContextAndObserverL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::AddContextAndObserverL()
    {
    TRACER( "CGlxContentHarvesterPluginAll::AddRemoveContextAndObserverL" );   
    if(iMediaList)
        {
        iMediaList->AddMediaListObserverL( this );
        iMediaList->AddContextL(iUriAttributeContext, KGlxFetchContextPriorityNormal);
        iMediaList->AddContextL(iThumbnailAttributeContext, KGlxFetchContextPriorityLow);   
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::RemoveContextAndObserver
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::RemoveContextAndObserver()
    {
    TRACER( "CGlxContentHarvesterPluginAll::RemoveContextAndObserver" );   
    if(iMediaList)
        {
        iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iUriAttributeContext);
        iMediaList->RemoveContext(iThumbnailAttributeContext);
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::HandleError
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleError(TInt /*aError*/)
    {
    TRACER( "CGlxContentHarvesterPluginAll::HandleError" );    

#ifdef _DEBUG
    if(iMediaList)
        {
        TInt count=iMediaList->Count();
        GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::HandleError,count=%d",count);
        for ( TInt i = 0; i < count ; i++ )
            {
            const TGlxMedia& item = iMediaList->Item( i );
            TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                    item.Properties(), KGlxMediaIdThumbnail );
            GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::HandleError,Error=%d ",thumbnailError);

            }
        }
#endif
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::Count
// ---------------------------------------------------------------------------
//
TInt CGlxContentHarvesterPluginAll::Count()
    {
    TRACER( "CGlxContentHarvesterPluginAll::Count" );    
    TInt count = KErrNone;
    if(iMediaList)
        {
        count = iMediaList->Count();
        GLX_LOG_INFO1("GlxCHP:All::Count(%d)",count);
        }
    return count;
    }
//  End of File


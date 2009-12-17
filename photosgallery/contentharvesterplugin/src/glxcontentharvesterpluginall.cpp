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


// ============================ MEMBER FUNCTIONS ==============================
LOCAL_C TInt TimerCallbackL( TAny* aPtr )
    {
    TRACER( "CGlxContentHarvesterPluginAll::TimerCallbackL" );
    static_cast<CGlxContentHarvesterPluginAll*>(aPtr)->UpdateDataL();
    return KErrNone;
    }

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
    
    iPeriodic = CPeriodic::NewL( CActive::EPriorityLow );
    iThumbnailContext = CGlxThumbnailContext::NewL(&iThumbnailIterator);
    
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

    if ( iPeriodic )
        {
        iPeriodic->Cancel();
        }
    delete iPeriodic;
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPluginAll::UpdateDataL()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::UpdateDataL() 
    {
    TRACER( "CGlxContentHarvesterPluginAll::UpdateDataL" );
    if(iMediaList && iMediaList->Count() && iPreviewItemCount.Count() )
        {
        GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::UpdateDataL(),iProgressIndex=%d",iProgressIndex);
        TInt ret = UpdateItem(iPreviewItemCount[iProgressIndex]);
        if(ret != KErrNotFound)
            {
            //Updates the thumbnail in the collection 
            UpdateDataInCPSL(ret);
            }
        else
            {
            UpdateDataInCPSL(GetBitmapHandle());
            }
        }
    else
        {
        // Show previous thumbnail until the new thumbnail is
        // fecthed.Added this check to avoid flicker
		if(iMediaList->Count() == 0)
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
// CGlxContentHarvesterPluginAll::HandleItemChanged()
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPluginAll::HandleItemChanged()
    {
    TRACER("CGlxContentHarvesterPluginAll::HandleItemChanged");

    iProgressIndex = 0;
    iPreviewItemCount.Reset();

    TSize gridIconSize = GetGridIconSize();
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,  gridIconSize.iWidth, gridIconSize.iHeight ) );
    if(iMediaList)  
        {
        TInt count = iMediaList->Count();
        GLX_LOG_INFO1("CGlxContentHarvesterPluginAll:: HandleItemChanged ,count=%d",count);

        TBool inFocus = IsFocused();
        for(TInt aItemIndex = 0; aItemIndex < count; aItemIndex++)
            {
            const TGlxMedia& item = iMediaList->Item( aItemIndex );
            const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
            if (value)
                {
                iPreviewItemCount.InsertInOrder(aItemIndex);
                if(!inFocus)
                    {
                    //if the collection is NOT in Focus,retrive only one thumbnail and break
                    GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::HandleItemChanged,Range=1,aItemIndex=%d",aItemIndex);
                    break;
                    }
                else if(iPreviewItemCount.Count() == KPreviewThumbnailFetchCount ||
                        iPreviewItemCount.Count() == count )
                    {
                    //if the collection is not in Focus,retrive 15 thumbnail and break
                    GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::HandleItemChanged,Range=15,aItemIndex=%d",aItemIndex);
                    break;
                    }

                }
            }
        }
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
        GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::UpdateItem,iProgressIndex=%d ",iProgressIndex);
        iProgressIndex++;
        if (iProgressIndex >= KPreviewThumbnailFetchCount || 
                iProgressIndex >= iPreviewItemCount.Count() ||
                iProgressIndex >= iMediaList->Count())
            {
            iProgressIndex = 0;
            }
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

    if (IsMatrixMenuInForegroundL() && aOn && !iPeriodic->IsActive() )
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

        iPeriodic->Start( KTimerInterval, 
                KTimerInterval, 
                TCallBack( TimerCallbackL, this ) );
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
        iPeriodic->Cancel();
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
            iPreviewItemCount.InsertInOrder(aItemIndex);

            //if the collection on the matrix menu is not focused,then show only one thumbnail
            if(!IsFocused())
                {
                //if one thumbnail is fetched,it is sufficent when the collection is not in focus.
                //remove the observer as client need not listen to the callbacks 
                GLX_LOG_INFO("CGlxContentHarvesterPluginAll::HandleAttributesAvailableL,one thumbnail fetched");
                UpdateDataInCPSL( value->iBitmap->Handle());           
                iMediaList->RemoveMediaListObserver( this );
                }
            else if (iPreviewItemCount.Count()  == KPreviewThumbnailFetchCount ||
                    iPreviewItemCount.Count() == aList->Count() )
                {
                //if the PreviewItemCount  equals 15 or if it is eqaul to the total count
                //remove the observer as client need not listen to the callbacks 
                GLX_LOG_INFO1("CGlxContentHarvesterPluginAll::HandleAttributesAvailableL,media list count=%d",aList->Count());
                iMediaList->RemoveMediaListObserver( this );
                }
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
        delete iThumbnailContext;
        iThumbnailContext = NULL;
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
        //if the collection is in focus then , create media list with context of 15 items else
        // with context of single item.
        if(IsFocused())
            {
            iThumbnailIterator.SetRange( KPreviewThumbnailFetchCount ); 
            }
        else
            {
            iThumbnailIterator.SetRange( KSinglePreviewThumbnail );
            }

        iMediaList = CreateMedialistAndThumbnailContextL( TGlxMediaId( 
                KGlxCollectionPluginAllImplementationUid ),iThumbnailContext);
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
        //This is called to show the preview thumbnails. If no thumbnails are
        //present, display nothing 
        UpdateDataL();
        }
    else if(IsFocused())
        {
        //1.This loop is executed,when the collection gets focus
        //2.This loop is executed,when the contents are added/deleted for this collection
        //and this collection has focus,so 15 thumbnails are fetched.
        //say for example
        //1.focus is on "All" collection/view and you insert MMC
        //2.capture in burst mode and return quickly to photos suite and focus on "All"
        HandleItemChanged();
        iThumbnailIterator.SetRange( KPreviewThumbnailFetchCount );
        RemoveContextAndObserver();
        AddContextAndObserverL();
        }
    else
        {
        //1.This loop is executed,when the contents are updated for this collection
        //and this collection doesn't have the focus,so only one thumbnail is fetched.

        //here we need to fetch only one item 
        //1.if the content is deleted,then creating a context doesn't fetch the attributes
        //2.if the content is added and the content is not the latest as per the sorted order of the
        // media list,then the thumbnails are not fetched.
        // so show the first available thumbnail in the media list.

        HandleItemChanged();
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
        iMediaList->AddContextL(iThumbnailContext, KGlxFetchContextPriorityNormal);
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
        iMediaList->RemoveContext(iThumbnailContext);
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


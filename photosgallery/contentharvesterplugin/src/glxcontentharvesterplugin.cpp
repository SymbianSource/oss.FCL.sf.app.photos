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
 * Description:    Updates CPS storage
 *
 */

// INCLUDE FILES

//photo headers


#include <glxattributecontext.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <mglxmedialist.h>       // for MGlxMediaList
#include <glxcollectiongeneraldefs.h>
#include <glxgallery.hrh>               // for KGlxGalleryApplicationUid
#include <mpxcollectionutility.h>
#include <glxcollectionpluginmonths.hrh>
#include <glxuistd.h>

#include <MtpPrivatePSKeys.h>
#include <e32property.h>

//local headers
#include "glxcontentharvesterplugin.h"
#include "glxcontentharvesterplugin.hrh"
#include "glxmapconstants.h"

#include "glxcontentharvesterpluginmonths.h"
#include "glxcontentharvesterpluginalbums.h"
#include "glxcontentharvesterplugintags.h"
#include "glxcontentharvesterpluginall.h"





// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPlugin::CGlxContentHarvesterPlugin( MLiwInterface* aCPSInterface )
    {
    TRACER( "CGlxContentHarvesterPlugin::CGlxContentHarvesterPlugin" );
    iCPSInterface = aCPSInterface;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::ConstructL()
    {
    TRACER( "CGlxContentHarvesterPlugin::ConstructL" );

    CreateMedialistL();

	// Bug Fix @ ESLM-7VWF28 :: " Downloads " collection folder is replaced by " ALL " collection folder    
    CGlxContentHarvesterPluginAll* allContents = CGlxContentHarvesterPluginAll::NewLC(iCPSInterface,this);
    iEntries.AppendL(allContents);
    CleanupStack::Pop(allContents); 

    CGlxContentHarvesterPluginMonths* monthsContents = CGlxContentHarvesterPluginMonths::NewLC(iCPSInterface,this);
    iEntries.AppendL(monthsContents);
    CleanupStack::Pop(monthsContents); 

    CGlxContentHarvesterPluginAlbums* albumContents = CGlxContentHarvesterPluginAlbums::NewLC(iCPSInterface,this);
    iEntries.AppendL(albumContents);
    CleanupStack::Pop(albumContents);  

    CGlxContentHarvesterPluginTags* tagContents = CGlxContentHarvesterPluginTags::NewLC(iCPSInterface,this);
    iEntries.AppendL(tagContents);
    CleanupStack::Pop(tagContents);
    for(TInt pluginIndex=0; pluginIndex < KTotalCollection; pluginIndex++)
        {
        iCollectionIndexes.Insert(KErrNotFound, pluginIndex);
        }
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxContentHarvesterPlugin* CGlxContentHarvesterPlugin::NewL(  MLiwInterface* aCPSInterface )
    {
    TRACER( "CGlxContentHarvesterPlugin::NewL" );
    CGlxContentHarvesterPlugin* self = new ( ELeave ) CGlxContentHarvesterPlugin( aCPSInterface );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxContentHarvesterPlugin::~CGlxContentHarvesterPlugin()
    {
    TRACER( "CGlxContentHarvesterPlugin::~CGlxContentHarvesterPlugin" );
    // destroy entries
    iEntries.ResetAndDestroy();
    DestroyMedialist();
    iCollectionIndexes.Reset();
    }

// ----------------------------------------------------------------------------
// This function updates information in CPS storage
// ----------------------------------------------------------------------------
//	
void CGlxContentHarvesterPlugin::UpdateL() 
    {
    TRACER( "CGlxContentHarvesterPlugin::UpdateL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::CollectionPathLC( )
// ---------------------------------------------------------------------------
//
CMPXCollectionPath* CGlxContentHarvesterPlugin::CollectionPathLC( 
        const TGlxMediaId& aPluginId ) const
        {
        TRACER( "CGlxContentHarvesterPlugin::CollectionPathLC" );
        // create the collection path - uses the category ID from the constructor    
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL( path );
        path->AppendL( aPluginId.Value() );               
        return path;
        }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleItemAddedL
// ----------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList*/* aList*/)
    {
    // Do Nothing
    TRACER( "CGlxContentHarvesterPlugin::HandleItemAddedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleMediaL(TInt /*aListIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPlugin::HandleMediaL" );
    } 

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleItemRemovedL(TInt /*aStartIndex*/, 
        TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPlugin::HandleItemRemovedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleItemModifiedL(
        const RArray<TInt>& aItemIndexes, 
        MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPlugin::HandleItemModifiedL" );
    TInt countOfIndexesChanged = aItemIndexes.Count();
    for (TInt i = 0; i < countOfIndexesChanged ; i++ )
        {
        UpdateDataInCPSL(i);
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleAttributesAvailableL(TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/)
    {
    TRACER( "CGlxContentHarvesterPlugin::HandleAttributesAvailableL" );
    TMPXAttribute subTitleAttrib(KGlxMediaCollectionPluginSpecificSubTitle);
    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
    GLX_LOG_INFO1("CGlxContentHarvesterPlugin::HandleAttributesAvailableL,aItemIndex=%d",aItemIndex);
    if (KErrNotFound != aAttributes.Find(subTitleAttrib, match))
        {
        GLX_LOG_INFO("CGlxContentHarvesterPlugin::HandleAttributesAvailableL,MATCH Found");
        if (aItemIndex < KTotalCollection)
            {
            TInt mediaCount = iEntries[aItemIndex]->Count();
            GLX_LOG_INFO1("GlxCH: HandleAttributesAvailableL mediaCount(%d)", mediaCount);
            if (iCollectionIndexes[aItemIndex] != mediaCount)
                {
		        GLX_LOG_INFO("GlxCH: *** UpdateDataInCPSL ***");
                SetRefreshNeeded(ETrue);
                UpdateDataInCPSL(aItemIndex);
                iCollectionIndexes[aItemIndex] = mediaCount;
                
                // Fix for ESLM-7WKJZR :: Sometimes Thumbnails are not updating when 
                // connecting/disconnecting USB 
                // If there is a change in media count, when photo suite is in forground,                
                // Update preview thumbnails of all collections.
                if(SuiteInFocus())
                	{
                	GLX_LOG_INFO("CGlxContentHarvesterPlugin : *** SuiteInFocus ***");
                	iEntries[aItemIndex]->UpdatePreviewThumbnailListL();
                	}
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, 
        TInt /*aOldIndex*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPlugin::HandleFocusChangedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPlugin::HandleItemSelectedL" );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleMessageL(const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/)
    {
    // Do nothing
    TRACER( "CGlxContentHarvesterPlugin::HandleMessageL" );
    }   

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::FillInputListWithDataL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::FillInputListWithDataL( 
        CLiwGenericParamList* aInParamList,
        const TDesC& aPublisherId, const TDesC& aContentType, 
        TInt aCollection)
    {
    TRACER( "CGlxContentHarvesterPlugin::FillInputListWithDataL" );

    if(iMediaList)
        {     
        aInParamList->Reset();
        if (!iCPSInterface) 
            {
            User::Leave( KErrNotFound );
            } 
        else
            {
            TLiwGenericParam cptype( KType, TLiwVariant( KCpData ));
            aInParamList->AppendL( cptype );

            CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
            CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

            TBuf8<KMaxName> subTitleTxt;
            const TGlxMedia& glxMedia = iMediaList->Item(aCollection);
            switch (aCollection)
                {
				// Bug Fix @ ESLM-7VWF28 :: " Downloads " collection folder is replaced by " ALL " collection folder
                case KItemIndexAll:
                    subTitleTxt.Append(KAllSubtitle);
                    break;

                case KItemIndexMonths:
                    subTitleTxt.Append(KMonthsSubtitle);
                    break;

                case KItemIndexAlbums:
                    subTitleTxt.Append(KAlbumsSubtitle);
                    break;

                case KItemIndexTags:
                    subTitleTxt.Append(KTagsSubtitle);
                    break;

                default:
                    break;
                }

#ifdef _DEBUG
            TPtrC pStr=  glxMedia.SubTitle();
            TBuf<KMaxName> buf;
            buf.Copy(pStr);
            GLX_LOG_INFO1("CGlxContentHarvesterPlugin::FillInputListWithDataL,SUB TITLE=%S", &subTitleTxt);
            GLX_LOG_INFO1("CGlxContentHarvesterPlugin::FillInputListWithDataL,SUB TITLE=%S", &buf);
#endif            

            map->Remove(subTitleTxt);
            map->InsertL( subTitleTxt,  TLiwVariant( glxMedia.SubTitle() ) );

            cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
            cpdatamap->InsertL( KContentType, TLiwVariant( aContentType )); 
            cpdatamap->InsertL( KContentId, TLiwVariant( KContId1 ));
            cpdatamap->InsertL( KDataMap, TLiwVariant(map) );

            TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));        
            aInParamList->AppendL( item );
            CleanupStack::PopAndDestroy( map );
            CleanupStack::PopAndDestroy( cpdatamap );
            item.Reset();
            cptype.Reset(); 
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::UpdateDataInCPSL()
// ----------------------------------------------------------------------------
//  
void CGlxContentHarvesterPlugin::UpdateDataInCPSL(TInt aCollection) 
    {
    TRACER( "CGlxContentHarvesterPlugin::UpdateDataInCPSL" );
    GLX_LOG_INFO1("GlxCH::UpdateDataInCPSL(%d)", aCollection);

    TInt mtpStatus = KErrNotFound;
    RProperty::Get(KMtpPSUid, KMtpPSStatus, mtpStatus);
    GLX_LOG_INFO1("GlxCH::UpdateDataInCPSL mtpStatus(%d)",mtpStatus);
    if ( (mtpStatus != KErrNotFound) &&
         (mtpStatus != EMtpPSStatusUninitialized) )
		{
	 	if (!SuiteInFocus())
	        {
	        GLX_LOG_INFO("GlxCH: *** MTP ACTIVE & SUITE NOT IN FOCUS ***");
    	    return;
        	}
		}

    //update data in CPS
    CLiwGenericParamList* inParamList = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* outParamList = CLiwGenericParamList::NewLC();
    TBuf<KMaxName> publisherTxt;
    TBuf<KMaxName> contTypeTxt;

    switch (aCollection)
        {
		// Bug Fix @ ESLM-7VWF28 :: " Downloads " collection folder is replaced by " ALL " collection folder
        case KItemIndexAll:
            publisherTxt.Append(KPublisherTextAll);
            contTypeTxt.Append(KContTypeTextAll);
            break;

        case KItemIndexMonths:
            publisherTxt.Append(KPublisherTextMonths);
            contTypeTxt.Append(KContTypeTextMonths);
            break;

        case KItemIndexAlbums:
            publisherTxt.Append(KPublisherTextAlbums);
            contTypeTxt.Append(KContTypeTextAlbums);
            break;

        case KItemIndexTags:
            publisherTxt.Append(KPublisherTextTags);
            contTypeTxt.Append(KContTypeTextTags);
            break;

        default:
            break;
        }

    FillInputListWithDataL(inParamList, publisherTxt, contTypeTxt, aCollection);

    iCPSInterface->ExecuteCmdL( KAdd,  *inParamList, *outParamList );
    CleanupStack::PopAndDestroy(outParamList);
    CleanupStack::PopAndDestroy(inParamList);
    } 

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::CreateMedialistL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::CreateMedialistL()
    {
    TRACER( "CGlxContentHarvesterPlugin::CreateMedialistL" );
    if(!iMediaList)
        {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL(path);
        iMediaList = MGlxMediaList::InstanceL( *path );
        CreateContextsL();
        iMediaList->AddMediaListObserverL( this ); 
        CleanupStack::PopAndDestroy( path );
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::DestroyMedialist
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::DestroyMedialist()
    {
    TRACER( "CGlxContentHarvesterPlugin::DestroyMedialist" );
    if( iMediaList )
        {
        iMediaList->RemoveMediaListObserver( this );
        iMediaList->RemoveContext(iSubtitleAttributeContext);
        delete iSubtitleAttributeContext;
        iSubtitleAttributeContext = NULL;
        iMediaList->Close();
        iMediaList = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::CreateContextsL
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::CreateContextsL()
    {
    TRACER( "CGlxContentHarvesterPlugin::CreateContextsL" );
    iSubtitleAttributeContext = CGlxDefaultAttributeContext::NewL();
    iSubtitleAttributeContext->SetRangeOffsets( KListDataWindowSize, KListDataWindowSize );
    iSubtitleAttributeContext->AddAttributeL( KGlxMediaCollectionPluginSpecificSubTitle );
    iMediaList->AddContextL( iSubtitleAttributeContext, KMaxInt );
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::HandleError
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::HandleError(TInt /*aError*/)
    {
    TRACER( "CGlxContentHarvesterPlugin::HandleError" );    
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::UpdatePlugins
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::UpdatePlugins(TBool aUpdate)
    {
    TRACER( "CGlxContentHarvesterPlugin::UpdatePlugins" );  
    TInt pluginIndex;
    if(aUpdate)
        {
        if(IsRefreshNeeded())
            {
            GLX_LOG_INFO1("CGlxContentHarvesterPlugin::UpdatePlugins=%d",iIsRefreshNeeded);
            for(pluginIndex=0; pluginIndex < KTotalCollection; pluginIndex++)
                {
                iEntries[pluginIndex]->UpdatePreviewThumbnailListL();
                }
            SetRefreshNeeded(EFalse);
            }
        }
    else
        {
        //This check is added here,since all the collection are not created on the power 
        // up of the phone,so we dont need to remove the context at that stage else it will panic.
        if(iEntries.Count()== KTotalCollection)
            {
            for(pluginIndex=0; pluginIndex < KTotalCollection; pluginIndex++)
                {
                iEntries[pluginIndex]->RemoveContextAndObserver();
                }
            //before exiting the matrix menu , need to set this to true
            //when again matrix menu is launched,the thumbnails has to be updated
            //this use case is when we have not modified any items in the photos
            //so refreshneeded will not be set and UpdatePreviewThumbnailListL is not called when
            //matrix menu gets the focused,so update the refresh state
            SetRefreshNeeded(ETrue);
            }
        }
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::IsRefreshNeeded
// ---------------------------------------------------------------------------
//
TBool CGlxContentHarvesterPlugin::IsRefreshNeeded()
    {
    TRACER( "CGlxContentHarvesterPlugin::IsRefreshNeeded" );  
    GLX_LOG_INFO1("CGlxContentHarvesterPlugin::IsRefreshNeeded(%d)", iIsRefreshNeeded);
    return iIsRefreshNeeded;
    }
// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::SetRefreshNeeded
// ---------------------------------------------------------------------------
//
void CGlxContentHarvesterPlugin::SetRefreshNeeded(TBool aRefreshNeed)
    {
    TRACER( "CGlxContentHarvesterPlugin::SetRefreshNeeded" );  
    GLX_LOG_INFO1("CGlxContentHarvesterPlugin::SetRefreshNeeded(%d)", aRefreshNeed);
    iIsRefreshNeeded = aRefreshNeed;
    }

// ---------------------------------------------------------------------------
// CGlxContentHarvesterPlugin::SuiteInFocus
// ---------------------------------------------------------------------------
//
TBool CGlxContentHarvesterPlugin::SuiteInFocus()
    {
    TRACER( "CGlxContentHarvesterPlugin::SuiteInFocus" );  
    TBool focus = EFalse;
    for(TInt pluginIndex=0; pluginIndex < KTotalCollection; pluginIndex++)
        {
        focus = iEntries[pluginIndex]->Focused();
        if (focus)
            {
            GLX_LOG_INFO1("GlxCH: SuiteInFocus pluginIndex(%d)", pluginIndex);
            break;
            }
        }
    GLX_LOG_INFO1("CGlxContentHarvesterPlugin::SuiteInFocus focus(%d)", focus);
    return focus;
    }

//  End of File


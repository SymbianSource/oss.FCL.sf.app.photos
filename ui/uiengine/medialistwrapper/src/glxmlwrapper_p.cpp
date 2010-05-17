/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

/*glxmlwrapper_p.cpp*/
//external includes
#include <hbicon.h>
#include <glxtracer.h>
#include <mglxmedialist.h>
#include <glxlog.h>
#include <glxthumbnailcontext.h>
#include <glxattributecontext.h>
#include <glxuistd.h>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectiongeneraldefs.h>
#include <glxthumbnailattributeinfo.h>
#include <glxfilterfactory.h>
#include <glxmedia.h>
#include <glxerrormanager.h>
#include <glxattributecontext.h>
#include <glxuistd.h>
#include <glxlistdefs.h>
#include <hal.h>
#include <hal_data.h>
#include <glxmediaid.h>
#include <caf/caferr.h>
//internal includes 
#include "glxmlwrapper_p.h"
#include "glxmlgenericobserver.h"
#include "glxattributeretriever.h"
#include "glxicondefs.h" //Contains the icon names/Ids

//#define GLXPERFORMANCE_LOG  
#include <glxperformancemacro.h>
#include "glxtitlefetcher.h"
//constant declaration
const TInt KTBAttributeAvailable(1);
const TInt KTBAttributeUnavailable(0);
const TInt KTBAttributeCorrupt(-1);
const TInt KListDataWindowSize(10);
const TInt KGridTNWIdth (127);
const TInt KGridTNHeight (110);
const TInt KGridTNPTWIdth (119);
const TInt KGridTNPTHeight (103);
const TInt KFullScreenTNLSWidth (640);
const TInt KFullScreenTNLSHeight (360);
const TInt KFullScreenTNPTWidth (360);
const TInt KFullScreenTNPTHeight (640);

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
GlxMLWrapperPrivate* GlxMLWrapperPrivate::Instance(GlxMLWrapper* aMLWrapper,
    int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType,QString uri)
    {
    TRACER("GlxMLWrapperPrivate::NewLC()");

    GlxMLWrapperPrivate* self = new GlxMLWrapperPrivate(aMLWrapper);
   if(self){
	   TRAPD(err,self->ConstructL(aCollectionId, aHierarchyId, aFilterType,uri));
	   if(err != KErrNone){
		   delete self;
		   self = NULL;
		   }
	   }
   
    return self;
    }
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
GlxMLWrapperPrivate::GlxMLWrapperPrivate(GlxMLWrapper* aMLWrapper): iMLWrapper(aMLWrapper),
								iGridContextActivated(EFalse), iLsFsContextActivated(EFalse),
								iPtFsContextActivated(EFalse), iPtListContextActivated(EFalse),
								iSelectionListContextActivated(EFalse)
    {
    TRACER("GlxMLWrapperPrivate::GlxMLWrapperPrivate");
	iGridThumbnailContext = NULL;
	iPtFsThumbnailContext = NULL;
	iLsFsThumbnailContext = NULL;
	iTitleAttributeContext = NULL;
	iSubtitleAttributeContext = NULL;
    iListThumbnailContext = NULL;
    iFocusGridThumbnailContext = NULL;
    iFocusFsThumbnailContext = NULL;
    iFilmStripThumbnailContext = NULL;
    iFavouriteContext = NULL;
    iTitleFetcher = NULL;
    iViewTitle = QString();
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//  
void GlxMLWrapperPrivate::ConstructL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType,QString uri)
    {
    TRACER("GlxMLWrapperPrivate::ConstructL");
     if(aCollectionId == KGlxCollectionPluginFavoritesAlbumId)
         {
         CreateMediaListFavoritesItemL(aCollectionId, aHierarchyId,aFilterType,uri);
         }
     else if(aCollectionId != KGlxAlbumsMediaId)
		{
		CreateMediaListL(aCollectionId, aHierarchyId,aFilterType);
		}
	else
		{
		//for creating Medial List for Albums Media path Items
		CreateMediaListAlbumItemL(aCollectionId, aHierarchyId,aFilterType);
		}
	iMLGenericObserver = CGlxMLGenericObserver::NewL(*iMediaList,this);
	iBlockyIteratorForFilmStrip.SetRangeOffsets(0,0);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
GlxMLWrapperPrivate::~GlxMLWrapperPrivate()
    {
    TRACER("GlxMLWrapperPrivate::~GlxMLWrapperPrivate");
	RemoveGridContext();
	RemovePtFsContext();
	RemoveLsFsContext();
	RemoveListContext();
	RemoveFavouriteContext();
	delete iMLGenericObserver;
	iMLGenericObserver = NULL;
	if (iMediaList)
        {
         iMediaList->Close();
        }
    }

// ---------------------------------------------------------------------------
// SetContextMode
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::SetContextMode(GlxContextMode aContextMode)
    {
	TInt err = KErrNone;
	if(aContextMode <= GlxContextPtFs) 
		{  
		TRAP(err, SetThumbnailContextL(aContextMode) ); //todo add a trap here
		}
	else if(aContextMode == GlxContextFavorite)
	    {
	    TRAP(err,SetFavouriteContextL());
	    }
	else
		{
		TRAP(err, SetListContextL(aContextMode) );
		}
	
	GLX_LOG_INFO1("GlxMLWrapperPrivate::SetContextMode error %d", err);
	iContextMode = aContextMode;
	}

// ---------------------------------------------------------------------------
// SetFavouriteContextL
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::SetFavouriteContextL()
    {
    iFavouriteContext = CGlxDefaultAttributeContext::NewL();
    iFavouriteContext->AddAttributeL( KMPXMediaGeneralCount );
    iMediaList->AddContextL( iFavouriteContext, KGlxFetchContextPriorityLow );
    }
	
// ---------------------------------------------------------------------------
// RemoveFavouriteContext
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::RemoveFavouriteContext()
    {
    if(iFavouriteContext )
        {
        iMediaList->RemoveContext(iFavouriteContext);
        delete iFavouriteContext;
        iFavouriteContext = NULL;        
        }
	}

// ---------------------------------------------------------------------------
// SetListContextL
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::SetListContextL(GlxContextMode aContextMode)
{
	if(GlxContextLsList == aContextMode || GlxContextPtList == aContextMode)
		{
		if(!iPtListContextActivated)
			{
			if(NULL == iTitleAttributeContext)
				{
				iTitleAttributeContext = CGlxDefaultListAttributeContext::NewL();
			    iTitleAttributeContext->SetRangeOffsets( KListDataWindowSize, 
						KListDataWindowSize );
			    iTitleAttributeContext->AddAttributeL( KMPXMediaGeneralTitle );
			    iTitleAttributeContext->AddAttributeL( KGlxMediaGeneralSlideshowableContent );
			    iTitleAttributeContext->AddAttributeL( KGlxMediaGeneralSystemItem );
				iMediaList->AddContextL( iTitleAttributeContext, KGlxFetchContextPriorityNormal );
				}
			if(NULL == iSubtitleAttributeContext)
				{
			    iSubtitleAttributeContext = CGlxDefaultListAttributeContext::NewL();
				iSubtitleAttributeContext->SetRangeOffsets( KListDataWindowSize, 
						 KListDataWindowSize );
				iSubtitleAttributeContext->AddAttributeL( 
								 KGlxMediaCollectionPluginSpecificSubTitle );
				iMediaList->AddContextL( iSubtitleAttributeContext, KGlxFetchContextPriorityNormal );

				}
            if(NULL == iListThumbnailContext)
	            {
                iThumbnailIterator.SetRangeOffsets(KListDataWindowSize,2);
	            iListThumbnailContext = CGlxThumbnailContext::NewL(&iThumbnailIterator);
	            iListThumbnailContext->SetDefaultSpec(KGridTNWIdth,KGridTNHeight );
	            iMediaList->AddContextL(iListThumbnailContext ,KGlxFetchContextPriorityNormal );   
	            }
			iPtListContextActivated = ETrue;
			}
		}
        else if(GlxContextSelectionList == aContextMode )
        {
            if(NULL == iTitleAttributeContext)
            {
                iTitleAttributeContext = CGlxDefaultListAttributeContext::NewL();
                iTitleAttributeContext->SetRangeOffsets( KListDataWindowSize, KListDataWindowSize );
                iTitleAttributeContext->AddAttributeL( KMPXMediaGeneralTitle );
                iTitleAttributeContext->AddAttributeL( KGlxMediaGeneralSlideshowableContent );
                iTitleAttributeContext->AddAttributeL( KGlxMediaGeneralSystemItem );
                iMediaList->AddContextL( iTitleAttributeContext, KGlxFetchContextPriorityNormal );
            }
        iSelectionListContextActivated = ETrue;
        }
}


// ---------------------------------------------------------------------------
// CreateThumbnailContext
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::SetThumbnailContextL(GlxContextMode aContextMode)
{
    TRACER("GlxMLWrapperPrivate::SetThumbnailContext()");
    if( aContextMode == GlxContextLsGrid || aContextMode == GlxContextPtGrid) {
        if(!iGridContextActivated) {
            CreateGridContextL();
        }
    
        if(iPtFsContextActivated) {
            RemovePtFsContext();
        }
        
        if(iLsFsContextActivated) {
            RemoveLsFsContext();
        }
    }
	
	if(aContextMode == GlxContextLsFs && !iLsFsContextActivated) {
		if(iGridContextActivated) {
            RemoveGridContext();
		}
		if(iPtFsContextActivated) {
			RemovePtFsContext();
		}
		CreateLsFsContextL();
	}
	
	if(aContextMode == GlxContextPtFs && !iPtFsContextActivated) {
		if(iGridContextActivated) {
            RemoveGridContext();
		}
		if(iLsFsContextActivated) {
			RemoveLsFsContext();
		}
		CreatePtFsContextL();
	}
}

// ---------------------------------------------------------------------------
// CreateGridContextL
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::CreateGridContextL()
    {
	TRACER("GlxMLWrapperPrivate::CreateGridContextL()");
	if(iGridThumbnailContext && !iGridContextActivated)
		{
		delete iGridThumbnailContext;
		iGridThumbnailContext = NULL;
		}
	if(!iGridContextActivated)
		{
		iGridThumbnailContext = CGlxThumbnailContext::NewL( &iBlockyIterator ); // set the thumbnail context
	    iGridThumbnailContext->SetDefaultSpec( KGridTNWIdth, KGridTNHeight );  //todo get these image sizes from  the layout.
        // show static items if required
        iMediaList->SetStaticItemsEnabled(EFalse);
	    iMediaList->AddContextL(iGridThumbnailContext, KGlxFetchContextPriorityNormal );
		iGridContextActivated = ETrue;
		}

	CMPXCollectionPath* path = iMediaList->PathLC( NGlxListDefs::EPathParent );
	iTitleFetcher = CGlxTitleFetcher::NewL(*this, path);
	CleanupStack::PopAndDestroy(path);

	}

// ---------------------------------------------------------------------------
// CreateLsFsContextL
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::CreateLsFsContextL()
    {
    TRACER("GlxMLWrapperPrivate::CreateGridContextL()");
    if(iLsFsThumbnailContext && !iLsFsContextActivated)
        {
        delete iLsFsThumbnailContext;
        iLsFsThumbnailContext = NULL;
        }
    if(!iLsFsContextActivated)
        {
        iLsFsThumbnailContext = CGlxDefaultThumbnailContext::NewL();
        iLsFsThumbnailContext->SetRangeOffsets(2,2);
        iLsFsThumbnailContext->SetDefaultSpec( KFullScreenTNLSWidth, KFullScreenTNLSHeight );  //todo get these image sizes from  the layout.

        if(!iFocusFsThumbnailContext)
            {
            iFocusFsThumbnailContext = CGlxDefaultThumbnailContext::NewL();
            iFocusFsThumbnailContext->SetRangeOffsets(0,0);
            iFocusFsThumbnailContext->SetDefaultSpec( KFullScreenTNLSWidth, KFullScreenTNLSHeight );  //todo get these image sizes from  the layout.
            }
        if(!iFocusGridThumbnailContext)
            {
            iFocusGridThumbnailContext = CGlxDefaultThumbnailContext::NewL(); // set the thumbnail context for Focus Grid
            iFocusGridThumbnailContext->SetRangeOffsets(0,0);
            iFocusGridThumbnailContext->SetDefaultSpec( KGridTNWIdth, KGridTNHeight );  //todo get these image sizes from  the layout.
            }

        if(!iFilmStripThumbnailContext)
            {
            iFilmStripThumbnailContext = CGlxThumbnailContext::NewL( &iBlockyIteratorForFilmStrip ); // set the thumbnail context for Focus Grid
            iFilmStripThumbnailContext->SetDefaultSpec( KGridTNWIdth, KGridTNHeight );  //todo get these image sizes from  the layout.
            }

        // show static items if required
        iMediaList->SetStaticItemsEnabled(EFalse);
        iMediaList->AddContextL(iFocusFsThumbnailContext, 8 );      // Temp will change this number  
        iMediaList->AddContextL(iFocusGridThumbnailContext, 9 );    // Temp will change this number
        iMediaList->AddContextL(iFilmStripThumbnailContext, 7 );    // Temp will change this number 
        iMediaList->AddContextL(iLsFsThumbnailContext, KGlxFetchContextPriorityGridViewFullscreenVisibleThumbnail );
        iLsFsContextActivated = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CreatePtFsContextL
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::CreatePtFsContextL()
    {
    TRACER("GlxMLWrapperPrivate::CreateGridContextL()");
    if(iPtFsThumbnailContext && !iPtFsContextActivated)
        {
        delete iPtFsThumbnailContext;
        iPtFsThumbnailContext = NULL;
        }
    if(!iPtFsContextActivated)
        {
        iPtFsThumbnailContext = CGlxDefaultThumbnailContext::NewL(); // set the thumbnail context
        iPtFsThumbnailContext->SetRangeOffsets(2,2);
        iPtFsThumbnailContext->SetDefaultSpec( KFullScreenTNPTWidth, KFullScreenTNPTHeight );  //todo get these image sizes from  the layout.

        if(!iFocusFsThumbnailContext)
            {
            iFocusFsThumbnailContext = CGlxDefaultThumbnailContext::NewL(); 
            iFocusFsThumbnailContext->SetRangeOffsets(0,0);
            iFocusFsThumbnailContext->SetDefaultSpec( KFullScreenTNPTWidth, KFullScreenTNPTHeight );  //todo get these image sizes from  the layout.
            }
        if(!iFocusGridThumbnailContext)
            {
            iFocusGridThumbnailContext = CGlxDefaultThumbnailContext::NewL(); // set the thumbnail context for Focus Grid
            iFocusGridThumbnailContext->SetRangeOffsets(0,0);
            iFocusGridThumbnailContext->SetDefaultSpec( KGridTNWIdth, KGridTNHeight );  //todo get these image sizes from  the layout.
            }

        if(!iFilmStripThumbnailContext)
            {
            iFilmStripThumbnailContext = CGlxThumbnailContext::NewL( &iBlockyIteratorForFilmStrip ); // set the thumbnail context for Focus Grid
            iFilmStripThumbnailContext->SetDefaultSpec( KGridTNPTWIdth, KGridTNPTHeight );  //todo get these image sizes from  the layout.
            }

        // show static items if required
        iMediaList->SetStaticItemsEnabled(EFalse);
        iMediaList->AddContextL(iFocusFsThumbnailContext, 8 );      // Temp will change this number  
        iMediaList->AddContextL(iFocusGridThumbnailContext, 9 );    // Temp will change this number  
        iMediaList->AddContextL(iFilmStripThumbnailContext, 7 );    // Temp will change this number 
        iMediaList->AddContextL(iPtFsThumbnailContext, KGlxFetchContextPriorityGridViewFullscreenVisibleThumbnail );
        iPtFsContextActivated = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// RemoveGridContext
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::RemoveGridContext()
    {
	if(iGridThumbnailContext && iGridContextActivated)
		{
		iMediaList->RemoveContext(iGridThumbnailContext);
		delete iGridThumbnailContext;
		iGridThumbnailContext = NULL;
		iGridContextActivated = EFalse;
		}
	delete iTitleFetcher;
	iTitleFetcher = NULL;
	}

// ---------------------------------------------------------------------------
// RemoveLsFsContext
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::RemoveLsFsContext()
    {
	if(iLsFsThumbnailContext && iLsFsContextActivated)
		{
		iMediaList->RemoveContext(iLsFsThumbnailContext);
		delete iLsFsThumbnailContext;
		iLsFsThumbnailContext = NULL;
		
		if(iFocusFsThumbnailContext)
		    {
            iMediaList->RemoveContext(iFocusFsThumbnailContext);
            delete iFocusFsThumbnailContext;
            iFocusFsThumbnailContext = NULL;
		    }
	    
		if(iFocusGridThumbnailContext)
		    {
            iMediaList->RemoveContext(iFocusGridThumbnailContext);
            delete iFocusGridThumbnailContext;
            iFocusGridThumbnailContext = NULL;
		    }
		if(iFilmStripThumbnailContext)
		    {
		    iMediaList->RemoveContext(iFilmStripThumbnailContext);
		    delete iFilmStripThumbnailContext;
		    iFilmStripThumbnailContext = NULL;
		    }		    
	    
		iLsFsContextActivated = EFalse;
		}
	}

// ---------------------------------------------------------------------------
// RemovePtFsContext
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::RemovePtFsContext()
    {
	if(iPtFsThumbnailContext && iPtFsContextActivated)
		{
		iMediaList->RemoveContext(iPtFsThumbnailContext);
		delete iPtFsThumbnailContext;
		iPtFsThumbnailContext = NULL;
		
        if(iFocusFsThumbnailContext)
            {
            iMediaList->RemoveContext(iFocusFsThumbnailContext);
            delete iFocusFsThumbnailContext;
            iFocusFsThumbnailContext = NULL;
            }
        
        if(iFocusGridThumbnailContext)
            {
            iMediaList->RemoveContext(iFocusGridThumbnailContext);
            delete iFocusGridThumbnailContext;
            iFocusGridThumbnailContext = NULL;
            }
        if(iFilmStripThumbnailContext)
            {
            iMediaList->RemoveContext(iFilmStripThumbnailContext);
            delete iFilmStripThumbnailContext;
            iFilmStripThumbnailContext = NULL;
            }
	        
		iPtFsContextActivated = EFalse;
		}
	}


// ---------------------------------------------------------------------------
// RemoveListContext
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::RemoveListContext()
	{
	if(iPtListContextActivated || iSelectionListContextActivated)
	    {
		if(iTitleAttributeContext)
			{
			iMediaList->RemoveContext(iTitleAttributeContext);
			delete iTitleAttributeContext;
			iTitleAttributeContext = NULL;
			}
		if(iSubtitleAttributeContext)
			{
			iMediaList->RemoveContext(iSubtitleAttributeContext);
			delete iSubtitleAttributeContext;
			iSubtitleAttributeContext = NULL;
			}
		if(iListThumbnailContext)
			{
			iMediaList->RemoveContext(iListThumbnailContext);
			delete iListThumbnailContext;
			iListThumbnailContext = NULL;
			}
		iPtListContextActivated = EFalse;
	    iSelectionListContextActivated = EFalse;
		}
	}
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
int GlxMLWrapperPrivate::GetItemCount()
    {
	return iMediaList->Count();
    }

// ---------------------------------------------------------------------------
// CreateMediaListAlbumItemL()
// Creates the media list for the album Item 
// ---------------------------------------------------------------------------
void GlxMLWrapperPrivate::CreateMediaListAlbumItemL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType)
	{
	TRACER("GlxMLWrapperPrivate::CreateMediaListAlbumItemL");
	Q_UNUSED(aCollectionId);
	Q_UNUSED(aHierarchyId);
	//retrieve the instance of Media list for ALBUMS LIST which will be saved in iMediaList
	CreateMediaListL(KGlxCollectionPluginAlbumsImplementationUid,0,EGlxFilterAlbum);
	//retrieve the path of the focussed item of the MediaList
	CMPXCollectionPath* path = iMediaList->PathLC(NGlxListDefs:: EPathFocusOrSelection);
	//close the existing instance of Media List
	iMediaList->Close();
	iMediaList = NULL;
	//create new media list with the derived path
	CMPXFilter* filter = TGlxFilterFactory::CreateItemTypeFilterL(aFilterType);   //todo take actual filter type
	CleanupStack::PushL(filter);
	iMediaList = MGlxMediaList::InstanceL( *path, 
	                  TGlxHierarchyId(0), filter );  //todo take actual hierarchy
	CleanupStack::PopAndDestroy( filter );
	
	GLX_LOG_INFO1("GlxMLWrapperPrivate::CreateMediaListAlbumItemL  - Path level = %d",
	                                                     path->Levels());
	CleanupStack::PopAndDestroy(path);
	}
// ---------------------------------------------------------------------------
// CreateMediaListL()
// Creates a collection path
// Create a filter as requested filtertype
// Creates the medialist
// ---------------------------------------------------------------------------
void GlxMLWrapperPrivate::CreateMediaListL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType) 
	{
	TRACER("GlxMLWrapperPrivate::CreateMediaListL");
  	Q_UNUSED(aHierarchyId);	
	// Create path to the list of images and videos
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	//path->AppendL(KGlxCollectionPluginAllImplementationUid);	// All item collection plugin  //todo take actual collection path
	path->AppendL(aCollectionId);
	// Create filter to filter out either images or videos
	GLX_LOG_INFO1("GlxMLWrapperPrivate::MediaListL  - Filter Type = %d",aFilterType);
	CMPXFilter* filter = NULL;
	if( EGlxFilterExcludeCamera == aFilterType)
		{
		filter = TGlxFilterFactory::CreateCameraAlbumExclusionFilterL();
		CleanupStack::PushL(filter);
		}
	else
		{
		filter = TGlxFilterFactory::CreateItemTypeFilterL(aFilterType);   //todo take actual filter type
		CleanupStack::PushL(filter);
		}
	// Create the media list
	iMediaList =  MGlxMediaList::InstanceL( *path, 
	                  TGlxHierarchyId(aHierarchyId), filter );  //todo take actual hierarchy 
	CleanupStack::PopAndDestroy( filter );
	
	GLX_LOG_INFO1("GlxMLWrapperPrivate::MediaListL  - Path level = %d",
	                                                     path->Levels());
		CleanupStack::PopAndDestroy(path);    
	}

// ---------------------------------------------------------------------------
// CreateMediaListL() for Favorites Album 
// Creates a collection path
// Create a filter as requested filtertype
// Creates the medialist
// ---------------------------------------------------------------------------
void GlxMLWrapperPrivate::CreateMediaListFavoritesItemL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType,QString uri) 
    {
    TRACER("GlxMLWrapperPrivate::CreateMediaListFavoritesItemL");
    Q_UNUSED(aHierarchyId); 
    Q_UNUSED(aCollectionId); 
    Q_UNUSED(aFilterType); 
    // Create path to the list of images and videos
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
    path->AppendL( TMPXItemId(KGlxCollectionFavoritesId) );
    path->Set( 0 );
    TPtrC16 str(reinterpret_cast<const TUint16*>(uri.utf16()));
    HBufC *ptr = str.Alloc();
    CMPXFilter* filter = TGlxFilterFactory::CreateURIFilterL(*ptr);  
    CleanupStack::PushL(filter);
    iMediaList =  MGlxMediaList::InstanceL( *path, 
            TGlxHierarchyId(KGlxCollectionPluginAlbumsDllUid), filter );
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( path );       
    }

/*
 * retrieveItemIcon
 */
HbIcon* GlxMLWrapperPrivate::RetrieveItemIcon(int aItemIndex, GlxTBContextType aTBContextType)
{
    TInt itemHeight = 0;
    TInt itemWidth = 0;
    switch (aTBContextType) {
        case GlxTBContextGrid: {
            itemHeight = KGridTNHeight;
            itemWidth = KGridTNWIdth;
        }
        break;
        
        case GlxTBContextPtFs:  {
            itemHeight = KFullScreenTNPTHeight;
            itemWidth = KFullScreenTNPTWidth;
        }
        break;
        
        case GlxTBContextLsFs: {
            itemHeight = KFullScreenTNLSHeight;
            itemWidth = KFullScreenTNLSWidth;
        }
        break;
        
        default :
        break;
    }
    
    const TGlxMedia& item = iMediaList->Item( aItemIndex );
    TMPXAttribute thumbnailAttribute( KGlxMediaIdThumbnail, 
                       GlxFullThumbnailAttributeId( ETrue, itemWidth, itemHeight ) ); //todo map icon size with actual mode        
    const CGlxThumbnailAttribute* value = item.ThumbnailAttribute( thumbnailAttribute );
    TInt tnError = GlxErrorManager::HasAttributeErrorL( item.Properties(), KGlxMediaIdThumbnail );
	
    if ( value && value->iBitmap != NULL ) {
        if( aTBContextType == GlxTBContextGrid ) {
            GLX_LOG_INFO1("### GlxMLWrapperPrivate::RetrieveItemIcon value-Index is %d",aItemIndex);
            
            if (  iContextMode == GlxContextPtGrid ) {
                return convertFBSBitmapToHbIcon( value->iBitmap , KGridTNPTWIdth, KGridTNPTHeight);
            }
            else {
                return convertFBSBitmapToHbIcon( value->iBitmap );
            }
        }
        else {
            return convertFBSBitmapToHbIcon( value->iBitmap, itemWidth, itemHeight ) ;
        }
    }
    /*else if (item.GetIconInfo(icon))   //todo will be required if we are planning to have static item else remove
    {  
        GLX_LOG_INFO1("### GlxMLWrapperPrivate::HandleAttributesAvailableL GetIconInfo-Index is %d",aItemIndex);
    }*/
    else if( tnError == KErrCANoRights)	{
        //handle DRM case
    }
    else if( tnError ) {
        return (new HbIcon(GLXICON_CORRUPT));
    }
    
    GLX_LOG_INFO1("### GlxMLWrapperPrivate::RetrieveItemIcon value-Index is %d and have returned empty icon",aItemIndex);
    return NULL;
}

QImage GlxMLWrapperPrivate::RetrieveItemImage(int aItemIndex, GlxTBContextType aTBContextType)
    {
    TInt itemHeight = 0;
    TInt itemWidth = 0;
    switch (aTBContextType)
        {
        case GlxTBContextGrid: 
            {
            itemHeight = KGridTNHeight;
            itemWidth = KGridTNWIdth;
            }
            break;
        case GlxTBContextPtFs: 
            {
            itemHeight = KFullScreenTNPTHeight;
            itemWidth = KFullScreenTNPTWidth;
            }
            break;
        case GlxTBContextLsFs: 
            {
            itemHeight = KFullScreenTNLSHeight;
            itemWidth = KFullScreenTNLSWidth;
            }
            break;
        }
    const TGlxMedia& item = iMediaList->Item( aItemIndex );
    TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue,itemWidth,itemHeight ) );      
    const CGlxThumbnailAttribute* value = item.ThumbnailAttribute(
            thumbnailAttribute );
    TInt tnError = GlxErrorManager::HasAttributeErrorL(
                      item.Properties(), KGlxMediaIdThumbnail );
    TSize iconSize(itemWidth, itemHeight);

    if (value && value->iBitmap != NULL)
        {
        value->iBitmap->LockHeap();
        TUint32 *tempData = value->iBitmap->DataAddress();
        uchar *data = (uchar *)(tempData);  
        int bytesPerLine = value->iBitmap->ScanLineLength(value->iBitmap->SizeInPixels().iWidth , value->iBitmap->DisplayMode());
        QImage image = QImage(data, value->iBitmap->SizeInPixels().iWidth, value->iBitmap->SizeInPixels().iHeight, bytesPerLine, QImage::Format_RGB16).convertToFormat(QImage::Format_ARGB32_Premultiplied);
        value->iBitmap->UnlockHeap();
        return image;
         }
    else if( tnError == KErrCANoRights) 
        {
        //handle DRM case
        }
    else if( tnError ) 
        {
        if(iCorruptImage.isNull())
            {
            HbIcon *icon = new HbIcon(GLXICON_CORRUPT);
            if(!icon->isNull())
                {
                // this image Creation is Slow. 
                // But what to do, Q class's Does not undersatnd our Localised File names
                iCorruptImage = icon->pixmap().toImage();
                }
            delete icon;
            }
        return iCorruptImage;
        }

     return QImage();
    }
// ---------------------------------------------------------------------------
//  RetrieveListTitle
// ---------------------------------------------------------------------------
//
QString GlxMLWrapperPrivate::RetrieveListTitle(int aItemIndex)
{
	const TGlxMedia& item = iMediaList->Item( aItemIndex );
	const TDesC &title = item.Title();
    QString albumTitle =  QString::fromUtf16(title.Ptr(), title.Length());
	return albumTitle;
 }

// ---------------------------------------------------------------------------
//  RetrieveListSubTitle
// ---------------------------------------------------------------------------
//
QString GlxMLWrapperPrivate::RetrieveListSubTitle(int aItemIndex)
{
	const TGlxMedia& item = iMediaList->Item( aItemIndex );
	const TDesC &subTitle = item.SubTitle();
    QString albumSubTitle =  QString::fromUtf16(subTitle.Ptr(), subTitle.Length());
	return albumSubTitle;
}

// ---------------------------------------------------------------------------
//  Retrieve number of image contained by album list
// ---------------------------------------------------------------------------
//
int GlxMLWrapperPrivate::RetrieveListItemCount(int aItemIndex)
{
    const TGlxMedia& item = iMediaList->Item( aItemIndex );
    int count = 0;
    TBool ret = item.GetSlideshowPlayableContainedItemCount(count);
    if(ret)
        return count;
    else 
        return 0;
}

// ---------------------------------------------------------------------------
//  Retrieve the item is system item or not
// ---------------------------------------------------------------------------
//
bool GlxMLWrapperPrivate::isSystemItem( int aItemIndex )
{
    const TGlxMedia& item = iMediaList->Item( aItemIndex );
    TBool systemItem = false;
    item.GetSystemItem( systemItem );
    return systemItem ;
}

// ---------------------------------------------------------------------------
//  RetrieveItemUri
// ---------------------------------------------------------------------------
//
QString GlxMLWrapperPrivate::RetrieveItemUri(int aItemIndex)
{
	const TGlxMedia& item = iMediaList->Item( aItemIndex );
	const TDesC &uri = item.Uri();
    QString itemUri =  QString::fromUtf16(uri.Ptr(), uri.Length());
	return itemUri;
 }

// ---------------------------------------------------------------------------
//  RetrieveItemFrameCount
// ---------------------------------------------------------------------------
//
int GlxMLWrapperPrivate::RetrieveItemFrameCount(int aItemIndex)
    {
    const TGlxMedia& item = iMediaList->Item( aItemIndex );
    TInt frameCount(0);
    TBool frameCountStatus = item.GetFrameCount(frameCount);
    return frameCount;
    }

// ---------------------------------------------------------------------------
//  RetrieveItemDimension
// ---------------------------------------------------------------------------
//
QSize GlxMLWrapperPrivate::RetrieveItemDimension(int aItemIndex)
{
	const TGlxMedia& item = iMediaList->Item( aItemIndex );
	TSize size(0,0);
	TBool result = item.GetDimensions(size);
    QSize itemSize(size.iWidth,size.iHeight);
	return itemSize;
 }

// ---------------------------------------------------------------------------
//  RetrieveItemDate
// ---------------------------------------------------------------------------
//
QDate GlxMLWrapperPrivate::RetrieveItemDate(int index)
    {
   	GLX_LOG_INFO1("GlxMLWrapperPrivate::RetrieveItemDate %d",index);
    const TGlxMedia& item = iMediaList->Item( index );
    TTime dateValue;
    QDate date = QDate();
    TBool returnValue =item.GetDate(dateValue);
    if(returnValue)
        {
        GLX_LOG_INFO1("GlxMLWrapperPrivate::RetrieveItemDate %d",returnValue);
        TDateTime dateTime = dateValue.DateTime();
        date = QDate(dateTime.Year(),TInt(dateTime.Month()+1),(dateTime.Day()+1));
        }
     return date;
    }

// ---------------------------------------------------------------------------
//  RetrieveFsBitmap
// ---------------------------------------------------------------------------
//
CFbsBitmap* GlxMLWrapperPrivate::RetrieveBitmap(int aItemIndex)
    {
    GLX_LOG_INFO1("GlxMLWrapperPrivate::RetrieveBitmap %d",aItemIndex);
    const TGlxMedia& item = iMediaList->Item( aItemIndex );
    TMPXAttribute fsTnAttrib= TMPXAttribute(KGlxMediaIdThumbnail,
                GlxFullThumbnailAttributeId(ETrue, KFullScreenTNPTWidth,
                        KFullScreenTNPTHeight));
    const CGlxThumbnailAttribute* fsTnValue = item.ThumbnailAttribute(
            fsTnAttrib);
    if (fsTnValue)
        {
        GLX_LOG_INFO("GlxMLWrapperPrivate::RetrieveBitmap - returning FS bitmap");
        CFbsBitmap* fsTnBitmap = new (ELeave) CFbsBitmap;
        fsTnBitmap->Duplicate( fsTnValue->iBitmap->Handle());
        
        GLX_LOG_INFO2("GlxMLWrapperPrivate::RetrieveBitmap - bitmap height=%d, bitmap width=%d",
                fsTnBitmap->SizeInPixels().iHeight,fsTnBitmap->SizeInPixels().iWidth);

        return fsTnBitmap;
        }
    else // fetch grid Thumbnail
        {
        TMPXAttribute gridTnAttrib = TMPXAttribute(KGlxMediaIdThumbnail,
                        GlxFullThumbnailAttributeId(ETrue, KGridTNWIdth,
                                KGridTNHeight));
        const CGlxThumbnailAttribute* gridTnValue = item.ThumbnailAttribute(
                gridTnAttrib);
        if (gridTnValue)
            {
            GLX_LOG_INFO("GlxMLWrapperPrivate::RetrieveBitmap - returning Grid bitmap");
            CFbsBitmap* gridTnBitmap = new (ELeave) CFbsBitmap;
            gridTnBitmap->Duplicate( gridTnValue->iBitmap->Handle());
            
            GLX_LOG_INFO2("GlxMLWrapperPrivate::RetrieveBitmap - bitmap height=%d, bitmap width=%d",
                    gridTnBitmap->SizeInPixels().iHeight,gridTnBitmap->SizeInPixels().iWidth);
            return gridTnBitmap;
            }
        else
            {
            GLX_LOG_INFO("GlxMLWrapperPrivate::RetrieveBitmap - returning default bitmap");
            CFbsBitmap* defaultBitmap = new (ELeave) CFbsBitmap;
            return defaultBitmap;
            }
        }
}

// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//
 void GlxMLWrapperPrivate::HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* /*aList*/ )
     {
	 iMLWrapper->itemsAdded(aStartIndex,aEndIndex);
     }
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList )
    {
	Q_UNUSED(aList);
	iMLWrapper->itemsRemoved(aStartIndex,aEndIndex);
	TInt mediaCount = aList->Count();
    if (mediaCount <=0)
        {
        if(iMediaList->VisibleWindowIndex() > iMediaList->Count())
            {
            iMediaList->SetVisibleWindowIndexL(0);
            }               
        }
    else if (iMediaList->VisibleWindowIndex() > iMediaList->Count())
        {
        iMediaList->SetVisibleWindowIndexL(iMediaList->Count()-1);
        }
	}
// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// check for Grid PtFS and LsFs thumbnails. If any of them is available notify
// iMLWrapper
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleAttributesAvailableL( TInt aItemIndex, 
		const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList )
	{
	GLX_LOG_INFO1("### GlxMLWrapperPrivate::HandleAttributesAvailableL %d",aItemIndex);
	Q_UNUSED(aList);
    // char temp[100];
	// sprintf(temp, "execution time of update %d", aItemIndex);
	// PERFORMANCE_ADV( d1, temp) {
	if ( iGridContextActivated || iLsFsContextActivated || iPtFsContextActivated )
	    CheckGridTBAttribute(aItemIndex, aAttributes);
	if ( iPtFsContextActivated )
	    CheckPtFsTBAttribute(aItemIndex, aAttributes);
	if ( iLsFsContextActivated )
	    CheckLsFsTBAttribute(aItemIndex, aAttributes);
	if (iPtListContextActivated || iSelectionListContextActivated)
	    CheckListAttributes(aItemIndex, aAttributes);
	// }
	}
// ---------------------------------------------------------------------------
// CheckGridTBAttribute
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::CheckGridTBAttribute(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes)
	{
	TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue, KGridTNWIdth, KGridTNHeight ) ); //todo map icon size with actual mode
	TInt searchStatus = CheckTBAttributesPresenceandSanity(aItemIndex, aAttributes, thumbnailAttribute);
	if(searchStatus == KTBAttributeAvailable)
		{
		GLX_LOG_INFO1("### GlxMLWrapperPrivate::CheckGridTBAttribute %d",aItemIndex);
		iMLWrapper->handleReceivedIcon(aItemIndex, GlxTBContextGrid);
		}
	else if(searchStatus == KTBAttributeCorrupt)
		{
		iMLWrapper->handleIconCorrupt(aItemIndex);
		}
	}

// ---------------------------------------------------------------------------
// CheckPtFsTBAttribute
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::CheckPtFsTBAttribute(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes)
	{
	TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue, KFullScreenTNPTWidth, KFullScreenTNPTHeight ) ); //todo map icon size with actual mode
	TInt searchStatus = CheckTBAttributesPresenceandSanity(aItemIndex, aAttributes, thumbnailAttribute);
	if(searchStatus == KTBAttributeAvailable)
		{
		iMLWrapper->handleReceivedIcon(aItemIndex, GlxTBContextPtFs);
		}
	else if(searchStatus == KTBAttributeCorrupt)
		{
		iMLWrapper->handleIconCorrupt(aItemIndex);
		}
	}

// ---------------------------------------------------------------------------
// CheckLsFsTBAttribute
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::CheckLsFsTBAttribute(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes)
	{
	TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue, KFullScreenTNLSWidth, KFullScreenTNLSHeight ) ); //todo map icon size with actual mode
	TInt searchStatus = CheckTBAttributesPresenceandSanity(aItemIndex, aAttributes, thumbnailAttribute);
	if(searchStatus == KTBAttributeAvailable)
		{
		iMLWrapper->handleReceivedIcon(aItemIndex, GlxTBContextLsFs);
		}
	else if(searchStatus == KTBAttributeCorrupt)
		{
		iMLWrapper->handleIconCorrupt(aItemIndex);
		}
	}
// ---------------------------------------------------------------------------
// CheckLsFsTBAttribute
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::CheckListAttributes(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes)
	{
	GLX_LOG_INFO1("### GlxMLWrapperPrivate::CheckListAttributes %d",aItemIndex);
	TBool attribPresent = EFalse;
	TMPXAttribute titleAttrib(KMPXMediaGeneralTitle);
	TMPXAttribute subTitleAttrib(KGlxMediaCollectionPluginSpecificSubTitle);
    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
	    
   	const TGlxMedia& item = iMediaList->Item(aItemIndex);

    if (KErrNotFound != aAttributes.Find(titleAttrib, match))
    	{
		attribPresent = ETrue;
		GLX_LOG_INFO1("### GlxMLWrapperPrivate::CheckListAttributes title present %d",aItemIndex);
    	}

    if (KErrNotFound != aAttributes.Find(subTitleAttrib, match))
    	{
		attribPresent = ETrue;
		GLX_LOG_INFO1("### GlxMLWrapperPrivate::CheckListAttributes subtitle present %d",aItemIndex);
    	}
   	TMPXAttribute thumbnailAttribute(KGlxMediaIdThumbnail, 
            GlxFullThumbnailAttributeId( ETrue, KGridTNWIdth, KGridTNHeight ) ); //todo map icon size with actual mode
	TInt searchStatus = CheckTBAttributesPresenceandSanity(aItemIndex, aAttributes, thumbnailAttribute);
    if(searchStatus == KTBAttributeAvailable)
    	{
		attribPresent = ETrue;
		GLX_LOG_INFO1("### GlxMLWrapperPrivate::CheckListAttributes Thumbnail present %d",aItemIndex);
    	}
	if(attribPresent)
		{
		iMLWrapper->handleListItemAvailable(aItemIndex);
		}
	}
// ---------------------------------------------------------------------------
// CheckTBAttributesPresenceandSanity
// check if the requested attributes are present and their sanity
// ---------------------------------------------------------------------------
//
TInt GlxMLWrapperPrivate::CheckTBAttributesPresenceandSanity( TInt aItemIndex, 
		const RArray<TMPXAttribute>& aAttributes, TMPXAttribute aThumbnailAttribute )
		{
		GLX_LOG_INFO1("### GlxMLWrapperPrivate::CheckTBAttributesPresenceandSanity %d",aItemIndex);
		TInt searchStatus = KTBAttributeUnavailable;	
		const TGlxMedia& item = iMediaList->Item( aItemIndex );
		TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
		if (KErrNotFound != aAttributes.Find( aThumbnailAttribute, match ))
			{
			const CGlxThumbnailAttribute* value = item.ThumbnailAttribute(
				aThumbnailAttribute );
			TInt tnError = GlxErrorManager::HasAttributeErrorL(
						item.Properties(), KGlxMediaIdThumbnail );
			if(value)
				{
				searchStatus = KTBAttributeAvailable;
				}
			else if ( KErrNone != tnError && KErrNotSupported != tnError )
				{
				 searchStatus = KTBAttributeCorrupt;
				}
			}
		else
			{
			searchStatus = KTBAttributeUnavailable;
			}
		return searchStatus;	

	 }    
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, 
    TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList )
{
Q_UNUSED(aType);
Q_UNUSED(aNewIndex);
Q_UNUSED(aOldIndex);
Q_UNUSED(aList);
}
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList )
{
	Q_UNUSED(aIndex);
	Q_UNUSED(aSelected);
	Q_UNUSED(aList);
}
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList )
{
Q_UNUSED(aMessage);
Q_UNUSED(aList);
}
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleError( TInt aError )
	{
	Q_UNUSED(aError);
    GLX_LOG_INFO1("GlxMLWrapperPrivate::HandleError Error %d", aError);	
	
    for ( TInt i = 0; i < iMediaList->Count(); i++ )
        {
        const TGlxMedia& item = iMediaList->Item( i );
        TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                item.Properties(), KGlxMediaIdThumbnail );
        GLX_LOG_INFO1("GlxMLWrapperPrivate::HandleError thumbnailError %d", thumbnailError);
		if (thumbnailError)
		    {
            GLX_LOG_INFO1("GlxMLWrapperPrivate::HandleError Index %d", i);
			iMLWrapper->handleIconCorrupt(i);
            }
        }
	
	
	}
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
/*void GlxMLWrapperPrivate::HandleCommandCompleteL( CMPXCommand* aCommandResult, TInt aError, 
    MGlxMediaList* aList )
	{}*/ //todo add handle command complete
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleMediaL( TInt aListIndex, MGlxMediaList* aList )
{
	Q_UNUSED(aListIndex);
	Q_UNUSED(aList);
}
// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
void GlxMLWrapperPrivate::HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList )
{
	Q_UNUSED(aItemIndexes);
	Q_UNUSED(aList);
}

void GlxMLWrapperPrivate::HandlePopulatedL(MGlxMediaList* aList)
{
    Q_UNUSED(aList);
    iMLWrapper->handlepopulated();
}

// ---------------------------------------------------------------------------
// GetItemCount
// ---------------------------------------------------------------------------
//
HbIcon * GlxMLWrapperPrivate::convertFBSBitmapToHbIcon(CFbsBitmap* aBitmap)
{
	GLX_LOG_INFO1("### GlxMLWrapperPrivate::convertFBSBitmapToHbIcon %d", 0);
	aBitmap->LockHeap();
	TUint32 *tempData = aBitmap->DataAddress();
	uchar *data = (uchar *)(tempData);	
	int bytesPerLine = aBitmap->ScanLineLength(aBitmap->SizeInPixels().iWidth , aBitmap->DisplayMode());
	//QImage share the memory occupied by data
	QImage image(data, aBitmap->SizeInPixels().iWidth, aBitmap->SizeInPixels().iHeight, bytesPerLine, QImage::Format_RGB16);
	aBitmap->UnlockHeap();
	HbIcon* targetIcon = new HbIcon( QIcon( QPixmap::fromImage(image) ) );
	return targetIcon;
}

HbIcon * GlxMLWrapperPrivate::convertFBSBitmapToHbIcon(CFbsBitmap* aBitmap, TInt itemWidth, TInt itemHeight)
{
    GLX_LOG_INFO1("### GlxMLWrapperPrivate::convertFBSBitmapToHbIcon 1 %d", 0);
    
    aBitmap->LockHeap();
    TUint32 *tempData = aBitmap->DataAddress();
    uchar *data = (uchar *)(tempData);  
    int bytesPerLine = aBitmap->ScanLineLength(aBitmap->SizeInPixels().iWidth , aBitmap->DisplayMode());
    //QImage share the memory occupied by data
    QImage image(data, aBitmap->SizeInPixels().iWidth, aBitmap->SizeInPixels().iHeight, bytesPerLine, QImage::Format_RGB16);
        
    QPixmap pixmap = QPixmap::fromImage(image);
    if ( aBitmap->SizeInPixels().iWidth > itemWidth || aBitmap->SizeInPixels().iHeight > itemHeight ) {
        pixmap = pixmap.scaled( itemWidth, itemHeight, Qt::KeepAspectRatio );
    }
    
    aBitmap->UnlockHeap();
    HbIcon* targetIcon = new HbIcon( QIcon( pixmap ) );
    return targetIcon;
}

 // ---------------------------------------------------------------------------
 // GetFocusIndex
 // ---------------------------------------------------------------------------
 //
 int GlxMLWrapperPrivate::GetFocusIndex() const
     {
     return (iMediaList->FocusIndex());
     }
 // ---------------------------------------------------------------------------
 // GetFocusIndex
 // ---------------------------------------------------------------------------
 //
 void GlxMLWrapperPrivate::SetFocusIndex(int aItemIndex)
     {
     iMediaList->SetFocusL(NGlxListDefs::EAbsolute,aItemIndex);
     }

 // ---------------------------------------------------------------------------
 // SetFocusIndex
 // ---------------------------------------------------------------------------
 //
 void GlxMLWrapperPrivate::SetSelectedIndex(int aItemIndex)
     {
     iMediaList->SetSelectedL(aItemIndex,ETrue);
     }

 // ---------------------------------------------------------------------------
 // GetVisibleWindowIndex
 // ---------------------------------------------------------------------------
 //
 int GlxMLWrapperPrivate::GetVisibleWindowIndex()
     {
     return (iMediaList->VisibleWindowIndex());
     }
 // ---------------------------------------------------------------------------
 // SetVisibleWindowIndex
 // ---------------------------------------------------------------------------
 //
 void GlxMLWrapperPrivate::SetVisibleWindowIndex(int aItemIndex)
     {
     iMediaList->SetVisibleWindowIndexL(aItemIndex);
     }

// -----------------------------------------------------------------------------
// HandleTitleAvailableL
// -----------------------------------------------------------------------------
//	
void GlxMLWrapperPrivate::HandleTitleAvailableL(
        const TDesC& aTitle)
    {
    iViewTitle = QString::fromUtf16(aTitle.Ptr(), aTitle.Length());	
	iMLWrapper->handleTitleAvailable(iViewTitle);
    RDebug::Print(_L("GlxPhotos: GlxMLWrapperPrivate : TITLE =-%S"),&aTitle);
	}

QString GlxMLWrapperPrivate::RetrieveViewTitle()
    {
    return iViewTitle;
    }
	 
bool GlxMLWrapperPrivate::IsPopulated()
    {
    return iMediaList->IsPopulated();
    }

	 

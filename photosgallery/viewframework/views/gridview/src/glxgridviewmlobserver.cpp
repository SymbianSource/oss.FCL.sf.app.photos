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
*  Description : Grid view Observers
*
*/


// INCLUDE FILES

// Ganes Headers
#include <ganes/HgItem.h>
#include <ganes/HgGrid.h>                               //Hg Grid Widget
//#include <hg/hgcontextutility.h>
#include <gulicon.h>

//Gallery Headers
#include <glxtracer.h>                                  // For Tracer
#include <glxlog.h>                                     // For Glx Logging
#include <glxuiutility.h>                               // For UiUtility instance
#include <glxicons.mbg>                                 // For Corrupted and not created icons
#include <glxerrormanager.h>                            // For CGlxErrormanager
#include <glxuistd.h>
#include <mglxmedialist.h>                              // CGlxMedialist
#include <glxthumbnailattributeinfo.h>                  // KGlxMediaIdThumbnail
#include <glxdrmutility.h>                              // DRM utility class to provide DRM-related functionality
#include <mpxmediadrmdefs.h>                            // KMPXMediaDrmProtected
#include <glxcollectionplugindownloads.hrh>
#include <glxgridviewdata.rsg>                          // Gridview resource

// Framework
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>

#include <bldvariant.hrh>                               // For feature constants
#include <featmgr.h>		                            // Feature Manager
#include <caf/caferr.h>
#include <AknUtils.h>

#include "glxgridviewmlobserver.h"

const TInt KRecreateGridSize(100); //minimum no of items added to trigger recreate grid
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxGridViewMLObserver* CGlxGridViewMLObserver::NewL(
        MGlxMediaList& aMediaList, CHgGrid* aHgGrid)
    {
    TRACER("CGlxGridViewMLObserver::NewLC()");
    CGlxGridViewMLObserver* self = 
            new (ELeave) CGlxGridViewMLObserver(aMediaList, aHgGrid);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxGridViewMLObserver::CGlxGridViewMLObserver(MGlxMediaList& aMediaList,
        CHgGrid* aHgGrid ) : iMediaList(aMediaList), iHgGrid(aHgGrid)
    {
    TRACER("CGlxGridViewMLObserver::CGlxGridViewMLObserver");
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::ConstructL()
    {
    TRACER("CGlxGridViewMLObserver::ConstructL");
    iMediaList.AddMediaListObserverL(this);
    // For DRm Utility
    iDRMUtility = CGlxDRMUtility::InstanceL();

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    iGridIconSize = uiUtility->GetGridIconSize();
    //Get the HgContextUtility instance
//    iContextUtility = uiUtility->ContextUtility();
    iItemsPerPage = uiUtility->VisibleItemsInPageGranularityL();
    uiUtility->Close() ;
    
   	iDownloadsPlugin = EFalse;
   	
    CMPXCollectionPath* path = iMediaList.PathLC( NGlxListDefs::EPathParent );
    if (path->Id() == KGlxCollectionPluginDownloadsImplementationUid)
    	{
    	iDownloadsPlugin = ETrue;
    	}
    CleanupStack::PopAndDestroy(path);

    iQualityTnAttrib = TMPXAttribute (KGlxMediaIdThumbnail, 
        GlxFullThumbnailAttributeId( ETrue,  iGridIconSize.iWidth, 
                iGridIconSize.iHeight ) );

    iSpeedTnAttrib = TMPXAttribute (KGlxMediaIdThumbnail, 
        GlxFullThumbnailAttributeId( EFalse,  iGridIconSize.iWidth, 
                iGridIconSize.iHeight ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxGridViewMLObserver::~CGlxGridViewMLObserver()
    {
    TRACER("CGlxGridViewMLObserver::~CGlxGridViewMLObserver");
    iMediaList.RemoveMediaListObserver( this );
    if (iDRMUtility)
        {
        iDRMUtility->Close();
        }
	iModifiedIndexes.Reset();
    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxGridViewMLObserver::HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, 
     MGlxMediaList* aList )
    {
    TRACER("CGlxGridViewMLObserver::HandleItemAddedL()");
    GLX_DEBUG3("CGlxGridViewMLObserver::HandleItemAddedL() aStartIndex(%d),"
            " aEndIndex(%d)", aStartIndex, aEndIndex);

    if (iHgGrid)
        {
        if ((aEndIndex - aStartIndex) >= KRecreateGridSize)
            {
            iHgGrid->ResizeL(aList->Count());
            }
        else
            {
            for (TInt i = aStartIndex; i<= aEndIndex; i++)
                {
                iHgGrid->InsertItem(CHgItem::NewL(), i);
                }
            }
        }
    // Setting the initial focus for all grid views except downloads,
    // for downloads it is already set.
	TInt focusIndex = aList->FocusIndex();
    iHgGrid->SetSelectedIndex(focusIndex);
    
    // if the Medialist has any item, set the First index context to Hg Context Utility
//    TGlxMedia item = aList->Item( focusIndex );
//    iContextUtility->PublishPhotoContextL(item.Uri());
    }

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::HandleItemRemovedL( TInt aStartIndex, 
        TInt aEndIndex, MGlxMediaList* aList )
    {
    TRACER("CGlxGridViewMLObserver::HandleItemRemovedL()");
    if (iHgGrid)
        {
        TInt mediaCount = aList->Count();   
        
        for (TInt i = aEndIndex; i>= aStartIndex; i--)
            {
            iHgGrid->RemoveItem(i);
            }

	    // If the last item is also deleted, this refreshes the view
	    if (mediaCount <=0)
	        {
            if(iMediaList.VisibleWindowIndex() > iMediaList.Count())
                {
                iMediaList.SetVisibleWindowIndexL(0);
                }	        	
			//This is done since the Hg doesnot refresh the screen
			//when we remove all the items from the grid
	        iHgGrid->DrawDeferred(); 
	        iHgGrid->Reset();
			return;
	        }
	    else if (iMediaList.VisibleWindowIndex() > iMediaList.Count())
            {
            iMediaList.SetVisibleWindowIndexL(iMediaList.Count()-1);
            }
        iHgGrid->RefreshScreen(iHgGrid->FirstIndexOnScreen()); 			
        }
    }

// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::HandleAttributesAvailableL( TInt aItemIndex, 
    const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxGridViewMLObserver::HandleAttributesAvailableL()");
    GLX_LOG_INFO1("CGlxGridViewMLObserver::HandleAttributesAvailableL "
                                               "aItemIndex(%d)", aItemIndex);
    if (!iHgGrid)
        {
        return;
        }

    TInt mediaCount = iMediaList.Count();
    const TGlxMedia& item = iMediaList.Item( aItemIndex );
    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
    
    if (KErrNotFound != aAttributes.Find( iQualityTnAttrib, match ) ||
        KErrNotFound != aAttributes.Find( iSpeedTnAttrib, match ))
        {
        TFileName resFile(KDC_APP_BITMAP_DIR);
        resFile.Append(KGlxIconsFilename);
        TSize setSize = CHgGrid::PreferredImageSize();
        TIconInfo icon;

        const CGlxThumbnailAttribute* qualityTn = item.ThumbnailAttribute(
                                                          iQualityTnAttrib );

        const CGlxThumbnailAttribute* speedTn = item.ThumbnailAttribute(
                                                            iSpeedTnAttrib );

        TInt tnError = GlxErrorManager::HasAttributeErrorL(
                          item.Properties(), KGlxMediaIdThumbnail );

        if (qualityTn)
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            bitmap->Duplicate( qualityTn->iBitmap->Handle());
            AknIconUtils::SetSize(bitmap, setSize);
            iHgGrid->ItemL(aItemIndex).SetIcon(CGulIcon::NewL(bitmap));
            GLX_LOG_INFO1("### CGlxGridViewMLObserver::HandleAttributesAvailableL"
                    " qualityTn-Index is %d",aItemIndex);
            }
        else if (speedTn)
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            bitmap->Duplicate( speedTn->iBitmap->Handle());
            AknIconUtils::SetSize(bitmap, setSize);
            iHgGrid->ItemL(aItemIndex).SetIcon(CGulIcon::NewL(bitmap));
            GLX_LOG_INFO1("### CGlxGridViewMLObserver::HandleAttributesAvailableL"
                    " speedTn-Index is %d",aItemIndex);
            }
        else if (item.GetIconInfo(icon))
            {  
            CFbsBitmap* bitmap = AknIconUtils::CreateIconL(icon.bmpfile, icon.bitmapId);
            AknIconUtils::SetSize(bitmap, setSize );
            iHgGrid->ItemL(aItemIndex).SetIcon(CGulIcon::NewL(bitmap));
            GLX_LOG_INFO1("### CGlxGridViewMLObserver::HandleAttributesAvailableL "
                    "GetIconInfo-Index is %d",aItemIndex);
            }
        else if ( KErrNone != tnError && KErrNotSupported != tnError &&
                            KErrArgument != tnError )
            {
            CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
                                 EMbmGlxiconsQgn_prop_image_corrupted);
            AknIconUtils::SetSize(bitmap, setSize);
            iHgGrid->ItemL(aItemIndex).SetIcon(CGulIcon::NewL(bitmap));
            }
        }
    
    //Now Update the items with the DRM/video icon and  date/time 
    UpdateItemsL(aItemIndex,aAttributes);
    
    //Now refresh the screen based on the attributes available index
    RefreshScreen(aItemIndex,aAttributes);
    }
    
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::HandleFocusChangedL( NGlxListDefs::
    TFocusChangeType /*aType*/, TInt aNewIndex, TInt /*aOldIndex*/, 
    MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxGridViewMLObserver::HandleFocusChangedL()");
    iHgGrid->SetSelectedIndex(aNewIndex);
    iHgGrid->RefreshScreen(aNewIndex); 
/*    if (aList->Count())
        {
        //  This us to set the context to HG Teleport
        TGlxMedia item = iMediaList.Item( aNewIndex );
        iContextUtility->PublishPhotoContextL(item.Uri());
        }*/
    }

// ----------------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::HandleItemSelectedL(TInt /*aIndex*/, 
    TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxGridViewMLObserver::HandleItemSelectedL");
    }

// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
 void CGlxGridViewMLObserver::HandleMessageL( const CMPXMessage& /*aMessage*/, 
    MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxGridViewMLObserver::HandleMessageL()");
    }
  
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxGridViewMLObserver::HandleError( TInt /*aError*/ ) 
    {
    TRACER("CGlxGridViewMLObserver::HandleError()");
    TRAP_IGNORE(HandleErrorL());
    }

// ----------------------------------------------------------------------------
// HandleErrorL
// ----------------------------------------------------------------------------
//
void CGlxGridViewMLObserver::HandleErrorL()
    {
    TRACER("CGlxGridViewMLObserver::HandleErrorL()");
    for ( TInt i = 0; i < iMediaList.Count(); i++ )
        {
        const TGlxMedia& item = iMediaList.Item( i );
        TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                item.Properties(), KGlxMediaIdThumbnail );

		if( thumbnailError== KErrCANoRights)
			{
			/*fix for EABI-7RKHDG
			 * this is a safe code added to show default
			 * TNM returns -17452 in case SD DRM files
			 */
			TFileName resFile(KDC_APP_BITMAP_DIR);
		    resFile.Append(KGlxIconsFilename);
		    CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
		    		EMbmGlxiconsQgn_prop_image_notcreated);
            AknIconUtils::SetSize(bitmap, CHgGrid::PreferredImageSize());
		    iHgGrid->ItemL(i).SetIcon(CGulIcon::NewL(bitmap),
		                        CHgItem::EHgItemFlagsDrmRightsExpired);
			}
		else if (thumbnailError)
		    {
            TFileName resFile(KDC_APP_BITMAP_DIR);
            resFile.Append(KGlxIconsFilename);

            CFbsBitmap* bitmap = AknIconUtils::CreateIconL(resFile,
                    EMbmGlxiconsQgn_prop_image_corrupted);

           	//@ Fix for EABI-7RJA8C, Changes for HG grid for corrupted icon.
            AknIconUtils::SetSize(bitmap, CHgGrid::PreferredImageSize() );

            iHgGrid->ItemL(i).SetIcon(CGulIcon::NewL(bitmap),
                    CHgItem::EHgItemFlagsNone);
            }
        }
    iHgGrid->RefreshScreen(iHgGrid->FirstIndexOnScreen());    
    }
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, 
    TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxGridViewMLObserver::HandleCommandCompleteL()");
    }
  
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxGridViewMLObserver::HandleMediaL()");
    }

// ----------------------------------------------------------------------------
// HandlePopulatedL
// ----------------------------------------------------------------------------
//
void CGlxGridViewMLObserver::HandlePopulatedL( MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxGridViewMLObserver::HandlePopulatedL()");
    if (iHgGrid)
        {
        // Setting the Empty Text
        HBufC* emptyText = 
                  StringLoader::LoadLC(R_GRID_EMPTY_VIEW_TEXT);
        iHgGrid->SetEmptyTextL(*emptyText);
        CleanupStack::PopAndDestroy(emptyText);
        GLX_DEBUG2("GridMLObserver::HandlePopulatedL() iMediaList.Count()=%d",
                                                          iMediaList.Count());
           
        
        if (iMediaList.Count() <= 0)
            {
            GLX_DEBUG1("GridMLObserver::HandlePopulatedL() - SetEmptyTextL()");
            iHgGrid->DrawNow();
            }
        }
    }
  
// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::HandleItemModifiedL(const RArray<TInt>& aItemIndexes,
    MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxGridViewMLObserver::HandleItemModifiedL()");
    for(TInt index = 0;index<aItemIndexes.Count();index++)
        {
        TInt modifiedIndex = aItemIndexes[index];
        iModifiedIndexes.AppendL(modifiedIndex);
        }  
    }
                  
// ----------------------------------------------------------------------------
// HasRelevantThumbnailAttribute
// ----------------------------------------------------------------------------
//
TBool CGlxGridViewMLObserver::HasRelevantThumbnail(TInt aIndex)
    {
    TRACER("CGlxGridViewMLObserver::HasRelevantThumbnail()");
    const TGlxMedia& item = iMediaList.Item( aIndex );
    const CGlxThumbnailAttribute* qualityTn = item.ThumbnailAttribute(
                                                      iQualityTnAttrib );
    const CGlxThumbnailAttribute* speedTn = item.ThumbnailAttribute(
                                                       iSpeedTnAttrib );
    if ( qualityTn || speedTn )
        {
        GLX_DEBUG1("GridMLObserver::HasRelevantThumbnail() - TN avail");                 
        return ETrue;
        }
    return EFalse;
    }


// ----------------------------------------------------------------------------
// RefreshScreen
// ----------------------------------------------------------------------------
// 
void CGlxGridViewMLObserver::RefreshScreen(TInt aItemIndex,
                                      const RArray<TMPXAttribute>& aAttributes)
    {
    TInt mediaCount = iMediaList.Count();
    TInt firstIndex = iHgGrid->FirstIndexOnScreen();
    firstIndex = (firstIndex<0 ? 0 : firstIndex);
    TInt lastOnScreen = firstIndex + iHgGrid->ItemsOnScreen() - 1;
    lastOnScreen = (lastOnScreen >mediaCount-1? mediaCount-1:lastOnScreen);
    if (mediaCount < iItemsPerPage || aItemIndex == firstIndex)
        {
        if (aItemIndex == firstIndex && HasRelevantThumbnail(firstIndex))
            {
            GLX_DEBUG2("## GridMLObserver::HandleAttributesAvailableL()"
                     " RefreshScreen - firstIndex(%d)", firstIndex);
            iHgGrid->RefreshScreen(firstIndex);
            }
        }
    else if (aItemIndex > firstIndex && aItemIndex <= lastOnScreen)
        {
        TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
        if (KErrNotFound != aAttributes.Find(iQualityTnAttrib, match) ||
            KErrNotFound != aAttributes.Find(iSpeedTnAttrib, match) )
            {
            if ( HasRelevantThumbnail(firstIndex) )
                {
                if ( HasRelevantThumbnail(lastOnScreen) )
                    {
                    GLX_DEBUG2("GridMLObserver::HandleAttributesAvailableL()"
                        " RefreshScreen - aItemIndex(%d)", aItemIndex);					
                    iHgGrid->RefreshScreen(aItemIndex);
                    }
                else if (aItemIndex == lastOnScreen)
                    {
                    GLX_DEBUG2("GridMLObserver::HandleAttributesAvailableL()"
                           " RefreshScreen - lastOnScreen(%d)", lastOnScreen); 
                    iHgGrid->RefreshScreen(lastOnScreen);
                    }
                }
            }
        }
    if (iModifiedIndexes.Count() > 0)        
        {
        for(TInt index = 0;index<iModifiedIndexes.Count();index++)
            {
            if (iModifiedIndexes[index] == aItemIndex && 
			                     HasRelevantThumbnail(aItemIndex))
                {
                GLX_DEBUG2("GridMLObserver::HandleAttributesAvailableL()"
                           " RefreshScreen - modified index(%d)", aItemIndex);
                iHgGrid->RefreshScreen(aItemIndex);
                iModifiedIndexes.Remove(index);
                iModifiedIndexes.Compress();
                }
            }
        }
    }
    
    
// ----------------------------------------------------------------------------
// UpdateItemsL
// ----------------------------------------------------------------------------
// 
void CGlxGridViewMLObserver::UpdateItemsL (TInt aItemIndex, 
                                      const RArray<TMPXAttribute>& aAttributes)
    {
    TInt mediaCount = iMediaList.Count();
    const TGlxMedia& item = iMediaList.Item( aItemIndex );
    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
    
    if (aAttributes.Find(KMPXMediaDrmProtected, match) != KErrNotFound)
        {
        if (item.IsDrmProtected())
            {
            const TDesC& uri = item.Uri();
            if( uri.Length() > 0)
                {
                if(iDRMUtility->IsForwardLockedL(uri))
                    {
                    /*
                     * fix for EABI-7RKHDG
                     * to show the invalid DRM icon
                     */
                    TMPXGeneralCategory  cat = item.Category();                  
                    TBool checkViewRights = (cat==EMPXImage);
                    
                    if(iDRMUtility->CheckOpenRightsL(uri, checkViewRights))
                        {
                        iHgGrid->ItemL(aItemIndex).SetFlags(
                                CHgItem::EHgItemFlagsDrmRightsValid);
                        }
                     else
                        {
                        iHgGrid->ItemL(aItemIndex).SetFlags(
                                CHgItem::EHgItemFlagsDrmRightsExpired);
                        }
                    }
                else 
                    {
                    TMPXGeneralCategory  cat = item.Category();                  
                    TBool checkViewRights = (cat==EMPXImage);
                    
                    if(iDRMUtility->CheckOpenRightsL(uri, checkViewRights))
                        {
                        iHgGrid->ItemL(aItemIndex).SetFlags(
                                CHgItem::EHgItemFlagsDrmRightsValid);
                        }
                     else
                        {
                        iHgGrid->ItemL(aItemIndex).SetFlags(
                                CHgItem::EHgItemFlagsDrmRightsExpired);
                        }
                    }
                }
            }
        }
    
    if (aAttributes.Find(KMPXMediaGeneralDate, match) != KErrNotFound)
        {
        TTime time(0);
        if (item.GetDate(time))
            {
            iHgGrid->ItemL(aItemIndex).SetTime(time);
            }
        
        // Sets up TLS, must be done before FeatureManager is used.
        FeatureManager::InitializeLibL();

        if (FeatureManager::FeatureSupported(KFeatureIdSeamlessLinks))
            {
            if (iDownloadsPlugin && mediaCount > iHgGrid->ItemsOnScreen()
                    && aItemIndex == 2)
                {
                if (iMediaList.Item(0).IsStatic())
                    {
                    iHgGrid->ItemL(0).SetTime(time); // Image Downloads link Icon	
                    }
                if (iMediaList.Item(1).IsStatic())
                    {
                    iHgGrid->ItemL(1).SetTime(time); // Video Downloads link Icon
                    }
                }
            }
        
        // Frees the TLS. Must be done after FeatureManager is used.
        FeatureManager::UnInitializeLib(); 
        }
    
    if (aAttributes.Find(KMPXMediaGeneralCategory, match) != KErrNotFound)
        {
        if (item.Category() == EMPXVideo)
            {
            iHgGrid->ItemL(aItemIndex).SetFlags(CHgItem::EHgItemFlagsVideo);
            }
        }    
    }
    
    

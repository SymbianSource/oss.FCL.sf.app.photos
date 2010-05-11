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
#include <gulicon.h>

//Photos Headers
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
#include <glxgeneraluiutilities.h>

// Framework
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>
#include <bldvariant.hrh>                               // For feature constants
#include <featmgr.h>		                            // Feature Manager
#include <caf/caferr.h>
#include <AknUtils.h>

#include "glxgridviewmlobserver.h"

const TInt KRecreateGridSize(5); //minimum no of items added to trigger recreate grid
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxGridViewMLObserver* CGlxGridViewMLObserver::NewL(
        MGlxMediaList& aMediaList, CHgGrid* aHgGrid,
        TGlxFilterItemType aFilterType)
    {
    TRACER("CGlxGridViewMLObserver::NewL()");
    CGlxGridViewMLObserver* self = new (ELeave) CGlxGridViewMLObserver(
            aMediaList, aHgGrid, aFilterType);
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
        CHgGrid* aHgGrid, TGlxFilterItemType aFilterType) :
    iMediaList(aMediaList), iHgGrid(aHgGrid), iFilterType(aFilterType)
    {
    TRACER("CGlxGridViewMLObserver::CGlxGridViewMLObserver()");
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//  
void CGlxGridViewMLObserver::ConstructL()
    {
    TRACER("CGlxGridViewMLObserver::ConstructL()");
    iMediaList.AddMediaListObserverL(this);
    // For DRm Utility
    iDRMUtility = CGlxDRMUtility::InstanceL();

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    iGridIconSize = uiUtility->GetGridIconSize();
    iItemsPerPage = uiUtility->VisibleItemsInPageGranularityL();
    uiUtility->Close() ;
    
    iQualityTnAttrib = TMPXAttribute (KGlxMediaIdThumbnail, 
        GlxFullThumbnailAttributeId( ETrue,  iGridIconSize.iWidth, 
                iGridIconSize.iHeight ) );

    iSpeedTnAttrib = TMPXAttribute (KGlxMediaIdThumbnail, 
        GlxFullThumbnailAttributeId( EFalse,  iGridIconSize.iWidth, 
                iGridIconSize.iHeight ) );
    
    iDiskErrorIntimated = EFalse ;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxGridViewMLObserver::~CGlxGridViewMLObserver()
    {
    TRACER("CGlxGridViewMLObserver::~CGlxGridViewMLObserver()");
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

    if (!iHgGrid)
        {
        return;
        }

    if ((aEndIndex - aStartIndex) > KRecreateGridSize)
        {
        iHgGrid->ResizeL(aList->Count());
        }
    else
        {
        for (TInt i = aStartIndex; i <= aEndIndex; i++)
            {
            iHgGrid->InsertItem(CHgItem::NewL(), i);
            }
        }

    // Setting the initial focus for all grid views except downloads,
    // for downloads it is already set.
    TInt focusIndex = aList->FocusIndex();
    iHgGrid->SetSelectedIndex(focusIndex);
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

    TIdentityRelation< TMPXAttribute > match ( &TMPXAttribute::Match );
    if (KErrNotFound != aAttributes.Find( iQualityTnAttrib, match ) ||
        KErrNotFound != aAttributes.Find( iSpeedTnAttrib, match ))
        {
        const TGlxMedia& item = iMediaList.Item( aItemIndex );
        TFileName resFile(KDC_APP_BITMAP_DIR);
        resFile.Append(KGlxIconsFilename);
        TSize setSize = CHgGrid::PreferredImageSize();

        const CGlxThumbnailAttribute* qualityTn = item.ThumbnailAttribute(
                                                          iQualityTnAttrib );

        const CGlxThumbnailAttribute* speedTn = item.ThumbnailAttribute(
                                                            iSpeedTnAttrib );

        TInt tnError = GlxErrorManager::HasAttributeErrorL(
                          item.Properties(), KGlxMediaIdThumbnail );
        GLX_DEBUG4("GlxGridMLObs::HandleAttributesAvailableL() tnError(%d)"
                "qualityTn(%x) and speedTn(%x)",tnError, qualityTn, speedTn );

        if (qualityTn)
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            bitmap->Duplicate(qualityTn->iBitmap->Handle());
            iHgGrid->ItemL(aItemIndex).SetIcon(CGulIcon::NewL(bitmap));
            GLX_LOG_INFO1("### CGlxGridViewMLObserver::HandleAttributesAvailableL"
                    " qualityTn-Index is %d",aItemIndex);
            }
        else if (speedTn)
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            bitmap->Duplicate(speedTn->iBitmap->Handle());
            iHgGrid->ItemL(aItemIndex).SetIcon(CGulIcon::NewL(bitmap));
            GLX_LOG_INFO1("### CGlxGridViewMLObserver::HandleAttributesAvailableL"
                    " speedTn-Index is %d",aItemIndex);
            }
        else if ( KErrNone != tnError && KErrNotSupported != tnError &&
                            KErrArgument != tnError )
            {
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), KAknsIIDNone,
                    bitmap, mask, resFile,
                    EMbmGlxiconsQgn_prop_image_corrupted,
                    EMbmGlxiconsQgn_prop_image_corrupted_mask);
            __ASSERT_DEBUG(bitmap, Panic(EGlxPanicNullPointer));
            __ASSERT_DEBUG(mask, Panic(EGlxPanicNullPointer));
            
            AknIconUtils::SetSize(bitmap, CHgGrid::PreferredImageSize(),
                    EAspectRatioPreservedAndUnusedSpaceRemoved);
            AknIconUtils::SetSize(mask, CHgGrid::PreferredImageSize(),
                    EAspectRatioPreservedAndUnusedSpaceRemoved);

            iHgGrid->ItemL(aItemIndex).SetIcon(CGulIcon::NewL(bitmap, mask));
            CleanupStack::Pop(mask);
            CleanupStack::Pop(bitmap);
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
    TRACER("CGlxGridViewMLObserver::HandleItemSelectedL()");
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

    TInt bitmapId  = EMbmGlxiconsQgn_prop_image_corrupted;
    TInt maskId = EMbmGlxiconsQgn_prop_image_corrupted_mask;	
    TInt flags     = CHgItem::EHgItemFlagsNone ;
    
    for ( TInt i = 0; i < iMediaList.Count(); i++ )
        {
        const TGlxMedia& item = iMediaList.Item( i );
        TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                item.Properties(), KGlxMediaIdThumbnail );
        
        if (KErrNone != thumbnailError)
            {
            switch (thumbnailError)
                {
                case KErrCANoRights:;   // Err id = -17452
                    bitmapId  = EMbmGlxiconsQgn_prop_image_notcreated;
					maskId = EMbmGlxiconsQgn_prop_image_notcreated_mask;
                    flags     = CHgItem::EHgItemFlagsDrmRightsExpired;
                    break;
                case KErrDiskFull:
                    bitmapId  = EMbmGlxiconsQgn_prop_image_notcreated;
					maskId = EMbmGlxiconsQgn_prop_image_notcreated_mask;
                    flags     = CHgItem::EHgItemFlagsNone ;
                    if (!iDiskErrorIntimated)
                        {
                        DisplayErrorNoteL(KErrDiskFull);
                        }
                    break;
                default:
                    bitmapId  = EMbmGlxiconsQgn_prop_image_corrupted;
					maskId = EMbmGlxiconsQgn_prop_image_corrupted_mask;
                    flags     = CHgItem::EHgItemFlagsNone ;
                    break;  
                }
            
            TFileName resFile(KDC_APP_BITMAP_DIR);
            resFile.Append(KGlxIconsFilename);

            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), KAknsIIDNone,
                    bitmap, mask, resFile, bitmapId, maskId);
            __ASSERT_DEBUG(bitmap, Panic(EGlxPanicNullPointer));
            __ASSERT_DEBUG(mask, Panic(EGlxPanicNullPointer));

            AknIconUtils::SetSize(bitmap, CHgGrid::PreferredImageSize(),
                    EAspectRatioPreservedAndUnusedSpaceRemoved);
            AknIconUtils::SetSize(mask, CHgGrid::PreferredImageSize(),
                    EAspectRatioPreservedAndUnusedSpaceRemoved);

            iHgGrid->ItemL(i).SetIcon(CGulIcon::NewL(bitmap, mask), flags);
            CleanupStack::Pop(mask); 
            CleanupStack::Pop(bitmap); 
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
        if (iFilterType == EGlxFilterImage)
            {
            HBufC* emptyText = StringLoader::LoadLC(
                    R_GRID_EMPTY_VIEW_TEXT_IMAGE);
            iHgGrid->SetEmptyTextL(*emptyText);
            CleanupStack::PopAndDestroy(emptyText);
            }
        else if (iFilterType == EGlxFilterVideo)
            {
            HBufC* emptyText = StringLoader::LoadLC(
                    R_GRID_EMPTY_VIEW_TEXT_VIDEO);
            iHgGrid->SetEmptyTextL(*emptyText);
            CleanupStack::PopAndDestroy(emptyText);
            }
        else
            {
            HBufC* emptyText = StringLoader::LoadLC(R_GRID_EMPTY_VIEW_TEXT);
            iHgGrid->SetEmptyTextL(*emptyText);
            CleanupStack::PopAndDestroy(emptyText);
            }

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
    TRACER("CGlxGridViewMLObserver::RefreshScreen()");
    GLX_DEBUG2("CGlxGridViewMLObserver::RefreshScreen(%d)", aItemIndex);
    TInt mediaCount = iMediaList.Count();
    TInt firstIndex = iHgGrid->FirstIndexOnScreen();
    firstIndex = (firstIndex<0 ? 0 : firstIndex);
    TInt lastOnScreen = firstIndex + iHgGrid->ItemsOnScreen() - 1;
    lastOnScreen = (lastOnScreen >mediaCount-1? mediaCount-1:lastOnScreen);
    if (mediaCount < iItemsPerPage || aItemIndex == firstIndex)
        {
        if (aItemIndex == firstIndex && HasRelevantThumbnail(firstIndex)
                && HasRelevantThumbnail(lastOnScreen))
            {
            GLX_DEBUG2("## GridMLObserver::HandleAttributesAvailableL()"
                     " RefreshScreen - firstIndex(%d)", firstIndex);
            iHgGrid->RefreshScreen(firstIndex);
            }
        else if (aItemIndex > firstIndex && aItemIndex <= lastOnScreen)
            {
            if (HasRelevantThumbnail(lastOnScreen) && HasRelevantThumbnail(
                    firstIndex))
                {
                GLX_DEBUG2("GridMLObserver::HandleAttributesAvailableL()"
                        " RefreshScreen - aItemIndex(%d)", aItemIndex);					
                iHgGrid->RefreshScreen(aItemIndex);
                }
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
void CGlxGridViewMLObserver::UpdateItemsL(TInt aItemIndex, 
                                      const RArray<TMPXAttribute>& aAttributes)
    {
    TRACER("CGlxGridViewMLObserver::UpdateItemsL()");
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
                    
                    if(iDRMUtility->ItemRightsValidityCheckL(uri, checkViewRights))
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
                    
                    if(iDRMUtility->ItemRightsValidityCheckL(uri, checkViewRights))
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
        }
    
    if (aAttributes.Find(KMPXMediaGeneralCategory, match) != KErrNotFound)
        {
        if (item.Category() == EMPXVideo)
            {
            iHgGrid->ItemL(aItemIndex).SetFlags(CHgItem::EHgItemFlagsVideo);
            }
        }    
    }
    
// ----------------------------------------------------------------------------
// DisplayErrorNoteL
// ----------------------------------------------------------------------------
// 
void CGlxGridViewMLObserver::DisplayErrorNoteL(TInt aError)
    {
    TRACER("CGlxGridViewMLObserver::DisplayErrorNoteL()");
    GLX_LOG_INFO1("CGlxGridViewMLObserver::DisplayErrorNoteL() "
            " aError(%d)", aError);
    GlxGeneralUiUtilities::ShowErrorNoteL(aError);
    iDiskErrorIntimated = ETrue;
    return;
    }

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
* Description:    Manager of visual lists
*
*/




/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */

#include "glxdrmiconmanager.h"

#include <data_caging_path_literals.hrh>
#include <glxdrmutility.h>
#include <glxuiutility.h>
#include <glxicons.mbg> // icons
#include <glxtracer.h>
#include <glxlog.h>
#include <glxtexturemanager.h>
#include <glxuistd.h>
#include <alf/alftexture.h>
#include <alf/alfvisual.h>
#include <mglxmedialist.h>
#include <mglxvisuallist.h>
#include <mpxmediadrmdefs.h>

const TInt KGlxDrmIconSize = 16;
const TInt KGlxDrmIconBorder = 0;

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxDrmIconManager* CGlxDrmIconManager::NewL( MGlxMediaList& aMediaList,
                                                MGlxVisualList& aVisualList)
    {
    CGlxDrmIconManager* self = 
            new(ELeave)CGlxDrmIconManager(aMediaList, aVisualList);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//  
EXPORT_C CGlxDrmIconManager::~CGlxDrmIconManager()
    {
    iMediaList.RemoveMediaListObserver(this);
    iVisualList.RemoveObserver(this);
    
    iThumbnailIcons.Close();
    
    if(iDRMUtility)
        {
        iDRMUtility->Close();
        }
    
    iValidDRMItems.Close();
    iInvalidDRMItems.Close();
    }

// ---------------------------------------------------------------------------
//  RemoveOverlayIcon
// ---------------------------------------------------------------------------
//  
void CGlxDrmIconManager::RemoveOverlayIcon(TInt aIndex, 
                                                TBool aInvalidIcon)
    {
    if(aInvalidIcon)
        {
        iVisualList.SetIconVisibility( aIndex,
                 *iDRMRightsExpiredTexture, EFalse );
        }
    else
        {
        iVisualList.SetIconVisibility( aIndex,
                 *iDRMSendForbidTexture, EFalse );
        }
        
    }
    
// ---------------------------------------------------------------------------
//  AddOverlayIconL
// ---------------------------------------------------------------------------
//    
void CGlxDrmIconManager::AddOverlayIconsL()
    {
    TRACER("CGlxDrmIconManager::AddOverlayIconsL");
    TInt itemCount = iMediaList.Count();
    for(TInt i = 0; i < itemCount; i++)
        {
        CAlfVisual* vis = iVisualList.Visual(i);
        if(vis)
            {
            // recheck rights as status may have changed
            CheckDRMRightsL(vis, i);
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleItemAddedL(TInt /*aStartIndex*/, 
                    TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleMediaL(TInt /*aListIndex*/, 
                                            MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// void CGlxDrmIconManager::HandleItemRemovedL(TInt /*aStartIndex*/, 

// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleItemRemovedL(TInt /*aStartIndex*/, 
                        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }
    
// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleItemModifiedL(
        const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }
   
// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleAttributesAvailableL(TInt aItemIndex,     
    const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/)
    {
    TIdentityRelation<TMPXAttribute> match(&TMPXAttribute::Match);
	if (aAttributes.Find(KMPXMediaDrmProtected, match) != KErrNotFound) 
		{	
	    // check if we have DRM attributes
	    CAlfVisual* vis = iVisualList.Visual(aItemIndex);
	    if(vis)
	    	{
	    	CheckDRMRightsL(vis, aItemIndex, EFalse);
	    	}
		}
    }
        
// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleFocusChangedL(
        NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleItemSelectedL(TInt /*aIndex*/, 
                        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleMessageL(const CMPXMessage& /*aMessage*/, 
                                                    MGlxMediaList* /*aList*/)
    {
    // No implementation
    }


// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleFocusChangedL( TInt /*aFocusIndex*/, 
    TReal32 /*aItemsPerSecond*/, MGlxVisualList* /*aList*/,
    NGlxListDefs::TFocusChangeType /*aType*/ )
    {
    // No implementation
    }
    
// ---------------------------------------------------------------------------
// HandleSizeChanged
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleSizeChanged( const TSize& /*aSize*/, 
    MGlxVisualList* /*aList*/ )
    {
    // No implementation
    }
    
// ---------------------------------------------------------------------------
// HandleVisualRemoved
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleVisualRemoved( 
    const CAlfVisual* /*aVisual*/,  MGlxVisualList* /*aList*/ )
    {
    // No implementation
    }
    
// ---------------------------------------------------------------------------
// HandleVisualAddedL
// ---------------------------------------------------------------------------
//
void CGlxDrmIconManager::HandleVisualAddedL( CAlfVisual* aVisual, 
    TInt aIndex, MGlxVisualList* /*aList*/ )
    {
    if(aIndex >= 0 && aIndex < iMediaList.Count() )
        {
        CheckDRMRightsL(aVisual, aIndex);
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//  
CGlxDrmIconManager::CGlxDrmIconManager(MGlxMediaList& aMediaList, 
                                            MGlxVisualList& aVisualList)
    : CGlxIconManager(aMediaList, aVisualList)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//  
void CGlxDrmIconManager::ConstructL()
    {
    TRACER("CGlxDrmIconManager::ConstructL");
    BaseConstructL();
    
    TFileName mifFile( KDC_APP_BITMAP_DIR );
    mifFile.Append( KGlxIconsFilename );
       
    GLX_LOG_INFO("CGlxDrmIconManager::ConstructL create valid icon");   
    // ask texture manager to create DRM valid icon
    iDRMSendForbidTexture = &(iUiUtility->GlxTextureManager().
             CreateIconTextureL( EMbmGlxiconsQgn_prop_drm_rights_valid_super, 
                            mifFile, TSize( KGlxDrmIconSize, KGlxDrmIconSize ) ) );
    GLX_LOG_INFO("CGlxDrmIconManager::ConstructL create invalid icon");                            
    
    // ask texture manager to create DRM invalid icon                        
    iDRMRightsExpiredTexture = &(iUiUtility->GlxTextureManager().
             CreateIconTextureL( EMbmGlxiconsQgn_prop_drm_rights_exp_super, 
                            mifFile, TSize( KGlxDrmIconSize, KGlxDrmIconSize ) ) );
    
    
    
    // add as observers
    iMediaList.AddMediaListObserverL(this); 
    iVisualList.AddObserverL(this);
    
    GLX_LOG_INFO("CGlxDrmIconManager::ConstructL create DRM utility");
    iDRMUtility = CGlxDRMUtility::InstanceL();
    
    // check for any visual already present in list
    TInt itemCount = iMediaList.Count();
    for(TInt i = 0; i < itemCount; i++)
        {
        CAlfVisual* vis = iVisualList.Visual( i );
        if(vis)
            {
            CheckDRMRightsL(vis, i);
            }
        }
        
    }

// ---------------------------------------------------------------------------
// IconAlreadyAdded
// ---------------------------------------------------------------------------
//  
TInt CGlxDrmIconManager::ThumbnailIconArrayPos(TIconInfo& aIconInfo)
    {
    // check if icon already generated
    TInt pos = KErrNotFound;
    TInt count = iThumbnailIcons.Count();
    for(TInt i=0; i<count; i++)
        {
        if((aIconInfo.bmpfile==iThumbnailIcons[i].bmpfile) &&
            (aIconInfo.bitmapId==iThumbnailIcons[i].bitmapId))
            {
            pos = i;
            break;
            }
        }
    return pos;
    }
    
// ---------------------------------------------------------------------------
// CheckDRMRightsL
// ---------------------------------------------------------------------------
//       
void CGlxDrmIconManager::CheckDRMRightsL(CAlfVisual* /*aVisual*/,
                                    TInt aIndex, TBool aForceIcon)
    {
    TRACER("CGlxDrmIconManager::CheckDRMRightsL");
    const TGlxMedia&  item = iMediaList.Item(aIndex);
    TGlxMediaId id = item.Id();
    
    GLX_LOG_INFO1("CGlxDrmIconManager::CheckDRMRightsL item idx (%d)",aIndex);
    // Do not assume DRM protected by default. In default case add no icon.
    // So cannot use TGlxMedia::IsDrmProtected()
    TBool drm = EFalse;
    if(item.GetDrmProtected(drm) && drm)
        {
        const TDesC& uri = item.Uri();
        if(uri.Length() > 0)
            {
            TGlxVisualIcon iconType;        
            if(iDRMUtility->IsForwardLockedL(uri))
                {
                GLX_LOG_INFO("CGlxDrmIconManager::CheckDRMRightsL forward locked");
                if(aForceIcon || !ShouldHaveIcon(id, iconType))
                    {
                    GLX_LOG_INFO("CGlxDrmIconManager::CheckDRMRightsL forward locked add icon");
                    AddValidIconL(id);
                    iVisualList.AddIconL( aIndex, *iDRMSendForbidTexture, 
                        NGlxIconMgrDefs::EGlxIconBottomRight, ETrue, EFalse, KGlxDrmIconBorder );
                        
                    iVisualList.SetIconVisibility( aIndex, *iDRMSendForbidTexture, ETrue );
                    }
               
                }
            else 
                {
                
                TMPXGeneralCategory  cat = item.Category();
                    
                TBool checkViewRights = (cat==EMPXImage);
                
                if(iDRMUtility->ItemRightsValidityCheckL(uri, checkViewRights))
                    {
                    GLX_LOG_INFO("CGlxDrmIconManager::CheckDRMRightsL valid rights");
                    if(aForceIcon ||!ShouldHaveIcon(id, iconType))
                        {
                        GLX_LOG_INFO("CGlxDrmIconManager::CheckDRMRightsL valid rights add icon");
                        AddValidIconL(id);
                        iVisualList.AddIconL( aIndex, *iDRMSendForbidTexture, 
                                            NGlxIconMgrDefs::EGlxIconBottomRight, 
                                            ETrue, EFalse, KGlxDrmIconBorder );
                                            
                        iVisualList.SetIconVisibility( aIndex, *iDRMSendForbidTexture, ETrue );
                        }
                    }
                 else
                    {
                    GLX_LOG_INFO("CGlxDrmIconManager::CheckDRMRightsL invalid rights");
                    
                    if(aForceIcon ||!ShouldHaveIcon(id, iconType))
                        {
                        iVisualList.RemoveIcon( aIndex, *iDRMSendForbidTexture );
                        GLX_LOG_INFO("CGlxDrmIconManager::CheckDRMRightsL add invalid icon");
                        AddInvalidIconL(id);
                        iVisualList.AddIconL( aIndex, *iDRMRightsExpiredTexture, 
                                            NGlxIconMgrDefs::EGlxIconBottomRight, 
                                                ETrue, EFalse, KGlxDrmIconBorder );
                
                        iVisualList.SetIconVisibility( aIndex, *iDRMRightsExpiredTexture, ETrue );                                                
                        }
                    }
                 }
            }
        }
    }

// ---------------------------------------------------------------------------
// AddValidIcon
// ---------------------------------------------------------------------------
//           
void CGlxDrmIconManager::AddValidIconL(TGlxMediaId& aId)
    {
    TInt count = iValidDRMItems.Count();
    TBool present = EFalse;
    for(TInt i=0; i<count; i++)
        {
        if(iValidDRMItems[i]==aId)
            {
            present = ETrue;
            break;
            }
        }
    
    if(present == EFalse)
        {
        iValidDRMItems.AppendL(aId);
        }
    
    // remove from list of items with broken thumbnails
    // if necessary    
    count = iInvalidDRMItems.Count();
    for(TInt j=0;j<count;j++)
        {
        if(iInvalidDRMItems[j]==aId)
            {
            iInvalidDRMItems.Remove(j);
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// AddInvalidIcon
// ---------------------------------------------------------------------------
//          
void CGlxDrmIconManager::AddInvalidIconL(TGlxMediaId& aId)
    {
    TInt count = iInvalidDRMItems.Count();
    TBool present = EFalse;
    for(TInt i=0; i<count; i++)
        {
        if(iInvalidDRMItems[i]==aId)
            {
            present = ETrue;
            break;
            }
        }
    
    if(present == EFalse)
        {
        iInvalidDRMItems.AppendL(aId);
        }
    
    // remove from list of items with broken thumbnails
    // if necessary    
    count = iValidDRMItems.Count();
    for(TInt j=0;j<count;j++)
        {
        if(iValidDRMItems[j]==aId)
            {
            iValidDRMItems.Remove(j);
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// ShouldHaveIcon
// ---------------------------------------------------------------------------
//       
TBool CGlxDrmIconManager::ShouldHaveIcon(TGlxMediaId& aId, 
                                        TGlxVisualIcon& aType)
    {
    TBool hasIcon = EFalse;
    
    TInt count = iValidDRMItems.Count();
    for(TInt i=0; i<count; i++)
        {
        if(iValidDRMItems[i]==aId)
            {
            hasIcon = ETrue;
            aType = EDRMRightsValidIcon;
            break;
            }
        }
        
    if(hasIcon == EFalse)
        {
        count = iInvalidDRMItems.Count();
        for(TInt i=0; i<count; i++)
            {
            if(iInvalidDRMItems[i]==aId)
                {
                hasIcon = ETrue;
                aType = EDRMRightsInvalidIcon;
                break;
                }
            }
        }
    return hasIcon;
    }

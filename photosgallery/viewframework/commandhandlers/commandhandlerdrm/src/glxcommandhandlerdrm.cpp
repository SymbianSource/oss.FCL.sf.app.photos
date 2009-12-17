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
* Description:    provide synchronized access to file systema and metadata source
*
*/




/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */

#include "glxcommandhandlerdrm.h"

#include <alf/alfvisual.h>
#include <alf/alfdisplay.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

#include <glxattributecontext.h>
#include <glxcommandhandlers.hrh>
#include "glxdrmiconmanager.h"
#include <glxdrmutility.h>
#include <glxuiutility.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <glxmediageneraldefs.h>
#include <glxthumbnailattributeinfo.h>
#include <glxthumbnailinfo.h>
#include <glxthumbnailutility.h>
#include <glxuistd.h>
#include <glxvisuallistmanager.h>
#include <mglxmedialist.h>
#include <mglxvisuallist.h>

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerDrm* CGlxCommandHandlerDrm::NewL(
        MGlxMediaListProvider* aMediaListProvider,
        TBool aContainerList)
    {
    CGlxCommandHandlerDrm* self = new ( ELeave )
        CGlxCommandHandlerDrm(aMediaListProvider);
    CleanupStack::PushL( self );
    self->ConstructL(aContainerList);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerDrm::CGlxCommandHandlerDrm(MGlxMediaListProvider*
                                                            aMediaListProvider)
        : CGlxMediaListCommandHandler(aMediaListProvider)
    {
    // Do nothing
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerDrm::ConstructL(TBool aIsContainerList)
    {
    TRACER("CGlxCommandHandlerDrm::ConstructL");
    iIsContainerList = aIsContainerList;

    iDrmUtility = CGlxDRMUtility::InstanceL();
    
    
    // Add supported commands
    
    TCommandInfo info( EGlxCmdDRMOpen );
    // Filter out static items
    info.iMinSelectionLength = 1;
    info.iDisallowSystemItems = aIsContainerList;
    AddCommandL(info);
    
    // Add view state dummy commands
    TCommandInfo view( EGlxCmdStateView );
    AddCommandL( view );
    TCommandInfo browse( EGlxCmdStateBrowse );
    AddCommandL( browse );
    
    // create fetch context 
    iFetchContext = CGlxDefaultAttributeContext::NewL();
    iFetchContext->AddAttributeL( KMPXMediaDrmProtected );
    iFetchContext->AddAttributeL( KMPXMediaGeneralCategory );
    iFetchContext->AddAttributeL( KMPXMediaGeneralUri );
    iFetchContext->AddAttributeL( KGlxMediaGeneralDimensions );
    
    iInFullScreen = EFalse;
    
    // get pointer to HUI utility
    iUiUtility = CGlxUiUtility::UtilityL();
    
    // get pointer to visual list manager
    iVisualListManager = CGlxVisualListManager::ManagerL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerDrm::~CGlxCommandHandlerDrm()
    {        
    iDrmUtility->Close();
    
    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
        
    if(iVisualListManager)
        {
        iVisualListManager->Close();
        }

    delete iFetchContext;
    delete iUrl;
    }

// ---------------------------------------------------------------------------
// ActivateL
// ---------------------------------------------------------------------------
//    
void CGlxCommandHandlerDrm::DoActivateL(TInt /*aViewId*/)
    {
    TRACER("CGlxCommandHandlerDrm::DoActivateL");
    MGlxMediaList& mediaList = MediaList();
    mediaList.AddContextL(iFetchContext, KGlxFetchContextPriorityNormal );
    
    mediaList.AddMediaListObserverL(this);
    
    // get handle to visual list
    iVisualList = iVisualListManager->ListL(mediaList,
                            *iUiUtility->Env(), *iUiUtility->Display());
    
    iVisualList->AddObserverL(this);
    
    // create DRM icon manager
    iDrmIconManager = CGlxDrmIconManager::NewL(mediaList, *iVisualList);
    }


// ---------------------------------------------------------------------------
// Deactivate
// ---------------------------------------------------------------------------
//      
void CGlxCommandHandlerDrm::Deactivate()
    {
    if( iDrmIconManager )
        {
        delete iDrmIconManager;
        iDrmIconManager = NULL;
        }
        
    if( iVisualList )
        {
        iVisualList->RemoveObserver(this);
        iVisualListManager->ReleaseList(iVisualList);
        iVisualList = NULL;
        }
    
    MGlxMediaList& mediaList = MediaList();
    mediaList.RemoveContext(iFetchContext);
    mediaList.RemoveMediaListObserver(this);
    }
    
// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleItemAddedL(TInt /*aStartIndex*/, 
                        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
                                                                 MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
                                                                    MGlxMediaList* /*aList*/)
    {
    // No implementation
    }
       
// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleAttributesAvailableL(TInt aItemIndex,     
    const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
    {
    // if in full-screen consume rights if necessary  
    // only if we have received a newer thumbnail
    // do not consume rights if already consumed
    if(iInFullScreen && aItemIndex == MediaList().FocusIndex())
        {
        if(!iDrmRightsChecked)
            {
            ConsumeDRMRightsL();
            }           
        }
    }
        
// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
                        TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, 
                                                        MGlxMediaList* /*aList*/)
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerDrm::HandleMessageL(const CMPXMessage& /*aMessage*/, 
                                                        MGlxMediaList* /*aList*/)
    {
    // No implementation
    }


// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleFocusChangedL( TInt /*aFocusIndex*/, 
                        TReal32 /*aItemsPerSecond*/, MGlxVisualList* /*aList*/,
                        NGlxListDefs::TFocusChangeType /*aType*/ )
    {
    // if in full-screen consume rights if necessary  
    if(iInFullScreen)
        {
        // reset rights consumed flag as we're moving to 
        // newer item
        iDrmRightsChecked = EFalse;
        ConsumeDRMRightsL();
        }
    }


// ---------------------------------------------------------------------------
// ConsumeDRMRightsL
// ---------------------------------------------------------------------------
//     
void CGlxCommandHandlerDrm::ConsumeDRMRightsL()
    {
    TRACER("CGlxCommandHandlerDrm::ConsumeDRMRightsL");
    
    MGlxMediaList& mediaList = MediaList();
    
    GLX_LOG_INFO("CGlxCommandHandlerDrm::ConsumeDRMRightsL have mlist");

    TInt index = mediaList.FocusIndex();
    User::LeaveIfError(index);          // FocusIndex() can return KErrNotFound

    const TGlxMedia&  item = mediaList.Item(index);
    const CGlxMedia* media = item.Properties();

    if(media)
        {
        if(item.IsDrmProtected())
            {
            GLX_LOG_INFO("CGlxCommandHandlerDrm::ConsumeDRMRightsL drm item");

            TMPXGeneralCategory cat = item.Category();
            const TDesC& uri = item.Uri();
            if( cat != EMPXNoCategory && uri.Length() > 0 )
                {
                GLX_LOG_INFO("CGlxCommandHandlerDrm::ConsumeDRMRightsL check rights");
                
                // check if rights have expired
                TBool expired = !iDrmUtility->CheckOpenRightsL(uri, (cat == EMPXImage));
                                    
                if(expired)
                    {
                    GLX_LOG_INFO("CGlxCommandHandlerDrm::ConsumeDRMRightsL show expiry note");
                    
                    if( iDrmIconManager )
                        {
                        iDrmIconManager->RemoveOverlayIcon( index, ETrue );    
                        }
                    
                    // check if rights have expired
                    // show expiry note
                    iDrmUtility->ShowRightsInfoL(uri);
                    return;
                    }
                
                if( iDrmIconManager )
                    {    
                    iDrmIconManager->RemoveOverlayIcon( index, EFalse );    
                    }
                    
                TSize size;
                if(EMPXImage == cat && item.GetDimensions(size))          
                    {
                    
                    // check size
                    TSize bmpSize = ThumbnailSize(media);
                    
                    if(ConsumeRightsBasedOnSize(index, size, bmpSize))
                        {
                        // pass URI to DRM utility
                        iDrmUtility->ConsumeRightsL(uri);
                        iDrmRightsChecked = ETrue;
                        }       
                    }
                }     
            }
        else
            {
            // not an DRM'd item no need to check again 
            iDrmRightsChecked = ETrue;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// HandleSizeChanged
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleSizeChanged( const TSize& /*aSize*/, MGlxVisualList* /*aList*/ )
    {
    // No implementation
    }
    
// ---------------------------------------------------------------------------
// HandleVisualRemoved
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleVisualRemoved( const CAlfVisual* /*aVisual*/,  MGlxVisualList* /*aList*/ )
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// HandleVisualAddedL
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerDrm::HandleVisualAddedL( CAlfVisual* /*aVisual*/, TInt /*aIndex*/, MGlxVisualList* /*aList*/ )
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// DoExecute - the relevant action for the command id
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerDrm::DoExecuteL(TInt aCommandId,
    MGlxMediaList& /*aList*/)
    {
    TRACER("CGlxCommandHandlerDrm::DoExecuteL");
    TBool handledCommand = ETrue;
    
    switch (aCommandId)
        {
        case EGlxCmdDRMOpen:
            // newly opened item
            // reset rights consumed flag
            iDrmRightsChecked = EFalse;
            
            // do not re-consume rights on return from UMP view
            // only consume if navigation has been forwards
            if ( iUiUtility->ViewNavigationDirection()
                                 ==  EGlxNavigationForwards )
                {
                ConsumeDRMRightsL();    
                }
                
            break;
            
        case EGlxCmdStateView:
            iInFullScreen = ETrue;
            handledCommand = EFalse;
            break;

        case EGlxCmdStateBrowse:
            iInFullScreen = EFalse;
            handledCommand = EFalse;
            if(iDrmIconManager)   
                {
                iDrmIconManager->AddOverlayIconsL();
                }
            break;          
        default:
            {
            handledCommand = EFalse;
            break;
            }
        }
        
    return handledCommand;
    }

// -----------------------------------------------------------------------------
// DoIsDisabled
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerDrm::DoIsDisabled(TInt /*aCommandId*/, 
                                        MGlxMediaList& /*aList*/) const 
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// ConsumeRightsBasedOnSize - check if DRM rights should be consumed
// -----------------------------------------------------------------------------
//    
TBool CGlxCommandHandlerDrm::ConsumeRightsBasedOnSize(TInt aItemIndex,
                                       TSize aImageSize, TSize aBitmapSize)
    {
    TRACER("CGlxCommandHandlerDrm::ConsumeRightsBasedOnSize");
    
    // minimum size (120 x 90)
    TInt minSize = KGlxThumbnailDrmWidth * KGlxThumbnailDrmHeight;
    
    // size of actual image
    TInt imgSize = aImageSize.iWidth * aImageSize.iHeight;
    
    // bitmap size
    TInt bmpSize = aBitmapSize.iWidth * aBitmapSize.iHeight;
    
    // size of HUI display
    TSize dispSize = iUiUtility->DisplaySize();
    
    // Get size of visual based on aspect ratio of image
    TSize visImgSize;
    if(iVisualList)
        {
        CAlfVisual* vis = iVisualList->Visual(aItemIndex);
        if(vis)
            {
            if(dispSize.iWidth>0 && dispSize.iHeight>0)
                {
                TReal widthRatio = (TReal)aImageSize.iWidth/(TReal)dispSize.iWidth;
                TReal heightRatio = (TReal)aImageSize.iHeight/(TReal)dispSize.iHeight;
                
                if(widthRatio > heightRatio)
                    {
                    dispSize.iHeight = aImageSize.iHeight / widthRatio;
                    }
                else
                    {
                    if(heightRatio>0)
                        {
                        dispSize.iWidth = aImageSize.iWidth / heightRatio;
                        }
                    }
                }

            }
        }
        
    TInt visSize2 = dispSize.iWidth * dispSize.iHeight;
    
    // if thumbnail is smaller than visual use this for comparison
    if(bmpSize < visSize2)
        {
        visSize2 = bmpSize;
        }
    
    // is bmp smaller than 1/4 of image size    
    if(imgSize/4 < minSize)
        {
        minSize = imgSize/4;
        }
        
    //
    if(visSize2 >= minSize)
        {
        GLX_LOG_INFO("CGlxCommandHandlerDrm::ConsumeRightsBasedOnSize true");
        iDrmRightsChecked = ETrue;
        }
    
    return iDrmRightsChecked;
    }

// -----------------------------------------------------------------------------
// ThumbnailSizeAndQuality - search for largest available thumbnail
// -----------------------------------------------------------------------------
// 
TSize CGlxCommandHandlerDrm::ThumbnailSize(const CGlxMedia* aMedia)
    {
    TRACER("CGlxCommandHandlerDrm::ThumbnailSize");
    TSize bmpSize(0,0);
    
    TArray<TMPXAttribute> attr = aMedia->Attributes();
    
    TInt selectedHeight = 0;
    TInt selectedWidth = 0;
    
    TInt count = attr.Count();
    for(TInt i=0; i<count; i++)
        {
        
        if ( KGlxMediaIdThumbnail == attr[i].ContentId()
            && GlxIsFullThumbnailAttribute(attr[i].AttributeId()) )
            {
            // Get selected Quality, height and width from attribute
            const CGlxThumbnailAttribute* thumbAtt =  
                    static_cast<const CGlxThumbnailAttribute*>
                         (aMedia->ValueCObject( attr[i]));
                         
            if(thumbAtt)
                {
                
                selectedHeight = thumbAtt->iDimensions.iHeight;
                selectedWidth = thumbAtt->iDimensions.iWidth;
                
                if((selectedHeight * selectedWidth) > 
                    (bmpSize.iHeight * bmpSize.iWidth))
                    {
                    bmpSize.iWidth = selectedWidth;
                    bmpSize.iHeight = selectedHeight;
                    }
                }
            }
        }
    return bmpSize;
    }

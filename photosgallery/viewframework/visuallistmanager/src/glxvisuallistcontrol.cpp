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
* Description:    Visual list manager
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#include <coemain.h>
#include <eikappui.h>
#include <eikenv.h>
#include <alf/alfborderbrush.h>
#include <alf/alfimagevisual.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfenv.h> 
#include <alf/alftexturemanager.h>
#include <alf/alfbrush.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
#include <alf/alftexture.h>
#include <alf/alfdecklayout.h>
#include <mpxmediageneraldefs.h>
#include <glxuiutility.h>
#include <glxtexturemanager.h>
#include <glxmediageneraldefs.h>
#include <glxattributecontext.h>
#include <glxuistd.h>
#include <alf/alflayout.h>
#include <mglxmedialist.h>
#include <glxlog.h>
#include <glxtracer.h>

#include "glxvisuallistcontrol.h"
#include "mglxvisuallistobserver.h" 
#include "glxvisuallistwindow.h" 
#include "glxitemvisual.h"
#include "glxvisualiconmanager.h" // for iVisualIconManager
#include "mglxlayoutobserver.h"

const TInt KMaxTimeBetweenNavigationsForSpeedConsideration = 2000;
const TInt KGlxVisualFetchOffset = 2;

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlxVisualListControl* CGlxVisualListControl::NewLC(
        MGlxMediaList& aMediaList, 
        CAlfEnv& aEnv, 
        CAlfDisplay& aDisplay,
        CAlfImageVisual::TScaleMode aThumbnailScaleMode )
    {
    TRACER("CGlxVisualListControl::NewLC");
    GLX_LOG_INFO("CGlxVisualListControl::NewLC");
    CGlxVisualListControl* self =
        new (ELeave) CGlxVisualListControl( 
            aMediaList, aEnv, aThumbnailScaleMode );
    CleanupStack::PushL(self);
    self->ConstructL( aEnv, aDisplay );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxVisualListControl::CGlxVisualListControl(
        MGlxMediaList& aMediaList,
        CAlfEnv& aEnv, 
		CAlfImageVisual::TScaleMode aThumbnailScaleMode )
    : iMediaList(aMediaList ),
      iEnv( aEnv ),
      iScaleMode( aThumbnailScaleMode )
	{
	TRACER("CGlxVisualListControl::CGlxVisualListControl");
	GLX_LOG_INFO("CGlxVisualListControl::CGlxVisualListControl");
	// set current time
	TTime time;
	time.HomeTime();
    iLastTime = time;
	}

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::ConstructL( CAlfEnv& aEnv, 
		CAlfDisplay& aDisplay ) 
	{
	TRACER("CGlxVisualListControl::ConstructL");
	GLX_LOG_INFO("CGlxVisualListControl::ConstructL");
	// call base class ConstructL
    CAlfControl::ConstructL( aEnv );
	BindDisplay( aDisplay );    
	iParentLayout = CAlfAnchorLayout::AddNewL(*this);
	iParentLayout->SetFlags(EAlfVisualFlagLayoutUpdateNotification);
	iParentLayout->EnableTransformationL();

	iBorderBrush = CAlfBorderBrush::NewL( iEnv, 1, 1, 0, 0 );
	iBorderBrush->SetColor(KRgbBlack);

	iMediaList.AddMediaListObserverL( this );

    iUiUtility = CGlxUiUtility::UtilityL();
    // create the visual list window
    iVisualWindow = CGlxVisualListWindow::NewL(this,&iMediaList, iUiUtility, iScaleMode);
    
	iControlGroupId = reinterpret_cast<int>((void*)this);   // @todo: Temp, have better logic for control group ids
	iControlGroup = &Env().NewControlGroupL(iControlGroupId);
	iControlGroup->AppendL(this);
	
	// The media list might not be empty, so add items if there are any
	TInt count = iMediaList.Count();
	if (count > 0) 
		{
		// add the items to the visual list first
		HandleItemAddedL( 0, count - 1, &iMediaList );
		// then set the focus as media list may have been navigated before
		iVisualWindow->SetFocusIndexL( iMediaList.FocusIndex() );
		}
    iAttributeContext = CGlxDefaultAttributeContext::NewL();
    iAttributeContext->AddAttributeL(KMPXMediaGeneralUri);
    iAttributeContext->AddAttributeL(KGlxMediaGeneralFramecount);
    iAttributeContext->SetRangeOffsets(KGlxVisualFetchOffset,
                                        KGlxVisualFetchOffset);
    iMediaList.AddContextL( iAttributeContext, KGlxFetchContextPriorityLow );
    
    
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxVisualListControl::~CGlxVisualListControl()
	{
	TRACER("CGlxVisualListControl::~CGlxVisualListControl");
	GLX_LOG_INFO("CGlxVisualListControl::~CGlxVisualListControl");
	// delete visual icon manager just in case
	delete iVisualIconManager;
	iVisualIconManager = NULL;
	
	// This control has to be removed from the control group, since deleting 
	// the control group also deletes the control (this object), and would
	// delete the control twice.
	if ( iControlGroup )
		{
		iControlGroup->Remove(this); // Gives ownership to caller, but I'm being deleted anyway
		Env().DeleteControlGroup(iControlGroupId);
		iControlGroup = NULL;
		}

    if ( iAttributeContext )
        {
        iMediaList.RemoveContext( iAttributeContext );
        }
    delete iAttributeContext;
    iAttributeContext = NULL;

	// and media list observer
	iMediaList.RemoveMediaListObserver( this );
	delete iVisualWindow;
	iVisualWindow = NULL;
	iContexts.Close();
	iObservers.Close();
	delete iBorderBrush;
	iBorderBrush = NULL;
    if ( iUiUtility )
        {
        iUiUtility->Close();
        iUiUtility = NULL;
        }
	}

// -----------------------------------------------------------------------------
// AddReference
// -----------------------------------------------------------------------------
//
TInt CGlxVisualListControl::AddReference()
	{
	TRACER("CGlxVisualListControl::AddReference");
	GLX_LOG_INFO("CGlxVisualListControl::AddReference");
	iReferenceCount++;
	return iReferenceCount;
	}
    
// -----------------------------------------------------------------------------
// RemoveReference
// -----------------------------------------------------------------------------
//
TInt CGlxVisualListControl::RemoveReference()
	{
	TRACER("CGlxVisualListControl::RemoveReference");
	GLX_LOG_INFO("CGlxVisualListControl::RemoveReference");
	__ASSERT_ALWAYS(iReferenceCount > 0, Panic(EGlxPanicLogicError));
	iReferenceCount--;
	return iReferenceCount;
	}

// -----------------------------------------------------------------------------
// MediaList
// -----------------------------------------------------------------------------
//
const MGlxMediaList& CGlxVisualListControl::MediaList() const
    {
    TRACER("CGlxVisualListControl::MediaList");
    GLX_LOG_INFO("CGlxVisualListControl::MediaList");
    return iMediaList;
    }

// -----------------------------------------------------------------------------
// Id
// -----------------------------------------------------------------------------
//
TGlxVisualListId CGlxVisualListControl::Id() const 
	{
	TRACER("CGlxVisualListControl::Id");
	GLX_LOG_INFO("CGlxVisualListControl::Id");
	return TGlxVisualListId(reinterpret_cast<unsigned int>((void*)this));
	}

// -----------------------------------------------------------------------------
// Returns visual by index
// -----------------------------------------------------------------------------
//
CAlfVisual* CGlxVisualListControl::Visual( TInt aListIndex )
	{
	TRACER("CGlxVisualListControl::Visual");
	GLX_LOG_INFO("CGlxVisualListControl::Visual");
    CGlxVisualObject* visualObject = Item( aListIndex );
    if ( visualObject )
        {
        return visualObject->Visual();
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// Returns visual by index
// -----------------------------------------------------------------------------
//
CGlxVisualObject* CGlxVisualListControl::Item( TInt aListIndex )
    {
    TRACER("CGlxVisualListControl::Item");
    GLX_LOG_INFO("CGlxVisualListControl::Item");
    return iVisualWindow->ObjectByIndex( aListIndex );
    }

// -----------------------------------------------------------------------------
// Returns item count
// -----------------------------------------------------------------------------
//
TInt CGlxVisualListControl::ItemCount(NGlxListDefs::TCountType aType) const
	{
	TRACER("CGlxVisualListControl::ItemCount");
	GLX_LOG_INFO("CGlxVisualListControl::ItemCount");
	return iMediaList.Count(aType);
	}

// -----------------------------------------------------------------------------
// Returns focus index
// -----------------------------------------------------------------------------
//
TInt CGlxVisualListControl::FocusIndex() const
	{
	TRACER("CGlxVisualListControl::FocusIndex");
	GLX_LOG_INFO("CGlxVisualListControl::FocusIndex");
	return iMediaList.FocusIndex();
	}

// -----------------------------------------------------------------------------
// ControlGroup
// -----------------------------------------------------------------------------
//
CAlfControlGroup* CGlxVisualListControl::ControlGroup() const 
	{
	TRACER("CGlxVisualListControl::ControlGroup");
	GLX_LOG_INFO("CGlxVisualListControl::ControlGroup");
	return iControlGroup;
	}

// -----------------------------------------------------------------------------
// AddContextL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::AddObserverL(MGlxVisualListObserver* aObserver) 
	{
	TRACER("CGlxVisualListControl::AddObserverL");
	GLX_LOG_INFO("CGlxVisualListControl::AddObserverL");
	__ASSERT_DEBUG(iObservers.Find(aObserver) == KErrNotFound, 
	                    Panic(EGlxPanicIllegalArgument)); // Already exists
	iObservers.Append(aObserver);
	}

// -----------------------------------------------------------------------------
// AddContextL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::RemoveObserver(MGlxVisualListObserver* aObserver)
	{
	TRACER("CGlxVisualListControl::RemoveObserver");
	GLX_LOG_INFO("CGlxVisualListControl::RemoveObserver");
	TInt i = iObservers.Find(aObserver);
	if (i != KErrNotFound)
		{
		iObservers.Remove(i);
		}
	}

// -----------------------------------------------------------------------------
// AddContextL
// -----------------------------------------------------------------------------
//
TGlxViewContextId CGlxVisualListControl::AddContextL(
                TInt aFrontVisibleRangeOffset, TInt aRearVisibleRangeOffset )
	{
	TRACER("CGlxVisualListControl::AddContextL");
	GLX_LOG_INFO("CGlxVisualListControl::AddContextL");
	iContexts.ReserveL( iContexts.Count() + 1 );
	TGlxViewContextId nextId;
	iContextIdProvider.NextId(nextId);
	
	// Add the context to define which visuals should be created
	TContext context;
	context.iId = nextId;
	context.iFrontVisibleRangeOffset = aFrontVisibleRangeOffset;
	context.aRearVisibleRangeOffset = aRearVisibleRangeOffset;
	iContexts.Append(context); // Cannot fail thanks to reservation
	
	// Combine the contexts, and update the window
	TInt frontOffset = 0;
	TInt rearOffset = 0;
	RangeOffsets(frontOffset, rearOffset);
	iVisualWindow->SetRangeOffsetsL(frontOffset, rearOffset);

	iVisualWindow->UpdatePositions();

	return nextId;
	}

// -----------------------------------------------------------------------------
// RemoveContext
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::RemoveContext(const TGlxViewContextId& aContextId) 
	{
	TRACER("CGlxVisualListControl::RemoveContext");
	GLX_LOG_INFO("CGlxVisualListControl::RemoveContext");
	// Remove context
	TInt count = iContexts.Count();
	TInt i;
	for (i = 0; i < count; i++)
		{
		if (iContexts[i].iId == aContextId)
			{
			iContexts.Remove(i);
			break;
			}
		}
	__ASSERT_DEBUG(i != count, Panic(EGlxPanicIllegalArgument)); // No such context
	
	// Combine the contexts, and update the window
	TInt frontOffset = 0;
	TInt rearOffset = 0;
	RangeOffsets(frontOffset, rearOffset); 
	// This can actually never fail (read CVieListWindow header). Trapped in case maintenance
	// changes change the behavior of window base class
	TRAP_IGNORE(iVisualWindow->SetRangeOffsetsL(frontOffset, rearOffset));
	}

// -----------------------------------------------------------------------------
// RangeOffsets
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::RangeOffsets(TInt& aFrontOffset, TInt& aRearOffsets)
	{
	TRACER("CGlxVisualListControl::RangeOffsets");
	GLX_LOG_INFO("CGlxVisualListControl::RangeOffsets");
	// Combine the contexts, and update the window
	aFrontOffset = 0;
	aRearOffsets = 0;
	
	TInt count = iContexts.Count();
	for (TInt i = 0; i < count; i++)
		{
		TContext& context = iContexts[i];
		// Pick smallest start offset
		aFrontOffset = Min(aFrontOffset, context.iFrontVisibleRangeOffset);
		// Pick largest end offset
		aRearOffsets = Max(aRearOffsets, context.aRearVisibleRangeOffset);
		}
	}
	
// -----------------------------------------------------------------------------
// AddLayoutL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::AddLayoutL(MGlxLayout* /*aLayout*/)
	{
	// No implementation
	}
	
// -----------------------------------------------------------------------------
// RemoveLayout
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::RemoveLayout(const MGlxLayout* /*aLayout*/)
	{
	// No implementation
	}

// BringVisualsToFront
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::BringVisualsToFront()
    {
    TRACER("CGlxVisualListControl::BringVisualsToFront");
    GLX_LOG_INFO("CGlxVisualListControl::BringVisualsToFront");
    iUiUtility->Display()->Roster().MoveVisualToFront( *iParentLayout );
    }

// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
		MGlxMediaList* /*aList*/)
	{
	TRACER("CGlxVisualListControl::HandleItemAddedL");
	GLX_LOG_INFO("CGlxVisualListControl::HandleItemAddedL");
	iVisualWindow->AddObjects( aStartIndex, aEndIndex );
	iVisualWindow->UpdatePositions();
	}
	
// -----------------------------------------------------------------------------
// HandleMediaL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleMediaL(TInt /*aIndex*/, MGlxMediaList* aList)
	{
	TRACER("CGlxVisualListControl::HandleMediaL");
	GLX_LOG_INFO("CGlxVisualListControl::HandleMediaL");
	__ASSERT_DEBUG(aList == &iMediaList, Panic(EGlxPanicIllegalArgument));
	}

// -----------------------------------------------------------------------------
// HandleItemRemovedL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,
		MGlxMediaList* /*aList*/)
	{
	TRACER("CGlxVisualListControl::HandleItemRemovedL");
	GLX_LOG_INFO("CGlxVisualListControl::HandleItemRemovedL");
	iVisualWindow->RemoveObjects( aStartIndex, aEndIndex );
	iVisualWindow->UpdatePositions();
	}

// -----------------------------------------------------------------------------
// HandleItemModifiedL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleItemModifiedL(
            const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/ )
    {
    // No implementation
    }

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleAttributesAvailableL( TInt aItemIndex, 	
		const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList ) 
	{
	TRACER("CGlxVisualListControl::HandleAttributesAvailableL");
	GLX_LOG_INFO("CGlxVisualListControl::HandleAttributesAvailableL");
	__ASSERT_DEBUG( aList == &iMediaList, Panic( EGlxPanicIllegalArgument ) );

    // forward the attribute availability info to the matching visual object,
    // if it exists in the window
    CGlxVisualObject* visualObject = iVisualWindow->ObjectByIndex( aItemIndex );
	if( visualObject )
		{
		visualObject->HandleAttributesAvailableL( aList->IdSpaceId( aItemIndex ), 
            aList->Item( aItemIndex ), aAttributes );
		}
	}

// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleFocusChangedL( 
                NGlxListDefs::TFocusChangeType aType, 
                TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList )
    {
    TRACER("CGlxVisualListControl::HandleFocusChangedL");
    GLX_LOG_INFO("CGlxVisualListControl::HandleFocusChangedL");
    __ASSERT_DEBUG( aList == &iMediaList, Panic( EGlxPanicIllegalArgument ) );

    iVisualWindow->SetFocusIndexL( aNewIndex );

    // Move the focused visual to front to make sure focused visual is above
    // other visuals
    if ( aNewIndex >= 0 )
        {
         iVisualWindow->ObjectByIndex( aNewIndex )->Visual()->MoveToFront();
        }

    // get the current time
    TTime time_now;
    time_now.HomeTime();
    // get the delta
    TTimeIntervalMicroSeconds elapsed = time_now.MicroSecondsFrom( iLastTime );
    // set last time to be time now
    iLastTime = time_now;

    TReal32 speed = 0;
    // Only consider the focus change if there was a focus before
    if ( aOldIndex != KErrNotFound
        && elapsed < KMaxTimeBetweenNavigationsForSpeedConsideration )
        {
        TInt indexesMoved = 0;

        if ( aType == NGlxListDefs::EForward ) 
            {
            indexesMoved = aNewIndex - aOldIndex;
            }
        else if ( aType == NGlxListDefs::EBackward ) 
            {
            indexesMoved = aOldIndex - aNewIndex;
            }
        else 
            {
            // Client did not use NavigateL for setting the focus index =>
            // don't guess, instead provide no speed.
            }

        if ( indexesMoved < 0 )
            {
            indexesMoved += ItemCount();
            }
        }

	TInt count = iObservers.Count();
	for (TInt i = 0; i < count; i++)
		{
		iObservers[i]->HandleFocusChangedL( aNewIndex, speed, this, aType );
		}
	} 

// -----------------------------------------------------------------------------
// Handles item selection/deselection
// -----------------------------------------------------------------------------
void CGlxVisualListControl::HandleItemSelectedL( TInt /*aIndex*/,
                                TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// Handles collection notification
// -----------------------------------------------------------------------------
void CGlxVisualListControl::HandleMessageL( const CMPXMessage& /*aMessage*/,
                                            MGlxMediaList* /*aList*/ )
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// NavigateL
// From MViuVisualOwner
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::NavigateL(TInt aIndexCount)
	{
	TRACER("CGlxVisualListControl::NavigateL");
	GLX_LOG_INFO("CGlxVisualListControl::NavigateL");
	if (aIndexCount == 0)
		{
 		__ASSERT_DEBUG(EFalse, Panic(EGlxPanicIllegalArgument)); // no navigation to either direction
		return;
		}
	iMediaList.SetFocusL(NGlxListDefs::ERelative, aIndexCount);
	}
	
// -----------------------------------------------------------------------------
// NavigateL
// From MViuVisualOwner
// -----------------------------------------------------------------------------
//
 TSize CGlxVisualListControl::Size() const 
 	{
 	TRACER("CGlxVisualListControl::Size");
 	GLX_LOG_INFO("CGlxVisualListControl::Size");
	return iParentLayout->Size().Target().AsSize();
 	}

// -----------------------------------------------------------------------------
// NavigateL
// From CHuiControl
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::VisualLayoutUpdated( CAlfVisual& aVisual )
	{
	TRACER("CGlxVisualListControl::VisualLayoutUpdated");
	GLX_LOG_INFO("CGlxVisualListControl::VisualLayoutUpdated");
	if (&aVisual == iParentLayout) 
		{
		TSize size = iParentLayout->Size().Target().AsSize();
		if(size != iCurrentLayoutSize)
		    {
            iCurrentLayoutSize = size;		    
    		TInt count = iObservers.Count();
    		for (TInt i = 0; i < count; i++)
    			{
    			iObservers[i]->HandleSizeChanged(size, this);
    			}
		    }
		}
	}

// -----------------------------------------------------------------------------
// HandleVisualRemoved
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleVisualRemoved(const CAlfVisual* aVisual)
	{
	TRACER("CGlxVisualListControl::HandleVisualRemoved");
	GLX_LOG_INFO("CGlxVisualListControl::HandleVisualRemoved");
	TInt count = iObservers.Count();
	for (TInt i = 0; i < count; i++)
		{
		iObservers[i]->HandleVisualRemoved(aVisual, this);
		}
	}

// -----------------------------------------------------------------------------
// HandleVisualRemoved
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::HandleVisualAddedL(CAlfVisual* aVisual, TInt aIndex)
	{
	TRACER("CGlxVisualListControl::HandleVisualAddedL");
	GLX_LOG_INFO("CGlxVisualListControl::HandleVisualAddedL");
	TInt count = iObservers.Count();
	for (TInt i = 0; i < count; i++)
		{
		iObservers[i]->HandleVisualAddedL(aVisual, aIndex, this);
		}
	}

// -----------------------------------------------------------------------------
// EnableAnimationL
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::EnableAnimationL(TBool aAnimate, TInt aIndex)
    {
    TRACER("CGlxVisualListControl::EnableAnimationL");
    GLX_LOG_INFO("CGlxVisualListControl::EnableAnimationL");
    CGlxVisualObject* visualObject = iVisualWindow->ObjectByIndex( aIndex );
    if ( visualObject )
        {
        if( aAnimate )
            {
            visualObject->TryAnimateL( iMediaList.Item( aIndex ) );
    		}
        else
            {
            // Stop animation
    		visualObject->StopAnimation();
            visualObject->SetAnimateWhenAttributesAvailable( EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// SetDefaultIconBehaviourL
// -----------------------------------------------------------------------------
void CGlxVisualListControl::SetDefaultIconBehaviourL( TBool aEnable )
    {
    TRACER("CGlxVisualListControl::SetDefaultIconBehaviourL");
    GLX_LOG_INFO("CGlxVisualListControl::SetDefaultIconBehaviourL");
    // do we want to disable
    if( !aEnable )
    	{
    	// disable by deleting it
    	delete iVisualIconManager;
    	// prevent double delete
    	iVisualIconManager = NULL;
    	}
    // do we want to enable and we dont yet have icon manager
    else if( !iVisualIconManager )
    	{
	    iVisualIconManager = CGlxVisualIconManager::NewL( iMediaList, *this );
    	}
    else
        {
        }
    }

// -----------------------------------------------------------------------------
// ThumbnailScaleMode
// -----------------------------------------------------------------------------
//
CAlfImageVisual::TScaleMode CGlxVisualListControl::ThumbnailScaleMode()
    {
    TRACER("CGlxVisualListControl::ThumbnailScaleMode");
    GLX_LOG_INFO("CGlxVisualListControl::ThumbnailScaleMode");
    return iScaleMode;
    }

// -----------------------------------------------------------------------------
// AddIconL
// -----------------------------------------------------------------------------
//    
void CGlxVisualListControl::AddIconL( TInt aListIndex, const CAlfTexture& aTexture, 
            NGlxIconMgrDefs::TGlxIconPosition aIconPos,
            TBool aForeground, TBool aStretch, TInt aBorderMargin,
            TReal32 aWidth, TReal32 aHeight )
    {
    TRACER("CGlxVisualListControl::AddIconL");
    GLX_LOG_INFO("CGlxVisualListControl::AddIconL");
    CGlxVisualObject* visItem = Item( aListIndex );
    if( visItem )
        {
        visItem->AddIconL( aTexture, aIconPos, 
                       aForeground, aStretch, aBorderMargin, aWidth, aHeight );
        }
    }

// -----------------------------------------------------------------------------
// RemoveIconL
// -----------------------------------------------------------------------------
//   
TBool CGlxVisualListControl::RemoveIcon( TInt aListIndex, const CAlfTexture& aTexture )
    {
    TRACER("CGlxVisualListControl::RemoveIcon");
    GLX_LOG_INFO("CGlxVisualListControl::RemoveIcon");
    TBool iconRemoved = EFalse;
    
    CGlxVisualObject* visItem = Item( aListIndex );
    if( visItem )
        {
        iconRemoved = visItem->RemoveIcon( aTexture );
        }
    
    return iconRemoved;
    }    

// -----------------------------------------------------------------------------
// SetIconVisibility
// -----------------------------------------------------------------------------
//
void CGlxVisualListControl::SetIconVisibility( TInt aListIndex, 
                                const CAlfTexture& aTexture, TBool aVisible )
    {
    TRACER("CGlxVisualListControl::SetIconVisibility");
    GLX_LOG_INFO("CGlxVisualListControl::SetIconVisibility");
    CGlxVisualObject* visItem = Item( aListIndex );
    if( visItem )
        {
        visItem->SetIconVisibility( aTexture, aVisible );
        }
    }


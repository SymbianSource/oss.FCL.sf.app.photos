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
 * Description:    Implementation of background processing indicator
 *
 */

#include <stringloader.h>
#include <data_caging_path_literals.hrh>            // KDC_APP_RESOURCE_DIR
#include <glxtracer.h>
#include <glxlog.h>                                 // Glx logs
#include <glxuistd.h>
#include <glxuiutility.h>                           // UiUtility Singleton
#include <glxicons.mbg>
#include <glxtexturemanager.h>                      // Texturemanager
#include <mglxmedialist.h>
#include <glxthumbnailattributeinfo.h>
#include <glxerrormanager.h>
// Alf Headers
#include <alf/alfutil.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfimagevisual.h>
#include "glxfullscreenbusyicon.h"

const TInt KGlxFullScreenControlGrpId = 0x113; // This is the same control grp id used in FS view
//define the opacity values
const TReal KOpacityOpaque = 1.0;
const TReal KOpacityTransparent = 0.0;
const TInt KBusyIconFrameCount = 10;
const TInt KMainVisSz = 60;
const TInt KIconOffset = 10;
const TTimeIntervalMicroSeconds32 KanInterval = 100000;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//  
CGlxFullScreenBusyIcon* CGlxFullScreenBusyIcon::NewL(MGlxMediaList& aMediaList,
		CGlxUiUtility& aUiUtility)
	{
	TRACER("CGlxFullScreenBusyIcon::NewL()");
	CGlxFullScreenBusyIcon* self = new (ELeave) CGlxFullScreenBusyIcon(
			aMediaList, aUiUtility);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CGlxFullScreenBusyIcon constructor
// ---------------------------------------------------------------------------
//  
CGlxFullScreenBusyIcon::CGlxFullScreenBusyIcon(MGlxMediaList& aMediaList,
		CGlxUiUtility& aUiUtility) :
	iMediaList(aMediaList), iUiUtility(aUiUtility), iBusyIconIndex(0)

	{
	//iBusyIconIndex(0),
	TRACER("CGlxFullScreenBusyIcon::CGlxFullScreenBusyIcon()");
	// No Implementation
	}
// ---------------------------------------------------------------------------
// ~CGlxFullScreenBusyIcon Destructor
// ---------------------------------------------------------------------------
//  
CGlxFullScreenBusyIcon::~CGlxFullScreenBusyIcon()
	{
	TRACER("CGlxFullScreenBusyIcon::~CGlxFullScreenBusyIcon()");

	iMediaList.RemoveMediaListObserver(this);

	if (iPeriodic->IsActive())
		{
		iPeriodic->Cancel();
		}

	delete iPeriodic;

	if (iMainVisual)
		{
		iMainVisual->RemoveAndDestroyAllD();
		iMainVisual = NULL;
		}
	}
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//  
void CGlxFullScreenBusyIcon::ConstructL()
	{
	TRACER("CGlxFullScreenBusyIcon::ConstructL");
	TFileName resFile(KDC_APP_BITMAP_DIR);
	resFile.Append(KGlxIconsFilename);
	iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
	iMediaList.AddMediaListObserverL(this);
	/* create the the alf visual,anchorlayout and add layout to visual*/
	iEnv = iUiUtility.Env();
	CAlfControl::ConstructL(*iEnv);
	iMainVisual = CAlfAnchorLayout::AddNewL(*this);

	//BackGround Border Image Visual
	iBackgroundBorderImageVisual = CAlfImageVisual::AddNewL(*this, iMainVisual);
	CAlfControlGroup* group = &iEnv->ControlGroup(KGlxFullScreenControlGrpId);
	group->AppendL(this);

	iMainVisual->SetFlag(EAlfVisualFlagManualLayout);

	// Update the pos and size of the visual 
	UpdatePosition();

	// Setting the Flag to get layout change notifications
	iMainVisual->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
	iMainVisual->SetSize(TAlfTimedPoint(KMainVisSz, KMainVisSz));
	// Store the Current Screensize
	iScreenSize = GetScreenSize();
	if (IsReadyL())
		{
		ShowBusyIconL(EFalse);
		}
	else
		{
		ShowBusyIconL(ETrue);
		}
	}
// ---------------------------------------------------------------------------
// PeriodicCallbackL
// ---------------------------------------------------------------------------
// 
TInt CGlxFullScreenBusyIcon::PeriodicCallbackL(TAny* aPtr)
	{
	TRACER("CGlxFullScreenBusyIcon::PeriodicCallbackL");
	static_cast<CGlxFullScreenBusyIcon*> (aPtr)->CallPeriodicCallbackL();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// Callback from periodic timer-- non static
// -----------------------------------------------------------------------------
//
inline void CGlxFullScreenBusyIcon::CallPeriodicCallbackL()
	{
	TRACER("CGlxFullScreenBusyIcon::CallPeriodicCallbackL");
	// Get the icon file
	TFileName resFile(KDC_APP_BITMAP_DIR);
	resFile.Append(KGlxIconsFilename);
	CAlfTexture& backgroundTexture =
			iUiUtility.GlxTextureManager().CreateIconTextureL(
					EMbmGlxiconsQgn_graf_ring_wait_01 + iBusyIconIndex * 2,
					resFile);
	iBusyIconIndex++;
	//reset the svg frames
	if (iBusyIconIndex >= KBusyIconFrameCount)
		{
		iBusyIconIndex = 0;
		}
	iBackgroundBorderImageVisual->SetImage(TAlfImage(backgroundTexture));
	ShowBusyIconL(ETrue);
	}
// ---------------------------------------------------------------------------
// ShowBusyIconL
// ---------------------------------------------------------------------------
//
void CGlxFullScreenBusyIcon::ShowBusyIconL(TBool aShow)
	{
	TRACER("CGlxFullScreenBusyIcon::ShowBusyIconL()");
	GLX_LOG_INFO1("CGlxFullScreenBusyIcon::ShowBusyIconL() value %d",aShow);
	if (aShow)
		{
		iMainVisual->SetOpacity(KOpacityOpaque);
		}
	else
		{
		iMainVisual->SetOpacity(KOpacityTransparent);
		}
	if (!aShow)
		{
		if (iPeriodic->IsActive())
			{
			iPeriodic->Cancel();
			}
		}
	if (!iPeriodic->IsActive() && aShow)
		{
		iPeriodic->Start(0, KanInterval, TCallBack(&PeriodicCallbackL,
				static_cast<TAny*> (this)));
		}
	}
// ---------------------------------------------------------------------------
// GetScreenSize
// ---------------------------------------------------------------------------
//
TRect CGlxFullScreenBusyIcon::GetScreenSize()
	{
	TRACER("CGlxFullScreenBusyIcon::GetScreenSize()");
	return AlfUtil::ScreenSize();
	}
// ---------------------------------------------------------------------------
// UpdatePosition
// ---------------------------------------------------------------------------
//
void CGlxFullScreenBusyIcon::UpdatePosition()
	{
	TRACER("CGlxFullScreenBusyIcon::UpdatePosition()");
	TRect rect = GetScreenSize();
	TInt screenWidth = rect.Width();
	// Switch off the Flag for layout update notification, else a change in size/pos 
	// will call VisualLayoutUpdated multiple times.
	iMainVisual->ClearFlag(EAlfVisualFlagLayoutUpdateNotification);
	//set the control size
	iMainVisual->SetSize(TSize(KMainVisSz, KMainVisSz));
	iMainVisual->SetPos(TAlfRealPoint(screenWidth / 2 - KMainVisSz / 2,
			KIconOffset));
	iMainVisual->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
	}

// ---------------------------------------------------------------------------
// VisualLayoutUpdated
// ---------------------------------------------------------------------------
//  
void CGlxFullScreenBusyIcon::VisualLayoutUpdated(CAlfVisual&/* aVisual*/)
	{
	TRACER("CGlxFullScreenBusyIcon::VisualLayoutUpdated()");
	// Update the Screen positions only if the layout has changed.
	// For performace improvement
	TRect presentScreenSize = GetScreenSize();
	if (iScreenSize.Width() != presentScreenSize.Width())
		{
		UpdatePosition();
		iScreenSize = presentScreenSize;
		}
	}
// ---------------------------------------------------------------------------
// IsReadyL
// ---------------------------------------------------------------------------
//  
TBool CGlxFullScreenBusyIcon::IsReadyL()
	{
	TRACER("CGlxFullScreenBusyIcon::IsReadyL()");
	if (iMediaList.Count() > 0)
		{
		TInt fsMediaIndex = iMediaList.FocusIndex();
		TGlxMedia focusItem = iMediaList.Item(fsMediaIndex);
		TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
				focusItem.Properties(), KGlxMediaIdThumbnail);
		if (thumbnailError)
			{
			return ETrue;
			}

		TMPXAttribute tnAttribQuality(KGlxMediaIdThumbnail,
				GlxFullThumbnailAttributeId(ETrue, iScreenSize.Width(),
						iScreenSize.Height()));
		// This is because there is only one context maintained for FullScreen TN
		TMPXAttribute tnAttribQualityOtherOrientation(KGlxMediaIdThumbnail,
				GlxFullThumbnailAttributeId(ETrue, iScreenSize.Height(),
						iScreenSize.Width()));

		const CGlxThumbnailAttribute* qualityTn = focusItem.ThumbnailAttribute(
				tnAttribQuality);
		if (!qualityTn)
			{
			qualityTn = focusItem.ThumbnailAttribute(
					tnAttribQualityOtherOrientation);
			}
		if (qualityTn)
			{
			return ETrue;
			}

		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
// 
void CGlxFullScreenBusyIcon::HandleAttributesAvailableL(TInt aItemIndex,
		const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
	{
	TRACER("CGlxFullScreenBusyIcon::HandleAttributesAvailableL()");
	if (aItemIndex == iMediaList.FocusIndex())
		{
		if (IsReadyL())
			{
			ShowBusyIconL(EFalse);
			}
		}
	}

// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//
void CGlxFullScreenBusyIcon::HandleFocusChangedL(
		NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/,
		TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
	{
	TRACER("CGlxFullScreenBusyIcon::HandleFocusChangedL()");
	ShowBusyIconL(EFalse);
	iBusyIconIndex = 0;//reset the icon index to start from the beginning
	if (!IsReadyL())
		{
		ShowBusyIconL(ETrue);
		}
	}
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxFullScreenBusyIcon::HandleError(TInt /*aError*/)
	{
	if (IsReadyL())
		{
		ShowBusyIconL(EFalse);
		}
	}

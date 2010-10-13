/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Volume control implementation
*
*/




// INCLUDE FILES
#include "shwslideshowvolumecontrol.h"

#include <alf/alfdisplay.h>
#include <alf/alftexture.h>
#include <alf/alfimage.h>
#include <alf/alfimagevisual.h>
#include <alf/alftextvisual.h>

#include <alf/alfgridlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfimagebrush.h>
#include <alf/alfroster.h>
#include <alf/alfbrusharray.h>

#include <data_caging_path_literals.hrh>
#include <AknsUtils.h>          // skins
#include <glxicons.mbg>
#include <avkon.mbg>
#include <glxresourceutilities.h>   // for CGlxResourceUtilities

#include <AknUtils.h>
#include <StringLoader.h>

#include <glxlog.h>
#include <glxtracer.h>
#include <shwslideshowview.rsg>

#include "glxuiutility.h"
#include "glxtexturemanager.h"
#include "shwcallback.h"

// CONSTANTS
namespace
    {
    const TInt KShwVolumeControlXOffset = 0;
    _LIT( KIconsFilename, "glxicons.mif" );
    const TInt KShwVolumeControlWidthScaleFactor = 2;
    const TInt KShwVolumeMaxPercent = 100;
    const TInt KShwVolumeTextMaxLineCount = 1;   
    const TReal KControlWidthMultiplier = 0.2;
    const TReal KControlHeightMultiplier = 0.12;
    const TReal KOpacityOpaque = 1.0;
    const TReal KHalfOpacityOpaque = 0.4;
    const TReal KOpacityTransperent = 0.0;
    const TInt KValueGridCols = 2;
    const TInt KValueGridRows = 1;
    }
    
//! NOTE : This implementation does not go with the current Slide Show UI Spec 1.1
// UI specs need to be updated and a fine print needs to pe prepared about the 
// LAF of this control
// In that case the visual positioning can be adjusted by paddin and giving weights

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CShwSlideshowVolumeControl()
// ---------------------------------------------------------------------------
inline CShwSlideshowVolumeControl::CShwSlideshowVolumeControl( CAlfEnv& aEnv,
		CGlxUiUtility& aUtility, TTimeIntervalMicroSeconds32 aInterval)
		: iUtility(aUtility), iInterval(aInterval),iAlfEnv(aEnv)
	{
	}

// ---------------------------------------------------------------------------
// ~CShwSlideshowVolumeControl
// ---------------------------------------------------------------------------
CShwSlideshowVolumeControl::~CShwSlideshowVolumeControl()
	{
	TRACER("CShwSlideshowVolumeControl::~CShwSlideshowVolumeControl");
	GLX_LOG_INFO("CShwSlideshowVolumeControl::~CShwSlideshowVolumeControl" );
	// visuals
	if (iMainVisual)
        {
        iMainVisual->RemoveAndDestroyAllD();
        iMainVisual = NULL;
        }
	// timer
	if ( iTimer )
	    {
	    iTimer->Cancel();
	    }
	delete iTimer;
	}
	
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------	
CShwSlideshowVolumeControl* CShwSlideshowVolumeControl::NewL(
	CAlfEnv& aEnv,
	CGlxUiUtility& aUtility,
    TTimeIntervalMicroSeconds32 aInterval)
	{
	TRACER("CShwSlideshowVolumeControl::NewL");
	GLX_LOG_INFO("CShwSlideshowVolumeControl::NewL" );
	CShwSlideshowVolumeControl* self = new( ELeave ) 
	    CShwSlideshowVolumeControl( aEnv, aUtility, aInterval );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CShwSlideshowVolumeControl::ConstructL()
    {
    TRACER("CShwSlideshowVolumeControl::ConstructL");
    GLX_LOG_INFO("CShwSlideshowVolumeControl::ConstructL" );
	// Construct the base class.
	CAlfControl::ConstructL(iAlfEnv);
	
	//----------------------- LAYOUT HIERARCHY -------------------------------
	//
	//						 	 ->iBackgroundImageVisual
	//							|
	//							|					  ->iSpeakerImageVisual
	//							|					 |
	// iMainVisual-->iVisualDeck ->iValueGridVisual->
	//							|					 |
	//							|					  ->iPercentTextVisual
	//							|
	//							 ->iMuteImageVisual
	//
	//-------------------------------------------------------------------------
	
    iMainVisual = CAlfAnchorLayout::AddNewL(*this);
    
    iVisualDeck = CAlfDeckLayout::AddNewL(*this,iMainVisual);
    
    iValueGridVisual = CAlfGridLayout::AddNewL(*this,KValueGridCols,
    										KValueGridRows,iVisualDeck);
    
    // The visual elements of the control
    iBackgroundImageVisual = CAlfImageVisual::AddNewL(*this,iVisualDeck);
    iMuteImageVisual = CAlfImageVisual::AddNewL(*this,iVisualDeck);
    iSpeakerImageVisual = CAlfImageVisual::AddNewL(*this,iValueGridVisual);
    iPercentTextVisual = CAlfTextVisual::AddNewL(*this,iValueGridVisual);
    
    // BackGround Visual
    iBackgroundImageVisual->EnableBrushesL();
    iBackgroundImageVisual->SetOpacity( KHalfOpacityOpaque );

	// Get the icon file
	TFileName mifFile( KDC_APP_BITMAP_DIR );
    mifFile.Append( KIconsFilename );
    User::LeaveIfError( CompleteWithAppPath( mifFile ) );
    // Get the texture manager
   	CGlxTextureManager& textureMgr = iUtility.GlxTextureManager();
    // Load the background texture
    
    // Below given icon ID is to be changed once the Capped_element Icon
    // is available in the build
    CAlfTexture& backgroundTexture = textureMgr.CreateIconTextureL
        ( EMbmGlxiconsQgn_graf_adapt_search_bg, mifFile );
    
    // apply an image brush to the visual
    iBrush = CAlfImageBrush::NewL(iAlfEnv, TAlfImage( backgroundTexture ) );
    iBackgroundImageVisual->Brushes()->AppendL( iBrush, EAlfHasOwnership );
	iBackgroundImageVisual->SetScaleMode( CAlfImageVisual::EScaleFitInside );
	// Muted visual
	CAlfTexture& textureMuted = iUtility.GlxTextureManager().CreateIconTextureL
	    ( EMbmGlxiconsQgn_indi_mup_speaker_muted, mifFile );										
	iMuteImageVisual->SetImage( textureMuted );
    iMuteImageVisual->SetScaleMode( CAlfImageVisual::EScaleFitInside );
    
	// Speaker visual
	CAlfTexture& textureSpkr = iUtility.GlxTextureManager().CreateIconTextureL
        ( EMbmGlxiconsQgn_indi_mup_speaker, mifFile );
    iSpeakerImageVisual->SetImage( textureSpkr );
    iSpeakerImageVisual->SetScaleMode( CAlfImageVisual::EScaleFitInside );	
	
	//hide the volume level visualation by default
	iMainVisual->SetOpacity(KOpacityTransperent);

	// Only create the timer if an interval's been specified - 
	// default is to not use one.
	if ( iInterval.Int() != 0 )
		{
        iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
		}
    }

// ---------------------------------------------------------------------------
// SetVolume
// ---------------------------------------------------------------------------
void CShwSlideshowVolumeControl::SetVolume(TInt aVolume, TInt aMaxVolume)
	{
	TRACER("CShwSlideshowVolumeControl::SetVolume");
	// Only allow positive values and
	// ensure that aVolume never exceeds aMaxVolume
	if ( aMaxVolume >= 0 && aVolume >= 0 && aVolume <= aMaxVolume )
		{
		iVolume = aVolume;
		iMaxVolume = aMaxVolume;
		}
	}

// ---------------------------------------------------------------------------
// Refresh
// ---------------------------------------------------------------------------
void CShwSlideshowVolumeControl::RefreshL()
	{
	TRACER("CShwSlideshowVolumeControl::RefreshL");
	if( ShowControlL() )
	    {
	    if ( iTimer )
	        {
	        // Restart the timer
	        iTimer->Cancel();
	        iTimer->Start(iInterval, iInterval, TShwCallBack< 
	                CShwSlideshowVolumeControl, 
	                &CShwSlideshowVolumeControl::TimerCallback> (this));
	        }
	    }
	}
	
// ---------------------------------------------------------------------------
// Hide
// ---------------------------------------------------------------------------
void CShwSlideshowVolumeControl::Hide()
	{
	TRACER("CShwSlideshowVolumeControl::Hide");
	// Hide our self
	CAlfDisplay* display = iUtility.Display();
	CAlfControlGroup* group = ControlGroup();

	if ( display && group )
		{
		CAlfRoster& roster = display->Roster();
		roster.Hide( *group );
		}	
	}
// ---------------------------------------------------------------------------
// ShowControlL
// ---------------------------------------------------------------------------
TBool CShwSlideshowVolumeControl::ShowControlL()
    {
    TRACER("CShwSlideshowVolumeControl::Refresh");
    TBool retVal = EFalse;
    // Hide and show our self
    CAlfDisplay* display = iUtility.Display();
    CAlfControlGroup* group = ControlGroup();

    if ( display && group )
        {
        CAlfRoster& roster = display->Roster();
        roster.Hide( *group );
        roster.ShowL( *group, KAlfRosterShowAtTop );
        retVal = ETrue;
        }  
    return retVal;  
    }
	
// ---------------------------------------------------------------------------
// TimerCallback
// ---------------------------------------------------------------------------
TInt CShwSlideshowVolumeControl::TimerCallback()
    {
    TRACER("CShwSlideshowVolumeControl::TimerCallback");
    GLX_LOG_ENTRY_EXIT( "CShwSlideshowVolumeControl::TimerCallback" );
    // Timed out so hide ourself
    Hide();
    if ( iTimer )
        {
        // Cancel the timer
        iTimer->Cancel();
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// NotifyControlVisibility
// ---------------------------------------------------------------------------
void CShwSlideshowVolumeControl::NotifyControlVisibility( TBool aIsVisible,
    CAlfDisplay& /*aDisplay*/ )
	{	
	TRACER("CShwSlideshowVolumeControl::NotifyControlVisibility");
    GLX_LOG_ENTRY_EXIT
        ( "CShwSlideshowVolumeControl::NotifyControlVisibility" );
    GLX_LOG_INFO1
        ( "CShwSlideshowVolumeControl, is visible = %d", aIsVisible );
	if ( !aIsVisible )
		{
		RemoveLayoutVisuals();
		RemoveControlVisuals();
		}
	else
		{
		TRAP_IGNORE( SetAndLayoutVisualsL() );
		}
	}

// ---------------------------------------------------------------------------
// RemoveLayoutVisuals
// ---------------------------------------------------------------------------	
void CShwSlideshowVolumeControl::RemoveLayoutVisuals()
    {
    TRACER("CShwSlideshowVolumeControl::RemoveLayoutVisuals");
    GLX_LOG_ENTRY_EXIT( "CShwSlideshowVolumeControl::RemoveLayoutVisuals" );
	//not required for now
    }

// ---------------------------------------------------------------------------
// RemoveControlVisuals
// ---------------------------------------------------------------------------	
void CShwSlideshowVolumeControl::RemoveControlVisuals()
    {
    TRACER("CShwSlideshowVolumeControl::RemoveControlVisuals");
    GLX_LOG_ENTRY_EXIT( "CShwSlideshowVolumeControl::RemoveControlVisuals" );
    // Remove the visuals visibility
    iMainVisual->SetOpacity(KOpacityTransperent);
    }
	
// ---------------------------------------------------------------------------
// SetAndLayoutVisualsL
// ---------------------------------------------------------------------------	
void CShwSlideshowVolumeControl::SetAndLayoutVisualsL()
    {
    TRACER("CShwSlideshowVolumeControl::SetAndLayoutVisualsL");
    GLX_LOG_ENTRY_EXIT( "CShwSlideshowVolumeControl::SetAndLayoutVisualsL" );

	iMainVisual->SetFlag(EAlfVisualFlagManualSize);
    iMainVisual->SetFlag(EAlfVisualFlagManualPosition);
	//calculate control size
	iControlSize  = iUtility.DisplaySize();
	iControlSize.iHeight *= KControlHeightMultiplier;
	iControlSize.iWidth *= KControlWidthMultiplier;
	//set the control size
	iMainVisual->SetSize(iControlSize);
	
	//calculate the control position
	TInt xPos = ( iUtility.DisplaySize().iWidth - iControlSize.iWidth )
		    / KShwVolumeControlWidthScaleFactor;
	TAlfRealPoint pos( xPos, KShwVolumeControlXOffset );
	iMainVisual->SetPos(pos);
	
	//set the anchors
	User::LeaveIfError( iMainVisual->SetRelativeAnchorRect(
  	0, EAlfAnchorOriginLeft, EAlfAnchorOriginTop, TAlfRealPoint(),
	 EAlfAnchorOriginRight, EAlfAnchorOriginBottom, TAlfRealPoint() ) );
	//update the child visuals
	 iMainVisual->UpdateChildrenLayout();
	 iVisualDeck->UpdateChildrenLayout();
	 iValueGridVisual->UpdateChildrenLayout();
	 
    // Other visuals' layout depends on the volume
    TInt volume = CurrentVolumeAsPercentage();
    
    if (volume == 0)
        {
        // volume = 0% 
        // show muted icon (no %age value)
        SetMuteVisualVisibility();
        }
    else
        {
        // 0 < volume <= 100% 
        // show speaker and %age
        SetValueVisualVisibilityL( volume );
        }
    }

// ---------------------------------------------------------------------------
// SetMuteVisualVisibility
// ---------------------------------------------------------------------------
void CShwSlideshowVolumeControl::SetMuteVisualVisibility()
    {
    TRACER("CShwSlideshowVolumeControl::SetMuteVisualVisibilitys");
    GLX_LOG_ENTRY_EXIT( "CShwSlideshowVolumeControl::SetMuteVisualVisibility" );

	// set the mute visual's visibility to 1 and the value visual to 0
    iValueGridVisual->SetOpacity(KOpacityTransperent);
    iMuteImageVisual->SetOpacity(KOpacityOpaque);
    iMainVisual->SetOpacity(KOpacityOpaque);
    }

// ---------------------------------------------------------------------------
// SetValueVisualVisibilityL
// ---------------------------------------------------------------------------
void CShwSlideshowVolumeControl::SetValueVisualVisibilityL(TInt aVolume)
    {
    TRACER("CShwSlideshowVolumeControl::SetValueVisualVisibilityL");
    GLX_LOG_ENTRY_EXIT( "CShwSlideshowVolumeControl::SetValueVisualVisibilityL" );
   	
   	// update the percentage visual
   	iPercentTextVisual->SetStyle( EAlfTextStyleSmall, EAlfBackgroundTypeDark );
    iPercentTextVisual->SetMaxLineCount( KShwVolumeTextMaxLineCount );
    iPercentTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
   
    // Load the localised string
    HBufC* text = StringLoader::LoadLC( R_SHW_VOLUME_PERCENTAGE, aVolume );
    TPtr ptr = text->Des();
    AknTextUtils::LanguageSpecificNumberConversion( ptr );

    iPercentTextVisual->SetTextL( *text );
    iPercentTextVisual->SetColor(KRgbBlack);
    
    CleanupStack::PopAndDestroy( text );

    iPercentTextVisual->EnableShadow(EFalse);    
    iPercentTextVisual->MoveToFront();
    
    // set the value grid visual's visibility to 1 and the mute visual to 0
    iMuteImageVisual->SetOpacity(KOpacityTransperent);
    iValueGridVisual->SetOpacity(KOpacityOpaque);
    // Text and speaker icon should come top on background
    // so making first in layout's visual list.
    iValueGridVisual->MoveToFront();
    iMainVisual->SetOpacity(KOpacityOpaque);
    }

// ---------------------------------------------------------------------------
// CurrentVolumeAsPercentage()
// ---------------------------------------------------------------------------
TInt CShwSlideshowVolumeControl::CurrentVolumeAsPercentage()
	{
	TRACER("CShwSlideshowVolumeControl::CurrentVolumeAsPercentage");
	GLX_LOG_ENTRY_EXIT("CShwSlideshowVolumeControl::CurrentVolumeAsPercentage");
	TInt retVal = iVolume;

	// guard against any undefined numbers
	if ( iVolume > 0 && iMaxVolume > 0 )
		{
		TReal numerator( iVolume );
		TReal denominator( iMaxVolume );
		TReal working = ( numerator / denominator ) * KShwVolumeMaxPercent;
		retVal = TInt( working );
		}
	else if ( iVolume == iMaxVolume )
		{
		retVal = KShwVolumeMaxPercent;
		}
	return retVal;
	}
// End of File



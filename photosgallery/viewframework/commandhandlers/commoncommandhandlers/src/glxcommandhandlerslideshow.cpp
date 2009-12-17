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
* Description:    Slideshow command handler
*
*/




#include "glxcommandhandlerslideshow.h"

#include <data_caging_path_literals.hrh>
#include <mpxcollectionpath.h>
#include <mpxmediadrmdefs.h>                     // for KMPXMediaDrmProtected
#include <mpxviewutility.h>			 
#include <aknViewAppUi.h>

#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <glxuistd.h>
#include <glxcommandhandlers.hrh>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxattributecontext.h>                 // for CGlxAttributeContext
#include <glxmedia.h>                            // for TGlxMedia
#include <StringLoader.h>                        // for stringloader
#include <glxlog.h>

#include "shwslideshowviewplugin.hrh"		 // for the slideshow view's UID
#include "shwslideshowsettingsplugin_UID.hrh"// for slideshow setting dlg UID
#include "glxuiutility.h"
#include "glxscreenfurniture.h"
#include <glxicons.mbg>
#include <glxsetappstate.h> // set PCFW app state
#include <glxsettingsmodel.h>
#include <glxupnprenderer.h> // get UPnP state
#include <glxgeneraluiutilities.h>
namespace
	{
	const TInt KShwDefaultBufferSize = 128;
	}

// This class does not have access to a CEikonEnv and hence 
// pls ignore the code scanner warning - Using CEikonEnv::Static	
#define GetAppUi() (dynamic_cast<CAknViewAppUi*>(CEikonEnv::Static()->EikAppUi()))

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSlideshow* CGlxCommandHandlerSlideshow::NewL(
    MGlxMediaListProvider* aMediaListProvider, TBool aStepBack, TBool aHasToolbarItem )
    {
    GLX_LOG_INFO( "CGlxCommandHandlerSlideshow::NewL" );
    CGlxCommandHandlerSlideshow* self = new ( ELeave )
    	CGlxCommandHandlerSlideshow(aMediaListProvider, aStepBack, aHasToolbarItem);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerSlideshow::CGlxCommandHandlerSlideshow( MGlxMediaListProvider*
	aMediaListProvider, TBool aStepBack, TBool aHasToolbarItem )
    : CGlxMediaListCommandHandler(aMediaListProvider, aHasToolbarItem), iStepBack(aStepBack)
    {
    // Do nothing
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::ConstructL()
    {
    GLX_LOG_INFO( "CGlxCommandHandlerSlideshow::ConstructL" );
    
    // Get a handle 
    iUiUtility = CGlxUiUtility::UtilityL();

    // Load resource file
	TParse parse;
    parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
   	iResourceOffset = CCoeEnv::Static()->AddResourceFileL( resourceFile );

   // CGlxSettingsModel* model = CGlxSettingsModel::InstanceL();
    iShowInToolbar = ETrue; //model->ShowSlideshowInToolbar();
   // model->Close();

   	// Add supported commands with filter fields
   	// Play slideshow forwards
   	TCommandInfo info( EGlxCmdSlideshowPlay );
   	// Disable for static items and dont enable empty slideshow
    info.iMinSelectionLength = 1;
   	// Enable only for albums that have more than one item
    info.iMinSlideshowPlayableContainedItemCount = 1;
    // Disable for animated GIFs
    info.iDisallowAnimatedGIFs = ETrue;
    // Disable DRM protected content
    info.iDisallowDRM = ETrue;
    // Note! cannot just require all to be images as user can also start 
    // slideshow for a whole album from list view and in that case
    // selection contains a container
	// Disable all videos
	TMPXGeneralCategory categoryFilter = EMPXVideo;
	// Disable the command for videos
	TCommandInfo::TCategoryRule categoryRule = TCommandInfo::EForbidAll;
	info.iCategoryFilter = categoryFilter;
	info.iCategoryRule = categoryRule;
   	AddCommandL( info );
   	
   	// Play slideshow backwards: only the id changes
   	//info.iCommandId = EGlxCmdSlideshowPlayBackwards;
   	//AddCommandL( info );

    // new info to get the default filters
   	TCommandInfo info_show_always( EGlxCmdSlideshowSettings );
   	// Disable for empty views and views with only static items
    info_show_always.iMinSelectionLength = 1;
   	// slideshow settings
   	AddCommandL( info_show_always );

   	// Menu
   	info_show_always.iCommandId = EGlxCmdSlideshow;
   	AddCommandL( info_show_always );

    // Dummy command to get UPnP state
    TCommandInfo infoUpnpState( EGlxCmdShowViaUpnpStateChanged );
    AddCommandL( infoUpnpState );

   	// Buffer
   	iBufFlat = CBufFlat::NewL( KShwDefaultBufferSize );
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerSlideshow::~CGlxCommandHandlerSlideshow()
    {
    GLX_LOG_INFO( "CGlxCommandHandlerSlideshow::~CGlxCommandHandlerSlideshow" );
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }

	if (iUiUtility)
        {
        iUiUtility->Close();
        }

  	delete iBufFlat;
  	delete iBuffer;
    }

// ---------------------------------------------------------------------------
// DoActivateL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::DoActivateL(TInt aViewId)
    {
    GLX_LOG_INFO( "CGlxCommandHandlerSlideshow::DoActivateL" );    
    iViewId = aViewId;

    // for media list item "focus changed" notification
    MGlxMediaList& mediaList( MediaList() );
    mediaList.AddMediaListObserverL( this );
    }


// ----------------------------------------------------------------------------
// Deactivate
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSlideshow::Deactivate()
    {
    GLX_LOG_INFO( "CGlxCommandHandlerSlideshow::Deactivate" );       

    MGlxMediaList& mediaList( MediaList() );
    mediaList.RemoveMediaListObserver( this );
    }

// ----------------------------------------------------------------------------
// IsSlideshowPlayableOnFocusedContainer
// ----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerSlideshow::IsSlideshowNotPlayableOnFocusedContainer
            (TInt aCommandId, MGlxMediaList& aList)
    {
    TBool slideshowDisabled = EFalse;

    // get the media item
    const TGlxMedia& item = aList.Item( aList.FocusIndex() );
    const TCommandInfo& info = CommandInfo(aCommandId);    

    // Check minimum item count if not yet disabled by previous rules
    TInt count(0);        

    if( (info.iMinSlideshowPlayableContainedItemCount )&&
            ( item.GetSlideshowPlayableContainedItemCount(count) ) )
        {
        // disable if less than required amount of items in container 
        // (ignore non-containers, i.e. -1)
        if( count != -1 )
            {
            slideshowDisabled = ( count < info.iMinSlideshowPlayableContainedItemCount );
            }
        }
    return slideshowDisabled;
    }

// -----------------------------------------------------------------------------
// DoExecute - the relevant action for the command id
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerSlideshow::DoExecuteL(TInt aCommandId,
	MGlxMediaList& aList )
	{
    GLX_LOG_INFO( "CGlxCommandHandlerSlideshow::DoExecuteL" );
	TBool handledCommand = ETrue;
	
	switch (aCommandId)
		{
        case EGlxCmdSlideshowPlay:
	        {        
	        // This check has to be done again here since Slideshow can be 
	        // activated from toolbar even when there are non-slideshow playable 
	        // items in a container and when that container is focused.	
	        if( IsSlideshowNotPlayableOnFocusedContainer( aCommandId, aList ) )
	            {
	            HBufC* popupText = NULL;
	            	            
	            //Load the "No Images to Play Slideshow" string from the resource file
	            popupText = StringLoader::LoadLC( R_GLX_NO_IMAGES_TO_PLAY_SLIDESHOW );
	            
	            // Show the Info Note.
	            GlxGeneralUiUtilities::ShowInfoNoteL( popupText->Des(), EFalse );  
	            
	            // LoadLC will push text on to cleanupstack, 
	            // hence it should be poped and destroyed
	            CleanupStack::PopAndDestroy( popupText );
	            }
	        else
	            {
            // Activate the slideshow view to play forwards
            ActivateViewL( NShwSlideshow::EPlayForwards );
            
            // set PCFW app state
            GlxSetAppState::SetState(EGlxInSlideshowView);
	            }
            break;
            }
       /* case EGlxCmdSlideshowPlayBackwards:
            {
            // Activate the slideshow view to play backwards
            ActivateViewL( NShwSlideshow::EPlayBackwards );
            
            // set PCFW app state
            GlxSetAppState::SetState(EGlxInSlideshowView);
            
            break;
            }  */          
		case EGlxCmdSlideshowSettings:
			{
			// In order for the Settings Dialogs Akn StatusPane to become 
			// visible the the Hui display should be deactivated & activated
			// before & after the Settings Dialog (which is uses the Akn 
			// framework) is activated & deactivated respectively.
			// Activating and deactivating the HUI within the Settings 
			// Dialog class will cause a Cone 8 panic.
			
			// hide HUI display
			CGlxUiUtility::HideAlfDisplayL();
				{
				MMPXViewUtility* viewUtility = MMPXViewUtility::UtilityL();
				CleanupClosePushL(*viewUtility);
	            
	            // Activate the slideshow settings dialog
	            viewUtility->ActivateViewL(
	            	TUid::Uid(KShwSettingsDlgImplementationUid));
	            	            
	            CleanupStack::PopAndDestroy(viewUtility);	
				}
			// show HUI display
			CGlxUiUtility::ShowAlfDisplayL();           	
			GetAppUi()->ProcessCommandL(EGlxCmdResetView);
            break;
            }
        case EGlxCmdShowViaUpnpStateChanged:
			{
        	handledCommand = EFalse;
			break;
			}
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
TBool CGlxCommandHandlerSlideshow::DoIsDisabled(
    TInt /*aCommandId*/, MGlxMediaList& /*aList*/) const
    {
    // Disable if UPnP is active
    return ( GlxUpnpRenderer::Status() == NGlxUpnpRenderer::EActive );
    }

// -----------------------------------------------------------------------------
// BypassFiltersForExecute
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerSlideshow::BypassFiltersForExecute() const
    {
    // Always bypass filters to minimise the time spent by the base class
    // when it initialises the slideshow menu item.
    return ETrue;
    }

// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSlideshow::HandleFocusChangedL(
                NGlxListDefs::TFocusChangeType /*aType*/,
                TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/ )
    {
    GLX_FUNC("CGlxCommandHandlerSlideshow::HandleFocusChangedL");

    }

// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::HandleItemAddedL(TInt /*aStartIndex*/,
            TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::HandleMediaL(TInt /*aListIndex*/,
            MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::HandleItemRemovedL(TInt /*aStartIndex*/,
            TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::HandleItemModifiedL(
            const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::HandleAttributesAvailableL(
            TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes,
            MGlxMediaList* /*aList*/ )
    {
    if ( iShowInToolbar && MediaList().FocusIndex() == aItemIndex )
        {
        TIdentityRelation<TMPXAttribute> match( TMPXAttribute::MatchContentId );

        if ( aAttributes.Find( KMPXMediaGeneralCategory, match ) >= 0
            || aAttributes.Find( KMPXMediaDrmProtected, match ) >= 0
            || aAttributes.Find( KGlxMediaGeneralFramecount, match ) >= 0 )
            {
            UpdateToolbar();
            }
        }
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::HandleItemSelectedL(TInt /*aIndex*/,
            TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::HandleMessageL(
            const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/)
    {
    }

// -----------------------------------------------------------------------------
// ActivateViewL - launch the view 
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::ActivateViewL( NShwSlideshow::TPlayDirection
	aPlaybackDirection )
	{
    GLX_LOG_INFO( "CGlxCommandHandlerSlideshow::ActivateViewL" );
    
    // Determine the path from the media list
    CMPXCollectionPath* path = MediaList().PathLC();
	// Ensure the path's at the correct level for the view
	if ( iStepBack && path->Levels() > 0 )
		{
		path->Back();
		}
	// Stream the data buffer
	RBufWriteStream stream;
	stream.Open( *iBufFlat );
	CleanupClosePushL( stream );
	// Write out the playback direction
	stream.WriteInt32L( aPlaybackDirection );
	stream.CommitL();
	// Externalize the path to the stream	
	path->ExternalizeL( stream );
	
	// ActivateViewL takes a TDesC*, rather than a TDesC8
	// so copy the data accordingly
	if ( iBuffer )
		{
		delete iBuffer;
		iBuffer = NULL;
		}
	
	TInt length = iBufFlat->Size();
	iBuffer = HBufC::NewL( length );
	TPtr bufferPtr = iBuffer->Des();
	TPtr8 ptr = iBufFlat->Ptr( 0 );
	bufferPtr.Copy( ptr );

   	// Get the view utility
    MMPXViewUtility* viewUtility = MMPXViewUtility::UtilityL();
	CleanupClosePushL( *viewUtility );
    
    viewUtility->ActivateViewL( TUid::Uid(KShwSlideshowViewImplementationId),
    	iBuffer );
    	
    // set the view navigation direction so that previous view keeps its
    // media list and focus
    iUiUtility->SetViewNavigationDirection( EGlxNavigationForwards );

    CleanupStack::PopAndDestroy( 3, path ); // viewUtility, stream and path
	}

// ---------------------------------------------------------------------------
// UpdateToolbarL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::UpdateToolbar()
    {
    TBool visible = EFalse;

    if ( GlxUpnpRenderer::Status() != NGlxUpnpRenderer::EActive )
        {
        TInt focus = MediaList().FocusIndex();

        if ( focus >= 0 )
            {
            const TGlxMedia& media = MediaList().Item( focus );

            TInt frameCount = 0;
            // get count, ignore return value
            (void)media.GetFrameCount( frameCount );

            // medialistcommandhandler has added these attributes to a low
            // priority fetch context when command was loaded so the values
            // should be loaded
            visible = ( EMPXImage == media.Category()
                    && !media.IsDrmProtected()
                    && 1 == frameCount );
            }
        }

    iUiUtility->ScreenFurniture()->SetToolbarItemVisibility(
            EGlxCmdSlideshowPlay, visible );
    }    
    

// ---------------------------------------------------------------------------
// PopulateToolbar
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerSlideshow::PopulateToolbarL()
	{
	
	iUiUtility->ScreenFurniture()->SetTooltipL( EGlxCmdSlideshowPlay, CAknButton::EPositionLeft );
	}

// End of File

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
* Description:    Video Playback command handler
*
*/




#include "glxcommandhandlervideoplayback.h"

#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <mpxcollectionpath.h>
#include <StringLoader.h>
#include <glxviewpluginuids.hrh>

#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <glxuistd.h>
#include <glxcommandhandlers.hrh>
#include <mpxviewutility.h>
#include <mglxmedialistprovider.h>
#include <glxattributeretriever.h>
#include <glxattributecontext.h>
//#include <ipvideo/ViaPlayerDefaultView.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <alf/alfdisplay.h>
#include <glxuiutility.h>
#include <glxicons.mbg>
#include <glxfetchcontextremover.h>
#include <glxscreenfurniture.h>
  
#include <mpxmessage2.h> 
#include <mpxplaybackmessage.h> 
#include <mpxmessagegeneraldefs.h> 

/**
 * @internal reviewed 11/06/2007 by Alex Birkett
 */

const TUid KVideoHelixPlaybackPluginUid = { 0x10282551 };

#include "glxcommandfactory.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerVideoPlayback* CGlxCommandHandlerVideoPlayback::NewL(
        MGlxMediaListProvider* aMediaListProvider)
    {
    CGlxCommandHandlerVideoPlayback* self = new ( ELeave )
    	CGlxCommandHandlerVideoPlayback(aMediaListProvider);
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
CGlxCommandHandlerVideoPlayback::CGlxCommandHandlerVideoPlayback(MGlxMediaListProvider*
															aMediaListProvider)
    : CGlxMediaListCommandHandler(aMediaListProvider), iIconDisplayState(EUndefined)
    {
    // Do nothing
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::ConstructL()
    {
    iUiUtility = CGlxUiUtility::UtilityL();
    
     
    iViewUtility = MMPXViewUtility::UtilityL(); 

   	// Add supported commands
   	
   	// Play videoplayback
   	TCommandInfo info(EGlxCmdPlay);
	// filter out everything except videos
	info.iCategoryFilter = EMPXVideo;
	info.iCategoryRule = TCommandInfo::ERequireAll;
	info.iMinSelectionLength = 1;
	info.iMaxSelectionLength = 1;
   	AddCommandL(info);
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerVideoPlayback::~CGlxCommandHandlerVideoPlayback()
    {
    if (iUiUtility)
        {
        iUiUtility->Close();
        }

    if ( iViewUtility ) 
        { 
        iViewUtility->Close(); 
        } 

    if ( iPlaybackUtility ) 
        { 
        TRAP_IGNORE( iPlaybackUtility->CommandL( EPbCmdClose ) ); 
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) ); 
        iPlaybackUtility->Close(); 
        } 
    }

// -----------------------------------------------------------------------------
// DoExecute - the relevant action for the command id
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerVideoPlayback::DoExecuteL(TInt aCommandId,
	MGlxMediaList& /*aList*/)
	{
    TRACER("CGlxCommandHandlerVideoPlayback::DoExecuteL()");
	TBool handledCommand = ETrue;
	
	switch (aCommandId)
		{
        case EGlxCmdPlay:
            {
            //Setting the navigational Status to Forwards when Play is activated
            //and is set backwards in ActivatePreviousViewL
            iUiUtility->SetViewNavigationDirection( EGlxNavigationForwards );
            // Activate the videoplayback view
            ActivateViewL();
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
// DoDynInitMenuPaneL 
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::DoDynInitMenuPaneL(TInt /*aResourceId*/, 
        CEikMenuPane* aMenuPane)
    {    
    TInt pos = 0;
    
    // If the menu contains Play command and if the device supports touch interface,
    // hide the Play command in the menu. (Since the user can Play by just tapping on the video 
    // thumbnail in case of grid view and play icon in case of fullscreen view)
    if(iUiUtility->IsPenSupported() && aMenuPane->MenuItemExists(EGlxCmdPlay,pos))
        {
        aMenuPane->SetItemDimmed( EGlxCmdPlay, ETrue);
        }
    }
// -----------------------------------------------------------------------------
// ActivateViewL - launch the ViaPlayer view 
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::ActivateViewL()
	{
    TRACER("CGlxCommandHandlerVideoPlayback::ActivateViewL()");
    
	// get the focused item from the media list
	MGlxMediaList& mediaList = MediaList();

    CGlxDefaultAttributeContext* attributeContext = CGlxDefaultAttributeContext::NewL();
    CleanupStack::PushL(attributeContext);
    attributeContext->AddAttributeL(KMPXMediaGeneralUri);
    attributeContext->AddAttributeL(KMPXMediaGeneralTitle);
    attributeContext->AddAttributeL(KMPXMediaGeneralCategory);
    mediaList.AddContextL(attributeContext, KGlxFetchContextPriorityBlocking);
    
    // TGlxContextRemover will remove the context when it goes out of scope
    // Used here to avoid a trap and still have safe cleanup   
    TGlxFetchContextRemover contextRemover (attributeContext, mediaList);    
	CleanupClosePushL( contextRemover );
	User::LeaveIfError(GlxAttributeRetriever::RetrieveL(*attributeContext, mediaList));
	// context off the list
    CleanupStack::PopAndDestroy( &contextRemover );	
	
	TInt index = mediaList.FocusIndex();
	TGlxMedia  item = mediaList.Item(index);
	const CGlxMedia* media = item.Properties();
	if(media)
		{
        if(media->IsSupported(KMPXMediaGeneralUri))
            {
            if (!iPlaybackUtility)
                {
                GLX_LOG_INFO( "CmdHandler VideoPlayback - SelectPlayerL(+)" );
                const TUid playbackMode = { 0x200009EE };  // photos UID
                iPlaybackUtility = MMPXPlaybackUtility::UtilityL( playbackMode );
                MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
                manager.SelectPlayerL( KVideoHelixPlaybackPluginUid );
                iPlaybackUtility->AddObserverL( *this ); 
                GLX_LOG_INFO( "CmdHandler VideoPlayback - SelectPlayerL(-)" );
                }
            // MPX playbackutility instead of VIA Player 
            const TDesC& filename = media->ValueText(KMPXMediaGeneralUri); 
            // Causes callback to HandlePlaybackMessage() 
            iPlaybackUtility->InitL(filename); 
            }
        }
    CleanupStack::PopAndDestroy(attributeContext);
    }
// -----------------------------------------------------------------------------
// DoActivateL - Activate this command handler
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::DoActivateL(TInt aViewId)
    {
    TRACER("CGlxCommandHandlerVideoPlayback::DoActivateL()");
    iViewId = aViewId;

	MGlxMediaList& mediaList = MediaList();

    mediaList.AddMediaListObserverL(this);
    
    iIconDisplayState = EVisible;

    iAttributeContext = CGlxDefaultAttributeContext::NewL();
    iAttributeContext->SetRangeOffsets(1, 1);
    iAttributeContext->AddAttributeL(KMPXMediaGeneralCategory);
    mediaList.AddContextL( iAttributeContext, KGlxFetchContextPriorityLow );
    
    SetToolBar();
    }
// -----------------------------------------------------------------------------
// Deactivate - Deactivate this command handler
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::Deactivate()
    {
    TRACER("CGlxCommandHandlerVideoPlayback::Deactivate()");
    
    MGlxMediaList& mediaList = MediaList();

    if (iAttributeContext)
        {
        mediaList.RemoveContext(iAttributeContext);
        delete iAttributeContext;
        iAttributeContext = NULL;
        }
        
    mediaList.RemoveMediaListObserver(this);
    }

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL - Sets the toolbar according to the attributes
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::HandleAttributesAvailableL(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /* aList */)
    {
    TRACER("CGlxCommandHandlerVideoPlayback::HandleAttributesAvailableL()");
    if (MediaList().FocusIndex() == aItemIndex )
        {
        TIdentityRelation<TMPXAttribute> match(TMPXAttribute::Match);
        if (KErrNotFound != aAttributes.Find(KMPXMediaGeneralCategory, match))
            {
            SetToolBar();
            }
        }
    }


// -----------------------------------------------------------------------------
// HandleFocusChangedL - Handles a Focus change in the media list
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::HandleFocusChangedL(NGlxListDefs::TFocusChangeType 
        /* aType */, TInt /* aNewIndex */, TInt /* aOldIndex */, MGlxMediaList* /* aList */)
    {
    TRACER("CGlxCommandHandlerVideoPlayback::HandleFocusChangedL()");
    SetToolBar();
    }

// -----------------------------------------------------------------------------
// SetToolBarL - Set the toolbar to the correct state.
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::SetToolBar()
    {
    TRACER("CGlxCommandHandlerVideoPlayback::SetToolBar()");
    TInt focus = MediaList().FocusIndex();
    if ( focus >= 0 )
        {
        // Only show the icon if video is in focus
        TMPXGeneralCategory category = MediaList().Item(focus).Category();
        TBool visible = (EMPXVideo == category);
        
        // Decide if we need to update the toolbar
        TBool UpdateToolBar = EFalse;
        
        if (visible)
            {
            if (iIconDisplayState != EVisible)
                {
                UpdateToolBar = ETrue;
                }
            }
        else
            {
            if (iIconDisplayState != EHidden)
                {
                UpdateToolBar = ETrue;
                }
            }
        
        // Update the toolbar if need be.    
        if (UpdateToolBar)
            {
            iUiUtility->ScreenFurniture()->SetToolbarItemVisibility( EGlxCmdPlay, visible );
            iIconDisplayState = (visible ? EVisible : EHidden);
            }
        }
    }

// -----------------------------------------------------------------------------
// HandlePlaybackMessage 
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
    { 
    TRACER("CGlxCommandHandlerVideoPlayback::HandlePlaybackMessage()"); 

    if ( aError == KErrNone && aMessage ) 
        { 
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) ); 
        } 
    } 
  
// -----------------------------------------------------------------------------
// DoHandlePlaybackMessageL 
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    { 
    TRACER("CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL()");
 
    TMPXMessageId id( *aMessage.Value< TMPXMessageId >( KMPXMessageGeneralId ) ); 

    GLX_LOG_INFO1( "CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL TMPXMessageId =0x%08x",id); 

    if ( KMPXMessageGeneral == id ) 
        { 
        switch ( *aMessage.Value<TInt>( KMPXMessageGeneralEvent ) ) 
            { 
            case TMPXPlaybackMessage::EPlayerChanged: 
                { 
                // Need to call MMPXPlaybackUtility::CurrentPlayer() to get new features 
                GLX_LOG_INFO( "CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL: EPlayerChanged" ); 
                HandlePlaybackPlayerChangedL(); 
                break; 
                } 
            case TMPXPlaybackMessage::EInitializeComplete: 
                { 
                GLX_LOG_INFO( "CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL: EInitializeComplete" ); 
                break; 
                } 
            case TMPXPlaybackMessage::EPlayerUnavailable: 
                { 
                GLX_LOG_INFO( "CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL: EPlayerUnavailable" ); 
                break; 
                } 
            case TMPXPlaybackMessage::EPropertyChanged: 
                { 
                GLX_LOG_INFO( "CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL: EPropertyChanged" ); 
                break; 
                } 
            case TMPXPlaybackMessage::EError: 
                { 
                GLX_LOG_INFO1( "CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL error = %d", 
                        *aMessage.Value<TInt>( KMPXMessageGeneralData ) ); 
                break; 
                } 

            case TMPXPlaybackMessage::EStateChanged: 
                { 
                break; 
                } 
            case TMPXPlaybackMessage::ECommandReceived: // iType=TMPXPlaybackCommand 
            case TMPXPlaybackMessage::ECommandComplete: // iType=TMPXPlaybackCommand 
            case TMPXPlaybackMessage::ESongCorrupt: 
            case TMPXPlaybackMessage::ESongContainerChanged: // Need to call MMPXMedia::CollectionPathL() to get new path 
            case TMPXPlaybackMessage::ESongChanged: // iData=new index; need to query to get new properties (e.g. duration: title etc.) 
            case TMPXPlaybackMessage::EActivePlayerChanged: // iData=active/inactive (ETrue/EFalse); iType (ETrue:rebind to  ActivePlayerMode) 
            case TMPXPlaybackMessage::ESubPlayersChanged: // Need to query to get new list of sub players 
            case TMPXPlaybackMessage::EPlayerSelectionChanged: // Need to query MMPXPlayerManager::GetSelectionL() 
            case TMPXPlaybackMessage::EDownloadStarted: // iData=total expected bytes 
            case TMPXPlaybackMessage::EDownloadUpdated: // iData=total bytes so far 
            case TMPXPlaybackMessage::EDownloadComplete: // iData=total bytes downloaded 
            case TMPXPlaybackMessage::EDownloadPositionChanged: // iData=total bytes so far 
            case TMPXPlaybackMessage::EDownloadStateChanged: // iData=TMPXPlaybackPdDownloadState 
            case TMPXPlaybackMessage::EDownloadCmdPauseDownload: // iData=transaction ID 
            case TMPXPlaybackMessage::EDownloadCmdResumeDownload: // iData=transaction ID 
            case TMPXPlaybackMessage::EDownloadCmdCancelDownload: // iData=transaction ID 
            case TMPXPlaybackMessage::EAccessoryChanged: // iType=TAccessoryMode 
            case TMPXPlaybackMessage::EMediaChanged: // When the song/playlist hasn't: but data may become available 
            case TMPXPlaybackMessage::ESkipping: //skipping event, iData=+1, skipping forward, -1, skipping backward 
            case TMPXPlaybackMessage::ESkipEnd: //skipping end 
            case TMPXPlaybackMessage::EPlaylistUpdated: // playlist updated 
            case TMPXPlaybackMessage::EReachedEndOfPlaylist: // playback or skipping forward has reached end of playlist 
                { 
                GLX_LOG_INFO1( "CGlxCommandHandlerVideoPlayback::DoHandlePlaybackMessageL KMPXMessageGeneralEvent = %d",
                        *aMessage.Value<TInt>( KMPXMessageGeneralEvent ) ); 
                break; 
                } 
            default: 
                { 
                break; 
                } 
            } 
        }  
    } 

// -----------------------------------------------------------------------------
// HandlePlaybackPlayerChangedL 
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerVideoPlayback::HandlePlaybackPlayerChangedL() 
    { 
    TRACER("CGlxCommandHandlerVideoPlayback::HandlePlaybackPlayerChangedL()");

    MMPXPlayer* player = iPlaybackUtility->PlayerManager().CurrentPlayer(); 

    TUid pluginUid( KNullUid ); 
    RArray<TUid> array; 
    CleanupClosePushL( array ); 

    if ( player ) 
        { 
        pluginUid = player->UidL(); 
        array.AppendL( pluginUid ); 
        } 
    // Causes view switch to playback view 
    iViewUtility->ActivateViewL( array ); 
    CleanupStack::PopAndDestroy( &array ); 
    } 


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
* Description:    AppUi class 
*
*/


 

#include "glxappui.h"
#include "glxcachemanager.h"
#include "glxiadupdate.h"

#include <avkon.hrh>
#include <StringLoader.h>    
#include <aknnotewrappers.h>
#include <AknUtils.h>
#include <mpxviewutility.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionpath.h>
#include <centralrepository.h>
#include <alf/alfenv.h>
#include <akntoolbar.h>
#include <glxgallery.hrh>
#include <glxcollectionplugincamera.hrh>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginmonths.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxcollectionpluginimageviewer.hrh>
#include <glxcollectionplugintype.hrh>
#include <glxfiltergeneraldefs.h>
#include <glxpanic.h>
#include <glxuistd.h>
#include <glxviewpluginuids.hrh>
#include <glxlog.h>
#include <glxuiutility.h>
#include <glxsettingsmodel.h>
#include <glxupnprenderer.h>
#include <glxcommandhandlers.hrh>
#include <glxzoomstatepublisher.h>
#include <glxnavigationalstate.h>
#include <glxnavigationalstatedefs.h>
#include <glxfullscreenviewplugin.hrh>
#include <glx.rsg>
#include <AknGlobalNote.h>
#include <hal.h>
#include <hal_data.h>
#include <oommonitorsession.h>
#include <glxtracer.h>

//OOM
#include <oommonitorplugin.h>
#include <oommonitorsession.h>
#include <glxuistd.h>
#include <apgcli.h>

//constants
const TInt KGlxGridThumbnailPages          = 9 ;       // 4 page Down + 4 page Up + 1 Visible page = 9 page 
const TInt KGlxFullThumbnailCount         = 11 ;       // 5 Thumnail Front + 5 Thumbnail Rear + 1 Visible = 11 page
#ifdef __MARM
const TInt KGlxMaxMegaPixelsSupportedByCamera = 5242880 ; // 5 MB
#else
const TInt KGlxMaxMegaPixelsSupportedByCamera = 2097152 ; // 2 MB
#endif
const TInt KGlxMaxMemoryToDecodeCapturedPicture = 2 * KGlxMaxMegaPixelsSupportedByCamera ;
const TInt KGlxMemoryForOOMFwk          = 1048576 ; // 1 MB
const TInt KGlxThumbNailRepresentation    = 2;         // Thumbnail Representation; Could be 3 also 

/**
 * Start Delay for the periodic timer, in microseconds
 */
const TInt KPeriodicStartDelay = 60000000; // 60 secs
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxAppUi::CGlxAppUi()
    {
    }

// -----------------------------------------------------------------------------
// ConstructL is called by the application framework
// -----------------------------------------------------------------------------
//
void CGlxAppUi::ConstructL()
    {
    TRACER("void CGlxAppUi::ConstructL()");

    // Enable Avkon skins.
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    // Create navigational state 
    iNavigationalState = CGlxNavigationalState::InstanceL();
    
    iNavigationalState->AddObserverL( *this );

	iNavigationalState->SetBackExitStatus(EFalse);
    
    iFocusLostLowMemory = EFalse;
    
    // Get an instance of view utility
    iViewUtility = MMPXViewUtility::UtilityL();
    
    iUiUtility = CGlxUiUtility::UtilityL();
    // Always start in default orientation
    iUiUtility->SetAppOrientationL(EGlxOrientationDefault);
    ReserveMemoryL(EEntryTypeStartUp);
    // publish zoom context, no zoom keys for now
    NGlxZoomStatePublisher::PublishStateL( EFalse );
    iEndKeyPressed = EFalse;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxAppUi::~CGlxAppUi()
    {
    TRACER("CGlxAppUi::~CGlxAppUi()");
	iEndKeyPressed = EFalse;
    if ( iNavigationalState )
        {
        iNavigationalState->RemoveObserver(*this);
        iNavigationalState->Close();
        }

    if ( iViewUtility )
        {
        iViewUtility->Close();
        }

    delete iActivationParam;

    if ( iUiUtility )
        {
        iUiUtility->Close();
        }

    if (iPeriodic)
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
    
    if( iIadUpdate )
        {	
    	delete iIadUpdate;
    	}
    }

// -----------------------------------------------------------------------------
// HandleCommandL
// -----------------------------------------------------------------------------
//
void CGlxAppUi::HandleCommandL(TInt aCommand)
    {
    TRACER("void CGlxAppUi::HandleCommandL(TInt aCommand)");
    GLX_LOG_INFO1("PHOTOS LOGS: void CGlxAppUi::HandleCommandL(TInt aCommand = %d)",aCommand );
    switch(aCommand)
        {
        case EEikCmdExit:
            {
            iUiUtility->SetExitingState(ETrue);
            Exit();
            }
            break;
        case EAknSoftkeyExit:
            {
			ClosePhotosL();
            GLX_LOG_INFO("CGlxAppUi::HandleCommandL: Exit() being Called");
            Exit();
            }
            break;

        case EAknSoftkeyBack:
            iNavigationalState->ActivatePreviousViewL();
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CGlxAppUi::HandleMessageL(
                            TUint32 /*aClientHandleOfTargetWindowGroup*/,
                            TUid aMessageUid, const TDesC8& aMessageParameters)
    {
    TRACER("MCoeMessageObserver::TMessageResponse CGlxAppUi::HandleMessageL(TUint32 aClientHandleOfTargetWindowGroup,TUid aMessageUid, const TDesC8& aMessageParameters)");

    // TO DO: Check if gallery is in background
    MCoeMessageObserver::TMessageResponse
                            response = MCoeMessageObserver::EMessageHandled;


    switch ( aMessageUid.iUid )
		{
        case KGlxActivationCmdShowLastModified:
        case KGlxActivationCameraAlbum:
        case KGlxActivationCmdShowAll:
            HandleActivationMessageL(aMessageParameters);
            break;
         
        default:
            //To prevent Continues Activation of the Same View; Same is Triggered in ProcessCommandParametersL
            HandleActivationMessageL(aMessageParameters);           
            break;
        }
    return response;
    }

// ---------------------------------------------------------------------------
// ProcessCommandParametersL
// ---------------------------------------------------------------------------
//
TBool CGlxAppUi::ProcessCommandParametersL(TApaCommand aCommand,
                            TFileName& aDocumentName, const TDesC8& aTail)
    {
    TRACER("TBool CGlxAppUi::ProcessCommandParametersL(TApaCommand aCommand, TFileName& aDocumentName, const TDesC8& aTail)");
    
    TRAPD(err, HandleActivationMessageL(aCommand, aDocumentName, aTail));
    if ( KErrNone != err )
        {
        // Open navigational state at root level
        CMPXCollectionPath* newState = CMPXCollectionPath::NewL();
        CleanupStack::PushL( newState );
        iNavigationalState->NavigateToL( *newState );
        CleanupStack::PopAndDestroy( newState );
        }

    if(0 == aTail.CompareC(KNullDesC8))
        {
      	return ETrue;
        }
    else
        {
        //other case
        return EFalse;
        }

    }

// ---------------------------------------------------------------------------
// Handle navigational state change
// ---------------------------------------------------------------------------
//
void CGlxAppUi::HandleNavigationalStateChangedL()
    {
    TRACER("void CGlxAppUi::HandleNavigationalStateChangedL()");

    /*
     * This thread chk is added for EDDG-7UUC53. In this scenario we get EPathChanged from MPX.
     * That initiates a viewnavigational change from CGlxNavigationalStateImp  HandleCollectionMessageL()
     * In these types of scenarios we don't want a view switch. So ignoring the event. 
     */
    TApaTaskList taskList( iCoeEnv->WsSession() );
	TApaTask task = taskList.FindApp( TUid::Uid( KGlxGalleryApplicationUid ) );
	TApaTask taskForeGround = taskList.FindByPos(0); // get fopreground app
	if ( task.Exists() && task.ThreadId() != taskForeGround.ThreadId() )
		{
		GLX_LOG_INFO("CGlxAppUi::HandleNavigationalStateChanged: Return ");
		return;
		}
    HBufC* activationParam = iActivationParam;
    iActivationParam = NULL; // release ownership
    CleanupStack::PushL( activationParam );

    if ( TUid::Null() != iStartupViewUid )
        {
        GLX_LOG_INFO("CGlxAppUi::HandleNavigationalStateChanged: Activating startup view");
        // Activate startup view
        iViewUtility->ActivateViewL( iStartupViewUid, activationParam );
        iStartupViewUid = TUid::Null();
        }
    else
        {
        GLX_LOG_INFO("CGlxAppUi::HandleNavigationalStateChanged: Activating view");
        
        // get ids for scoring a view
        RArray< TUid > scoringIds;
        CleanupClosePushL( scoringIds );
        GetViewScoringIdsL( scoringIds );
        
        GLX_LOG_INFO1( "CGlxAppUi::HandleNavigationalStateChanged: Uid count %d", scoringIds.Count());
        // let view utility to select the best view based on scoring ids
        iViewUtility->ActivateViewL( scoringIds, activationParam );
    
        CleanupStack::PopAndDestroy( &scoringIds );
        }

    CleanupStack::PopAndDestroy( activationParam );
    }

// ---------------------------------------------------------------------------
// Populate array with view scoring ids
// ---------------------------------------------------------------------------
//
void CGlxAppUi::GetViewScoringIdsL( RArray<TUid>& aIds ) const
    {
    TRACER("void CGlxAppUi::GetViewScoringIdsL( RArray<TUid>& aIds ) const");

    aIds.Reset(); // For maintenance safety

    // get current navigational state
    CMPXCollectionPath* naviState = iNavigationalState->StateLC();
   
      // no if check in needed here ,it makes the aapui aware of the list view depth
      // to be removed.added by gopa   
    if ( naviState->Levels() >= 1)
        {
        if (iNavigationalState->ViewingMode() == NGlxNavigationalState::EBrowse) 
            {
            // For image viewer collection, goto view mode
            if (naviState->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
                {
                aIds.AppendL( TUid::Uid(  KGlxViewingModeView ) );
                }
            else
                {
                aIds.AppendL( TUid::Uid(  KGlxViewingModeBrowse ) );
                }
            } 
        else 
            {
            aIds.AppendL( TUid::Uid(  KGlxViewingModeView ) );
            }                 
        }

    if( TUid::Null() != GetViewScoringIdForCollectionPlugin( *naviState ) )
        {
        // add scoring id for collection plugin
        aIds.AppendL( GetViewScoringIdForCollectionPlugin( *naviState ) );
        }
    
    if( TUid::Null() != ViewScoringIdForNaviStateDepth( *naviState ) )
        {
         // add scoring id for depth in the ui hierarchy
        aIds.AppendL( ViewScoringIdForNaviStateDepth( *naviState ) );
        }
       
     CleanupStack::PopAndDestroy( naviState );
    }
// ---------------------------------------------------------------------------
// Handles the foreground events
// ---------------------------------------------------------------------------
//
 void CGlxAppUi::HandleForegroundEventL( TBool aForeground )
    {
    TRACER("void CGlxAppUi::HandleForegroundEventL( TBool aForeground )");

    // first let base class handle it so that we dont break anything
    CAknViewAppUi::HandleForegroundEventL( aForeground );

    }
// ---------------------------------------------------------------------------
// Return scoring id for collection plugin
// ---------------------------------------------------------------------------
//
TUid CGlxAppUi::GetViewScoringIdForCollectionPlugin( const CMPXCollectionPath& aNaviState ) const
    {
    TRACER("TUid CGlxAppUi::GetViewScoringIdForCollectionPluginL( const CMPXCollectionPath& aNaviState ) const");

    GLX_LOG_INFO1( "CGlxAppUi::GetViewScoringIdForCollectionPluginL: Depth %d", aNaviState.Levels() );
    // score view based on collection plugin if not on root level
    if ( aNaviState.Levels() )
        {
        return TUid::Uid( aNaviState.Id( 0 ) );
        }
    // return null as id to be ignored in scoring
    return TUid::Null(); 
    }


// ---------------------------------------------------------------------------
// Return scoring id for depth
// ---------------------------------------------------------------------------
//
TUid CGlxAppUi::ViewScoringIdForNaviStateDepth( const CMPXCollectionPath& aNaviState ) const
    {
    TRACER("TUid CGlxAppUi::ViewScoringIdForNaviStateDepthL( const CMPXCollectionPath& aNaviState ) const");
    GLX_LOG_INFO1( "CGlxAppUi::ViewScoringIdForNaviStateDepthL: Level %d", aNaviState.Levels() );

    switch ( aNaviState.Levels() )
        {
        case 0:
      
            GLX_LOG_INFO1( "CGlxAppUi::ViewScoringIdForNaviStateDepthL: \
                            Depth  case 0 %x", TUid::Uid( KGlxDepthOne ) );
            return TUid::Uid( KGlxDepthOne );
        
        case 1:
       
            GLX_LOG_INFO1( "CGlxAppUi::ViewScoringIdForNaviStateDepthL: \
                            Depth case 1 %x", TUid::Uid( KGlxDepthTwo ) );
            return TUid::Uid( KGlxDepthTwo );
            

        case 2:

            GLX_LOG_INFO1( "CGlxAppUi::GetViewScoringIdForUiHieararchyDepthL: \
                            Depth case 2 %x", TUid::Uid( KGlxDepthThree ) );
            return TUid::Uid( KGlxDepthThree );

        default:
            GLX_LOG_WARNING( "CGlxAppUi::GetViewScoringIdsL: Navigational state deeper than supported" );
            // return null as an id to be ignored in scoring
            return TUid::Null(); 
        }
    }

// ---------------------------------------------------------------------------
// HandleActivationMessageL
// ---------------------------------------------------------------------------
//
void CGlxAppUi::HandleActivationMessageL(const TApaCommand& aCommand, 
        const TFileName& aDocumentName, const TDesC8& aData)
    {
    TRACER("void CGlxAppUi::HandleActivationMessageL(const TApaCommand& aCommand, const TFileName& aDocumentName, const TDesC8& aData)");
    GLX_LOG_INFO1("Glx-HandleActivationMessageL() aCommand=%d", aCommand);
    GLX_LOG_INFO1("Glx-HandleActivationMessageL() aDocumentName length=%d",
                                                    aDocumentName.Length());

    delete iActivationParam;
    iActivationParam = NULL;

    if (aDocumentName.Length() > 0 && 0 == aData.CompareC(KNullDesC8) )
        {
        GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL() Image Viewer!");        
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL(path);
        iNavigationalState->SetBackExitStatus( EFalse );
        path->AppendL( KGlxCollectionPluginImageViewerImplementationUid );
        iNavigationalState->NavigateToL( *path );
        CleanupStack::PopAndDestroy( path );
        }
    else
        {
        GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL(aData)");        
        HandleActivationMessageL( aData );
        }

    // Introduced to fix bug EMJN-78GH6N. 
    if (0 != iEikonEnv->RootWin().OrdinalPosition())
        {
        iEikonEnv->RootWin().SetOrdinalPosition(0);
        }
    }

// ---------------------------------------------------------------------------
// HandleActivationMessageL
// ---------------------------------------------------------------------------
//
void CGlxAppUi::HandleActivationMessageL(const TDesC8& aData)
    {
    TRACER("void CGlxAppUi::HandleActivationMessageL(const TDesC8& aData)");

    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);

    delete iActivationParam;
    iActivationParam = NULL;

    TUid msgUid;
    RDesReadStream stream(aData);
    CleanupClosePushL(stream);
    stream >> msgUid;
    
    //Start a timer to check for thr IAD update after 30 Secs.
    if(!iPeriodic)
        {
        iPeriodic = CPeriodic::NewL(CActive::EPriorityLow);
        }    
    if ( !iPeriodic->IsActive() )
        {
        iPeriodic->Start( KPeriodicStartDelay, KMaxTInt, 
                TCallBack( &PeriodicCallback, static_cast<TAny*>(this) ) );
        }
    
    switch ( msgUid.iUid )
        {
        case KGlxActivationCmdShowLastModified:
        case KGlxActivationCameraAlbum:
        case KGlxActivationCmdShowAll:
            {
            // Go to All grid view
            GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL: All Grid View");
            // Send the command to reset the view
            ProcessCommandL(EGlxCmdResetView);
            iNavigationalState->SetBackExitStatus(ETrue);
            path->AppendL(KGlxCollectionPluginAllImplementationUid);
            }
            break;

        default:
            GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL: unknown command");
            User::Leave(KErrNotSupported);
        }
    CleanupStack::PopAndDestroy(&stream);
    iNavigationalState->SetStartingLevel(path->Levels());
    iNavigationalState->NavigateToL( *path );
    CleanupStack::PopAndDestroy(path);
    
    // Introduced to fix bug EMJN-78GH6N. 
    if (0 != iEikonEnv->RootWin().OrdinalPosition())
        {
        iEikonEnv->RootWin().SetOrdinalPosition(0);
        }
    }

// ---------------------------------------------------------------------------
// SetActivationParamL
// ---------------------------------------------------------------------------
//
void CGlxAppUi::SetActivationParamL(const TDesC8& aParam)
    {
    TRACER("void CGlxAppUi::SetActivationParamL(const TDesC8& aParam)");
    iActivationParam = HBufC::NewL(aParam.Length());
    iActivationParam->Des().Copy(aParam);
    }

// ---------------------------------------------------------------------------
// RequestFreeMemory
// ---------------------------------------------------------------------------
//
TInt CGlxAppUi::OOMRequestFreeMemoryL( TInt aBytesRequested)
    {
    TRACER("TInt CGlxAppUi::OOMRequestFreeMemoryL( TInt aBytesRequested)");
    GLX_LOG_INFO1("CGlxAppUi::OOMRequestFreeMemoryL() aBytesRequested=%d",
                                                        aBytesRequested);

    ROomMonitorSession oomMonitor;
    User::LeaveIfError( oomMonitor.Connect() );
    // No leaving code after this point, so no need to use cleanup stack
    // for oomMonitor
    TInt errorCode = oomMonitor.RequestFreeMemory( aBytesRequested );
    GLX_LOG_INFO1("CGlxAppUi::OOMRequestFreeMemoryL(1) errorCode=%d",errorCode);
    if ( errorCode != KErrNone )
        {
        // try one more time 
        errorCode = oomMonitor.RequestFreeMemory( aBytesRequested );
        GLX_LOG_INFO1("CGlxAppUi::OOMRequestFreeMemoryL(2) errorCode=%d",errorCode);
        }
    oomMonitor.Close();
    return errorCode;
    }


// ---------------------------------------------------------------------------
// ReserveMemory
// ---------------------------------------------------------------------------
//
TInt CGlxAppUi::ReserveMemoryL(TInt aCriticalMemoryRequired)
    {
    TRACER("void CGlxAppUi::ReserveMemory(TInt aCriticalMemoryRequired)");

    TInt memoryLeft = 0;
    TInt error = KErrNone ; 
    HAL::Get( HALData::EMemoryRAMFree, memoryLeft );
    GLX_LOG_INFO2("CGlxAppUi::ReserveMemoryL() - aCriticalMemoryRequired=%d, memoryLeft=%d",
                                       aCriticalMemoryRequired, memoryLeft);
    if ( aCriticalMemoryRequired > memoryLeft )
        {
        // Request for critical memory required 
        error = OOMRequestFreeMemoryL( aCriticalMemoryRequired);
        GLX_LOG_INFO1("CGlxAppUi::ReserveMemoryL() - OOMRequestFreeMemoryL() error=%d", error);
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CriticalRamMemory
// ---------------------------------------------------------------------------
//
TInt CGlxAppUi::RamRequiredInBytesL(TEntryType aType)
    {
    TRACER("TInt CGlxAppUi::RamRequiredInBytesL(TEntryType aType)");
	TSize gridIconSize = iUiUtility->GetGridIconSize();
    TInt criticalRamMemory = 0 ;
    TSize displaySize = iUiUtility->DisplaySize();
    TInt itemsInPage =  iUiUtility->VisibleItemsInPageGranularityL();
    if(EEntryTypeStartUp == aType)
        {
        // This is Bare Minimum Required Memory for Photos to start 
        // For Framework to work and to do the on-the-fly decoding 
        // for the just captured picture = KGlxMemoryForOOMFwk + KGlxMaxMemoryToDecodeCapturedPicture
        // For Grid Number of pages * Thumbnail width * Thumbnail Height * KGlxThumbNailRepresentation *
        // Number of images in a page +
        // For FullScreen to Work Number of Thumbnails * Width * Height * Representation
        criticalRamMemory =  KGlxMemoryForOOMFwk + KGlxMaxMemoryToDecodeCapturedPicture + 
                                             (KGlxFullThumbnailCount *
                                              displaySize.iWidth * displaySize.iHeight * 
                                              KGlxThumbNailRepresentation ) +
                                             (KGlxGridThumbnailPages *  
                                              itemsInPage * gridIconSize.iWidth *
                                              gridIconSize.iHeight * 
                                              KGlxThumbNailRepresentation);
        
        GLX_LOG_INFO1("CGlxAppUi::RamRequiredInBytesL(EEntryTypeStartUp): criticalRamMemory=%d",
                                                                            criticalRamMemory);
        }
    else
        {
        GLX_LOG_INFO("CGlxAppUi::RamRequiredInBytesL(): Photos Already Running");
        }
 
    return criticalRamMemory;
    }

// ---------------------------------------------------------------------------
// ReserveMemoryL
// ---------------------------------------------------------------------------
//
void CGlxAppUi::ReserveMemoryL(TEntryType aType)
    {
    TRACER("TInt CGlxAppUi::ReserveMemoryL(TEntryType aType)");
    
    TInt error = ReserveMemoryL(RamRequiredInBytesL(aType));
    GLX_LOG_INFO1("CGlxAppUi::ReserveMemoryL() error=%d", error);
    
    if (KErrNoMemory == error)
        {
        GLX_LOG_INFO("CGlxAppUi::ReserveMemoryL(): LEAVE with KErrNoMemory ");
        User::Leave(KErrNoMemory);
        }
    }

//OOM

// ---------------------------------------------------------------------------
// HandleApplicationSpecificEventL
// Handle OOM events.
// ---------------------------------------------------------------------------
//

void CGlxAppUi::HandleApplicationSpecificEventL(TInt aEventType, const TWsEvent& aWsEvent)
{
TRACER("CGlxAppUi::HandleApplicationSpecificEventL");
CAknViewAppUi::HandleApplicationSpecificEventL(aEventType,aWsEvent);
	if(aWsEvent.Type() == KAknUidValueEndKeyCloseEvent)
		{
		/**
		* Making the flag etrue ensures that it will not call LaunchMmViewL in handlecommandl while 
		* red key is pressed. Otherwise it will launch matrix menu, not idle screen. 
		*/
		iEndKeyPressed = ETrue;
		ClosePhotosL();
		}
switch(aEventType)	
	{
	case KAppOomMonitor_FreeRam:
		{
		StartCleanupL();				
		}
		break;
	case KAppOomMonitor_MemoryGood:
		{
		StopCleanupL();
		}
		break;
	default:
		break;
	}
}

// ---------------------------------------------------------------------------
// StartCleanup
// Initiate cleanup from cache manager on low memory event
// ---------------------------------------------------------------------------
//
void CGlxAppUi::StartCleanupL()
    {
    MGlxCache* cacheManager = MGlxCache::InstanceL();
    cacheManager->ReleaseRAML();
    cacheManager->Close();
    }

// ---------------------------------------------------------------------------
// StopCleanup
// Cancel cache cleanup on memory good event
// ---------------------------------------------------------------------------
//
void CGlxAppUi::StopCleanupL()
    {
    MGlxCache* cacheManager = MGlxCache::InstanceL();
    cacheManager->StopRAMReleaseL();
    cacheManager->Close();
    }

// -----------------------------------------------------------------------------
// Callback from periodic timer
// -----------------------------------------------------------------------------
//
TInt CGlxAppUi::PeriodicCallback(TAny* aPtr)
    {
    TRACER("CGlxAppUi::PeriodicCallback(TAny* aPtr)");
    // get "this" pointer    
    static_cast<CGlxAppUi*>(aPtr)->PeriodicCallback();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Callback from periodic timer, non-static
// -----------------------------------------------------------------------------
//
void CGlxAppUi::PeriodicCallback()
    {
    TRACER("CGlxAppUi::PeriodicCallback()");
    TRAP_IGNORE(DoCheckForIADUpdatesL());
    iPeriodic->Cancel();
    }    

// ---------------------------------------------------------------------------
// CCGlxNsAppUi::DoCheckForIADUpdatesL()
// Check for updates via IAD
// ---------------------------------------------------------------------------
// 
void CGlxAppUi::DoCheckForIADUpdatesL()
    {
    TRACER("CGlxAppUi::DoCheckForIADUpdatesL()");
#ifdef _DEBUG
    TTime startTime;
    GLX_LOG_INFO("CGlxAppUi::DoCheckForIADUpdatesL(+)");  
    startTime.HomeTime();
#endif
    if ( !iIadUpdate )
        {
        iIadUpdate = CGlxIadUpdate::NewL();
        }
    iIadUpdate->StartL();
#ifdef _DEBUG
    TTime stopTime;
    stopTime.HomeTime();
    GLX_DEBUG2("CGlxAppUi::DoCheckForIADUpdatesL(-) took <%d> us", 
                    (TInt)stopTime.MicroSecondsFrom(startTime).Int64());
#endif    
    }

// -----------------------------------------------------------------------------
// ClosePhotosL
// -----------------------------------------------------------------------------
//
void CGlxAppUi::ClosePhotosL()
    {
	TRACER("CGlxNsAppUi::ClosePhotosL()");
	iUiUtility->SetViewNavigationDirection(EGlxNavigationBackwards);
	//Temparory fix provided for Kern Exec 3 bug
	//UPnP Stop Showin is explicitly called when exiting the gallery
	GlxUpnpRenderer:: StopShowingL();

	iUiUtility->SetExitingState(ETrue);         
	GLX_LOG_INFO("CGlxAppUi::HandleWsEventL: Exit() for C key being Called");	

    }
//OOM

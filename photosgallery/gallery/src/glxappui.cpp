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
#include "glxcachemanager.h"//OOM
//OOM

#include <oommonitorplugin.h>
#include <oommonitorsession.h>

//OOM


#include <glxbackservicewrapper.h>


#include <glxuistd.h>
#include <apgcli.h>
#include "glxiadupdate.h"

//constants
const TInt KGlxGridThumbnailPages          = 9 ;       // 4 page Down + 4 page Up + 1 Visible page = 9 page 
const TInt KGlxFullThumbnailCount         = 11 ;       // 5 Thumnail Front + 5 Thumbnail Rear + 1 Visible = 11 page
const TInt KGlxMaxMegaPixelsSupportedByCamera = 5242880 ; // 5 MB
const TInt KGlxMaxMemoryToDecodeCapturedPicture = 2 * KGlxMaxMegaPixelsSupportedByCamera ;
const TInt KGlxMemoryForOOMFwk          = 1048576 ; // 1 MB
const TInt KGlxThumbNailRepresentation    = 2;         // Thumbnail Representation; Could be 3 also 

_LIT8( KPhotosSuiteNavigation, "SuiteNavigation" );
_LIT8( KPhotosCaptured, "Captured" );
_LIT8( KPhotosMonths, "Months" );
_LIT8( KPhotosTags, "Tags" );
_LIT8( KPhotosAlbums, "Albums" );
_LIT8( KPhotosAllValue,"Allcs");

_LIT8( KPhotosSuiteExitMessage, "mm://photossuite?action=exit" );

// Matrix uid, needed for activating the suite view.
const TInt KMatrixUid = 0x101F4CD2;
const TInt KCapturedAlbumId = 2 ;
        
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

    // Create Back Stepping Service wrapper
	iBSWrapper = CGlxBackServiceWrapper::NewL( TUid::Uid( KGlxGalleryApplicationUid ) );
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

    if( iBSWrapper )
        {	
        delete iBSWrapper;    
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
            // Send message to Menu and exit.
            /**
            * In case of exit is pressed from options menu of photos grid, this flag will be 
            * false. so it will launch matrix menu. If we don't call LaunchMmViewL, in the above 
            * scenario, photos will exit,but photosuite will come. This is misleading to user.
            */
            if(!iEndKeyPressed)
	            {
	            LaunchMmViewL( KPhotosSuiteExitMessage );	
	            }
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
            HandleActivationMessageL(aMessageParameters);
            break;
         
        case KGlxActivationPhotosMenu:
        case KGlxActivationCameraView:
        case KGlxActivationMonthsView:
        case KGlxActivationAlbumsView:
        case KGlxActivationTagsView:
        case KGlxActivationAllView:
        	{
            TApaTaskList taskList( iCoeEnv->WsSession() );
        	TApaTask task = taskList.FindApp( TUid::Uid( KGlxGalleryApplicationUid ) );
        	TApaTask taskForeGround = taskList.FindByPos(0); // get fopreground app
	        if ( task.Exists() && task.ThreadId() != taskForeGround.ThreadId() )
		        {
                // No matter which collection is selected,
                // Photos is running in background, bring to foreground
                iEikonEnv->RootWin().SetOrdinalPosition(0);
		        }
        	}
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
        // Open collection for main view
        iStartupViewUid = TUid::Null();
        // Open navigational state at root level
        CMPXCollectionPath* newState = CMPXCollectionPath::NewL();
        CleanupStack::PushL( newState );
        iNavigationalState->NavigateToL( *newState );
        CleanupStack::PopAndDestroy( newState );
        }

    return (EApaCommandOpen == aCommand) && (0 != aDocumentName.Size());
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

    delete iActivationParam;
    iActivationParam = NULL;

    //Check for the IADUpdate
    //TBD: Need to check the location this has to be called.
    //This might not be proper place.
    DoCheckForIADUpdatesL();

    if ( aDocumentName.Length() > 0 && EApaCommandOpen == aCommand )
        {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL(path);
        iNavigationalState->SetBackExitStatus( EFalse );
        path->AppendL( KGlxCollectionPluginImageViewerImplementationUid );
        iNavigationalState->NavigateToL( *path );
        CleanupStack::PopAndDestroy( path );
        }
    else
        {
        HandleActivationMessageL( aData );
        }

    // Introduced to fix bug EMJN-78GH6N. Rowland Cook 10/12/2007
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
    
    //Check for the IADUpdate
    //TBD: Need to check the location this has to be called.
    //This might not be proper place.
    DoCheckForIADUpdatesL();
    
    switch ( msgUid.iUid )
        {
        case KGlxActivationCmdShowLastModified:
            // Go to camera album full screen view
            GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL: show last modified");
            // Send the command to reset the view
            ProcessCommandL(EGlxCmdResetView);
            // Not using KGlxCollectionPluginCameraImplementationUid
            path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);            
            path->AppendL(KCapturedAlbumId);
            SetActivationParamL(KGlxActivationFullScreen);
            break;

        case KGlxActivationCameraAlbum:
            // Go to camera album tile view
            GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL: camera album");
            path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
            path->AppendL(KCapturedAlbumId);
            break;

        case KGlxActivationPhotosMenu:
            // Open the main view
            GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL: photos menu");
            break;            

        default:
            GLX_LOG_INFO("CGlxAppUi::HandleActivationMessageL: unknown command");


            if(0 == aData.CompareC(KPhotosCaptured))
                {
                iNavigationalState->SetBackExitStatus(ETrue);
                path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
				path->AppendL(KCapturedAlbumId);
                }
            else if(0 == aData.CompareC(KPhotosAllValue))
                {
                iNavigationalState->SetBackExitStatus(ETrue);
                path->AppendL(KGlxCollectionPluginAllImplementationUid);
                }
            else if(0 == aData.CompareC(KPhotosMonths))
                {
                iNavigationalState->SetBackExitStatus(ETrue);
                path->AppendL(KGlxCollectionPluginMonthsImplementationUid);
                }
            else if(0 == aData.CompareC(KPhotosAlbums))
                {
                iNavigationalState->SetBackExitStatus(ETrue);
                path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
                }
            else if(0 == aData.CompareC(KPhotosTags))
                {
                iNavigationalState->SetBackExitStatus(ETrue);
                path->AppendL(KGlxTagCollectionPluginImplementationUid);
                }
            else
                {
                User::Leave(KErrNotSupported);
                }
            TBuf8<15> buf;
            buf.Append( KPhotosSuiteNavigation );
            TRAP_IGNORE(iBSWrapper->ForwardActivationEventL( buf, ETrue ))

            break;
        }
    CleanupStack::PopAndDestroy(&stream);
    iNavigationalState->NavigateToL( *path );
    CleanupStack::PopAndDestroy(path);
    
    // Introduced to fix bug EMJN-78GH6N. Rowland Cook 10/12/2007
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

// ---------------------------------------------------------------------------
// CCGlxNsAppUi::DoCheckForIADUpdatesL()
// Check for updates via IAD
// ---------------------------------------------------------------------------
// 
void CGlxAppUi::DoCheckForIADUpdatesL()
    {
    TRACER("CGlxNsAppUi::CheckForUpdatesL()");
    
    if ( !iIadUpdate )
        {
        iIadUpdate = CGlxIadUpdate::NewL();
        }
    iIadUpdate->StartL();
    }

// ---------------------------------------------------------------------------
// LaunchMmViewL
//
// ---------------------------------------------------------------------------
//
void CGlxAppUi::LaunchMmViewL( const TDesC8& aMessage )
    {
 	TRACER("CGlxNsAppUi::LaunchMmViewL()");
	TApaTaskList taskList( iCoeEnv->WsSession() );
	TApaTask task = taskList.FindApp( TUid::Uid( KMatrixUid ) );

	if ( task.Exists() )
		{
		task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ),aMessage );
		}
	else
		{ // app not yet running
		RApaLsSession appArcSession;
		CleanupClosePushL( appArcSession );
		User::LeaveIfError( appArcSession.Connect() );      
		TApaAppInfo appInfo;
		TInt err = appArcSession.GetAppInfo( appInfo, 
		TUid::Uid( KMatrixUid ) );
		if( err == KErrNone )
			{
			CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
			cmdLine->SetExecutableNameL( appInfo.iFullName );
			cmdLine->SetCommandL( EApaCommandRun );
			cmdLine->SetTailEndL( aMessage );
			appArcSession.StartApp( *cmdLine );
			CleanupStack::PopAndDestroy( cmdLine );
			}
		CleanupStack::PopAndDestroy( &appArcSession ); 
		}
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

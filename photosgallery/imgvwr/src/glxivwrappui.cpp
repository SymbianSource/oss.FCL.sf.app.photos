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
 * Description: Image Viewer AppUi class 
 *
 */




#include "glxivwrappui.h"
#include "glxivwrdocument.h"

#include <apgcli.h>
#include <hal.h>
#include <oommonitorsession.h>
#include <mpxviewutility.h>
#include <mpxcollectionpath.h>

#include <glxmedialist.h>
#include <glxcollectionpluginimageviewer.hrh>
#include <glxviewpluginuids.hrh>
#include <glxuiutility.h>
#include <glxzoomstatepublisher.h>
#include <glxnavigationalstate.h>
#include <glxnavigationalstatedefs.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <glxivwr.rsg>



//constants
const TInt KGlxFullThumbnailCount         = 1 ;       // 1 visible thumnail
#ifdef __MARM
const TInt KGlxMaxMegaPixelsSupportedByCamera = 5242880 ; // 5 MB
#else
const TInt KGlxMaxMegaPixelsSupportedByCamera = 2097152 ; // 2 MB
#endif
const TInt KGlxMaxMemoryToDecodeCapturedPicture = 2 * KGlxMaxMegaPixelsSupportedByCamera ;
const TInt KGlxMemoryForOOMFwk          = 1048576 ; // 1 MB
const TInt KGlxThumbNailRepresentation    = 4;         // Thumbnail Representation; Could be 3 also 

// UID for the application, 
// this should correspond to the uid defined in the mmp file
static const TUid KUidGlxIvwrApp = {0x200104E7};

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxIVwrAppUi::CGlxIVwrAppUi()
    {
    }

// -----------------------------------------------------------------------------
// ConstructL is called by the application framework
// -----------------------------------------------------------------------------
//
void CGlxIVwrAppUi::ConstructL()
    {
    TRACER("void CGlxIVwrAppUi::ConstructL()");

    // Enable Avkon skins.
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );

    // Create navigational state 
    iNavigationalState = CGlxNavigationalState::InstanceL();
    iNavigationalState->AddObserverL( *this );
    iNavigationalState->SetBackExitStatus(EFalse);

    // Get an instance of view utility
    iViewUtility = MMPXViewUtility::UtilityL();

    iUiUtility = CGlxUiUtility::UtilityL();
    // Always start in default orientation
    iUiUtility->SetAppOrientationL(EGlxOrientationDefault);

    ReserveMemoryL(EEntryTypeStartUp);
    
    // publish zoom context, no zoom keys for now
    NGlxZoomStatePublisher::PublishStateL( EFalse );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxIVwrAppUi::~CGlxIVwrAppUi()
    {
    TRACER("CGlxIVwrAppUi::~CGlxIVwrAppUi()");
    if ( iNavigationalState )
        {
        iNavigationalState->RemoveObserver(*this);
        iNavigationalState->Close();
        }

    if ( iViewUtility )
        {
        iViewUtility->Close();
        }

    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// HandleCommandL
// -----------------------------------------------------------------------------
//
void CGlxIVwrAppUi::HandleCommandL(TInt aCommand)
    {
    TRACER("void CGlxIVwrAppUi::HandleCommandL(TInt aCommand)");
    GLX_LOG_INFO1("void CGlxIVwrAppUi::HandleCommandL(aCommand=%d)", aCommand );
    switch (aCommand)
        {
        case EEikCmdExit:
            {
            Exit();
            }
            break;
        case EAknSoftkeyExit:
            {
            CloseImgVwr();
            GLX_LOG_INFO("CGlxIVwrAppUi::HandleCommandL: Exit() being Called");
            Exit();
            }
            break;

        case EAknSoftkeyBack:
            {
            iNavigationalState->ActivatePreviousViewL();
            }
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// ProcessCommandParametersL
// ---------------------------------------------------------------------------
//
TBool CGlxIVwrAppUi::ProcessCommandParametersL(TApaCommand /*aCommand*/,
        TFileName& /*aDocumentName*/, const TDesC8& /*aTail*/)
    {
    TRACER("TBool CGlxIVwrAppUi::ProcessCommandParametersL()");

    // Bring the application to foreground, if not already
    if (0 != iEikonEnv->RootWin().OrdinalPosition())
        {
        iEikonEnv->RootWin().SetOrdinalPosition(0);
        }

	//To call OpenFileL in document class it must retun ETrue
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Handle navigational state change
// ---------------------------------------------------------------------------
//
void CGlxIVwrAppUi::HandleNavigationalStateChangedL()
    {
    TRACER("void CGlxIVwrAppUi::HandleNavigationalStateChangedL()");
    // get current navigational state
    CMPXCollectionPath* naviState = iNavigationalState->StateLC();

    // Case: Open photos in grid, go to filemanager via menu and open any image
    if (naviState->Id() != TMPXItemId(
            KGlxCollectionPluginImageViewerImplementationUid))
        {
        /*
         * This thread chk is added for EDDG-7UUC53. In this scenario we get EPathChanged from MPX.
         * That initiates a viewnavigational change from CGlxNavigationalStateImp  HandleCollectionMessageL()
         * In these types of scenarios we don't want a view switch. So ignoring the event. 
         */
        TApaTaskList taskList(iCoeEnv->WsSession());
        TApaTask task = taskList.FindApp(KUidGlxIvwrApp);
        TApaTask taskForeGround = taskList.FindByPos(0); // get fopreground app
        if (task.Exists() && task.ThreadId() != taskForeGround.ThreadId())
            {
            GLX_LOG_INFO("CGlxIVwrAppUi ::HandleNavigationalStateChanged: Return ");
            return;
            }
        }

    CleanupStack::PopAndDestroy(naviState);

    if (TUid::Null() != iStartupViewUid)
        {
        GLX_LOG_INFO("CGlxIVwrAppUi::HandleNavigationalStateChanged:Activating startup view");
        // Activate startup view
        iViewUtility->ActivateViewL(iStartupViewUid, NULL);
        iStartupViewUid = TUid::Null();
        }
    else
        {
        GLX_LOG_INFO("CGlxIVwrAppUi::HandleNavigationalStateChanged:Activating view");

        // get ids for scoring a view
        RArray<TUid> scoringIds;
        CleanupClosePushL(scoringIds);
        GetViewScoringIdsL(scoringIds);

        GLX_LOG_INFO1("CGlxIVwrAppUi::HandleNavigationalStateChanged:Uid count %d", scoringIds.Count());
        // let view utility to select the best view based on scoring ids
        iViewUtility->ActivateViewL(scoringIds, NULL);

        CleanupStack::PopAndDestroy(&scoringIds);
        }
    }

// ---------------------------------------------------------------------------
// Populate array with view scoring ids
// ---------------------------------------------------------------------------
//
void CGlxIVwrAppUi::GetViewScoringIdsL( RArray<TUid>& aIds ) const
    {
    TRACER("void CGlxIVwrAppUi::GetViewScoringIdsL()");
    CleanupClosePushL(aIds);
    aIds.Reset(); // For maintenance safety

    // get current navigational state
    CMPXCollectionPath* naviState = iNavigationalState->StateLC();

    // no if check in needed here ,it makes the aapui aware of the list view depth
    // to be removed.added by gopa   
    if (naviState->Levels() >= 1)
        {
        if (iNavigationalState->ViewingMode()
                == NGlxNavigationalState::EBrowse)
            {
            // For image viewer collection, goto view mode
            if (naviState->Id() == TMPXItemId(
                    KGlxCollectionPluginImageViewerImplementationUid))
                {
                aIds.AppendL(TUid::Uid(KGlxViewingModeView));
                }
            else
                {
                aIds.AppendL(TUid::Uid(KGlxViewingModeBrowse));
                }
            }
        else
            {
            aIds.AppendL(TUid::Uid(KGlxViewingModeView));
            }
        }

    if (TUid::Null() != GetViewScoringIdForCollectionPlugin(*naviState))
        {
        // add scoring id for collection plugin
        aIds.AppendL(GetViewScoringIdForCollectionPlugin(*naviState));
        }

    if (TUid::Null() != ViewScoringIdForNaviStateDepth(*naviState))
        {
        // add scoring id for depth in the ui hierarchy
        aIds.AppendL(ViewScoringIdForNaviStateDepth(*naviState));
        }

    CleanupStack::PopAndDestroy(naviState);
    CleanupStack::Pop(&aIds);
    }

// ---------------------------------------------------------------------------
// Return scoring id for collection plugin
// ---------------------------------------------------------------------------
//
TUid CGlxIVwrAppUi::GetViewScoringIdForCollectionPlugin(
        const CMPXCollectionPath& aNaviState) const
    {
    TRACER("TUid CGlxIVwrAppUi::GetViewScoringIdForCollectionPluginL()");

    GLX_LOG_INFO1( "CGlxIVwrAppUi::GetViewScoringIdForCollectionPluginL:Depth %d", aNaviState.Levels() );
    // score view based on collection plugin if not on root level
    if (aNaviState.Levels())
        {
        return TUid::Uid(aNaviState.Id(0));
        }
    // return null as id to be ignored in scoring
    return TUid::Null();
    }

// ---------------------------------------------------------------------------
// Return scoring id for depth
// ---------------------------------------------------------------------------
//
TUid CGlxIVwrAppUi::ViewScoringIdForNaviStateDepth(
        const CMPXCollectionPath& aNaviState) const
    {
    TRACER("TUid CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL()");
    GLX_LOG_INFO1( "CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL: Level %d",aNaviState.Levels() );

    switch (aNaviState.Levels())
        {
        case 0:

            GLX_LOG_INFO1( "CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL:Depth  case 0 %x", TUid::Uid( KGlxDepthOne ) );
            return TUid::Uid(KGlxDepthOne);

        case 1:

            GLX_LOG_INFO1( "CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL:Depth case 1 %x", TUid::Uid( KGlxDepthTwo ) );
            return TUid::Uid(KGlxDepthTwo);

        case 2:

            GLX_LOG_INFO1( "CGlxIVwrAppUi::GetViewScoringIdForUiHieararchyDepthL:Depth case 2 %x", TUid::Uid( KGlxDepthThree ) );
            return TUid::Uid(KGlxDepthThree);

        default:
            GLX_LOG_WARNING( "CGlxIVwrAppUi::GetViewScoringIdsL:Navigational state deeper than supported" );
            // return null as an id to be ignored in scoring
            return TUid::Null();
        }
    }

// ---------------------------------------------------------------------------
// HandleOpenFileL
// ---------------------------------------------------------------------------
//
void CGlxIVwrAppUi::HandleOpenFileL()
    {
    TRACER("void CGlxIVwrAppUi::HandleOpenFileL()");
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    path->AppendL( KGlxCollectionPluginImageViewerImplementationUid );
    iNavigationalState->NavigateToL( *path );
    CleanupStack::PopAndDestroy( path );    
    }

// ---------------------------------------------------------------------------
// OOMRequestFreeMemoryL
// ---------------------------------------------------------------------------
//
TInt CGlxIVwrAppUi::OOMRequestFreeMemoryL(TInt aBytesRequested)
    {
    TRACER("TInt CGlxIVwrAppUi::OOMRequestFreeMemoryL(TInt aBytesRequested)");
    GLX_LOG_INFO1("CGlxIVwrAppUi::OOMRequestFreeMemoryL() aBytesRequested=%d",aBytesRequested);

    ROomMonitorSession oomMonitor;
    User::LeaveIfError( oomMonitor.Connect() );
    // No leaving code after this point, so no need to use cleanup stack
    // for oomMonitor
    TInt errorCode = oomMonitor.RequestFreeMemory( aBytesRequested );
    GLX_LOG_INFO1("CGlxIVwrAppUi::OOMRequestFreeMemoryL(1) errorCode=%d",errorCode);
    if ( errorCode != KErrNone )
        {
        // try one more time 
        errorCode = oomMonitor.RequestFreeMemory( aBytesRequested );
        GLX_LOG_INFO1("CGlxIVwrAppUi::OOMRequestFreeMemoryL(2) errorCode=%d",errorCode);
        }
    oomMonitor.Close();
    return errorCode;
    }

// ---------------------------------------------------------------------------
// ReserveMemoryL
// ---------------------------------------------------------------------------
//
TInt CGlxIVwrAppUi::ReserveMemoryL(TInt aCriticalMemoryRequired)
    {
    TRACER("void CGlxIVwrAppUi::ReserveMemoryL(TInt aCriticalMemoryRequired)");

    TInt memoryLeft = 0;
    TInt error = KErrNone ; 
    HAL::Get( HALData::EMemoryRAMFree, memoryLeft );
    GLX_LOG_INFO2("CGlxIVwrAppUi::ReserveMemoryL() - aCriticalMemoryRequired=%d, memoryLeft=%d", aCriticalMemoryRequired, memoryLeft);
    if ( aCriticalMemoryRequired > memoryLeft )
        {
        // Request for critical memory required 
        error = OOMRequestFreeMemoryL( aCriticalMemoryRequired);
        GLX_LOG_INFO1("CGlxIVwrAppUi::ReserveMemoryL() - OOMRequestFreeMemoryL() error=%d", error);
        }
    return error;
    }

// ---------------------------------------------------------------------------
// RamRequiredInBytesL
// ---------------------------------------------------------------------------
//
TInt CGlxIVwrAppUi::RamRequiredInBytesL(TEntryType aType)
    {
    TRACER("TInt CGlxIVwrAppUi::RamRequiredInBytesL(TEntryType aType)");
    TInt criticalRamMemory = 0 ;
    TSize displaySize = iUiUtility->DisplaySize();
    if(EEntryTypeStartUp == aType)
        {
        // This is Bare Minimum Required Memory for Photos to start 
        // For Framework to work and to do the on-the-fly decoding 
        // for the just captured picture = KGlxMemoryForOOMFwk + KGlxMaxMemoryToDecodeCapturedPicture
        // For FullScreen to Work Number of Thumbnail(s) * Width * Height * Representation
        criticalRamMemory = KGlxMemoryForOOMFwk
                + KGlxMaxMemoryToDecodeCapturedPicture
                + (KGlxFullThumbnailCount * displaySize.iWidth
                        * displaySize.iHeight * KGlxThumbNailRepresentation);

        GLX_LOG_INFO1("CGlxIVwrAppUi::RamRequiredInBytesL(EEntryTypeStartUp):criticalRamMemory=%d", criticalRamMemory);
        }
    else
        {
        GLX_LOG_INFO("CGlxIVwrAppUi::RamRequiredInBytesL():Viewer Already Running");
        }
 
    return criticalRamMemory;
    }

// ---------------------------------------------------------------------------
// ReserveMemoryL
// ---------------------------------------------------------------------------
//
void CGlxIVwrAppUi::ReserveMemoryL(TEntryType aType)
    {
    TRACER("TInt CGlxIVwrAppUi::ReserveMemoryL(TEntryType aType)");
    
    TInt error = ReserveMemoryL(RamRequiredInBytesL(aType));
    GLX_LOG_INFO1("CGlxIVwrAppUi::ReserveMemoryL() error=%d", error);
    
    if (KErrNoMemory == error)
        {
        GLX_LOG_INFO("CGlxIVwrAppUi::ReserveMemoryL():LEAVE with KErrNoMemory");
        User::Leave(KErrNoMemory);
        }
    }

// ---------------------------------------------------------------------------
// HandleApplicationSpecificEventL
// 
// ---------------------------------------------------------------------------
//
void CGlxIVwrAppUi::HandleApplicationSpecificEventL(TInt aEventType, 
        const TWsEvent& aWsEvent)
    {
    TRACER("CGlxIVwrAppUi::HandleApplicationSpecificEventL");
    CAknViewAppUi::HandleApplicationSpecificEventL(aEventType,aWsEvent);
    if(aWsEvent.Type() == KAknUidValueEndKeyCloseEvent)
        {
        CloseImgVwr();
        }
    }

// -----------------------------------------------------------------------------
// CloseImgVwr
// -----------------------------------------------------------------------------
//
void CGlxIVwrAppUi::CloseImgVwr()
    {
    TRACER("CGlxIVwrAppUi::CloseImgVwr()");
    iUiUtility->SetViewNavigationDirection(EGlxNavigationBackwards);
    iUiUtility->SetExitingState(ETrue);         
    }

// ----------------------------------------------------------------------------
// CGlxIVwrAppUi::OpenFileL
// This is called by framework when application is already open in background
// and user open other file in eg. File Browse.
// New file to been shown is passed via aFileName.
// ----------------------------------------------------------------------------
//
void CGlxIVwrAppUi::OpenFileL(const TDesC& aFileName)
    {
    TRACER("CGlxIVwrAppUi::OpenFileL()");
    GLX_LOG_URI("CGlxIVwrAppUi::OpenFileL(%S)", &aFileName);

    // File changed. Open new file with documents OpenFileL method.
    Document()->OpenFileL(EFalse, aFileName, iEikonEnv->FsSession());

    GLX_DEBUG1("CGlxIVwrAppUi::OpenFileL() *** File Changed *** ");
    MGlxCache* cacheManager = MGlxCache::InstanceL();
    CleanupClosePushL(*cacheManager);

    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    path->AppendL(KGlxCollectionPluginImageViewerImplementationUid);

    MGlxMediaList* mediaList = MGlxMediaList::InstanceL(*path);
    CleanupClosePushL(*mediaList);

    if (mediaList->Count() > 0)
        {
        GLX_DEBUG1("CGlxIVwrAppUi::OpenFileL() - Cleanup & Refresh Media!");
        cacheManager->ForceCleanupMedia(mediaList->IdSpaceId(0),
                mediaList->Item(0).Id());
        cacheManager->RefreshL();
        }

    CleanupStack::PopAndDestroy(mediaList);
    CleanupStack::PopAndDestroy(path);
    CleanupStack::PopAndDestroy(cacheManager);
    }

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




#include "glxivwrappui.h"

#include <avkon.hrh>
#include <StringLoader.h>    
#include <aknnotewrappers.h>
#include <AknUtils.h>
#include <mpxviewutility.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionpath.h>

#include <alf/alfenv.h>
#include <akntoolbar.h>
#include <glxgallery.hrh>

#include <glxcollectionpluginimageviewer.hrh>
#include <glxfiltergeneraldefs.h>
#include <glxpanic.h>
#include <glxuistd.h>
#include <glxviewpluginuids.hrh>
#include <glxlog.h>
#include <glxuiutility.h>
#include <glxsettingsmodel.h>
#include <glxcommandhandlers.hrh>
#include <glxzoomstatepublisher.h>
#include <glxnavigationalstate.h>
#include <glxnavigationalstatedefs.h>
#include <glxfullscreenviewplugin.hrh>
#include <glxivwr.rsg>
#include <AknGlobalNote.h>
#include <glxtracer.h>


#include <glxbackservicewrapper.h>

#include <glxuistd.h>
#include <apgcli.h>

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

    // Create Back Stepping Service wrapper
    iBSWrapper = CGlxBackServiceWrapper::NewL( TUid::Uid( KGlxGalleryApplicationUid ) );
    iNavigationalState->SetBackExitStatus(EFalse);

    // Get an instance of view utility
    iViewUtility = MMPXViewUtility::UtilityL();

    iUiUtility = CGlxUiUtility::UtilityL();
    // Always start in default orientation
    iUiUtility->SetAppOrientationL(EGlxOrientationDefault);
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

    delete iActivationParam;

    if ( iUiUtility )
        {
        iUiUtility->Close();
        }

    if( iBSWrapper )
        {	
        delete iBSWrapper;    
        }
    }

// -----------------------------------------------------------------------------
// HandleCommandL
// -----------------------------------------------------------------------------
//
void CGlxIVwrAppUi::HandleCommandL(TInt aCommand)
    {
    TRACER("void CGlxIVwrAppUi::HandleCommandL(TInt aCommand)");
    GLX_LOG_INFO1("PHOTOS LOGS: void CGlxIVwrAppUi::HandleCommandL(TInt aCommand = %d)",aCommand );
    switch(aCommand)
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
            iNavigationalState->ActivatePreviousViewL();
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// ProcessCommandParametersL
// ---------------------------------------------------------------------------
//
TBool CGlxIVwrAppUi::ProcessCommandParametersL(TApaCommand aCommand,
        TFileName& aDocumentName, const TDesC8& aTail)
    {
    TRACER("TBool CGlxIVwrAppUi::ProcessCommandParametersL(TApaCommand /*aCommand*/,TFileName& /*aDocumentName*/, const TDesC8& aTail)");

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
    if(0 == aTail.CompareC(KNullDesC8))
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }

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
    if (naviState->Id() != TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
        {
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
            GLX_LOG_INFO("CGlxIVwrAppUi ::HandleNavigationalStateChanged: Return ");
            return;
            }
        }

    CleanupStack::PopAndDestroy( naviState );

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

        GLX_LOG_INFO1( "CGlxIVwrAppUi::HandleNavigationalStateChanged: Uid count %d", scoringIds.Count());
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
void CGlxIVwrAppUi::GetViewScoringIdsL( RArray<TUid>& aIds ) const
{
TRACER("void CGlxIVwrAppUi::GetViewScoringIdsL( RArray<TUid>& aIds ) const");

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
void CGlxIVwrAppUi::HandleForegroundEventL( TBool aForeground )
    {
    TRACER("void CGlxIVwrAppUi::HandleForegroundEventL( TBool aForeground )");

    // first let base class handle it so that we dont break anything
    CAknViewAppUi::HandleForegroundEventL( aForeground );

    }
// ---------------------------------------------------------------------------
// Return scoring id for collection plugin
// ---------------------------------------------------------------------------
//
TUid CGlxIVwrAppUi::GetViewScoringIdForCollectionPlugin( const CMPXCollectionPath& aNaviState ) const
{
TRACER("TUid CGlxIVwrAppUi::GetViewScoringIdForCollectionPluginL( const CMPXCollectionPath& aNaviState ) const");

GLX_LOG_INFO1( "CGlxIVwrAppUi::GetViewScoringIdForCollectionPluginL: Depth %d", aNaviState.Levels() );
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
TUid CGlxIVwrAppUi::ViewScoringIdForNaviStateDepth( const CMPXCollectionPath& aNaviState ) const
{
TRACER("TUid CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL( const CMPXCollectionPath& aNaviState ) const");
GLX_LOG_INFO1( "CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL: Level %d", aNaviState.Levels() );

switch ( aNaviState.Levels() )
    {
    case 0:

        GLX_LOG_INFO1( "CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL: \
                Depth  case 0 %x", TUid::Uid( KGlxDepthOne ) );
        return TUid::Uid( KGlxDepthOne );

    case 1:

        GLX_LOG_INFO1( "CGlxIVwrAppUi::ViewScoringIdForNaviStateDepthL: \
                Depth case 1 %x", TUid::Uid( KGlxDepthTwo ) );
        return TUid::Uid( KGlxDepthTwo );


    case 2:

        GLX_LOG_INFO1( "CGlxIVwrAppUi::GetViewScoringIdForUiHieararchyDepthL: \
                Depth case 2 %x", TUid::Uid( KGlxDepthThree ) );
        return TUid::Uid( KGlxDepthThree );

    default:
        GLX_LOG_WARNING( "CGlxIVwrAppUi::GetViewScoringIdsL: Navigational state deeper than supported" );
        // return null as an id to be ignored in scoring
        return TUid::Null(); 
    }
}

// ---------------------------------------------------------------------------
// HandleActivationMessageL
// ---------------------------------------------------------------------------
//
void CGlxIVwrAppUi::HandleActivationMessageL(const TApaCommand& aCommand, 
        const TFileName& aDocumentName, const TDesC8& aData)
    {
    TRACER("void CGlxIVwrAppUi::HandleActivationMessageL(const TApaCommand& aCommand, const TFileName& aDocumentName, const TDesC8& aData)");

    delete iActivationParam;
    iActivationParam = NULL;

    if ( 0 == aData.CompareC(KNullDesC8) )
        {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL(path);
        path->AppendL( KGlxCollectionPluginImageViewerImplementationUid );
        iNavigationalState->NavigateToL( *path );
        CleanupStack::PopAndDestroy( path );
        }

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
void CGlxIVwrAppUi::SetActivationParamL(const TDesC8& aParam)
    {
    TRACER("void CGlxIVwrAppUi::SetActivationParamL(const TDesC8& aParam)");
    iActivationParam = HBufC::NewL(aParam.Length());
    iActivationParam->Des().Copy(aParam);
    }

// ---------------------------------------------------------------------------
// HandleApplicationSpecificEventL
// 
// ---------------------------------------------------------------------------
//

void CGlxIVwrAppUi::HandleApplicationSpecificEventL(TInt aEventType, const TWsEvent& aWsEvent)
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
    TRACER("CGlxNsAppUi::CloseImgVwr()");
    iUiUtility->SetViewNavigationDirection(EGlxNavigationBackwards);
    iUiUtility->SetExitingState(ETrue);         
    }


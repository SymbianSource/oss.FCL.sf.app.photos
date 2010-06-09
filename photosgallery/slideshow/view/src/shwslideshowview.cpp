/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Implementation of Slideshow view
*
*/





// INCLUDE FILES
#include <aknnotewrappers.h> 
#include "shwslideshowview.h"

#include <aknViewAppUi.h>
#include <eikmenub.h>   // for access to CEikButtonGroupContainer (from Cba())
#include <AknUtils.h>	// for CompleteWithAppPath
#include <AknWaitDialog.h>
#include <StringLoader.h>

#include <alf/alfcontrolgroup.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfutil.h>
#include <alf/alfroster.h>

#include <glxhdmicontroller.h>
#include <glxresourceutilities.h>               // for CGlxResourceUtilities
#include <glxresolutionutility.h>               // for CGlxResolutionUtility
#include <shwslideshowview.rsg> // view's resource
#include <data_caging_path_literals.hrh>	// for resource directory path
#include <glxlog.h>
#include <glxtracer.h>
#include <aknsoundsystem.h>				// for CAknKeySoundSystem
#include <avkon.rsg>					// for R_AVKON_SILENT_SKEY_LIST
#include <mpxcollectionpath.h>
#include <coeutils.h> 
#include <eiksoftkeypostingtransparency.h>
#include "mpxcollectionutility.h"
#include "glxfilterfactory.h"  // for TGlxFilterFactory
#include "mglxmedialist.h"
#include "glxuiutility.h"
#include "shwslideshowengine.h"
#include "shweffectinfo.h"
#include "shwslideshowvolumecontrol.h"
#include "shwslideshowbacklighttimer.h"
#include "shwslideshowview.hrh"   // for EShwSlideshowCmdContinue
#include "shwslideshowkeyhandler.h"
#include "shwslideshowpausehandler.h"
#include "shwslideshowtelephonehandler.h"
#include "shwcallback.h"
#include "shwsettingsmodel.h"
#include "shwgesturecontrol.h"
#include "shwviewtimer.h"
#include "shwmediakeyshandler.h"
#include <glxtexturemanager.h>
#include <glxerrormanager.h>             
#include <glxthumbnailattributeinfo.h>   
#include <glxicons.mbg>
#include <AknIconUtils.h>
#include <glxuistd.h>

namespace
    {
    _LIT(KShwSlideshowViewResource,"shwslideshowview.rsc");
    const TInt KShwDefaultDelayMicroSeconds = 5000000;	// 5s
    const TInt KVolumeControlGroupId = 1;
    // Reuse the plug-in's uid
    const TInt KViewId = 0x20007199;
    //gontrol goup id has to be locally unique
    const TInt KShwGestureControlGroupId = 2;
    const TInt KGestureControlGroupId = 44;
    //This constant is used to calculate the index of the item for which texture has to removed.
    //6 = 5(iterator value in forward or backward direction for fullscreen) + 1(focus index)
    const TInt KSlideShowIterator = 2; 
    //Constant which says maximum number of fullscreen textures that we have have at a time.
    //11 = (5(5 fullscreen texture backwards)+1(fucus index texture)+5(5 fullscreen texture forwards))
    const TInt KSlideShowTextureOffset = 3;
    }
//display screen furniture for KShowForSeconds    
const TInt KShowForSeconds = 5;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor. Inlined to save a few bits of ROM
// ---------------------------------------------------------------------------
// 
inline CShwSlideshowView::CShwSlideshowView() :
        CGlxViewBase(ETrue) // workaround - force synchronous view activation
    {
    TRACER("CShwSlideshowView::CShwSlideshowView");
    GLX_LOG_INFO( "CShwSlideshowView::CShwSlideshowView" );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CShwSlideshowView* CShwSlideshowView::NewLC()
    {
    TRACER("CShwSlideshowView::NewLC");
    GLX_LOG_INFO( "CShwSlideshowView::NewLC" );
    CShwSlideshowView* self = new ( ELeave ) CShwSlideshowView;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CShwSlideshowView::~CShwSlideshowView()
    {
    TRACER("CShwSlideshowView::~CShwSlideshowView");
  	GLX_LOG_INFO( "CShwSlideshowView::~CShwSlideshowView()" );
  	
  	delete iMMCNotifier;
  	iMMCNotifier = NULL;
  	
  	// delete media list population call back
    delete iPopulateListCallBack;
    
    // delete engine async starter
    delete iAsyncCallBack;

    // Cleanup both the lists in case view was not deactivated
    if ( iMediaList )
        {
        iMediaList->Close();
        }

    if ( iFilteredList )
        {
        iFilteredList->Close();
        }
		
    // Remove the back light timer
    delete iBackLightTimer;

    // Delete the control groups
    if ( iEnv )
        {
        iEnv->DeleteControlGroup( KVolumeControlGroupId );
        iVolumeControlGroup = NULL;
        if(iGestureControlGroup)
            {
            CAlfControl* gesture = (&(iGestureControlGroup->Control(1))); // CGestureControl is 1
            iGestureControlGroup->Remove(gesture);
            }
        iEnv->DeleteControlGroup( KShwGestureControlGroupId );
        iGestureControlGroup = NULL;
    delete iGestureControl;
        }

    // Turn back on the keypad tones
    AppUi()->KeySounds()->PopContext();
    
    // Delete resource file
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        iResourceOffset = NULL;
        }
    }

// -----------------------------------------------------------------------------
// ShowProgressDialogL.
// -----------------------------------------------------------------------------	
TInt CShwSlideshowView::ShowProgressDialogL()
	{
	TRACER("CShwSlideshowView::ShowProgressDialogL");
  	GLX_LOG_INFO( "CShwSlideshowView::ShowProgressDialogL" );

    // Set to landscape orientation
    // Note that the orientation switch is performed here rather than in the
    // view activation to ensure the correct use of the screen furniture layout
    iUiUtility->SetAppOrientationL( EGlxOrientationLandscape );
    // Set display background as a solid black colour
	iDisplay->SetClearBackgroundL( CAlfDisplay::EClearWithColor );
	
	// Setup the progress dialog
    iWaitDialog = new ( ELeave ) CAknWaitDialog(
		reinterpret_cast< CEikDialog** >( &iWaitDialog ), ETrue );
    iWaitDialog->PrepareLC( R_SHW_WAIT_NOTE );
    // Dialog is non-modal so request dismissal callbacks
    iWaitDialog->SetCallback( this );
    // Load string for dialog
    HBufC* title = StringLoader::LoadLC( R_SHW_WAIT_DLG_OPENING );
    iWaitDialog->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

	// Show the dialog, we show it non blocking so no use for the return value
	iWaitDialog->RunLD();
	// set the callback to be engine start
    iAsyncCallBack->Set(TShwCallBack<CShwSlideshowView,
            &CShwSlideshowView::StartEngineL> (this));
    // if both lists are ready
    if( iInputListReady && iPlayListReady )
        {
    	// make the async callback, this will end up calling StartEngineL()
    	// in CAsyncCallback::RunL once the scheduler next time runs.
    	iAsyncCallBack->CallBack();
        }

    // return value needed as this is a TCallBack
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// StartEngineL
// -----------------------------------------------------------------------------
TInt CShwSlideshowView::StartEngineL()
	{
	TRACER("CShwSlideshowView::StartEngineL");
	GLX_LOG_INFO( "CShwSlideshowView::StartEngineL" );
	// Check that the list has some items
	if (iFilteredList)
		{
		TInt filteredCount = iFilteredList->Count();
		TInt inputlistCount = iMediaList->Count();
		if ((inputlistCount < 1) || (filteredCount < 1))
			{
			GLX_LOG_INFO2(
					"CShwSlideshowView::PopulateListL error, counts: %d, %d",
					inputlistCount, filteredCount );
			iEngineStartFailed = ETrue;
			// need to dismiss the dialog if it's still there
			if (iWaitDialog)
				{
				iWaitDialog->ProcessFinishedL();
				}
			}
		else
			{
			// The list should now be populated, so set the focus
			SetListFocusL();
			// Need to take latest screen size as layout has changed
			TRect currentScreen;
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen,
					currentScreen);
			iScrnSize = currentScreen.Size();

			iHdmiWidth
					= (iScrnSize.iWidth > iScrnSize.iHeight ? iScrnSize.iWidth
							: iScrnSize.iHeight);
			iHdmiHeight
					= (iScrnSize.iWidth < iScrnSize.iHeight ? iScrnSize.iWidth
							: iScrnSize.iHeight);
			iEngine->StartL(*iEnv, *iDisplay, *iFilteredList, *this, iScrnSize);
			if (iHdmiController == NULL || (iHdmiController
					&& !iHdmiController->IsHDMIConnected()))
				{
				iEngine->HandleHDMIDecodingEventL(EHdmiDisconnected);
				}
			SetItemToHDMIL();
			iHdmiActive = ETrue;
			}
		}
	// return value needed as this is a TCallBack
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// PopulateListL
// -----------------------------------------------------------------------------
TInt CShwSlideshowView::PopulateListL()
    {
    TRACER("CShwSlideshowView::PopulateListL");
  	GLX_LOG_INFO( "CShwSlideshowView::PopulateListL" );
    // Setup a filter given the play direction and the path
    CMPXFilter* filter = NULL;

    // take current filter and add the slideshow parameters in it
    // If we play forwards, we dont revert the original order and if we
    // play backwards we need to just revert the original order
    // Note we dont know the original order and cant ask for it as its set by the
    // plug-in and never returned to the client side.

    // we need the full path for the filter creation so that the selection can be 
    // extracted from there
    CMPXCollectionPath* fullpath = iMediaList->PathLC();
    // create the filter
    filter = TGlxFilterFactory::CreateSlideShowFilterFromExistingFilterL( 
        iMediaList->Filter(), fullpath,
        iPlayDirection);
    CleanupStack::PushL( filter );

    // Use this DLL's uid as the hierarchy id
    TGlxHierarchyId hierarchyId = TUid( NShwSlideshow::KEngineDllUid ).iUid;
    iFilteredList = MGlxMediaList::InstanceL( *iCollectionPath, hierarchyId, filter );
    iFilteredList->AddMediaListObserverL( this );

    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( fullpath );

    // return value needed as this is a TCallBack
    return KErrNone;
    }
	
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CShwSlideshowView::ConstructL()
    {
    TRACER("CShwSlideshowView::ConstructL");
  	GLX_LOG_INFO( "CShwSlideshowView::ConstructL()" );

	// Load the view's resources
    TFileName resourceFile( KDC_APP_RESOURCE_DIR );
    resourceFile.Append( KShwSlideshowViewResource ); 
    CGlxResourceUtilities::GetResourceFilenameL( resourceFile );  
    iResourceOffset = iCoeEnv->AddResourceFileL( resourceFile );
	
	// AknView base construction
    BaseConstructL( R_SHW_SLIDESHOW_PLAY_VIEW );
    
	// Glx view base construction
    ViewBaseConstructL();
    
    iMMCNotifier = CGlxMMCNotifier::NewL(*this);
    
    // Create async engine starter with standard priority
	iAsyncCallBack = new( ELeave ) CAsyncCallBack( CActive::EPriorityStandard );

    // Create the back light timer
    iBackLightTimer = CShwSlideshowBackLightTimer::NewL();
    
	iEnv = iUiUtility->Env();
   	iDisplay = iUiUtility->Display();
   	iGridIconSize = iUiUtility->GetGridIconSize();
   	
    // Construct the volume control
    iVolumeControl = CShwSlideshowVolumeControl::
        NewL( *iEnv,
        	  *iUiUtility,
        	  KShwDefaultDelayMicroSeconds );
    // Create a control group for the volume control
    iVolumeControlGroup = &iEnv->NewControlGroupL( KVolumeControlGroupId );
    // doc says the following takes ownership, but reality is different
	iVolumeControlGroup->AppendL( iVolumeControl );
	
    iGestureControlGroup = &iEnv->NewControlGroupL( KShwGestureControlGroupId );
	
	// construct the gesture control group
	iShwGestureControl = CShwGestureControl::NewL(*iEnv,*iDisplay);
	iGestureControlGroup->AppendL( iShwGestureControl );
	
	GestureHelper::CGestureControl* gestureControl = GestureHelper::CGestureControl::NewLC( 
            *iShwGestureControl, *iEnv, *iDisplay, KGestureControlGroupId );
    iGestureControl = gestureControl;
    CleanupStack::Pop( gestureControl ); // doc says the following takes ownership, but reality is different
	iGestureControlGroup->AppendL( iGestureControl );

	// Need a mechanism to distinguish when we receive the MusicVolume 
	// callback for the first time
	iCurrentVolume = -1;
    iPopulateListCallBack = new( ELeave )
        CAsyncCallBack( CActive::EPriorityStandard );
    // set the callback for the media list population
    iPopulateListCallBack->Set(TShwCallBack<CShwSlideshowView,
            &CShwSlideshowView::PopulateListL> (this));
    iMSKPressed = EFalse;
    iLSKPressed = EFalse;
    }
   
// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CShwSlideshowView::Id() const
    {
    TRACER("CShwSlideshowView::Id()");
    GLX_LOG_INFO( "CShwSlideshowView::Id()" );
    return TUid::Uid( KViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleForegroundEventL(TBool aForeground)
    {
    TRACER("CShwSlideshowView::HandleForegroundEventL");
  	GLX_LOG_INFO( "CShwSlideshowView::HandleForegroundEventL()" );
	iIsForegrnd = aForeground;
    if( aForeground )
        {
        if (iHdmiController && iHdmiActive)
            {
            iHdmiController->ShiftToPostingMode();
            }
        // we gained the foreground
        if(iMMCState && iPrevNotInBackground)
            {
            ProcessCommandL(EAknSoftkeyClose);
            }
        else
            {
            iPauseHandler->SwitchToForegroundL();
            iEngine->GetMusicVolumeL();
            }
        }
    else
        {
        if (iHdmiController && iHdmiActive)
            {
            iHdmiController->ShiftToCloningMode();
            }
        // Something else has gained the foreground
        iPauseHandler->SwitchToBackgroundL();
        }

    CAknView::HandleForegroundEventL(aForeground);
    }

// -----------------------------------------------------------------------------
// From CGlxViewBase
// DoViewActivateL.
// -----------------------------------------------------------------------------
void CShwSlideshowView::DoViewActivateL(const TVwsViewId& /*aPrevViewId*/,
										TUid /*aCustomMessageId*/,
										const TDesC8& aCustomMessage)
    {
    TRACER("CShwSlideshowView::DoViewActivateL");
    GLX_LOG_INFO( "CShwSlideshowView::DoViewActivateL()" );
    
    // remove the status pane
	AppUi()->StatusPane()->MakeVisible(EFalse);

    //make the softkeys invisible
	AppUi()->Cba()->MakeVisible(EFalse);
	
	// Hide the application toolbar when slide show is started 
	if ( AppUi()->PopupToolbar() ) 
		{
	    AppUi()->PopupToolbar()->SetToolbarVisibility( EFalse );
		AppUi()->PopupToolbar()->MakeVisible(EFalse);
		}	
	iHdmiActive = EFalse;
    // reset failure flag
    iEngineStartFailed = EFalse;

	if(!iHdmiController)
		{
	    iHdmiController = CGlxHdmiController::NewL(*this);
		}
	if(!iTvConnection)
		{
		iTvConnection = CGlxTv::NewL( *this );
		}
    // Engine related construction
    // Instantiate the slideshow engine, with this class as its observer
    __ASSERT_DEBUG( !iEngine, Panic( EGlxPanicAlreadyInitialised ) );
    iEngine = CShwSlideshowEngine::NewL( *this );

    // Instantiate the pause handler, it encapsulates the pause rules
    __ASSERT_DEBUG( !iPauseHandler, Panic( EGlxPanicAlreadyInitialised ) );
    iPauseHandler = CShwSlideShowPauseHandler::NewL( *iEngine );

    // Instantiate the key handler, encapsulates all key handling logic
    __ASSERT_DEBUG( !iKeyHandler, Panic( EGlxPanicAlreadyInitialised ) );
    iKeyHandler = CShwSlideShowKeyHandler::NewL( *iEngine, *iPauseHandler,
                                                  reinterpret_cast<TInt&>(iShwState ));
    AppUi()->AddToStackL( iKeyHandler, ECoeStackPriorityDefault,
        ECoeStackFlagRefusesFocus );

    // Create the telephone call handler
    __ASSERT_DEBUG( !iTelephoneHandler, Panic( EGlxPanicAlreadyInitialised ) );
    iTelephoneHandler = CShwTelephoneHandler::NewL( *iEngine, *iPauseHandler );
    iTelephoneHandler->StartL();

    // Extract the media list's path and the playback direction from
    // the custom message data
    GetPathAndPlaybackDirectionL( aCustomMessage );

    // Ensure the back light stays on
    iBackLightTimer->StartL();

    // Request asynch callback that will end up in ShowProgressDialogL
    // once the active scheduler runs
    iAsyncCallBack->Set(TShwCallBack<CShwSlideshowView,
            &CShwSlideshowView::ShowProgressDialogL> (this));
    iAsyncCallBack->CallBack();

    iDisplay->Roster().ShowL( *iGestureControlGroup,KAlfRosterShowAtTop );
    
    iTicker = CShwTicker::NewL(*this);
    
    InitializeShwFurnitureL();
    iUiUtility->Display()->SetVisibleArea(TRect(TPoint(0,0),AlfUtil::ScreenSize()));
	// We will require to act on events ONLY when the view is active.
	// So listen to them only when the view is active.
    iShwGestureControl->AddObserver(this);    
    }

// -----------------------------------------------------------------------------
// From CGlxViewBase
// DoViewDeactivate.
// -----------------------------------------------------------------------------
void CShwSlideshowView::DoViewDeactivate()
    {
    TRACER("CShwSlideshowView::DoViewDeactivate");
    GLX_LOG_INFO( "CShwSlideshowView::DoViewDeactivate()" );
    if (iHdmiController)
        {
        GLX_LOG_INFO( "CShwSlideshowView::DoViewDeactivate() - delete hdmicontroller" );
        delete iHdmiController;
        iHdmiController = NULL;
        }
    if(iTvConnection)
        {
        delete iTvConnection;
        iTvConnection = NULL;
        }

    //check if we have engine active object starting
    if( iPopulateListCallBack->IsActive() )
        {
        //cancel the engine start
        iPopulateListCallBack->Cancel();
        }

    //check if we have engine active object starting
    if( iAsyncCallBack->IsActive() )
        {
        //cancel the engine start
        iAsyncCallBack->Cancel();
        }
    
    //Hide the volume control group, iDisplay and iVolumeControlGroup
    //are always valid so no need to test for != NULL
   	iDisplay->Roster().Hide( *iVolumeControlGroup );


  	//Ensure we revert to a proper background
    TRAP_IGNORE( 
        {
        iDisplay->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );
        } ); 


    //Ensure we don't get any more key events
    AppUi()->RemoveFromStack( iKeyHandler );	
    delete iKeyHandler;
    iKeyHandler = NULL;
    
    //Stop handling telephone calls
    delete iTelephoneHandler;
    iTelephoneHandler = NULL;
    
    //delete the pause handler 
    //it is used by key and telephone handlers so do this last
    delete iPauseHandler;
    iPauseHandler = NULL;

	//Cancel the back light timer
    iBackLightTimer->Cancel();


    //Stop the slideshow
    //NOTE: this needs to be done before the list is closed as the 
    //destructor removes the engine as medialist observer and also
    //removes the contexts from the list
    delete iEngine;
    iEngine = NULL;

    delete iCollectionPath;
    iCollectionPath = NULL;

    //Ensure we don't get any more list callbacks
    if( iMediaList )
        {
        iMediaList->RemoveMediaListObserver( this );
        //need to close the list
        iMediaList->Close();
        //set to NULL to prevent double delete
        iMediaList = NULL;
        }
    //Ensure we don't get any more list callbacks
    if ( iFilteredList )
        {
        iFilteredList->RemoveMediaListObserver( this );
        //need to also close the list
        iFilteredList->Close();
        //set to NULL to prevent double delete
        iFilteredList = NULL;
        }
    iShwGestureControl->RemoveObserver(this);
   iDisplay->Roster().Hide( *iGestureControlGroup );
    
    if(iTicker)
    	{
    	delete iTicker;
    	iTicker =NULL;
    	}
	AppUi()->RemoveFromStack( iShwCba );
	delete iShwCba;
	iShwCba =NULL;
	if(iMediaKeyHandler)
		{
		delete iMediaKeyHandler;
		iMediaKeyHandler = NULL;
		}
    }

// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleItemAddedL
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleItemAddedL( TInt /*aStartIndex*/,
										  TInt /*aEndIndex*/,
										  MGlxMediaList* /*aList*/ )
    {
    }
	
// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleMediaL
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleMediaL( TInt /*aListIndex*/,
									  MGlxMediaList* /*aList*/)
    {
    }
	
// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleItemRemoved
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleItemRemovedL( TInt /*aStartIndex*/,
										   TInt /*aEndIndex*/,
										   MGlxMediaList* /*aList*/ )
	{
	}
	
// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleItemModifiedL.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleItemModifiedL( 
	const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/ )
	{
	}
	
// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleAttributesAvailableL.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleAttributesAvailableL(
	TInt /*aItemIndex*/,
	const RArray<TMPXAttribute>& /*aAttributes*/,
	MGlxMediaList*/* aList*/ )
    {
    }
	
// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleFocusChangedL.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/,
											TInt /*aNewIndex*/,
											TInt /*aOldIndex*/,
											MGlxMediaList* /*aList*/)
    {
    TRACER("CShwSlideshowView::HandleFocusChangedL()");
	if(iFilteredList)
	    {
	    if (iFilteredList->Count() && iHdmiActive)
            {
            if(iFilteredList->Count() > KSlideShowTextureOffset)
                {
                RemoveTexture();
                }
            SetItemToHDMIL();
            }    
	    }
    }
	
// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleItemSelected.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleItemSelectedL( TInt /*aIndex*/,
											TBool /*aSelected*/,
											MGlxMediaList* /*aList*/)
    {
    }
	
// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandleMessageL.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleMessageL( const CMPXMessage& /*aMessage*/,
										MGlxMediaList* /*aList*/)
	{
	}

// ---------------------------------------------------------------------------
// From MGlxMediaListObserver
// HandlePopulated.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandlePopulatedL( MGlxMediaList* aList )
	{
	TRACER("CShwSlideshowView::HandlePopulatedL");
    GLX_LOG_INFO( "CShwSlideshowView::HandlePopulatedL" );

    if( iMediaList == aList )
        {
        // input list is ready
        iInputListReady = ETrue;
        // read the selection and filter
        // note that these cannot be read until the list has been populated
    	// make the async callback, this will end up calling PopulateListL()
    	// in CAsyncCallback::RunL once the scheduler next time runs.
    	iPopulateListCallBack->CallBack();
        }
    else if( iFilteredList == aList )
        {
        // playable list is ready
        iPlayListReady = ETrue;
        }
    
    // the filtered list has been populated so start the engine
    if( iInputListReady && iPlayListReady )
        {
    	// make the async callback, this will end up calling StartEngineL()
    	// in CAsyncCallback::RunL once the scheduler next time runs.
        // this is a no-op if the callback is already active
    	iAsyncCallBack->CallBack();
        }
	}

// ---------------------------------------------------------------------------
// From MShwEngineObserver
// Engine started callback.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::EngineStartedL()
	{
	TRACER("CShwSlideshowView::EngineStartedL");
	GLX_LOG_INFO( "CShwSlideshowView::EngineStartedL()" );

	if (iWaitDialog)
		{
		// cancel the progress bar
		iWaitDialog->ProcessFinishedL();
		}
	if (iHdmiController)
		{
		iHdmiController->ShiftToPostingMode();
		}
	iShwState = EShwPlay;
	ReplaceCommandSetL(R_SHW_SOFTKEYS_END_PAUSE, R_SHW_SOFTKEYS_END_PAUSE);
	ShowShwFurnitureL();
	}

// ---------------------------------------------------------------------------
// From MShwEngineObserver
// Engine paused callback.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::EnginePausedL()
    {
    TRACER("CShwSlideshowView::EnginePausedL");
  	GLX_LOG_INFO( "CShwSlideshowView::EnginePausedL()" );
    
    // Cancel the backlight if it's on
    if ( iBackLightTimer->IsRunning() )
        {
        iBackLightTimer->Cancel();
        }
        
    if(!iUiUtility->IsExitingState())
    	{
    	iShwState = EShwPause;
   		ReplaceCommandSetL(R_SHW_SOFTKEYS_END_CONTINUE,R_SHW_SOFTKEYS_END_PAUSE);
    	ShowShwFurnitureL();
    	}
    }

// ---------------------------------------------------------------------------
// From MShwEngineObserver
// Engine resumed callback.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::EngineResumedL()   
	{
	TRACER("CShwSlideshowView::EngineResumedL");
  	GLX_LOG_INFO( "CShwSlideshowView::EngineResumedL" );
	iEngine->GetMusicVolumeL();
    // Re-enable the backlight if it's off
    if ( !iBackLightTimer->IsRunning() )
        {
        iBackLightTimer->StartL();
        }
    iShwState = EShwPlay;
    ReplaceCommandSetL(R_SHW_SOFTKEYS_END_PAUSE,R_SHW_SOFTKEYS_END_CONTINUE);
    ShowShwFurnitureL();
	}
// ---------------------------------------------------------------------------
// From MShwEngineObserver
// Engine LSK Pressed
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::EngineLSKPressedL()   
    {
    TRACER("CShwSlideshowView::EngineLSKPressedL");
    GLX_LOG_INFO( "CShwSlideshowView::EngineLSKPressedL" );
    iLSKPressed = ETrue;
    iPauseHandler->UserToggledPauseL();     
    }
// ---------------------------------------------------------------------------
// From MShwEngineObserver
// Engine Toggle Ui callback.EngineLSKPressedL
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::EngineToggleUiStateL()   
    {
    TRACER("CShwSlideshowView::EngineToggleUiStateL");
    GLX_LOG_INFO( "CShwSlideshowView::EngineToggleUiStateL" );
    iMSKPressed = ETrue;
    if(iShwFurniture == EFurnitureVisible)
        {
        HideShwFurniture();
        }
    else
        {
        ShowShwFurnitureL();
        }    
    }

// ---------------------------------------------------------------------------
// From MShwEngineObserver
// Engine encountered an error and we need to exit.
// ---------------------------------------------------------------------------
void CShwSlideshowView::ErrorDuringSlideshowL()
    {
    TRACER("CShwSlideshowView::ErrorDuringSlideshowL");
  	GLX_LOG_INFO( "CShwSlideshowView::ErrorDuringSlideshowL" );
    iEngineStartFailed = ETrue;
    // need to dismiss the dialog if it's still there
    if( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        }
    else
        {
        ProcessCommandL( EAknSoftkeyBack );
        }
    }

// -----------------------------------------------------------------------------
// From MShwMusicObserver
// MusicOn.
// -----------------------------------------------------------------------------
void CShwSlideshowView::MusicOnL()
    {
    TRACER("CShwSlideshowView::MusicOnL");
  	GLX_LOG_INFO( "CShwSlideshowView::MusicOnL" );
    // Turn on the keypad tones
    AppUi()->KeySounds()->PushContextL( R_AVKON_SILENT_SKEY_LIST );
    // if MusicOn leaves, MusicOff will get called
    // initialize the medikey control only if music is on.
    // delayed initialization intended
    // since there is a conflict if slideshow is launched from full screen
   	iMediaKeyHandler = CShwMediaKeyUtility::NewL();
    }
	
// -----------------------------------------------------------------------------
// From MShwMusicObserver
// MusicOff.
// -----------------------------------------------------------------------------
void CShwSlideshowView::MusicOff()
    {
    TRACER("CShwSlideshowView::MusicOff");
  	GLX_LOG_INFO( "CShwSlideshowView::MusicOff" );
  	// 
  	TInt resource_id = AppUi()->KeySounds()->TopContext();
  	if( R_AVKON_SILENT_SKEY_LIST == resource_id )
  	    {
        // Turn off the keypad tones
        AppUi()->KeySounds()->PopContext();
  	    }

    // If it's visible, hide the volume control
    iDisplay->Roster().Hide( *iVolumeControlGroup );
    //delete the media key handler
    if(iMediaKeyHandler)
		{
		delete iMediaKeyHandler;
		iMediaKeyHandler = NULL;
		}
    }
	
// -----------------------------------------------------------------------------
// From MShwMusicObserver
// MusicVolume.
// -----------------------------------------------------------------------------
void CShwSlideshowView::MusicVolumeL(TInt aCurrentVolume, TInt aMaxVolume)
    {
    TRACER("CShwSlideshowView::MusicVolume");
  	GLX_LOG_INFO( "CShwSlideshowView::MusicVolume" );
    if (aCurrentVolume != iCurrentVolume)
        {
        // Store the changed value
        iCurrentVolume = aCurrentVolume;
        iVolumeControl->SetVolume(aCurrentVolume, aMaxVolume);
        }
    // Always show the control even if the volume hasn't changed, e.g. @ 100%
    iVolumeControl->RefreshL();
    }

// -----------------------------------------------------------------------------
// From MShwMusicObserver
// ErrorWithTrackL.
// -----------------------------------------------------------------------------
void CShwSlideshowView::ErrorWithTrackL( TInt /*aErrorCode*/ )
    {
    TRACER("CShwSlideshowView::ErrorWithTrackL");
    // we dont use the error code as any kind of error with track file
    // means we cannot play it.
    // set the music to off if there was an error in track (file missing etc)
    // this way the user gets notification only once
    // ER:  EMSR-77PCSJ
    CShwSettingsModel* shwSettingsMdl = CShwSettingsModel::NewL();
    CleanupStack::PushL( shwSettingsMdl );
    shwSettingsMdl->SaveMusicStateL( EFalse );
    CleanupStack::PopAndDestroy( shwSettingsMdl );

	// Display the error note
	HBufC* noteBuf = StringLoader::LoadLC(
	    R_SHW_QTN_LGAL_NOTE_SLIDESHOW_DEFINE );
    CAknInformationNote* note = new (ELeave) CAknInformationNote;
    CleanupStack::Pop( noteBuf );
    note->ExecuteLD( *noteBuf );
    }

// -----------------------------------------------------------------------------
// From MProgressDialogCallback
// Gets called when a dialog is dismissed.
// -----------------------------------------------------------------------------
void CShwSlideshowView::DialogDismissedL( TInt aButtonId )
    {
    TRACER("CShwSlideshowView::DialogDismissedL");
  	GLX_LOG_INFO( "CShwSlideshowView::DialogDismissedL" );
    if ( iEngineStartFailed || aButtonId == EEikBidCancel )
        {
        // Cancel the callback
        iAsyncCallBack->Cancel();
        // Send the back command to return to the previous view
        ProcessCommandL( EAknSoftkeyBack );    
        }	       
    }

// -----------------------------------------------------------------------------
// SetupScreenFurnitureL.
// -----------------------------------------------------------------------------	
void CShwSlideshowView::SetupScreenFurnitureL()
    {
    TRACER("CShwSlideshowView::SetupScreenFurnitureL");
  	GLX_LOG_INFO( "CShwSlideshowView::SetupScreenFurnitureL" );
    // Create the soft keys
    // Left (bottom in landscape orientation)
    HBufC* softKeyTextLeft = StringLoader::LoadLC( R_SHW_SOFTKEY_LEFT );
    CleanupStack::Pop( softKeyTextLeft );
    // Right (top in landscape orientation)
    HBufC* softKeyTextRight = StringLoader::LoadLC( R_SHW_SOFTKEY_RIGHT );
    CleanupStack::Pop( softKeyTextRight );
    
    }

// -----------------------------------------------------------------------------
// GetPathAndPlaybackDirectionL
// -----------------------------------------------------------------------------
void CShwSlideshowView::GetPathAndPlaybackDirectionL( const TDesC8& aData )
	{
	TRACER("CShwSlideshowView::GetPathAndPlaybackDirectionL");
  	GLX_LOG_INFO( "CShwSlideshowView::GetPathAndPlaybackDirectionL" );
	// reset state flags for list population
	iInputListReady = iPlayListReady = EFalse;

	 // Create a copy of the media list using the path and the play direction
	RDesReadStream stream( aData );
	CleanupClosePushL( stream );
	stream.ReadInt32L();


	//Get the play direction.
	
	CShwSettingsModel* shwSettingsMdl = CShwSettingsModel::NewL();
        CleanupStack::PushL( shwSettingsMdl );
        iPlayDirection = static_cast< NShwSlideshow::
	TPlayDirection>(shwSettingsMdl->PlayOrderL());	
        CleanupStack::PopAndDestroy( shwSettingsMdl );   



	// Retrieve the path
	iCollectionPath = CMPXCollectionPath::NewL();
	iCollectionPath->InternalizeL( stream );
	// Create the media list from the path
	iMediaList = MGlxMediaList::InstanceL( *iCollectionPath  );
	// the list might be already populated
	if( iMediaList->IsPopulated() )
	    {
        iInputListReady = ETrue;
        // list is ready so read the selection and filter already now
        // need to do this while on DoViewActivateL as the previous view
        // may loose the selection while it is closing
        PopulateListL();
	    }
	else
	    {
    	// add us as an observer for the input list as well
    	// we cannot read the path and filter before the list is populated
    	iMediaList->AddMediaListObserverL( this );
	    }
	
	CleanupStack::PopAndDestroy( &stream );
	}

// -----------------------------------------------------------------------------
// SetListFocusL.
// -----------------------------------------------------------------------------
void CShwSlideshowView::SetListFocusL()
    {
    TRACER("CShwSlideshowView::SetListFocusL");
  	GLX_LOG_INFO( "CShwSlideshowView::SetListFocusL" );
    // Ensure that we start the slideshow from the correct image index:
    // if there are any selected images we always start from the first one,
    // otherwise we try to use the item with focus from the unfiltered list
    // so long as it hasn't been filtered out, in which case we use the first image.
    TInt selectionCount = iCollectionPath->Selection().Count();
    TInt focusIndex = 0;
    if ( selectionCount == 0 )
        {
        // nothing selected, so determine which item has focus in the original list
        focusIndex = iMediaList->FocusIndex();
        const TGlxMedia& mediaItem = iMediaList->Item( focusIndex );
        // Check if this item is in the filtered list
        TGlxIdSpaceId spaceId = iMediaList->IdSpaceId( focusIndex );
        focusIndex = iFilteredList->Index( spaceId, mediaItem.Id() );
        if ( focusIndex == KErrNotFound )
            {
            // it's been filtered out so just use the first item
            focusIndex = 0;
            }
        }
    iFilteredList->SetFocusL( NGlxListDefs::EAbsolute, focusIndex );
    }


// -----------------------------------------------------------------------------
// HandleTickL.
// act on timer expiry
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::HandleTickL()
	{
	TRACER("CShwSlideshowView::HandleTickL");
	GLX_LOG_INFO( "CShwSlideshowView::HandleTickL" );
	if(iShwFurniture != EFurnitureVisible)
		{
		ShowShwFurnitureL();
		}
	else
		{
		HideShwFurniture();
		}
	}

// -----------------------------------------------------------------------------
// HandleTickCancelled.
// act on timer cancelled
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::HandleTickCancelled()
	{
    TRACER("CShwSlideshowView::HandleTickCancelled()");
	}
	
// -----------------------------------------------------------------------------
// HandleShwGestureEventL.
// act on gestures
// -----------------------------------------------------------------------------
//	
void CShwSlideshowView::HandleShwGestureEventL(MShwGestureObserver::TShwGestureEventType aType)
	{
	TRACER("CShwSlideshowView::HandleShwGestureEventL");
	GLX_LOG_INFO( "CShwSlideshowView::HandleShwGestureEventL" );
	
	// If there is no Engine, the vehicle is handicapped and there is no way to know 
	// whether it is possible for it to PROPERLY respond to events.
	// So it is safer to not do so. 
	if (NULL == iEngine)
	    {
        return ;
	    }
	
	iTicker->CancelTicking();
	switch(aType)
		{
		case ETapEvent:
			{
			if(iShwFurniture == EFurnitureVisible)
				{
				HideShwFurniture();
				}
			else
				{
				ShowShwFurnitureL();
				}
			break;
			}
		case ESwipeLeft:
			{
			iEngine->NextItemL();
			break;
			}
		case ESwipeRight:
			{
			iEngine->PreviousItemL();
			break;
			}
		default:
			{
			break;
			}
		}
	}
// -----------------------------------------------------------------------------
// InitializeShwFurnitureL.
// set sof keys and the volume slider
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::InitializeShwFurnitureL()
	{
	TRACER("CShwSlideshowView::InitializeShwFurnitureL");
	GLX_LOG_INFO( "CShwSlideshowView::InitializeShwFurnitureL" );
	// initialize softkeys
	InitializeCbaL();
	}
	
// -----------------------------------------------------------------------------
// InitializeCbaL.
// set sof keys
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::InitializeCbaL()
	{
	TRACER("CShwSlideshowView::InitializeCbaL");
	GLX_LOG_INFO( "CShwSlideshowView::InitializeCbaL" );
	// initialize with the end-pause command set as that is the one 
	// we are supposed to show immediately on slideshow play
	// orientation: set to Vertical as slide show is always intended to
	// run in landscape mode
    iShwCba = CEikButtonGroupContainer::NewL(
        CEikButtonGroupContainer::ECba,
        CEikButtonGroupContainer::EVertical,
        this, R_SHW_SOFTKEYS_END_PAUSE );
    iShwCba->MakeVisible(EFalse); 
    //set the current active command set
	ReplaceCommandSetL(R_SHW_SOFTKEYS_END_PAUSE,R_SHW_SOFTKEYS_END_PAUSE);
	}
	
// -----------------------------------------------------------------------------
// ReplaceCommandSetL.
// replaces a  commandset with a new one
// -----------------------------------------------------------------------------
//	
void CShwSlideshowView::ReplaceCommandSetL(TInt aNewComandId, TInt aOldCommandSet)
	{
	TRACER("CShwSlideshowView::ReplaceCommandSetL");
	GLX_LOG_INFO( "CShwSlideshowView::ReplaceCommandSetL" );
	//find and remove the command set
    TInt pos = iShwCba->PositionById( aOldCommandSet );
    if( pos != KErrNotFound )
        {
        iShwCba->RemoveCommandFromStack( pos, aOldCommandSet );
        }
    // set the new command set
    iShwCba->SetCommandSetL( aNewComandId );
    // keep the current active command set
    iCurrentActiveCommandSet = aNewComandId;
    }

	
// -----------------------------------------------------------------------------
// HideShwFurnitureL.
// hide softkeys and the volume slider
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::HideShwFurniture()
	{
	TRACER("CShwSlideshowView::HideShwFurnitureL");
	GLX_LOG_INFO( "CShwSlideshowView::HideShwFurnitureL" );
	iTicker->CancelTicking();
	iShwCba->MakeVisible(EFalse);
	iShwCba->DrawNow();
	if(iMediaKeyHandler)
	    {
	    iVolumeControl->Hide();
	    }	
	iShwFurniture = EFurnitureHidden;
	
	}
	
// -----------------------------------------------------------------------------
// ShowShwFurnitureL.
// show softkeys and the volume slider
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::ShowShwFurnitureL()
	{
	TRACER("CShwSlideshowView::ShowShwFurnitureL");
	GLX_LOG_INFO( "CShwSlideshowView::ShowShwFurnitureL" );
	iTicker->CancelTicking();
	iShwCba->MakeVisible(ETrue);
	iShwCba->DrawNow();
	if(iMediaKeyHandler)
	    {
	    iVolumeControl->ShowControlL();
	    }
	iShwFurniture = EFurnitureVisible;
	//start the timer to keep the up the screen furniture for 5 seconds
	iTicker->StartTicking(KShowForSeconds);
	}
	
// -----------------------------------------------------------------------------
// ProcessCommandL.
// handle softkey events
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::ProcessCommandL(TInt aCommandId)
    {
    TRACER("CShwSlideshowView::ProcessCommandL");
    GLX_LOG_INFO( "CShwSlideshowView::ProcessCommandL" );
    switch(aCommandId)
        {
        case EShwSlideshowCmdEnd:
            {
            iShwState = EShwExiting;
            aCommandId = EAknSoftkeyBack;
            iDisplay->Roster().Hide( *iGestureControlGroup );        
            break;
            }
            //When user presses MSK or LSK this cmd will Generated
        case EShwSlideshowCmdPause:
        case EShwSlideshowCmdContinue:
            {
            // If MSK preesed to toggle visibility of softekey
            if(iMSKPressed)
                {
                iMSKPressed = EFalse;
                }            
            else if(iLSKPressed)
                {
                iLSKPressed = EFalse;// Already Handlled
                }
            else
                {
                iPauseHandler->UserToggledPauseL();
                }
            break;  
            }

        default:
            {
            break;
            }
        }
    CGlxViewBase::ProcessCommandL(aCommandId);
    }
// ----------------------------------------------------------------------------- 
// Set the image to external display - HDMI
// -----------------------------------------------------------------------------
//
void CShwSlideshowView::SetItemToHDMIL()
    {
    TRACER("CShwSlideshowView::SetImageToHDMIL() ");

    TInt focusIndex = iFilteredList->FocusIndex();
    TInt nextIndex = GetNextIndex();

    // If we dont know what item we are focussing on 
    // or if our filtered list is empty
    // or if HDMI is not connected 
    // or if there is no HDMI Controller at all 
    // then dont SetImageToHDMI :)  
    if( ( KErrNotFound == focusIndex)
            || (0 == iFilteredList->Count())
            || (NULL == iHdmiController)) 
        {
        GLX_LOG_INFO("CShwSlideshowView::SetImageToHDMIL - Cant Set Image To HDMI");
        return;
        }
    
    TGlxMedia item = iFilteredList->Item(focusIndex);
    TInt error = GlxErrorManager::HasAttributeErrorL(item.Properties(),
        KGlxMediaIdThumbnail);
    TGlxMedia nextItem = iFilteredList->Item(nextIndex);
    TInt nexterror = GlxErrorManager::HasAttributeErrorL(nextItem.Properties(),
        KGlxMediaIdThumbnail);
    
    // Item will be supported by HDMI ONLY if
    // it is not a video
    // and it has no attribute error 
    if ( (item.Category() != EMPXVideo) 
            && (error == KErrNone) )
        {
        GLX_LOG_INFO("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - Fetch FS thumbnail");
        TMPXAttribute fsTnAttrib = TMPXAttribute(KGlxMediaIdThumbnail,
                GlxFullThumbnailAttributeId(ETrue, iScrnSize.iWidth,
                        iScrnSize.iHeight));
        const CGlxThumbnailAttribute* fsValue = item.ThumbnailAttribute(
                fsTnAttrib);
        if (fsValue)
            {
            GLX_LOG_INFO("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - Setting FS Bitmap");
            CFbsBitmap* fsBitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(fsBitmap);
            fsBitmap->Duplicate( fsValue->iBitmap->Handle());
            
            GLX_LOG_INFO2("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - FS Bitmap Size width=%d, height=%d", 
                    fsBitmap->SizeInPixels().iWidth, fsBitmap->SizeInPixels().iHeight);
            if(nexterror == KErrNone)
            	{
                iHdmiController->SetImageL(item.Uri(), nextItem.Uri(), fsBitmap);            	
            	}
            else
            	{
            	iHdmiController->SetImageL(item.Uri(), KNullDesC, fsBitmap);
            	}
            CleanupStack::PopAndDestroy(fsBitmap);
            }
        else
            {
            GLX_LOG_INFO("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - Fetch Grid thumbnail");
            TMPXAttribute gridTnAttrib = TMPXAttribute(KGlxMediaIdThumbnail,
                    GlxFullThumbnailAttributeId(ETrue, iGridIconSize.iWidth,
                            iGridIconSize.iHeight));
            const CGlxThumbnailAttribute* gridvalue = item.ThumbnailAttribute(
                    gridTnAttrib);

            if (gridvalue)
                {
                GLX_LOG_INFO("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - Setting Grid Bitmap");
                CFbsBitmap* gridBitmap = new (ELeave) CFbsBitmap;
                CleanupStack::PushL(gridBitmap);
                gridBitmap->Duplicate( gridvalue->iBitmap->Handle());
                
                GLX_LOG_INFO2("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - gridBitmap Size width=%d, height=%d", 
                        gridBitmap->SizeInPixels().iWidth, gridBitmap->SizeInPixels().iHeight);
                if(nexterror == KErrNone)
                	{
                	iHdmiController->SetImageL(item.Uri(), nextItem.Uri(), gridBitmap);
                	}
                else
                	{
                	iHdmiController->SetImageL(item.Uri(), KNullDesC, gridBitmap);                	
                	}
                CleanupStack::PopAndDestroy(gridBitmap);
                }
            else
                {
                GLX_LOG_INFO("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - Setting Null Bitmap");
                TFileName resFile(KDC_APP_BITMAP_DIR);
                resFile.Append(KGlxIconsFilename);
                CFbsBitmap* defaultBitmap = AknIconUtils::CreateIconL(resFile,
                        EMbmGlxiconsQgn_prop_image_notcreated);
                CleanupStack::PushL(defaultBitmap);
                
                // always need to setsize on the raw bitmap for it to be visible
                AknIconUtils::SetSize(defaultBitmap, TSize(iHdmiWidth,iHdmiHeight),EAspectRatioPreserved);
                
                GLX_LOG_INFO2("CShwSlideshowView::SetImageToHDMIL - CGlxHdmi - Default Size width=%d, height=%d", 
                        defaultBitmap->SizeInPixels().iWidth, defaultBitmap->SizeInPixels().iHeight);
                if(nexterror == KErrNone)
                	{
                    iHdmiController->SetImageL(item.Uri(), nextItem.Uri(),
							defaultBitmap);                	
                	}
                else
                	{
                    iHdmiController->SetImageL(item.Uri(), KNullDesC , defaultBitmap);                	
                	}
                CleanupStack::PopAndDestroy(defaultBitmap); 
                }
            }
        }
    else
        {
        GLX_LOG_INFO("CShwSlideshowView::SetImageToHDMIL - Unsupported Item");
        //Set the external display to cloning mode if
        //the current item is something we wont support (e.g. video, corrupted item)
        iHdmiController->ShiftToCloningMode();
        }
    }
// ---------------------------------------------------------------------------
// 
// Gets the index of the item for which the texture has to be removed
// ---------------------------------------------------------------------------
//
TInt CShwSlideshowView::GetIndexToBeRemoved()
    {
    TRACER("CShwSlideshowView::GetIndexToBeRemoved");
    TInt indextoberemoved;
    TInt count = iFilteredList->Count();
    TInt focusIndex = iFilteredList->FocusIndex();
    if(iPlayDirection == NShwSlideshow::EPlayForwards)
        {
        indextoberemoved = focusIndex-KSlideShowIterator;
        if(indextoberemoved < 0)
            {
            indextoberemoved = count+indextoberemoved;              
            }
        }
       else
        {
        indextoberemoved = focusIndex+KSlideShowIterator;
        if(indextoberemoved > count-1)
          {
          indextoberemoved = indextoberemoved-count;
          }        
        }
    GLX_LOG_INFO1("CShwSlideshowView::GetIndexToBeRemoved Index to be removed is %d",indextoberemoved);
    return indextoberemoved;
    }
// ---------------------------------------------------------------------------
// 
// Remove the texture on every swipe while HDMI is connected.
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::RemoveTexture()
    {
    TRACER("CShwSlideshowView::RemoveTexture");
    const TGlxMedia& item = iFilteredList->Item(GetIndexToBeRemoved());
    iUiUtility->GlxTextureManager().RemoveTexture(item.Id(),EFalse);
    }

// ---------------------------------------------------------------------------
// 
// HandleMMCInsertionL
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleMMCInsertionL()
    {
    TRACER("CShwSlideshowView::HandleMMCInsertionL()");
    iMMCState = ETrue;
    iPrevNotInBackground = iIsForegrnd;
    if(iIsForegrnd)
        {
        ProcessCommandL(EAknSoftkeyClose);
        }
    }

// ---------------------------------------------------------------------------
// 
// HandleMMCRemovalL
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleMMCRemovalL()
    {
    TRACER("CShwSlideshowView::HandleMMCRemovalL()");
    ProcessCommandL(EAknSoftkeyExit);
    }

// ---------------------------------------------------------------------------
//
// HandleTvStatusChangedL
// ---------------------------------------------------------------------------
//
void CShwSlideshowView::HandleTvStatusChangedL(TTvChangeType aChangeType )
	{
	TRACER("CShwSlideshowView::HandleTvStatusChangedL");
	// This is a common function for both HDMI ,TV-out and Headset disconnected.  
	// (see CGlxTv::HandleTvConnectionStatusChangedL)
	if (aChangeType == EDisconnected)
		{
		GLX_LOG_INFO("CShwSlideshowView::HandleTvStatusChangedL- Acessory Not Connected");
		//Issue pause command if not already paused
		if (!iPauseHandler->IsSlideShowEngineStatePaused())
			{
			ProcessCommandL(EShwSlideshowCmdPause);
			}
		}

	}

// ---------------------------------------------------------------------------
// GetNextIndex
// ---------------------------------------------------------------------------
//
TInt CShwSlideshowView::GetNextIndex()
	{
	TRACER("CShwSlideshowView::GetNextIndex");
	TInt count = iFilteredList->Count();
	TInt focusIndex = iFilteredList->FocusIndex();
	TInt nextIndex = focusIndex + 1;
	if (nextIndex > count - 1)
		{
		nextIndex = nextIndex - count;
		}
	GLX_LOG_INFO1("CShwSlideshowView::GetNextIndex-nextIndex:%d",nextIndex);
	return nextIndex;
	}

//---------------------------------------------------------------------------
// From MGlxHDMIDecoderObserver
// Handle notification of HDMI Image Decoder.
//---------------------------------------------------------------------------   
void CShwSlideshowView::HandleHDMIDecodingEventL(THdmiDecodingStatus aStatus)
	{
	TRACER("CShwSlideshowView::HandleHDMIDecodingEventL()");
	iEngine->HandleHDMIDecodingEventL(aStatus);
	}

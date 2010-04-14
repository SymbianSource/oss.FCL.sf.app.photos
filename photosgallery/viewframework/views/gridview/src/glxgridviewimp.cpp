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
* Description:    Implementation of Grid view
*
*/





// INCLUDE FILES
#include <aknbutton.h>
#include <StringLoader.h>
#include <AknsBasicBackgroundControlContext.h>

//Gallery Headers
#include <glxactivemedialistregistry.h>                 // For medialist registry
#include <glxcommandhandlers.hrh>                       // For EGlxCmdFullScreenBack                   
#include <glxgridviewdata.rsg>                          // Gridview resource

#include <mglxmedialist.h>                              // CGlxMedialist
#include <glxsetappstate.h>
#include <glxtracer.h>                                  // For Tracer


// User Includes
#include "glxgridviewimp.h"                         
#include "glxgridviewmlobserver.h"                      // medialist observer for Hg Grid                
#include "glxgridviewcontainer.h"

// For transition effects
#include <akntranseffect.h>                             
#include <gfxtranseffect/gfxtranseffect.h>
#include "glxgfxtranseffect.h"  // For transition effects

const TInt KGlxToolbarButtonUnLatched = 0;              // Toolbar mark button's unlatched state defined in the rss file

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxGridViewImp* CGlxGridViewImp::NewL(
		MGlxMediaListFactory* aMediaListFactory,
		const TGridViewResourceIds& aResourceIds,
		TInt aViewUID,
		const TDesC& aTitle)
	{
	TRACER("CGlxGridViewImp::NewL");
	CGlxGridViewImp* self = CGlxGridViewImp::NewLC(aMediaListFactory, 
			aResourceIds,
			aViewUID,
			aTitle);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxGridViewImp* CGlxGridViewImp::NewLC(
		MGlxMediaListFactory* aMediaListFactory,
		const TGridViewResourceIds& aResourceIds,
		TInt aViewUID,
		const TDesC& aTitle)
	{
	TRACER("CGlxGridViewImp::NewLC()");
	CGlxGridViewImp* self = 
	new (ELeave) CGlxGridViewImp(aResourceIds, aViewUID);
	CleanupStack::PushL(self);
	self->ConstructL(aMediaListFactory, aTitle);
	return self;
	}

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxGridViewImp::CGlxGridViewImp(const TGridViewResourceIds& aResourceIds,
		TInt aViewUID) :
		iResourceIds(aResourceIds),
		iViewUID(aViewUID),
		iPreviousFocusChangeType(NGlxListDefs::EUnknown)
    	{
    	TRACER("CGlxGridViewImp::CGlxGridViewImp()");
    	}

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//  
void CGlxGridViewImp::ConstructL(MGlxMediaListFactory* aMediaListFactory,
		const TDesC& aTitle)
	{
	TRACER("CGlxGridViewImp::ConstructL()");
	BaseConstructL(iResourceIds.iViewId);
	ViewBaseConstructL();
	MLViewBaseConstructL(aMediaListFactory, aTitle);   

	//create the tool bar dynamically
	//to reduce the startup time of the application.
	iToolbar = CAknToolbar::NewL(R_GLX_GRID_VIEW_TOOLBAR);
	SetGridToolBar(iToolbar);
	SetToolbarObserver(this);
	iToolbar->SetToolbarVisibility(ETrue);
    
	// Get object that stores the active media list registry
	iActiveMediaListRegistry = CGlxActiveMediaListRegistry::InstanceL();
	}

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CGlxGridViewImp::DoMLViewActivateL(
		const TVwsViewId& /* aPrevViewId */, 
		TUid /* aCustomMessageId */,
		const TDesC8& /*aCustomMessage*/)
	{
	TRACER("CGlxGridViewImp::DoMLViewActivateL()");

    TUint transitionID = (iUiUtility->ViewNavigationDirection()==
          EGlxNavigationForwards)?KActivateTransitionId:KFSDeActivateTransitionId; 
    
	GfxTransEffect::BeginFullScreen( transitionID, TRect(),
                                AknTransEffect::EParameterType, 
                                AknTransEffect::GfxTransParam( KPhotosUid,
                                AknTransEffect::TParameter::EEnableEffects) );	
	GfxTransEffect::EndFullScreen();
	
	if(StatusPane())
		{
		StatusPane()->MakeVisible(ETrue);
		CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
		CAknTitlePane* titlePane = ( CAknTitlePane* )statusPane->ControlL(
				TUid::Uid( EEikStatusPaneUidTitle ));     
		const TDesC* titleText = titlePane->Text();
		HBufC* tempTitle = titleText->AllocLC();
		TPtr titleptr = tempTitle->Des();
		titleptr.Trim();
		if(!tempTitle->Length())
			{
			if( iTitletext )
				{
				// Set the required Title
				titlePane->SetTextL( *iTitletext );
				}
			}
		CleanupStack::PopAndDestroy(tempTitle);        
		}

	GlxSetAppState::SetState(EGlxInCarouselView);
	// Setting the Context sensitive menu id
	MenuBar()->SetContextMenuTitleResourceId( iResourceIds.iOkOptionsMenuId );
	iActiveMediaListRegistry->RegisterActiveMediaList(iMediaList);
	if(!iToolbar)
        {
        iToolbar = CAknToolbar::NewL(R_GLX_GRID_VIEW_TOOLBAR);
        SetGridToolBar(iToolbar);
        SetToolbarObserver(this);
        iToolbar->SetToolbarVisibility(ETrue);
        }
	//Create HG Grid, medialist observer, FS thumbnailcontext
	iGlxGridViewContainer = CGlxGridViewContainer::NewL(iMediaList,iUiUtility,*this,iToolbar);
	iEikonEnv->AppUi()->AddToStackL(*this,iGlxGridViewContainer);
	}

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CGlxGridViewImp::DoMLViewDeactivate()
	{
	TRACER("CGlxGridViewImp::DoMLViewDeactivate");
	if(StatusPane())
		{
		if(iTitletext)
			{
			delete iTitletext;
			iTitletext = NULL;
			}
		CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
		TRAP_IGNORE(CAknTitlePane* titlePane = ( CAknTitlePane* )statusPane->ControlL(
				TUid::Uid( EEikStatusPaneUidTitle ));        
		iTitletext = titlePane->Text()->AllocL());
		}
	// Deregister active media list pointer
	iActiveMediaListRegistry->DeregisterActiveMediaList(iMediaList);
	if(iToolbar)
        {
         delete iToolbar;
         iToolbar = NULL;
		 //set the gridtoolbar to NULL in viewbase.
         SetGridToolBar(iToolbar);
        }
	// Destroy Grid widget before going to next view
	DestroyGridWidget();
	}

// ---------------------------------------------------------------------------
// DestroyGridWidget
// ---------------------------------------------------------------------------
//  
void CGlxGridViewImp::DestroyGridWidget()
	{
	TRACER("CGlxGridViewImp::DestroyGridWidget()");
	//while view deactivating need to remove from the control stack.
	iEikonEnv->AppUi()->RemoveFromViewStack(*this,iGlxGridViewContainer);        
	delete iGlxGridViewContainer;
    iGlxGridViewContainer = NULL;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxGridViewImp::~CGlxGridViewImp()
	{
	TRACER("CGlxGridViewImp::~CGlxGridViewImp");
	delete iTitletext;
	if(iToolbar)
	    {
        delete iToolbar;
        iToolbar = NULL;
	    }
	if (iActiveMediaListRegistry)
		{
		iActiveMediaListRegistry->Close();
		}    
	}

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
TUid CGlxGridViewImp::Id() const
{
TRACER("CGlxGridViewImp::Id()");
return TUid::Uid(iViewUID);
}

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
TBool CGlxGridViewImp::HandleViewCommandL(TInt aCommand)
	{
	TRACER("CGlxGridViewImp::HandleViewCommandL()");
    GLX_LOG_INFO1( "CGlxGridViewImp::HandleViewCommandL(%x) entering", aCommand );
    return iGlxGridViewContainer->HandleViewCommandL(aCommand);
	}

// ---------------------------------------------------------------------------
// From CAknView
// HandleForegroundEventL
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CGlxGridViewImp::HandleForegroundEventL(TBool aForeground)
	{
    TRACER("CGlxGridViewImp::HandleForegroundEventL()");
	if(iMMCState)
	    {
        iMMCState = EFalse;
        ProcessCommandL(EAknSoftkeyClose);
	    }
	CAknView::HandleForegroundEventL(aForeground);
	}

// ---------------------------------------------------------------------------
// From MGlxHandleGridEvents
// HandleGridEventsL()
// handles processcommandL related commands
// -----------------------------------------------------------------------------
//
void CGlxGridViewImp::HandleGridEventsL(TInt aCmd)
	{
	TRACER("CGlxGridViewImp::HandleGridEventsL()");
	//do processcmd related event handling
	if(EAknSoftkeyClose == aCmd)
	    {
        iMMCState = ETrue;
	    }
	ProcessCommandL(aCmd);
	}

// ---------------------------------------------------------------------------
// From MGlxHandleButtonMark
// HandleLatchToolbar()
// sets toolbar mark unmark status
// -----------------------------------------------------------------------------
//
void CGlxGridViewImp::HandleLatchToolbar()
	{
	TRACER("CGlxGridViewImp::HandleLatchToolbarL()");
	CAknButton* markButton = static_cast<CAknButton*>
									(iToolbar->ControlOrNull( EGlxCmdStartMultipleMarking ));

	if(markButton)
		{
		markButton->SetCurrentState( KGlxToolbarButtonUnLatched, ETrue );
		}	
	}
//  End of File

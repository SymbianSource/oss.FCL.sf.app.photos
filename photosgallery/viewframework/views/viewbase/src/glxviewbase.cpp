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
* Description:    View Base implementation
*
*/




#include "glxviewbase.h"

#include <aknViewAppUi.h>
#include <avkon.rsg>
#include <data_caging_path_literals.hrh>

#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <alf/alfroster.h>
#include <StringLoader.h> 
#include <akntoolbar.h>
#include <glxcommandhandlers.hrh>
#include <glxuiutility.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <glxpanic.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxuistd.h>
#include <glxuiutilities.rsg>
#include <mglxanimation.h>
#include "glxcommandhandler.h"

#include <aknbutton.h>                      // for getting the button state

// For transition effects                           
#include <gfxtranseffect/gfxtranseffect.h>

_LIT(KGlxViewBaseResource, "glxviewbase.rsc");

/// Length of time a view-switch animation should take
const TInt KGlxViewSwitchAnimationDuration = 1000 * KGlxAnimationSlowDownFactor;

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxViewBase::CGlxViewBase(TBool aSyncActivation) :
    iViewAnimationTime(KGlxViewSwitchAnimationDuration),
    iIsTransEffectStarted(EFalse),
    iViewAnimationInProgress(EGlxViewAnimationNone),
    iSyncActivation(aSyncActivation)
    {
    TRACER( "CGlxViewBase::CGlxViewBase()" );
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxViewBase::~CGlxViewBase()
    {
    TRACER( "CGlxViewBase::~CGlxViewBase" );
    iCommandHandlerList.ResetAndDestroy();

    if ( iUiUtility )
        {
        iUiUtility->Close();
        iUiUtility = NULL;
        }

    if ( iViewBaseResourceOffset )
        {
        iCoeEnv->DeleteResourceFile( iViewBaseResourceOffset );
        }

    if ( iUiUtilitiesResourceOffset )
        {
        iCoeEnv->DeleteResourceFile( iUiUtilitiesResourceOffset );
        }

    if ( iViewActivateCallback )
        {
        iViewActivateCallback->Cancel();
        delete iViewActivateCallback;
        }

    delete iCustomMessage;
    }

// -----------------------------------------------------------------------------
// ViewBaseConstructL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::ViewBaseConstructL()
	{    
	TRACER( "CGlxViewBase::ViewBaseConstructL" );
	// Load resources
	TFileName resourceFile(KDC_APP_RESOURCE_DIR);
    resourceFile.Append(KGlxViewBaseResource); 
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile,
                                                iCoeEnv->FsSession());  
    iViewBaseResourceOffset = iCoeEnv->AddResourceFileL(resourceFile);

    // Load UiUtilities resources, for wait dialog
    TFileName uiUtilsResourceFile( KDC_APP_RESOURCE_DIR );
    uiUtilsResourceFile.Append( KGlxUiUtilitiesResource );
    CGlxResourceUtilities::GetResourceFilenameL(uiUtilsResourceFile,
                                                iCoeEnv->FsSession());
   	iUiUtilitiesResourceOffset = iCoeEnv->AddResourceFileL(uiUtilsResourceFile);

    iUiUtility = CGlxUiUtility::UtilityL();
    
    iViewActivateCallback = new (ELeave) CAsyncCallBack ( TCallBack ( ViewActivateCallbackL, this ),
                                                          CActive::EPriorityStandard);
    iCustomMessage = HBufC8::NewL(0);
	}

// -----------------------------------------------------------------------------
// AddCommandHandlerL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::AddCommandHandlerL(CGlxCommandHandler* aCommandHandler)
	{
	TRACER( "CGlxViewBase::AddCommandHandlerL" );
    if ( aCommandHandler )
        {
        CleanupDeletePushL( aCommandHandler );
        if ( KErrNotFound == iCommandHandlerList.Find( aCommandHandler ))
            {
            iCommandHandlerList.AppendL(aCommandHandler);
            DoPrepareCommandHandlerL(aCommandHandler);
            }
        CleanupStack::Pop( aCommandHandler );
        }
    else
    	{
    	User::Leave(KErrArgument);
    	}
	}

// -----------------------------------------------------------------------------
// HandleCommandL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::HandleCommandL(TInt aCommand)
	{
	TRACER( "CGlxViewBase::HandleCommandL" );
    if(iViewActivated)  // Act on this Command if view is Activated or Pass to AppUi
        {
    if ( EAknSoftkeyBack == aCommand ) 
        {
		// Pass it to view
        DoHandleCommandL(aCommand);
        iUiUtility->SetViewNavigationDirection(EGlxNavigationBackwards);
        }
    else if ( EAknCmdOpen == aCommand ) 
        {
        iUiUtility->SetViewNavigationDirection(EGlxNavigationForwards); 
        }

    if ( EAknSoftkeyOptions == aCommand )
        {
        FetchAttributesL();
        // Open the options menu properly
        ProcessCommandL(EAknSoftkeyOptions);
        }
    else
        {
    	// Fetch the attributes required to execute the command provided it is not one of
    	// the state changing commands (EGlxCmdStateView and EGlxCmdStateBrowse)
        if (aCommand != EGlxCmdStateView && aCommand != EGlxCmdStateBrowse)
        	{
        	FetchAttributesForCommandL(aCommand);
        	}
        
    	TBool handled = EFalse;

        // Pass the command to the command handlers
        TInt count = iCommandHandlerList.Count();
        TInt i = count - 1;

        // Iterating backwards to give last-added handlers chance to override
        while ( i >= 0 && !handled )
            {
            handled = iCommandHandlerList[i]->ExecuteL( aCommand );
            i--;
            }

        if ( !handled )
            {
            // Give the implemented view chance at handling the command	
            handled = DoHandleCommandL(aCommand);
            }

    	if ( !handled )
    		{
    		// None of the command handlers have handled it, pass it on to the AppUi
            AppUi()->HandleCommandL(aCommand);
            }
        }            
	}
    else
        {
        // Command Not Handled
        AppUi()->HandleCommandL(aCommand);
        }
	}

// -----------------------------------------------------------------------------
// DynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
	TRACER( "CGlxViewBase::DynInitMenuPaneL" );
    TInt count = iCommandHandlerList.Count();

    // NB Member variable used for loop counter, so if the user interrupts (e.g. Cancels the wait
    // dialog), we know which command handler we need to stop (see DialogDismissedL)
    for (TInt i = 0; i < count; i++ )
        {
        iCommandHandlerList[i]->PreDynInitMenuPaneL( aResourceId );
        }
    
	// Allow implementing view to filter menu first
	ViewDynInitMenuPaneL(aResourceId, aMenuPane);

	//Since the toolbar is set only incase of grid view we can assume that
	//it is in browse mode.	   
	TBool isBrowseMode = EFalse;
    if(iUiUtility->GetGridToolBar())
        {
        isBrowseMode = ETrue;
        } 
    for (TInt i = 0; i < count; i++ )
        {
        iCommandHandlerList[i]->DynInitMenuPaneL( aResourceId, aMenuPane, isBrowseMode );
        }
	}


// -----------------------------------------------------------------------------
// SetTitleL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::SetTitleL(const TDesC& aTitleText)
	{
	TRACER( "CGlxViewBase::SetTitleL" );
	iTitlePane = GetTitlePaneL();
	if(iTitlePane)
	    {
	    iTitlePane->SetTextL(aTitleText);
	    }
	}

// -----------------------------------------------------------------------------
// DisableTitleL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::DisableTitle()
	{
	TRACER( "CGlxViewBase::DisableTitle" );
	}

// DoActivateL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage)
    {
    TRACER( "CGlxViewBase::DoActivateL" );
    // Store view activate values
    iPrevViewId = aPrevViewId;
    iCustomMessageId = aCustomMessageId;
    
    delete iCustomMessage;
    iCustomMessage = NULL;
    iCustomMessage = HBufC8::NewL( aCustomMessage.Length() );
    
    TPtr8 customMessagePtr = iCustomMessage->Des();
    customMessagePtr.Copy( aCustomMessage );

	// show the view specific toolbar
    CAknToolbar* toolbar = GetToolBar();

    // Get the pointer to the application toolbar 
    // (when there isn't a view specific toolbar) to set it visible 
    if ( !toolbar ) 
        { 
        toolbar = AppUi()->PopupToolbar();       
        } 
    else
        {
        // Deactivate the toolbar untill the view gets activated properly.
        SetToolbarItemsDimmed(ETrue);
        }
    toolbar->SetToolbarVisibility(ETrue);
    
    // View should be activated asynchronously, since there can be a lot of processing
    // that may result in the view being in an indeterminate state if it leaves

    // HOWEVER, making the view asynchronous breaks the assumptions of the slideshow design
    // and results in slideshow not functioning correctly
    // Therefore, we have used the following nasty workaround to make this work,
    // otherwise, either view activation or slideshow would require considerable refactoring
    // TODO: This should be refactored whenever the opportunity arises
    //
    // The workaround is to have a flag to determine if the view should be activated
    // synchronously or asynchronously.
    if ( iSyncActivation )
        {
        ViewActivateL();
        }
    else
        {
        if ( !iViewActivateCallback->IsActive() )
        	{
            iViewActivateCallback->CallBack();
            }
        }
    }
	
// -----------------------------------------------------------------------------
// DoDeactivate
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::DoDeactivate()
	{
	TRACER( "CGlxViewBase::DoDeactivate" );
	// hide the view specific toolbar
    CAknToolbar* toolbar = GetToolBar();

    // Get the pointer to the application toolbar 
    // (when there isn't a view specific toolbar) to hide it properly 
    if ( !toolbar ) 
        { 
        toolbar = AppUi()->PopupToolbar(); 
        } 
    toolbar->SetToolbarVisibility(EFalse); 
    // Clear the toolbar background as well
    toolbar->MakeVisible(EFalse); 

	if ( iViewActivateCallback->IsActive() )
	    {
	    iViewActivateCallback->Cancel();
	    }
	
    CancelViewAnimation(); 
	
    // Deactivate command handlers
    TInt count = iCommandHandlerList.Count();
    TInt i = 0;
    
    while ( i < count )
        {
        iCommandHandlerList[i]->Deactivate();
        i++;
        }
	
    TRAP_IGNORE(InitAnimationL(EGlxViewAnimationExit));
	}
	

// -----------------------------------------------------------------------------
// CGlxViewBase::FetchAttributesL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::FetchAttributesL()
	{
	TRACER( "CGlxViewBase::FetchAttributesL" );
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CGlxViewBase::FetchAttributesForCommandL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::FetchAttributesForCommandL(TInt /*aCommand*/)
	{
	TRACER( "CGlxViewBase::FetchAttributesForCommandL" );
	// No implementation required
	}

// -----------------------------------------------------------------------------
// Handles a view activation event from viewserv.
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxViewBase::ViewActivatedL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    TRACER("CGlxViewBase::ViewActivatedL()");
    if ( dynamic_cast<CAknViewAppUi*>( iEikonEnv->EikAppUi() ) )
        {
        CAknView::ViewActivatedL(aPrevViewId, aCustomMessageId, aCustomMessage);
        }
    else 
        {
        GLX_LOG_INFO("Not in CAknViewAppUi -> CAknView::AknViewActivatedL()");
        AknViewActivatedL( aPrevViewId, aCustomMessageId, aCustomMessage );
        }
    }

// -----------------------------------------------------------------------------
// Handles a view deactivation event from viewserv.
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxViewBase::ViewDeactivated()
    {   
    TRACER("CGlxViewBase::ViewDeactivated()");
    if ( dynamic_cast<CAknViewAppUi*>( iEikonEnv->EikAppUi() ) )
        {
        CAknView::ViewDeactivated();
        }
    else 
        {
        GLX_LOG_INFO("Not in CAknViewAppUi -> DoDeactivate()");
        DoDeactivate();
        }
    }
  
    
// -----------------------------------------------------------------------------
// HandleViewCommandL
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxViewBase::HandleViewCommandL(TInt /*aCommand*/)
	{
	TRACER( "CGlxViewBase::HandleViewCommandL" );
	return EFalse;
	}
 
// -----------------------------------------------------------------------------
// ViewDynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::ViewDynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
    {
    TRACER( "CGlxViewBase::ViewDynInitMenuPaneL" );
    // No implementation - derived classes may override this
    }

// -----------------------------------------------------------------------------
// ViewAnimationSupported
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxViewBase::ViewAnimationSupported(TGlxViewswitchAnimation /*aType*/) 
    {
    TRACER( "CGlxViewBase::ViewAnimationSupported" );
    // Default implementation - may be overridden
    return EFalse;
    }
     
// -----------------------------------------------------------------------------
// DoViewAnimationL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::DoViewAnimationL(TGlxViewswitchAnimation /*aType*/, TGlxNavigationDirection /*aDirection*/) 
    {
    TRACER( "CGlxViewBase::DoViewAnimationL" );
    // Do nothing by default
    } 

// -----------------------------------------------------------------------------
// SetToolbarStateL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxViewBase::SetToolbarStateL()
    {
    TRACER( "CGlxViewBase::SetToolbarStateL" );
    // Derived MediaListViewBase has the implementation.
    }
// -----------------------------------------------------------------------------
// SetGridToolBar
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxViewBase::SetGridToolBar(CAknToolbar* aToolbar)
    {
    TRACER("CGlxViewBase::SetGridToolBar()");
    iToolbar = aToolbar;
    iUiUtility->SetGridToolBar(aToolbar);
    }
// -----------------------------------------------------------------------------
// GetToolBar
// -----------------------------------------------------------------------------
//
EXPORT_C CAknToolbar* CGlxViewBase::GetToolBar()
    {
    TRACER("CGlxViewBase::GetToolBar()");
    if(iToolbar)
        {
        return iToolbar;
        }
    return Toolbar();     
    }
// -----------------------------------------------------------------------------
// InitAnimationL
// -----------------------------------------------------------------------------
//	
void CGlxViewBase::InitAnimationL(TGlxViewswitchAnimation aType) 
    { 
    TRACER( "CGlxViewBase::InitAnimationL" );
    if ( ViewAnimationSupported(aType) ) 
        { 
		/*@@ 
        /// @todo : Remove hack when TAlfTimedValue fixed
        iViewAnimationTimedValue.Now(); // Hack to force reevaluation of internal "interpolating" flag: otherwise
            // call to RemainingTime() returns a hugely wrong number
            
        TInt remainingTime = iViewAnimationTimedValue.RemainingTime(); 
            
        iViewAnimationTimedValue.Set(1 - iViewAnimationTimedValue.Now()); // In case previous animation is incomplete
                                // Assumes opposite animations happen in sequence: entry - exit - entry etc 
        iViewAnimationTimedValue.Set(1, KGlxViewSwitchAnimationDuration - remainingTime); */
        
        DoViewAnimationL(aType, iUiUtility->ViewNavigationDirection());
        iViewAnimationInProgress = aType;
        } 
    else 
        { 
        // Immediately complete animation 
        ViewAnimationComplete(aType); 
        } 
    }

// -----------------------------------------------------------------------------
// AnimationComplete
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxViewBase::AnimationComplete(MGlxAnimation* /*aAnimation*/) // From MGlxAnimationObserver 
    { 
    TRACER( "CGlxViewBase::AnimationComplete" );
    CancelViewAnimation(); 
    } 

// -----------------------------------------------------------------------------
// OfferToolbarEventL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxViewBase::OfferToolbarEventL( TInt aCommand )
    {
    TRACER( "CGlxViewBase::OfferToolbarEventL" );
    CAknToolbar* toolbar = GetToolBar();
    if(toolbar)
        {
        CAknButton* slideshowButton =
            static_cast<CAknButton*> (toolbar->ControlOrNull(EGlxCmdSlideshow));
        TBool slideshowdimmed = EFalse;

        //Here after the toolbar cmd is processed it is enabled
        //back. For share the toolbar state should be same as it was 
        //earlier, so we take the current state and reset back after
        //the command is processed.
        CAknButton* uploadButton =
            static_cast<CAknButton*> (toolbar->ControlOrNull(EGlxCmdUpload));
        TBool uploaddimmed = EFalse;

        CAknButton* markButton =
            static_cast<CAknButton*> (toolbar->ControlOrNull(EGlxCmdStartMultipleMarking));
        TBool markButtondimmed = EFalse;

        if(slideshowButton)
            {
            // Get current button state
            CAknButtonState* currentState = slideshowButton->State();
            slideshowdimmed = slideshowButton->IsDimmed();
            }

        if(markButton)
            {
            // Get current button state
            CAknButtonState* currentState = markButton->State();
            markButtondimmed = markButton->IsDimmed();
            }        
        
        if(uploadButton)
            {
            // Get current button state
            CAknButtonState* currentState = uploadButton->State();
            uploaddimmed = uploadButton->IsDimmed();
            }

        // Deactivate the toolbar. Don't accept the toolbar input when the command
        // execution is already in progress.
        SetToolbarItemsDimmed(ETrue); 
        
        // Execute the command recieved.
        ProcessCommandL(aCommand);

        // Activate back the toolbar and set it's state properly
        // after command execution.
        SetToolbarStateL();

        if(!markButtondimmed)
            {
            toolbar->SetItemDimmed(EGlxCmdStartMultipleMarking, EFalse, ETrue);
            }

        if(!slideshowdimmed)
            {
            toolbar->SetItemDimmed(EGlxCmdSlideshowPlay, EFalse, ETrue);
            }
        
        if(uploaddimmed || (aCommand == EGlxCmdStartMultipleMarking))
            {
            toolbar->SetItemDimmed(EGlxCmdUpload, ETrue, ETrue);
            }
        }    
    }

// -----------------------------------------------------------------------------
// SetToolbarObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxViewBase::SetToolbarObserver(MAknToolbarObserver* aObserver)
    {
    TRACER( "CGlxViewBase::SetToolbarObserver" );
    //Register the view to recieve toolbar events. ViewBase handles the events
    if ( GetToolBar() )
        {
        GetToolBar()->SetToolbarObserver( aObserver );
        }
     }

// -----------------------------------------------------------------------------
// CancelViewAnimation
// -----------------------------------------------------------------------------
//	
void CGlxViewBase::CancelViewAnimation() 
    { 
    TRACER( "CGlxViewBase::CancelViewAnimation" );
    if ( iViewAnimationInProgress != EGlxViewAnimationNone ) 
        { 
        ViewAnimationComplete(iViewAnimationInProgress); 
        iViewAnimationInProgress = EGlxViewAnimationNone; 
        } 
    } 

// -----------------------------------------------------------------------------
// ViewAnimationComplete
// -----------------------------------------------------------------------------
//	
void CGlxViewBase::ViewAnimationComplete(TGlxViewswitchAnimation aType) 
    {
    TRACER( "CGlxViewBase::ViewAnimationComplete" );
    if ( aType == EGlxViewAnimationExit )
        {
        // Deactivate view        
        DoViewDeactivate();
        iViewActivated = EFalse; // View is DeActivated
        }
    } 

//------------------------------------------------------------------------------
// GetTitlePaneControlL
//------------------------------------------------------------------------------
//
CAknTitlePane* CGlxViewBase::GetTitlePaneL()
    {
    TRACER( "CGlxViewBase::GetTitlePaneL" );
 
     return (( CAknTitlePane* )StatusPane()->ControlL
                (TUid::Uid( EEikStatusPaneUidTitle )));
    }

// -----------------------------------------------------------------------------
// DoHandleCommandL
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CGlxViewBase::DoHandleCommandL(TInt /*aCommand*/)
    {
    TRACER( "CGlxViewBase::DoHandleCommandL" );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Callback for view activate
// -----------------------------------------------------------------------------
//	
TBool CGlxViewBase::ViewActivateCallbackL(TAny* aPtr)
    {
    TRACER( "CGlxViewBase::ViewActivateCallbackL()" );
    __ASSERT_DEBUG( aPtr, Panic( EGlxPanicNullPointer ) );
    static_cast<CGlxViewBase*>( aPtr )->ViewActivateL();
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Perform view activate
// -----------------------------------------------------------------------------
//	
void CGlxViewBase::ViewActivateL()
    {
    TRACER("CGlxViewBase::ViewActivateL()");
    CancelViewAnimation(); 

    // View is Activated
    iViewActivated = ETrue;
	DoViewActivateL( iPrevViewId, iCustomMessageId, *iCustomMessage );

    TInt count = iCommandHandlerList.Count();
    TInt i = 0;
    
    while ( i < count )
        {
        iCommandHandlerList[i]->ActivateL(Id().iUid);
        i++;
        }
    
    //Check if transition effect is already started.
    //Calling the 'EndFullScreen()' actually starts the FS transition effect.
    if(iIsTransEffectStarted)
		{
		GfxTransEffect::EndFullScreen();
		iIsTransEffectStarted = EFalse;
		}

    InitAnimationL(EGlxViewAnimationEntry); 
    }

// ----------------------------------------------------------------------------
// SetToolbarItemsDimmed
// ----------------------------------------------------------------------------
//
void CGlxViewBase::SetToolbarItemsDimmed(TBool aDimmed)
    {
    TRACER("CGlxViewBase::SetToolbarItemsDimmed()");
    CAknToolbar* toolbar = GetToolBar();
    if (toolbar)
        {
        toolbar->SetItemDimmed(EGlxCmdSlideshowPlay, aDimmed, ETrue);
        toolbar->SetItemDimmed(EGlxCmdStartMultipleMarking, aDimmed, ETrue);
        toolbar->SetItemDimmed(EGlxCmdUpload, aDimmed, ETrue);
        }
    }



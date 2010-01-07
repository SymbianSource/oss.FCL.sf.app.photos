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
* Description:    Creates a Fetcher Dialog
*
*/



//  EXTERNAL INCLUDES
#include <aknappui.h>
#include <akntoolbar.h>
#include <eikdialg.h>					  // for CEikDialog
#include <AknUtils.h>
#include <StringLoader.h>
#include <eikdialogext.h>
#include <AknsBasicBackgroundControlContext.h>
#include <MMGFetchVerifier.h>             // For VerifySelectionL()

#include <glxuiutility.h>
#include <glxscreenfurniture.h>
#include <glxtracer.h> 					  // Logging macros
#include <glxlog.h>
#include <glxcommandhandlers.hrh>         //command ids

#include <mglxmedialist.h>				  // MGlxMediaList, CMPXCollectionPath

#include <alf/alfevent.h>
#include <alf/alfdisplay.h>

//  CLASS HEADER
#include <glxfetcherdialog.rsg>           // FOR GETTING VIEW ID AND RESOURCE ID
#include "glxfetcherdialog.h"
#include "glxfetchercontainer.h"
#include "glxfetcher.hrh"

#define GetAppUi() (dynamic_cast<CAknAppUi*>(iEikonEnv->EikAppUi()))
const TInt KControlId = 1;
     
//-----------------------------------------------------------------------------
// C++ default constructor.
//-----------------------------------------------------------------------------
inline CGlxFetcherDialog::CGlxFetcherDialog(
            CDesCArray& aSelectedFiles, 
            MMGFetchVerifier* aVerifier, 
            TGlxFilterItemType aFilterType,
            const TDesC& aTitle,
            TBool aMultiSelectionEnabled)
        : iSelectedFiles( aSelectedFiles ),
          iVerifier( aVerifier ),
          iFilterType( aFilterType ),
          iTitle( aTitle ),
          iMultiSelectionEnabled(aMultiSelectionEnabled)
	{
	TRACER("CGlxFetcherDialog::CGlxFetcherDialog");
	// No implementation required
	}

//-----------------------------------------------------------------------------
// Two-phased constructor.
//-----------------------------------------------------------------------------
CGlxFetcherDialog* CGlxFetcherDialog::NewL(CDesCArray& aSelectedFiles, 
               MMGFetchVerifier* aVerifier, TGlxFilterItemType aFilterType ,
                const TDesC& aTitle, TBool aMultiSelectionEnabled )
	{
	TRACER("CGlxFetcherDialog::NewL");
	CGlxFetcherDialog* self=
	    new( ELeave ) CGlxFetcherDialog(
	        aSelectedFiles, aVerifier, aFilterType, aTitle, aMultiSelectionEnabled );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
   	}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGlxFetcherDialog::~CGlxFetcherDialog()
	{
	TRACER("CGlxFetcherDialog::~CGlxFetcherDialog");
	if(iFetcherContainer)
	    {
	    // Restore the Toolbar as it was in the Calling application
	    if(GetAppUi()->CurrentFixedToolbar())  // there is Hardware Specific Output for Fixed ToolBar
	        {
	        GetAppUi()->CurrentFixedToolbar()->SetToolbarVisibility(ETrue);
	        }

	    TRAP_IGNORE(iFetcherContainer->SetPreviousTitleL());
		}
	if (iUiUtility)
		{
	    iUiUtility->Close();
		}
	}

//-----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// Create the alfscreenbuffer with a generic buffer screen id
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::ConstructL()
	{
	TRACER("CGlxFetcherDialog::ConstructL");
	// always first call the base class
    CAknDialog::ConstructL( R_MULTISELECT_FETCHER_MENUBAR );

    // Get the Instance of the toolbar and disable it as it is note required in the Fetcher Application
    if(GetAppUi()->CurrentFixedToolbar())  // there is Hardware Specific Output for Fixed ToolBar
        {
        GetAppUi()->CurrentFixedToolbar()->SetToolbarVisibility(EFalse);
        }
	// Get the Hitchcock environment.
    iUiUtility = CGlxUiUtility::UtilityL();
    iEnv = iUiUtility->Env();
    iFetchUri = EFalse;
    }
    
//-----------------------------------------------------------------------------
// CGlxFetcherDialog::CreateCustomControlL
//-----------------------------------------------------------------------------
SEikControlInfo CGlxFetcherDialog::CreateCustomControlL(TInt aControlType)
    {
    TRACER("CGlxFetcherDialog::CreateCustomControlL");
    // Create Carousal control and add it to HUI CCoecontrol
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;

    switch( aControlType )
        {
        case EFetcherControl :
            {
            iFetcherContainer = CGlxFetcherContainer::NewL( 
                    iFilterType, iTitle, *this, iMultiSelectionEnabled );
            controlInfo.iControl = iFetcherContainer;  // transfers ownership      
            break;
            }
        default :
            break;
        }
    return controlInfo;
    }

//-----------------------------------------------------------------------------
// CGlxFetcherDialog::ProcessCommandL
// This processes the events to the OkToExitL
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::ProcessCommandL(TInt aCommandId)
	{
	TRACER("CGlxFetcherDialog::ProcessCommandL");

	if( iMenuBar )
	    {
	    iMenuBar->StopDisplayingMenuBar();
	    }
	switch( aCommandId )
		{
		case EAknSoftkeyCancel :
			{
			TryExitL( aCommandId );
			break;
			}
		case EAknSoftkeySelect :
		case EGlxCmdSelectMarked :
        case EAknSoftkeyOk :
            {
            GLX_LOG_INFO("CGlxFetcherDialog::ProcessCommandL() : EAknSoftkeyOk");
			TryExitL( aCommandId );
			break;
            }
		default :
		    {
            iFetcherContainer->DoExecuteL( aCommandId );
            break;
		    }
		}
	}
//-----------------------------------------------------------------------------
// CGlxFetcherDialog::OkToExitL
//-----------------------------------------------------------------------------
TBool CGlxFetcherDialog::OkToExitL(TInt aKeycode)
	{
	TRACER("CGlxFetcherDialog::OkToExitL");
	GLX_LOG_INFO1("CGlxFetcherDialog::OkToExitL : %d",aKeycode );
    TBool retVal = ETrue;
    TBool retrieveUriValue = EFalse;
    switch( aKeycode )
        {
        case EAknSoftkeySelect :
        case EGlxCmdSelectMarked :
        case EAknSoftkeyOk :
        case EAknCmdOpen :
            {
            // Retreives the uri's of the selected files and verifies if it is a 
            // supported format
            if (!iFetchUri)
                {
                iFetchUri = ETrue;
                retrieveUriValue = iFetcherContainer->RetrieveUrisL(iSelectedFiles, iFetchUri);
                if ( iVerifier && !iVerifier->VerifySelectionL( &iSelectedFiles ) )
                    {
                    iSelectedFiles.Reset();
                    retVal = EFalse;
                    //if the corrupt file is selected then reset the flag to again enable
                    //the selection.
                    iFetcherContainer->SetFileAttached(EFalse);
                    } 
                if (!retrieveUriValue)
                    {
                    retVal = EFalse;
                    }
                if (iMultiSelectionEnabled && retVal)
                    {
                    iFetcherContainer->DoExecuteL(EGlxCmdEndMultipleMarking);    
                    }
                }
            else
                {
                // if uri is being fetched then do not set the return value true, else it would cause
                // Destructor of Dialog to be called and result in a crash when OKtoExitL is called multiple times.
                retVal = EFalse;
                }
            break;
            }
        case EAknSoftkeyCancel : // exit dialog
        case EAknCmdExit :
            {
            break;
            }
        case EAknCmdMark:
        case EAknSoftkeyMark:
            {
            if (iMultiSelectionEnabled)
                {                
                if(iUiUtility->IsPenSupported())
                    {
                    //Since the MSK is disabled we always get EAknCmdMark
                    //when we select msk hardkey in touch phone, so we need 
                    //to toggle between mark/unmark on the same hardkey event
                    if (!iMarkStarted)
                        {
                        iFetcherContainer->DoExecuteL(EGlxCmdStartMultipleMarking);
                        iMarkStarted = ETrue;
                        }
                    else
                        {
                        MGlxMediaList& mediaList = iFetcherContainer->MediaList();
                        TInt focusIdx = mediaList.FocusIndex();                        
                        if ( mediaList.IsSelected(focusIdx) )
                            iFetcherContainer->DoExecuteL(EAknCmdUnmark);
                        else
                            iFetcherContainer->DoExecuteL(EAknCmdMark);
                        }
                    }
                else
                    {
                    if (!iMarkStarted)
                        {
                        iFetcherContainer->DoExecuteL(EGlxCmdStartMultipleMarking);
                        iMarkStarted = ETrue;
                        }
                    else
                        {
                        iFetcherContainer->DoExecuteL(EAknCmdMark);
                        }
                    }
                }
            retVal = EFalse;
            break;
            }
        case EAknCmdUnmark:
        case EAknSoftkeyUnmark:
            {
            if (iMultiSelectionEnabled)
                {
                iFetcherContainer->DoExecuteL(EAknCmdUnmark);
				                
                //@ fix for ELWU-7RA7NX 
                //@ Reset the flag on no selection, else it'll not pass the events to container for
                //@ EGlxCmdStartMultipleMarking  case.
                MGlxMediaList& mediaList = iFetcherContainer->MediaList() ;
                if ( mediaList.SelectionCount() <= 0 )
	                {
	                iMarkStarted = EFalse;	
	                } 
					
                HBufC* mskTextMark = StringLoader::LoadLC( R_GLX_MARKING_MARK );
                HBufC* rskTextCancel = StringLoader::LoadLC( R_GLX_SOFTKEY_CANCEL );
                iUiUtility->ScreenFurniture()->ModifySoftkeyIdL(
                        CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
                        EAknCmdMark, 0, *mskTextMark);
                iUiUtility->ScreenFurniture()->ModifySoftkeyIdL(
                        CEikButtonGroupContainer::ERightSoftkeyPosition,
                        EAknSoftkeyCancel, 0, *rskTextCancel);
                CleanupStack::PopAndDestroy(rskTextCancel);
                CleanupStack::PopAndDestroy(mskTextMark);
                }
            retVal = EFalse;
            break;
            }
        default :
            break;
        }
    return retVal;
	}

//-----------------------------------------------------------------------------
// CGlxFetcherDialog::SizeChanged
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::SizeChanged()
	{
	TRACER("CGlxFetcherDialog::SizeChanged");
	if (iFetcherContainer)
	    {
	    TRAP_IGNORE (iFetcherContainer->HandleSizeChangeL());    
	    }
	}

//-----------------------------------------------------------------------------
// CGlxFetcherDialog::PreLayoutDynInitL
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::PreLayoutDynInitL()
    {
    TRACER("CGlxFetcherDialog::PreLayoutDynInitL");
    CreateLineByTypeL( KNullDesC, KControlId, EFetcherControl, NULL );
    }
//-----------------------------------------------------------------------------
// CGlxFetcherDialog::PostLayoutDynInitL
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::PostLayoutDynInitL()
    {
    TRACER("CGlxFetcherDialog::PostLayoutDynInitL");
    
    // Create and Display Grid widget 
    iFetcherContainer->CreateAndDisplayGridL();
    }

//-----------------------------------------------------------------------------
// CGlxFetcherDialog::DynInitMenuPaneL
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::DynInitMenuPaneL(
        TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    TRACER("CGlxFetcherDialog::DynInitMenuPaneL");
    // No Implementation
    }

//-----------------------------------------------------------------------------
// CGlxFetcherDialog::OfferKeyEventL
//-----------------------------------------------------------------------------
TKeyResponse CGlxFetcherDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    TRACER("CGlxFetcherDialog::OfferKeyEventL");
    TKeyResponse response = EKeyWasNotConsumed;
    response = iFetcherContainer->OfferKeyEventL(aKeyEvent,aType);
 
    if(response == EKeyWasNotConsumed)
        response = CAknDialog::OfferKeyEventL(aKeyEvent,aType);

    return response;    
    }

//-----------------------------------------------------------------------------
// CGlxFetcherDialog::HandlePointerEventL
// Handles all pointer events to the screen.
// Offers the events to the primary display control (container - widgets)
// and also finally calls handlescreenbufferevent and draws the bitmap onto
// the screen
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
    {
    TRACER("CGlxFetcherDialog::HandlePointerEventL");
    CCoeControl::HandlePointerEventL( aPointerEvent );
    iEnv->PrimaryDisplay().HandlePointerEventL( aPointerEvent );
    }

//-----------------------------------------------------------------------------
// CGlxFetcherDialog::HandleDoubleTapEvent
// Callback from the container for double tap events
// Offers the event from container to Dialog
//-----------------------------------------------------------------------------
void CGlxFetcherDialog::HandleDoubleTapEventL(TInt aCommandId)
    {
    TRACER("CGlxFetcherDialog::HandleTapEvent");
    Extension()->iPublicFlags.Set(CEikDialogExtension::EDelayedExit);
    ProcessCommandL(aCommandId);
    Extension()->iPublicFlags.Clear(CEikDialogExtension::EDelayedExit);
    }
    
//-----------------------------------------------------------------------------
// CGlxFetcherDialog::CallCancelFetcherL
// This function will be called from CGlxFetcher::CancelFetcherL
// This will ensure exit of fetcher dialog & fetcher app
//-----------------------------------------------------------------------------

void CGlxFetcherDialog::CallCancelFetcherL(TInt aCommandId)
	{
	TRACER("CGlxFetcherDialog::CallCancelFetcherL");
	TryExitL(aCommandId);
	}
//  End of File

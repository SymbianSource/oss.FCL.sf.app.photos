/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Implementation of Slideshow Settings Dialog
*
*/




//  CLASS HEADER
#include "shwslideshowsettingsdialog.h"

//  EXTERNAL INCLUDES
#include <hlplch.h>						// for HlpLauncher
#include <eikdialg.h>					// for CEikDialog
#include <aknappui.h>					// for CAknAppUi
#include <akntitle.h>					// for CAknTitlePane
#include <StringLoader.h>				// for StringLoader
#include <bautils.h>					// for BaflUtils
#include <data_caging_path_literals.hrh>// for KDC_APP_RESOURCE_DIR
#include <coeutils.h>					// for ConeUitls
#include <eikon.hrh>                    // for Navi-scroll key events
#include <akntoolbar.h>

//  INTERNAL INCLUDES
#include <glxlog.h>
#include <glxtracer.h>
#include <glxpanic.h>                    // For Panics
#include <shwsettingsdialog.rsg> 
#include <glxresourceutilities.h>        // for CGlxResourceUtilities		  
#include "shwslideshowsettings.hrh"
#include "shwslideshowengine.h"			 // for CShwSlideshowEngine
#include "shwslideshowsettings.hrh"		 // for Command Ids and Control Ids
#include "shwsettingsmodel.h"			 // for CShwSettingsModel
#include "shwslideshowsettingslist.h"	 // for CShwSlideshowSettingsList

#define GetAppUi() (dynamic_cast<CAknAppUi*>(iEikonEnv->EikAppUi()))
//CONSTANTS
namespace
	{
    // uid and id from doc/help_parameters_for_photos.xls
	const TInt KShwHelpUID = 0x2000A778; 
	_LIT( KSHW_HLP_SLIDESHOW_SETTINGS, "LGAL_HLP_SLIDESHOW_SETTINGS" );
	}

//-----------------------------------------------------------------------------
// C++ default constructor.
//-----------------------------------------------------------------------------
inline CShwSlideshowSettingsDialog::CShwSlideshowSettingsDialog()
	{
	// No implementation required
	}

//-----------------------------------------------------------------------------
// Two-phased constructor.
//-----------------------------------------------------------------------------
CShwSlideshowSettingsDialog* CShwSlideshowSettingsDialog::NewL()
	{
	TRACER("CShwSlideshowSettingsDialog::NewL");
	GLX_LOG_INFO("CShwSlideshowSettingsDialog::NewL");	
	CShwSlideshowSettingsDialog* self=new(ELeave)CShwSlideshowSettingsDialog();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}



//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CShwSlideshowSettingsDialog::~CShwSlideshowSettingsDialog()
	{
	TRACER("CShwSlideshowSettingsDialog::~CShwSlideshowSettingsDialog");
	GLX_LOG_INFO("CShwSlideshowSettingsDialog::~CShwSlideshowSettingsDialog");	
	
	// Draw the toolbar 
	SetSlShwToolbarVisibility(ETrue);
	
	delete iShwSettings;
    // Delete resource file
	if (iResourceOffset)
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        iResourceOffset = NULL;
        }
	// If the status pane was enabled , set it to not visbible.
	// used in FS view
	if (iStatusPaneChanged)
	    {
	    iStatusPane->MakeVisible(EFalse);
	    }
	if (iPreviousTitle)
        {
        delete iPreviousTitle;
        }
    }

//-----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
//-----------------------------------------------------------------------------
void CShwSlideshowSettingsDialog::ConstructL()
	{
	TRACER("CShwSlideshowSettingsDialog::ConstructL");
	GLX_LOG_INFO("CShwSlideshowSettingsDialog::ConstructL");	
	
	// hide toolbar.	
	SetSlShwToolbarVisibility(EFalse);
	iStatusPaneChanged = EFalse;
	iStatusPane = GetAppUi()->StatusPane();
    iTitlePane = ( CAknTitlePane* )iStatusPane->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ));
	// If Status pane not visible, then make it visible.
	// Set the status pane flag as True, to be used in destructor, if 
	// status pane is made visible.
	// This is used in Fullscreen.
	if(!iStatusPane->IsVisible())  
        {
        iStatusPane->MakeVisible(ETrue);
        iStatusPaneChanged = ETrue;
        }   
	// Load the dialog resource file
	_LIT(KShwSettingDlgResource,"shwsettingsdialog.rsc");	
    TFileName resourceFile(KDC_APP_RESOURCE_DIR);
    resourceFile.Append(KShwSettingDlgResource); 
    CGlxResourceUtilities::GetResourceFilenameL( resourceFile );  
    iResourceOffset = iCoeEnv->AddResourceFileL( resourceFile );

	iShwSettings = CShwSettingsModel::NewL();

    // CAknDialog takes the menu resource as constructor parameter
	CAknDialog::ConstructL( R_SHW_SETTINGSLIST_MENU_BAR );
    
    // set the title to the dialog, Note that avkon dialogs do not support
    // setting the title in the status pane so we need to do it the hard way
    // get status pane
	SetTitleL();
	}
	
// ---------------------------------------------------------------------------
// SetSlShwToolbarVisibility()
// ---------------------------------------------------------------------------
void CShwSlideshowSettingsDialog::SetSlShwToolbarVisibility(TBool aVisible)
    {
    TRACER("CShwSlideshowSettingsDialog::SetSlShwToolbarVisibility");
    CAknAppUi* appUi = GetAppUi();
    __ASSERT_DEBUG(appUi, Panic(EGlxPanicNullPointer));
    CAknToolbar* popupToolbar = appUi->PopupToolbar();
    if(popupToolbar)
        {
        popupToolbar->SetToolbarVisibility( !aVisible ); 
        popupToolbar->MakeVisible( !aVisible );
	     if(!aVisible)
			{
	        popupToolbar->DrawNow();	
			}
        }  	
	
	CAknToolbar* toolbar = appUi->CurrentFixedToolbar();
    if(toolbar)
        {
        toolbar->SetToolbarVisibility(aVisible);
        toolbar->MakeVisible( aVisible );
        } 
	
	CAknToolbar* currentPopupToolbar = appUi->CurrentPopupToolbar();
    if(currentPopupToolbar)
        {
        currentPopupToolbar->SetToolbarVisibility(aVisible);
        currentPopupToolbar->MakeVisible( aVisible ); 
        }
    }
	
// ---------------------------------------------------------------------------
// SetTitleL()
// ---------------------------------------------------------------------------
void CShwSlideshowSettingsDialog::SetTitleL()
    {
    TRACER("CShwSlideshowSettingsDialog::SetTitleL");
    GLX_LOG_INFO("CShwSlideshowSettingsDialog::SetTitleL");
    // Stores the previous Title
    iPreviousTitle = iTitlePane->Text()->AllocL();
    HBufC* aTitleText = StringLoader::LoadLC( 
                    R_SHW_SETTINGSLIST_TITLE_PANE_TXT, iEikonEnv );
                
    iTitlePane->SetTextL( *aTitleText );
    CleanupStack::PopAndDestroy( aTitleText );
    }

// -----------------------------------------------------------------------------
// SetPreviousTitleL - Restores back the previous title, used in Slideshow Settings only
// -----------------------------------------------------------------------------
//
void CShwSlideshowSettingsDialog::SetPreviousTitleL()
    {
    TRACER("CShwSlideshowSettingsDialog::SetPreviousTitleL");
    GLX_LOG_INFO("CShwSlideshowSettingsDialog::SetPreviousTitleL");
    // Restore the Title back of the Calling View
    iTitlePane->SetTextL( *iPreviousTitle );
    }

//-----------------------------------------------------------------------------
// CShwSlideshowSettingsDialog::ProcessCommandL
//-----------------------------------------------------------------------------
void CShwSlideshowSettingsDialog::ProcessCommandL(TInt aCommandId)
	{
	TRACER("CShwSlideshowSettingsDialog::ProcessCommandL");
	GLX_LOG_INFO("CShwSlideshowSettingsDialog::ProcessCommandL");			
	switch (aCommandId)
		{
		case EShwCmdSettingsChange: 
			{
			CAknDialog::ProcessCommandL(aCommandId);
		    iItemList->UpdateListBoxL(iItemList->ListBox()->CurrentItemIndex());
			break;			
			}
		case EShwCmdHelp:
			{
			TCoeHelpContext helpContext;
			helpContext.iMajor = TUid::Uid( KShwHelpUID );
			helpContext.iContext.Copy( KSHW_HLP_SLIDESHOW_SETTINGS );
			
			const TInt KListSz = 1;
			CArrayFix<TCoeHelpContext>* contextList =
						new (ELeave) CArrayFixFlat<TCoeHelpContext>( KListSz );
			CleanupStack::PushL(contextList);
			contextList->AppendL(helpContext);
			
			HlpLauncher::LaunchHelpApplicationL(
                iEikonEnv->WsSession(), contextList );
			CleanupStack::Pop( contextList );
			break;
			}
		case EShwCmdExit:
			{
			TryExitL(EShwCmdExit);
			break;
			}			
		default:
			{
			CAknDialog::ProcessCommandL(aCommandId);
			break;
			}
		};				
	}
	
	
//-----------------------------------------------------------------------------
// CShwSlideshowSettingsDialog::CreateCustomControlL
//-----------------------------------------------------------------------------
SEikControlInfo CShwSlideshowSettingsDialog::CreateCustomControlL(TInt 
																aControlType)
	{
	TRACER("CShwSlideshowSettingsDialog::CreateCustomControlL");
	GLX_LOG_INFO("CShwSlideshowSettingsDialog::CreateCustomControlL");
    // create control info, no flags or trailer text set
	SEikControlInfo controlInfo;
	controlInfo.iControl 		= NULL;
	controlInfo.iTrailerTextId 	= 0;
	controlInfo.iFlags 			= 0;
	
  	if (aControlType == EShwCtSettingListBox)
		{
		iItemList = CShwSlideShowSettingsList::NewL(*iShwSettings);
		controlInfo.iControl = iItemList; // giving ownership
		}
	return controlInfo; // returns ownership of ItemList
	}

//-----------------------------------------------------------------------------
// CShwSlideshowSettingsDialog::OkToExitL
//-----------------------------------------------------------------------------
TBool CShwSlideshowSettingsDialog::OkToExitL(TInt aKeycode)
	{
	TRACER("CShwSlideshowSettingsDialog::OkToExitL");
	GLX_LOG_INFO("CShwSlideshowSettingsDialog::OkToExitL");					
    TBool retVal = EFalse;
    switch (aKeycode)
        {
        case EAknSoftkeyChange:
            {
            iItemList->UpdateListBoxL(iItemList->ListBox()->CurrentItemIndex(), ETrue);
            break;
            }
        case EAknSoftkeyOptions: // display menu only
            {
            DisplayMenuL();
            break;
            }
        case EAknSoftkeyBack: // exit dialog
        case EEikCmdExit:
        case EAknCmdExit:
        case EShwCmdExit:        
            {
            SetPreviousTitleL();
            retVal = ETrue;
            break;
            }
        }
    return retVal;
	}

//-----------------------------------------------------------------------------
// CShwSlideshowSettingsDialog::SizeChanged
//-----------------------------------------------------------------------------
void CShwSlideshowSettingsDialog::SizeChanged()
	{
	TRACER("CShwSlideshowSettingsDialog::SizeChanged");
	GLX_LOG_INFO("CShwSlideshowSettingsDialog::SizeChanged");						
	if(iItemList) 
		{
		iItemList->ListBox()->SetRect(Rect());
		}
	CAknDialog::SizeChanged();
	}

//-----------------------------------------------------------------------------
// CShwSlideshowSettingsDialog::OfferKeyEventL
//-----------------------------------------------------------------------------
TKeyResponse CShwSlideshowSettingsDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TRACER("CShwSlideshowSettingsDialog::OfferKeyEventL");
    GLX_LOG_INFO("CShwSlideshowSettingsDialog::OfferKeyEventL");   
    // Fix for EEBD-79UL8Q - "No Naviscroll support in Slideshow settings
    // options menu"
    TKeyEvent localKeyEvent = aKeyEvent;
    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            // Handle the previous and next navi-scroll events and modify the 
            // key code to the corresponding values used by the dialog
            case EKeyPrevious:
                localKeyEvent.iCode = EKeyUpArrow;
                break;
            case EKeyNext:
                localKeyEvent.iCode = EKeyDownArrow;
                break;
            default:
                // nothing to change
                break;
            }
        }
    
    // pass the key event up to the parent to handle
    return CAknDialog::OfferKeyEventL( localKeyEvent, aType );
    }
//-----------------------------------------------------------------------------
// CShwSlideshowSettingsDialog::FocusChanged
//-----------------------------------------------------------------------------
void CShwSlideshowSettingsDialog::FocusChanged(TDrawNow  aDrawNow)
	{
    TRACER("CShwSlideshowSettingsDialog::FocusChanged");
    GLX_LOG_INFO("CShwSlideshowSettingsDialog::FocusChanged");   
	// hide toolbar.
    if(aDrawNow == EDrawNow )
    	{
    	SetSlShwToolbarVisibility(EFalse);
    	}
	}

//  End of File

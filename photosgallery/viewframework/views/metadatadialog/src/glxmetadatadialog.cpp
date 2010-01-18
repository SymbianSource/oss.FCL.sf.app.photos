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
* Description:    Implementation of Metadata dialog
*
*/

#include "glxmetadatadialog.h"

//system includes
#include <AknUtils.h>                   //for AknUtils
#include <lbsposition.h> 
#include <akntitle.h>
#include <coeaui.h>
#include <data_caging_path_literals.hrh>// KDC_APP_RESOURCE_DIR	
#include <eikmenub.h>                   // for CEikMenuBar
#include <StringLoader.h>
#include <akntoolbar.h>
#include <glxcommandhandleraddtocontainer.h>         // For CGlxCommandHandlerAddToContainer

//User includes
#include <glxmetadatadialog.rsg>
#include <glxresourceutilities.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <glxcollectionpluginall.hrh>
#include <glxfilterfactory.h>               // for TGlxFilterFactory
#include <glxuiutility.h>
#include <glxcommandhandlers.hrh>
#include <hlplch.h>						// for HlpLauncher
#include <photos.hlp.hrh>
#include <glxgallery.hrh>
#include <glxdetailsboundcommand.hrh>
#include <glxscreenfurniture.h>
#include <glxuiutilities.rsg>
#include <glxpanic.h>                    // For Panics
#include "glxmetadatacommandhandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxMetadataDialog* CGlxMetadataDialog::NewL( const TDesC& aUri  )
	{
	TRACER("CGlxMetadataDialog::NewL");
	
	CGlxMetadataDialog* self = new(ELeave) CGlxMetadataDialog(aUri );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CGlxMetadataDialog::CGlxMetadataDialog(const TDesC& aUri):iUri(aUri)
{

}
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxMetadataDialog::ConstructL()
	{
	TRACER("CGlxMetadataDialog::ConstructL");
		
	// Load dialog's resource file
	InitResourceL();
	
	
	iStatusPaneAvailable = EFalse;
	// set the title to the dialog, Note that avkon dialogs do not support
	// setting the title in the status pane so we need to do it the hard way
	// get status pane
	CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
	
	if(statusPane && statusPane->IsVisible())
		{
		iStatusPaneAvailable = ETrue;	    
		}

    // make the toolbar disabled
    SetDetailsDlgToolbarVisibility(EFalse);	       

	// do we have status pane
	if( statusPane )
		{
		GLX_LOG_INFO1("GLX_UMP::CGlxMetadataDialog::ConstructL::STATUS PANE = %d",statusPane->IsVisible());    
		// load the title text
		HBufC* text = StringLoader::LoadL(R_GLX_METADATA_VIEW_TITLE_DETAILS, iEikonEnv );
		SetTitleL( *text );
		if( text)
		    {
		    delete text;
		    }
		iAvkonAppUi->StatusPane()->MakeVisible(ETrue);			  
		}
		
	iUiUtility = CGlxUiUtility::UtilityL();	
	iAddToTag =  CGlxCommandHandlerAddToContainer::NewL(this, EGlxCmdAddTag, EFalse);
	iAddToAlbum =  CGlxCommandHandlerAddToContainer::NewL(this, EGlxCmdAddToAlbum, EFalse);
  
	// Call the base class' two-phased constructor
  	CAknDialog::ConstructL( R_METADATA_MENUBAR );
	
	// Instantiate the command handler
	iMetadataCmdHandler = CGlxMetadataCommandHandler::NewL(this);
	
	} 

// -----------------------------------------------------------------------------
// ~CGlxMetadataDialog
// -----------------------------------------------------------------------------
//
CGlxMetadataDialog::~CGlxMetadataDialog()
	{
    TRACER("CGlxMetadataDialog::~CGlxMetadataDialog");

    //To Disable the status pane if the dialog is launched from fullscreenview	
    if (!iStatusPaneAvailable && iAvkonAppUi)
        {
        iAvkonAppUi->StatusPane()->MakeVisible(EFalse);
        }

    TRAP_IGNORE(SetPreviousTitleL());

    delete iPreviousTitle;
    delete iMetadataCmdHandler;

    // Restore the Toolbar as it was in the Calling application
    SetDetailsDlgToolbarVisibility(ETrue);

    // If details launched from FullScreen View, while moving back,
    // all the UI components should be hidden. Hence processing this command here.
    if (iAvkonAppUi)
        {
        TRAP_IGNORE(iAvkonAppUi->ProcessCommandL(EGlxCmdResetView));
        }

    if (iAddToTag)
        {
        delete iAddToTag;
        }

    if (iAddToAlbum)
        {
        delete iAddToAlbum;
        }

    if (iUiUtility)
        {
        iUiUtility->Close();
        }

    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
    }

// ---------------------------------------------------------------------------
// SetDetailsDlgToolbarVisibility()
// ---------------------------------------------------------------------------
void CGlxMetadataDialog::SetDetailsDlgToolbarVisibility(TBool aVisible)
    {
    TRACER("CGlxMetadataDialog::SetDetailsDlgToolbarVisibility");
  
    HandleToolbarResetting(aVisible);
    CAknToolbar* currentPopupToolbar = iAvkonAppUi->CurrentPopupToolbar();
    if(currentPopupToolbar)
        {
        currentPopupToolbar->SetToolbarVisibility(aVisible);
        currentPopupToolbar->MakeVisible( aVisible ); 
        }
    }

// -----------------------------------------------------------------------------
// ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxMetadataDialog::ExecuteLD()
	{
	TRACER("CGlxMetadataDialog::ExecuteLD");	 
	return CAknDialog::ExecuteLD( R_METADATA_DIALOG );
	}
// ---------------------------------------------------------------------------
// MediaList.
// ---------------------------------------------------------------------------
//
 MGlxMediaList& CGlxMetadataDialog::MediaList()
    {
    TRACER("CGlxMetadataDialog::MediaList");    
    // return the refernce of media list
    return iContainer->MediaList();
    }
// -----------------------------------------------------------------------------
// ProcessCommandL
// -----------------------------------------------------------------------------
//
void CGlxMetadataDialog::ProcessCommandL( TInt aCommandId )
	{
	TRACER("CGlxMetadataDialog::ProcessCommandL");
	
	// hide menu bar
	iMenuBar->StopDisplayingMenuBar();

	switch( aCommandId )
		{
		case EAknSoftkeyEdit:
		case EAknSoftkeyCancel:
		case EAknSoftkeySelect:
		case EAknSoftkeyOk:
			{
			TryExitL( aCommandId );
			break;
			}

		case EAknCmdHelp:
			{
			TCoeHelpContext helpContext;
			helpContext.iMajor = TUid::Uid( KGlxGalleryApplicationUid );
			helpContext.iContext.Copy( LGAL_HLP_DETAILS_VIEW );
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
		case KGlxDeleteBoundMenuCommandId:
	    {
	    //Event passed on to container to handle	
      //Delete the location information of the data.	  
	    iContainer->RemoveLocationL();
	    break;
	    }	      
		case KGlxEditBoundMenuCommandId:
		case KGlxViewBoundMenuCommandId:
	    {
	    //To edit the details - forward the event to container to edit
  		//Both edit and view details command are handled in the same function based on the item.	
	    iContainer->HandleListboxChangesL();
	    break;
	    }
		case EGlxCmdAiwBase:
			{
			// pass aCommandId to command handler
			iMetadataCmdHandler->DoExecuteL( aCommandId, MediaList() );
			}
		default:
			break;
		}
	}
//-----------------------------------------------------------------------------
// CGlxMetadataDialog::CreateCustomControlL
//-----------------------------------------------------------------------------
SEikControlInfo CGlxMetadataDialog::CreateCustomControlL(TInt 
                                                                aControlType)
    {
    GLX_LOG_INFO("CShwSlideshowSettingsDialog::CreateCustomControlL");
    
    // create control info, no flags or trailer text set
    SEikControlInfo controlInfo;
    controlInfo.iControl        = NULL;
    controlInfo.iTrailerTextId  = 0;
    controlInfo.iFlags          = 0;
    if (aControlType == EMetaDataDialogListBox)
        {
        iContainer = CGlxMetadataContainer::NewL(iAvkonAppUi->ClientRect(),
		                                         *this, iUri, *this);        
        controlInfo.iControl = iContainer; // giving ownership   
        }
    return controlInfo; // returns ownership of ItemList
    }


// -----------------------------------------------------------------------------
// CGlxMetadataDialog::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CGlxMetadataDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
	{
	TRACER("CGlxMetadataDialog::OfferKeyEventL");
	TKeyResponse response = EKeyWasNotConsumed;
		switch(aKeyEvent.iCode)
			{
			case EKeyUpArrow:
			case EKeyDownArrow:
				{
				if(!iUiUtility->IsPenSupported())
				    {
                        iContainer->ChangeMskL();
				    }
				iContainer->EnableMarqueingL();
				break;
				}
			default:
				break;
			}
    if ( response == EKeyWasNotConsumed )
         {
         // container didn't consume the key so try the base class
         // this is crucial as platform uses a key event to dismiss dialog
         // when a view changes to another. the base class also consumes all
         // the keys we dont want to handle automatically as this is a 
         // blocking dialog
         response = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
         }
     return response;
	}

// -----------------------------------------------------------------------------
// CGlxMetadataDialog::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxMetadataDialog::DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane )
    {
    TRACER("CGlxMetadataDialog::DynInitMenuPaneL");
    iMetadataCmdHandler->PreDynInitMenuPaneL(aMenuId);
    iMetadataCmdHandler->DynInitMenuPaneL(aMenuId,aMenuPane);   
    //To enable/diable the options based on the item selected.
    iContainer->ViewDynInitMenuPaneL(aMenuId, aMenuPane);
    }

// -----------------------------------------------------------------------------
// OkToExitL
// -----------------------------------------------------------------------------
//
TBool CGlxMetadataDialog::OkToExitL( TInt aKeycode )
	{
	TRACER("CGlxMetadataDialog::OkToExitL");
	
	TBool retVal = EFalse;
	
	switch (aKeycode)
		{
		case EAknSoftkeySelect:
		case EAknSoftkeyOk:
			{
			break;
			}
		case EAknSoftkeyOptions: 
			{
			// display menu only
			// show the option menu that can have mutiselect options
			iMenuBar->SetMenuType( CEikMenuBar::EMenuOptions );
			retVal = CAknDialog::OkToExitL( aKeycode );
			break;
			}
		case EAknSoftkeyBack:
		case EAknSoftkeyCancel: 
			{
			retVal = ETrue;
			break;
			}

		case EAknSoftkeyEdit:
			{
			if(!iUiUtility->IsPenSupported())
				{
				iContainer->HandleListboxChangesL();
				}
			else
			    {
				HandleViewCommandL(aKeycode);
				}
			break;
			}            
		default :
			break;
		}
	return retVal;
	}

//-----------------------------------------------------------------------------
// CGlxMetadataDialog::SizeChanged
//-----------------------------------------------------------------------------
void CGlxMetadataDialog::SizeChanged()
	{
	TRACER("CGlxMetadataDialog::SizeChanged");
  CAknDialog::SizeChanged();
	}
// -----------------------------------------------------------------------------
// CGlxMetadataDialog::InitResourceL
// -----------------------------------------------------------------------------
//
void CGlxMetadataDialog::InitResourceL()    
	{
	TRACER("CGlxMetadataDialog::InitResourceL");
	
	_LIT(KGlxMetadataDialogResource,"glxmetadatadialog.rsc");
	//add resource file
	TParse parse;
	parse.Set(KGlxMetadataDialogResource, &KDC_APP_RESOURCE_DIR, NULL);
	TFileName resourceFile;
	resourceFile.Append(parse.FullName()); 
	CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);
	}  


// -----------------------------------------------------------------------------
// CGlxMetadataDialog::HandleViewCommandL
// -----------------------------------------------------------------------------
//    
TBool CGlxMetadataDialog::HandleViewCommandL( TInt /*aCommand*/ )
	{
	TRACER("CGlxMetadataDialog::HandleViewCommandL");
	return EFalse;
	}
// ---------------------------------------------------------------------------
// CGlxMetadataDialog::PreLayoutDynInitL
// ---------------------------------------------------------------------------
//
void CGlxMetadataDialog::PreLayoutDynInitL()
	{
	// No Implementation
	}
	
//-----------------------------------------------------------------------------
// CGlxMetadataDialog::PostLayoutDynInitL
//-----------------------------------------------------------------------------
//
void CGlxMetadataDialog::PostLayoutDynInitL()
	{
	TRACER("CGlxMetadataDialog::PostLayoutDynInitL");
	if(!iUiUtility->IsPenSupported())
		{
		iUiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
															EAknSoftkeyEdit,R_GLX_METADATA_MSK_EDIT);	
		}		
	}

//-----------------------------------------------------------------------------
// CGlxMetadataDialog::Draw
//-----------------------------------------------------------------------------
//
void CGlxMetadataDialog::Draw( const TRect& /*aRect*/ ) const
	{
	TRACER("CGlxMetadataDialog::Draw");
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, rect);

	// Get the standard graphics context
	CWindowGc& gc = SystemGc();
	gc.SetBrushColor(KRgbWhite);
	gc.DrawRect(rect);
	}

//-----------------------------------------------------------------------------
// CGlxMetadataDialog::HandlePointerEventL
//-----------------------------------------------------------------------------
//
void CGlxMetadataDialog::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
	{
	TRACER("CGlxMetadataDialog::HandlePointerEventL");
	CCoeControl::HandlePointerEventL( aPointerEvent );
	}

// ---------------------------------------------------------------------------
// CGlxMetadataDialog::OnLocationEditL
// ---------------------------------------------------------------------------
//
void  CGlxMetadataDialog::OnLocationEditL()   
	{
	TRACER("CGlxMetadataDialog::OnLocationEditL");
	OkToExitL( EAknSoftkeyOptions );
	}
	
// ---------------------------------------------------------------------------
// CGlxMetadataDialog::AddTag
// ---------------------------------------------------------------------------
//
void CGlxMetadataDialog::AddTagL()
{
    iAddToTag->ExecuteL(EGlxCmdAddTag);
}
// ---------------------------------------------------------------------------
// CGlxMetadataDialog::AddAlbum
// ---------------------------------------------------------------------------
//
void CGlxMetadataDialog::AddAlbumL()
{
    iAddToAlbum->ExecuteL(EGlxCmdAddToAlbum);
}

// ---------------------------------------------------------------------------
// CGlxMetadataDialog::SetTitleL()
// ---------------------------------------------------------------------------
void CGlxMetadataDialog::SetTitleL(const TDesC& aTitleText)
    {
    TRACER("CGlxFetcherContainer::SetTitleL");
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CleanupStack::PushL(statusPane);
    // get pointer to the default title pane control
    CAknTitlePane* titlePane = ( CAknTitlePane* )statusPane->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ));       
    CleanupStack::PushL(titlePane);
    // set the title if we got the title pane control
    if( titlePane )
        {
        GLX_LOG_INFO("GLX_UMP::CGlxMetadataDialog::ConstructL::INSIDE titlePane");
        iPreviousTitle = titlePane->Text()->AllocL();
        // Set the required Title
        titlePane->SetTextL( aTitleText );
        }
    CleanupStack::Pop(titlePane);
    CleanupStack::Pop(statusPane);
    }

// ---------------------------------------------------------------------------
// CGlxMetadataDialog::SetPreviousTitleL
// ---------------------------------------------------------------------------
void CGlxMetadataDialog::SetPreviousTitleL()
    {
    TRACER("CGlxFetcherContainer::SetPreviousTitleL");
    CEikStatusPane* prevStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CleanupStack::PushL(prevStatusPane);
    CAknTitlePane* prevTitlePane = ( CAknTitlePane* )prevStatusPane->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ));       
    CleanupStack::PushL(prevTitlePane);
    if( prevTitlePane )
        {
        // Restore the Title back of the Calling Application
        prevTitlePane->SetTextL( *iPreviousTitle );
        }
    CleanupStack::Pop(prevTitlePane);
    CleanupStack::Pop(prevStatusPane);
    }
// -----------------------------------------------------------------------------
// CGlxMetadataDialog::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CGlxMetadataDialog::HandleResourceChange( TInt aType )
    {
    TRACER("CGlxMetadataDialog::HandleResourceChange");
    //Handle global resource changes, such as scalable UI or skin events and orientation change (override)
    CAknDialog::HandleResourceChange( aType );
    }

// -----------------------------------------------------------------------------
// CGlxMetadataDialog::HandleToolbarResetting
// -----------------------------------------------------------------------------
//
void CGlxMetadataDialog::HandleToolbarResetting(TBool aVisible)
    {
    TRACER("CGlxMetadataDialog::HandleToolbarResetting");

    CAknToolbar* popupToolbar = iAvkonAppUi->PopupToolbar();
    if(popupToolbar)
        {
        popupToolbar->SetToolbarVisibility( !aVisible ); 
        popupToolbar->MakeVisible( !aVisible );
        if(!aVisible)
        	{
        	popupToolbar->DrawNow();    
        	}
        }   
    CAknToolbar* toolbar = iAvkonAppUi->CurrentFixedToolbar();
    if(toolbar)
        {
        toolbar->HideItemsAndDrawOnlyBackground(!aVisible);
        toolbar->SetToolbarVisibility(aVisible);
        toolbar->MakeVisible( aVisible );
        }
    }
// End of File

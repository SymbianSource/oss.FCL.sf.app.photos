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
* Description:    glximgvwrmetadatadialog.cpp
*
*/


#include "glximgvwrmetadatadialog.h"

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
#include <hlplch.h>                     // for HlpLauncher
#include <photos.hlp.hrh>
#include <glxgallery.hrh>
#include <glxdetailsboundcommand.hrh>
#include <glxscreenfurniture.h>
#include <glxuiutilities.rsg>
#include <glxpanic.h>                    // For Panics
#include "glxmetadatacommandhandler.h"

#define GetAppUi() (dynamic_cast<CAknAppUi*>(iEikonEnv->EikAppUi()))


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxImgVwrMetadataDialog* CGlxImgVwrMetadataDialog::NewL( const TDesC& aUri  )
    {
    TRACER("CGlxImgVwrMetadataDialog::NewL");

    CGlxImgVwrMetadataDialog* self = new(ELeave) CGlxImgVwrMetadataDialog(aUri );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CGlxImgVwrMetadataDialog::CGlxImgVwrMetadataDialog(const TDesC& aUri):iUri(aUri)
    {

    }
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::ConstructL()
    {
    TRACER("CGlxImgVwrMetadataDialog::ConstructL");

    // Load dialog's resource file
    InitResourceL();

    // make the toolbar disabled
    SetDetailsDlgToolbarVisibility(EFalse);  

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
        GLX_LOG_INFO1("GLX_UMP::CGlxImgVwrMetadataDialog::ConstructL::STATUS PANE = %d",statusPane->IsVisible());    
        // load the title text
        HBufC* text = StringLoader::LoadL(R_GLX_METADATA_VIEW_TITLE_DETAILS, iEikonEnv );
        SetTitleL( *text );
        if( text)
            {
            delete text;
            }
        GetAppUi()->StatusPane()->MakeVisible(ETrue);             
        }

    iUiUtility = CGlxUiUtility::UtilityL(); 

    // Call the base class' two-phased constructor
    CAknDialog::ConstructL( R_IMGVWR_METADATA_MENUBAR );

    } 

// -----------------------------------------------------------------------------
// ~CGlxImgVwrMetadataDialog
// -----------------------------------------------------------------------------
//
CGlxImgVwrMetadataDialog::~CGlxImgVwrMetadataDialog()
    {

    TRACER("CGlxImgVwrMetadataDialog::~CGlxImgVwrMetadataDialog");


    //To Disable the status pane if the dialog is launched from fullscreenview  
    if(!iStatusPaneAvailable)
        {
        GetAppUi()->StatusPane()->MakeVisible(EFalse);
        }       

    TRAP_IGNORE(SetPreviousTitleL());

    delete iPreviousTitle;

    // Restore the Toolbar as it was in the Calling application
    SetDetailsDlgToolbarVisibility(ETrue);

    // If details launched from FullScreen View, while moving back,
    // all the UI components should be hidden. Hence processing this command here.
    TRAP_IGNORE(GetAppUi()->ProcessCommandL(EGlxCmdResetView));

    if( iUiUtility )
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
void CGlxImgVwrMetadataDialog::SetDetailsDlgToolbarVisibility(TBool aVisible)
    {
    TRACER("CGlxImgVwrMetadataDialog::SetDetailsDlgToolbarVisibility");
    CAknAppUi* appUi = GetAppUi();
    __ASSERT_DEBUG(appUi, Panic(EGlxPanicNullPointer));

    HandleToolbarResetting(aVisible);
    CAknToolbar* currentPopupToolbar = appUi->CurrentPopupToolbar();
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
EXPORT_C TInt CGlxImgVwrMetadataDialog::ExecuteLD()
    {
    TRACER("CGlxImgVwrMetadataDialog::ExecuteLD");     
    return CAknDialog::ExecuteLD( R_IMG_VIEWER_METADATA_DIALOG );
    }
// -----------------------------------------------------------------------------
// ProcessCommandL
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::ProcessCommandL( TInt aCommandId )
    {

    TRACER("CGlxImgVwrMetadataDialog::ProcessCommandL");
    // hide menu bar
    iMenuBar->StopDisplayingMenuBar();
    switch( aCommandId )
        {
        /// @todo need to put the proper help file.for the time being it will not do any thing.
        /*
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
            }*/
        
        default:
            break;
        }
    
    }
//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::CreateCustomControlL
//-----------------------------------------------------------------------------
SEikControlInfo CGlxImgVwrMetadataDialog::CreateCustomControlL(TInt 
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
        iContainer = CGlxImgVwrMetadataContainer::NewL(GetAppUi()->ClientRect(),iUri);        
        controlInfo.iControl = iContainer; // giving ownership   
        }
    return controlInfo; // returns ownership of ItemList
    }

// -----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane )
    {
		//no implementation
    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::SizeChanged
//-----------------------------------------------------------------------------
void CGlxImgVwrMetadataDialog::SizeChanged()
    {
    TRACER("CGlxImgVwrMetadataDialog::SizeChanged");
    CAknDialog::SizeChanged();
    }
// -----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::InitResourceL
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::InitResourceL()    
    {
    TRACER("CGlxImgVwrMetadataDialog::InitResourceL");

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
// CGlxImgVwrMetadataDialog::HandleViewCommandL
// -----------------------------------------------------------------------------
//    
TBool CGlxImgVwrMetadataDialog::HandleViewCommandL( TInt /*aCommand*/ )
    {
    TRACER("CGlxImgVwrMetadataDialog::HandleViewCommandL");
    return EFalse;
    }
// ---------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::PreLayoutDynInitL
// ---------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::PreLayoutDynInitL()
    {
    // No Implementation
    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::PostLayoutDynInitL
//-----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::PostLayoutDynInitL()
    {}

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::Draw
//-----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::Draw( const TRect& /*aRect*/ ) const
{
TRACER("CGlxImgVwrMetadataDialog::Draw");
TRect rect;
AknLayoutUtils::LayoutMetricsRect (AknLayoutUtils::EMainPane, rect);

// Get the standard graphics context
CWindowGc& gc = SystemGc();
gc.SetBrushColor(KRgbWhite);
gc.DrawRect(rect);
}

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::HandlePointerEventL
//-----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {
    TRACER("CGlxImgVwrMetadataDialog::HandlePointerEventL");
    CCoeControl::HandlePointerEventL( aPointerEvent );
    }

// ---------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::SetTitleL()
// ---------------------------------------------------------------------------
void CGlxImgVwrMetadataDialog::SetTitleL(const TDesC& aTitleText)
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
        GLX_LOG_INFO("GLX_UMP::CGlxImgVwrMetadataDialog::ConstructL::INSIDE titlePane");
        iPreviousTitle = titlePane->Text()->AllocL();
        // Set the required Title
        titlePane->SetTextL( aTitleText );
        }
    CleanupStack::Pop(titlePane);
    CleanupStack::Pop(statusPane);
    }

// ---------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::SetPreviousTitleL
// ---------------------------------------------------------------------------
void CGlxImgVwrMetadataDialog::SetPreviousTitleL()
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
// CGlxImgVwrMetadataDialog::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::HandleResourceChange( TInt aType )
    {
    TRACER("CGlxImgVwrMetadataDialog::HandleResourceChange");
    //Handle global resource changes, such as scalable UI or skin events and orientation change (override)
    CAknDialog::HandleResourceChange( aType );
    }
// -----------------------------------------------------------------------------
// CGlxImgVwrMetadataDialog::HandleToolbarResetting
// -----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataDialog::HandleToolbarResetting(TBool aVisible)
    {
    TRACER("CGlxImgVwrMetadataDialog::HandleToolbarResetting");
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
        toolbar->HideItemsAndDrawOnlyBackground(!aVisible);
        toolbar->SetToolbarVisibility(aVisible);
        toolbar->MakeVisible( aVisible );
        }
    }

// End of File


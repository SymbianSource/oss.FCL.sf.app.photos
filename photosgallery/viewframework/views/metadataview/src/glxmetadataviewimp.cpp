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
* Description:    Implementation of Metadataview
*
*/


// INCLUDE FILES

#include 	"glxmetadataviewimp.h"

#include 	<aknviewappui.h>
#include 	<data_caging_path_literals.hrh>
#include 	<avkon.rsg>

#include 	<alf/ialfviewwidget.h>
#include 	<alf/alfdisplay.h>
#include    <alf/alfwidgetenvextension.h>
#include    <alf/alfwidgetcontrol.h>

#include 	<glxmedialist.h>
#include	<glxtracer.h>
#include	<glxlog.h>
#include 	<glxresourceutilities.h>       // for CGlxResourceUtilities
#include	<glxsetappstate.h>             // set app state in PCFW key

#include 	<glxfilterfactory.h>               // for TGlxFilterFactory
#include	<glxdetailsmulmodelprovider.h>	//Details data provider
#include    "glxmetadatabindingsetfactory.h" //for Binding list entries 
#include    <glxdetailsboundcommand.hrh>

#include 	"glxmetadataview.h"
#include 	<glxmetadataview.rsg>

#include <glxuiutilities.rsg>

#include <glxcommandhandleraiwshowmap.h>

//CONSTANTS
const TInt  KViewId = 0x200071B0;
const TInt  KMainMediaListId = 0x2000D248;
const TInt  KDetialsDataWindowSize = 5;
_LIT( KGlxStrETrue, "\x0001");

// ============================== MEMBER FUNCTIONS ===========================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMetadataViewImp* CGlxMetadataViewImp::NewL
									( MGlxMediaListFactory* aMediaListFactory )
    {
	TRACER("CGlxMetadataViewImp::NewL");
    
    CGlxMetadataViewImp* self = CGlxMetadataViewImp::NewLC( aMediaListFactory );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxMetadataViewImp* CGlxMetadataViewImp::NewLC
									( MGlxMediaListFactory* aMediaListFactory )
    {
    TRACER("CGlxMetadataViewImp::NewLC");

    CGlxMetadataViewImp* self = new (ELeave) CGlxMetadataViewImp();
    CleanupStack::PushL(self);
    self->ConstructL( aMediaListFactory );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxMetadataViewImp::CGlxMetadataViewImp()
    {
    //No implementation
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxMetadataViewImp::~CGlxMetadataViewImp()
    {
	TRACER("CGlxMetadataViewImp::~CGlxMetadataViewImp");
	
	
    if ( iResourceOffset )
       {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
       }    
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//    
void CGlxMetadataViewImp::ConstructL( MGlxMediaListFactory* /*aMediaListFactory*/ )
    {
    TRACER("CGlxMetadataViewImp::ConstructL");
    
	_LIT(KGlxMetadataViewResource,"glxmetadataview.rsc");

	//add resource file
    TParse parse;
    parse.Set(KGlxMetadataViewResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName()); 
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);
	
	//call base class's consructor
    BaseConstructL(R_GLX_METADATA_VIEW);
    ViewBaseConstructL();
    
   	HBufC* title = iEikonEnv->AllocReadResourceLC(R_GLX_METADATA_VIEW_TITLE_DETAILS);
	MLViewBaseConstructL(NULL,*title);   	 	
	CleanupStack::PopAndDestroy(title);
	
    // ShowMap commandhandler                         
    GLX_LOG_INFO("Adding CGlxCommandHandlerAiwShowMap");
    this->AddCommandHandlerL(CGlxCommandHandlerAiwShowMap::
                                NewL(this, R_METADATA_MENU));
	
	// Creating the Alf Environment
    ConstructAlfEnvL();
	}
	
// ---------------------------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::ViewDynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane *aMenuPane)
    {
	TRACER("CGlxMetadataViewImp::DynInitMenuPaneL");
    
    iDetailsMulModelProvider->InitMenuL(*aMenuPane);
    }
    
// ---------------------------------------------------------------------------
// HandleViewCommandL
// ---------------------------------------------------------------------------
//
TBool CGlxMetadataViewImp::HandleViewCommandL(TInt aCommand)
    {
	TRACER("CGlxMetadataViewImp::HandleViewCommandL");
        
    if( aCommand )
    	{
    	return iDetailsMulModelProvider->OfferCommandL( aCommand );
    	}
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From CGlxViewBase
// return menuresource id
// ---------------------------------------------------------------------------
//    
TInt CGlxMetadataViewImp::MenuResourceId()
	{
	return R_METADATA_MENU;
	}
	
// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CGlxMetadataViewImp::Id() const
    {
    return TUid::Uid(KViewId);
    }
    
// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::DoMLViewActivateL( const TVwsViewId& /* aPrevViewId */, 
    TUid /* aCustomMessageId */, const TDesC8&  aCustomMessage )
	{
	TRACER("CGlxMetadataViewImp::DoMLViewActivateL");
	CreateMediaListL( aCustomMessage );
	
	StatusPane()->MakeVisible(ETrue);
	
	// Construct the application's default view i.e., listview widget
	ConstructInitialViewL();
	GlxSetAppState::SetState( EGlxInListView );
	}
	
// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::DoMLViewDeactivate()
    {
	TRACER("CGlxMetadataViewImp::DoMLViewDeactivate");
	
	
		
    DestroyListViewWidget();
	if( iItemMediaList ) 
		{
		iItemMediaList->Close();
		iItemMediaList = NULL;	
		} 
	if( iItemURI )
		{
		delete iItemURI;
		iItemURI = NULL;
		}
   }
   
// ---------------------------------------------------------------------------
// ConstructAlfEnvL
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::ConstructAlfEnvL()
    {
	TRACER("CGlxMetadataViewImp::ConstructAlfEnvL");
    
	
	// Creating the Alf Environment
	// Should be the first thing to be done before widgets can be created
	iEnv = iUiUtility->Env();

	// Creating the Alf display    
	// display is need to show anything related to Alf
	// Avkon perceives the Alf display as one CoeControl, 
	// it can't see widgets inside the display
	iDisp = iUiUtility->Display();
	
	// Use the Avkon skin background as the display background.
	iDisp->SetClearBackgroundL (CAlfDisplay::EClearWithSkinBackground);
   }

// ---------------------------------------------------------------------------
// ConstructInitialViewL
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::ConstructInitialViewL()
    {
    TRACER("CGlxMetadataViewImp::ConstructInitialViewL");
    
    const char* KLoadName("mullistwidget");
    const char* KWidgetLoadName("ListWidget");
    
    iControlGroupId = reinterpret_cast < TInt > (this);
    
    try
        {
        // Get widget factory from CAlfEnv
        // Factory is then used to create the individual widgets & data model

        IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv); 

        
        iViewWidget = widgetFactory.createViewWidget("detaillistviewwidget", iControlGroupId); 
        
        //Enable status pane
        iViewWidget->enableStatusPane(true);
        iViewWidget->setRect( ClientRect() );
        iViewWidget->show(true);
        
        // Create List widget. CAlfEnv owns widget
        iListWidget = widgetFactory.createWidget<IMulListWidget>( KLoadName, 
                                        KWidgetLoadName, *iViewWidget, NULL);
        
        // Disable marking for list, need to remove this flag when widget  
        // construction has no marking flag set by default
        iListWidget->ClearFlags( IMulMultiItemWidget::EMulWidgetMarkingMode ); 
        
        //Sets the default text for the List View when empty
        CreateEmptyText();
    
        if ( iListWidget )
            {
            // Widget takes the ownership
            iListWidget->AddEventHandler (*this);
            iListWidget->ShowWidget(true);
            iListWidget->control()->AcquireFocus();
            } 
        
        AddListEntryL();
               
    	}
    catch(...)
        {
        User::Leave(KErrGeneral);
        }
    }

// ---------------------------------------------------------------------------
// Destroys the Widget
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::DestroyListViewWidget()
    {
    if( iDetailsMulModelProvider )
        {
        delete iDetailsMulModelProvider;
        iDetailsMulModelProvider = NULL;
        }
      if( iListWidget )
        {
        iListWidget->RemoveEventHandler (*this);
        iListWidget->ShowWidget(false);
        iViewWidget->show(false);
        }

    IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv); 
    widgetFactory.destroyWidget(iViewWidget);
      
    iListWidget = NULL;  
    iViewWidget = NULL;
    }
	
// ---------------------------------------------------------------------------
// From AddListEntryL
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::AddListEntryL()
	{
	TRACER("CGlxMetadataViewImp::AddListEntryL");
	
	iDetailsMulModelProvider = CGlxDetailsMulModelProvider::NewL( *iEnv,
										*iListWidget,
										*iItemMediaList, 
										mulwidget::KTemplate5, KDetialsDataWindowSize );
										
	iDetailsMulModelProvider->AddEntryL(TGlxMulTitleDetailsBindingSetFactory() );
	iDetailsMulModelProvider->AddEntryL(TGlxMulDateAndTimeDetailsBindingSetFactory() );
	iDetailsMulModelProvider->AddEntryL(TGlxMulDescriptionDetailsBindingSetFactory() );
	iDetailsMulModelProvider->AddEntryL(TGlxMulTagsDetailsBindingSetFactory() ); 
	iDetailsMulModelProvider->AddEntryL(TGlxMulAlbumsDetailsBindingSetFactory() ); 
	iDetailsMulModelProvider->AddEntryL(TGlxMulLocationDetailsBindingSetFactory() ); 
	iDetailsMulModelProvider->AddEntryL(TGlxMulFileSizeDetailsBindingSetFactory() );
	iDetailsMulModelProvider->AddEntryL(TGlxMulResolutionDetailsBindingSetFactory() );
	if( iIsVideo)
	    {
	    iDetailsMulModelProvider->AddEntryL(TGlxMulDurationDetailsBindingSetFactory() );
	    }
	if( iIsDrm )
		{
		iDetailsMulModelProvider->AddEntryL(TGlxMulUsageRightsDetailsBindingSetFactory() );
		}
	}
	
// ---------------------------------------------------------
// CreateEmptyText
// ---------------------------------------------------------
//	
void CGlxMetadataViewImp::CreateEmptyText()
	{
	TRACER("CGlxMetadataViewImp::CreateEmptyText");
	
	// The listwidget can set an empty text on the display
	// If there are no data items in the associated data model,
	// then the empty text is shown on the widget
	// If the empty text is not set, then widget won't shown any text by default
	UString* defaultText = new UString("No Items Present");
	iListWidget->SetEmptyText (*defaultText); // Widget takes the ownership
	delete defaultText;
	}
	
// ---------------------------------------------------------------------------
// CreateMediaListL
// ---------------------------------------------------------------------------
//
void CGlxMetadataViewImp::CreateMediaListL( const TDesC8& aURI )
	{
	TRACER("CGlxMetadataViewImp::CreateMediaListL");
	
	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
	CleanupStack::PushL( path );
	
	path->AppendL(KGlxCollectionPluginAllImplementationUid);
	
	//convert uri from 8 bit to unicode
	iItemURI = HBufC::NewL( aURI.Length() );
	iItemURI->Des().Copy( aURI );
	
	//Get last two characters for checking drm or video and reset the length for uri
    TPtr uriPtr (iItemURI->Des());
    
    TPtr isVideo = uriPtr.MidTPtr(uriPtr.Length()-1,1);
    if( !isVideo.Compare(KGlxStrETrue ))
	    {
	    iIsVideo = ETrue;	
	    }
	    
    TPtr isDrm = uriPtr.MidTPtr(uriPtr.Length()-2,1);
    if( !isDrm.Compare(KGlxStrETrue))
	    {
	    iIsDrm = ETrue;	
	    }
    
    uriPtr.SetLength(uriPtr.Length()-2);

	CMPXFilter* filter  = TGlxFilterFactory::CreateURIFilterL( *iItemURI );  
	CleanupStack::PushL( filter );	
	
	//@TODO remove magic no - allocate uid from uid list
	iItemMediaList = MGlxMediaList::InstanceL(*path, TGlxHierarchyId(KMainMediaListId), filter);
    
	CleanupStack::PopAndDestroy( filter );	
    CleanupStack::PopAndDestroy( path ); 
	}
	
// ---------------------------------------------------------------------------
// MediaList.
// ---------------------------------------------------------------------------
//
MGlxMediaList& CGlxMetadataViewImp::MediaList()
	{
	return *iItemMediaList;
	}
	
// ---------------------------------------------------------
// offerEvent
// ---------------------------------------------------------
//	
AlfEventStatus CGlxMetadataViewImp::offerEvent( CAlfWidgetControl& /*aControl*/, const TAlfEvent& aEvent )
	{
	    
	//Check if its a Custom Event else return
	if(!aEvent.IsCustomEvent())
		{   
		GLX_LOG_INFO("Event Not Handled");     		
		return EEventNotHandled;
		}
    TInt eventId = aEvent.CustomParameter();
    switch(eventId)
        {
        case KAlfActionIdDeviceLayoutChanged:
          {
          iViewWidget->setRect( ClientRect() ); 
          return EEventNotHandled;
          }
        case ETypeSelect:
            {
            TRAP_IGNORE(HandleViewCommandL((TInt)KGlxEditBoundMenuCommandId));
            }
        }
 
	return EEventConsumed;	
	}

//----------------------------------------------------------------------------------
// eventHandlerType
//----------------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerType CGlxMetadataViewImp::eventHandlerType() 
	{
	return IAlfWidgetEventHandler::EPresentationEventHandler;
	}

//----------------------------------------------------------------------------------
// eventExecutionPhase
//----------------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase CGlxMetadataViewImp::eventExecutionPhase()
	{
		return EBubblingPhaseEventHandler;
	}  
// ---------------------------------------------------------
// accept
// ---------------------------------------------------------
//	

bool CGlxMetadataViewImp::accept( CAlfWidgetControl& /*aControl*/, const TAlfEvent& /*aEvent*/ ) const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// setActiveStates
// ---------------------------------------------------------
//	
    
 void CGlxMetadataViewImp::setActiveStates( unsigned int /*aStates*/ )
    {
    
    }

// ---------------------------------------------------------
// makeInterface
// ---------------------------------------------------------
//	

IAlfInterfaceBase* CGlxMetadataViewImp::makeInterface( const IfId& /*aType*/ )
    {
    return NULL; 
    }

// ---------------------------------------------------------
// setEventHandlerData
// ---------------------------------------------------------
//	
    
void CGlxMetadataViewImp::setEventHandlerData(const AlfWidgetEventHandlerInitData& /*aData*/ )
    {
    
    }

// ---------------------------------------------------------
// eventHandlerData
// ---------------------------------------------------------
//	
    
AlfWidgetEventHandlerInitData* CGlxMetadataViewImp::eventHandlerData()	  
   {
   return NULL;
   }
   
//  End of File

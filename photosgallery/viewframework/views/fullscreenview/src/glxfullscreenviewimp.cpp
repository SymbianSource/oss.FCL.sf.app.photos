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
* Description:    Implementation of fULL-SCREEN view
*
*/




// Avkon and System Headers 
#include <eikbtgpc.h>                           // CEikButtonGroupContainer
#include <aknViewAppUi.h>
#include <data_caging_path_literals.hrh>        // for KDC_APP_RESOURCE_DIR
#include <glxcollectionpluginimageviewer.hrh>
#include <StringLoader.h>                       // String Loader

// Alf Headers
#include <alf/ialfwidgetfactory.h>              // widget factory
#include <alf/alfwidgetcontrol.h>               // To get CAlfControl from CAlfWidgetControl
#include <alf/ialfviewwidget.h>                 // View Widget
#include <alf/alfwidgetenvextension.h>          // Widget Env Extensions to create view widget
#include <alf/alfutil.h>                        // AlfUtil

// Antariksh Headers
#include <mul/mulevent.h>
#include <mul/imulcoverflowwidget.h>            // Coverflow widget
#include <mul/imulsliderwidget.h>               // For Slider Widget
#include <mpxcollectionpath.h>

#include <gesturehelper.h>

using namespace GestureHelper;

//Hg 
//#include <hg/hgcontextutility.h>
#include <glxhdmicontroller.h>

// Photos Headers
#include <glxlog.h>                             // CGlxLogger
#include <glxtracer.h>                          // For Log
#include <glxmedialistmulmodelprovider.h>       // For MulModelProvider
#include <glxcommandhandlers.hrh>               // For EGlxCmdFullScreenBack               
#include <glxfullscreenviewdata.rsg>            // FS View Resource
#include <glxthumbnailattributeinfo.h>          // KGlxMediaIdThumbnail
//#include "glxmediakeyutility.h"
#include <glxactivemedialistregistry.h>         // For medialist registry
#include <glxerrormanager.h>
#include <glxzoomcontrol.h>
#include <glxuiutilities.rsg>
#include <glxgeneraluiutilities.h>
#include <glxtexturemanager.h>
#include <glxicons.mbg>
#include <glxsetappstate.h>
#include <glxdrmutility.h>
#include <glxscreenfurniture.h>
//#include <glxsinglelinemetapanecontrol.h>
#include "glxfullscreenviewimp.h"
#include <glxnavigationalstate.h>

using namespace Alf;

//To disable AppUi status pane
#define GetAppViewUi() (dynamic_cast<CAknViewAppUi*>(CEikonEnv::Static()->EikAppUi()))
const TInt KGlxScreenTimeout =10000000;
const TInt KCoverflowDataWindowSize = 1;
const TInt KGlxMaxSmallImageZoomLevel =150;
const TInt KGlxMaxBigImageZoomLevel   =100;
const TInt KGlxMinSmallImageZoomLevel =100;
const TInt KGlxNeutralPinchPosition   =100;
const TInt KGlxSliderTickValue        =5;
/**
 * Start Delay for the periodic timer, in microseconds
 */
const TInt KPeriodicStartDelay = 250000; 
//This constant is used to calculate the index of the item for which texture has to removed.
//6 = 5(iterator value in forward or backward direction for fullscreen) + 1(focus index);
const TInt KFullScreenIterator = 3; 
//Constant which says maximum number of fullscreen textures that we have have at a time.
//11 = (5(5 fullscreen texture backwards)+1(fucus index texture)+5(5 fullscreen texture forwards))
const TInt KFullScreenTextureOffset = 5;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxFullScreenViewImp::CGlxFullScreenViewImp(
		const TFullScreenViewResourceIds& aResourceIds, TInt aViewUID) :
	iResourceIds(aResourceIds), iViewUid(aViewUID)
	{
	TRACER("CGlxFullScreenViewImp::CGlxFullScreenViewImp");
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxFullScreenViewImp* CGlxFullScreenViewImp::NewL(
		MGlxMediaListFactory* aMediaListFactory,
		const TFullScreenViewResourceIds& aResourceIds, 
		TInt aViewUID,
		const TDesC& aTitle)
	{
	TRACER("CGlxFullScreenViewImp::NewL");
	CGlxFullScreenViewImp* self = CGlxFullScreenViewImp::NewLC (
			aMediaListFactory, aResourceIds, aViewUID, aTitle);
	CleanupStack::Pop (self);
	return self;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxFullScreenViewImp* CGlxFullScreenViewImp::NewLC(
		MGlxMediaListFactory* aMediaListFactory,
		const TFullScreenViewResourceIds& aResourceIds, TInt aViewUID,
		const TDesC& aTitle)
	{
	TRACER("CGlxFullScreenViewImp::NewLC");
	CGlxFullScreenViewImp* self =new (ELeave) CGlxFullScreenViewImp(aResourceIds, aViewUID);
	CleanupStack::PushL (self);
	self->ConstructL (aMediaListFactory, aTitle);
	return self;
	}

// ---------------------------------------------------------------------------
//  ConstructL.
// ---------------------------------------------------------------------------
//
void  CGlxFullScreenViewImp::ConstructL(
		MGlxMediaListFactory* aMediaListFactory, const TDesC& aTitle)
	{
	TRACER("CGlxFullScreenViewImp::ConstructL");
	BaseConstructL (iResourceIds.iViewId);
	ViewBaseConstructL();
	MLViewBaseConstructL (aMediaListFactory, aTitle , EFalse);
	
	iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
	//Register the view to recieve toolbar events. ViewBase handles the events
	SetToolbarObserver(this);
	
	// Presently image viewer dont have toolbar
	// So need to remove if image viewer using full screen
	CGlxNavigationalState* navigationalState =  CGlxNavigationalState::InstanceL();
	CleanupClosePushL( *navigationalState );
	CMPXCollectionPath* naviState = navigationalState->StateLC();
	if (naviState->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
	    {
	    CAknToolbar* toolbar = Toolbar();
	    if(toolbar)
	        {
	        toolbar->DisableToolbarL(ETrue);
	        }
	    ShowToolbarOnViewActivation(EFalse);
	    }
	else
	    {
	    ShowToolbarOnViewActivation(ETrue);
	    }
	CleanupStack::PopAndDestroy(naviState);
	CleanupStack::PopAndDestroy(navigationalState);
	
	
	//Get the ScreenFurniture instance
	iScreenFurniture = iUiUtility->ScreenFurniture();
	
    //Get the env from the uiutilities
	iEnv = iUiUtility->Env ();
	iZoomButtonGroup = CEikButtonGroupContainer::NewL(
	        CEikButtonGroupContainer::ECba,  // type
	        CEikButtonGroupContainer::EHorizontal,  // orientation
	        this,  // command observer
	        R_GLX_FULLSCREEN_SOFTKEYS_ZOOM ); 
	iZoomButtonGroup->MakeVisible( EFalse );

	//create the drmutility instance
	iDrmUtility = CGlxDRMUtility::InstanceL();
   // Get object that stores the active media list registry
    iActiveMediaListRegistry = CGlxActiveMediaListRegistry::InstanceL();
    if(!iPeriodic)
		{
		iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
		}
    }

// ---------------------------------------------------------------------------
// ~CGlxFullScreenViewImp()
// ---------------------------------------------------------------------------
//
CGlxFullScreenViewImp::~CGlxFullScreenViewImp()
    {
    TRACER("CGlxFullScreenViewImp::~CGlxFullScreenViewImp");
    delete iMediaListMulModelProvider;   
    if(iTimer->IsActive())
	    {
	    iTimer->Cancel();
	    }
	delete iTimer;   
	if(iZoomButtonGroup)
        {
        AppUi()->RemoveFromStack( iZoomButtonGroup );
        delete iZoomButtonGroup;
        }
    if(iActiveMediaListRegistry)
        {
        iActiveMediaListRegistry->Close();
	    }
    if( iDrmUtility )
		{
		iDrmUtility->Close();
		}
    if (iPeriodic)
 		{
 		iPeriodic->Cancel();
 		delete iPeriodic;
 		}
	}
 // ---------------------------------------------------------------------------
// From CGlxViewBase
// ---------------------------------------------------------------------------
//
TUid CGlxFullScreenViewImp::Id() const
    {
    TRACER("CGlxFullScreenViewImp::Id");
    return TUid::Uid(iViewUid);
	}


// ---------------------------------------------------------------------------
// Destroys a coverflow widget
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DestroyWidgets()
    {
    TRACER("CGlxFullScreenViewImp::DestroyCoverflowWidget");
    if (iCoverFlowWidget)
        {
        iCoverFlowWidget->RemoveEventHandler(*this);
        }
    // Destror Slider widget
    DestroySliderWidget();
    
    delete iMediaListMulModelProvider;
    iMediaListMulModelProvider = NULL;

    IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv); 
    // Destroying this widget calls destructor of single line meta pane
    widgetFactory.destroyWidget(iViewWidget);

    iCoverFlowWidget = NULL;
    iViewWidget = NULL;
    }

// ---------------------------------------------------------------------------
// Destroys a Slider widget
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DestroySliderWidget()
    {
    TRACER("CGlxFullScreenViewImp::DestroySliderWidget");
    if(iSliderWidget)
        {
        iSliderWidget->RemoveEventHandler(*this);
        }
    iSliderWidget = NULL;
    iSliderModel = NULL;
    }
// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DoMLViewActivateL(
		const TVwsViewId & /* aPrevViewId */, TUid /* aCustomMessageId */,
		const TDesC8 & /* aCustomMessage */)
	{
    TRACER("CGlxFullScreenViewImp::DoMLViewActivateL");
    // hide the toolbar
    CAknToolbar* toolbar = Toolbar();
    if(toolbar)
        {
        toolbar->SetToolbarVisibility(EFalse); 
        }
    	//Fix For EPKA-7U5DT7-slideshow launched from FullScreen and connect USB in mass storage mode results in Photos crash
	if(!iMediaList->Count())
		{
	  	CGlxNavigationalState* navigationalState =  CGlxNavigationalState::InstanceL();
	  	CleanupClosePushL( *navigationalState );
	  	navigationalState->ActivatePreviousViewL();
	  	CleanupStack::PopAndDestroy( navigationalState );
		}  
	//need to create the instance first,other wise panics while
	// zooming from imgvwr.
	iHdmiController = CGlxHdmiController::NewL();
	if(iMediaList->Count())
	    {
	    iOldFocusIndex = iMediaList->FocusIndex(); 
        TGlxMedia item = iMediaList->Item( iMediaList->FocusIndex() );
        GLX_LOG_INFO("CGlxHdmi - CGlxFullScreenViewImp Create HdmiController");
        if (item.Category() != EMPXVideo)
            {
            TInt frameCount(0);
            TSize orignalSize;
            TBool aFramesPresent  = item.GetFrameCount(frameCount);
            TBool adimension  = item.GetDimensions(orignalSize);
            iHdmiController->SetImageL(item.Uri(), orignalSize, frameCount);
            }    
	    }
    iScreenFurniture->SetActiveView(iViewUid);

    // create the screen furniture for touch devices
    CreateScreenFurnitureL();
    
    //set the ui state to off,when the Fullscreen launches
    SetUiSate(NGlxNFullScreenUIState::EUiOff);
   
   	GlxSetAppState::SetState(EGlxInFullScreenView);
   	 
    // create the coverflow
    CreateCoverflowWidgetL();
    
    
    CreateSliderWidgetL();
    
//    iMediaKeyUtility = CGlxMediaKeyUtility::NewL();
    
    // create a zoom control
    iZoomControl = CGlxZoomControl::NewL(*this, *iMediaList,
            *iZoomButtonGroup, *iSliderWidget, iGestureHelper);

    // Create the Metapane
    //iSingleLineMetaPane = CGlxSinleLineMetaPane::NewL(*this,*iMediaList,*iUiUtility);

    // hide the screen furniture
    HideUi(ETrue);
	// Set active media list pointer, so that UPnP renderer knows about this list
    iActiveMediaListRegistry->RegisterActiveMediaList(iMediaList);
    
    //Get the HgContextUtility instance
//    iContextUtility = iUiUtility->ContextUtility();

    // if the Medialist has any item, set the First index context to Hg Context Utility
//    TGlxMedia item = iMediaList->Item( iMediaList->FocusIndex() );
//    iContextUtility->PublishPhotoContextL(item.Uri());
    	if ( !iPeriodic->IsActive() )
		{
		iPeriodic->Start( KPeriodicStartDelay, KMaxTInt, TCallBack( 
		        &PeriodicCallback, static_cast<TAny*>( this ) ) );
		}
	}
	

// ---------------------------------------------------------------------------
// Creates a coverflow widget
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::CreateCoverflowWidgetL()
    {
    TRACER("CGlxFullScreenViewImp::CreateCoverflowWidgetL");

    const char* KWidgetName("mulcoverflowwidget");
    const char* KCoverflowViewWidget("CoverflowViewWidget");
    const char* KCoverflowWidget("CoverflowWidget");
    try
        {
        // Get widget factory from CAlfEnv
        // Factory is then used to create the individual widgets & data model

        IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv); 

    // create a view widget  
    iViewWidget = widgetFactory.createViewWidget (
            KCoverflowViewWidget, 0x113);
    // hide control and status pane
    iViewWidget->enableControlPane(EFalse);
    iViewWidget->enableStatusPane(EFalse);
    iViewWidget->setRect(TRect(TPoint(0,0),AlfUtil::ScreenSize()));
    iViewWidget->show(true);
    // create coverflow widget
    iCoverFlowWidget = widgetFactory.createWidget<IMulCoverFlowWidget> (
            KWidgetName, KCoverflowWidget, *iViewWidget, NULL);			
    iCoverFlowWidget->SetFlags(IMulMultiItemWidget::EMulWidgetDoubleTap);

    // Widget takes the ownership
    iCoverFlowWidget->AddEventHandler (*this);

    // hide appui's status pane  
    GetAppViewUi()->StatusPane()->MakeVisible(EFalse);

    // hide appui's softkeys
    GetAppViewUi()->Cba()->MakeVisible(EFalse);
    iCoverFlowWidget->ShowWidget(ETrue);
    iCoverFlowWidget->control()->AcquireFocus();

        iMediaListMulModelProvider = CGlxMediaListMulModelProvider::NewL (*iEnv,
                *iCoverFlowWidget, *iMediaList, iFullScreenBindingSet,
                mulwidget::KTemplate4, KCoverflowDataWindowSize );
        
        iGestureHelper = (GestureHelper::CGestureHelper*)iCoverFlowWidget->Gesturehelper();
        }

    catch(...)
        {
        User::Leave(KErrGeneral);
        }

    }

//----------------------------------------------------------------------------
//Initialise the slider widget and the slider model
//----------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::CreateSliderWidgetL()
    {
    const char* KSliderWidgetName("mulsliderwidget");
    const char* KZoomSliderWidget("ZoomSliderWidget");

    try
        {
        IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv);
    
        iSliderWidget = widgetFactory.createWidget<IMulSliderWidget>( KSliderWidgetName, 
                KZoomSliderWidget, *iViewWidget, NULL);
    
        iSliderWidget->AddEventHandler(*this);
        iSliderWidget->SetHandleKeyEvent(EFalse);
        iSliderModel = widgetFactory.createModel<IMulSliderModel> ("mulslidermodel");
        iSliderModel->SetTemplate(ESliderTemplate3);
        iSliderWidget->setModel(iSliderModel);
    
        //adding the range and slider tick value 
        if(iSliderModel)
            {
            iSliderModel->SetTick(KGlxSliderTickValue);
            }
        
        //SetSliderLevel();
        
        }
    catch(...)
        {
        User::Leave(KErrGeneral);
        }
    }

// ---------------------------------------------------------------------------
// ShowUiL
// ---------------------------------------------------------------------------
//	
void  CGlxFullScreenViewImp::ShowUiL(TBool aStartTimer)
    {
    TRACER("CGlxFullScreenViewImp::ShowUiL");

    TInt index = iMediaList->FocusIndex();
    const TGlxMedia& item = iMediaList->Item(index);
    TInt error = GlxErrorManager::HasAttributeErrorL(item.Properties(), KGlxMediaIdThumbnail);
    // Display slider only for non corrupted images
    if (error == KErrNone && item.Category() == EMPXImage)
        {
        //To set the Slider values.
        SetSliderLevel();
        //show the slider
        iSliderWidget->ShowWidget(ETrue);
        }
    
    if (iCoverFlowWidget)
        {
		// This is done to set the item counter
        iCoverFlowWidget->SetUIMode(ETrue);
        }

    //show the toolbar
    CAknToolbar* toolbar = Toolbar();
    if(toolbar)
        {
        toolbar->SetToolbarVisibility(ETrue);    
        }
     
    // For floating toolbar in non-touch devices
    iScreenFurniture->SetToolbarVisibility(ETrue);
    //show the softkeys
    Cba()->MakeVisible( ETrue );
    Cba()->DrawNow();

    // Show the Metapane
    //iSingleLineMetaPane->ShowMetaPane(ETrue);

    //set the ui state to On
    SetUiSate(NGlxNFullScreenUIState::EUiOn);
    
    //start the timer,for the screen timeout
    iTimer->Cancel();
    if(aStartTimer)
        {
        iTimer->Start(KGlxScreenTimeout,KGlxScreenTimeout,TCallBack( TimeOut,this ));
        }
    }
    
// ---------------------------------------------------------------------------
// HideUi
// ---------------------------------------------------------------------------
//	
void  CGlxFullScreenViewImp::HideUi(TBool aSliderStatus)
    {
    TRACER("CGlxFullScreenViewImp::HideUi");
    //cancel the timer
    iTimer->Cancel();

    // For floating toolbar in non-touch devices
    iScreenFurniture->SetToolbarVisibility(EFalse);
    // hide the slider
    if(aSliderStatus && iSliderWidget)
        {
   	    iSliderWidget->ShowWidget(EFalse);
		}

    if (iCoverFlowWidget)
        {
        iCoverFlowWidget->SetUIMode(EFalse);
        }
    // hide the toolbar
    CAknToolbar* toolbar = Toolbar();
    if(toolbar)
        {
        toolbar->SetToolbarVisibility(EFalse); 
        }    
    
    // hide the softkeys
    Cba()->MakeVisible( EFalse );
    Cba()->DrawNow();
    // Hide the metapane
    //iSingleLineMetaPane->ShowMetaPane(EFalse);
 
    // set the ui state to On
    SetUiSate(NGlxNFullScreenUIState::EUiOff);
    }

// ---------------------------------------------------------------------------
// CreateScreenFurnitureL
// ---------------------------------------------------------------------------
//	
void  CGlxFullScreenViewImp::CreateScreenFurnitureL() 
    {
    TRACER("CGlxFullScreenViewImp::CreateScreenFurnitureL");
    // create the softkeys
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    CleanupStack::PushL( cba );
    cba->SetCommandSetL(R_GLX_FULLSCREEN_SOFTKEYS);
    // hide the softkeys
    cba->MakeVisible( EFalse );
    cba->DrawNow();
    CleanupStack::Pop(cba);
    }

// ---------------------------------------------------------------------------
// SetUiSate
// ---------------------------------------------------------------------------
//	
void CGlxFullScreenViewImp::SetUiSate (NGlxNFullScreenUIState::TUiState  aState)
    {
    TRACER("CGlxFullScreenViewImp::SetUiSate");
    iUiState = aState;
    }

// ---------------------------------------------------------------------------
// GetUiSate
// ---------------------------------------------------------------------------
//	
NGlxNFullScreenUIState::TUiState CGlxFullScreenViewImp::GetUiSate()    
    {
    TRACER("CGlxFullScreenViewImp::GetUiSate");
    return iUiState;
    }

// ---------------------------------------------------------------------------
// TimeOut
// ---------------------------------------------------------------------------
//	
TInt CGlxFullScreenViewImp::TimeOut(TAny* aSelf)
    {
    TRACER("CGlxFullScreenViewImp::TimeOut");
    if(aSelf)
        {
        CGlxFullScreenViewImp* self = static_cast <CGlxFullScreenViewImp*> (aSelf);
        if (self)
            {
            if ( NGlxNFullScreenUIState::EUiOn == self->GetUiSate())
                {
                // After time out, hide UI only when menu is not being displayed.
                if( self->MenuBar() && !(self->MenuBar()->IsDisplayed()) )
                    {
		            self->HideUi(ETrue);
					}
                }
            }            
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//DeactivateFullScreen function.
// ---------------------------------------------------------------------------
//
void  CGlxFullScreenViewImp::DeactivateFullScreen()
    {
    TRACER("CGlxFullScreenViewImp::DeactivateFullScreen");
    if (iCoverFlowWidget)
        {
        iCoverFlowWidget->RemoveEventHandler(*this);
        iCoverFlowWidget->ShowWidget(EFalse);
        }
    if(iSliderWidget)
        {
        iSliderWidget->RemoveEventHandler(*this);
        }
    //Dont Hide the slider,when activating the Zoom control,so pass EFalse
    HideUi(EFalse);
    iViewWidget->show(ETrue);
    }

// ---------------------------------------------------------------------------
//ActivateFullScreen function.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::ActivateFullScreenL()
    {
    TRACER("CGlxFullScreenViewImp::ActivateFullScreenL");
    
     if (iCoverFlowWidget)
        {
        iCoverFlowWidget->AddEventHandler(*this);
        iCoverFlowWidget->ShowWidget(ETrue);
        }
    if(iSliderWidget)
        {
        iSliderWidget->ShowWidget( EFalse );
        iSliderWidget->AddEventHandler(*this);
        }
    iViewWidget->show(ETrue);
    iViewWidget->setRect(TRect(TPoint(0,0),AlfUtil::ScreenSize()));
	GlxSetAppState::SetState(EGlxInFullScreenView);   
    }

// ---------------------------------------------------------------------------
//ActivateZoomControl function.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::ActivateZoomControlL(TZoomStartMode aStartMode, TPoint* apZoomFocus)
    {
    TRACER("CGlxFullScreenViewImp::ActivateZoomControlL");
    TInt focus = iMediaList->FocusIndex();
    TGlxMedia item = iMediaList->Item( focus );
    TInt error = GlxErrorManager::HasAttributeErrorL(item.Properties() , KGlxMediaIdThumbnail);
           
    if(KErrNone == error)
        {        
        if(EMPXImage == item.Category())
            {
            if(iZoomControl && !iZoomControl->Activated())
                {
                iHdmiController->ActivateZoom();
                iZoomControl->ActivateL(GetInitialZoomLevel(),aStartMode, focus, item, apZoomFocus);

                // Now to remove all textures other than the one we are focussing on.  
                TInt count = iMediaList->Count();
                while (count > 0)
                    {
                    TGlxMedia mediaItem = iMediaList->Item(count-1);	
                    if (mediaItem.Id() != item.Id() )
                        {
                        iUiUtility->GlxTextureManager().RemoveTexture(mediaItem.Id(),EFalse);
                        }
                    count--;
                    }
                }
            else
                {
                return;
                }
            DeactivateFullScreen();
           	GlxSetAppState::SetState(EGlxInZoomedView);
            }
        }
    }

// ---------------------------------------------------------------------------
//DeactivateZoomControl function.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DeactivateZoomControlL()
    {
    TRACER("CGlxFullScreenViewImp::DeactivateZoomControlL");
    ActivateFullScreenL(); 
    if(iZoomControl->Activated())
        {
        iHdmiController->DeactivateZoom();
        iZoomControl->Deactivate();
        }
    //Once into fullscreen view,show the screen furniture
    HideUi(ETrue);
    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DoMLViewDeactivate()
    {
    TRACER("CGlxFullScreenViewImp::DoMLViewDeactivate");
    iScreenFurniture->ViewDeactivated(iViewUid);
    if (iHdmiController)
        {
        delete iHdmiController;
        iHdmiController = NULL;
        }
    
    if(iZoomControl)
        {
        if(iZoomControl->Activated())
            {
            iZoomControl->Deactivate();
            }
        delete iZoomControl;
        iZoomControl = NULL;
        } 
    DestroyWidgets();
    iTimer->Cancel();
	// Deregister active media list pointer
    iActiveMediaListRegistry->DeregisterActiveMediaList(iMediaList);
    
    /*
     * safe code-- if user press back very quickly
     * cancelling the timer while going back to prev view
     */
    if (iPeriodic->IsActive())
 		{
 		iPeriodic->Cancel();
 		}

    }      
// ---------------------------------------------------------------------------
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::HandleForegroundEventL(TBool aForeground)
    {
    TRACER("CGlxFullScreenViewImp::HandleForegroundEventL");
    CAknView::HandleForegroundEventL(aForeground);
    
	
    if(iZoomControl && iZoomControl->Activated())
        {
        iZoomControl->HandleZoomForegroundEvent(aForeground);
        }

    if (!aForeground)
        {
        iUiUtility->GlxTextureManager().FlushTextures();
        }
    else
        {
        if (iMediaList && iMediaList->Count() && iMediaListMulModelProvider)
            {
            TInt focusIndex = iMediaList->FocusIndex();
            iMediaListMulModelProvider->UpdateItems(focusIndex, 1);
            }
        }
    }


// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler..
// ---------------------------------------------------------------------------
//
bool CGlxFullScreenViewImp::accept( CAlfWidgetControl& /*aControl*/,
		const TAlfEvent& /*aEvent*/) const
    {
    TRACER("CGlxFullScreenViewImp::accept()");
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler..
// ---------------------------------------------------------------------------
//
AlfEventStatus CGlxFullScreenViewImp::offerEvent(
		CAlfWidgetControl& /*aControl*/, const TAlfEvent& aEvent)
    {
    TRACER("CGlxFullScreenViewImp::offerEvent");
    // The below function call will not add any values except
    // to reduce the codescanner warnings to one
    return OfferEventL(aEvent);
    }

// ---------------------------------------------------------------------------
// OfferEventL()
// ---------------------------------------------------------------------------
//
AlfEventStatus CGlxFullScreenViewImp::OfferEventL(const TAlfEvent& aEvent)
    {
    TRACER("CGlxFullScreenViewImp::offerEventL");
    if ( aEvent.IsKeyEvent())
        {
        switch ( aEvent.KeyEvent().iScanCode )
            {
            case EStdKeyNkpAsterisk :
				if(iUiUtility->IsPenSupported())
					{
					return EEventNotHandled;
					}
            case EStdKeyIncVolume :
                //Listen EStdKeyApplicationD as EKeyZoomIn key is mapped to TKeyCode:: 
                //EKeyApplicationC for which TStdScancode is EStdKeyApplicatoinC
            case EStdKeyApplicationC: 
                {
                if(EEventKeyDown == aEvent.Code())
                    {
                    //Dont start the timer while activating the zoom control,
                    //when the timer activates while launching the zoom 
                    //it causes the crash.
                    ShowUiL(EFalse);
                    SetSliderLevel();
                    TRAP_IGNORE( ActivateZoomControlL(EZoomStartKey));
                    return EEventConsumed;
                    }
                }
            case EStdKeyUpArrow:            
            case EStdKeyDownArrow:
            case EStdKeyDevice0:
            case EStdKeyDevice1:
                //Listen EStdKeyApplicationC as EKeyZoomOut key is mapped to TKeyCode:: 
                //EKeyApplicationD for which TStdScancode is EStdKeyApplicatoinD 
            case EStdKeyApplicationD: 
                {
                if ( NGlxNFullScreenUIState::EUiOff == GetUiSate()&& (
                        aEvent.Code() == EEventKey ) )
                    {
                    //the Ui timer sjould be started once the UI screen furniture is shown
                    ShowUiL(ETrue);
                    } 
                return EEventHandled;              
                }
            case EStdKeyLeftArrow:
            case EStdKeyRightArrow:
                {
                if ( NGlxNFullScreenUIState::EUiOn == GetUiSate() )                    
                    {
                    HideUi(ETrue);
                    }
                return EEventHandled;
                }
			case EStdKeyApplication1C:
           		{
				if(iUiUtility->IsPenSupported())
					{
					return EEventNotHandled;
					}
				else
					{
					ProcessCommandL(KGlxCmdMnShowMap);
                    return EEventHandled;  				
					}
				}
            default:
                return EEventNotHandled;
            }
        }
    if(!aEvent.IsCustomEvent())
            {   
            GLX_LOG_INFO("Event Not Handled");          
            return EEventNotHandled;
            }

    if(aEvent.IsCustomEvent())
        {
        TInt eventId = aEvent.CustomParameter();
        
        switch(eventId)
            {   
            case KAlfActionIdDeviceLayoutChanged:
                 {
                 iViewWidget->setRect(TRect(TPoint(0,0),AlfUtil::ScreenSize()));
                 
                 iScreenFurniture->SetToolbarPosition();
                                  
                 return EEventNotHandled;
                 }
            case ETypeSelect:
                {
                TBool pensupport = iUiUtility->IsPenSupported();
                if ( pensupport )
                    {
                    if ( NGlxNFullScreenUIState::EUiOn == GetUiSate() )                    
                        {
                        //Hide the slider 
                        HideUi(ETrue);
                        }
                    else 
                        {
                        //the Ui timer should be started once the UI screen furniture is shown
                        ShowUiL(ETrue);
                        }
                    }
                else
                    {
                    if ( NGlxNFullScreenUIState::EUiOff == GetUiSate() )                    
                        {
                        //the Ui timer should be started once the UI screen furniture is shown
                        ShowUiL(ETrue);
                        }
                    }
                }
                break;
                
            case ETypeRemove:
                {
                // Handle the "C" key or the BackSpace key to Delete an item.
                ProcessCommandL(EGlxCmdDelete);
                return EEventConsumed;
                }               
                
            case ETypeDoubleTap:
                {
                GLX_LOG_INFO("CGlxFullScreenViewImp::OfferEventL ETypeDoubleTap");   

                MulDoubleTapData* DoubleTapData = (MulDoubleTapData*)(aEvent.CustomEventData()); 
                TPoint doubleTapPoint = DoubleTapData->mDoubleTapPoint;
                SetSliderLevel();
                TRAP_IGNORE( ActivateZoomControlL(EZoomStartDoubleTap, &doubleTapPoint)  );
                return EEventConsumed;
                }
            case ETypeHighlight:
                {
                iMediaList->SetFocusL( NGlxListDefs::EAbsolute,(aEvent.CustomEventData())); 
                if (AknLayoutUtils::PenEnabled())
                	{
                	iMediaList->SetVisibleWindowIndexL(aEvent.CustomEventData()); 
                	}
                if(iMediaList->Count()> KFullScreenTextureOffset)
                    {
                    RemoveTexture();
                    }
				TGlxMedia item1 = iMediaList->Item( iMediaList->FocusIndex() );
                TInt frameCount(0);
                TSize orignalSize;
                TBool aFramesPresent  = item1.GetFrameCount(frameCount);
                TBool adimension  = item1.GetDimensions(orignalSize);
                if (item1.Category() != EMPXVideo)
                    {
                    GLX_LOG_INFO("CGlxHdmi - FullscreenView - SetImageL");                    
                    iHdmiController->SetImageL(item1.Uri(), orignalSize, frameCount);
                    }
                else
                    {                    
                    iHdmiController->IsVideo();                    
				    }	                    
                iOldFocusIndex = iMediaList->FocusIndex();
                if ( NGlxNFullScreenUIState::EUiOn == GetUiSate() )
                    {
                    HideUi(ETrue);
                    }
                TRAP_IGNORE(ShowDrmExpiaryNoteL());
                return EEventConsumed;
                }
            case EVideoIconSelect:
                {
                TGlxMedia item = iMediaList->Item( iMediaList->FocusIndex() );
                if(item.Category() == EMPXVideo)
                    {                                 
                    ProcessCommandL(EGlxCmdPlay);
                    } 
                else
                    { 
                    if ( NGlxNFullScreenUIState::EUiOn == GetUiSate() )                    
                        {
                        HideUi(ETrue);
                        }
                    else 
                        {
                        //the Ui timer should be started once the UI screen furniture is shown
                        ShowUiL(ETrue);
                        }
                    }
                }         
                break;
            //Zoom Start via pinch
            case ETypePinch:
                // Handle only a pinch out. not a pinch in. 
                if (KGlxNeutralPinchPosition < aEvent.CustomEventData())
                    {
                    SetSliderLevel();
                    TRAP_IGNORE( ActivateZoomControlL(EZoomStartPinch));
                    }
                return EEventConsumed; // will be consumed even if we are pinching IN (trying to zoom OUT). 
                //slider event handling   
            case ETypePrimaryValueChange:
                {
                // Zoomview is only launched if the currentslider value is above the level
                // of fullscreenlevel.
                iTimer->Cancel();
                iSliderWidget->ShowWidget( ETrue, 0 );
                SetSliderLevel();
                TRAP_IGNORE( ActivateZoomControlL(EZoomStartSlider));
                }
                return EEventConsumed;
            default:
                return EEventNotHandled;
            }
        }
    return EEventNotHandled;
    }
// ---------------------------------------------------------------------------
// From HandleViewCommandL..
// ---------------------------------------------------------------------------
//
TBool CGlxFullScreenViewImp::HandleViewCommandL(TInt aCommand)
	{
	TRACER("GCGlxFullScreenViewImp::HandleViewCommandL");
	//size calculated for rotation
	TSize size = iUiUtility->GetRotatedImageSize(); // ViewBase iUiUtility is use
	TBool consumed = EFalse;
	switch (aCommand)
		{
		case EGlxCmdResetView:
		    {		    
		    HideUi(ETrue);
		    consumed = ETrue;
		    break;
		    }
		case EGlxCmdFullScreenBack:
		    SetSliderLevel();
		    DeactivateZoomControlL();
		    consumed = ETrue;
		    break;
//		case EGlxCmdRotateLeft:
//			iCoverFlowWidget->RotateImage(IMulWidget::ERotateLeft,size,KGlxRotationDelay);
//		    consumed = ETrue;
//		    break;
//		case EGlxCmdRotateRight:
//		    iCoverFlowWidget->RotateImage(IMulWidget::ERotateRight,size,KGlxRotationDelay);
//            consumed = ETrue;
//            break;
        } 
    return consumed;
    }

// ---------------------------------------------------------------------------
// From HandleCommandL..
// ---------------------------------------------------------------------------
//
 void CGlxFullScreenViewImp::HandleCommandL(TInt aCommandId, CAlfControl* aControl) 
	 {
	 TRACER("CGlxFullScreenViewImp::HandleCommandLCAlfControl");
	 //Gets a callback from zoomview if zoomlevel goes beyound the launch zoomlevel
	 // and activates the fullscreenview
	 if((aControl == iZoomControl)&& (aCommandId == KGlxZoomOutCommand))
	     {
	     SetSliderLevel();
	     DeactivateZoomControlL();
	     }
	 } 

// ---------------------------------------------------------------------------
// From HandleResourceChangeL..
// ---------------------------------------------------------------------------
//
 void CGlxFullScreenViewImp::HandleResourceChangeL (TInt aType)
     {
     TRACER("CGlxFullScreenViewImp::HandleResourceChangeL");
         HandleResourceChangeL ( aType);
         switch (aType)
             {
        case KEikDynamicLayoutVariantSwitch:
            {
            //if resolution changed
//                 iSliderWidget->control()->Display()->SetVisibleArea( ClientRect() );
            break;
            }
        }       
    }   

//----------------------------------------------------------------------------------
// InitialZoomLevel:Calculate the initial Zoom Level for the Image
//----------------------------------------------------------------------------------
//
TInt CGlxFullScreenViewImp::GetInitialZoomLevel()
    {
    TRACER("CGlxFullScreenViewImp::InitialZoomLevel");
    TInt focus     = iMediaList->FocusIndex();
    TGlxMedia item = iMediaList->Item( focus );
    TSize size;
    TUint8 initialZoomLevel;
    item.GetDimensions( size );
    TRect rect = AlfUtil::ScreenSize();

    if( rect.Width()>= size.iWidth && rect.Height() >= size.iHeight)
        {
        //if Both the Width and Height are lesser than the screen size,the initial Zoom Level will be 100
        initialZoomLevel = KGlxMinSmallImageZoomLevel;
        }
    else
        {
        //Calculate (Display Area Width)  /Image Width   * 100
        //Calculate (Display Area Height) /Image Height  * 100
        //Choose the Minimum Of the above.
        TReal32 imageWidthRatio  = ((TReal32) rect.Width() / size.iWidth )*100.0F;
        TReal32 imageHeightRatio = ((TReal32)rect.Height()/ size.iHeight )*100.0F;
        initialZoomLevel = Min(imageWidthRatio,imageHeightRatio);
        } 
    return initialZoomLevel;
    }

//----------------------------------------------------------------------------------
// SetSliderLevel() Set the Initial Zoom Level for the Image
//----------------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::SetSliderLevel()
    {
    TRACER("CGlxFullScreenViewImp::SetSliderLevel");

    TInt value = GetInitialZoomLevel();
    if(iSliderModel)
        {
        iSliderModel->SetMinRange(value);
        (value==KGlxMaxBigImageZoomLevel)?(iSliderModel->SetMaxRange(KGlxMaxSmallImageZoomLevel))
            :(iSliderModel->SetMaxRange(KGlxMaxBigImageZoomLevel));
        iSliderModel->SetPrimaryValue(value);
        }
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler..
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::setEventHandlerData(
		const AlfWidgetEventHandlerInitData& /*aData*/)
    {
    TRACER("CGlxFullScreenViewImp::setEventHandlerData");
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler..
// ---------------------------------------------------------------------------
//
AlfWidgetEventHandlerInitData* CGlxFullScreenViewImp::eventHandlerData()
    {
    TRACER("CGlxFullScreenViewImp::eventHandlerData");
    return NULL;
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::setActiveStates( unsigned int /*aStates*/ )
    {
    TRACER("CGlxFullScreenViewImp::setActiveStates");
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler.
// ---------------------------------------------------------------------------
//

IAlfInterfaceBase* CGlxFullScreenViewImp::makeInterface( const IfId& /*aType*/ )
    {
    TRACER("CGlxFullScreenViewImp::makeInterface");
    return NULL;
    }

//----------------------------------------------------------------------------------
// eventHandlerType
//----------------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerType CGlxFullScreenViewImp::eventHandlerType() 
	{
	TRACER("CGlxFullScreenViewImp::eventHandlerType");
	return IAlfWidgetEventHandler::ELogicalEventHandler;
	}

//----------------------------------------------------------------------------------
// eventExecutionPhase
//----------------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase CGlxFullScreenViewImp::eventExecutionPhase()
	{
	TRACER("CGlxFullScreenViewImp::eventHandlerType");
    return EBubblingPhaseEventHandler;
	}   

// -----------------------------------------------------------------------------
// Callback from periodic timer
// -----------------------------------------------------------------------------
//
TInt CGlxFullScreenViewImp::PeriodicCallback(TAny* aPtr )
    {
    TRACER("CGlxFullScreenViewImp::PeriodicCallback");
    static_cast< CGlxFullScreenViewImp* >( aPtr )->CallPeriodicCallback();
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// Callback from periodic timer-- non static
// -----------------------------------------------------------------------------
//
inline void CGlxFullScreenViewImp::CallPeriodicCallback()
    {
    TRACER("CGlxFullScreenViewImp::CallPeriodicCallback");
    if(iPeriodic->IsActive())
		{
		iPeriodic->Cancel();
		}
    /*
     * function can leave
     */
    TRAP_IGNORE(ShowDrmExpiaryNoteL());
    }
	
// ---------------------------------------------------------------------------
// 
// Shows expiry note/dialog for expired DRM files, can leave
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::ShowDrmExpiaryNoteL()
	{
	TRACER("CGlxFullScreenViewImp::ShowDrmExpiaryNoteL");
	if(iMediaList->Count()>0)
		{
	const TGlxMedia& media = iMediaList->Item(iMediaList->FocusIndex());
	if (media.IsDrmProtected())
		{
			const TDesC& uri = media.Uri();
			if ( !iDrmUtility->CheckOpenRightsL(uri , (media.Category() == EMPXImage)) && ( uri.Length()>0 ))
				{
				iDrmUtility->ShowRightsInfoL(uri);
				}
			}
		}
	}
// ---------------------------------------------------------------------------
// 
// Gets the Swipe direction
// ---------------------------------------------------------------------------
//
TSwipe CGlxFullScreenViewImp::GetSwipeDirection()
    {
    TRACER("CGlxFullScreenViewImp::GetSwipeDirection");
    
    TInt count = iMediaList->Count();
    TInt focusindex = iMediaList->FocusIndex();
    TSwipe swipedirection;
    
    if(focusindex == 0)
        {
        swipedirection = (iOldFocusIndex == 1 ? EBackward : EForward);
        }
    else if(focusindex == count-1)
        {
        swipedirection = (iOldFocusIndex == 0 ? EBackward : EForward);
        }
    else
        {
        swipedirection = (focusindex-iOldFocusIndex < 0 ? EBackward : EForward) ;
        }     
      return swipedirection;        
    }
// ---------------------------------------------------------------------------
// 
// Gets the index of the item for which the texture has to be removed
// ---------------------------------------------------------------------------
//
TInt CGlxFullScreenViewImp::GetIndexToBeRemoved()
    {
    TRACER("CGlxFullScreenViewImp::GetIndexToBeRemoved");
    
    TSwipe swipedirection = GetSwipeDirection();
    TInt indextoberemoved;
    TInt count = iMediaList->Count();
    TInt focusindex = iMediaList->FocusIndex();
    if(swipedirection == EForward)
        {
        indextoberemoved = focusindex-KFullScreenIterator;
        if(indextoberemoved < 0)
            {
            indextoberemoved = count+indextoberemoved;
            }
        }
    else
        {
        indextoberemoved = focusindex+KFullScreenIterator;
        if(indextoberemoved > count-1)
          {
          indextoberemoved = indextoberemoved-count;
          }        
        }
    GLX_LOG_INFO1("CGlxFullScreenViewImp::GetIndexToBeRemoved index tobe removed %d",indextoberemoved );
    return indextoberemoved;
    }
// ---------------------------------------------------------------------------
// 
// Remove the texture on every swipe while HDMI is connected.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::RemoveTexture()
    {
    TRACER("CGlxFullScreenViewImp::RemoveTexture");
    
    const TGlxMedia& item = iMediaList->Item(GetIndexToBeRemoved());
    iUiUtility->GlxTextureManager().RemoveTexture(item.Id(),EFalse);
    }

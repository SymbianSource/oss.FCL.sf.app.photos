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
* Description:    Implementation of FULL-SCREEN view
*
*/




// Avkon and System Headers 
#include <eikbtgpc.h>                           // CEikButtonGroupContainer
#include <aknViewAppUi.h>
#include <data_caging_path_literals.hrh>        // for KDC_APP_RESOURCE_DIR
#include <glxcollectionpluginimageviewer.hrh>
#include <StringLoader.h>                       // String Loader
#include <AknIconUtils.h>
#include <e32math.h>

//For animation Effects
#include <akntranseffect.h>
#include <akntransitionutils.h>
#include <alf/alflayout.h>

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
#include <coeutils.h>

using namespace GestureHelper;

#include <glxhdmicontroller.h>

// Photos Headers
#include <glxlog.h>                             // CGlxLogger
#include <glxtracer.h>                          // For Log
#include <glxmedialistmulmodelprovider.h>       // For MulModelProvider
#include <glxcommandhandlers.hrh>               // For EGlxCmdFullScreenBack               
#include <glxfullscreenviewdata.rsg>            // FS View Resource
#include <glxthumbnailattributeinfo.h>          // KGlxMediaIdThumbnail
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
#include "glxfullscreenviewimp.h"
#include <glxnavigationalstate.h>
#include <glxmedia.h>
#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>
#include <glxuistd.h>
#include "glxfullscreenbusyicon.h"

using namespace Alf;

const TInt KGlxScreenTimeout =10000000;
const TInt KCoverflowDataWindowSize = 1;
const TInt KGlxMaxSmallImageZoomLevel =150;
const TInt KGlxMaxBigImageZoomLevel   =100;
const TInt KGlxMinSmallImageZoomLevel =100;
const TInt KGlxNeutralPinchPosition   =100;
const TInt KGlxSliderTickValue        =5;
const TInt KGlxRealTruncationPadding  =0.5;

/**
 * Start Delay for the periodic timer, in microseconds
 */
const TInt KPeriodicStartDelay = 250000; 
/**
 * Interval delay for the periodic timer, in microseconds
 */
const TInt KPeriodicIntervalDelay = 500000; 

/** 
 * Fullscreen texture offset 
 */ 
const TInt KTextureOffset = 2; 

// This constant is used to calculate the index of the item 
// for which texture has to removed.
const TInt KFullScreenIterator = KTextureOffset + 1; 

// This constant is used to calculate the maximum number of fullscreen textures 
// needs to be updated during foreground event.
const TInt KFullScreenTextureOffset = KFullScreenIterator + KTextureOffset;

const TInt KGlxDecodingThreshold = 3000000; // pixels

_LIT( KTfxResourceActivateFullScreen, "z:\\resource\\effects\\photos_fullscreen_open.fxml" );
_LIT( KTfxResourceNoEffect, "");

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
	iImgViewerMode = EFalse;
    //Disable the toolbar always while entering fullscreen
	EnableFSToolbar(EFalse);
    ShowToolbarOnViewActivation(EFalse);
	
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
    
    iIsDialogLaunched = EFalse;
    iIsMMCRemoved = EFalse;
	
	iMMCNotifier = CGlxMMCNotifier::NewL(*this);
	if(!iPeriodic)
		{
		iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
		}
	iGlxTvOut = CGlxTv::NewL(*this);
	
	iNaviState = CGlxNavigationalState::InstanceL();
    }

// ---------------------------------------------------------------------------
// ~CGlxFullScreenViewImp()
// ---------------------------------------------------------------------------
//
CGlxFullScreenViewImp::~CGlxFullScreenViewImp()
    {
    TRACER("CGlxFullScreenViewImp::~CGlxFullScreenViewImp");
    if(iAlfEffectObs)
        {
        delete iAlfEffectObs;
        }
    if(iMMCNotifier)
        {
        delete iMMCNotifier;
        iMMCNotifier = NULL;
        }
    
    if (iUri)
        {
        delete iUri;
        }
    
    if(iMediaListMulModelProvider)
        {
        delete iMediaListMulModelProvider;
        }
    if(iTimer)
        {
        if(iTimer->IsActive())
            {
            iTimer->Cancel();
            }
        delete iTimer;
        }
	if(iZoomButtonGroup)
        {
        AppUi()->RemoveFromStack( iZoomButtonGroup );
        delete iZoomButtonGroup;
        }
	if(iGlxTvOut)
        {
        delete iGlxTvOut;
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
    
    if (iNaviState)
        {
        iNaviState->Close();
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
    EnableFSToolbar(EFalse);
    
    if (iMediaList->Count() == 0)
        {
        GLX_LOG_INFO("CGlxFullScreenViewImp::DoMLViewActivateL() - No items!");	
		// While in slideshow from fullscreen view,
        // 1) Connect USB in mass storage mode or 
        // 2) Delete items through file manager 
        // shall result in activating fullscreen view with no items; so, goto grid view.
        iNaviState->ActivatePreviousViewL();
        }
    else
        {
        //fix for ESLM-7YYDXC: When in FullScreen View, view mode must be 'EView'
        //While coming back from SlideShow to FullScreen view, need to set view mode.
        if (iMediaList->Count() && iNaviState->ViewingMode()
                != NGlxNavigationalState::EView)
            {
            iNaviState->SetToViewMode();
            }
        }

    CMPXCollectionPath* collPath = iNaviState->StateLC();
    if (collPath->Id() == TMPXItemId(
            KGlxCollectionPluginImageViewerImplementationUid))
        {
        iImgViewerMode = ETrue;
        CreateImageViewerInstanceL();
        }
    CleanupStack::PopAndDestroy(collPath);

    iScrnSize = iUiUtility->DisplaySize();
    iGridIconSize = iUiUtility->GetGridIconSize();

    iHdmiWidth = (iScrnSize.iWidth > iScrnSize.iHeight ?iScrnSize.iWidth :iScrnSize.iHeight);
    iHdmiHeight = (iScrnSize.iWidth < iScrnSize.iHeight ?iScrnSize.iWidth :iScrnSize.iHeight);

    //Create hdmicontroller when it is only launched from fullscreen.  
    //From filemanager show only clone mode.
    iHdmiController = CGlxHdmiController::NewL(*this); 
    
    if( !iImgViewerMode )    
        {
        SetItemToHDMIL();
        }
    iScreenFurniture->SetActiveView(iViewUid);
    
    //set the ui state to off,when the Fullscreen launches
    SetUiState(EUiOff);
   
   	GlxSetAppState::SetState(EGlxInFullScreenView);
   	 
    // create the coverflow
    CreateCoverflowWidgetL();
    
    CreateSliderWidgetL();
    
    iBusyIcon = CGlxFullScreenBusyIcon::NewL(*iMediaList,*iUiUtility);
    // create a zoom control
    iZoomControl = CGlxZoomControl::NewL(*this, *iMediaList,
            *iZoomButtonGroup, *iSliderWidget, iGestureHelper);

    // hide the screen furniture
    HideUi(ETrue);
	// Set active media list pointer, so that UPnP renderer knows about this list
    iActiveMediaListRegistry->RegisterActiveMediaList(iMediaList);
    
	if ( !iPeriodic->IsActive() )
		{
	    iPeriodic->Start(KPeriodicStartDelay, KPeriodicIntervalDelay,
	            TCallBack(&PeriodicCallback, static_cast<TAny*> (this)));
		}
		
	//Set the animation effect for the coverflow widget since the animation
	//does not work for both avkon and alf together. This can be done since the
	//UI Screen furnitures are disabled by default while going to fullscreen.
	//Check if the transitions are enabled from themes
    if (CAknTransitionUtils::TransitionsEnabled(
                                    AknTransEffect::EFullScreenTransitionsOff))
        {
        if(!iAlfEffectObs)
            {
            iAlfEffectObs = CAlfEffectObserver::NewL();
            }
        const_cast<CAlfLayout&> (iCoverFlowWidget->ContainerLayout()).SetEffectL(
                KTfxResourceActivateFullScreen);
        iEffectHandle = iCoverFlowWidget->ContainerLayout().Identifier();
        iAlfEffectObs->SubscribeCallbackL(this,iEffectHandle);
        }
    
	//Disable the status pane here as it causes flicker while animating
    CAknViewAppUi* appui = AppUi();
    if ( appui )
        {
        SetTitlePaneTextL(KNullDesC);
        appui->StatusPane()->MakeVisible(EFalse);
        appui->Cba()->MakeVisible(EFalse);
        }

	iViewWidget->show(true);    
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

        IAlfWidgetFactory& widgetFactory =
                AlfWidgetEnvExtension::widgetFactory(*iEnv);

        // create a view widget  
        iViewWidget = widgetFactory.createViewWidget(KCoverflowViewWidget,
                0x113);
        // hide control and status pane
        iViewWidget->enableControlPane(EFalse);
        iViewWidget->enableStatusPane(EFalse);
        iViewWidget->setRect(TRect(TPoint(0, 0), AlfUtil::ScreenSize()));
        // create coverflow widget
        iCoverFlowWidget = widgetFactory.createWidget<IMulCoverFlowWidget> (
                KWidgetName, KCoverflowWidget, *iViewWidget, NULL);
        iCoverFlowWidget->SetFlags(IMulMultiItemWidget::EMulWidgetDoubleTap);

        // Widget takes the ownership
        iCoverFlowWidget->AddEventHandler(*this);

        iCoverFlowWidget->control()->AcquireFocus();
        iMediaListMulModelProvider = CGlxMediaListMulModelProvider::NewL(
                *iEnv, *iCoverFlowWidget, *iMediaList, iFullScreenBindingSet,
                mulwidget::KTemplate4, KCoverflowDataWindowSize);

        iGestureHelper
                = (GestureHelper::CGestureHelper*) iCoverFlowWidget->Gesturehelper();
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
        iSliderWidget->control()->disableState(IAlfWidgetControl::Focusable);
    
        //adding the range and slider tick value 
        if(iSliderModel)
            {
            iSliderModel->SetTick(KGlxSliderTickValue);
            }
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
	if(CheckIfSliderToBeShownL())
        {
        //To set the Slider values.
        SetSliderToMin();
        //show the slider
        iSliderWidget->ShowWidget(ETrue);
        }
    else
        {
        //hide slider if UI is needed to be on and HDMI is Connected
        iSliderWidget->ShowWidget(EFalse);
        }
    
    if (iCoverFlowWidget)
        {
		// This is done to set the item counter
        iCoverFlowWidget->SetUIMode(ETrue);
        }

    //Since the toolbar should not be present for ImageViewer.
    if(!iImgViewerMode)
        {
        //show the toolbar
        EnableFSToolbar(ETrue);
        }
     
    // For floating toolbar in non-touch devices
    iScreenFurniture->SetToolbarVisibility(ETrue);
    
    //show the softkeys
    CEikButtonGroupContainer* cba = Cba();
    cba->SetCommandSetL( R_GLX_FULLSCREEN_SOFTKEYS );
    cba->MakeVisible( ETrue );
    cba->DrawNow();

    //set the ui state to On
    SetUiState(EUiOn);
    
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
void  CGlxFullScreenViewImp::HideUi(TBool aHideSlider)
    {
    TRACER("CGlxFullScreenViewImp::HideUi");
    //cancel the timer
    iTimer->Cancel();

    // For floating toolbar in non-touch devices
    iScreenFurniture->SetToolbarVisibility(EFalse);
    
    // hide/show the slider
    if(iSliderWidget) 
        {
        iSliderWidget->ShowWidget(!aHideSlider);        
        }

    if (iCoverFlowWidget)
        {
        iCoverFlowWidget->SetUIMode(EFalse);
        }
    
    //Since the toolbar should not be present for ImageViewer.
    if(!iImgViewerMode)
        {
        // hide the toolbar
        CAknToolbar* toolbar = Toolbar();
        if(toolbar)
            {
            toolbar->SetToolbarVisibility(EFalse); 
            }
        }
    
    // hide the softkeys
    Cba()->MakeVisible( EFalse );
    Cba()->DrawNow();
 
    // set the ui state to On
    SetUiState(EUiOff);
    }

// ---------------------------------------------------------------------------
// SetUiState
// ---------------------------------------------------------------------------
//	
void CGlxFullScreenViewImp::SetUiState (TUiState  aState)
    {
    TRACER("CGlxFullScreenViewImp::SetUiState");
    iUiState = aState;
    }

// ---------------------------------------------------------------------------
// GetUiState
// ---------------------------------------------------------------------------
//	
TUiState CGlxFullScreenViewImp::GetUiState()    
    {
    TRACER("CGlxFullScreenViewImp::GetUiState");
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
            if ( EUiOn == self->GetUiState())
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
    
        //show/hide the slider based on the return value of
        //IsHidden()
	    if(iZoomControl->Activated())
	        {
	        HideUi(iSliderWidget->IsHidden());
	        }
		}
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
        iSliderWidget->AddEventHandler(*this);
        }
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
    /*
     * We will not do any zoom while HDMI is connected.
     * This is as part of HDMI improvements.
     */
    if (iHdmiController && iHdmiController->IsHDMIConnected())
        {
        // Hide zoom slider in HDMI mode as Zoom is disable.
        // Let the other screen furnitures intact.
        iSliderWidget->ShowWidget( EFalse);
        return;
        }
    TInt focus = iMediaList->FocusIndex();
    TGlxMedia item = iMediaList->Item( focus );
    TInt error = GlxErrorManager::HasAttributeErrorL(item.Properties() , KGlxMediaIdThumbnail);
   
    // Check if DRM rights expired for a specific media item 
    TBool isDrmRightsValid = ETrue;
    if(item.IsDrmProtected())
    	{
        if (iImgViewerMode && iImageViewerInstance->IsPrivate())
            {
            isDrmRightsValid = iDrmUtility->DisplayItemRightsCheckL(
                    iImageViewerInstance->ImageFileHandle(), (item.Category()
                            == EMPXImage));
            }
        else
            {
            //Since it is always for the focused item - use DisplayItemRightsCheckL 
            //instead of ItemRightsValidityCheckL
            isDrmRightsValid = iDrmUtility->DisplayItemRightsCheckL(
                    item.Uri(), (item.Category() == EMPXImage));
            }
        }
    // Activate Zoom if the item is an image and its DRM rights is not expired
    if(KErrNone == error && (EMPXImage == item.Category()) && isDrmRightsValid)
        {
        if(iZoomControl && !iZoomControl->Activated())
            {
            if(iHdmiController)
                {
                TBool autoZoomOut = ETrue;
                //Set autozoomout to false in pinch zoom, since we would be 
                //zooming out on deactivate once we get the release event.
                if(aStartMode == EZoomStartPinch)                    
                    {
                    autoZoomOut = EFalse; 
                    }
                
                iHdmiController->ActivateZoom(autoZoomOut);
                }
            
            GLX_LOG_INFO1("ActivateZoomControlL: Slider MaxRange = %d   ", iSliderModel->MaxRange() );
            GLX_LOG_INFO1("ActivateZoomControlL: Slider MinRange = %d   ", iSliderModel->MinRange() );
            GLX_LOG_INFO1("ActivateZoomControlL: Slider PrimaryValue= %d", iSliderModel->PrimaryValue() );
            
            if (aStartMode == EZoomStartSlider) 
                {
                iZoomControl->ActivateL(iSliderModel->PrimaryValue(),aStartMode, focus,
                                        item, apZoomFocus,iImgViewerMode);
                }
            else 
                {
                iZoomControl->ActivateL(GetInitialZoomLevel(),aStartMode, focus,
                                        item, apZoomFocus,iImgViewerMode);
                }
            // Now to remove all textures other than the one we are focussing on.  
            TInt frameCount = KErrNone;
            item.GetFrameCount(frameCount);
            // If .gif file, then only call stopanimation
			if (frameCount > 1)
                {
                iUiUtility->GlxTextureManager().AnimateMediaItem(
                        item.Id(), EFalse);
                }
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

// ---------------------------------------------------------------------------
//DeactivateZoomControl function.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DeactivateZoomControlL()
    {
    TRACER("CGlxFullScreenViewImp::DeactivateZoomControlL");
    ActivateFullScreenL();
    UpdateItems();
    //Deactivate HDMI controller for zoom out while pinch zooming.
    if(iHdmiController)
        {
        iHdmiController->DeactivateZoom();
        }
    if(iZoomControl->Activated())
        {        
        iZoomControl->Deactivate();
        }
    if (iMediaList->FocusIndex() != KErrNotFound)
        {
        TGlxMedia item = iMediaList->Item(iMediaList->FocusIndex());
        TInt frameCount = KErrNone;
        item.GetFrameCount(frameCount);
		// If .gif file, then only call start animation
        if (frameCount > 1)
            {
            iUiUtility->GlxTextureManager().AnimateMediaItem(
                    item.Id(), ETrue);
            }
        }
    
    SetSliderToMin();
    //check if the slider is already visible in zoom view.
    //if yes then do not disable the slider.
    TBool sliderInvisible = ETrue;
    if (EUiOn == iZoomControl->ZoomUiState())
        {
        sliderInvisible = EFalse;
        }
    
    HideUi(sliderInvisible);
    
    if (!sliderInvisible)
        {
        //Set the UI State to On Explicitly since the slider is ON and we have
        //to disable on timeout. The timer is already cancelled in HideUi().
        SetUiState(EUiOn);
        iTimer->Start(KGlxScreenTimeout,KGlxScreenTimeout,TCallBack( TimeOut,this ));
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DoMLViewDeactivate()
    {
    TRACER("CGlxFullScreenViewImp::DoMLViewDeactivate");
	//Disabling the toolbar here since it would give a crash when
	//we try to enable the toolbar in activate without exiting photos.
    if(!iImgViewerMode)
        {    
        // hide the toolbar
        EnableFSToolbar(EFalse);
        
        //Setting Soft key to EmptySoftKeys would cause crash
		//When Fullscreen is opened from ImageViewer.
        //And also, when Photos exits from FullScreen View.
        //So Set Soft Keys to empty only when not in Image Viewer mode 
        //& Photos is not exiting.
		if( !iUiUtility->IsExitingState() )
			{
			CEikButtonGroupContainer* cba = Cba();
			TRAP_IGNORE( cba->SetCommandSetL( R_GLX_FULLSCREEN_EMPTYSOFTKEYS ) );
			cba->DrawNow();
			}
        }
	else
		{
		DeleteImageViewerInstance();
		}
    
    HideUi(ETrue); 
	// In Order to hide the softkeys immediately. The above statement does not do that as soon as we need. 
	// So we do the below trick. The SK overlap is still there but much much less noticable. 
    CCoeEnv::Static()->WsSession().Flush(); 

    //Clear the last uri for which DRM Rights were consumed before going back to grid view
    //since the GridView::Activate() and FullScreen::DeActivate() can be called in any order,
    //this call is being made to be on safer side
    iDrmUtility->ClearLastConsumedItemUriL();
    iScreenFurniture->ViewDeactivated(iViewUid);
    iImgViewerMode = EFalse;
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

    iUiUtility->SetForegroundStatus(aForeground);

    if(iMMCState)
        {
        iMMCState =EFalse;
        NavigateToMainListL();
        }
    
    if(iZoomControl && iZoomControl->Activated())
        {
        iZoomControl->HandleZoomForegroundEvent(aForeground);
        if (aForeground && iUri && !ConeUtils::FileExists(iUri->Des()))
            {
            GLX_LOG_INFO("File does not exist, Exit zoom view!");
            HandleItemRemovedL();
            }
        }

    if (!aForeground)
        {
        if(iHdmiController)
			{   
            iHdmiController->ShiftToCloningMode();
			}
		iUiUtility->GlxTextureManager().FlushTextures();
		}
	else
		{
		if (iMediaList)
			{
			/** if there is no image to show go back to the previous view */
			if (!iMediaList->Count() && iNaviState->ViewingMode()
                    == NGlxNavigationalState::EView)
                {
                iUiUtility->SetViewNavigationDirection(
                        EGlxNavigationBackwards);
                iNaviState->ActivatePreviousViewL();
                }
			else if (iMediaListMulModelProvider)
				{
	            UpdateItems();

				if (iHdmiController)
					{
					iHdmiController->ShiftToPostingMode();
					}
				}
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
                    HideUi(EFalse);
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
                if ( EUiOff == GetUiState()&& (
                        aEvent.Code() == EEventKey ) )
                    {
                    //the Ui timer should be started once the UI screen furniture is shown
                    ShowUiL(ETrue);
                    } 
                return EEventHandled;              
                }
            case EStdKeyLeftArrow:
            case EStdKeyRightArrow:
                {
                if ( EUiOn == GetUiState() )                    
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
                    if ( EUiOn == GetUiState() )                    
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
                    if ( EUiOff == GetUiState() )                    
                        {
                        //the Ui timer should be started once the UI screen furniture is shown
                        ShowUiL(ETrue);
                        }
                    }
                }
                break;
                
            case ETypeRemove:
                {
                // If From photos, delete the img.
                // If Image-Viewer collection and not in private Path 
                // handle the "C" or BackSpace key to delete the item
                if (!iImgViewerMode || (iImageViewerInstance
                        && !iImageViewerInstance->IsPrivate()))
                    {
                    ProcessCommandL(EGlxCmdDelete);
                    return EEventConsumed;
                    }
                return EEventNotHandled;
                }               
            case ETypeDoubleTap:
                {
                GLX_LOG_INFO("CGlxFullScreenViewImp::OfferEventL ETypeDoubleTap");   

                MulDoubleTapData* DoubleTapData = (MulDoubleTapData*)(aEvent.CustomEventData()); 
                TPoint doubleTapPoint = DoubleTapData->mDoubleTapPoint;
                TInt focus = iMediaList->FocusIndex();
                TGlxMedia item = iMediaList->Item( focus );
                TSize size;
                item.GetDimensions( size );
                TRect rect = AlfUtil::ScreenSize();
                TInt zoomLevel = GetInitialZoomLevel();
                TInt tlX = (rect.Width() - (size.iWidth*zoomLevel)/100)/2;
                TInt tlY = (rect.Height() - (size.iHeight*zoomLevel)/100)/2;
                TRect imageRect(TPoint(tlX, tlY), TSize((size.iWidth*zoomLevel)/100, (size.iHeight*zoomLevel)/100));
                if (imageRect.Contains(doubleTapPoint))
                    {
                    SetSliderToMin();
                    TRAP_IGNORE( ActivateZoomControlL(EZoomStartDoubleTap, &doubleTapPoint)  );
                    }
                return EEventConsumed;
                }
            case ETypeItemRemoved:
                {
                GLX_LOG_INFO("CGlxFullScreenViewImp::OfferEventL ETypeItemRemoved");
                HandleItemRemovedL();
                return EEventConsumed;
                }
            case ETypeHighlight:
                {
                GLX_LOG_INFO("CGlxFullScreenViewImp::OfferEventL ETypeHighlight");
                iMediaList->SetFocusL( NGlxListDefs::EAbsolute,(aEvent.CustomEventData())); 
                if (AknLayoutUtils::PenEnabled())
                	{
                	iMediaList->SetVisibleWindowIndexL(aEvent.CustomEventData()); 
                	}
                if(iMediaList->Count()> KFullScreenTextureOffset)
                    {
                    RemoveTexture();
                    }
                SetItemToHDMIL();
                if ( EUiOn == GetUiState() )
                    {
                    HideUi(ETrue);
                    }
                TRAP_IGNORE(ShowDrmExpiryNoteL());
                return EEventConsumed;
                }
            case EVideoIconSelect:
                {
                TGlxMedia item = iMediaList->Item( iMediaList->FocusIndex() );
                if(item.Category() == EMPXVideo)
                    {                                 
                    ProcessCommandL(EGlxCmdPlay);
                    iViewWidget->show(false);
                    } 
                else
                    { 
                    if ( EUiOn == GetUiState() )                    
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
                    SetSliderToMin();
                    TRAP_IGNORE( ActivateZoomControlL(EZoomStartPinch));
                    }
                return EEventConsumed; // will be consumed even if we are pinching IN (trying to zoom OUT). 
                //slider event handling   
            case ETypePrimaryValueChange:
                {
                iTimer->Cancel();
                iSliderWidget->ShowWidget( ETrue, 0 );
                TRAPD( err, ActivateZoomControlL(EZoomStartSlider));
                if (err != KErrNone)
                    {
                    iSliderModel->SetPrimaryValue(GetInitialZoomLevel());
                    }
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
		    if (iIsDialogLaunched && iIsMMCRemoved)
		        {
                ProcessCommandL(EAknSoftkeyExit);
		        }
		    consumed = ETrue;
		    iIsDialogLaunched = EFalse;
		    break;
		    }
		case EGlxCmdFullScreenBack:
		    DeactivateZoomControlL();
			//Show the screen furniture when we press back from zoom
		    ShowUiL(ETrue);
		    consumed = ETrue;
		    break;
		case EGlxCmdRenameCompleted:
		    {
            SetItemToHDMIL();
			consumed = ETrue;
            break;
            }
		case EGlxCmdDialogLaunched:
            {
            iIsDialogLaunched = ETrue;
            break;
            }
        case EGlxCmdDialogDismissed:
            {
            if (iIsDialogLaunched && iIsMMCRemoved)
                {
                ProcessCommandL(EAknSoftkeyExit);
                }
            consumed = ETrue;
            iIsDialogLaunched = EFalse;
            break;
            }
		case EAknSoftkeyBack:
            {
            HideUi(ETrue);
            // Enable status pane  and  Set null text 
			StatusPane()->MakeVisible(ETrue);
			SetTitlePaneTextL(KNullDesC);
            break;
            }                        
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
    if (aControl == iZoomControl)
        {
        if (aCommandId == KGlxZoomOutCommand)
            {
            DeactivateZoomControlL();
            }
        else if(aCommandId == KGlxZoomOrientationChange)
            {
            iViewWidget->setRect(TRect(TPoint(0,0),AlfUtil::ScreenSize()));
            }
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
    if (KGlxDecodingThreshold < (size.iWidth * size.iHeight))
        {
        TReal areaRatio = TReal(size.iWidth*size.iHeight)/KGlxDecodingThreshold ;
        
        TReal sideRatio;
        Math::Sqrt(sideRatio, areaRatio);
        
        size.iHeight = size.iHeight /  sideRatio ;
        size.iWidth  = size.iWidth  /  sideRatio ;
        }
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
    
    // A correction of 0.5 (KGlxRealTruncationPadding) is added to the resultant value. 
    // This ensures that all return values greater than X.5 (e.g lets say 12.8) is pegged to X+1(13)
    // instead of X(12) while calculating. 
    // Changing the calculations to real might have been a better idea, 
    // but that involves many changes in many places and was therefore avoided, 
    // maybe a good idea to take up in PS2.
    GLX_LOG_INFO1("CGlxFullScreenViewImp::GetInitialZoomLevel() = %d ", KGlxRealTruncationPadding + initialZoomLevel);
    return KGlxRealTruncationPadding + initialZoomLevel ;
    }

//----------------------------------------------------------------------------------
// SetSliderLevel() Set the Initial Zoom Level for the Image
//----------------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::SetSliderToMin()
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
    TRAP_IGNORE(ShowDrmExpiryNoteL());
    }
	
// ---------------------------------------------------------------------------
// Shows expiry note / Consumes DRM rights for expired DRM files
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::ShowDrmExpiryNoteL()
	{
    TRACER("CGlxFullScreenViewImp::ShowDrmExpiryNoteL");
    if (iMediaList->Count() > 0)
        {
        const TGlxMedia& media = iMediaList->Item(iMediaList->FocusIndex());
        TInt tnError = GlxErrorManager::HasAttributeErrorL(
                media.Properties(), KGlxMediaIdThumbnail);
        GLX_LOG_INFO1("CGlxFullScreenViewImp::ShowDrmExpiryNoteL()"
                " tnError=%d ", tnError);

        if (media.IsDrmProtected())
            {
            GLX_LOG_INFO("CGlxFullScreenViewImp::ShowDrmExpiryNoteL()"
                    "- ConsumeDRMRightsL()");
            //Consume DRM Rights & cancel Periodic timer for DRM images
            //only if Quality TN is available
            ConsumeDRMRightsL(media);
            }
        else
            {
            //Cancel the periodic timer if quality thumbnail is available
            //and no error in fetching quality thumbnail.
            TSize tnSize = iUiUtility->DisplaySize();
            TMPXAttribute qtyTnAttrib = TMPXAttribute(KGlxMediaIdThumbnail,
                    GlxFullThumbnailAttributeId(ETrue, tnSize.iWidth,
                            tnSize.iHeight));
            const CGlxThumbnailAttribute* qtyTn = media.ThumbnailAttribute(
                    qtyTnAttrib);
            if (qtyTn && tnError == KErrNone)
                {
                if (iPeriodic->IsActive())
                    {
                    iPeriodic->Cancel();
                    }
                }
            }
        
        //check if any Error message is to be displayed
        TMPXGeneralCategory cat = media.Category();
        TBool checkViewRights = ETrue;
        if (media.IsDrmProtected())
            {
            GLX_LOG_INFO("CGlxFullScreenViewImp::ShowDrmExpiryNoteL()"
                    "- ItemRightsValidityCheckL()");
            if (iImgViewerMode && iImageViewerInstance->IsPrivate())
                {
                checkViewRights = iDrmUtility->ItemRightsValidityCheckL(
                        iImageViewerInstance->ImageFileHandle(),
                        (media.Category() == EMPXImage));
                }
            else
                {
                //Since it is always for the focused item - use DisplayItemRightsCheckL instead of ItemRightsValidityCheckL
                checkViewRights = iDrmUtility->ItemRightsValidityCheckL(
                        media.Uri(), (media.Category() == EMPXImage));
                }
            }
        if (checkViewRights && tnError != KErrNone)
            {
            if (iPeriodic->IsActive())
                {
                iPeriodic->Cancel();
                }

            if (iImgViewerMode)
                {
                GLX_LOG_INFO1("CGlxFullScreenViewImp::ShowDrmExpiryNoteL()"
                        "- ShowErrorNoteL(%d)", tnError);
                if (tnError == KErrNoMemory || tnError == KErrNotSupported
                        || tnError == KErrInUse || tnError == KErrDiskFull
                        || tnError == KErrTimedOut || tnError
                        == KErrPermissionDenied)
                    {
                    GlxGeneralUiUtilities::ShowErrorNoteL(tnError);
                    }
                else
                    {
                    // Generic "Unable to open image" error note
                    HBufC* str = StringLoader::LoadLC(
                            R_GLX_ERR_FORMAT_UNKNOWN);
                    CAknErrorNote* note = new (ELeave) CAknErrorNote(ETrue);
                    note->ExecuteLD(*str); // ignore return value, not used
                    CleanupStack::PopAndDestroy(str);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Consumes DRM rights / Shows DRM rights (if expired) information 
// for DRM files
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::ConsumeDRMRightsL(const TGlxMedia& aMedia)
    {
    TRACER("CGlxFullScreenViewImp::ConsumeDRMRightsL");
    const TDesC& uri = aMedia.Uri();
    if (uri.Length() > 0)
        {
        // check if rights have expired
        TBool expired = EFalse;
        if (iImgViewerMode && iImageViewerInstance->IsPrivate())
            {
            expired = !iDrmUtility->ItemRightsValidityCheckL(
                    iImageViewerInstance->ImageFileHandle(),
                    ETrue);
            }
        else
            {
            expired = !iDrmUtility->ItemRightsValidityCheckL(uri, ETrue);
            }

        if (expired)
            {
            if (iPeriodic->IsActive())
                {
                iPeriodic->Cancel();
                }
            iBusyIcon->ShowBusyIconL(EFalse);
            if(iImgViewerMode && iImageViewerInstance->IsPrivate())
                {
                iDrmUtility->ShowRightsInfoL(iImageViewerInstance->ImageFileHandle());
                }
            else
                {
                iDrmUtility->ShowRightsInfoL(uri);
                }
            return;
            }

        TSize tnSize = iUiUtility->DisplaySize();
        TMPXAttribute qtyTnAttrib = TMPXAttribute(KGlxMediaIdThumbnail,
                GlxFullThumbnailAttributeId(ETrue, tnSize.iWidth,
                        tnSize.iHeight));
        const CGlxThumbnailAttribute* qtyTn = aMedia.ThumbnailAttribute(
                qtyTnAttrib);

        TInt tnError = GlxErrorManager::HasAttributeErrorL(
                aMedia.Properties(), KGlxMediaIdThumbnail);
        GLX_LOG_INFO1("CGlxFullScreenViewImp::ConsumeDRMRightsL()"
                " tnError=%d ", tnError);
        if (qtyTn && tnError == KErrNone)
            {
            if (iPeriodic->IsActive())
                {
                iPeriodic->Cancel();
                }
            GLX_LOG_INFO("CGlxFullScreenViewImp::ConsumeDRMRightsL()"
                    "- ConsumeRights");
            if(iImgViewerMode && iImageViewerInstance->IsPrivate())
                {
                iDrmUtility->ConsumeRightsL(iImageViewerInstance->ImageFileHandle());
                }
            else
                {
                iDrmUtility->ConsumeRightsL(uri);
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
    GLX_LOG_INFO1("CGlxFullScreenViewImp::GetSwipeDirection()"
            " Direction = [%d] ", swipedirection);

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

// ---------------------------------------------------------------------------
// 
// Set the focused item to external display - HDMI
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::SetItemToHDMIL()
    {
    TRACER("CGlxFullScreenViewImp::SetItemToHDMIL()");

    TInt focusIndex = iMediaList->FocusIndex();

    // If we dont know what item we are focussing on 
    // or if our medialist is empty
    // or if there is no HDMI Controller at all 
    // then dont SetItemToHDMI :)  
    if ((KErrNotFound == focusIndex) || (0 == iMediaList->Count()) || (NULL
            == iHdmiController))
        {
        GLX_LOG_INFO("CGlxFullScreenViewImp::SetItemToHDMIL - Cant Set Image To HDMI");
        return;
        }
    
    TGlxMedia item = iMediaList->Item(focusIndex);
    TInt error = GlxErrorManager::HasAttributeErrorL(item.Properties(),
            KGlxMediaIdThumbnail);
    GLX_LOG_INFO1("CGlxFullScreenViewImp::SetItemToHDMIL - error=%d", error);

    TBool canView = ETrue;
    if (item.IsDrmProtected())
        {
        canView = iDrmUtility->ItemRightsValidityCheckL(item.Uri(),
                (item.Category() == EMPXImage));
        }
    GLX_LOG_INFO1("CGlxFullScreenViewImp::SetItemToHDMIL - canView=%d", canView);        
    
    // Item will be supported by HDMI ONLY if
    // it is not a video
    // and it is DRM protected and has valid DRM Viewing rights
    // and it has no attribute error 
    if ((item.Category() != EMPXVideo) && canView && (error == KErrNone))
        {
        GLX_LOG_INFO("CGlxFullScreenViewImp::SetItemToHDMIL - Fetch FS thumbnail");
        TMPXAttribute fsTnAttrib = TMPXAttribute(KGlxMediaIdThumbnail,
                GlxFullThumbnailAttributeId(ETrue, iScrnSize.iWidth,
                        iScrnSize.iHeight));
        const CGlxThumbnailAttribute* fsValue = item.ThumbnailAttribute(
                fsTnAttrib);
        if (fsValue)
            {
            GLX_LOG_INFO("CGlxFullScreenViewImp::SetItemToHDMIL - Setting FS Bitmap");
            CFbsBitmap* fsBitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(fsBitmap);
            fsBitmap->Duplicate( fsValue->iBitmap->Handle());
            
            GLX_LOG_INFO2("CGlxFullScreenViewImp::SetItemToHDMIL - FS Bitmap Size width=%d, height=%d", 
                    fsBitmap->SizeInPixels().iWidth, fsBitmap->SizeInPixels().iHeight);
            iHdmiController->SetImageL(item.Uri(), KNullDesC, fsBitmap);
            CleanupStack::PopAndDestroy(fsBitmap);
            }
        else
            {
            GLX_LOG_INFO("CGlxFullScreenViewImp::SetItemToHDMIL - Fetch Grid thumbnail");
            TMPXAttribute gridTnAttrib = TMPXAttribute(KGlxMediaIdThumbnail,
                    GlxFullThumbnailAttributeId(ETrue, iGridIconSize.iWidth,
                            iGridIconSize.iHeight));
            const CGlxThumbnailAttribute* gridvalue = item.ThumbnailAttribute(
                    gridTnAttrib);

            if (gridvalue)
                {
                GLX_LOG_INFO("CGlxFullScreenViewImp::SetItemToHDMIL - Setting Grid Bitmap");
                CFbsBitmap* gridBitmap = new (ELeave) CFbsBitmap;
                CleanupStack::PushL(gridBitmap);
                gridBitmap->Duplicate( gridvalue->iBitmap->Handle());
                
                GLX_LOG_INFO2("CGlxFullScreenViewImp::SetItemToHDMIL - gridBitmap Size width=%d, height=%d", 
                        gridBitmap->SizeInPixels().iWidth, gridBitmap->SizeInPixels().iHeight);
                iHdmiController->SetImageL(item.Uri(), KNullDesC, gridBitmap);
                CleanupStack::PopAndDestroy(gridBitmap);
                }
            else
                {
                GLX_LOG_INFO("CGlxFullScreenViewImp::SetItemToHDMIL - Setting Default Bitmap");
                TFileName resFile(KDC_APP_BITMAP_DIR);
                resFile.Append(KGlxIconsFilename);
                CFbsBitmap* defaultBitmap = AknIconUtils::CreateIconL(resFile,
                        EMbmGlxiconsQgn_prop_image_notcreated);
                CleanupStack::PushL(defaultBitmap);
                
                // always need to setsize on the raw bitmap for it to be visible
                AknIconUtils::SetSize(defaultBitmap, TSize(iHdmiWidth,iHdmiHeight),EAspectRatioPreserved);

                GLX_LOG_INFO2("CGlxFullScreenViewImp::SetItemToHDMIL - Default Size width=%d, height=%d", 
                        defaultBitmap->SizeInPixels().iWidth, defaultBitmap->SizeInPixels().iHeight);
                iHdmiController->SetImageL(item.Uri(), KNullDesC, defaultBitmap);
                CleanupStack::PopAndDestroy(defaultBitmap); 
                }
            }
        }
    else
        {
        GLX_LOG_INFO("CGlxFullScreenViewImp::SetItemToHDMIL - Unsupported Item");
        //Set the external display to cloning mode if
        //the current item is something we dont support 
        //(e.g. video, corrupted item, item with invalid DRM)
        iHdmiController->ItemNotSupported();
        }
    
    iOldFocusIndex = iMediaList->FocusIndex();
    delete iUri;
    iUri = NULL;   
    iUri = item.Uri().AllocL();
    }

// ---------------------------------------------------------------------------
// HandleMMCInsertionL
// 
// ---------------------------------------------------------------------------
void CGlxFullScreenViewImp::HandleMMCInsertionL()
    {
    TRACER("CGlxFullScreenViewImp::HandleMMCInsertionL()");
    iMMCState = ETrue;
    NavigateToMainListL();
    }
	
// ---------------------------------------------------------------------------
// NavigateToMainListL
// 
// ---------------------------------------------------------------------------
void CGlxFullScreenViewImp::NavigateToMainListL()
    {
    TRACER("CGlxFullScreenViewImp::NavigateToMainListL()");
    if (!iImgViewerMode)
        {
        if (iZoomControl && iZoomControl->Activated())
            {
            DeactivateZoomControlL();
            }
        ProcessCommandL( EAknSoftkeyClose);
        }
    }
	
// ---------------------------------------------------------------------------
// HandleMMCRemovalL
// 
// ---------------------------------------------------------------------------
void CGlxFullScreenViewImp::HandleMMCRemovalL()
    {
    TRACER("CGlxFullScreenViewImp::HandleMMCRemovalL()");
    iIsMMCRemoved = ETrue;
    if(!iIsDialogLaunched)
        {
        if(iZoomControl && iZoomControl->Activated())
            {
            DeactivateZoomControlL();   
            }
        ProcessCommandL(EAknSoftkeyExit);
        }
    }
	
// ---------------------------------------------------------------------------
// EnableFSToolbar
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::EnableFSToolbar(TBool aEnable)
    {
	TRACER("CGlxFullScreenViewImp::EnableFSToolbar()");
    CAknToolbar* toolbar = Toolbar();
    if(toolbar)
        {
        TRAP_IGNORE(toolbar->DisableToolbarL(!aEnable));
        toolbar->SetToolbarVisibility(aEnable); 
        }
    }
	
// ---------------------------------------------------------------------------
// HandleEffectCallback
// 
// ---------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::HandleEffectCallback(TInt aType, TInt aHandle, 
                                                 TInt /*aStatus*/)
    {
    TRACER("CGlxFullScreenViewImp::HandleEffectCallback()");
    if (aHandle == iEffectHandle && aType == EAlfEffectComplete
            && iCoverFlowWidget)
        {
        TRAP_IGNORE(const_cast<CAlfLayout&>
                (iCoverFlowWidget->ContainerLayout()).SetEffectL(
                        KTfxResourceNoEffect));
        }
    }

// -----------------------------------------------------------------------------
// CreateImageViewerInstanceL
// -----------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::CreateImageViewerInstanceL()
    {
    TRACER("CGlxFullScreenViewImp::CreateImageViewerInstanceL");
    iImageViewerInstance = CGlxImageViewerManager::InstanceL();    
    __ASSERT_ALWAYS(iImageViewerInstance, Panic(EGlxPanicNullPointer));
    }

// -----------------------------------------------------------------------------
// DeleteImageViewerInstance
// -----------------------------------------------------------------------------
//
void CGlxFullScreenViewImp::DeleteImageViewerInstance()
    {
    TRACER("CGlxFullScreenViewImp::DeleteImageViewerInstance");
    if ( iImageViewerInstance )
        {
        iImageViewerInstance->DeleteInstance();
        }
    }
	
// -----------------------------------------------------------------------------
// HandleTvStatusChangedL 
// -----------------------------------------------------------------------------
void CGlxFullScreenViewImp::HandleTvStatusChangedL( TTvChangeType aChangeType )
    {
    TRACER("CGlxFullScreenViewImp::HandleTvStatusChangedL()");
    GLX_LOG_INFO1("CGlxFullScreenViewImp::HandleTvStatusChangedL(%d)",
            aChangeType);

    if (aChangeType == ETvConnectionChanged)
        {
        // Hide UI furnitures when HDMI cable is connected/Disconnected
        // irrespective of UI state on/off.
        HideUi(ETrue);
        }
    }

// ---------------------------------------------------------------------------
// CheckIfSliderToBeShownL
// ---------------------------------------------------------------------------
TBool CGlxFullScreenViewImp::CheckIfSliderToBeShownL()
    {
    TRACER("CGlxFullScreenViewImp::CheckIfSliderToBeShownL()");

    TInt index = iMediaList->FocusIndex();
    const TGlxMedia& item = iMediaList->Item(index);
    TInt error = GlxErrorManager::HasAttributeErrorL(item.Properties(),
            KGlxMediaIdThumbnail);
       
    TBool isDrmRightsValid = ETrue;
	if(item.IsDrmProtected())
		{
        if (iImgViewerMode && iImageViewerInstance->IsPrivate())
            {
            isDrmRightsValid = iDrmUtility->DisplayItemRightsCheckL(
                    iImageViewerInstance->ImageFileHandle(), (item.Category()
                            == EMPXImage));
            }
        else
            {
            //Since it is always for the focused item - use DisplayItemRightsCheckL instead of ItemRightsValidityCheckL
            isDrmRightsValid = iDrmUtility->DisplayItemRightsCheckL(
                    item.Uri(), (item.Category() == EMPXImage));
            }
        }
	
    // Display slider only for non corrupted images and items with valid DRM license
	if (iHdmiController && iHdmiController->IsHDMIConnected())
        {
        //hide slider if UI is needed to be on and HDMI is Connected
		return EFalse;
        }
    else if(error == KErrNone && item.Category() == EMPXImage
            && isDrmRightsValid )
        {
        return ETrue;
        }
    return EFalse;
    }

//---------------------------------------------------------------------------
// HandleHDMIDecodingEventL
//---------------------------------------------------------------------------   
void CGlxFullScreenViewImp::HandleHDMIDecodingEventL(
        THdmiDecodingStatus /*aStatus*/)
    {
    TRACER("CGlxFullScreenViewImp::HandleHDMIDecodingEventL()");
    }

// ---------------------------------------------------------------------------
// UpdateItems
// ---------------------------------------------------------------------------
void CGlxFullScreenViewImp::UpdateItems()
    {
    TRACER("CGlxFullScreenViewImp::UpdateItems()");
    TInt focusIndex = iMediaList->FocusIndex();
    TInt count = iMediaList->Count();
    GLX_LOG_INFO2("CGlxFullScreenViewImp::UpdateItems()"
            " focusIndex(%d), count(%d)", focusIndex, count);
    
    if (focusIndex != KErrNotFound && count)
        {        
        // update the focus index first
		iMediaListMulModelProvider->UpdateItems(focusIndex, 1);

        TInt startIndex = focusIndex;
        TInt iteratorCount = (KTextureOffset > count) ? 
                                count : KTextureOffset;
        TInt textureCount = (KFullScreenTextureOffset > count)?
                                count : KFullScreenTextureOffset;

        startIndex = focusIndex - iteratorCount;
        if (startIndex < 0)
            {
            startIndex = count + startIndex;
            }

        TInt i = 0;
        //Update all the textures in the window
        while (i < textureCount)
            {
            if (startIndex != focusIndex)
                {
                GLX_LOG_INFO1("CGlxFullScreenViewImp::UpdateItems(%d)", startIndex);
                iMediaListMulModelProvider->UpdateItems(startIndex, 1);
                }
            if (++startIndex == count)
                {
                startIndex = 0;
                }
            i++;
            }
        
        // When the focused index is first or last, 
        // need to update the items adjacent to KFullScreenIterator also!
        if (count > (KFullScreenTextureOffset))
            {
            TInt lastIndex = count - 1;
            if (focusIndex == 0)
                {
                iMediaListMulModelProvider->UpdateItems(KFullScreenIterator,
                        1);
                }
            else if (focusIndex == lastIndex)
                {
                iMediaListMulModelProvider->UpdateItems(lastIndex
                        - KFullScreenIterator, 1);
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Sets the title pane text
// ---------------------------------------------------------------------------
void CGlxFullScreenViewImp::SetTitlePaneTextL(const TDesC& aTitleText)
	{
    TRACER("CGlxFullScreenViewImp::SetTitlePaneTextL()");
    CAknViewAppUi* appui = AppUi();
    if (appui)
        {
        CAknTitlePane* titlePane =
                (CAknTitlePane*) appui->StatusPane()->ControlL(TUid::Uid(
                        EEikStatusPaneUidTitle));
        titlePane->SetTextL(aTitleText);
        titlePane->DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL
// ---------------------------------------------------------------------------
void CGlxFullScreenViewImp::HandleItemRemovedL()
    {
    TRACER("CGlxFullScreenViewImp::HandleItemRemovedL()");
    TInt focusIndex = iMediaList->FocusIndex();
    TInt mlCount = iMediaList->Count();
    GLX_LOG_INFO2("CGlxFullScreenViewImp::HandleItemRemovedL focusIndex=%d, iOldFocusIndex=%d",
            focusIndex, iOldFocusIndex);
    // When photos is in background, the Following scenario could happen,
    // 1) First item is deleted => iOldFocusIndex == focusIndex (or)
    // 2) Last item is deleted => iOldFocusIndex == mlCount (or)
    // 3) New item is added and focused item is deleted => iOldFocusIndex != focusIndex
    if (mlCount && (iOldFocusIndex == focusIndex || iOldFocusIndex == mlCount
            || iOldFocusIndex != focusIndex) && iZoomControl
            && iZoomControl->Activated())
        {
        GLX_LOG_INFO("Focused item is removed, Exit zoom view!");
        DeactivateZoomControlL();
        }
    SetItemToHDMIL();
    if (focusIndex != KErrNotFound && EUiOn == GetUiState())
        {
        // show/hide the slider
        if (iSliderWidget)
            {
            iSliderWidget->ShowWidget(CheckIfSliderToBeShownL());
            }
        }
    /** if this is the last image deleted when Photo is in foreground, go back to the previous view*/
    if (mlCount == 0 && IsForeground() && iNaviState->ViewingMode()
            == NGlxNavigationalState::EView)
        {
        iUiUtility->SetViewNavigationDirection(EGlxNavigationBackwards);
        iNaviState->ActivatePreviousViewL();
        }
    TRAP_IGNORE(ShowDrmExpiryNoteL());
    }

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
* Description:    Implementation of Cloud view
 *
*/




// INCLUDE FILES
#include "glxcloudviewimp.h" //header file

//ALF headers
#include <alf/alfcontrolgroup.h> //for Alfcontrolgroup
#include <alf/alfwidgetcontrol.h> //for alfwidget controls
#include <alf/alfgridlayout.h> // For CAlfGridLayout
#include <alf/ialflayoutmanager.h> // For CAlfLAyoutManager
#include <alf/ialfscrollbarmodel.h> // For CAlfScrollBarModel
#include <alf/ialfscrollbardefaultbaseelement.h> // For CAlfScrollBarDefaultBaseelement
#include <alf/alfwidgetenvextension.h>
#include <glxuiutility.h>
#include <glxresourceutilities.h>  // for CGlxResourceUtilities
#include <glxcollectionplugintags.hrh>
#include <glxfiltergeneraldefs.h>

#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <glxtracer.h>	
#include <glxlog.h>					// For Logs
#include <glxsetappstate.h> // For view state
#include <mglxmedialist.h> //medialist
#include <data_caging_path_literals.hrh> // For directory paths 

#include <aknlayoutscalable_uiaccel.cdl.h>
#include <glxtagsbrowserview.rsg>               // For resources

#include "glxcloudviewcontrol.h"//cloud view control

// For transition effects
#include <akntranseffect.h>                             
#include <gfxtranseffect/gfxtranseffect.h>
#include "glxgfxtranseffect.h"  // For transition effects
#include <akntransitionutils.h> //For CAknTransitionUtils

const TInt KViewId = 0x200071B7;

//Transition animation used for Cloud view activation
_LIT( KTfxResourceActivate , "z:\\resource\\effects\\photos_gridview_appear.fxml");
_LIT( KTfxResourceNoEffect, "");

using namespace Alf;
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
// 
EXPORT_C CGlxCloudViewImp *CGlxCloudViewImp::NewL(MGlxMediaListFactory
		*aMediaListFactory, const TDesC &aFileName, TInt aViewResId, 
		TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId)
	{
	TRACER("GLX_CLOUD::CGlxCloudViewImp::NewL");
	CGlxCloudViewImp *self = CGlxCloudViewImp::NewLC(aMediaListFactory,
		aFileName, aViewResId, aEmptyListResId, aSoftKeyResId
		, aSoftkeyMskDisabledId);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCloudViewImp *CGlxCloudViewImp::NewLC(MGlxMediaListFactory
		*aMediaListFactory, const TDesC &aFileName, TInt aViewResId,
		TInt aEmptyListResId, TInt aSoftKeyResId, TInt aSoftkeyMskDisabledId)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewImp::NewLC");
    CGlxCloudViewImp *self = new(ELeave)CGlxCloudViewImp(aSoftKeyResId,aSoftkeyMskDisabledId);
    CleanupStack::PushL(self);
    self->ConstructL(aMediaListFactory, aFileName, aViewResId, aEmptyListResId, aSoftKeyResId);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCloudViewImp::CGlxCloudViewImp(TInt aSoftKeyResId,
		TInt aSoftkeyMskDisabledId) :
	         iSoftKeyResId(aSoftKeyResId), iSoftkeyMskDisabledId(aSoftkeyMskDisabledId)
	{
	TRACER("GLX_CLOUD::CGlxCloudViewImp::CGlxCloudViewImp");
	}

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CGlxCloudViewImp::ConstructL(MGlxMediaListFactory *aMediaListFactory,
		const TDesC &aFileName, TInt aViewResId, TInt aEmptyListResId,
		TInt /*aSoftKeyResId*/)

	{
	TRACER("GLX_CLOUD::CGlxCloudViewImp::ConstructL");
	
	TFileName resourceFile(KDC_APP_RESOURCE_DIR);
	resourceFile.Append (aFileName);
	CGlxResourceUtilities::GetResourceFilenameL (resourceFile);
	iResourceOffset = iCoeEnv->AddResourceFileL (resourceFile);
	BaseConstructL (aViewResId);
	ViewBaseConstructL ();
	MLViewBaseConstructL (aMediaListFactory);
	
	//Register the view to recieve toolbar events. ViewBase handles the events    
	SetToolbarObserver(this);
	ShowToolbarOnViewActivation(ETrue);
	
	iEmptyListText = iEikonEnv->AllocReadResourceL(aEmptyListResId);
	iDisplay = iUiUtility->Display ();
	CAknToolbar* toolbar = Toolbar();
	if(toolbar)
        {
        toolbar->DisableToolbarL(ETrue);  
        }
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxCloudViewImp::~CGlxCloudViewImp()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewImp::~CGlxCloudViewImp");
    if(iAlfEffectObs)
		{
		delete iAlfEffectObs;
		}  
    
    CleanupVisuals ();
    delete iEmptyListText;
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile (iResourceOffset);
        }    
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CGlxCloudViewImp::Id()const
	{
	TRACER("GLX_CLOUD::CGlxCloudViewImp::Id()");
	return TUid::Uid (KViewId);
	}

// ---------------------------------------------------------------------------
// HandleViewCommandL
// ---------------------------------------------------------------------------
//
TBool CGlxCloudViewImp::HandleViewCommandL(TInt aCommand)
	{
	TRACER("GLX_CLOUD::CGlxCloudViewImp::HandleViewCommandL");
	
	TBool consumed = EFalse;
	switch (aCommand)
		{
		case EAknCmdOpen:
		case EKeyEnter:
			{
            // Processing of this Command depends on Medialist Count ( > 0 tags Available) 
            if (iMediaList->Count())
                {
                // There can be no selection in cloud view, so assume that path contains focused item
                iUiUtility->SetViewNavigationDirection(EGlxNavigationForwards);
                CMPXCollectionPath* path = iMediaList->PathLC(
                        NGlxListDefs::EPathFocusOrSelection);
                // When a collection is opened for browsing, 
                // there are two queries executed with similar filter. 
                // First query to open the collection from list / cloud view.
                // Second one from grid view construction. To improve the grid opening
                // performance, the first query will be completed with empty Id list.
                RArray<TMPXAttribute> attributeArray;
                CleanupClosePushL(attributeArray);
                attributeArray.AppendL(KGlxFilterGeneralNavigationalStateOnly);
                iCollectionUtility->Collection().OpenL(*path,
                        attributeArray.Array());
                CleanupStack::PopAndDestroy(&attributeArray);
                CleanupStack::PopAndDestroy(path);
                consumed = ETrue;
                }
            break;
            }
		}
		
	return consumed;
	}


// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::DoMLViewActivateL(const TVwsViewId & /* aPrevViewId */,
		TUid /* aCustomMessageId */, const TDesC8 & /* aCustomMessage */)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewImp::DoMLViewActivateL");
        
    if(StatusPane())
        {
        StatusPane()->MakeVisible(ETrue);
        }
    
    ConstructCloudControlL();
    GLX_LOG_INFO("CGlxCloudViewImp::DoMLViewActivateL Cloud View Control Created" );  
    
    // set app state to tag-browser view
    GlxSetAppState::SetState (EGlxInTagBrowserView);
    iMMCNotifier = CGlxMMCNotifier::NewL(*this);
    
    //Set the ALF animation effect to CAlfAnchorLayout since the animation
    //does not work for both avkon and alf together.
    //Check if the transitions are enabled from themes
    if (CAknTransitionUtils::TransitionsEnabled( AknTransEffect::EFullScreenTransitionsOff ))
		{
		if(!iAlfEffectObs)
			{
			iAlfEffectObs = CAlfEffectObserver::NewL();
			}
		
		iAnchorlayout->SetEffectL( KTfxResourceActivate );
		iEffectHandle = iAnchorlayout->Identifier();
		iAlfEffectObs->SubscribeCallbackL(this,iEffectHandle);
		}

    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::DoMLViewDeactivate()
	{
	TRACER("GLX_CLOUD::CGlxCloudViewImp::DoMLViewDeactivate");
	
	delete iMMCNotifier;
	iMMCNotifier = NULL;
	
	//Hide softkeys and toolbar upon view de-activation.
	iViewWidget->enableControlPane(EFalse);
	if ( EGlxNavigationForwards == iUiUtility->ViewNavigationDirection() )
        {
        iViewWidget->enableStatusPane(EFalse);    
        }
	CleanupVisuals ();
	}

// ---------------------------------------------------------------------------
// HandleMskChangedL().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::HandleMskChangedL(TBool aMskEnabled)
    {
    TRACER("GLX_CLOUD ::CGlxCloudViewImp::HandleMskChangedL");
    GLX_LOG_INFO1("GLX_CLOUD ::GLX_CLOUD ::CGlxCloudViewImp::HandleMskChangedL MSk State  %d ",
        aMskEnabled);
	 }

// ---------------------------------------------------------------------------
// HandleLayoutChanged().
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::HandleLayoutChanged()
    {	
    iViewWidget->setRect( ClientRect() );
    SetScrollBarRect();   
    }

// --------------------------------------------------------------------------- 
//ControlGroupId()
// ---------------------------------------------------------------------------
//
TInt CGlxCloudViewImp::ControlGroupId()const
	{
	TRACER("GLX_CLOUD::CGlxCloudViewImp::ControlGroupId");
	return reinterpret_cast < TInt > (this);
	}
	
// ---------------------------------------------------------------------------
// CleanupVisuals
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::CleanupVisuals()
    {
    TRACER("GLX_CLOUD::CGlxCloudViewImp::CleanupVisuals()");
    if( iViewWidget )
        {        
        iViewWidget->show(false); 
        }  

    if(iScrollWidget)
        {
        iScrollWidget->control()->removeEventHandler(*iCloudControl);
        iScrollWidget =  NULL;
        }
    if( iUiUtility )
        {
		IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*(iUiUtility->Env ())); 
        widgetFactory.destroyWidget(iViewWidget);
        }
    iViewWidget = NULL; 
    iScrollbarDefaultBaseElement = NULL;
    }

// ---------------------------------------------------------------------------
// HandleEnterKeyEventL
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::HandleEnterKeyEventL(TInt aCommand)
    {
    HandleViewCommandL(aCommand);
    }

// ---------------------------------------------------------------------------
// HandleEnterKeyEventL
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::ConstructCloudControlL()
    {
   	const char* KScrollBarWidget("scrollbarwidget");
	const char* KVerScrollbarWidgetName("alfscrollbarwidget_ver");
	const char* KScrollBarModel(/*"mulmodel"*/"scrollbarmodel");

	IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*(iUiUtility->Env ())); 

    iViewWidget = widgetFactory.createViewWidget("viewwidget", 0, 0, iDisplay);
	iViewWidget->setRect(ClientRect());
	iViewWidget->show(true);

	IAlfLayoutManager* layoutmanager = IAlfInterfaceBase::makeInterface<
			IAlfLayoutManager>(iViewWidget->control());
	iViewWidget->setRect(ClientRect());
   
    // parent layout handle for scrollbar
    iScrollPaneHandle = AknLayoutScalable_UiAccel::aa_scroll_pane(0).LayoutLine();

    iAnchorlayout = &((CAlfAnchorLayout&)layoutmanager->getLayout());

    iAnchorlayout->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
    iAnchorlayout->SetFlag(EAlfVisualFlagAutomaticLocaleMirroringEnabled);
    
    iCloudControl = CGlxCloudViewControl::NewL (*iDisplay, *iUiUtility->Env (), *iMediaList,
               *iEmptyListText, *this,*this,iAnchorlayout,*this,*this); 
    
    CAlfControlGroup * ctrlGroup = iUiUtility->Env ()->FindControlGroup(0);
    if(ctrlGroup)
        {
        ctrlGroup->AppendL(iCloudControl);
        }
   iScrollWidget = widgetFactory.createWidget<IAlfScrollBarWidget>
    	( KScrollBarWidget, KVerScrollbarWidgetName, *iViewWidget, 0 );
    	
    IAlfScrollBarModel* verScrollModel = widgetFactory.createModel<IAlfScrollBarModel> (KScrollBarModel);
        
    iScrollWidget->setModel( verScrollModel );
    
    IAlfElement* vertBaseElement =(iScrollWidget->control()->findElement ("BaseElement"));

    iScrollbarDefaultBaseElement=static_cast<IAlfScrollBarDefaultBaseElement*> (
        vertBaseElement->makeInterface (IAlfScrollBarDefaultBaseElement::type() ) );
  
	if(iScrollWidget) 
		{		
        if(iScrollbarDefaultBaseElement)
	        {
			iCloudControl->InitializeScrollBar(iScrollWidget);    
	        }
        (iScrollWidget->control())->addEventHandler (iCloudControl);
		}		

    SetScrollBarRect();
    
     //acquire the focus so as to get events to your control instead of widgets
    iCloudControl->AcquireFocus();
    }
	
// ---------------------------------------------------------------------------
// SetScrollBarRect()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::SetScrollBarRect()
    {
    TRect rct  = ClientRect();
    rct.iBr.iX = rct.iBr.iX - iScrollPaneHandle.iW;
    // anchor points for cloud control
    iAnchorlayout->SetAnchor(
            EAlfAnchorTopLeft, 
            0,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint(0,0));

    iAnchorlayout->SetAnchor(
            EAlfAnchorBottomRight, 
            0,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint(ClientRect().iBr.iX - iScrollPaneHandle.iW,(ClientRect().iBr.iY  - ClientRect().iTl.iY)));

    // anchor points for scrollbar widget
    iAnchorlayout->SetAnchor(
            EAlfAnchorTopLeft, 
            1,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint(ClientRect().iBr.iX - iScrollPaneHandle.iW,0));

    iAnchorlayout->SetAnchor(
            EAlfAnchorBottomRight, 
            1,
            EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop,
            EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute,
            TAlfTimedPoint(ClientRect().iBr.iX ,(ClientRect().iBr.iY  - ClientRect().iTl.iY)));

    iAnchorlayout->UpdateChildrenLayout(0);
    }
    
// ---------------------------------------------------------------------------
// HandleGridMenuListL()
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::HandleGridMenuListL(TInt aCommand)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewImp::HandleGridMenuListL");
    iCloudControl->ShowContextItemMenuL(EFalse);
    if (aCommand)
        {
        ProcessCommandL(aCommand);
        }
    }

// ----------------------------------------------------------------------------
// ViewDynInitMenuPaneL
// ----------------------------------------------------------------------------
// 
void CGlxCloudViewImp::ViewDynInitMenuPaneL(TInt aMenuId, CEikMenuPane* /*aMenuPane*/)
    {
    TRACER("GLX_CLOUD::CGlxCloudViewImp::ViewDynInitMenuPaneL");
    if( aMenuId == R_TAGSBROWSER_MENU)
        {
        iCloudControl->ShowContextItemMenuL(EFalse);
        }
    }

// ---------------------------------------------------------------------------
// HandleMMCInsertionL
// 
// ---------------------------------------------------------------------------
void CGlxCloudViewImp::HandleMMCInsertionL()
    {
    TRACER("CGlxCloudViewImp::HandleMMCInsertionL()");
    iMMCState = ETrue;
    ProcessCommandL(EAknSoftkeyClose);
    }

// ---------------------------------------------------------------------------
// HandleMMCRemovalL
// 
// ---------------------------------------------------------------------------
void CGlxCloudViewImp::HandleMMCRemovalL()
    {
    TRACER("CGlxCloudViewImp::HandleMMCRemovalL()");
    ProcessCommandL(EAknSoftkeyExit);
    }

// ---------------------------------------------------------------------------
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::HandleForegroundEventL(TBool aForeground)
    {
    TRACER("CGlxCloudViewImp::HandleForegroundEventL");
    CAknView::HandleForegroundEventL(aForeground);
    if(iMMCState)
        {
        iMMCState = EFalse;
        ProcessCommandL(EAknSoftkeyClose);
        }
    }

// ---------------------------------------------------------------------------
// HandleEffectCallback
// 
// ---------------------------------------------------------------------------
//
void CGlxCloudViewImp::HandleEffectCallback(TInt aType, TInt aHandle, TInt /*aStatus*/)
    {
    TRACER("CGlxCloudViewImp::HandleEffectCallback()");
    if (aHandle == iEffectHandle && aType == EAlfEffectComplete
			&& iAnchorlayout)
        {
        TRAP_IGNORE(iAnchorlayout->SetEffectL(KTfxResourceNoEffect));
        }
    }


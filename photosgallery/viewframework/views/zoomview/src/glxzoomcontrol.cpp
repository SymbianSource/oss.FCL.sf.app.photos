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
* Description:    Implementation of CGlxZoomControl
*
*/


// INCLUDE FILES

#include "glxzoomcontrol.h"

// INCLUDES
#include <data_caging_path_literals.hrh>
#include <eikbtgpc.h>                       //For CEikButtonGroupContainer
#include <ExifRead.h>

//Alf Headers
#include <alf/alfviewportlayout.h>          //  For CAlfViewPortLayout
#include <alf/alfimagevisual.h>             //  For CAlfImageVisual
#include <alf/alfcontrolgroup.h>            //  For CAlfControlGroup
#include <alf/alfdisplay.h>                 //  For CAlfDisplay
#include <alf/alfroster.h>                  //  For CAlfRoster
#include <alf/alfutil.h>                    //  For AlfUtil
// Antariksh
#include <mul/imulsliderwidget.h>           //  For Slider Widget
#include <mul/mulevent.h>                   //  For Slider events

#include <gesturehelper.h>
#include <e32math.h>

using namespace GestureHelper;

//Gallery Headers
#include <glxuiutility.h>
#include <glxattributecontext.h>
#include <glxtracer.h>                      //  For Tracer
#include <glxlog.h>                         //  For Log
#include <glxtexturemanager.h>              //  For Creating Texture
#include <glxuistd.h>                       //  For Fetch Context Priority
#include <mglxmedialist.h>                  //  For MGlxMediaList
#include <mglxuicommandhandler.h>           //  For MGlxUiCommandHandler

#include "glxtv.h"                          // for CGlxTv
// LOCAL CONSTANTS AND MACROS
const TReal KGlxOpacityOpaque = 1.0;
const TInt KGlxMaxExifSize = 0x10000;   
const TReal KGlxOpacityTransparent = 0.0;
//zoom delay for animation while hdmi cable,
//is connected and zoom is initiated
const TInt KZoomDelay = 150000; 
//Zoom level for the animation , assuming the innitial level is 1.
const TReal KGlxZoomLevel = 1.5;
const TInt KGlxMinSmallImageZoomLevel =100;

const TInt KGlxDecodingThreshold = 3000000; // pixels


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlxZoomControl::CGlxZoomControl
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGlxZoomControl::CGlxZoomControl(MGlxUiCommandHandler& aCommandHandler,
        MGlxMediaList& aMediaList,CEikButtonGroupContainer& aZoomBackKeys,
        IMulSliderWidget& aSliderWidget, GestureHelper::CGestureHelper* aGestureHelper) :
        iMediaList(aMediaList), iGestureHelper(aGestureHelper),
        iZoomSliderWidget(aSliderWidget), iZoomBackKey(&aZoomBackKeys), 
        iCommandHandler(aCommandHandler)
    {
    TRACER("CGlxZoomControl::CGlxZoomControl");
    }

// -----------------------------------------------------------------------------
// CGlxZoomControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::ConstructL()
    {
    TRACER("CGlxZoomControl::ConstructL()");
    iAttributeContext = CGlxDefaultAttributeContext::NewL();
    iAttributeContext->AddAttributeL(KGlxMediaGeneralDimensions);
    iMediaList.AddContextL( iAttributeContext, KGlxFetchContextPriorityLow );
    iUiUtility = CGlxUiUtility::UtilityL();
    iEnv = iUiUtility->Env();
    iDisplay = iUiUtility->Display();
    iTextureMgr = &iUiUtility->GlxTextureManager();
    iScreenSize = ScreenSize();

    iDisplay->SetVisibleArea(TRect(TPoint(0,0),iScreenSize));
    CAlfControl::ConstructL(*iEnv);
    
    //Func creating black background and visual that has to zoomed     
    CreateZoomVisualL();

    iControlGroup = &iEnv->NewControlGroupL(0x119);
    iControlGroup->AppendL(this);
    
    iDisplay->Roster().ShowL( *iControlGroup);

    // Hide the Zoom at the construction
    ShowZoom(EFalse);
    iZoomActive = EFalse;
	iIsHDMIconnected = EFalse;
	
    //To know if HDMi cable is connected.
    iGlxTvOut = CGlxTv::NewL(*this);
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// -----------------------------------------------------------------------------
// ScreenSize : gets the screensize, Note that , this is not stored in member variable as
// Screen rect can change based on orientation
// -----------------------------------------------------------------------------
//
TSize CGlxZoomControl::ScreenSize()
    {
    TRACER("CGlxZoomControl::ScreenSize()");
    return AlfUtil::ScreenSize();
    }

// -----------------------------------------------------------------------------
// ShowZoom
// -----------------------------------------------------------------------------
//    
void CGlxZoomControl::ShowZoom(TBool aShow)
    {
    TRACER("CGlxZoomControl::ShowZoom()");
    if (aShow)
        {
        iViewPort->SetOpacity(KGlxOpacityOpaque);
        iGestureHelper->AddObserver(this);
        }
    else
        {
        iViewPort->SetOpacity(KGlxOpacityTransparent);
        }
    }

// -----------------------------------------------------------------------------
// CreateZoomVisual
// -----------------------------------------------------------------------------
//    
void CGlxZoomControl::CreateZoomVisualL()
    {
    TRACER("CGlxZoomControl::CreateZoomVisualL()");

    CAlfTexture *blackoutTexture = &(iTextureMgr->CreateFlatColourTextureL(KRgbBlack));
    CleanupStack::PushL(blackoutTexture);

    iViewPort = CAlfViewportLayout::AddNewL(*this);
    //Create the Image visual ,which holds the texture,which is complete black in color
    CAlfImageVisual *blackoutImageVisual = CAlfImageVisual::AddNewL(*this,iViewPort);
    CleanupStack::PushL(blackoutImageVisual);

    // Visual is larger than the screen to deal with control transformations that
    // may show areas usually outside the normal screen coordinates
    TInt16 blackImageSize =(iScreenSize.iWidth > iScreenSize.iHeight) ?iScreenSize.iWidth:iScreenSize.iHeight;
    blackoutImageVisual->SetImage(TAlfImage(*blackoutTexture));
    blackoutImageVisual->SetFlag(EAlfVisualFlagManualLayout);
    blackoutImageVisual->SetSize(TAlfRealSize(blackImageSize, blackImageSize));
    blackoutImageVisual->SetPos(TAlfRealPoint(0, 0));
    CleanupStack::Pop(blackoutImageVisual);
    CleanupStack::Pop(blackoutTexture);

    //  VisualLayoutUpdated
    //  Enable scrolling of the Viewport.
    //  The content area of a layout can be thought to be larger than the real visible size. 
    //  Scrolling changes which portion of the layout’s content is actually visible at  the moment.
    iViewPort->SetFlag(EAlfVisualFlagLayoutUpdateNotification);
    iViewPort->SetVirtualSize(TAlfRealSize(iScreenSize.iWidth,iScreenSize.iHeight), 0);
    iViewPort->SetViewportSize(TAlfRealSize(iScreenSize.iWidth,iScreenSize.iHeight), 0);
    iViewPort->SetViewportPos(TAlfRealPoint(0,0), 0);
    iViewPort->SetClipping(EFalse);
    iViewPort->SetOrigin( EAlfVisualHOriginLeft, EAlfVisualVOriginTop );

    //Enable scrolling of the Viewport.
    //The content area of a layout can be thought to be larger than the real visible size. 
    //Scrolling changes which portion of the layout’s content is actually visible at  the moment.
    iViewPort->EnableScrollingL();
    iViewPort->EnableTransformationL();

    iImageVisual = CAlfImageVisual::AddNewL(*this,iViewPort);
    iImageVisual->SetScaleMode(CAlfImageVisual::EScaleFitInside);

    iEventHandler = CGlxZoomPanEventHandler::NewL(*this);
    }

// -----------------------------------------------------------------------------
// CGlxZoomControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxZoomControl* CGlxZoomControl::NewL(
        MGlxUiCommandHandler& aCommandHandler,
        MGlxMediaList& aMediaList,CEikButtonGroupContainer& aZoomKeys,
        IMulSliderWidget& aSliderWidget, GestureHelper::CGestureHelper* aGestureHelper)
    {
    TRACER("CGlxZoomControl::NewL");
    CGlxZoomControl* self = new (ELeave) CGlxZoomControl(aCommandHandler,
            aMediaList,aZoomKeys,aSliderWidget,aGestureHelper);

    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxZoomControl::~CGlxZoomControl()
    {
    TRACER("CGlxZoomControl::~CGlxZoomControl()");
    if(iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    delete iTimer;

    if(iGlxTvOut)
        {
        delete iGlxTvOut;
        }

    if(iEventHandler)
        {
        delete iEventHandler;
        iEventHandler = NULL ;
        }
    
    if(iViewPort)
        {
        iViewPort->RemoveAndDestroyAllD();
        iViewPort = NULL;
        }
    iMediaList.RemoveContext(iAttributeContext);
    delete iAttributeContext;
    if(iImageTexture)
        {
        delete iImageTexture;
        iImageTexture=NULL;
        }
    iDisplay->Roster().Hide(*iControlGroup);
    if ( iControlGroup )
        {
        iControlGroup->Remove(this);
        iEnv->DeleteControlGroup(0x119);
        iControlGroup = NULL;
        }
    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// ActivateL:Activates the Zoom Control,set the image visual ,do the initial setup
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxZoomControl::ActivateL(TInt aInitialZoomRatio, TZoomStartMode aStartMode, 
        TInt aFocusIndex, TGlxMedia& aItem, TPoint* aZoomFocus)
    {
    TRACER("CGlxZoomControl::ActivateL()");

    //To know if HDMi cable is connected.
    if ( !iZoomActive )
        {
        //This Varaiable updates that we are in zoom state now.
        iZoomActive = ETrue;
        //To Retrive the image details
        TMPXAttribute thumbNailAttribute(0,0);

        TGlxIdSpaceId idspace = iMediaList.IdSpaceId( aFocusIndex );
        //Get the texture Created in fullscreen View.
        iImageTexture = &(iTextureMgr->CreateNewTextureForMediaL(
                ScreenSize(),aItem, idspace, this ));
        iImageVisual->SetImage(*iImageTexture);
        
        
        if(iGlxTvOut->IsHDMIConnected())
            {
            iIsHDMIconnected = ETrue;
            StartZoomAnimation();
            }
        else
            {
            ShowZoom(ETrue);
            iZoomSliderWidget.AddEventHandler(*this);
            iZoomSliderWidget.SetHandleKeyEvent(EFalse);
            iZoomSliderModel = (IMulSliderModel*) iZoomSliderWidget.model();
            // Get size, return value tells us if it was available
            //We need this Value to calculate the size of the visual/Layout corresponding to the Zoom factor
            TSize imageSize;
            aItem.GetDimensions( imageSize );
            TSize maxVirtualImageSize = imageSize;
    
            if (KGlxDecodingThreshold < (imageSize.iWidth * imageSize.iHeight))
                {
                TReal areaRatio = TReal(imageSize.iWidth*imageSize.iHeight)/KGlxDecodingThreshold ;
                
                TReal sideRatio;
                Math::Sqrt(sideRatio, areaRatio);
                
                maxVirtualImageSize.iHeight = imageSize.iHeight /  sideRatio ;
                maxVirtualImageSize.iWidth  = imageSize.iWidth  /  sideRatio ;
                }
            
            // Now since our maximum size possible is 3 MP. all our zoom percentages will be relative to it. 
            // So our initial zoom ratio will be different and cnsequently our minimum slider value too will change. 
            // Maximum is an
            TInt initialZoomRatio = GetInitialZoomLevel(maxVirtualImageSize);
            iZoomSliderModel->SetMinRange(initialZoomRatio);
    
            iEventHandler->SetZoomActivated(ETrue);
            iEventHandler->ActivateZoom(initialZoomRatio,
                    maxVirtualImageSize,
                    aStartMode,
                    iZoomSliderModel->MinRange(), 
                    iZoomSliderModel->MaxRange(),
                    maxVirtualImageSize,
                    aZoomFocus);
            
            TRAP_IGNORE(iImageTexture = 
            &(iTextureMgr->CreateZoomedTextureL(aItem,thumbNailAttribute,idspace,this)));
           
            //This is for handling the alaram interrupt events,that causes the phone to freeze.
            CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
            if (NULL != cba)
                {
                if (cba->IsVisible())
                    {
                    cba->ActivateL();
                    }
                }
            }
        }
    }
// ---------------------------------------------------------------------------
// StartZoomAnimation
// ---------------------------------------------------------------------------
// 
void CGlxZoomControl::StartZoomAnimation()
    {
    TRACER("CGlxZoomControl::StartZoomAnimation()");
    iZoomIn = ETrue;
    //Set zoom visible but not enable the gesturehelper events    
    iViewPort->SetOpacity(KGlxOpacityOpaque);  
    TAlfTimedValue timedvalue;
    //using KGlxOpacityOpaque for the value 1 , assuming the initial zoom level as 1.
    timedvalue.SetValueNow(KGlxOpacityOpaque); 
    timedvalue.SetTarget(KGlxZoomLevel,KZoomDelay/1000);    
    iImageVisual->SetScale(timedvalue);
    iTimer->Cancel();
    iTimer->Start(KZoomDelay,KZoomDelay,TCallBack( TimeOut,this ));
    }
// ---------------------------------------------------------------------------
// TimeOut
// ---------------------------------------------------------------------------
//  
TInt CGlxZoomControl::TimeOut(TAny* aSelf)
    {
    TRACER("CGlxZoomControl::TimeOut");
    if(aSelf)
        {
        CGlxZoomControl* self = static_cast <CGlxZoomControl*> (aSelf);
        self->ActivateFullscreen();            
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// ActivateZoomAnimation
// ---------------------------------------------------------------------------
//
void CGlxZoomControl::ActivateFullscreen()
    {
    TRACER("CGlxZoomControl::ActivateFullscreen");
    if(iZoomIn)
        {
        iZoomIn = EFalse;
        TAlfTimedValue timedvalue;
        timedvalue.SetValueNow(KGlxZoomLevel);
        timedvalue.SetTarget(KGlxOpacityOpaque,KZoomDelay/1000);
        iImageVisual->SetScale(timedvalue);
        }
    else
        {
        if(iTimer->IsActive())
          {
          iTimer->Cancel();
          }
        //once the animation  is done come back to fullscreen.
        HandleZoomOutL(KGlxZoomOutCommand);
        }
    }

// -----------------------------------------------------------------------------------
// Deactivate:Remove the screen furniture ,cancel the timers if any
// ----------------------------------------------------------------------------------
//
EXPORT_C void CGlxZoomControl::Deactivate()
    {
    TRACER("CGlxZoomControl::Deactivate()");
    iIsHDMIconnected = EFalse;

    if ( iZoomActive )
        {
         if(iTimer->IsActive())
           {
           iTimer->Cancel();           
           }  
        iZoomSliderWidget.ShowWidget( EFalse, 0 );
        iZoomSliderWidget.RemoveEventHandler(*this);
        iZoomBackKey->MakeVisible( EFalse );
        iTextureMgr->RemoveZoomList();

        iImageVisual->SetImage(*iImageTexture);
        iEventHandler->DeactivateZoom();
        CleanUpVisual();

        iZoomActive = EFalse;
        iEventHandler->SetZoomActivated(EFalse);        
        }
    // Hide the Zoom View
    ShowZoom(EFalse);
    } 

// -----------------------------------------------------------------------------------
// HandleZoomForegroundEventL:Zoom Foreground event handling function
// ----------------------------------------------------------------------------------
//
EXPORT_C void CGlxZoomControl::HandleZoomForegroundEvent(TBool aForeground)
    {
    TRACER("CGlxZoomControl::HandleZoomForegroundEventL()");

    //Refeed the textures if we are coming back to foreground from background
    //To Retrive the image details
    TMPXAttribute thumbNailAttribute(0,0);
    TInt focusIndex = iMediaList.FocusIndex();
    TGlxIdSpaceId idspace = iMediaList.IdSpaceId( focusIndex );
    //Get the texture Created in fullscreen View.
    TGlxMedia item = iMediaList.Item( focusIndex );
    
    if (!aForeground)
        {
        iEventHandler->CancelZoomPanTimer();
        iEventHandler->CancelUITimer();
        iEventHandler->CancelAnimationTimer();
        }
    else
        {
        // if we already have the decoded zoomed image bitmap use the texture corresponding to that.
        // Else make do with the fullscreen texture till that happens.  
        TRAP_IGNORE(iImageTexture = 
            iTextureMgr->CreateZoomedTextureL());
        
        if (NULL == iImageTexture)
            {
            iImageTexture = &(iTextureMgr->CreateNewTextureForMediaL(
                    ScreenSize(),item, idspace, this ));
            }

        iImageVisual->SetImage(*iImageTexture);
        }
    } 

// -----------------------------------------------------------------------------
// CleanUpVisual:reset the values of the variables.
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::CleanUpVisual()
    {
    TRACER("CGlxZoomControl::CleanUpVisual() ");
    }
// -----------------------------------------------------------------------------
// Activated
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxZoomControl::Activated()
    {
    TRACER("CGlxZoomControl::Activated");
    return iZoomActive;
    }

// -----------------------------------------------------------------------------
// OfferEventL
// -----------------------------------------------------------------------------
//
TBool CGlxZoomControl::OfferEventL(const TAlfEvent &aEvent)
    {
    TRACER("CGlxZoomControl::OfferEventL()");
    
    GLX_LOG_INFO1("OfferEventL 2: iZoomEnabled = %d   ", iZoomActive );
    // All the events related to the keys and pointers will be coming to this offerevent
    return iEventHandler->HandleEventL(aEvent) ;
    }


// -----------------------------------------------------------------------------
// offerEvent
// -----------------------------------------------------------------------------
//
AlfEventStatus CGlxZoomControl::offerEvent( CAlfWidgetControl& /*aControl*/,
        const TAlfEvent& aEvent)
    {
    TRACER("CGlxZoomControl::OfferEvent( CAlfWidgetControl& /*aControl*/)");
    // All the events which are related to slider widget will be coming
    // to this offerevent.
    AlfEventStatus eventStatus = EEventNotHandled;

    TBool consumed = iEventHandler->HandleEvent(aEvent);
    if ( consumed )
        {
        eventStatus = EEventConsumed;
        }

    return eventStatus;
    }


// -----------------------------------------------------------------------------
// TextureContentChangedL
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::TextureContentChangedL( TBool /*aHasContent*/ , CAlfTexture* aNewTexture)
    {
    TRACER("CGlxZoomControl::TextureContentChangedL ");
    //if both the textures are null that means we do not have memory to 
    //decode and show anything. So try and check if the fullscreen texture 
    //is created by now if not then go to fullscreen view.

    if(NULL == aNewTexture )
        {
        if(NULL == iImageTexture)
            {
            TSize TextureSize = ScreenSize();
            TInt focus = iMediaList.FocusIndex();
            TGlxMedia item = iMediaList.Item( focus );
            TGlxIdSpaceId idspace = iMediaList.IdSpaceId( focus );
    
            CAlfTexture* newTexture = NULL;
            //Get the texture Created in fullscreen View.
            TRAPD(errtexture, newTexture = &(iTextureMgr->CreateNewTextureForMediaL(
                            TextureSize,item, idspace, this )));
            
            if(errtexture != KErrNone)
                {
                GLX_LOG_INFO( "CGlxTextureManagerImpl::HandleBitmapDecodedL::CreateNewTextureForMediaL Failed");
                //Exit zoom and goto fullscreen
                HandleZoomOutL(KGlxZoomOutCommand);
                }
            else
                {
                //show the fullscreen texture.
                iImageTexture = newTexture;
                iImageVisual->SetImage( *iImageTexture );   
                }           
            }
         else
            {
            //show the existing first level decoded image texture in case second level
            //decoding fails
            iImageVisual->SetImage( *iImageTexture );   
            }                   
        }
    else
        {
        iImageTexture = aNewTexture;
        iImageVisual->SetImage( *iImageTexture );        
        }
    }

// -----------------------------------------------------------------------------
//  VisualLayoutUpdated:updates the co-ordinates when phone is tilted
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::VisualLayoutUpdated(CAlfVisual& aVisual)
    {
    TRACER("CGlxZoomControl::VisualLayoutUpdated ");
    // Callback comes to this function when there is a  resolution change
    TRect rect;
    rect = AlfUtil::ScreenSize();
    if ( (rect.Width() != iScreenSize.iWidth) && ( rect.Height() != iScreenSize.iHeight) && (Activated()) )
        {
        //notify slider about Orientation Change
        iZoomSliderWidget.ContainerLayout().Owner().VisualLayoutUpdated(aVisual);
        //when the Phone is tilted,Height becomes the width and vice versa. So update with the new dimensions
        iScreenSize.iWidth = rect.Width();     
        iScreenSize.iHeight = rect.Height();   
        iDisplay->SetVisibleArea(TRect(TPoint(0,0),iScreenSize));
        
        //[TODO]: Use the Maths engine ro arrive at this figure (virtual and viewport sizes). else there might be problems in fringe conditions
        iViewPort->SetVirtualSize(TAlfRealSize(iScreenSize.iWidth,iScreenSize.iHeight), 0);
        iViewPort->SetViewportSize(TAlfRealSize(iScreenSize.iWidth,iScreenSize.iHeight), 0);

        iEventHandler->OrientationChanged(rect);
        }
    }

// ---------------------------------------------------------------------------
// ShowUi
// ---------------------------------------------------------------------------
//  
void CGlxZoomControl::ShowUi(TBool aShow)
    {
    TRACER("CGlxZoomControl::ShowUi ");

    if (aShow)
        {
        iEventHandler->SetZoomUiState(EUiOn);
        }
    else
        {
        iEventHandler->SetZoomUiState(EUiOff);
        }
    
    iZoomBackKey->MakeVisible( aShow );
    iZoomSliderWidget.ShowWidget( aShow, 0 );
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler..
// ---------------------------------------------------------------------------
//
bool CGlxZoomControl::accept( CAlfWidgetControl& /*aControl*/,
        const TAlfEvent& /*aEvent*/) const
    {
    TRACER("CGlxZoomControl::accept ");
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler..
// ---------------------------------------------------------------------------
//
void CGlxZoomControl::setEventHandlerData(
        const AlfWidgetEventHandlerInitData& /*aData*/)
    {
    TRACER("CGlxZoomControl::setEventHandlerData ");
    }
// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler..
// ---------------------------------------------------------------------------
//

AlfWidgetEventHandlerInitData* CGlxZoomControl::eventHandlerData()
    {
    TRACER("CGlxZoomControl::eventHandlerData ");
    return NULL;
    }
// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler.
// ---------------------------------------------------------------------------
//

void CGlxZoomControl::setActiveStates( unsigned int /*aStates*/ )
    {
    TRACER("CGlxZoomControl::setActiveStates ");
    }
// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler.
// ---------------------------------------------------------------------------
//

IAlfInterfaceBase* CGlxZoomControl::makeInterface( const IfId& /*aType*/ )
    {
    TRACER("CGlxZoomControl::makeInterface Enter/Exit");
    return NULL;
    }
//----------------------------------------------------------------------------------
// eventHandlerType
//----------------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerType CGlxZoomControl::eventHandlerType() 
    {
    TRACER("CGlxZoomControl::eventHandlerType Enter/Exit");
    return IAlfWidgetEventHandler::ELogicalEventHandler;
    }

//----------------------------------------------------------------------------------
// eventExecutionPhase
//----------------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase CGlxZoomControl::eventExecutionPhase()
    {
    TRACER("CGlxZoomControl::eventExecutionPhase Enter/Exit");
    return EBubblingPhaseEventHandler;
    }

//----------------------------------------------------------------------------------
// GetOrientationL: to retreive orientation from the exif tags in the file
//----------------------------------------------------------------------------------
//    
TUint16 CGlxZoomControl::GetOrientationL(const TDesC& aFileName) 
    {
    TRACER("CGlxZoomControl::GetOrientationL()");
    //Get Exif Metadata and the orientation tag from the file first
    RFs fs;
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.Connect());

    RFile file;
    CleanupClosePushL(file);
    User::LeaveIfError(file.Open(fs,
            aFileName, EFileRead));
    
    TInt size;
    User::LeaveIfError(file.Size(size));
    if ( KGlxMaxExifSize < size )
        {
        size = KGlxMaxExifSize;
        }
    TUint16 orientation = 0;
    HBufC8* exifData = HBufC8::NewLC(size);
    TPtr8 ptr(exifData->Des());
    User::LeaveIfError(file.Read(ptr));
    CExifRead* exifReader = NULL;
    TRAPD(exifErr,exifReader = CExifRead::NewL(*exifData, CExifRead::ENoJpeg));
    if(exifErr == KErrNone)
        {
        CleanupStack::PushL(exifReader);

        TInt readErr = exifReader->GetOrientation(orientation);
        if(readErr != KErrNone)
            {
            orientation = 0;
            }
        CleanupStack::PopAndDestroy(exifReader);
        }
    CleanupStack::PopAndDestroy(exifData);
    //Close and pop file Session
    CleanupStack::PopAndDestroy(&file);
    CleanupStack::PopAndDestroy(&fs);
    return orientation;

    }

// -----------------------------------------------------------------------------
// UpdateViewPort
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::UpdateViewPort(TPoint& aViewPortTopLeft,
        TInt aTransitionTime,
        TSize *apViewPortDimension,
        TInt aPrimarySliderRatio)
    {
    TRACER("void CGlxZoomControl::UpdateViewPort");
    
    iViewPort->SetViewportPos(aViewPortTopLeft,aTransitionTime);

    if (apViewPortDimension)
        {
        iViewPort->SetVirtualSize(*apViewPortDimension,0);
        }

    if(aPrimarySliderRatio >= 0)
        {
        iZoomSliderModel->SetPrimaryValue(aPrimarySliderRatio);
        }
    }

// -----------------------------------------------------------------------------
// HandleShowUi
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::HandleShowUi( TBool aShow)
    {
    TRACER("void CGlxZoomControl::HandleShowUi");
    ShowUi(aShow);
    }
    


// -----------------------------------------------------------------------------
// HandleSetViewPort
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::HandleViewPortParametersChanged(TPoint& aViewPortTopLeft ,
        TInt aTransitionTime ,
        TSize *apViewPortDimension ,
        TInt aPrimarySliderLevel  )
    {
    TRACER("void CGlxZoomControl::HandleSetViewPort");
    UpdateViewPort(aViewPortTopLeft, aTransitionTime, apViewPortDimension, 
                                                            aPrimarySliderLevel);
    }

// -----------------------------------------------------------------------------
// HandleZoomOut
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::HandleZoomOutL(TInt aCommandId)
    {
    TRACER("void CGlxZoomControl::HandleZoomOut");
    iCommandHandler.HandleCommandL(aCommandId, this);
    }


// -----------------------------------------------------------------------------
// HandlePointerEventsL
// -----------------------------------------------------------------------------
//
TBool CGlxZoomControl::HandlePointerEventsL(const TAlfEvent &aEvent)
    {
    TRACER("void CGlxZoomControl::HandlePointerEventsL");    
    
    return iGestureHelper->OfferEventL(aEvent);
    
    }

// -----------------------------------------------------------------------------
// HandleGestureL
// -----------------------------------------------------------------------------
//
void CGlxZoomControl::HandleGestureL( const GestureHelper::MGestureEvent& aEvent )
    {
    TRACER("void CGlxZoomControl::HandleGestureL");
    
    TGestureCode code = aEvent.Code(MGestureEvent::EAxisBoth); 
	GLX_LOG_INFO1("_PHOTOS_LOG_: void CGlxZoomControl::HandleGestureL  Code : %d", code);
    
    // Todo: This switch should go into the event handler.  
    switch (code)
        {
        case EGestureDrag:
            iEventHandler->HandleDragEvent(aEvent);
            break;
        case EGestureTap:
            iEventHandler->HandleSingleTap(aEvent);
            break;
        case EGesturePinch:
            iEventHandler->HandlePinchEventL(aEvent);
            break;
        case EGestureDoubleTap:
            iEventHandler->HandleDoubleTap(aEvent);
            break;
        case EGestureReleased:
            iEventHandler->HandleGestureReleased(aEvent);
            break;
        default :
            break;
        }
    iEventHandler->SetPreviousEventCode(code);
    }

// -----------------------------------------------------------------------------
// GetInitialZoomLevel
// -----------------------------------------------------------------------------
//
TInt CGlxZoomControl::GetInitialZoomLevel(TSize& aSize )
    {
    TRACER("CGlxZoomControl::InitialZoomLevel");

    TRect rect = AlfUtil::ScreenSize();
    TUint8 initialZoomLevel;

    if( rect.Width()>= aSize.iWidth && rect.Height() >= aSize.iHeight)
        {
        //if Both the Width and Height are lesser than the screen size,the initial Zoom Level will be 100
        initialZoomLevel = KGlxMinSmallImageZoomLevel;
        }
    else
        {
        //Calculate (Display Area Width)  /Image Width   * 100
        //Calculate (Display Area Height) /Image Height  * 100
        //Choose the Minimum Of the above.
        TReal32 imageWidthRatio  = ((TReal32) rect.Width()/ aSize.iWidth  )*100.0F;
        TReal32 imageHeightRatio = ((TReal32)rect.Height()/ aSize.iHeight )*100.0F;
        initialZoomLevel = Min(imageWidthRatio,imageHeightRatio);
        } 
    return initialZoomLevel;
    }
// -----------------------------------------------------------------------------
// HandleTvStatusChangedL 
// -----------------------------------------------------------------------------
void CGlxZoomControl::HandleTvStatusChangedL( TTvChangeType aChangeType )
    {
    TRACER("CGlxZoomControl::HandleTvStatusChangedL()");
    if ( aChangeType == ETvConnectionChanged && !iIsHDMIconnected && iZoomActive)          
        {
        if ( iGlxTvOut->IsHDMIConnected() )
            {
            //go to fullscreen.
            HandleZoomOutL(KGlxZoomOutCommand);
            }
        }
    }


//  End of File

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
* Description:    ALFTK utilities
*
*/





// INCLUDE FILES
// Class header

// USER INCLUDE FILES
#include "glxuiutility.h"
#include "glxscreenfurniture.h"
#include "glxskinchangemonitor.h"

// SYSTEM INCLUDE FILES
#include <aknappui.h>
#include <AknUtils.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfutil.h>
#include <alf/alfenv.h>
#include <alf/alftexture.h>
#include <alf/alftextstyle.h> 
#include <alf/alftextstylemanager.h>
#include <featmgr.h>
#include <bldvariant.hrh>   // For feature constants

// Internal incudes
#include <glxresolutionmanager.h>       // for CGlxResolutionManager
#include <glxsingletonstore.h>      
#include <glxpanic.h>
#include <glxassert.h>                  // for GLX_ASSERT_DEBUG
#include <glxtvconstants.h>             // for ETvConnectionChanged
#include <glxtv.h>                      // for CGlxTv
#include <glxthumbnail.h>
#include <glxsettingsmodel.h>
#include <glxsingletonstore.h>
#include <glxtexturemanager.h>
#include <glxtracer.h>

#include <e32property.h>

//Publish-Subscribe from Thumbnail manager
const TUid KTAGDPSNotification = { 0x2001FD51 }; //PS category 
const TInt KForceBackgroundGeneration = 0x00000010; //PS Key 
const TInt KItemsleft = 0x00000008; //PS key value


//Hg 
//#include <hg/hgcontextutility.h>

// TV size constants
using namespace glxTvOut;


// -----------------------------------------------------------------------------
// UtilityL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxUiUtility* CGlxUiUtility::UtilityL()
	{
	TRACER("CGlxUiUtility::UtilityL()");
	return CGlxSingletonStore::InstanceL(&NewL);
	}

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::Close()
	{
	TRACER("CGlxUiUtility::Close()");
    CGlxSingletonStore::Close(this);
	}

// -----------------------------------------------------------------------------
// 2-phase constructor
// -----------------------------------------------------------------------------
//
CGlxUiUtility* CGlxUiUtility::NewL() 
    {
    TRACER("CGlxUiUtility::NewL()");
	CGlxUiUtility* obj = new (ELeave) CGlxUiUtility();
	CleanupStack::PushL(obj);
	obj->ConstructL();
	CleanupStack::Pop(obj);
    return obj;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxUiUtility::CGlxUiUtility()
:   iNavigationDirection( EGlxNavigationForwards ),
    iOrientation( EGlxOrientationUninitialised )
    {
    TRACER("CGlxUiUtility::CGlxUiUtility()");
    iAppUi = CCoeEnv::Static()->AppUi();
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxUiUtility::ConstructL()
    {
    TRACER("CGlxUiUtility::ConstructL()");
    iSettingsModel = CGlxSettingsModel::InstanceL();
    

	// Always start in default orientation
    //SetAppOrientationL(EGlxOrientationDefault);

	iOrientation = EGlxOrientationDefault;
    // in emulator use bitgdi as open GL does not support changing orientation

    // Ferhan (28/06/07) : commenting out usage of opengl because causing drawing problems when using
    // the preview list (gldrawelements method in opengl seems to hang)
   // iEnv = CHuiEnv::Static();
   	//@ Fix for error id EABI-7RJA8C
    iEnv = CAlfEnv::Static();
    if (!iEnv)
       {
       iEnv = CAlfEnv::NewL();
       // change to this on hw that supports opengl
       // iEnv = CHuiEnv::NewL( EHuiRendererGles10 );
       }
    else
       {
       iShared = ETrue;
       }
    iGlxTextureManager = CGlxTextureManager::NewL(iEnv->TextureManager());

    //Skin change observer
    iGlxSkinChangeMonitor = CGlxSkinChangeMonitor::NewL();
    iAppUi->AddToStackL(iGlxSkinChangeMonitor);
    
    // Add TextureManager as a skin change event observer here itself because
    // calling CGlxHuiUtility::Utility() in CGlxTextureManager creates a recursive call
    // to CGlxTextureManager::NewL which again calls CGlxHuiUtility::Utility() and so on.
    AddSkinChangeObserverL( *iGlxTextureManager );    
	GridIconSizeL();
	TRect rect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect ); 
    iAlfDisplay = &iEnv->NewDisplayL(rect,
            CAlfEnv::ENewDisplayAsCoeControl | CAlfEnv::ENewDisplayFullScreen );
            
     iEnv->AddActionObserverL (this);
     
    // Use the Avkon skin background as the display background.    
    //iAlfDisplay->SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);
    
    // create the resoltuion manager - needs to be informed when screen size 
    // changes
    iGlxResolutionManager = CGlxResolutionManager::NewL();
    
    iGlxTvOut = CGlxTv::NewL(*this);
    
    // Is the TV Out display on
    // Note that this also sets the initial size for the resolution manager
    HandleTvStatusChangedL( ETvConnectionChanged );
    
    iScreenFurniture = CGlxScreenFurniture::NewL(*this);
    
//    iContextUtility = CHgContextUtility::NewL();
//    iContextUtility->RePublishWhenFgL( ETrue );
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxUiUtility::~CGlxUiUtility()
    {
    TRACER("CGlxUiUtility::~CGlxUiUtility()");
    GLX_LOG_INFO("~CGlxUiUtility");
    delete iScreenFurniture;
/*    if (iContextUtility)
        {
        delete iContextUtility;
        iContextUtility = NULL;
        }
*/
    // Destroy TV Out related objects
    delete iGlxTvOut;
    delete iGlxResolutionManager;
    DestroyTvOutDisplay();
    
	if( iGlxTextureManager )
		{
	    RemoveSkinChangeObserver( *iGlxTextureManager ); 
	    delete iGlxTextureManager;   	
		}

    if(iAlfDisplay)
        {
        delete iAlfDisplay;
        }
   
    iEnv->RemoveActionObserver(this);

    if ( iGlxSkinChangeMonitor )
        {
        iAppUi->RemoveFromStack(iGlxSkinChangeMonitor);
        delete iGlxSkinChangeMonitor;
        }
        
    if (!iShared)
        {
        delete iEnv;
        }
        
    iTextStyles.Close();
    if ( iSettingsModel )
        {
        iSettingsModel->Close();
        }    

    }

// -----------------------------------------------------------------------------
// Env
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfEnv* CGlxUiUtility::Env() const
	{
	TRACER("CGlxUiUtility::Env()");
	return iEnv;
	}

// -----------------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfDisplay* CGlxUiUtility::Display() const
	{
	TRACER("CGlxUiUtility::Display()");
	return iAlfDisplay;
	}

// -----------------------------------------------------------------------------
// ShowAlfDisplay
// -----------------------------------------------------------------------------
EXPORT_C void CGlxUiUtility::ShowAlfDisplayL()
	{
	TRACER("CGlxUiUtility::ShowAlfDisplayL()");
	// make it visible as well
	CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();

	CCoeControl* contl = (CCoeControl*)uiUtility->Display()->ObjectProvider();
	contl->MakeVisible (ETrue);

	uiUtility->Close();
	// no need to do anything else, the coecontrol handles the visibility
	}

// -----------------------------------------------------------------------------
// HideAlfDisplay
// -----------------------------------------------------------------------------
EXPORT_C void CGlxUiUtility::HideAlfDisplayL()
	{
	TRACER("CGlxUiUtility::HideAlfDisplayL()");
	// make it invisible as well (this might be all that is needed)
	CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
	
	CCoeControl* contl = (CCoeControl*)uiUtility->Display()->ObjectProvider();
	contl->MakeVisible (EFalse);
	
	uiUtility->Close();
	// no need to do anything else, the coecontrol handles the visibility
	}
	
// -----------------------------------------------------------------------------
// GlxTextureManager
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxTextureManager& CGlxUiUtility::GlxTextureManager()
	{
	TRACER("CGlxUiUtility::GlxTextureManager()");
	__ASSERT_ALWAYS(iGlxTextureManager, Panic(EGlxPanicLogicError));
    return *iGlxTextureManager;
	}
	
// -----------------------------------------------------------------------------
// ViewNavigationDirection
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxNavigationDirection CGlxUiUtility::ViewNavigationDirection()
    {
    TRACER("CGlxUiUtility::ViewNavigationDirection()");
    return iNavigationDirection;
    }
    

// -----------------------------------------------------------------------------
// SetViewNavigationDirection
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::SetViewNavigationDirection(TGlxNavigationDirection aDirection)
    {
    TRACER("CGlxUiUtility::SetViewNavigationDirection()");
    iNavigationDirection = aDirection;
    }

// -----------------------------------------------------------------------------
// DisplaySize
// -----------------------------------------------------------------------------
//
EXPORT_C TSize CGlxUiUtility::DisplaySize() const
    {
    TRACER("CGlxUiUtility::DisplaySize()");
    //return Env()->PrimaryDisplay().Size();
    const TRect& rect = Env()->PrimaryDisplay().VisibleArea();
	return rect.Size();
    }

// -----------------------------------------------------------------------------
// TextStyleIdL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::TextStyleIdL(TInt aFontId, TInt aSizeInPixels)
    {
    TRACER("CGlxUiUtility::TextStyleIdL()");
    TInt id = 0;
    TBool found = EFalse;
    TInt index = 0;
    TInt count = iTextStyles.Count();
    // Look for existing style with this font and size
    while(index < count && !found)
        {
        if(iTextStyles[index].iSizeInPixels == aSizeInPixels &&
           iTextStyles[index].iFontId == aFontId)
            {
            // found it. Prepare to return found id
            id = iTextStyles[index].iStyleId;
            found = ETrue;
            }
        else
            {
            // check next style
            index++;
            }
        }
    if(!found)
        {
        // Create a new style based on the required font
        CAlfTextStyleManager& styleMan = iEnv->TextStyleManager();
        
        // remember its id for return later
        id = styleMan.CreatePlatformTextStyleL(aFontId);
        
        // Get style. It is not owned
        CAlfTextStyle* style = styleMan.TextStyle(id);
        
        // Set this style to have required size and to be normal weight
        style->SetTextSizeInPixels(aSizeInPixels);
        
        style->SetBold(EFalse);
        
        // Add this style to list
        TGlxTextStyle textStyle(aFontId, aSizeInPixels, id);
        iTextStyles.AppendL(textStyle);
        }
    return id;
    }

// ---------------------------------------------------------------------------
// SetAppOrientationL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::SetAppOrientationL(TGlxOrientation aOrientation)
    {
    TRACER("CGlxUiUtility::SetAppOrientationL()");
    GLX_LOG_INFO1( "CGlxUiUtility::SetAppOrientationL(%d)", aOrientation );

    if ( iOrientation != aOrientation )
        {
        CAknAppUiBase::TAppUiOrientation orientation(
                                CAknAppUiBase::EAppUiOrientationUnspecified);

        switch ( aOrientation )
            {
            case EGlxOrientationTextEntry:
                if ( iSettingsModel->IsTextEntryPortrait() )
                    {
                    orientation = CAknAppUiBase::EAppUiOrientationPortrait;
                    break;
                    }
                // else fall through to EGlxOrientationDefault

            case EGlxOrientationDefault:
                {
                if ( CGlxSettingsModel::ELandscape
                        == iSettingsModel->SupportedOrientations() )
                    {
                    orientation = CAknAppUiBase::EAppUiOrientationLandscape;
                    }
                else
                    {
                    orientation = CAknAppUiBase::EAppUiOrientationAutomatic;
                    }
                }
                break;

            case EGlxOrientationLandscape:
                orientation = CAknAppUiBase::EAppUiOrientationLandscape;
                break;

            default:
                Panic(EGlxPanicIllegalArgument);
                break;
            }

        static_cast<CAknAppUiBase*>(iAppUi)->SetOrientationL(orientation);
        iOrientation = aOrientation;
        }
    }

// ---------------------------------------------------------------------------
// AppOrientation
// ---------------------------------------------------------------------------
//
EXPORT_C TGlxOrientation CGlxUiUtility::AppOrientation() const
    {
    TRACER("CGlxUiUtility::AppOrientation()");
    return iOrientation;
    }
// -----------------------------------------------------------------------------
// GetGridIconSize
// -----------------------------------------------------------------------------
//  
EXPORT_C TSize CGlxUiUtility::GetGridIconSize()
    {
	 TRACER("CGlxUiUtility::GetGridIconSize()");
    return iGridIconSize;
    }    
// -----------------------------------------------------------------------------
// HandleTvStatusChangedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::HandleTvStatusChangedL( TTvChangeType aChangeType )
    {
    TRACER("CGlxUiUtility::HandleTvStatusChangedL()");

    if ( aChangeType == ETvDisplayNotVisible )         // Visibility event
        {
        iEnv->Release();
        return; // don't redraw the display
        }
    else if ( aChangeType == ETvDisplayIsVisible )     // Visibility event
        {
        iEnv->RestoreL();
        }
     else                                              // TV Connection event
        {
        GLX_ASSERT_DEBUG( (aChangeType == ETvConnectionChanged), 
                            Panic( EGlxPanicLogicError ), 
                            "Expected TV Connection Changed" );

        if ( iGlxTvOut->IsConnected() )
            {
            // Remove the TV Diplay - a ETvConnectionChanged may result from
            // the user changing the central repository TV values.
            DestroyTvOutDisplay();
            CreateTvOutDisplayL();
            }
        else
            {
            DestroyTvOutDisplay();
            // Set the display size to that of the phone
            iGlxResolutionManager->SetScreenSizeL( DisplaySize() );
            }
        }
        
    if (iTvDisplay)
        {
        iTvDisplay->SetDirty(); // redraw
        }        
    }
    
// -----------------------------------------------------------------------------
// SetRotatedImageSize
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::SetRotatedImageSize(TSize aRotatedImageSize)
    {
    iRotatedImageSize = aRotatedImageSize;
    }

// -----------------------------------------------------------------------------
// SetRotatedImageSize
// -----------------------------------------------------------------------------
//
EXPORT_C TSize CGlxUiUtility::GetRotatedImageSize()
    {
    return iRotatedImageSize;
    }

void  CGlxUiUtility::HandleActionL (const TAlfActionCommand &aActionCommand) 
    {
    TRACER("CGlxUiUtility::HandleActionL()");
   	
   	if(aActionCommand.Id() == KAlfActionIdDeviceLayoutChanged  )
   	    {   	  
    	// check if tv-out is connected
        if ( iGlxTvOut->IsConnected() )
            {
        	// The primary (phone) display has changed orientation
        	DestroyTvOutDisplay();
                CreateTvOutDisplayL(); //@ will generate false positive in codescanner
            }
        else
            {
            // reset the screen size in case the orientation changed for example
            iGlxResolutionManager->SetScreenSizeL( DisplaySize() );
            }
   	    }
    }
  
// -----------------------------------------------------------------------------
// DestroyTvOutDisplay
// -----------------------------------------------------------------------------
//
void CGlxUiUtility::DestroyTvOutDisplay()
    {
    TRACER("CGlxUiUtility::DestroyTvOutDisplay()");
    if (iTvDisplay)
        {
        delete iTvDisplay;
        iTvDisplay = NULL; 
        // Remove Primary Window Refresh observer
        //Display()->iRefreshObservers.Remove( *this );  
        iEnv->PauseRefresh();
        // Disable Primary Window Visibility events
        CCoeControl* contl = (CCoeControl*)iEnv->PrimaryDisplay().ObjectProvider();
        contl->DrawableWindow()->DisableVisibilityChangeEvents();
        }
    }
       
        
// -----------------------------------------------------------------------------
// CreateTvOutDisplayL
// -----------------------------------------------------------------------------
//
void CGlxUiUtility::CreateTvOutDisplayL()
	{
	TRACER("CGlxUiUtility::CreateTvOutDisplayL()");
	
	// Get the TV Out display size buffer in pixels
    TSize tvOutDisplaySz = iGlxTvOut->ScreenSizeL();
    
    // Calculate the TV Out buffer by using
    // A) aspect ratio that matches the phone screen and 
    // B) size that fills the TV so that either width or height of TV is fully  
    //             used (in practice height is always restricting dimension). 
    TSize phoneDisplaySz   = DisplaySize();
  
    // Scale the tv display size to match that of the phone display size
    tvOutDisplaySz = TSize (tvOutDisplaySz.iHeight * phoneDisplaySz.iWidth 
                          / phoneDisplaySz.iHeight, tvOutDisplaySz.iHeight );
    const TRect tvOutDisplayBuffer( tvOutDisplaySz );
 
	// Commenting out Creation of second display as it goes into posting surface, 
	// Right now, only cloning is going to be the funcationality and posting would be done
	// only for specific views (FS , Zoom and Slideshow)
//    if (!iTvDisplay)
//	    {
//        iTvDisplay = &iEnv->NewDisplayL( tvOutDisplayBuffer, 
//                 CAlfEnv::ENewDisplayAsCoeControl, 
//                 iGlxTvOut->IsWidescreen() ? CAlfDisplay::EDisplayTvOutWide :
//                 CAlfDisplay::EDisplayTvOut );
//        
//        if (iTvDisplay)                                 
//            {
//            // Use the Avkon skin background as the display background.
//            iTvDisplay->SetClearBackgroundL( 
//                                        CAlfDisplay::EClearWithSkinBackground );
//            //Todo
//            // Primary Window Refresh observer
//            iEnv->SetRefreshMode(EAlfRefreshModeAutomatic);
//            }
//	    }
			
	// Set the TV screen size    
    iGlxResolutionManager->SetScreenSizeL( tvOutDisplaySz );   
	}

// -----------------------------------------------------------------------------
// AddSkinChangeObserverL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::AddSkinChangeObserverL(MGlxSkinChangeObserver& aObserver)
    {
    TRACER("CGlxUiUtility::AddSkinChangeObserverL()");
    iGlxSkinChangeMonitor->AddSkinChangeObserverL( aObserver );
    }
    
// -----------------------------------------------------------------------------
// RemoveSkinChangeObserver
// -----------------------------------------------------------------------------
//    
EXPORT_C void CGlxUiUtility::RemoveSkinChangeObserver(MGlxSkinChangeObserver& aObserver)
    {      
    TRACER("CGlxUiUtility::RemoveSkinChangeObserver()");
    iGlxSkinChangeMonitor->RemoveSkinChangeObserver( aObserver );
    }

// -----------------------------------------------------------------------------
// ScreenFurniture
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxScreenFurniture* CGlxUiUtility::ScreenFurniture()
    {
    return iScreenFurniture;
    }

// -----------------------------------------------------------------------------
// Context Utility for Teleport
// -----------------------------------------------------------------------------
//
/*EXPORT_C CHgContextUtility* CGlxUiUtility::ContextUtility()
    {
    return iContextUtility;
    }
*/
// -----------------------------------------------------------------------------
// IsPenSupported
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxUiUtility::IsPenSupported()
    {
    TRACER("CGlxUiUtility::IsPenSupported");

    return AknLayoutUtils::PenEnabled();
    }

// -----------------------------------------------------------------------------
// GridIconSize
// -----------------------------------------------------------------------------
//	
void CGlxUiUtility::GridIconSizeL()
	{
	TRACER("CGlxUiUtility::GridIconSize()");
	// Sets up TLS, must be done before FeatureManager is used.
	    FeatureManager::InitializeLibL();
	    
	  if(FeatureManager::FeatureSupported( KFeatureIdLayout640_360_Touch ) || FeatureManager::FeatureSupported( KFeatureIdLayout360_640_Touch ))
	      {
	      iGridIconSize = TSize(111,83);
	      }
	  else if(FeatureManager::FeatureSupported(KFeatureIdLayout640_480_Touch) || FeatureManager::FeatureSupported(KFeatureIdLayout480_640_Touch) || 
	          FeatureManager::FeatureSupported(KFeatureIdLayout640_480) || FeatureManager::FeatureSupported(KFeatureIdLayout480_640))
	      {
	      iGridIconSize = TSize(146,110);
	      }
	  else
	      {
	      iGridIconSize = TSize(146,110);
	      }
	// Frees the TLS. Must be done after FeatureManager is used.
	    FeatureManager::UnInitializeLib(); 
	}


 
EXPORT_C TBool CGlxUiUtility::IsExitingState()
    {
    TRACER("CGlxUiUtility::ExitingState");
    return iIsExiting;
    }

EXPORT_C void CGlxUiUtility::SetExitingState(TBool aIsExiting)
    {
    TRACER("CGlxUiUtility::SetExitingState");
    iIsExiting = aIsExiting;
    }


// -----------------------------------------------------------------------------
// VisibleItemsInPageGranularityL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::VisibleItemsInPageGranularityL()
    {
    TRACER("CGlxUiUtility::VisibleItemsInPageGranularityL");
    
    //TBD: In MCL 9.1/Corolla env currently the feature flags 
    //KFeatureIdLayout640_480 and KFeatureIdLayout480_640 are not defined.
    //so making the default granularity as 21 instead of 15.
    //Need to replace with KGlxDefaultVisibleItemsGranularity after
    //getting the feature flag.
    TInt ret = KGlxVGAVisibleItemsGranularity;
    FeatureManager::InitializeLibL();
    
    if (FeatureManager::FeatureSupported( KFeatureIdLayout640_360_Touch ) || 
        FeatureManager::FeatureSupported( KFeatureIdLayout360_640_Touch ))
      {
      ret = KGlxQHDVisibleItemsGranularity;
      }
    else if(FeatureManager::FeatureSupported(KFeatureIdLayout640_480_Touch) || 
          FeatureManager::FeatureSupported(KFeatureIdLayout480_640_Touch) || 
          FeatureManager::FeatureSupported(KFeatureIdLayout640_480) || 
          FeatureManager::FeatureSupported(KFeatureIdLayout480_640))
      {
      ret = KGlxVGAVisibleItemsGranularity;
      }
    FeatureManager::UnInitializeLib(); 
    return ret;
    }

// -----------------------------------------------------------------------------
// StartTNMDaemonL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::StartTNMDaemonL()
    {
    TRACER("CGlxUiUtility::StartTNMDaemonL");
    TInt err = RProperty::Set(KTAGDPSNotification, KForceBackgroundGeneration, ETrue);
    if(err != KErrNone)
        {
        GLX_LOG_INFO1("GetItemsLeftCountL: RProperty::Set errorcode %d",err);
        //need to check what to do in fail cases
        User::Leave(err);
        }
    }
	
// -----------------------------------------------------------------------------
// StopTNMDaemonL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::StopTNMDaemonL()
    {
    TRACER("CGlxUiUtility:: StopTNMDaemonL ");
    TInt err = RProperty::Set(KTAGDPSNotification, KForceBackgroundGeneration, EFalse);
    GLX_LOG_INFO1("CGlxUiUtility::StopTNMDaemonL err = %d",err);
    if(err != KErrNone)
        {
        GLX_LOG_INFO1("StopTNMDaemonL: RProperty::Set errorcode %d",err);
        //need to check what to do in fail cases
        User::Leave(err);
        }
    }

// -----------------------------------------------------------------------------
// GetItemsLeftCountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::GetItemsLeftCountL()
    {
    TRACER("CGlxUiUtility::GetItemsLeftCountL");
    TInt leftVariable = 0;
    TInt err = RProperty::Get(KTAGDPSNotification, KItemsleft, leftVariable);
    GLX_LOG_INFO1("GetItemsLeftCountL: GetItemsLeftCountL %d",leftVariable);
    if(err != KErrNone)
        {
        GLX_LOG_INFO1("GetItemsLeftCountL: RProperty::Get errorcode %d",err);
        User::Leave(err);
        }
    return leftVariable;	
    }
// End of file

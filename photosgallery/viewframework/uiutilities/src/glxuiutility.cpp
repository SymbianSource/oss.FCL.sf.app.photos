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
#include <eikenv.h> 
#include <eikappui.h>
#include <aknclearer.h>
#include <avkondomainpskeys.h> // Keyguard Status
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

//For animation Effects
#include <akntranseffect.h>
#include <akntransitionutils.h>

//Publish-Subscribe from Thumbnail manager
const TUid KTAGDPSNotification = { 0x2001FD51 }; //PS category 
const TInt KForceBackgroundGeneration = 0x00000010; //PS Key 
const TInt KItemsleft = 0x00000008; //PS key value
/**
 * Start Delay for the periodic timer, in microseconds
 */
const TInt KPeriodicStartDelay = 1000000; 
const TInt KIgnoreItemsLeftCount = 5;
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
:   iEnv(NULL),iNavigationDirection( EGlxNavigationForwards ),
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

    iOrientation = EGlxOrientationDefault; // Always start in default orientation
	
    if (!iPeriodic)
        {
        iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
        }
		
    if (!iPeriodic->IsActive())
        {
        iPeriodic->Start(KPeriodicStartDelay, KMaxTInt,
                TCallBack(&PeriodicCallback, static_cast<TAny*> (this)));
        }
		
    GridIconSizeL();
    iScreenFurniture = CGlxScreenFurniture::NewL(*this);
    iIsForeground = EFalse;
    }

// -----------------------------------------------------------------------------
// Callback from periodic timer
// -----------------------------------------------------------------------------
//
TInt CGlxUiUtility::PeriodicCallback(TAny* aPtr )
    {
    TRACER("CGlxUiUtility::PeriodicCallback");
    static_cast< CGlxUiUtility* >( aPtr )->CreateAlfEnvCallbackL();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Callback from periodic timer-- non static
// -----------------------------------------------------------------------------
//
void CGlxUiUtility::CreateAlfEnvCallbackL()
    {
    TRACER("CGlxUiUtility::CreateAlfEnvCallbackL");
    CreateAlfEnvL();
    }

// -----------------------------------------------------------------------------
// Create ALF env
// -----------------------------------------------------------------------------
//
void CGlxUiUtility::CreateAlfEnvL()
    {
    TRACER("CGlxUiUtility::CreateAlfEnvL");
    if (iPeriodic->IsActive())
        {
        iPeriodic->Cancel();
        }
    
    iEnv = CAlfEnv::Static();
    if (!iEnv)
       {
       iEnv = CAlfEnv::NewL();
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
    
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect ); 
    iAlfDisplay = &iEnv->NewDisplayL(rect,
            CAlfEnv::ENewDisplayAsCoeControl | CAlfEnv::ENewDisplayFullScreen );
            
     iEnv->AddActionObserverL (this);
     
    // create the resoltuion manager - needs to be informed when screen size 
    // changes
    iGlxResolutionManager = CGlxResolutionManager::NewL();
    
    iGlxTvOut = CGlxTv::NewL(*this);
    
    // Is the TV Out display on
    // Note that this also sets the initial size for the resolution manager
    HandleTvStatusChangedL( ETvConnectionChanged );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxUiUtility::~CGlxUiUtility()
    {
    TRACER("CGlxUiUtility::~CGlxUiUtility()");
    GLX_LOG_INFO("~CGlxUiUtility");

    if(iClearer)
        {
        delete iClearer;
        }

    if(iScreenFurniture)
        {
        delete iScreenFurniture;
        }
    
    if(iGlxTvOut)
        {
        // Destroy TV Out related objects
        delete iGlxTvOut;
        }
    
    if(iGlxResolutionManager)
        {
        delete iGlxResolutionManager;
        }
    
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
    if(iEnv)
       {
       iEnv->RemoveActionObserver(this);
       }

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
    
	if (iPeriodic)
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
    }

// -----------------------------------------------------------------------------
// Env
// -----------------------------------------------------------------------------
//
EXPORT_C CAlfEnv* CGlxUiUtility::Env()
	{
	TRACER("CGlxUiUtility::EnvL()");
	if(!iEnv)
	    {
        TRAP_IGNORE(CreateAlfEnvL());
	    }
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

    CCoeControl* contl =
            (CCoeControl*) uiUtility->Display()->ObjectProvider();
    contl->MakeVisible(ETrue);

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
	
    CCoeControl* contl =
            (CCoeControl*) uiUtility->Display()->ObjectProvider();
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
EXPORT_C void CGlxUiUtility::SetViewNavigationDirection(
        TGlxNavigationDirection aDirection)
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
    const TRect& rect = CEikonEnv::Static()->EikAppUi()->ApplicationRect();
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
// SetGridToolBar
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::SetGridToolBar(CAknToolbar* aToolbar)
    {
    TRACER("CGlxUiUtility::SetGridToolBar()");
    iToolbar = aToolbar;
    }

// -----------------------------------------------------------------------------
// GetGridToolBar
// -----------------------------------------------------------------------------
//
EXPORT_C CAknToolbar* CGlxUiUtility::GetGridToolBar()
    {
    TRACER("CGlxUiUtility::GetGridToolBar()");
    if(iToolbar)
        {
        return iToolbar;
        }
    return NULL;     
    }

// -----------------------------------------------------------------------------
// HandleTvStatusChangedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::HandleTvStatusChangedL( TTvChangeType aChangeType )
    {
    TRACER("CGlxUiUtility::HandleTvStatusChangedL()");

    if (aChangeType == ETvDisplayNotVisible) // Visibility event
        {
        iEnv->Release();
        return; // don't redraw the display
        }
    else if (aChangeType == ETvDisplayIsVisible) // Visibility event
        {
        iEnv->RestoreL();
        }
    else // TV Connection event
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
// SetForegroundStatus
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::SetForegroundStatus(TBool aForeground)
    {
    TRACER("CGlxUiUtility::SetForegroundStatus()");
    iIsForeground = aForeground;
    }

// -----------------------------------------------------------------------------
// GetForegroundStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxUiUtility::GetForegroundStatus()
    {
    TRACER("CGlxUiUtility::GetForegroundStatus()");
    GLX_DEBUG2("CGlxUiUtility::GetForegroundStatus() iIsForeground=%d", iIsForeground);
    return iIsForeground;
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

        iEnv->PauseRefresh();

        // Disable Primary Window Visibility events
        CCoeControl* contl =
                (CCoeControl*) iEnv->PrimaryDisplay().ObjectProvider();
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
 
	// Set the TV screen size    
    iGlxResolutionManager->SetScreenSizeL( tvOutDisplaySz );   
	}

// -----------------------------------------------------------------------------
// AddSkinChangeObserverL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::AddSkinChangeObserverL(
        MGlxSkinChangeObserver& aObserver)
    {
    TRACER("CGlxUiUtility::AddSkinChangeObserverL()");
    iGlxSkinChangeMonitor->AddSkinChangeObserverL( aObserver );
    }

// -----------------------------------------------------------------------------
// RemoveSkinChangeObserver
// -----------------------------------------------------------------------------
//    
EXPORT_C void CGlxUiUtility::RemoveSkinChangeObserver(
        MGlxSkinChangeObserver& aObserver)
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

    if (FeatureManager::FeatureSupported(KFeatureIdLayout640_360_Touch)
            || FeatureManager::FeatureSupported(KFeatureIdLayout360_640_Touch))
        {
        iGridIconSize = TSize(111, 83);
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdLayout640_480_Touch)
            || FeatureManager::FeatureSupported(KFeatureIdLayout480_640_Touch)
            || FeatureManager::FeatureSupported(KFeatureIdLayout640_480)
            || FeatureManager::FeatureSupported(KFeatureIdLayout480_640))
        {
        iGridIconSize = TSize(146, 110);
        }
    else
        {
        iGridIconSize = TSize(146, 110);
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

    if (FeatureManager::FeatureSupported(KFeatureIdLayout640_360_Touch)
            || FeatureManager::FeatureSupported(KFeatureIdLayout360_640_Touch))
        {
        ret = KGlxQHDVisibleItemsGranularity;
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdLayout640_480_Touch)
            || FeatureManager::FeatureSupported(KFeatureIdLayout480_640_Touch)
            || FeatureManager::FeatureSupported(KFeatureIdLayout640_480)
            || FeatureManager::FeatureSupported(KFeatureIdLayout480_640))
        {
        ret = KGlxVGAVisibleItemsGranularity;
        }
    FeatureManager::UnInitializeLib();
    return ret;
    }

// -----------------------------------------------------------------------------
// StartTNMDaemon
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::StartTNMDaemon()
    {
    TRACER("CGlxUiUtility::StartTNMDaemon");
    return RProperty::Set(KTAGDPSNotification, KForceBackgroundGeneration,
            ETrue);
    }

// -----------------------------------------------------------------------------
// StopTNMDaemon
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::StopTNMDaemon()
    {
    TRACER("CGlxUiUtility:: StopTNMDaemon");
    return RProperty::Set(KTAGDPSNotification, KForceBackgroundGeneration,
            EFalse);
    }

// -----------------------------------------------------------------------------
// GetItemsLeftCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::GetItemsLeftCount()
    {
    TRACER("CGlxUiUtility::GetItemsLeftCount");
    TInt itemsLeftCount = 0;
    TInt err =
            RProperty::Get(KTAGDPSNotification, KItemsleft, itemsLeftCount);
    GLX_LOG_INFO1("GetItemsLeftCount: GetItemsLeftCount %d", itemsLeftCount);

    // In case of error, enter in the next view. Don't block photos permanently.
    // [Though this use case is very unlikely]
    if (err != KErrNone)
        {
        GLX_LOG_INFO1("GetItemsLeftCount: RProperty::Get errorcode %d", err);
        itemsLeftCount = 0;
        }

    // This case is added as per UI-Improvements.
    // Use case: Take a pic. open photos from Menu (not "Goto photos")
    // Progress bar SHOULD NOT be displayed.
    if ((itemsLeftCount != KErrNotReady) && (itemsLeftCount
            <= KIgnoreItemsLeftCount))
        {
        GLX_LOG_INFO("GetItemsLeftCount( < KIgnoreItemsLeftCount )");
        itemsLeftCount = 0;
        }

    return itemsLeftCount;
    }

// -----------------------------------------------------------------------------
// DisplayScreenClearer
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::DisplayScreenClearerL()
    {
    TRACER("CGlxUiUtility::DisplayScreenClearerL");
    if(!iClearer && CAknTransitionUtils::TransitionsEnabled(
            AknTransEffect::EFullScreenTransitionsOff))
        {
        iClearer = CAknLocalScreenClearer::NewL( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// DisplayScreenClearer
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxUiUtility::DestroyScreenClearer()
    {
    TRACER("CGlxUiUtility::DestroyScreenClearer");
    if(iClearer)
        {
        delete iClearer;
        iClearer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// SetTNMDaemonPSKeyvalue
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::SetTNMDaemonPSKeyvalue()
    {
    TRACER("CGlxUiUtility::SetTNMDaemonPSKeyvalue");
    TInt ret = RProperty::Define(KTAGDPSNotification, KItemsleft,
            RProperty::EInt);
    GLX_LOG_INFO1("CGlxUiUtility::SetTNMDaemon()RProperty::Define %d", ret);
    if (ret == KErrNone)
        {
        ret = RProperty::Set(KTAGDPSNotification, KItemsleft, KErrNotReady);
        GLX_LOG_INFO1("CGlxUiUtility::SetTNMDaemon() RProperty::Set %d", ret);
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// GetKeyguardStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxUiUtility::GetKeyguardStatus()
    {
    TRACER("CGlxUiUtility::GetKeyguardStatus");
    TInt keyguardStatus = KErrNone;
    RProperty::Get(KPSUidAvkonDomain, KAknKeyguardStatus, keyguardStatus);
    GLX_LOG_INFO1("CGlxUiUtility::GetKeyguardStatus() keyguardStatus=%d", keyguardStatus);
    return keyguardStatus;
    }
// End of file

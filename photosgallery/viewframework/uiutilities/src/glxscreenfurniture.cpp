/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Screenfurniture interface. Used to show/hide UI or 
* 				 enable/disable induvidual screenfurniture items.
*
*/




#include "glxscreenfurniture.h"

#include <glxtracer.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxuiutilities.rsg>
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>
#include <StringLoader.h>

_LIT(KGlxUiUtilitiesResource,"glxuiutilities.rsc");

// CONSTANTS AND FORWARD DECLARATIONS
const TInt KSfNoView = 0;
const TInt KGlxFloatingToolbarXPosOffset = 80;
const TInt KGlxFloatingToolbarYPosOffset = 120;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxScreenFurniture* CGlxScreenFurniture::NewL(
        CGlxUiUtility& aParentUtility)
    {
    TRACER("CGlxScreenFurniture::NewL()");
    
    CGlxScreenFurniture* self = CGlxScreenFurniture::NewLC(aParentUtility);
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxScreenFurniture* CGlxScreenFurniture::NewLC(
        CGlxUiUtility& aParentUtility)
    {
    TRACER("CGlxScreenFurniture::NewLC()");
        
    CGlxScreenFurniture* self = new(ELeave) CGlxScreenFurniture(aParentUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//  
CGlxScreenFurniture::CGlxScreenFurniture(CGlxUiUtility& aParentUtility) : iParentUtility(aParentUtility)
    {
    TRACER("CGlxScreenFurniture::CGlxScreenFurniture()");
    
    }

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
//
CGlxScreenFurniture::~CGlxScreenFurniture()
	{
	TRACER("CGlxScreenFurniture::~CGlxScreenFurniture");
		
	if ( iResourceOffset )
	    {
	    CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
	    }
	    
	delete iToolbar;
	
	}
    
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//  
void CGlxScreenFurniture::ConstructL()
    {
    TRACER("CGlxScreenFurniture::ConstructL()");
    
    // No view is active at the time of ScreenFurniture object construction
    iActiveView = KSfNoView; 
    
    // Load resource
    TParse parse;
    parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);

    // Create the fullscreen floating toolbar (used only for non-touch)
    iToolbar = CAknToolbar::NewL(R_GLX_FULLSCREEN_FLOATING_TOOLBAR); 
    iToolbar->SetOrientation(EAknOrientationVertical);        
    iToolbar->SetWithSliding(EFalse);
    
    // Set ScreenFurniture to observe the toolbar events
    SetToolbarObserver(this);
    }

// -----------------------------------------------------------------------------
// SetActiveView
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::SetActiveView( TInt aViewId)
    {
    TRACER("CGlxScreenFurniture::SetActiveView()");
    
    iActiveView = aViewId;
    
    // Calculate toolbar position and display toolbar on the screen 
    // as soon as the calling view is activated.
    SetToolbarPosition();    
    SetToolbarVisibility(ETrue);
    }

// -----------------------------------------------------------------------------
// ViewDeactivated
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::ViewDeactivated( TInt aViewId)
    {
    TRACER("CGlxScreenFurniture::ViewDeactivated()");
    
    if ( aViewId == iActiveView )
        {        
        iActiveView = KSfNoView;
        SetToolbarVisibility(EFalse);
        }
    }

// -----------------------------------------------------------------------------
// SetVisibility
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::SetToolbarVisibility( TBool /*aVisible*/ )
    {
    TRACER("CGlxScreenFurniture::SetToolbarVisibility()");
    // Do Nothing
    }

// -----------------------------------------------------------------------------
// SetVisibility
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::SetToolbarItemVisibility( TInt aCommand, 
        TBool aVisible )
    {
    TRACER("CGlxScreenFurniture::SetToolbarItemVisibility()");
    
    iToolbar->HideItem(aCommand,!aVisible,EFalse);
    }

// -----------------------------------------------------------------------------
// SetItemDimmedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::SetToolbarItemDimmed( TInt aCommand, 
        TBool aDimmed )
    {
    TRACER("CGlxScreenFurniture::SetToolbarItemDimmed()");
    
    iToolbar->SetItemDimmed(aCommand,aDimmed,ETrue);
    }

// -----------------------------------------------------------------------------
// SetFocusL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::SetFocusL( TInt aCommand )
    {
    TRACER("CGlxScreenFurniture::SetFocusL()");
    
    iToolbar->SetFocusedItemL( aCommand );
    }

// -----------------------------------------------------------------------------
//       PLACEMENT OF FLOATING TOOLBAR ON THE SCREEN 
//
// |<------xOffset---------->|      
//
// --------------------------------   ---       ---      
// |                         |    |    |      yOffset
// |                      ---.--- |    |        --- 
// |                         |    | dispHeight          
// |                              |    |         
// |                              |    |         
// --------------------------------   ---          
//                                                  
// |<-------- dispWidth --------->|                    
//
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// SetPosition
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::SetToolbarPosition()
    {
    TRACER("CGlxScreenFurniture::SetToolbarPosition()");
    
    TSize displaySize(iParentUtility.DisplaySize());
    TInt dispWidth = displaySize.iWidth;
    TInt dispHeight = displaySize.iHeight;

    // Please refer to the figure above for a detailed description
    TInt xOffset = dispWidth - KGlxFloatingToolbarXPosOffset;
    TInt yOffset = (dispHeight/2) - KGlxFloatingToolbarYPosOffset;
    iToolbar->SetPosition(TPoint( xOffset, yOffset ) );
   
    }

// -----------------------------------------------------------------------------
// SetTooltipL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxScreenFurniture::SetTooltipL( TInt aCommand, 
        CAknButton::TTooltipPosition aToolTipPos, const TDesC& aToolTipText )
    {
    TRACER("CGlxScreenFurniture::SetTooltipL()");
    
    // Get the button reference of the command specified
    CAknButton* toolbarButton = static_cast<CAknButton*>
                                (iToolbar->ControlOrNull(aCommand));
                                
    if( toolbarButton )
        {
        if( aToolTipText != KNullDesC )
            {
            // Set tooltip text
            toolbarButton->State()->SetHelpTextL(aToolTipText);
            }
        toolbarButton->SetTooltipPosition( aToolTipPos );
        }
    }

// -----------------------------------------------------------------------------
// OfferToolbarEventL
// -----------------------------------------------------------------------------
//
void CGlxScreenFurniture::OfferToolbarEventL( TInt aCommand )
    {
    TRACER("CGlxScreenFurniture::OfferToolbarEventL()");
    
    // This class does not have access to a CEikonEnv and hence 
    // pls ignore the code scanner warning - Using CEikonEnv::Static
    MEikCommandObserver* commandObserver = CEikonEnv::Static()->EikAppUi();
    commandObserver->ProcessCommandL( aCommand );    
    }

// -----------------------------------------------------------------------------
// SetToolbarObserver
// -----------------------------------------------------------------------------
//
void CGlxScreenFurniture::SetToolbarObserver( MAknToolbarObserver* aObserver )
    {
    TRACER("CGlxScreenFurniture::SetToolbarObserver()");
    
    // Add ScreenFurniture to ToolbarObserver
    iToolbar->SetToolbarObserver(aObserver);    
    }

// -----------------------------------------------------------------------------
// ChangeMskIdL
// -----------------------------------------------------------------------------
//    
EXPORT_C void CGlxScreenFurniture::ModifySoftkeyIdL(CEikButtonGroupContainer::TCommandPosition aPosition, 
        TInt aCommandId, TInt aResourceId, const TDesC& aText)
    {
    TRACER("CGlxScreenFurniture::ChangeMskIdL(TInt aCommandId, TDesC& aText)"); 
    if (!AknLayoutUtils::PenEnabled())
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if (!aResourceId)
            {
            cba->SetCommandL(aPosition, aCommandId, aText );
            cba->DrawNow();
            }
        else
            {
            HBufC* textbuf =  StringLoader::LoadLC(aResourceId);
            TPtr msktext = textbuf->Des();
            cba->SetCommandL(aPosition, aCommandId, msktext );
            cba->DrawNow();
            CleanupStack::PopAndDestroy(textbuf);
            }
        }
    }


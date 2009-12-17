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
* Description:    Text entry popup class for entering textual input
*
*/




/**
 * @internal reviewed 06/06/2007 by Dave Schofield
 */
 
#include "glxtextentrypopup.h"

#include <featmgr.h>

#include "glxuiutility.h"
#include <glxtracer.h>
#include "glxsetappstate.h"
#include <glxuiutilities.rsg>

/**
 * TGlxTextEntryPopupRestoreState
 * Contains state information to be
 * restored when CGlxTextEntryPopup is
 * dismissed or if CAknTextQueryDialog::ExecuteLD
 * leaves
 */
class TGlxTextEntryPopupRestoreState
    {
public:
    /**
     * Default constructor
     */
    TGlxTextEntryPopupRestoreState();

public:
    /** Pointer to the HUI utility (not owned) */
    CGlxUiUtility* iUiUtility;

    /** Previous screen orientation */
    TGlxOrientation iPreviousOrientation;

    /** Previous application state */
    TGlxAppState iPreviousAppState;
    };

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
TGlxTextEntryPopupRestoreState::TGlxTextEntryPopupRestoreState()
	{
	iUiUtility = NULL;
	iPreviousOrientation = EGlxOrientationDefault;
	iPreviousAppState = EGlxAppStateUndefined;
	}

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxTextEntryPopup* CGlxTextEntryPopup::NewL(const TDesC& aTitle, TDes& aText)
	{
	CGlxTextEntryPopup* self = new (ELeave) CGlxTextEntryPopup(aText);
	CleanupStack::PushL(self);
	self->ConstructL(aTitle);
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxTextEntryPopup::CGlxTextEntryPopup(TDes& aText) : CAknTextQueryDialog(aText)
	{
	}

// -----------------------------------------------------------------------------
// ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxTextEntryPopup::ExecuteLD()
    {
    TRACER( "CGlxTextEntryPopup::ExecuteLD" );

    TGlxTextEntryPopupRestoreState rollbackState;
    // attempt to get current PCFW app state
    rollbackState.iPreviousAppState = GlxSetAppState::AppState();
    // set app state to text entry
    GlxSetAppState::SetState(EGlxInTextEntry);

    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL( *uiUtility );

    // create cleanup stack item to  revert previous state if dlg leaves
    TCleanupItem cleanupItem( RollbackState, &rollbackState );
    CleanupStack::PushL( cleanupItem );

    rollbackState.iUiUtility = uiUtility;
    rollbackState.iPreviousOrientation = uiUtility->AppOrientation();
        
    // If Device supports text-entry mode only in portrait,
    // then the orientation has to be forced to EGlxOrientationTextEntry here.     
    if(uiUtility->IsPenSupported()) 
	    {
	    uiUtility->SetAppOrientationL( EGlxOrientationDefault );
	    }
    else
	    {
	    uiUtility->SetAppOrientationL( EGlxOrientationTextEntry );
	    }       
      
    TInt retVal = CAknTextQueryDialog::ExecuteLD(R_GLX_TEXT_ENTRY_QUERY);

    CleanupStack::PopAndDestroy( &rollbackState ); // cleanupItem
    CleanupStack::PopAndDestroy( uiUtility );

    return retVal;
    }

// -----------------------------------------------------------------------------
// UpdateLeftSoftKeyL
// -----------------------------------------------------------------------------
//
void CGlxTextEntryPopup::UpdateLeftSoftKeyL()
    {
    //Check whether the entered text length is bigger than zero
    if( !CheckIfEntryTextOk () && !iAllowEmptyString)
        {        
        MakeLeftSoftkeyVisible(EFalse);            
        }
    else 
        {
        MakeLeftSoftkeyVisible(ETrue);
        }
    }

// -----------------------------------------------------------------------------
// SetLeftSoftKeyL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxTextEntryPopup::SetLeftSoftKeyL(TBool aAllowEmptyString)
    {
    iAllowEmptyString = aAllowEmptyString;
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxTextEntryPopup::ConstructL(const TDesC& aTitle)
	{
	SetPromptL(aTitle);
	}

// -----------------------------------------------------------------------------
// RollbackState
// -----------------------------------------------------------------------------
//	
void CGlxTextEntryPopup::RollbackState(TAny* aParam)
    {
    TRACER( "CGlxTextEntryPopup::RollbackState" );
    TGlxTextEntryPopupRestoreState* restoreState
                    = static_cast<TGlxTextEntryPopupRestoreState*>( aParam );

    // Rollback app state to previous state
    GlxSetAppState::SetState( restoreState->iPreviousAppState );

    if ( restoreState->iUiUtility)
        {
        TRAP_IGNORE( restoreState->iUiUtility->SetAppOrientationL(
                                    restoreState->iPreviousOrientation ) );
        }
    }

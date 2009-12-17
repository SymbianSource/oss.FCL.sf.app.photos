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
* Description:    Base class for timed animations
*
*/




/**
 * @internal reviewed 12/07/2007 by Michael Yip
 */

#include "glxanimationtimed.h"
#include "mglxanimationobserver.h"
#include "glxuiutility.h"

#include <alf/alfenv.h>
#include <alf/alfevent.h>

const TInt KGlxEventAnimationComplete = 0;

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxAnimationTimed::CGlxAnimationTimed()
    {
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxAnimationTimed::~CGlxAnimationTimed()
    {
    if ( iUiUtility )
        {
        iUiUtility->Env()->CancelCommands(this);
        
        iUiUtility->Close();
        iUiUtility = NULL;
        }
    }

// -----------------------------------------------------------------------------
// AnimateL
// -----------------------------------------------------------------------------
//
void CGlxAnimationTimed::AnimateL( TInt aTime, MGlxAnimationObserver* aObserver )
    {
    if ( aObserver )
        {
        iObserver = aObserver;

        // if we dont yet have ui utility handle, get it now
        if ( !iUiUtility )
            {
            iUiUtility = CGlxUiUtility::UtilityL();
            }

        // send event once the animation should complete        
        iUiUtility->Env()->Send( 
            TAlfCustomEventCommand( KGlxEventAnimationComplete, this ), aTime );
        }

    // call the abstract method
    StartTimedAnimationL( aTime );
    }
    
// -----------------------------------------------------------------------------
// OfferEventL
// -----------------------------------------------------------------------------
//
TBool CGlxAnimationTimed::OfferEventL( const TAlfEvent &aEvent )
    {
    TBool consumed = EFalse;
    
    if ( aEvent.IsCustomEvent() )
        {
        // Rowland (30/10/2007) Changed iParam to CustomParameter()
        if ( KGlxEventAnimationComplete == aEvent.CustomParameter() )
            {
            // we consumed the event
            consumed = ETrue;

            if ( iObserver )
                {
                iObserver->AnimationComplete( this );
                }
            // IMPORTANT!
            // No code after this! Client may delete the animation!
            }
        }
        
    return consumed;
    }

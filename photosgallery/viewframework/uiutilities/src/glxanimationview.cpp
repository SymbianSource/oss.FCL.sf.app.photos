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
* Description:    View-transition animations
*
*/




/**
 * @internal reviewed 12/07/2007 by Michael Yip
 */

#include "glxanimationview.h"

#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alftransformation.h>


const TReal KGlxViewSwitchBackgroundScalingFactor = 0.2;
const TReal KGlxViewSwitchForegroundScalingFactor = 0.3;



// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxAnimationView* CGlxAnimationView::NewL( 
        TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection, 
        const RPointerArray<CAlfControlGroup>& aControlGroups)
    {
    CGlxAnimationView* self = new (ELeave) CGlxAnimationView(aAnimType, aDirection);
    CleanupStack::PushL(self);
    self->ConstructL(aControlGroups);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxAnimationView::CGlxAnimationView(
        TGlxViewswitchAnimation aAnimType, 
        TGlxNavigationDirection aDirection)
:   iAnimType(aAnimType),
    iDirection(aDirection)
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxAnimationView::ConstructL(const RPointerArray<CAlfControlGroup>& aControlGroups)
    {
    TInt count = aControlGroups.Count();
    
    for (TInt index = 0; index < count; index++)
        {
        iControlGroups.AppendL(aControlGroups[index]);
        }
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxAnimationView::~CGlxAnimationView()
    {
    iControlGroups.Close();
    }
    
// -----------------------------------------------------------------------------
// StartTimedAnimationL
// -----------------------------------------------------------------------------
//
void CGlxAnimationView::StartTimedAnimationL( TInt aTime )
    {
    // Set up timed values
    TAlfTimedValue scale( 1.0 );
    TAlfTimedValue opacity( 1.0 );

    // Set the scaling effect    
    if ( EGlxNavigationForwards == iDirection )
        {
        if( EGlxViewAnimationEntry == iAnimType )
            {
            scale.SetValueNow( 1 - KGlxViewSwitchBackgroundScalingFactor );
            scale.SetTarget( 1, aTime ); // and target
            }
        else if ( EGlxViewAnimationExit == iAnimType )
            {
            scale.SetValueNow( 1 );
            scale.SetTarget( 1 + KGlxViewSwitchForegroundScalingFactor, aTime ); // and target
            }
        }
    else if ( EGlxNavigationBackwards == iDirection )
        {
        if( EGlxViewAnimationEntry == iAnimType )
            {
            scale.SetValueNow( 1 + KGlxViewSwitchForegroundScalingFactor );
            scale.SetTarget( 1, aTime ); // and target
            }
        else if ( EGlxViewAnimationExit == iAnimType )
            {
            scale.SetValueNow( 1 );
            scale.SetTarget( 1 - KGlxViewSwitchBackgroundScalingFactor, aTime ); // and target
            }
        }

    // Set the opacity effect
    if( EGlxViewAnimationEntry == iAnimType )
        {
        opacity.SetValueNow( 0 );
        opacity.SetTarget(1.0, aTime );
        }
    else if ( EGlxViewAnimationExit == iAnimType )
        {
        opacity.SetValueNow( 1.0 );
        opacity.SetTarget(0.0, aTime );
        }
    opacity.SetStyle(EAlfTimedValueStyleLinear);

    // Find out the screen size
    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
    CleanupClosePushL( *uiUtility );
    //TSize dispSize = uiUtility->ScreenFurniture().CentralScreenRect().Size();// tobe implemented by sf
    CleanupStack::PopAndDestroy( uiUtility );

    // Animate each of the control groups
    TInt cgCount = iControlGroups.Count();
    for ( TInt cg = 0; cg < cgCount; cg++ )
        {
        // If exiting, disable input events
        if ( EGlxViewAnimationExit == iAnimType )
            {
            iControlGroups[cg]->SetAcceptInput( EFalse );
            }

        // Set up control group transformations
        iControlGroups[cg]->EnableTransformationL();
        CAlfTransformation& trans = iControlGroups[cg]->Transformation();

        //trans.Translate(dispSize.iWidth/2, dispSize.iHeight/2);//tobe implemented by sf
        trans.Scale( scale, scale );
       // trans.Translate(-dispSize.iWidth/2, -dispSize.iHeight/2);//tobe implemented by sf
        
        // Set opacity of all toplevel visuals in all controls in this group
        TInt controlCount = iControlGroups[cg]->Count();
        for(TInt i = 0; i < controlCount; i++ )
            {
            CAlfControl& control = iControlGroups[cg]->Control(i);
            for(TInt j = 0; j < control.VisualCount(); j++)
                {
                if ( control.Visual(j).Layout() )
                    {
                    control.Visual(j).SetOpacity( opacity );
                    }
                }
            }
        }
    }



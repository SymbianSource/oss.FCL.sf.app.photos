/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The pause handling control for the slideshow
 *
*/



 
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfevent.h> 

#include <glxlog.h>
#include <glxtracer.h>
#include <glxpanic.h>
#include "shwslideshowengine.h"
#include "shwgesturecontrol.h"

using namespace GestureHelper;


// -----------------------------------------------------------------------------
// NewL.
// -----------------------------------------------------------------------------
//
CShwGestureControl* CShwGestureControl::NewL( CAlfEnv& aEnv,CAlfDisplay& aDisplay)
	{
	TRACER("CShwGestureControl::NewL");
	GLX_LOG_INFO( "CShwGestureControl::NewL" );
	CShwGestureControl* self = new(ELeave)CShwGestureControl();
	CleanupStack::PushL( self );
	self->ConstructL( aEnv,aDisplay);
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
//
void CShwGestureControl::ConstructL( CAlfEnv& aEnv,CAlfDisplay& aDisplay)
	{
	TRACER("CShwGestureControl::ConstructL");
	GLX_LOG_INFO( "CShwGestureControl::ConstructL" );
	CAlfControl::ConstructL(aEnv);    
    BindDisplay(aDisplay);
	}

// -----------------------------------------------------------------------------
// CShwGestureControl.
// -----------------------------------------------------------------------------
//
CShwGestureControl::CShwGestureControl( )
	{
	TRACER("CShwGestureControl::CShwGestureControl");
	//no implementation
	}

// -----------------------------------------------------------------------------
// ~CShwGestureControl.
// -----------------------------------------------------------------------------
//  
CShwGestureControl::~CShwGestureControl()
    {
	TRACER("CShwGestureControl::~CShwGestureControl");
	iObservers.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// AddObserverL
// -----------------------------------------------------------------------------
//
void CShwGestureControl::AddObserverL(MShwGestureObserver* aObserver) 
    {
    TRACER("CShwGestureControl::AddObserverL");
    GLX_LOG_INFO("CShwGestureControl::AddObserverL");
    __ASSERT_DEBUG( NULL != aObserver , Panic(EGlxPanicNullPointer)); 
    
    // dont want to observe the same thing again and again.
    if (iObservers.Find(aObserver) == KErrNotFound)
        {
        iObservers.AppendL(aObserver);
        GLX_LOG_INFO1("CShwGestureControl::AddObserverL  Observer Added "
                "observer count now [%d]", iObservers.Count());
        }
    }

// -----------------------------------------------------------------------------
// RemoveObserver
// -----------------------------------------------------------------------------
//
void CShwGestureControl::RemoveObserver(MShwGestureObserver* aObserver)
    {
    TRACER("CShwGestureControl::RemoveObserver");
    GLX_LOG_INFO("CShwGestureControl::RemoveObserver");
    TInt observerPosition = iObservers.Find(aObserver);
    if (observerPosition != KErrNotFound)
        {
        iObservers.Remove(observerPosition);
        GLX_LOG_INFO1("CShwGestureControl::RemoveObserver  One Observer removed "
                "observer count now [%d]", iObservers.Count());
        }
    }

// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
void CShwGestureControl::HandleGestureL( const GestureHelper::MGestureEvent& aEvent )
	{
	TRACER("CShwGestureControl::HandleGestureL");
	GLX_LOG_INFO1( "CShwGestureControl::HandleGestureL(%d)", aEvent.Code( MGestureEvent::EAxisBoth ));

	// No one is listening, Do not resolve the events for the listeners 
	if (iObservers.Count() > 0)
	    {
        MShwGestureObserver::TShwGestureEventType aType  = MShwGestureObserver::ENoEvent;
		switch ( aEvent.Code( MGestureEvent::EAxisBoth ) )
		    {
	    	case EGestureSwipeLeft:
	    		{
	    		aType  = MShwGestureObserver::ESwipeLeft ;
	    		break;
	    		}
	    	case EGestureSwipeRight:  
	    		{
	    		aType  = MShwGestureObserver::ESwipeRight;
	    		break;
	    		}
		    case EGestureTap:
		    	{
		    	aType  = MShwGestureObserver::ETapEvent;
		    	break;
		    	}
		    default: 
		    	{
		    	// we are interested in only 
		    	// swipe left(EGestureSwipeLeft) 
		    	// swipe right(EGestureSwipeRight)
		    	// and tap events
		    	// we wont be handling any other case yet. 
		    	break;
		    	}
		    }
        for (TInt index = 0 ; index < iObservers.Count() ; index ++)
            {
            if (MShwGestureObserver::ENoEvent != aType )
                {
                iObservers[index]->HandleShwGestureEventL(aType);
                }
            }
	    }
	}

//end of file
 

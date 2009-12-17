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
#include "shwslideshowengine.h"
#include "shwgesturecontrol.h"

using namespace GestureHelper;



// -----------------------------------------------------------------------------
// NewL.
// -----------------------------------------------------------------------------
//
CShwGestureControl* CShwGestureControl::NewL( CAlfEnv& aEnv,CAlfDisplay& aDisplay,
										MShwGestureObserver& aObserver)
	{
	TRACER("CShwGestureControl::NewL");
	GLX_LOG_INFO( "CShwGestureControl::NewL" );
	CShwGestureControl* self = new(ELeave)CShwGestureControl(aObserver);
	CleanupStack::PushL( self );
	self->ConstructL( aEnv,aDisplay);
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
//
void CShwGestureControl::ConstructL( CAlfEnv& aEnv,CAlfDisplay& aDisplay )
	{
	TRACER("CShwGestureControl::ConstructL");
	GLX_LOG_INFO( "CShwGestureControl::ConstructL" );
	//CGestureControl::ConstructL(*this,aEnv,aDisplay,KShwgestureEventControlGroup);
	CAlfControl::ConstructL(aEnv);    
    BindDisplay(aDisplay);
	}

// -----------------------------------------------------------------------------
// CShwGestureControl.
// -----------------------------------------------------------------------------
//
CShwGestureControl::CShwGestureControl( MShwGestureObserver& aObserver):
					iObserver(aObserver)
	{
	//no implementation
	}

// -----------------------------------------------------------------------------
// ~CShwGestureControl.
// -----------------------------------------------------------------------------
//	
CShwGestureControl::~CShwGestureControl()
	{
	//no implementation
	}

// -----------------------------------------------------------------------------
// HandleGestureL.
// -----------------------------------------------------------------------------
//
void CShwGestureControl::HandleGestureL( const GestureHelper::MGestureEvent& aEvent )
	{
	TRACER("CShwGestureControl::HandleGestureL");
	GLX_LOG_INFO1( "CShwGestureControl::HandleGestureL(%d)", aEvent.Code( MGestureEvent::EAxisBoth ));
	// we are interested in only 
	// swipe left(EGestureSwipeLeft) 
	// swipe right(EGestureSwipeRight)
	// and tap events
	
		switch ( aEvent.Code( MGestureEvent::EAxisBoth ) )
		    {
		    	
	    	case EGestureSwipeLeft:
	    		{
	    		//call back the view
				iObserver.HandleShwGestureEventL(MShwGestureObserver::ESwipeLeft);
	    		break;
	    		}
	    	case EGestureHoldLeft:
	    		{
				//skip for now
	    		break;
	    		}
	    	case EGestureSwipeRight:  
	    		{
	    		//callback
				iObserver.HandleShwGestureEventL(MShwGestureObserver::ESwipeRight);
	    		break;
	    		}
		    case EGestureHoldRight:
	    		{
				//skip for now
	    		break;
	    		}
		    case EGestureTap:
		    	{
	    		iObserver.HandleShwGestureEventL(MShwGestureObserver::ETapEvent);
		    	break;
		    	}
	    	//fall through
		    case EGestureSwipeUp:
	  	    case EGestureHoldUp:
		    case EGestureSwipeDown:
		    case EGestureHoldDown:       
		    default: 
		    	{
		    	break;
		    	}
		        
		    }
	}

//end of file
 

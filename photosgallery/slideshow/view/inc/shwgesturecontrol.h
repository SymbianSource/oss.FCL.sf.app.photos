/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The settings model for the slideshow
 *
*/


#ifndef SHWGESTURECONTROL_H
#define SHWGESTURECONTROL_H


#include <gestureobserver.h>
#include <gesturecontrol.h>
#include "shwgestureobserver.h"

//namespace
using namespace GestureHelper;

//Forward declaration
class CAlfEnv;
class CGestureHelper;
class MGestureObserver;
class CShwSlideshowEngine;

/**
* CShwGestureControl is used to enable handling of gesture events in slide show
* Gesture support is necessary to handle swipes in "Manual Mode" 
* @ MC Photos Slideshow UI spec DeSW R8 v. 1.0.doc
* Not exposed, for internal use of Slide show
* For documentation Gesture Helper Classes ,
* see @ref MGestureObserver.
* see @ref CGestureControl
*/

NONSHARABLE_CLASS( CShwGestureControl ) : public CAlfControl, 
                     public GestureHelper::MGestureObserver
	{	
	
public: 

	/**
	 * NewL
	 * static function for 2-phased object creation
	 * NewLC not provided as the CShwGestureControl is primarily intended to be a
	 * member variable
	 * @param aEngine, handle to the slide show engine
	 * @param aEnv, handle to CAlfEnv 
	 * @param aDisplay, handle to CAlfDisplay
	 */ 
	 static CShwGestureControl* NewL(CAlfEnv& aEnv,CAlfDisplay& aDisplay); 
         
	/**
	 * HandleGestureL
	 * From MGestureObserver
	 * see @ref MGestureObserver
	 */ 
    void HandleGestureL( const GestureHelper::MGestureEvent& aEvent );
    
	/**
	 * ~CShwGestureControl
	 * standard C++ destructor 
	 */ 
    ~CShwGestureControl();
    
    /**
     * AddObserverL
     * Add an Observer 
     * @param aObserver, Observer to be added. 
     */ 
    void AddObserverL(MShwGestureObserver* aObserver); 

    /**
     * RemoveObserver
     * Remove an Observer  
     * @param aEngine, handle to the slide show engine
     */ 
    void RemoveObserver(MShwGestureObserver* aObserver);
    
private:

	/**
	 * CShwGestureControl
	 * C++ constructor
	 */
	CShwGestureControl();
	
	/**
	 * ConstructL
	 * 2nd Phase constructor
	 */ 
	void ConstructL( CAlfEnv& aEnv,CAlfDisplay& aDisplay ); 
	
private:

    /** Array of observers (does not own ) */
	// As per present design of the SS view, there is need only for one observer, 
	// but this is more generic. Also this doesnt seem like a perfomance critical path 
    RPointerArray<MShwGestureObserver> iObservers;
	};


#endif //SHWGESTURECONTROL_H


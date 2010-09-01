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
* Description:    CoeControl utility for monitoring skin change
*
*/





#ifndef C_GLXSKINCHANGEMONITOR_H
#define C_GLXSKINCHANGEMONITOR_H

#include <coecntrl.h>
#include <AknUtils.h>
#include "mglxskinchangeobserver.h"

/**
 *  CGlxSkinChangeMonitor
 *
 *  Utility to handle skin changes.
 *
 *  @lib 
 *
 *  @internal reviewed 
 */
class CGlxSkinChangeMonitor : public CCoeControl
	{
	
	/* for unit testing */
	friend class t_glxskinchangemonitor;
	friend class t_cglxhuiutility;
	
	public: // the API

	    virtual ~CGlxSkinChangeMonitor();

	    /**
         * Two-phased constructor.
         *
         * @return Pointer
         */
		static CGlxSkinChangeMonitor* NewL();
	
     

	private:	// Implementation

	    /**
	     * C++ constructor 
	     * @param aEnv the Hui environment
	     */
	    CGlxSkinChangeMonitor();
	    
	    
	    /**
        * Symbian 2nd phase constructor
        */
	    void ConstructL();

        
    public:
		/**
		 * Handles resource change. From CCoeControl
		 *
		 * @ref CCoeControl::HandleResourceChange
		 */ 
		void HandleResourceChange( TInt aType );
		
		/**
		 *Add MGlxSkinChangeObserver to skin change observers array. 
		 *@param aObserver Reference of the MGlxSkinChangeObserver.
		 */
		 void AddSkinChangeObserverL( MGlxSkinChangeObserver& aObserver );
		 
		/**
		 *Remove MGlxSkinChangeObserver from skin change observers array. 
		 *@param aObserver Reference of the MGlxSkinChangeObserver.
		 */
		 void RemoveSkinChangeObserver( MGlxSkinChangeObserver& aObserver );

    private:	
    	
    	/**
         * Observers that will be notified if skin is changed
         * owned
         */
        RPointerArray<MGlxSkinChangeObserver> iSkinChangeObservers; 

	};

#endif // C_GLXUIUTILITYCOECONTROL_H


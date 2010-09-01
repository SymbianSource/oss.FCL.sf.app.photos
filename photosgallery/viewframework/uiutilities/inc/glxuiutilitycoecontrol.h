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
* Description:    CoeControl utility
*
*/




#ifndef C_GLXUIUTILITYCOECONTROL_H
#define C_GLXUIUTILITYCOECONTROL_H

#include <aknutils.h>
#include <uiacceltk/huidisplaycoecontrol.h>
#include <uiacceltk/huienv.h>

/**
 *  CGlxUiUtilityCoeControl
 *
 *  Utility to handle display orientation changes.
 *
 *  @lib glxhuiutils.lib
 *
 *  @internal reviewed 12/06/2007 by Dave Schofield
 */
NONSHARABLE_CLASS( CGlxUiUtilityCoeControl ) : public CHuiDisplayCoeControl
	{
	public: // the API

	    /**
	     * Static factory method for constructing a new instance of
	     * CGlxUiUtilityCoeControl.
	     * @param aEnv the HUI environment
	     * @param aArea the area for the control 
	     *					@ref AknLayoutUtils::TAknLayoutMetrics
	     * @param aFlags the screen creation flag 
	     *					@ref CHuiEnv::TNewDisplayFlags
	     */
		static CGlxUiUtilityCoeControl* NewL(
			CHuiEnv& aEnv, 
			AknLayoutUtils::TAknLayoutMetrics aArea = AknLayoutUtils::EScreen, 
			TInt aFlags = CHuiEnv::ENewDisplayFullScreen );

        /**
         * Destructor
         */
        ~CGlxUiUtilityCoeControl();

        /**
		 * Set the area for the control.
	     * @param aArea the area for the control 
	     *					@ref AknLayoutUtils::TAknLayoutMetrics
		 * @return aRect the rect corresponding to the area
		 */
		TRect SetArea( AknLayoutUtils::TAknLayoutMetrics aArea );

	private:	// Implementation

	    /**
	     * C++ constructor 
	     * @param aEnv the Hui environment
	     */
	    CGlxUiUtilityCoeControl( CHuiEnv& aEnv );

	    /**
	     * Second-phase constructor
	     * @param aArea the area for the control 
	     *					@ref AknLayoutUtils::TAknLayoutMetrics
	     * @param aFlags the screen creation flag 
	     *					@ref CHuiEnv::TNewDisplayFlags
	     */
	    void ConstructL( AknLayoutUtils::TAknLayoutMetrics aArea, TInt aFlags );

		/**
		 * Handles resource change. From CCoeControl
		 *
		 * @ref CCoeControl::HandleResourceChange
		 */ 
		void HandleResourceChange( TInt aType );

	private:
	
		/// Own: the area for the display
		AknLayoutUtils::TAknLayoutMetrics iArea;
		
        /** Pointer to the appui (not owned) */
        CCoeAppUi* iAppUi;

	};


#endif // C_GLXUIUTILITYCOECONTROL_H


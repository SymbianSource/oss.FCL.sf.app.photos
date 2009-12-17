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
* Description:    Utility that contains HUI related slideshow code
 *
*/




#ifndef __SHWHUIUTILITY_H__
#define __SHWHUIUTILITY_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <alf/alfdisplay.h>

// Forward declarations
class MGlxVisualList;
class CHuiDisplay;

// CLASS DECLARATION

/**
 * ShwHuiUtility. 
 * Isolates HUI runtime dependency so that tests can stub it
 * @lib shwslideshowengine.lib
 */
NONSHARABLE_CLASS( ShwUiUtility )
	{
	private: // Constructors and destructor

		/**
		 * C++ constructor. Not to be used.
		 */
		ShwUiUtility();
		
		/**
		 * Destructor. Not to be used.
		 */
		~ShwUiUtility();

	public: // the API

		/**
		 * This method shows the given visual list on the HUI display
		 * @param aDisplay the HUI display to use
		 * @param aVisualList the visual list to display
		 */
		static void ShowVisualListL( CAlfDisplay* aDisplay, MGlxVisualList* aVisualList );

	};

#endif // __SHWHUIUTILITY_H__

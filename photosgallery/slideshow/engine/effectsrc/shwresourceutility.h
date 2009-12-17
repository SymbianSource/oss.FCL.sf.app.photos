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
* Description:    Localisation utility for UI visible strings
 *
*/




//  Include Files
#include <e32std.h>
#include <shwslideshowengine.rsg>

/**
 * Utility to handle resource file loading.
 * Resource file contains the localised effect names
 */
NONSHARABLE_CLASS( ShwResourceUtility )
	{
	public:

		/**
		 * Returns a localised name for the given resource id
		 * @param aResourceId the id for the string
		 * @return the localised name or NULL if string not found
		 */
		static HBufC* LocalisedNameL( TInt aResourceId );
	};

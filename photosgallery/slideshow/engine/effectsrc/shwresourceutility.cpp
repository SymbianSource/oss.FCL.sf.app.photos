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





//  CLASS HEADER
#include "shwresourceutility.h"

//  EXTERNAL INCLUDES
#include <StringLoader.h>
#include <AknUtils.h>
#include <bautils.h>						// for BaflUtils
#include <data_caging_path_literals.hrh> 	// for KDC_APP_RESOURCE_DIR

//  INTERNAL INCLUDES
#include <glxresourceutilities.h>           // for CGlxResourceUtilities
#include <glxlog.h>
#include <glxtracer.h>

_LIT(KShwSlideshowEngineResource, "shwslideshowengine.rsc");

// -----------------------------------------------------------------------------
// LocalisedNameL
// -----------------------------------------------------------------------------
HBufC* ShwResourceUtility::LocalisedNameL(TInt aResourceId)
	{
	TRACER(" ShwResourceUtility::LocalisedNameL");
	GLX_LOG_INFO( "ShwResourceUtility::LocalisedNameL" );
	// Load the engine's resource
    TFileName resourceFile( KDC_APP_RESOURCE_DIR );
	// append resource file name
    resourceFile.Append( KShwSlideshowEngineResource ); 
	// add the application path
    CGlxResourceUtilities::GetResourceFilenameL( resourceFile );
	// add the resource to CCoeEnv
	TInt id = CCoeEnv::Static()->AddResourceFileL( resourceFile );
	// read the string and return it
	HBufC* tmp  = CCoeEnv::Static()->AllocReadResourceL( aResourceId );
	// remove the resource as its not needed anymore
	CCoeEnv::Static()->DeleteResourceFile( id );
	// return the string and ownership
	return tmp;
	}

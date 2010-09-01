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
* Description:    Metadata dialog implementation
*
*/





#ifndef GLXMETADATAVIEWUTILITY_H_
#define GLXMETADATAVIEWUTILITY_H_

// INCLUDES
#include <e32std.h>

/**
 *  Metadata view utility
 * 
 *  Utility for creating/launching metadata view from gallery and other application
 *
 *  @lib glxunifiedmetadataview.lib
 */
 
 class GlxMetadataViewUtility
 	{
 public:
 	
 	/**
 	 * Activate Metadata view for item.
 	 * 
 	 * @param aURI filename or URI of item
 	 */	
 	IMPORT_C static void ActivateViewL( const TDesC& aURI );	
 	};

#endif /*C_GLXMETADATAVIEWUTILITY_H_*/

//End of file


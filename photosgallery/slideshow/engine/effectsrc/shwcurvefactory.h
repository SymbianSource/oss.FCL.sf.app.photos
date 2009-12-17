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
* Description:    Zoom and pan effect curve factory
 *
*/




// Include Files
#include <e32std.h>
#include <alf/alfcurvepath.h>

#ifndef __SHWCURVEFACTORY_H__
#define __SHWCURVEFACTORY_H__

// Forward declares
class CAlfCurvePath;

/**
 * NShwCurveFactory.
 * Grouping of the slideshow curve creation.
 * This is in a namespace to not pollute global namespace and not 
 * a class as its just a "c-style" function, so no benefit of being a class.
 * @internal reviewed 06/06/2007 by Dan Rhodes
 */
namespace NShwCurveFactory
	{
	/**
	 * @param aSize the width and height of the ellipsis
	 * @param aLenght the length of the ellipsis
	 * @return the HUI curvepath that defines the ellipsis
	 */
	CAlfCurvePath* CreateEllipsisL( TSize aSize, TInt aLength );
	}

#endif // __SHWCURVEFACTORY_H__

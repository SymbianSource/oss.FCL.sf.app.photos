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
* Description:    Layout base classes
*
*/




#ifndef __GLXLAYOUTINFO_INL__
#define __GLXLAYOUTINFO_INL__

#include <glxpanic.h> // Panic codes

class CAlfVisual;
inline TInt TGlxLayoutInfo::Index() const
	{
	return iIndex;
	}

inline CAlfVisual& TGlxLayoutInfo::Visual()
	{
	// visual must never be null when doing layout
//	__ASSERT_ALWAYS( iVisual, Panic( EGlxPanicNullCAlfVisual ) );
	return *iVisual;
	}

#endif // __GLXLAYOUTINFO_INL__


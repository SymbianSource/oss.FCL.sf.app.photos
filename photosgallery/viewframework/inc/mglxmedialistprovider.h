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
* Description:   To Get a medialist Pointer 
*
*/




#ifndef M_GLXMEDIALISTPROVIDER_H
#define M_GLXMEDIALISTPROVIDER_H

class MGlxMediaList;

/**
 * Interface to allow a class that owns a media list to share access to it
 */
class MGlxMediaListProvider
	{
public:
	/**
         * Returns a pointer to the owned media list. Ownership is not tranferred.
         */
	virtual MGlxMediaList& MediaList() = 0;
	};

#endif // M_GLXMediaListProvider_H

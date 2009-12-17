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
* Description:    Media List provider interface
*
*/




#ifndef M_GLXSMediaListFactory_H
#define M_GLXSMediaListFactory_H

class MGlxMediaList;
class MMPXCollectionUtility;

/**
 *  Provider of a media list
 */
class MGlxMediaListFactory 
	{
public:
	/**
	 * Creates and returns a Media List. Ownership of Media List is transfered.
	 * @param aCollectionUtility The collection utility to use when creating the media list
	 * @return The Media List (ownership transfered to caller)
	 */
	IMPORT_C virtual MGlxMediaList& CreateMediaListL(MMPXCollectionUtility& aCollectionUtility) const;
	
protected:
	/**
	 * Creates media list based on collection path
	 * @param aCollectionUtility The collection utility to use when creating the media list
	 * @return The Media List (ownership transfered to caller)
	 */
	IMPORT_C MGlxMediaList& CurrentListL(MMPXCollectionUtility& aCollectionUtility) const;
	};



#endif // M_GLXSMediaListFactory_H



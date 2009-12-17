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
* Description:    Media List provider default implementation
*
*/




#include <e32base.h>
#include "glxmedialistfactory.h"

#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include "mglxmedialist.h"


// -----------------------------------------------------------------------------
// MediaList
// -----------------------------------------------------------------------------
//	
EXPORT_C MGlxMediaList& MGlxMediaListFactory::CreateMediaListL(MMPXCollectionUtility& aCollectionUtility) const 
	{
	return CurrentListL(aCollectionUtility);
	}
	
// -----------------------------------------------------------------------------
// CurrentList
// -----------------------------------------------------------------------------
//	
EXPORT_C MGlxMediaList& MGlxMediaListFactory::CurrentListL(MMPXCollectionUtility& aCollectionUtility) const
	{
	MGlxMediaList* mediaList = NULL;
	
    // Create media list
    CMPXCollectionPath* path = aCollectionUtility.Collection().PathL();
    CleanupStack::PushL(path);
    path->Back();
    mediaList = MGlxMediaList::InstanceL(*path);
    CleanupStack::PopAndDestroy(path);

	return *mediaList;
	}

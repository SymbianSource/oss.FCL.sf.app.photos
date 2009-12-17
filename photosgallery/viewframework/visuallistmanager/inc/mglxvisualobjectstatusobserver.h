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
* Description:    Visual object status observer
*
*/




#ifndef M_GLXVISUALOBJECTSTATUSOBSERVER_H
#define M_GLXVISUALOBJECTSTATUSOBSERVER_H

#include <e32std.h>

class CGlxVisualObject;

/**
 * Observer interface for notification when visual thumbnail status changes.
 *
 * @author Dan Rhodes
 */
NONSHARABLE_CLASS( MGlxVisualObjectStatusObserver )
    {
public:
    /**
    * Called when the visual thumbnail status changes.
    * @param aObject The visual whose status has changed.
    * @param aListIndex Media list index.
    * @param aHasThumbnail Whether the thumbnail texture has content.
    */
    virtual void VisualStatusChangedL( CGlxVisualObject& aObject,
                                TInt aListIndex, TBool aHasThumbnail ) = 0;
    /**
    * Called when a thumbnail has been loaded for an item which previously
    * didn't have one.
    * @param aObject The visual whose status has changed.
    */
    virtual void ThumbnailLoadedL( CGlxVisualObject& aObject ) = 0;
    };

#endif  // M_GLXVISUALOBJECTSTATUSOBSERVER_H

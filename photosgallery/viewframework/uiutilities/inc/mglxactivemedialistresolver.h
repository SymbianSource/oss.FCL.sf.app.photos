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
* Description:    Class that resolves the active media list
*
*/




#ifndef __M_GLXACTIVEMEDIALISTRESOLVER_H__
#define __M_GLXACTIVEMEDIALISTRESOLVER_H__

#include <e32std.h>

class MGlxActiveMediaListChangeObserver;
class MGlxMediaList;

/**
 * Interface that allows client to resolve the currently active 
 * media list.
 */
class MGlxActiveMediaListResolver 
    { 
public:
    /**
     * InstanceL
     * Call Close to close the instance
     * @param aObserver Observer to add or NULL
     * @return an reference counter instance of the object
     */
    IMPORT_C static MGlxActiveMediaListResolver* InstanceL(
        MGlxActiveMediaListChangeObserver* aObserver);
    
    /**
     * Close reference count and remove observer
     * @param aObserver Observer to remove
     */ 
    virtual void Close(MGlxActiveMediaListChangeObserver* aObserver) = 0;
    
    /**
     * Returns the pointer to the currently active media list
     * The active media list is the list in the currently open view that 
     * the user is navigating, or NULL if that media list is not known
     * @return active media list, or NULL
     */ 
    virtual MGlxMediaList* ActiveMediaList() const = 0;
    };
    
#endif // __M_GLXACTIVEMEDIALISTRESOLVER_H__
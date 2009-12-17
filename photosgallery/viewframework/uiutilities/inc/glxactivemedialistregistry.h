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
* Description:    Class that stores the active media list pointer
*
*/



 
#ifndef __C_GLXACTIVEMEDIALISTREGISTRY_H__
#define __C_GLXACTIVEMEDIALISTREGISTRY_H__

#include <e32std.h>
#include <e32base.h>

#include "mglxactivemedialistresolver.h"

class MGlxMediaList;
class MGlxActiveMediaListChangeObserver;

/**
 * Class to store the active media list
 */
NONSHARABLE_CLASS( CGlxActiveMediaListRegistry ) : public CBase, public MGlxActiveMediaListResolver
    {
public:
    /**
     * Return a (singleton) instance of the interface
     * Close the instance by calling Close() when done with the instance
     * @param aObserver Observer to add
     */
    IMPORT_C static CGlxActiveMediaListRegistry* InstanceL(
        MGlxActiveMediaListChangeObserver* aObserver = NULL); 
    
    /**
     * Close the instance and remove observer if any
     * See @ref MGlxActiveMediaListResolver::Close
     * @param aObserver Observer to remove
     */
    void Close(MGlxActiveMediaListChangeObserver* aObserver = NULL);

    /**
     * Registers a media list as being active
     * Calls MGlxActiveMediaListChangeObserver::HandleActiveMediaListChanged
     * @param aMediaList list to register as active 
     */
    IMPORT_C void RegisterActiveMediaList(MGlxMediaList* aMediaList);

    /**
     * Deregisters a media list that has previously registerd as active
     * Takes a pointer to the media list, so that implementation can 
     * know if the deregistration refers to a list that has been registered 
     * before the currenlty active list has been registered.
     * The background is that the DoDeactivateL of the previous view
     * is called after the DoActivateL of the new view.
     * Note: This must be called while the media list is still alive,
     * i.e., not closed!
     * If no other media list has been registered, the currently active
     * media list will be set to NULL.
     * Calls MGlxActiveMediaListChangeObserver::HandleActiveMediaListChanged
     * @param aMediaList list to deregister as active
     */
    IMPORT_C void DeregisterActiveMediaList(MGlxMediaList* aMediaList);

    /** 
     * See @ref MGlxActiveMediaListResolver::ActiveMediaList
     * @return the active media list
     */
    MGlxMediaList* ActiveMediaList() const;

private:
    /**
     * Constructor 
     */
    CGlxActiveMediaListRegistry();

    /** 
     * Destructor 
     */
    ~CGlxActiveMediaListRegistry();
    
    /** 
     * Notify observers of change to the list pointer
     */
    void NotifyObservers();

    /**
     * 2-phase constructor
     */
    static CGlxActiveMediaListRegistry* NewL();

private:
    /** 
     * The list in the currently open view that the user is navigating, or 
     * NULL if that media list is not known 
     */    
    MGlxMediaList* iActiveMediaList;
    
    /** 
     * True if two views have registered the currently active list 
     * Allows the current and previous view to register the same list
     */
    TBool iRegisteredTwice;
    
    /** List of observers */
    RPointerArray<MGlxActiveMediaListChangeObserver> iObservers;
    };     
    
#endif // __C_GLXACTIVEMEDIALISTREGISTRY_H__
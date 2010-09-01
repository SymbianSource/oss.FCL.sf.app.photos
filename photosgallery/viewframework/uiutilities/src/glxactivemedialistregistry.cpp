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




#include "glxactivemedialistregistry.h"

#include <glxtracer.h>
#include <glxpanic.h>
#include <glxsingletonstore.h>
#include <glxlog.h>
#include "mglxactivemedialistchangeobserver.h"

// -----------------------------------------------------------------------------
// Return an instance
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxActiveMediaListRegistry* CGlxActiveMediaListRegistry::InstanceL(
        MGlxActiveMediaListChangeObserver* aObserver)
    {
    GLX_LOG_INFO1("CGlxActiveMediaListRegistry::InstanceL: %x", aObserver);
    
    CGlxActiveMediaListRegistry* obj = CGlxSingletonStore::InstanceL(&NewL);
    
    // Add the observer if has not already been added
    if (aObserver && KErrNotFound == obj->iObservers.Find(aObserver))
        {
        TRAPD(err, obj->iObservers.AppendL(aObserver));
        // Close tls reference and leave if AppendL leaves.
        // Otherwise there will be one reference that will be never closed
        if (KErrNone != err)
            {
            CGlxSingletonStore::Close(obj);
            User::Leave(err);
            }
        }
    return obj;
    }
    
// -----------------------------------------------------------------------------
// Return new object
// -----------------------------------------------------------------------------
//
CGlxActiveMediaListRegistry* CGlxActiveMediaListRegistry::NewL() 
    {
    CGlxActiveMediaListRegistry* obj = new (ELeave) CGlxActiveMediaListRegistry();
    // (No ConstructL necessary)
    return obj;    
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxActiveMediaListRegistry::CGlxActiveMediaListRegistry()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxActiveMediaListRegistry::~CGlxActiveMediaListRegistry()
    {
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// Close a reference
// -----------------------------------------------------------------------------
//
void CGlxActiveMediaListRegistry::Close(MGlxActiveMediaListChangeObserver* aObserver)
    {
    TRACER("CGlxActiveMediaListRegistry::Close");
    // Remove the observer if it has been added earlier
    TInt index = iObservers.Find(aObserver);
    if (KErrNotFound != index)
        {
        iObservers.Remove(index);
        }
        
    // Decrease reference count, and delete if last one
    // May delete this object
    CGlxSingletonStore::Close(this);
    // Member variable access not safe after CGlxTls::Close()
    }
    
// -----------------------------------------------------------------------------
// Register an active media list
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxActiveMediaListRegistry::RegisterActiveMediaList(MGlxMediaList* aMediaList)
    {
    GLX_LOG_INFO1("CGlxActiveMediaListRegistry::RegisterActiveMediaList: %x", aMediaList);
    // Add to registration count if the new list is the same as the existing one
    if (aMediaList == iActiveMediaList) 
        {
        __ASSERT_DEBUG(!iRegisteredTwice, Panic(EGlxPanicIllegalState)); // Cannot register three times
        GLX_LOG_INFO("CGlxActiveMediaListRegistry::RegisterActiveMediaList: Media list registered twice");
        // The same media list has been registered twice
        iRegisteredTwice = ETrue;
        }

    // If active list has changed, pick up the new list and notify observers
    if (iActiveMediaList != aMediaList)
        {
        GLX_LOG_INFO("CGlxActiveMediaListRegistry::RegisterActiveMediaList: Media list changed");
        // Pick up the new list
        iActiveMediaList = aMediaList;
        
        // This is the first registration
        iRegisteredTwice = EFalse;
        
        // Notify observers
        NotifyObservers();
        }
    }
 
// -----------------------------------------------------------------------------
// Deregister an active media list
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxActiveMediaListRegistry::DeregisterActiveMediaList(MGlxMediaList* aMediaList)
    {
    GLX_LOG_INFO1("CGlxActiveMediaListRegistry::DeregisterActiveMediaList: %x", aMediaList);
    // Ignore deregistration if another list has already been registered
    // (Another view was activated, and it registered a list)
    if (aMediaList == iActiveMediaList) 
        {
        // The list may be registered twice if the new and old view
        // registered the same list. 
        if (iRegisteredTwice)   
            {
            GLX_LOG_INFO("CGlxActiveMediaListRegistry::DeregisterActiveMediaList: Remove second registration");
            // Remove second registration
            iRegisteredTwice = EFalse;
            }
        else 
            {
            GLX_LOG_INFO("CGlxActiveMediaListRegistry::DeregisterActiveMediaList: Setting active list to NULL");
            // No new media list has been registered and current one is being
            // deregistered, so set active list to NULL
            iActiveMediaList = NULL;
            
            // Notify observers
            NotifyObservers();
            }
        }
    }

// -----------------------------------------------------------------------------
// Notify observers
// -----------------------------------------------------------------------------
//
void CGlxActiveMediaListRegistry::NotifyObservers() 
    {
    // Notify observers
    for (TInt i = iObservers.Count() - 1; i >= 0; i--)
        {
        iObservers[i]->HandleActiveMediaListChanged();
        }
    }
    
// -----------------------------------------------------------------------------
// Return active media list
// -----------------------------------------------------------------------------
//
MGlxMediaList* CGlxActiveMediaListRegistry::ActiveMediaList() const 
    {
    GLX_LOG_INFO("CGlxActiveMediaListRegistry::ActiveMediaList()");
    return iActiveMediaList;
    }

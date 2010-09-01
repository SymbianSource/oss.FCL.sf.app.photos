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
* Description:    Visual list manager
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#include <coemain.h>

#include <alf/alfimagevisual.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfenv.h> 
#include <alf/alfbrush.h>
#include <alf/alfcontrolgroup.h>

#include <glxlog.h>
#include <glxtracer.h>

#include "mglxmedialist.h"

#include "mglxvisuallistobserver.h" 
#include "glxitemvisual.h"
#include "glxvisuallistcontrol.h"
#include "glxvisuallistmanager.h" 

/**
 * CGlxVlmTls
 *
 * Global object stored in TLS. 
 * Owns the only instance of a Visual List Manager.
 */
struct CGlxVlmTls
	{
	CGlxVlmTls(CGlxVisualListManager* aListManager) 
		{
		iListManager = aListManager;
		iReferenceCount = 0;
		}
		
	CGlxVisualListManager* iListManager;
	TInt iReferenceCount;
	};
	
// -----------------------------------------------------------------------------
// ManagerL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxVisualListManager* CGlxVisualListManager::ManagerL()
	{
	TRACER("CGlxVisualListManager::ManagerL");
    GLX_LOG_INFO("CGlxVisualListManager::ManagerL");
	CGlxVlmTls* tls = reinterpret_cast<CGlxVlmTls*>(Dll::Tls());

	// Create tls struct if does not exist
	if ( !tls ) 
		{
		// Create list manager instance
		CGlxVisualListManager* lm = new (ELeave) CGlxVisualListManager();
		CleanupStack::PushL(lm);
      	tls = new (ELeave) CGlxVlmTls(lm);

		// MUST NOT LEAVE AFTER THIS POINT
		CleanupStack::Pop(lm);
		
        Dll::SetTls( reinterpret_cast<TAny*>(tls));
		}
		
	// Add user	
	tls->iReferenceCount++;
	
	return tls->iListManager;
	}

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxVisualListManager::Close()
	{
	
	TRACER("CGlxVisualListManager::Close");
    GLX_LOG_INFO("CGlxVisualListManager::Close");
	CGlxVlmTls* tls = reinterpret_cast<CGlxVlmTls*>(Dll::Tls());
	__ASSERT_DEBUG(tls != NULL, Panic(EGlxPanicLogicError));
	__ASSERT_DEBUG(tls->iReferenceCount > 0, Panic(EGlxPanicLogicError)); // There's nothign to close

    if ( tls ) 
    	{
		tls->iReferenceCount--;
		
		// Delete the tls pointer and list manager instance if this was the 
		// last reference
    	if (tls->iReferenceCount == 0)
	        {
	        delete tls->iListManager;
    	    delete tls;
        	Dll::SetTls(NULL);
	        }
    	}	
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxVisualListManager::CGlxVisualListManager()
	{
	// No implementation
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxVisualListManager::~CGlxVisualListManager()
	{
	iVisualLists.Close();
	}

// -----------------------------------------------------------------------------
// AllocListL
// -----------------------------------------------------------------------------
//
EXPORT_C MGlxVisualList* CGlxVisualListManager::AllocListL(
        MGlxMediaList& aMediaList, CAlfEnv& aEnv, CAlfDisplay& aDisplay,
        CAlfImageVisual::TScaleMode aThumbnailScaleMode )
    {
    TRACER("CGlxVisualListManager::AllocListL");
    GLX_LOG_INFO("CGlxVisualListManager::AllocListL");
   __ASSERT_DEBUG( &aMediaList, Panic(EGlxPanicNullPointer));

    CGlxVisualListControl* lc = CGlxVisualListControl::NewLC(aMediaList, 
                                        aEnv, aDisplay, aThumbnailScaleMode);
    iVisualLists.AppendL(lc);
    lc->AddReference(); // Add first user
    CleanupStack::Pop(lc);
    return lc;
    }

// -----------------------------------------------------------------------------
// ListL
// -----------------------------------------------------------------------------
//	
EXPORT_C MGlxVisualList* CGlxVisualListManager::ListL(
        MGlxMediaList& aItemList,
        CAlfEnv& aEnv, CAlfDisplay& aDisplay,
        CAlfImageVisual::TScaleMode aThumbnailScaleMode)
    {
    TRACER("CGlxVisualListManager::ListL");
    GLX_LOG_INFO("CGlxVisualListManager::ListL");
    __ASSERT_DEBUG( &aItemList, Panic(EGlxPanicNullPointer) );

    // Try to find an existing visual list with the same media list
    MGlxVisualList* visList = NULL;

    // search in existing list
    TInt listCount = iVisualLists.Count();
    for ( TInt i = 0; i < listCount; i++ )
        {
        if ( &iVisualLists[i]->MediaList() == &aItemList
            && iVisualLists[i]->ThumbnailScaleMode() == aThumbnailScaleMode)
            {
            visList = iVisualLists[i];
            iVisualLists[i]->AddReference();
            break;
            }
        }

    // if not found allocate a new one
    if ( !visList )
        {
        visList = AllocListL(aItemList, aEnv, aDisplay, aThumbnailScaleMode);
        }

    return visList;
    }

// -----------------------------------------------------------------------------
// Removes a reference to the list, an deletes it if no more references remain
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxVisualListManager::ReleaseList(MGlxVisualList* aList)
	{
	TRACER("CGlxVisualListManager::ReleaseList");
    GLX_LOG_INFO("CGlxVisualListManager::ReleaseList");
	if ( !aList ) 
		{
		return;
		}
	
	TGlxVisualListId id = aList->Id();
	TInt count = iVisualLists.Count();
	for (TInt i = 0; i < count; i++)
		{
		CGlxVisualListControl* list = iVisualLists[i];
		if (list->Id() == id)
			{
			TInt refsLeft = list->RemoveReference();
			if (refsLeft == 0)
				{
				// No users left, delete
				delete list;
				iVisualLists.Remove(i);
				}
			break;
			}
		}
	}	



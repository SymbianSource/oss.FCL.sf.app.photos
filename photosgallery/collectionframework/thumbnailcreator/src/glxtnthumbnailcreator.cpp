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
* Description:    Classes for thumbnail-related tasks.
*
*/




/**
 * @internal reviewed 31/07/2007 by Simon Brooks
 */

#include "glxtnthumbnailcreator.h"

#include "glxpanic.h"
#include "glxtnbackgroundgenerationtask.h"
#include "glxtncleanuptask.h"
#include "glxtndeletethumbnailstask.h"
#include "glxtnfileutility.h"
#include "glxtnfilteravailabletask.h"
#include "glxtngeneratethumbnailtask.h"
#include "glxtnquickthumbnailtask.h"
#include "glxtntaskmanager.h"
#include "glxtnthumbnailrequest.h"
#include "glxtnzoomedimagetask.h"
#include "mglxtnthumbnailcreatorclient.h"

#include <glxtracer.h>
#include <glxsettingsmodel.h>

// -----------------------------------------------------------------------------
// InstanceL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxtnThumbnailCreator* CGlxtnThumbnailCreator::InstanceL()
    {
    TRACER("CGlxtnThumbnailCreator::InstanceL()");
    CGlxtnThumbnailCreator* instance
                    = reinterpret_cast<CGlxtnThumbnailCreator*>(Dll::Tls());

    // Create instance if does not exist
    if ( !instance ) 
        {
    	instance = new (ELeave) CGlxtnThumbnailCreator();
    	CleanupStack::PushL(instance);
    	instance->ConstructL();
    	CleanupStack::Pop(instance);

        Dll::SetTls(instance);
        }

    // Add user	
    instance->iReferenceCount++;

    return instance;
    }

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxtnThumbnailCreator::Close(MGlxtnThumbnailStorage* aStorage)
    {
    TRACER("CGlxtnThumbnailCreator::Close()");
    // Cancel any tasks using the client's storage
    if ( aStorage )
        {
        iTaskManager->CancelTasks(aStorage);
        }

    __ASSERT_DEBUG(iReferenceCount > 0, Panic(EGlxPanicIllegalState));
    iReferenceCount--;
    if ( iReferenceCount < 1 )
        {
        Dll::SetTls(NULL);
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxtnThumbnailCreator::CGlxtnThumbnailCreator() 
	{
    TRACER("CGlxtnThumbnailCreator::CGlxtnThumbnailCreator()");
	}

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxtnThumbnailCreator::ConstructL() 
	{
    TRACER("CGlxtnThumbnailCreator::ConstructL()");
	iFileUtility = CGlxtnFileUtility::NewL();
	iTaskManager = CGlxtnTaskManager::NewL();
	iSettingsModel = CGlxSettingsModel::InstanceL();
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxtnThumbnailCreator::~CGlxtnThumbnailCreator() 
	{
    TRACER("CGlxtnThumbnailCreator::~CGlxtnThumbnailCreator()");
	delete iTaskManager;
	delete iFileUtility;
	if (iSettingsModel)
	    {
	    iSettingsModel->Close();
	    }
	}

// -----------------------------------------------------------------------------
// FetchThumbnailL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxtnThumbnailCreator::FetchThumbnailL(
                                    const TGlxThumbnailRequest& aRequestInfo,
                                    MGlxtnThumbnailCreatorClient& aClient) 
	{
    TRACER("void CGlxtnThumbnailCreator::FetchThumbnailL()");
	// Create the task
	CGlxtnTask* task = NULL;

    if ( 0 == aRequestInfo.iBitmapHandle )
        {
        task = CGlxtnBackgroundGenerationTask::NewL(aRequestInfo.iId,
                                                *iFileUtility, aClient, iSettingsModel->SupportedOrientations() );
        }
    else if ( ( 0 == aRequestInfo.iSizeClass.iWidth) || ( 0 == aRequestInfo.iSizeClass.iHeight ) )
        {
        User::Leave(KErrArgument);
        }
	else if ( !aRequestInfo.iCroppingRect.IsEmpty() )
	    {
	    task = CGlxtnZoomedImageTask::NewL(aRequestInfo,
	                                            *iFileUtility, aClient);
        }
	else if ( TGlxThumbnailRequest::EPrioritizeQuality == aRequestInfo.iPriorityMode )
		{
		task = CGlxtnGenerateThumbnailTask::NewL(aRequestInfo,
		                                        *iFileUtility, aClient);
		}
    else
        {
        task = CGlxtnQuickThumbnailTask::NewL(aRequestInfo,
                                                *iFileUtility, aClient);
        }

    iTaskManager->AddTaskL(task);   // Takes ownership
    } 

// -----------------------------------------------------------------------------
// DeleteThumbnailsL
// Starts deleting all stored thumbnails for a given item.  This will result
// in a callback to MGlxtnThumbnailCreatorClient::ThumbnailDeletionComplete().
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxtnThumbnailCreator::DeleteThumbnailsL(
            const TGlxMediaId& aItemId, MGlxtnThumbnailCreatorClient& aClient)
    {
    TRACER("void CGlxtnThumbnailCreator::DeleteThumbnailsL()");
    iTaskManager->AddTaskL(CGlxtnDeleteThumbnailsTask::NewL(aItemId, aClient));
    }

// -----------------------------------------------------------------------------
// FilterAvailableThumbnailsL
// Starts the filtering of items for which a thumbnail is available from a
// list of item IDs.  This will result in a callback to
// MGlxtnThumbnailCreatorClient::FilterAvailableComplete().
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxtnThumbnailCreator::FilterAvailableThumbnailsL(
                    const TArray<TGlxMediaId>& aItemArray, const TSize& aSize,
                    MGlxtnThumbnailCreatorClient& aClient)
    {
    TRACER("void CGlxtnThumbnailCreator::FilterAvailableThumbnailsL()");
    iTaskManager->AddTaskL(CGlxtnFilterAvailableTask::NewL(aItemArray, aSize,
                                                            aClient));
    }

// -----------------------------------------------------------------------------
// CleanupThumbnailsL
// Starts the cleanup of obsolete thumbnails from storage.  There is no
// callback indicating completion of the request.
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxtnThumbnailCreator::CleanupThumbnailsL(
                                        MGlxtnThumbnailStorage* aStorage)
    {
    TRACER("void CGlxtnThumbnailCreator::CleanupThumbnailsL()");
    iTaskManager->AddTaskL(CGlxtnCleanupTask::NewL(aStorage));
    }

// -----------------------------------------------------------------------------
// CancelRequest
// Cancel any ongoing tasks for a given item.  This ensures the TNC is not
// locking the media file.
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxtnThumbnailCreator::CancelRequest(const TGlxMediaId& aItemId)
    {
    TRACER("void CGlxtnThumbnailCreator::CancelRequest()");
    iTaskManager->CancelTasks(aItemId);
    }

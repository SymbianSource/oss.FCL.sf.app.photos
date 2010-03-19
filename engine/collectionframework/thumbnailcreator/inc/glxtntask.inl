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
* Description:   Base classes for multi-purpose tasks.
*
*/



/**
 * @internal reviewed 30/07/2007 by Simon Brooks
 */

// -----------------------------------------------------------------------------
// SetPriority
// Gets the current task specific state
// -----------------------------------------------------------------------------
//
inline CGlxtnTask::TState CGlxtnTask::State() const
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// SetPriority
// Gets the type of task this is.
// -----------------------------------------------------------------------------
//
inline TGlxtnTaskId CGlxtnTask::Id() const
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// SetPriority
// Gets the media ID of the item to which this task relates.
// -----------------------------------------------------------------------------
//
inline TGlxMediaId CGlxtnTask::ItemId() const
    {
    return iItemId;
    }

// -----------------------------------------------------------------------------
// SetPriority
// Sets the priority of the task. By default it is set to EPriorityStandard.
// -----------------------------------------------------------------------------
//
inline void CGlxtnTask::SetPriority(TInt aPriority)
    {
    iPriority = aPriority;
    }

// -----------------------------------------------------------------------------
// Priority
// Gets the task priority.
// -----------------------------------------------------------------------------
//
inline TInt CGlxtnTask::Priority() const
    {
    return iPriority;
    }

// -----------------------------------------------------------------------------
// Storage
// Get the thumbnail storage for this task to use, if any.
// -----------------------------------------------------------------------------
//
inline MGlxtnThumbnailStorage* CGlxtnTask::Storage() const
    {
    return iStorage;
    }

// -----------------------------------------------------------------------------
// Client
// Get the client for this task.
// -----------------------------------------------------------------------------
//
inline MGlxtnThumbnailCreatorClient& CGlxtnClientTask::Client()
    {
    return iClient;
    }

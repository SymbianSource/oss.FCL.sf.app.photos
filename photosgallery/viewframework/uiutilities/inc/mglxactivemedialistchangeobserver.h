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
* Description:    Observer interface to be notified in change of media list
*
*/




#ifndef __M_GLXACTIVEMEDIALISTCHANGEOBSERVER_H__
#define __M_GLXACTIVEMEDIALISTCHANGEOBSERVER_H__

/**
 * Observer interface to observe notifications if the currently 
 * active media list changes
 */
class MGlxActiveMediaListChangeObserver 
    {
public:
    /**
     * Called to notify that the currently active media list pointer
     * has changed.
     * Use MGlxActiveMediaListResolver to get the pointer of the 
     * newly active list.
     */
    virtual void HandleActiveMediaListChanged() = 0;
    };

#endif // __M_GLXACTIVEMEDIALISTCHANGEOBSERVER_H__
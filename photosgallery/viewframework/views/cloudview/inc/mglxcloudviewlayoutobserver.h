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
* Description:    Cloud View layout change observer interface
*
*/




#ifndef M_GLXCLOUDVIEWLAYOUTOBSERVER_H
#define M_GLXCLOUDVIEWLAYOUTOBSERVER_H

#include <e32base.h>

/**
 *  MGlxCloudViewLayoutObserver
 *
 *  widget rect change notification interface
 *
 *  @lib glxcloudview.lib
 */
class MGlxCloudViewLayoutObserver
	{
public:	
	/**
	 * Called when there is layout change 
	 */
    virtual void HandleLayoutChanged() = 0;  
	
	};

#endif // M_GLXCLOUDVIEWLAYOUTOBSERVER_H




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
* Description:    Cloud View Msk observer interface
*
*/




#ifndef M_GLXCLOUDVIEWMSKOBSERVER_H
#define M_GLXCLOUDVIEWMSKOBSERVER_H

#include <e32base.h>

/**
 *  MGlxCloudViewMskObserver
 *
 *  List event notification interface
 *
 *  @lib glxcloudview.lib
 */
class MGlxCloudViewMskObserver
	{
public:	
	/**
	 * Called when item focus change has happened it the listbox
	 * @param aDirection The direction of the focus change
	 */
    virtual void HandleMskChangedL(TBool aMskEnabled) = 0;
  
	
	};

#endif // M_GLXCLOUDVIEWMSKOBSERVER_H




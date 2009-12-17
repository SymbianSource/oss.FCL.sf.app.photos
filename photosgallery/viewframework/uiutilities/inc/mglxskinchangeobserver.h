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
* Description:    Skin change observer
*
*/





#ifndef M_GLXSKINCHANGEOBSERVER_H
#define M_GLXSKINCHANGEOBSERVER_H

/**
 * Class that observes the skin change
 */
class MGlxSkinChangeObserver 
    {
public:     
    /*
     * Handle notification whenever skin changes
     * @param The change type - resource change type
     */    
    virtual void HandleSkinChanged() = 0;    
    };

#endif // M_GLXSKINCHANGEOBSERVER_H
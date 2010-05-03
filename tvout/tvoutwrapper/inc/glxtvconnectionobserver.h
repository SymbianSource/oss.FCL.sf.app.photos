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
* Description:    Header of Tv connection Observer class
*
*/

#ifndef GLXTVCONNECTIONOBSERVER_H
#define GLXTVCONNECTIONOBSERVER_H

#include <e32base.h>
#include <mglxtvobserver.h>     // for inteface MGlxTvObserver

// forward decelration
class CGlxTv;
class GlxTvOutWrapperPrivate;

class CGlxConnectionObserver : public CBase,
                public MGlxTvObserver
    {
public:
    /*
     * NewL() 
     */
    static CGlxConnectionObserver* NewL(GlxTvOutWrapperPrivate* aTvWrapperPrivate);
    
    /*
     * destructor
     */
    ~CGlxConnectionObserver();
private:
    /*
     * ConstructL
     */
    void ConstructL();
    
    /*
     * constructor 
     */
    CGlxConnectionObserver(GlxTvOutWrapperPrivate* aTvWrapperPrivate);
    
private:// From MGlxTvObserver
    void HandleTvStatusChangedL ( TTvChangeType aChangeType );

private:
    CGlxTv*                     iGlxTvOut;
    GlxTvOutWrapperPrivate*     iTvWrapperPrivate;
    };
#endif // GLXTVCONNECTIONOBSERVER_H



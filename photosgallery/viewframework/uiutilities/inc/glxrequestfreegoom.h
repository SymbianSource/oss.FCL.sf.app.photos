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
 * Description:    Request Goom to free memroy in asynch way
 *
 */

#ifndef GLXREQUESTFREEGOOM_H_
#define GLXREQUESTFREEGOOM_H_

#include <goommonitorsession.h>

//observer class to notify events
class MGoomNotifierObserver
    {
public:
    virtual void HandleGoomMemoryReleased(TInt aStatus) = 0;
    };

/**
 * class declaration
 */
class CGlxRelaseGPUMemory : public CActive
    {
public:
    /**
     * Two-phased constructor.
     */
    IMPORT_C
    static CGlxRelaseGPUMemory* NewL(MGoomNotifierObserver& aNotify);

    /**
     * Two-phased constructor.
     */
    static CGlxRelaseGPUMemory* NewLC(MGoomNotifierObserver& aNotify);
    /**
     * Destructor
     */
    ~CGlxRelaseGPUMemory();
    /*
     * Start Memory release process
     */
    IMPORT_C void RequestMemory();
private:
    /**
     * Default constructor
     */
    CGlxRelaseGPUMemory(MGoomNotifierObserver& aNotify);
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    /**
     * active object's default implelemtaions
     */
    void RunL();
    void DoCancel();

    /**
     * IssueRequest 
     */
    void IssueRequest();

private:
    /**
     * reference of observer
     */
    MGoomNotifierObserver& iNotify;
    RGOomMonitorSession iGoom;
    TBool iIsFirstRequest;
    
#ifdef _DEBUG
    TTime iStartTime;
    TTime iStopTime;
#endif

    };

#endif /* GLXREQUESTFREEGOOM_H_ */

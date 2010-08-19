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
* Description:    MMC Notifier
*
*/

#ifndef GLXMMCNOTIFIER_H_
#define GLXMMCNOTIFIER_H_

#include <f32file.h>  

//observer class to notify events
class MStorageNotifierObserver
    {
public:
    virtual void HandleMMCInsertionL() =0;
    virtual void HandleMMCRemovalL() =0;
    };
/**
 * states for MMC card
 */
enum TGlxMMCState
    {
    EMMCStateReset,
    EMMCStateInsert
    };
/**
 * class declaration
 */
class CGlxMMCNotifier : public CActive   
    {  
public: 
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CGlxMMCNotifier* NewL(MStorageNotifierObserver& aNotify);
    
    /**
     * Two-phased constructor.
     */
    static CGlxMMCNotifier* NewLC(MStorageNotifierObserver& aNotify);    
    /**
     * Destructor
     */
    ~CGlxMMCNotifier ();  
    
private:
    /**
     * Default constructor
     */
    CGlxMMCNotifier (MStorageNotifierObserver& aNotify);    
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
     * file session
     */
    RFs  iFs;
    /**
     * reference of observer
     */
    MStorageNotifierObserver& iNotify;
    /**
     * MMC state
     */
    TGlxMMCState istate;
    
    /**
     * Default Memory Card drive identifier specified by TDriveNumber 
     */
    TInt iDefaultMemoryCardDrive;
    };

#endif /* GLXMMCNOTIFIER_H_ */

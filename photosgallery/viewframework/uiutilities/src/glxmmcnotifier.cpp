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


#include <glxtracer.h>
#include <glxlog.h>

#include "glxmmcnotifier.h"

// ---------------------------------------------------------
// CGlxMMCNotifier::NewL
// ---------------------------------------------------------
//
EXPORT_C CGlxMMCNotifier* CGlxMMCNotifier::NewL(MStorageNotifierObserver& aNotify)    
    { 
    TRACER("CGlxMMCNotifier::NewL()");
    CGlxMMCNotifier* self = CGlxMMCNotifier::NewLC(aNotify);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CGlxMMCNotifier::NewLC
// ---------------------------------------------------------
//
CGlxMMCNotifier* CGlxMMCNotifier::NewLC(MStorageNotifierObserver& aNotify)    
    {    
    TRACER("CGlxMMCNotifier::NewLC()");
    CGlxMMCNotifier* self = new (ELeave) CGlxMMCNotifier(aNotify);    
    CleanupStack::PushL(self);    
    self->ConstructL();    
    return self;
    }

// ---------------------------------------------------------
// CGlxMMCNotifier::CGlxMMCNotifier
// ---------------------------------------------------------
//
CGlxMMCNotifier::CGlxMMCNotifier(MStorageNotifierObserver& aNotify)
        : CActive( CActive::EPriorityStandard ),iNotify(aNotify)    
    {
    TRACER("CGlxMMCNotifier::CGlxMMCNotifier()");
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CGlxMMCNotifier::~CGlxMMCNotifier()
// ---------------------------------------------------------
//
CGlxMMCNotifier::~CGlxMMCNotifier()
    {
    TRACER("CGlxMMCNotifier::~CGlxMMCNotifier()");
    Cancel();
    iFs.Close();
    }

// ---------------------------------------------------------
// CGlxMMCNotifier::IssueRequest()
// ---------------------------------------------------------
//
void CGlxMMCNotifier::IssueRequest()    
    {
    TRACER("CGlxMMCNotifier::IssueRequest()");
    if ( !IsActive() )       
        {      
        // Request to get notified of MMC insertion/removal events      
        iFs.NotifyChange( ENotifyDisk, iStatus );      
        SetActive();      
        }    
    }

// ---------------------------------------------------------
// CGlxMMCNotifier::ConstructL()
// ---------------------------------------------------------
//
void CGlxMMCNotifier::ConstructL()    
    {    
    TRACER("CGlxMMCNotifier::ConstructL()");
    TInt err = iFs.Connect();
    GLX_LOG_INFO1("CGlxMMCNotifier::ConstructL iFs.Connect err %d",err );
    IssueRequest();
    }

// ---------------------------------------------------------
// CGlxMMCNotifier::DoCancel()
// ---------------------------------------------------------
//
void CGlxMMCNotifier::DoCancel()
    {
    TRACER("CGlxMMCNotifier::DoCancel()");
    iFs.NotifyChangeCancel();   
    }

// ---------------------------------------------------------
// CGlxMMCNotifier::RunL
// ---------------------------------------------------------
//
void CGlxMMCNotifier::RunL()    
    {    
    TRACER("CGlxMMCNotifier::RunL()");
    TDriveInfo driveInfo;
    // Get the drive info for memory card     
    TInt err = iFs.Drive( driveInfo, EDriveF );
    GLX_LOG_INFO1("CGlxMMCNotifier::RunL err %d",err );
    if( err == KErrNone )      
        {      
        switch( driveInfo.iType )        
            {        
            case EMediaNotPresent:          
                {          
                //MMC removed  don't do anything   
                iNotify.HandleMMCRemovalL();
                break;          
                }        
            default:          
                { 
                iNotify.HandleMMCInsertionL();
                //MMC inserted                    
                break;          
                }        
            }      
        }    
    // Issue request for next event notification    
    IssueRequest();    
    }


